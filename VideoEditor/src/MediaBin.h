#pragma once
#include <QWidget>
#include <QListWidget>
#include <QMap>
#include "TimelineClip.h"
#include "ThumbnailGenerator.h"

struct MediaItem {
    QString  filePath;
    QString  name;
    ClipType type;
    qint64   durationMs = 0;
    QPixmap  thumbnail;
};

// ── Custom list widget that starts a proper drag with mime data ───────────────
class MediaListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit MediaListWidget(QWidget* parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
private:
    QPoint m_dragStartPos;
};

// ─── MediaBin ─────────────────────────────────────────────────────────────────
class MediaBin : public QWidget
{
    Q_OBJECT
public:
    explicit MediaBin(QWidget* parent = nullptr);
    const QList<MediaItem>& mediaItems() const { return m_items; }
    MediaItem itemForPath(const QString& path) const;

public slots:
    void importMedia();
    void removeSelected();

signals:
    void mediaItemDoubleClicked(const MediaItem& item);

private slots:
    void onItemDoubleClicked(QListWidgetItem* item);
    void onThumbnailReady(const QString& filePath, qint64 offsetMs, QPixmap pix);

private:
    void buildUi();
    void addMediaItem(const MediaItem& item);
    ClipType detectType(const QString& filePath) const;
    qint64   probeDuration(const QString& filePath) const;

    MediaListWidget*    m_list     = nullptr;
    ThumbnailGenerator* m_thumbGen = nullptr;
    QList<MediaItem>    m_items;
    QMap<QString, int>  m_pathToRow;
};
