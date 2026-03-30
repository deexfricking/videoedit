#pragma once
#include <QWidget>
#include <QScrollArea>
#include <QVector>
#include <QMenu>
#include <QUndoStack>
#include "TimelineClip.h"
#include "ToolBar.h"
#include "ThumbnailGenerator.h"

static constexpr int    kTrackHeaderWidth = 160;
static constexpr int    kTrackHeight      = 72;
static constexpr int    kRulerHeight      = 28;
static constexpr int    kAudioTrackHeight = 52;
static constexpr double kDefaultPxPerSec  = 60.0;

// mime type matching MediaBin
static const char* kMediaMimeType = "application/x-videoeditor-mediapath";

// ─── TimelineView ─────────────────────────────────────────────────────────────
class TimelineView : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineView(QWidget* parent = nullptr);

    void setTracks(QVector<Track>* tracks);

    qint64 playheadMs()  const { return m_playheadMs; }
    void   setPlayheadMs(qint64 ms);

    double pxPerSec()    const { return m_pxPerSec; }
    void   setPxPerSec(double v);

    void setCurrentTool(EditTool t) { m_currentTool = t; }

    void addClipToTrack(int trackIndex, const TimelineClip& clip);
    void removeSelectedClips();

    // Public operations (called from Timeline container / command panel)
    void splitAtPlayhead();
    void duplicateSelected();
    void addKeyframeAtPlayhead();   // Ctrl+K
    void selectAll();
    void deselectAll();
    void seekToKeyframeOfSelected(); // Shift+K

    QUndoStack* undoStack() { return m_undoStack; }
    qint64 totalDurationMs() const;
    QSize  sizeHint() const override;

signals:
    void playheadMoved(qint64 ms);
    void clipSelected(const TimelineClip& clip);
    void clipDoubleClicked(const TimelineClip& clip);
    void requestScrub(qint64 ms);
    void tracksChanged();
    void keyframeAdded(qint64 posMs);

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseDoubleClickEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;
    void contextMenuEvent(QContextMenuEvent*) override;
    void keyPressEvent(QKeyEvent*) override;
    // Drag-and-drop from MediaBin
    void dragEnterEvent(QDragEnterEvent*) override;
    void dragMoveEvent(QDragMoveEvent*) override;
    void dropEvent(QDropEvent*) override;

private:
    void drawRuler(QPainter& p);
    void drawTrackHeaders(QPainter& p);
    void drawTracks(QPainter& p);
    void drawClip(QPainter& p, const TimelineClip& clip, const QRect& rect);
    void drawPlayhead(QPainter& p);
    void drawDropIndicator(QPainter& p);

    double msToX(qint64 ms)            const;
    qint64 xToMs(double x)             const;
    int    trackTop(int i)             const;
    int    trackHeight(int i)          const;
    QRect  clipRect(const TimelineClip& c) const;

    struct HitResult {
        int  trackIndex  = -1;
        int  clipIndex   = -1;
        bool onLeftEdge  = false;
        bool onRightEdge = false;
        bool onHeader    = false;
    };
    HitResult hitTest(const QPoint& pos) const;
    
    void extracted(qint64 &cutMs, TimelineClip &c, TimelineClip &left,
                   TimelineClip &right);
    void razorAtPoint(const QPoint &pos);
    void deleteSelected();
    qint64 snapMs(qint64 ms, qint64 thresh = 200) const;

    // Drop indicator state
    int    m_dropTrackIndex = -1;
    qint64 m_dropMs         = 0;
    bool   m_showDropIndicator = false;

    QVector<Track>*     m_tracks      = nullptr;
    qint64              m_playheadMs  = 0;
    double              m_pxPerSec    = kDefaultPxPerSec;
    EditTool            m_currentTool = EditTool::Select;
    QUndoStack*         m_undoStack   = nullptr;
    ThumbnailGenerator* m_thumbGen    = nullptr;

    bool   m_dragging       = false;
    bool   m_dragPlayhead   = false;
    bool   m_trimLeft       = false;
    bool   m_trimRight      = false;
    int    m_dragTrackIndex = -1;
    int    m_dragClipIndex  = -1;
    qint64 m_dragOffsetMs   = 0;
    qint64 m_originalStart  = 0;
    QPoint m_lastMousePos;
};

// ─── Timeline ─────────────────────────────────────────────────────────────────
class Timeline : public QWidget
{
    Q_OBJECT

public:
    explicit Timeline(QWidget* parent = nullptr);

    void setCurrentTool(EditTool t);
    void addClipToTrack(int trackIndex, const TimelineClip& clip);
    void addVideoTrack(const QString& name = QString());
    void addAudioTrack(const QString& name = QString());
    void removeTrack(int index);
    qint64 playheadMs() const;

    QVector<Track>& tracks()             { return m_tracks; }
    const QVector<Track>& tracks() const { return m_tracks; }

    QUndoStack* undoStack();

    // Expose view for command panel connections
    TimelineView* view() { return m_view; }

public slots:
    void setPlayheadMs(qint64 ms);
    void onSplitAtPlayhead();
    void onDuplicateSelected();
    void onAddKeyframe();
    void onZoomIn();
    void onZoomOut();
    void onZoomReset();

signals:
    void playheadMoved(qint64 ms);
    void clipSelected(const TimelineClip& clip);
    void clipDoubleClicked(const TimelineClip& clip);
    void requestScrub(qint64 ms);
    void keyframeAdded(qint64 posMs);
    void tracksModified();

private:
    void buildUi();
    void initDefaultTracks();
    void updateViewSize();

    QVector<Track>  m_tracks;
    TimelineView*   m_view   = nullptr;
    QScrollArea*    m_scroll = nullptr;
};
