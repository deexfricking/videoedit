#pragma once
#include <QMainWindow>
#include <QDockWidget>
#include <QSplitter>
#include <QLabel>
#include <QUndoStack>
#include <QUuid>
#include "TimelineClip.h"
#include "CanvasSettingsDialog.h"
#include "MediaBin.h"
#include "EffectsPanel.h"

class VideoPlayer;
class Timeline;
class ToolBar;
class ProjectManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* e) override;
    void dragEnterEvent(QDragEnterEvent* e) override;
    void dropEvent(QDropEvent* e) override;

private slots:
    void onNewProject();
    void onOpenProject();
    void onSaveProject();
    void onSaveProjectAs();
    void onImportMedia();
    void onExport();

    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onDelete();
    void onSelectAll();

    void onCanvasSettings();
    void onCropSelected();
    void onToggleMediaBin(bool visible);

    void onPlay();
    void onPause();
    void onStop();
    void onTogglePlayPause();
    void onSkipForward();
    void onSkipBackward();
    void onStepForward();
    void onStepBackward();
    void onPlayheadMoved(qint64 ms);
    void updatePlaybackClip();  // Helper to sync player with timeline
    void onMediaItemDoubleClicked(const MediaItem& item);
    void onClipAddedToTimeline(const MediaItem& item);
    void onClipSelected(const TimelineClip& clip);
    void onClipDoubleClicked(const TimelineClip& clip);
    void onKeyframeAdded(qint64 posMs);

    void onOpenRecentFile();
    void updateStatusBar();

private:
    // Playback tracking
    QUuid m_currentPlayingClipId;
    void buildMenuBar();
    void buildStatusBar();
    void buildCentralArea();
    void buildDocks();
    void buildCommandPanel();
    void buildKeyboardShortcuts();
    void connectSignals();
    void applyDarkTheme();

    void setProjectPath(const QString& path);
    void setModified(bool m);
    bool confirmDiscardChanges();
    void updateRecentFilesMenu();
    QString exportConcatFile() const;

    // Panels
    MediaBin*       m_mediaBin     = nullptr;
    EffectsPanel*   m_effectsPanel = nullptr;
    VideoPlayer*    m_player       = nullptr;
    Timeline*       m_timeline     = nullptr;
    ToolBar*        m_toolBar      = nullptr;

    // Docks
    QDockWidget*  m_mediaDock    = nullptr;
    QDockWidget*  m_effectsDock   = nullptr;
    QDockWidget*  m_commandDock   = nullptr;

    // State
    ProjectManager* m_projectMgr = nullptr;
    CanvasSettings  m_canvas;
    QString         m_projectPath;
    bool            m_modified = false;

    QMenu*  m_recentMenu = nullptr;

    // Status bar
    QLabel* m_lblTimecode   = nullptr;
    QLabel* m_lblResolution = nullptr;
    QLabel* m_lblFps        = nullptr;
    QLabel* m_lblTool       = nullptr;
};
