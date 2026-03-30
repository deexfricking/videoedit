#pragma once
#include <QObject>
#include <QString>
#include <QPixmap>
#include <QMap>
#include <QMutex>

// ─── ThumbnailGenerator ───────────────────────────────────────────────────────
// Generates video frame thumbnails asynchronously using Qt Concurrent + FFmpeg
// (or Qt Multimedia fallback if FFmpeg is unavailable).

class ThumbnailGenerator : public QObject
{
    Q_OBJECT

public:
    explicit ThumbnailGenerator(QObject* parent = nullptr);

    // Request a thumbnail for filePath at time offsetMs (milliseconds).
    // Emits thumbnailReady() when done.
    void requestThumbnail(const QString& filePath,
                          qint64 offsetMs = 0,
                          QSize  size     = QSize(160, 90));

    // Synchronous version — blocks caller thread. Suitable for small files / images.
    QPixmap generateSync(const QString& filePath,
                         qint64 offsetMs = 0,
                         QSize  size     = QSize(160, 90));

    // Clear cached thumbnails
    void clearCache();

signals:
    void thumbnailReady(const QString& filePath, qint64 offsetMs, QPixmap pixmap);

private:
    QPixmap generateImage(const QString& filePath, QSize size);

#ifdef USE_FFMPEG
    QPixmap generateVideoFrame(const QString& filePath, qint64 offsetMs, QSize size);
#endif

    QMap<QString, QPixmap> m_cache;
    QMutex                 m_mutex;
};
