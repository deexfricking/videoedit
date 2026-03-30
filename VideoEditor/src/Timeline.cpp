#include "Timeline.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include <QMenu>
#include <QAction>
#include <QUndoCommand>
#include <QUndoStack>
#include <QFileInfo>
#include <QDebug>
#include <algorithm>
#include <cmath>
#include <QProcess>

double getVideoDuration(const QString& filePath)
{
    QProcess process;

    QString program = "C:/ffmpeg-8.1-full_build/bin/ffprobe.exe"; // safer than relying on PATH
    QStringList args;

    args << "-v" << "error"
         << "-show_entries" << "format=duration"
         << "-of" << "default=noprint_wrappers=1:nokey=1"
         << filePath;

    process.start(program, args);
    process.waitForFinished();

    QString output = process.readAllStandardOutput().trimmed();

    bool ok;
    double duration = output.toDouble(&ok);

    if (!ok) return 10.0; // fallback

    return duration;
}
// ═══════════════════════════════════════════════════════════════════════════════
//  Undo Commands
// ═══════════════════════════════════════════════════════════════════════════════

class MoveClipCmd : public QUndoCommand {
public:
    MoveClipCmd(QVector<Track>* t, int ti, int ci, qint64 oldS, qint64 newS)
        : QUndoCommand("Move Clip"), m_t(t), m_ti(ti), m_ci(ci), m_old(oldS), m_new(newS) {}
    void redo() override { (*m_t)[m_ti].clips[m_ci].timelineStart = m_new; }
    void undo() override { (*m_t)[m_ti].clips[m_ci].timelineStart = m_old; }
private:
    QVector<Track>* m_t; int m_ti, m_ci; qint64 m_old, m_new;
};

class AddClipCmd : public QUndoCommand {
public:
    AddClipCmd(QVector<Track>* t, int ti, TimelineClip c)
        : QUndoCommand("Add Clip"), m_t(t), m_ti(ti), m_clip(c) {}
    void redo() override { (*m_t)[m_ti].clips.append(m_clip); }
    void undo() override {
        auto& clips = (*m_t)[m_ti].clips;
        for (int i = clips.size()-1; i >= 0; --i)
            if (clips[i].id == m_clip.id) { clips.removeAt(i); break; }
    }
private:
    QVector<Track>* m_t; int m_ti; TimelineClip m_clip;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  TimelineView
// ═══════════════════════════════════════════════════════════════════════════════

TimelineView::TimelineView(QWidget* parent) : QWidget(parent)
{
    m_undoStack = new QUndoStack(this);
    m_thumbGen  = new ThumbnailGenerator(this);
    connect(m_thumbGen, &ThumbnailGenerator::thumbnailReady,
            [this](const QString&, qint64, QPixmap){ update(); });

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAcceptDrops(true);   // ← accept drops from MediaBin
    setMinimumHeight(kRulerHeight + 4 * kTrackHeight + 20);
}

void TimelineView::setTracks(QVector<Track>* t) { m_tracks = t; update(); }

void TimelineView::setPlayheadMs(qint64 ms) { m_playheadMs = ms; update(); }

void TimelineView::setPxPerSec(double v)
{
    m_pxPerSec = qBound(4.0, v, 2000.0);
    updateGeometry(); update();
}

// ─── Geometry ─────────────────────────────────────────────────────────────────

double TimelineView::msToX(qint64 ms) const
{ return kTrackHeaderWidth + ms / 1000.0 * m_pxPerSec; }

qint64 TimelineView::xToMs(double x) const
{ return qMax(qint64(0), qint64((x - kTrackHeaderWidth) / m_pxPerSec * 1000.0)); }

int TimelineView::trackTop(int i) const
{
    int y = kRulerHeight;
    if (!m_tracks) return y;
    for (int t = 0; t < i && t < m_tracks->size(); ++t)
        y += trackHeight(t);
    return y;
}

int TimelineView::trackHeight(int i) const
{
    if (!m_tracks || i >= m_tracks->size()) return kTrackHeight;
    return ((*m_tracks)[i].type == TrackType::Audio) ? kAudioTrackHeight : kTrackHeight;
}

QRect TimelineView::clipRect(const TimelineClip& c) const
{
    int x = int(msToX(c.timelineStart));
    int w = qMax(4, int(c.duration() / 1000.0 * m_pxPerSec));
    int y = trackTop(c.trackIndex) + 2;
    int h = trackHeight(c.trackIndex) - 4;
    return QRect(x, y, w, h);
}

qint64 TimelineView::totalDurationMs() const
{
    qint64 mx = 10000;
    if (!m_tracks) return mx;
    for (auto& t : *m_tracks)
        for (auto& c : t.clips)
            mx = qMax(mx, c.timelineEnd() + 2000);
    return mx;
}

QSize TimelineView::sizeHint() const
{
    int h = kRulerHeight;
    if (m_tracks) for (int i = 0; i < m_tracks->size(); ++i) h += trackHeight(i);
    h += 40;
    int w = kTrackHeaderWidth + int(totalDurationMs() / 1000.0 * m_pxPerSec) + 300;
    return QSize(w, h);
}

// ─── Paint ────────────────────────────────────────────────────────────────────

void TimelineView::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);
    p.fillRect(rect(), QColor(18, 18, 22));
    if (!m_tracks) return;
    drawTracks(p);
    drawRuler(p);
    drawTrackHeaders(p);
    if (m_showDropIndicator) drawDropIndicator(p);
    drawPlayhead(p);
}

void TimelineView::drawRuler(QPainter& p)
{
    QRect rr(kTrackHeaderWidth, 0, width()-kTrackHeaderWidth, kRulerHeight);
    p.fillRect(rr, QColor(30,30,36));
    p.fillRect(QRect(0,0,kTrackHeaderWidth,kRulerHeight), QColor(24,24,28));

    double secsVis = (width()-kTrackHeaderWidth) / m_pxPerSec;
    double iv = 1.0;
    if (secsVis > 300) iv = 60;
    else if (secsVis > 120) iv = 30;
    else if (secsVis > 60)  iv = 10;
    else if (secsVis > 20)  iv = 5;
    else if (secsVis > 10)  iv = 2;
    double sub = iv / 5.0;

    p.setFont(QFont("monospace", 9));
    for (double sec = 0; sec <= totalDurationMs()/1000.0 + iv; sec += sub) {
        int x = int(msToX(qint64(sec*1000)));
        if (x < kTrackHeaderWidth || x > width()) continue;
        bool major = std::fmod(sec, iv) < 1e-6 || std::fmod(sec, iv) > iv-1e-6;
        if (major) {
            p.setPen(QColor(160,160,170));
            p.drawLine(x, kRulerHeight-12, x, kRulerHeight);
            int si = int(sec);
            int h2 = si/3600, m2 = (si%3600)/60, s2 = si%60;
            QString lbl = h2>0
                ? QString("%1:%2:%3").arg(h2).arg(m2,2,10,QChar('0')).arg(s2,2,10,QChar('0'))
                : QString("%1:%2").arg(m2,2,10,QChar('0')).arg(s2,2,10,QChar('0'));
            p.drawText(x+3, kRulerHeight-14, lbl);
        } else {
            p.setPen(QColor(60,60,72));
            p.drawLine(x, kRulerHeight-5, x, kRulerHeight);
        }
    }
    p.setPen(QColor(50,50,60));
    p.drawLine(0, kRulerHeight-1, width(), kRulerHeight-1);
}

void TimelineView::drawTrackHeaders(QPainter& p)
{
    if (!m_tracks) return;
    for (int i = 0; i < m_tracks->size(); ++i) {
        const Track& t = (*m_tracks)[i];
        int y = trackTop(i), h = trackHeight(i);
        bool audio = (t.type == TrackType::Audio);
        p.fillRect(QRect(0, y, kTrackHeaderWidth, h),
                   audio ? QColor(28,32,40) : QColor(30,34,44));
        p.setPen(QColor(50,55,70));
        p.drawLine(0, y+h-1, kTrackHeaderWidth, y+h-1);
        p.drawLine(kTrackHeaderWidth-1, y, kTrackHeaderWidth-1, y+h);

        p.setPen(t.muted ? QColor(100,100,100) : QColor(220,220,225));
        p.setFont(QFont("Arial", 10, QFont::Medium));
        p.drawText(QRect(8, y, kTrackHeaderWidth-66, h), Qt::AlignVCenter, t.name);

        // Mute / Solo buttons
        int bw=24, bh=16, bx=kTrackHeaderWidth-bw*2-8, by=y+(h-bh)/2;
        QRect mR(bx, by, bw, bh), sR(bx+bw+2, by, bw, bh);
        p.fillRect(mR, t.muted ? QColor(200,60,60) : QColor(55,60,75));
        p.setPen(Qt::white); p.setFont(QFont("Arial",7));
        p.drawText(mR, Qt::AlignCenter, "M");
        p.fillRect(sR, t.solo ? QColor(220,180,30) : QColor(55,60,75));
        p.drawText(sR, Qt::AlignCenter, "S");
    }
}

void TimelineView::drawTracks(QPainter& p)
{
    if (!m_tracks) return;

    for (int i = 0; i < m_tracks->size(); ++i) {
        const Track& t = (*m_tracks)[i];
        int y = trackTop(i), h = trackHeight(i);
        bool audio = (t.type == TrackType::Audio);
        QRect cr(kTrackHeaderWidth, y, width()-kTrackHeaderWidth, h);
        p.fillRect(cr, audio ? QColor(20,24,32) : QColor(22,26,35));

        // Grid
        p.setPen(QColor(32,36,46));
        double secsVis = (width()-kTrackHeaderWidth)/m_pxPerSec;
        double iv = secsVis > 60 ? 5.0 : secsVis > 20 ? 2.0 : 1.0;
        for (double s = 0; s <= totalDurationMs()/1000.0; s += iv) {
            int x = int(msToX(qint64(s*1000)));
            if (x > kTrackHeaderWidth && x < width()) p.drawLine(x, y, x, y+h);
        }

        p.setPen(QColor(40,44,55));
        p.drawLine(kTrackHeaderWidth, y+h-1, width(), y+h-1);

        for (auto& clip : t.clips) {
            QRect r = clipRect(clip);
            if (r.right() < kTrackHeaderWidth || r.left() > width()) continue;
            drawClip(p, clip, r);
        }
    }
}

void TimelineView::drawClip(QPainter& p, const TimelineClip& clip, const QRect& r)
{
    bool audio = (clip.type == ClipType::Audio);

    // Body
    QColor body = clip.selected ? clip.tintColor.lighter(145) : clip.tintColor.darker(115);
    p.fillRect(r, body);

    // Header strip with gradient
    QRect hdr(r.x(), r.y(), r.width(), 20);
    QLinearGradient grad(hdr.topLeft(), hdr.bottomLeft());
    grad.setColorAt(0, clip.tintColor);
    grad.setColorAt(1, clip.tintColor.darker(120));
    p.fillRect(hdr, grad);

    // Thumbnail or waveform or pattern
    if (!audio && !clip.thumbnail.isNull()) {
        QRect tr(r.x()+1, r.y()+21, r.width()-2, r.height()-22);
        p.drawPixmap(tr, clip.thumbnail.scaled(tr.size(),
            Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    } else if (!audio) {
        // Show a film-strip pattern as placeholder (looks more like video)
        QRect tr(r.x()+1, r.y()+21, r.width()-2, r.height()-22);
        p.fillRect(tr, QColor(15, 15, 20));
        p.setPen(QPen(clip.tintColor, 1));
        int stripWidth = tr.height() / 3;
        for (int x = tr.x(); x < tr.right(); x += stripWidth + 2) {
            p.drawRect(QRect(x, tr.y(), stripWidth, tr.height()));
        }
    } else if (audio) {
        p.setPen(QPen(clip.tintColor.lighter(180), 1));
        int mid = r.y() + r.height()/2;
        for (int x = r.x()+2; x < r.right()-2; x += 3) {
            int hh = QRandomGenerator::global()->bounded(2, qMax(3, r.height()/2-2));
            p.drawLine(x, mid-hh, x, mid+hh);
        }
    }

    // Clip name
    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 9, QFont::Bold));
    p.drawText(hdr.adjusted(4,0,-4,0), Qt::AlignVCenter|Qt::AlignLeft, clip.name);

    // Duration text at bottom right with proper ms conversion
    qint64 durationMs = clip.duration();
    qint64 durationSec = durationMs / 1000;
    qint64 durationMin = durationSec / 60;
    qint64 displaySec = durationSec % 60;
    QString durStr = (durationMin > 0) ? QString("%1:%2").arg(durationMin).arg(displaySec, 2, 10, QChar('0'))
                                       : QString("%1s").arg(durationSec);
    p.setFont(QFont("Courier", 7));
    p.setPen(QColor(255, 255, 200));
    p.drawText(r.adjusted(2, -12, -2, -1), Qt::AlignBottom | Qt::AlignRight, durStr);

    // Keyframes — small orange diamonds on the header strip
    p.setBrush(QColor(255, 180, 0));
    p.setPen(Qt::NoPen);
    for (qint64 kf : clip.keyframes) {
        int kx = int(msToX(clip.timelineStart + kf));
        if (kx >= r.x() && kx <= r.right()) {
            QPolygon diamond;
            diamond << QPoint(kx, r.y()+2)
                    << QPoint(kx+4, r.y()+10)
                    << QPoint(kx, r.y()+18)
                    << QPoint(kx-4, r.y()+10);
            p.drawPolygon(diamond);
        }
    }

    // Selection border
    if (clip.selected) {
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(Qt::white, 2));
        p.drawRect(r.adjusted(1,1,-1,-1));
    }

    // Trim handles
    p.fillRect(QRect(r.x(), r.y(), 4, r.height()), QColor(255,255,255,60));
    p.fillRect(QRect(r.right()-4, r.y(), 4, r.height()), QColor(255,255,255,60));
}

void TimelineView::drawPlayhead(QPainter& p)
{
    int x = int(msToX(m_playheadMs));
    if (x < kTrackHeaderWidth || x > width()) return;
    p.setPen(QPen(QColor(255,60,60), 2));
    p.drawLine(x, 0, x, height());
    QPolygon tri;
    tri << QPoint(x-7,0) << QPoint(x+7,0) << QPoint(x,14);
    p.setBrush(QColor(255,60,60));
    p.setPen(Qt::NoPen);
    p.drawPolygon(tri);
}

void TimelineView::drawDropIndicator(QPainter& p)
{
    if (m_dropTrackIndex < 0) return;
    int x = int(msToX(m_dropMs));
    int y = trackTop(m_dropTrackIndex);
    int h = trackHeight(m_dropTrackIndex);
    // Glowing vertical line
    p.setPen(QPen(QColor(100,200,255,200), 3));
    p.drawLine(x, y, x, y+h);
    // Small label
    p.setPen(QColor(100,200,255));
    p.setFont(QFont("Arial",8));
    p.drawText(x+4, y+14, "Drop here");
}

// ─── Hit test ─────────────────────────────────────────────────────────────────

TimelineView::HitResult TimelineView::hitTest(const QPoint& pos) const
{
    HitResult hr;
    if (!m_tracks) return hr;
    if (pos.x() < kTrackHeaderWidth) { hr.onHeader = true; return hr; }
    for (int ti = 0; ti < m_tracks->size(); ++ti) {
        int ty = trackTop(ti), th = trackHeight(ti);
        if (pos.y() < ty || pos.y() >= ty+th) continue;
        hr.trackIndex = ti;
        const auto& clips = (*m_tracks)[ti].clips;
        for (int ci = 0; ci < clips.size(); ++ci) {
            QRect r = clipRect(clips[ci]);
            if (!r.contains(pos)) continue;
            hr.clipIndex   = ci;
            hr.onLeftEdge  = (pos.x()-r.left()  < 8);
            hr.onRightEdge = (r.right()-pos.x()  < 8);
            return hr;
        }
        return hr;
    }
    return hr;
}

// ─── Mouse ────────────────────────────────────────────────────────────────────

void TimelineView::mousePressEvent(QMouseEvent* e)
{
    if (!m_tracks) return;
    setFocus();
    m_lastMousePos = e->pos();

    if (e->pos().y() < kRulerHeight) {
        m_dragPlayhead = true;
        qint64 ms = xToMs(e->pos().x());
        setPlayheadMs(ms);
        emit playheadMoved(ms);
        emit requestScrub(ms);
        return;
    }

    HitResult hr = hitTest(e->pos());

    if (m_currentTool == EditTool::Razor && !hr.onHeader) {
        razorAtPoint(e->pos()); return;
    }

    if (hr.clipIndex >= 0 && hr.trackIndex >= 0) {
        TimelineClip& clip = (*m_tracks)[hr.trackIndex].clips[hr.clipIndex];
        if (!(e->modifiers() & Qt::ShiftModifier)) deselectAll();
        clip.selected = true;
        emit clipSelected(clip);
        if (m_currentTool == EditTool::Select || m_currentTool == EditTool::Trim) {
            m_dragging       = true;
            m_dragTrackIndex = hr.trackIndex;
            m_dragClipIndex  = hr.clipIndex;
            m_trimLeft       = hr.onLeftEdge;
            m_trimRight      = hr.onRightEdge;
            m_dragOffsetMs   = xToMs(e->pos().x()) - clip.timelineStart;
            m_originalStart  = clip.timelineStart;
        }
    } else {
        deselectAll();
    }
    update();
}

void TimelineView::mouseMoveEvent(QMouseEvent* e)
{
    if (!m_tracks) return;
    if (m_dragPlayhead) {
        qint64 ms = xToMs(e->pos().x());
        setPlayheadMs(ms);
        emit playheadMoved(ms);
        emit requestScrub(ms);
        update(); return;
    }
    if (m_dragging && m_dragClipIndex >= 0 && m_dragTrackIndex >= 0) {
        TimelineClip& clip = (*m_tracks)[m_dragTrackIndex].clips[m_dragClipIndex];
        qint64 mMs = xToMs(e->pos().x());
        if (m_trimLeft) {
            qint64 ni = qBound(qint64(0), mMs, clip.timelineStart+clip.duration()-33);
            qint64 d  = ni - clip.timelineStart;
            clip.timelineStart += d; clip.inPoint += d;
        } else if (m_trimRight) {
            qint64 ne  = qMax(mMs, clip.timelineStart+33);
            clip.outPoint = clip.inPoint + (ne - clip.timelineStart);
        } else {
            clip.timelineStart = snapMs(qMax(qint64(0), mMs - m_dragOffsetMs));
        }
        update(); updateGeometry(); return;
    }
    // Cursor hints
    HitResult hr = hitTest(e->pos());
    if      (hr.onLeftEdge || hr.onRightEdge) setCursor(Qt::SizeHorCursor);
    else if (hr.clipIndex >= 0)
        setCursor(m_currentTool == EditTool::Razor ? Qt::CrossCursor : Qt::SizeAllCursor);
    else    setCursor(Qt::ArrowCursor);
}

void TimelineView::mouseReleaseEvent(QMouseEvent*)
{
    if (m_dragging && m_dragClipIndex >= 0 && m_dragTrackIndex >= 0) {
        auto& clip = (*m_tracks)[m_dragTrackIndex].clips[m_dragClipIndex];
        if (clip.timelineStart != m_originalStart)
            m_undoStack->push(new MoveClipCmd(m_tracks, m_dragTrackIndex,
                m_dragClipIndex, m_originalStart, clip.timelineStart));
    }
    m_dragging = m_dragPlayhead = m_trimLeft = m_trimRight = false;
    m_dragClipIndex = -1;
    emit tracksChanged();
    update();
}

void TimelineView::mouseDoubleClickEvent(QMouseEvent* e)
{
    HitResult hr = hitTest(e->pos());
    if (hr.clipIndex >= 0 && hr.trackIndex >= 0)
        emit clipDoubleClicked((*m_tracks)[hr.trackIndex].clips[hr.clipIndex]);
}

void TimelineView::wheelEvent(QWheelEvent* e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        double f = e->angleDelta().y() > 0 ? 1.15 : 1.0/1.15;
        setPxPerSec(m_pxPerSec * f);
        e->accept();
    } else {
        QWidget::wheelEvent(e);
    }
}

void TimelineView::contextMenuEvent(QContextMenuEvent* e)
{
    HitResult hr = hitTest(e->pos());
    QMenu menu(this);
    if (hr.clipIndex >= 0) {
        menu.addAction(tr("Split at Playhead"),  this, &TimelineView::splitAtPlayhead);
        menu.addAction(tr("Razor Here"),         [this,pos=e->pos()]{ razorAtPoint(pos); });
        menu.addAction(tr("Duplicate"),          this, &TimelineView::duplicateSelected);
        menu.addAction(tr("Add Keyframe (Ctrl+K)"), this, &TimelineView::addKeyframeAtPlayhead);
        menu.addSeparator();
        menu.addAction(tr("Delete"),             this, &TimelineView::deleteSelected);
    } else {
        menu.addAction(tr("Select All"),  this, &TimelineView::selectAll);
        menu.addAction(tr("Deselect All"),this, &TimelineView::deselectAll);
    }
    menu.exec(e->globalPos());
}

void TimelineView::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Delete || e->key() == Qt::Key_Backspace) { deleteSelected(); return; }
    if (e->key() == Qt::Key_A && (e->modifiers()&Qt::ControlModifier)) { selectAll(); return; }
    if (e->key() == Qt::Key_D) { duplicateSelected(); return; }
    if (e->key() == Qt::Key_S) { splitAtPlayhead(); return; }
    if (e->key() == Qt::Key_K && (e->modifiers()&Qt::ControlModifier)) {
        addKeyframeAtPlayhead(); return;
    }
    if (e->key() == Qt::Key_K && (e->modifiers()&Qt::ShiftModifier)) {
        seekToKeyframeOfSelected(); return;
    }
    QWidget::keyPressEvent(e);
}

// ─── Drag-and-Drop from MediaBin ──────────────────────────────────────────────

void TimelineView::dragEnterEvent(QDragEnterEvent* e)
{
    if (e->mimeData()->hasFormat(kMediaMimeType) || e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void TimelineView::dragMoveEvent(QDragMoveEvent* e)
{
    if (!m_tracks) return;
    if (!e->mimeData()->hasFormat(kMediaMimeType) && !e->mimeData()->hasUrls()) return;

    QPoint pos = e->position().toPoint();
    m_dropMs = snapMs(xToMs(pos.x()));

    // Find which track the cursor is over
    m_dropTrackIndex = -1;
    for (int i = 0; i < m_tracks->size(); ++i) {
        int ty = trackTop(i), th = trackHeight(i);
        if (pos.y() >= ty && pos.y() < ty+th) {
            m_dropTrackIndex = i; break;
        }
    }
    m_showDropIndicator = (m_dropTrackIndex >= 0);
    update();
    e->acceptProposedAction();
}

void TimelineView::dropEvent(QDropEvent* e)
{
    m_showDropIndicator = false;
    if (!m_tracks) { update(); return; }

    QString filePath;
    if (e->mimeData()->hasFormat(kMediaMimeType)) {
        filePath = QString::fromUtf8(e->mimeData()->data(kMediaMimeType));
    } else if (e->mimeData()->hasUrls()) {
        auto urls = e->mimeData()->urls();
        if (!urls.isEmpty()) filePath = urls.first().toLocalFile();
    }

    if (filePath.isEmpty() || m_dropTrackIndex < 0) { update(); return; }

    // Build a clip from the dropped file
    QFileInfo fi(filePath);
    TimelineClip clip;
    clip.filePath      = filePath;
    clip.name          = fi.fileName();
    clip.timelineStart = m_dropMs;
    clip.inPoint       = 0;

    // Guess type from extension
TimelineClip clip;
clip.filePath      = filePath;
clip.name          = fi.fileName();
clip.timelineStart = m_dropMs;
clip.inPoint       = 0;
clip.trackIndex    = m_dropTrackIndex;

// Type detection
QString ext = fi.suffix().toLower();
QStringList aExts = {"mp3","wav","aac","flac","ogg","m4a","wma","aiff", "mkv","mp4","avi","mov","mpeg","mpg","flv"};
QStringList iExts = {"jpg","jpeg","png","bmp","gif","tiff","tif","webp"};

if      (aExts.contains(ext)) clip.type = ClipType::Audio;
else if (iExts.contains(ext)) clip.type = ClipType::Image;
else                          clip.type = ClipType::Video;

// ✅ FIXED DURATION
if (clip.type == ClipType::Image)
{
    clip.outPoint = 5000;
}
else
{
    double durationSec = getVideoDuration(filePath);
    clip.outPoint = static_cast<qint64>(durationSec * 1000.0);
}

    // Pick a colour per type
    if (clip.type == ClipType::Audio)
        clip.tintColor = QColor(40, 140, 100);
    else if (clip.type == ClipType::Image)
        clip.tintColor = QColor(150, 80, 200);
    else
        clip.tintColor = QColor(60, 120, 210);

    m_undoStack->push(new AddClipCmd(m_tracks, m_dropTrackIndex, clip));

    // Request thumbnail
    m_thumbGen->requestThumbnail(filePath, 0, QSize(120, 68));

    updateGeometry();
    update();
    emit tracksChanged();
    e->acceptProposedAction();
}

// ─── Operations ───────────────────────────────────────────────────────────────

void TimelineView::razorAtPoint(const QPoint& pos)
{
    if (!m_tracks) return;
    qint64 cutMs = xToMs(pos.x());
    for (auto& track : *m_tracks) {
        for (int ci = track.clips.size()-1; ci >= 0; --ci) {
            TimelineClip& c = track.clips[ci];
            if (cutMs <= c.timelineStart || cutMs >= c.timelineEnd()) continue;
            TimelineClip left = c, right = c;
            right.id = QUuid::createUuid();
            left.outPoint  = c.inPoint + (cutMs - c.timelineStart);
            right.inPoint  = left.outPoint;
            right.timelineStart = cutMs;
            // Split keyframes
            left.keyframes.clear();
            right.keyframes.clear();
            for (qint64 kf : c.keyframes) {
                qint64 absKf = c.timelineStart + kf;
                if (absKf < cutMs) left.keyframes.append(kf);
                else right.keyframes.append(absKf - cutMs);
            }
            track.clips[ci] = left;
            track.clips.insert(ci+1, right);
            break;
        }
    }
    update(); emit tracksChanged();
}

void TimelineView::splitAtPlayhead()
{
    razorAtPoint(QPoint(int(msToX(m_playheadMs)), kRulerHeight+10));
    
    // Also split any clips that have keyframes at the playhead position
    if (!m_tracks) return;
    for (auto& track : *m_tracks) {
        for (int i = 0; i < track.clips.size(); ++i) {
            auto& clip = track.clips[i];
            qint64 offset = m_playheadMs - clip.timelineStart;
            
            // Check if this clip has a keyframe at playhead
            if (m_playheadMs > clip.timelineStart && m_playheadMs <= clip.timelineEnd() &&
                clip.keyframes.contains(offset))
            {
                // Split the clip at the keyframe
                TimelineClip newClip = clip;
                clip.outPoint = clip.inPoint + offset;
                newClip.inPoint = clip.inPoint + offset;
                newClip.timelineStart = m_playheadMs;
                
                // Distribute keyframes
                clip.keyframes.clear();
                newClip.keyframes.clear();
                for (qint64 kf : clip.keyframes) {
                    if (kf < offset) clip.keyframes.append(kf);
                    else newClip.keyframes.append(kf - offset);
                }
                
                track.clips.insert(i + 1, newClip);
                ++i;
            }
        }
    }
}

void TimelineView::duplicateSelected()
{
    if (!m_tracks) return;
    QVector<TimelineClip> toAdd; QVector<int> toTi;
    for (auto& track : *m_tracks)
        for (auto& c : track.clips) if (c.selected) {
            TimelineClip d = c;
            d.id = QUuid::createUuid();
            d.timelineStart = c.timelineEnd() + 100;
            d.selected = false;
            toAdd.append(d); toTi.append(c.trackIndex);
        }
    for (int i = 0; i < toAdd.size(); ++i)
        (*m_tracks)[toTi[i]].clips.append(toAdd[i]);
    update(); emit tracksChanged();
}

void TimelineView::addKeyframeAtPlayhead()
{
    if (!m_tracks) return;
    bool added = false;
    for (auto& track : *m_tracks)
        for (auto& clip : track.clips)
            if (clip.selected)
            {
                qint64 offset = m_playheadMs - clip.timelineStart;
                if (offset >= 0 && offset <= clip.duration() && !clip.keyframes.contains(offset)) {
                    clip.keyframes.append(offset);
                    std::sort(clip.keyframes.begin(), clip.keyframes.end());
                    added = true;
                }
            }
    if (added) { update(); emit keyframeAdded(m_playheadMs); emit tracksChanged(); }
}

void TimelineView::seekToKeyframeOfSelected()
{
    if (!m_tracks) return;
    // Find first selected clip with keyframes and move playhead to first keyframe
    for (auto& track : *m_tracks)
        for (auto& clip : track.clips)
            if (clip.selected && !clip.keyframes.empty())
            {
                qint64 keyframeMs = clip.timelineStart + clip.keyframes[0];
                m_playheadMs = keyframeMs;
                update();
                emit playheadMoved(m_playheadMs);
                return;
            }
}

void TimelineView::deleteSelected()
{
    if (!m_tracks) return;
    for (auto& track : *m_tracks)
        track.clips.erase(
            std::remove_if(track.clips.begin(), track.clips.end(),
                           [](const TimelineClip& c){ return c.selected; }),
            track.clips.end());
    update(); emit tracksChanged();
}

void TimelineView::selectAll()
{
    if (!m_tracks) return;
    for (auto& t : *m_tracks) for (auto& c : t.clips) c.selected = true;
    update();
}

void TimelineView::deselectAll()
{
    if (!m_tracks) return;
    for (auto& t : *m_tracks) for (auto& c : t.clips) c.selected = false;
    update();
}

void TimelineView::addClipToTrack(int ti, const TimelineClip& clip)
{
    if (!m_tracks || ti >= m_tracks->size()) return;
    m_undoStack->push(new AddClipCmd(m_tracks, ti, clip));
    if (!clip.filePath.isEmpty())
        m_thumbGen->requestThumbnail(clip.filePath, clip.inPoint);
    updateGeometry(); update();
}

void TimelineView::removeSelectedClips() { deleteSelected(); }

qint64 TimelineView::snapMs(qint64 ms, qint64 thresh) const
{
    if (!m_tracks) return ms;
    qint64 best = ms, bestD = thresh;
    auto trySnap = [&](qint64 cand){ qint64 d=qAbs(cand-ms); if(d<bestD){bestD=d;best=cand;} };
    trySnap(0); trySnap(m_playheadMs);
    for (auto& t : *m_tracks)
        for (auto& c : t.clips) { trySnap(c.timelineStart); trySnap(c.timelineEnd()); }
    return best;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Timeline (container)
// ═══════════════════════════════════════════════════════════════════════════════

Timeline::Timeline(QWidget* parent) : QWidget(parent)
{
    buildUi();
    initDefaultTracks();
}

void Timeline::buildUi()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    // ── Top bar ───────────────────────────────────────────────────────────────
    auto* topBar = new QHBoxLayout();
    topBar->setContentsMargins(4,4,4,4);

    auto* btnAddVid = new QPushButton(tr("+ Video Track"), this);
    connect(btnAddVid, &QPushButton::clicked, [this]{ addVideoTrack(); });
    topBar->addWidget(btnAddVid);

    auto* btnAddAud = new QPushButton(tr("+ Audio Track"), this);
    connect(btnAddAud, &QPushButton::clicked, [this]{ addAudioTrack(); });
    topBar->addWidget(btnAddAud);

    topBar->addStretch();

    topBar->addWidget(new QLabel(tr("Zoom:"), this));
    auto* btnZOut  = new QPushButton("-", this); btnZOut->setFixedWidth(28);
    auto* btnZReset= new QPushButton("1:1",this); btnZReset->setFixedWidth(36);
    auto* btnZIn   = new QPushButton("+", this); btnZIn->setFixedWidth(28);
    connect(btnZOut,   &QPushButton::clicked, this, &Timeline::onZoomOut);
    connect(btnZReset, &QPushButton::clicked, this, &Timeline::onZoomReset);
    connect(btnZIn,    &QPushButton::clicked, this, &Timeline::onZoomIn);
    topBar->addWidget(btnZOut);
    topBar->addWidget(btnZReset);
    topBar->addWidget(btnZIn);
    root->addLayout(topBar);

    auto* sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color:#333;");
    root->addWidget(sep);

    m_scroll = new QScrollArea(this);
    m_scroll->setWidgetResizable(false);
    m_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scroll->setStyleSheet("QScrollArea{border:none;}");

    m_view = new TimelineView(this);
    m_scroll->setWidget(m_view);
    root->addWidget(m_scroll);

    connect(m_view, &TimelineView::playheadMoved,   this, &Timeline::playheadMoved);
    connect(m_view, &TimelineView::clipSelected,    this, &Timeline::clipSelected);
    connect(m_view, &TimelineView::clipDoubleClicked,this,&Timeline::clipDoubleClicked);
    connect(m_view, &TimelineView::requestScrub,    this, &Timeline::requestScrub);
    connect(m_view, &TimelineView::keyframeAdded,   this, &Timeline::keyframeAdded);
    connect(m_view, &TimelineView::tracksChanged,   [this]{ updateViewSize(); emit tracksModified(); });
}

void Timeline::initDefaultTracks()
{
    addVideoTrack("Video 1");
    addVideoTrack("Video 2");
    addAudioTrack("Audio 1");
    addAudioTrack("Audio 2");
    m_view->setTracks(&m_tracks);
}

void Timeline::addVideoTrack(const QString& name)
{
    Track t;
    t.name = name.isEmpty() ? QString("Video %1").arg(m_tracks.size()+1) : name;
    t.type = TrackType::Video;
    // Insert before audio tracks
    int insertAt = 0;
    for (int i = 0; i < m_tracks.size(); ++i)
        if (m_tracks[i].type == TrackType::Video) insertAt = i+1;
    m_tracks.insert(insertAt, t);
    for (int i = 0; i < m_tracks.size(); ++i)
        for (auto& c : m_tracks[i].clips) c.trackIndex = i;
    m_view->setTracks(&m_tracks);
    updateViewSize();
}

void Timeline::addAudioTrack(const QString& name)
{
    Track t;
    t.name = name.isEmpty() ? QString("Audio %1").arg(m_tracks.size()+1) : name;
    t.type = TrackType::Audio;
    m_tracks.append(t);
    m_view->setTracks(&m_tracks);
    updateViewSize();
}

void Timeline::removeTrack(int i)
{
    if (i < 0 || i >= m_tracks.size()) return;
    m_tracks.removeAt(i);
    m_view->setTracks(&m_tracks);
    updateViewSize();
}

void Timeline::addClipToTrack(int ti, const TimelineClip& clip)
{
    m_view->addClipToTrack(ti, clip);
    updateViewSize();
}

qint64 Timeline::playheadMs() const { return m_view ? m_view->playheadMs() : 0; }

void Timeline::setPlayheadMs(qint64 ms) { if(m_view) m_view->setPlayheadMs(ms); }

void Timeline::onSplitAtPlayhead()    { if(m_view) m_view->splitAtPlayhead(); }
void Timeline::onDuplicateSelected() { if(m_view) m_view->duplicateSelected(); }
void Timeline::onAddKeyframe()        { if(m_view) m_view->addKeyframeAtPlayhead(); }
void Timeline::onZoomIn()    { if(m_view){ m_view->setPxPerSec(m_view->pxPerSec()*1.5); updateViewSize(); } }
void Timeline::onZoomOut()   { if(m_view){ m_view->setPxPerSec(m_view->pxPerSec()/1.5); updateViewSize(); } }
void Timeline::onZoomReset() { if(m_view){ m_view->setPxPerSec(kDefaultPxPerSec);      updateViewSize(); } }

void Timeline::setCurrentTool(EditTool t) { if(m_view) m_view->setCurrentTool(t); }
QUndoStack* Timeline::undoStack()         { return m_view ? m_view->undoStack() : nullptr; }

void Timeline::updateViewSize()
{
    if (m_view) m_view->resize(m_view->sizeHint());
}
