#include "MediaBin.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QDebug>
#include <QMediaPlayer>
#include <QTimer>
#include <QEventLoop>
#include <QUrl>
#include <QThread>
#include <cstdio>

// ── helpers ──────────────────────────────────────────────────────────────────
static QStringList videoExts(){ return {"mp4","mov","avi","mkv","webm","flv","wmv","m4v","ts","mts"}; }
static QStringList audioExts(){ return {"mp3","wav","aac","flac","ogg","m4a","wma","aiff"}; }
static QStringList imageExts(){ return {"jpg","jpeg","png","bmp","gif","tiff","tif","webp","svg"}; }

// MimeType used to carry a file-path from MediaBin → TimelineView
static const char* kMediaMime = "application/x-videoeditor-mediapath";

// ─── MediaListWidget ──────────────────────────────────────────────────────────
MediaListWidget::MediaListWidget(QWidget* parent) : QListWidget(parent) {}

void MediaListWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
        m_dragStartPos = e->pos();
    QListWidget::mousePressEvent(e);
}

void MediaListWidget::mouseMoveEvent(QMouseEvent* e)
{
    if (!(e->buttons() & Qt::LeftButton)) return;
    if ((e->pos() - m_dragStartPos).manhattanLength() < QApplication::startDragDistance()) return;

    QListWidgetItem* it = currentItem();
    if (!it) return;

    QString path = it->data(Qt::UserRole).toString();
    if (path.isEmpty()) return;

    auto* mime = new QMimeData;
    mime->setData(kMediaMime, path.toUtf8());
    // Also set plain URLs so external drop targets get something useful
    mime->setUrls({ QUrl::fromLocalFile(path) });

    auto* drag = new QDrag(this);
    drag->setMimeData(mime);
    if (!it->icon().isNull())
        drag->setPixmap(it->icon().pixmap(80, 45));
    drag->exec(Qt::CopyAction);
}

// ─── MediaBin ─────────────────────────────────────────────────────────────────
MediaBin::MediaBin(QWidget* parent) : QWidget(parent)
{
    m_thumbGen = new ThumbnailGenerator(this);
    connect(m_thumbGen, &ThumbnailGenerator::thumbnailReady,
            this,       &MediaBin::onThumbnailReady);
    buildUi();
}

void MediaBin::buildUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4,4,4,4);
    layout->setSpacing(4);

    auto* header = new QHBoxLayout();
    auto* lbl    = new QLabel(tr("Media Bin"), this);
    lbl->setStyleSheet("font-weight:bold;font-size:13px;");
    header->addWidget(lbl);
    header->addStretch();

    auto* btnImport = new QPushButton(tr("+ Import"), this);
    connect(btnImport, &QPushButton::clicked, this, &MediaBin::importMedia);
    header->addWidget(btnImport);

    auto* btnRemove = new QPushButton(tr("Remove"), this);
    connect(btnRemove, &QPushButton::clicked, this, &MediaBin::removeSelected);
    header->addWidget(btnRemove);
    layout->addLayout(header);

    m_list = new MediaListWidget(this);
    m_list->setViewMode(QListWidget::IconMode);
    m_list->setIconSize(QSize(120, 68));
    m_list->setResizeMode(QListWidget::Adjust);
    m_list->setDragEnabled(true);
    m_list->setDragDropMode(QAbstractItemView::DragOnly);
    m_list->setSpacing(6);
    m_list->setWordWrap(true);
    m_list->setStyleSheet(R"(
        QListWidget { background:#1e1e22; border:1px solid #333; border-radius:4px; }
        QListWidget::item { color:#ccc; padding:4px; }
        QListWidget::item:selected { background:#2a4a7a; border-radius:4px; }
    )");
    connect(m_list, &QListWidget::itemDoubleClicked, this, &MediaBin::onItemDoubleClicked);
    layout->addWidget(m_list);

    auto* hint = new QLabel(tr("← Drag clips onto the timeline tracks"), this);
    hint->setStyleSheet("color:#666; font-size:10px; padding:2px;");
    hint->setAlignment(Qt::AlignCenter);
    layout->addWidget(hint);
}

void MediaBin::importMedia()
{
    QStringList filters;
    filters << "All Media (*.mp4 *.mov *.avi *.mkv *.webm *.flv *.wmv *.m4v "
               "*.mp3 *.wav *.aac *.flac *.ogg *.m4a "
               "*.jpg *.jpeg *.png *.bmp *.gif *.tiff *.webp)"
            << "Video Files (*.mp4 *.mov *.avi *.mkv *.webm *.flv *.wmv *.m4v)"
            << "Audio Files (*.mp3 *.wav *.aac *.flac *.ogg *.m4a *.wma *.aiff)"
            << "Image Files (*.jpg *.jpeg *.png *.bmp *.gif *.tiff *.tif *.webp)"
            << "All Files (*)";

    QStringList files = QFileDialog::getOpenFileNames(
        this, tr("Import Media"), QDir::homePath(), filters.join(";;"));

    for (const QString& path : files) {
        if (m_pathToRow.contains(path)) continue;
        MediaItem item;
        item.filePath   = path;
        item.name       = QFileInfo(path).fileName();
        item.type       = detectType(path);
        item.durationMs = probeDuration(path);
        addMediaItem(item);
        m_thumbGen->requestThumbnail(path, 0, QSize(120, 68));
    }
}

void MediaBin::removeSelected()
{
    auto selected = m_list->selectedItems();
    for (auto* lwItem : selected) {
        QString path = lwItem->data(Qt::UserRole).toString();
        int row = m_list->row(lwItem);
        if (row >= 0 && row < m_items.size()) {
            m_pathToRow.remove(path);
            m_items.removeAt(row);
            m_pathToRow.clear();
            for (int i = 0; i < m_items.size(); ++i)
                m_pathToRow[m_items[i].filePath] = i;
        }
        delete m_list->takeItem(m_list->row(lwItem));
    }
}

void MediaBin::addMediaItem(const MediaItem& item)
{
    m_items.append(item);
    m_pathToRow[item.filePath] = m_items.size() - 1;

    auto* lwItem = new QListWidgetItem(m_list);
    QPixmap placeholder(120, 68);
    placeholder.fill(QColor(40, 40, 45));
    lwItem->setIcon(QIcon(placeholder));

    QString label = item.name;
    if (item.durationMs > 0) {
        qint64 secs = item.durationMs / 1000;
        label += QString("\n%1:%2").arg(secs/60,2,10,QChar('0')).arg(secs%60,2,10,QChar('0'));
    } else if (item.type == ClipType::Image) {
        label += "\n[Image]";
    } else {
        label += "\n[Video]";
    }
    lwItem->setText(label);
    lwItem->setData(Qt::UserRole, item.filePath);
    lwItem->setToolTip(item.filePath);
}

MediaItem MediaBin::itemForPath(const QString& path) const
{
    if (m_pathToRow.contains(path))
        return m_items[m_pathToRow[path]];
    // Return a minimal item built from the path
    MediaItem item;
    item.filePath = path;
    item.name     = QFileInfo(path).fileName();
    item.type     = ClipType::Video;
    return item;
}

void MediaBin::onThumbnailReady(const QString& filePath, qint64, QPixmap pix)
{
    if (!m_pathToRow.contains(filePath)) return;
    int row = m_pathToRow[filePath];
    m_items[row].thumbnail = pix;
    if (row < m_list->count())
        m_list->item(row)->setIcon(QIcon(pix));
}

void MediaBin::onItemDoubleClicked(QListWidgetItem* lwItem)
{
    QString path = lwItem->data(Qt::UserRole).toString();
    if (!m_pathToRow.contains(path)) return;
    emit mediaItemDoubleClicked(m_items[m_pathToRow[path]]);
}

ClipType MediaBin::detectType(const QString& filePath) const
{
    QString ext = QFileInfo(filePath).suffix().toLower();
    if (audioExts().contains(ext)) return ClipType::Audio;
    if (imageExts().contains(ext)) return ClipType::Image;
    return ClipType::Video;
}

qint64 MediaBin::probeDuration(const QString& filePath) const
{
    ClipType t = detectType(filePath);
    if (t == ClipType::Image) return 5000;
    
    // Use ffprobe if available (most reliable for video/audio duration)
    QString ffprobeCmd = "ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 \"%1\"";
    
    #ifdef _WIN32
        // Windows: use popen
        QString cmd = ffprobeCmd.arg(filePath);
        FILE* pipe = _popen(cmd.toLocal8Bit().constData(), "r");
        if (pipe) {
            char buffer[128] = {0};
            if (fgets(buffer, sizeof(buffer), pipe)) {
                _pclose(pipe);
                bool ok = false;
                double seconds = QString(buffer).trimmed().toDouble(&ok);
                if (ok && seconds > 0) {
                    return qint64(seconds * 1000 + 0.5);
                }
            } else {
                _pclose(pipe);
            }
        }
    #endif
    
    // Fallback: Quick QMediaPlayer check without blocking long
    QMediaPlayer player;
    player.setSource(QUrl::fromLocalFile(filePath));
    
    // Allow a tiny amount of time for duration to load
    for (int i = 0; i < 20; ++i) {
        qint64 dur = player.duration();
        if (dur > 0) return dur;
        qApp->processEvents();
        QThread::msleep(25);
    }
    
    // Final fallback
    qint64 dur = player.duration();
    return dur > 0 ? dur : 30000;  // Default to 30 seconds if detection fails
}
