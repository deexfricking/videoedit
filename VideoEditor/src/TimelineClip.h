#pragma once
#include <QString>
#include <QPixmap>
#include <QUuid>
#include <QColor>
#include <QVector>

enum class ClipType  { Video, Audio, Image };
enum class TrackType { Video, Audio };

struct TimelineClip
{
    QUuid    id            = QUuid::createUuid();
    QString  filePath;
    QString  name;
    ClipType type          = ClipType::Video;

    qint64   timelineStart  = 0;
    qint64   inPoint        = 0;
    qint64   outPoint       = 0;
    qint64   sourceDuration = 0;

    qint64 duration()    const { return outPoint - inPoint; }
    qint64 timelineEnd() const { return timelineStart + duration(); }

    int     trackIndex = 0;
    QPixmap thumbnail;
    bool    selected   = false;
    float   volume     = 1.0f;
    float   opacity    = 1.0f;
    QColor  tintColor  = QColor(80, 140, 220);

    // Keyframes: ms offset from clip's timelineStart
    QVector<qint64> keyframes;

    bool isAudioOnly() const { return type == ClipType::Audio; }
};

struct Track
{
    QUuid     id      = QUuid::createUuid();
    QString   name;
    TrackType type    = TrackType::Video;
    bool      muted   = false;
    bool      locked  = false;
    bool      solo    = false;
    bool      visible = true;
    float     volume  = 1.0f;
    QVector<TimelineClip> clips;
};
