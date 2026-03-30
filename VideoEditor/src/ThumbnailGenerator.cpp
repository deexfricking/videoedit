#include "ThumbnailGenerator.h"
#include <QtConcurrent/QtConcurrent>
#include <QMutexLocker>
#include <QImageReader>
#include <QPainter>
#include <QDebug>

#ifdef USE_FFMPEG
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}
#endif

ThumbnailGenerator::ThumbnailGenerator(QObject* parent)
    : QObject(parent)
{}

void ThumbnailGenerator::requestThumbnail(const QString& filePath,
                                           qint64 offsetMs,
                                           QSize  size)
{
    // Check cache first
    {
        QMutexLocker lock(&m_mutex);
        QString key = filePath + QString("@%1").arg(offsetMs);
        if (m_cache.contains(key)) {
            emit thumbnailReady(filePath, offsetMs, m_cache[key]);
            return;
        }
    }

    // Run asynchronously
    QtConcurrent::run([=]() {
        QPixmap pix = generateSync(filePath, offsetMs, size);
        {
            QMutexLocker lock(&m_mutex);
            m_cache[filePath + QString("@%1").arg(offsetMs)] = pix;
        }
        emit thumbnailReady(filePath, offsetMs, pix);
    });
}

QPixmap ThumbnailGenerator::generateSync(const QString& filePath,
                                          qint64 offsetMs,
                                          QSize  size)
{
    // Image files — use Qt directly
    QImageReader reader(filePath);
    if (reader.canRead()) {
        return generateImage(filePath, size);
    }

#ifdef USE_FFMPEG
    return generateVideoFrame(filePath, offsetMs, size);
#else
    // Fallback: grey placeholder with filename
    QPixmap pix(size);
    pix.fill(QColor(40, 40, 45));
    QPainter p(&pix);
    p.setPen(QColor(180, 180, 180));
    p.setFont(QFont("Arial", 8));
    p.drawText(pix.rect(), Qt::AlignCenter | Qt::TextWordWrap,
               QFileInfo(filePath).fileName());
    return pix;
#endif
}

QPixmap ThumbnailGenerator::generateImage(const QString& filePath, QSize size)
{
    QImageReader reader(filePath);
    reader.setScaledSize(size);
    QImage img = reader.read();
    if (img.isNull()) {
        QPixmap pix(size);
        pix.fill(Qt::darkGray);
        return pix;
    }
    return QPixmap::fromImage(img).scaled(size, Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation);
}

#ifdef USE_FFMPEG
QPixmap ThumbnailGenerator::generateVideoFrame(const QString& filePath,
                                                qint64 offsetMs,
                                                QSize  size)
{
    AVFormatContext* fmtCtx = nullptr;
    QByteArray path = filePath.toUtf8();

    if (avformat_open_input(&fmtCtx, path.constData(), nullptr, nullptr) != 0) {
        qWarning() << "ThumbnailGenerator: cannot open" << filePath;
        QPixmap pix(size); pix.fill(Qt::darkGray); return pix;
    }

    avformat_find_stream_info(fmtCtx, nullptr);

    // Find first video stream
    int videoStream = -1;
    const AVCodec* codec = nullptr;
    for (unsigned i = 0; i < fmtCtx->nb_streams; ++i) {
        if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = (int)i;
            codec = avcodec_find_decoder(fmtCtx->streams[i]->codecpar->codec_id);
            break;
        }
    }

    if (videoStream < 0 || !codec) {
        avformat_close_input(&fmtCtx);
        QPixmap pix(size); pix.fill(Qt::darkGray); return pix;
    }

    AVCodecContext* codecCtx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecCtx,
                                   fmtCtx->streams[videoStream]->codecpar);
    avcodec_open2(codecCtx, codec, nullptr);

    // Seek to offset
    if (offsetMs > 0) {
        int64_t seekTarget = offsetMs * AV_TIME_BASE / 1000;
        av_seek_frame(fmtCtx, -1, seekTarget, AVSEEK_FLAG_BACKWARD);
        avcodec_flush_buffers(codecCtx);
    }

    AVFrame* frame    = av_frame_alloc();
    AVPacket* packet  = av_packet_alloc();
    QPixmap result(size);
    result.fill(Qt::darkGray);

    bool gotFrame = false;
    while (!gotFrame && av_read_frame(fmtCtx, packet) >= 0) {
        if (packet->stream_index == videoStream) {
            if (avcodec_send_packet(codecCtx, packet) == 0) {
                if (avcodec_receive_frame(codecCtx, frame) == 0) {
                    // Convert to RGB
                    SwsContext* swsCtx = sws_getContext(
                        frame->width, frame->height,
                        (AVPixelFormat)frame->format,
                        size.width(), size.height(),
                        AV_PIX_FMT_RGB24,
                        SWS_BILINEAR, nullptr, nullptr, nullptr);

                    if (swsCtx) {
                        QImage img(size.width(), size.height(),
                                   QImage::Format_RGB888);
                        uint8_t* dstData[1]  = { img.bits() };
                        int      dstLinesize[1] = { (int)img.bytesPerLine() };
                        sws_scale(swsCtx,
                                  frame->data, frame->linesize,
                                  0, frame->height,
                                  dstData, dstLinesize);
                        sws_freeContext(swsCtx);
                        result = QPixmap::fromImage(img);
                        gotFrame = true;
                    }
                }
            }
        }
        av_packet_unref(packet);
    }

    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codecCtx);
    avformat_close_input(&fmtCtx);

    return result;
}
#endif

void ThumbnailGenerator::clearCache()
{
    QMutexLocker lock(&m_mutex);
    m_cache.clear();
}
