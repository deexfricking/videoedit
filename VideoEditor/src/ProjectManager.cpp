#include "ProjectManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QFileInfo>
#include <QDebug>

// ─── Helpers ──────────────────────────────────────────────────────────────────

static QJsonObject clipToJson(const TimelineClip& c)
{
    QJsonObject obj;
    obj["id"]            = c.id.toString();
    obj["filePath"]      = c.filePath;
    obj["name"]          = c.name;
    obj["type"]          = int(c.type);
    obj["timelineStart"] = c.timelineStart;
    obj["inPoint"]       = c.inPoint;
    obj["outPoint"]      = c.outPoint;
    obj["sourceDuration"]= c.sourceDuration;
    obj["trackIndex"]    = c.trackIndex;
    obj["volume"]        = c.volume;
    obj["opacity"]       = c.opacity;
    obj["tintR"]         = c.tintColor.red();
    obj["tintG"]         = c.tintColor.green();
    obj["tintB"]         = c.tintColor.blue();
    return obj;
}

static TimelineClip clipFromJson(const QJsonObject& obj)
{
    TimelineClip c;
    c.id             = QUuid(obj["id"].toString());
    c.filePath       = obj["filePath"].toString();
    c.name           = obj["name"].toString();
    c.type           = ClipType(obj["type"].toInt());
    c.timelineStart  = qint64(obj["timelineStart"].toDouble());
    c.inPoint        = qint64(obj["inPoint"].toDouble());
    c.outPoint       = qint64(obj["outPoint"].toDouble());
    c.sourceDuration = qint64(obj["sourceDuration"].toDouble());
    c.trackIndex     = obj["trackIndex"].toInt();
    c.volume         = float(obj["volume"].toDouble(1.0));
    c.opacity        = float(obj["opacity"].toDouble(1.0));
    c.tintColor      = QColor(obj["tintR"].toInt(80),
                               obj["tintG"].toInt(140),
                               obj["tintB"].toInt(220));
    return c;
}

static QJsonObject trackToJson(const Track& t)
{
    QJsonObject obj;
    obj["id"]     = t.id.toString();
    obj["name"]   = t.name;
    obj["type"]   = int(t.type);
    obj["muted"]  = t.muted;
    obj["locked"] = t.locked;
    obj["solo"]   = t.solo;
    obj["visible"]= t.visible;
    obj["volume"] = t.volume;

    QJsonArray clips;
    for (const auto& c : t.clips)
        clips.append(clipToJson(c));
    obj["clips"] = clips;
    return obj;
}

static Track trackFromJson(const QJsonObject& obj)
{
    Track t;
    t.id      = QUuid(obj["id"].toString());
    t.name    = obj["name"].toString();
    t.type    = TrackType(obj["type"].toInt());
    t.muted   = obj["muted"].toBool();
    t.locked  = obj["locked"].toBool();
    t.solo    = obj["solo"].toBool();
    t.visible = obj["visible"].toBool(true);
    t.volume  = float(obj["volume"].toDouble(1.0));

    for (const auto& v : obj["clips"].toArray())
        t.clips.append(clipFromJson(v.toObject()));
    return t;
}

static QJsonObject canvasToJson(const CanvasSettings& s)
{
    QJsonObject obj;
    obj["width"]       = s.width;
    obj["height"]      = s.height;
    obj["frameRate"]   = s.frameRate;
    obj["sampleRate"]  = s.sampleRate;
    obj["colorSpace"]  = s.colorSpace;
    obj["pixelFormat"] = s.pixelFormat;
    return obj;
}

static CanvasSettings canvasFromJson(const QJsonObject& obj)
{
    CanvasSettings s;
    s.width       = obj["width"].toInt(1920);
    s.height      = obj["height"].toInt(1080);
    s.frameRate   = obj["frameRate"].toDouble(30.0);
    s.sampleRate  = obj["sampleRate"].toInt(48000);
    s.colorSpace  = obj["colorSpace"].toString("Rec.709");
    s.pixelFormat = obj["pixelFormat"].toString("yuv420p");
    return s;
}

// ─── ProjectManager ───────────────────────────────────────────────────────────

ProjectManager::ProjectManager(QObject* parent)
    : QObject(parent)
{}

bool ProjectManager::saveProject(const QString& filePath,
                                  const QVector<Track>& tracks,
                                  const CanvasSettings& canvas)
{
    QJsonObject root;
    root["version"] = "1.0";
    root["canvas"]  = canvasToJson(canvas);

    QJsonArray tArr;
    for (const auto& t : tracks)
        tArr.append(trackToJson(t));
    root["tracks"] = tArr;

    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly)) {
        m_lastError = tr("Cannot write to: ") + filePath;
        return false;
    }
    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    addRecentFile(filePath);
    return true;
}

bool ProjectManager::loadProject(const QString& filePath,
                                   QVector<Track>& outTracks,
                                   CanvasSettings& outCanvas)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) {
        m_lastError = tr("Cannot open: ") + filePath;
        return false;
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &err);
    if (doc.isNull()) {
        m_lastError = tr("JSON parse error: ") + err.errorString();
        return false;
    }

    QJsonObject root = doc.object();
    outCanvas = canvasFromJson(root["canvas"].toObject());

    outTracks.clear();
    for (const auto& v : root["tracks"].toArray())
        outTracks.append(trackFromJson(v.toObject()));

    addRecentFile(filePath);
    return true;
}

CanvasSettings ProjectManager::defaultCanvas()
{
    return CanvasSettings();  // 1920×1080 @ 30fps
}

QVector<Track> ProjectManager::defaultTracks()
{
    QVector<Track> tracks;

    Track v1; v1.name = "Video 1"; v1.type = TrackType::Video; tracks.append(v1);
    Track v2; v2.name = "Video 2"; v2.type = TrackType::Video; tracks.append(v2);
    Track a1; a1.name = "Audio 1"; a1.type = TrackType::Audio; tracks.append(a1);
    Track a2; a2.name = "Audio 2"; a2.type = TrackType::Audio; tracks.append(a2);
    return tracks;
}

QStringList ProjectManager::recentFiles() const
{
    QSettings s("VideoEditor", "VideoEditor");
    return s.value("recentFiles").toStringList();
}

void ProjectManager::addRecentFile(const QString& path)
{
    QSettings s("VideoEditor", "VideoEditor");
    QStringList list = s.value("recentFiles").toStringList();
    list.removeAll(path);
    list.prepend(path);
    while (list.size() > 10) list.removeLast();
    s.setValue("recentFiles", list);
}

void ProjectManager::clearRecentFiles()
{
    QSettings s("VideoEditor", "VideoEditor");
    s.remove("recentFiles");
}
