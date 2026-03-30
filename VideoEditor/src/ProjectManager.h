#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include "TimelineClip.h"
#include "CanvasSettingsDialog.h"

// ─── ProjectManager ───────────────────────────────────────────────────────────
// Serialises/deserialises the project to a JSON file (.veproj).
// The format is intentionally simple so it can be hand-edited.

class ProjectManager : public QObject
{
    Q_OBJECT

public:
    explicit ProjectManager(QObject* parent = nullptr);

    // Returns true on success, sets m_lastError on failure.
    bool saveProject(const QString& filePath,
                     const QVector<Track>& tracks,
                     const CanvasSettings& canvas);

    bool loadProject(const QString& filePath,
                     QVector<Track>& outTracks,
                     CanvasSettings& outCanvas);

    QString lastError() const { return m_lastError; }

    // New project defaults
    static CanvasSettings defaultCanvas();
    static QVector<Track> defaultTracks();

    // Recently opened file list (persisted via QSettings)
    QStringList recentFiles() const;
    void        addRecentFile(const QString& path);
    void        clearRecentFiles();

private:
    QString m_lastError;
};
