#include "MainWindow.h"
#include "MediaBin.h"
#include "VideoPlayer.h"
#include "Timeline.h"
#include "ToolBar.h"
#include "CropDialog.h"
#include "CanvasSettingsDialog.h"
#include "ExportDialog.h"
#include "ProjectManager.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QAction>
#include <QKeySequence>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QSettings>
#include <QLabel>
#include <QPushButton>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QDir>
#include <QFrame>
#include <QApplication>
#include <QShortcut>
#include <QTextStream>
#include <QDebug>

// ═══════════════════════════════════════════════════════════════════════════════
//  Construction
// ═══════════════════════════════════════════════════════════════════════════════

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("VideoEditor");
    setMinimumSize(1280, 720);
    resize(1600, 920);
    setAcceptDrops(true);

    applyDarkTheme();

    m_projectMgr = new ProjectManager(this);
    m_canvas     = ProjectManager::defaultCanvas();

    buildCentralArea();
    buildDocks();
    buildMenuBar();
    buildStatusBar();
    buildKeyboardShortcuts();
    connectSignals();

    updateStatusBar();
    setModified(false);

    QSettings s("VideoEditor","VideoEditor");
    restoreGeometry(s.value("geometry").toByteArray());
    restoreState(s.value("windowState").toByteArray());
}

MainWindow::~MainWindow() {}

// ═══════════════════════════════════════════════════════════════════════════════
//  UI Building
// ═══════════════════════════════════════════════════════════════════════════════

void MainWindow::buildCentralArea()
{
    m_toolBar = new ToolBar(this);
    addToolBar(Qt::TopToolBarArea, m_toolBar);

    auto* cw     = new QWidget(this);
    auto* vlay   = new QVBoxLayout(cw);
    vlay->setContentsMargins(0,0,0,0);
    vlay->setSpacing(0);

    m_player   = new VideoPlayer(cw);
    m_player->setMinimumHeight(220);

    m_timeline = new Timeline(cw);
    m_timeline->setMinimumHeight(200);

    auto* spl = new QSplitter(Qt::Vertical, cw);
    spl->addWidget(m_player);
    spl->addWidget(m_timeline);
    spl->setStretchFactor(0,3);
    spl->setStretchFactor(1,2);
    spl->setHandleWidth(4);

    vlay->addWidget(spl);
    setCentralWidget(cw);
}

void MainWindow::buildDocks()
{
    // ── Media Bin ─────────────────────────────────────────────────────────────
    m_mediaBin  = new MediaBin(this);
    m_mediaDock = new QDockWidget(tr("Media Bin"), this);
    m_mediaDock->setObjectName("MediaBinDock");
    m_mediaDock->setWidget(m_mediaBin);
    m_mediaDock->setMinimumWidth(200);
    m_mediaDock->setFeatures(QDockWidget::DockWidgetMovable |
                              QDockWidget::DockWidgetFloatable |
                              QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::LeftDockWidgetArea, m_mediaDock);

    // ── Effects Panel ─────────────────────────────────────────────────────────
    m_effectsPanel = new EffectsPanel(this);
    m_effectsDock = new QDockWidget(tr("Effects"), this);
    m_effectsDock->setObjectName("EffectsDock");
    m_effectsDock->setWidget(m_effectsPanel);
    m_effectsDock->setMinimumWidth(240);
    m_effectsDock->setFeatures(QDockWidget::DockWidgetMovable |
                                QDockWidget::DockWidgetFloatable |
                                QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::RightDockWidgetArea, m_effectsDock);

    // ── Command Panel ─────────────────────────────────────────────────────────
    buildCommandPanel();
}

void MainWindow::buildCommandPanel()
{
    auto* panel = new QWidget(this);
    panel->setMinimumWidth(220);
    auto* vlay  = new QVBoxLayout(panel);
    vlay->setContentsMargins(8,8,8,8);
    vlay->setSpacing(6);

    auto makeBtn = [&](const QString& label, const QString& shortcut = {}) {
        auto* b = new QPushButton(label, panel);
        if (!shortcut.isEmpty())
            b->setToolTip(shortcut);
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        return b;
    };

    // ── Playback group ────────────────────────────────────────────────────────
    auto* grpPlay = new QGroupBox(tr("Playback"), panel);
    auto* gPlay   = new QGridLayout(grpPlay);
    gPlay->setSpacing(4);

    auto* btnSkipBack  = makeBtn("⏮ -10s",   "Ctrl+Left");
    auto* btnStepBack  = makeBtn("◀ Frame",   "Left");
    auto* btnPlayPause = makeBtn("▶ / ⏸",    "Space");
    auto* btnStepFwd   = makeBtn("Frame ▶",   "Right");
    auto* btnSkipFwd   = makeBtn("+10s ⏭",   "Ctrl+Right");
    auto* btnStop      = makeBtn("⏹ Stop",    "Escape");

    // Make play/pause prominent
    btnPlayPause->setStyleSheet(
        "QPushButton{background:#1a5a28;border:1px solid #2a8a3a;"
        "font-weight:bold;font-size:13px;border-radius:4px;padding:6px;}"
        "QPushButton:hover{background:#226b32;}"
        "QPushButton:pressed{background:#134420;}");

    gPlay->addWidget(btnSkipBack,  0, 0);
    gPlay->addWidget(btnStepBack,  0, 1);
    gPlay->addWidget(btnPlayPause, 0, 2);
    gPlay->addWidget(btnStepFwd,   0, 3);
    gPlay->addWidget(btnSkipFwd,   0, 4);
    gPlay->addWidget(btnStop,      1, 0, 1, 5);

    connect(btnPlayPause, &QPushButton::clicked, this, &MainWindow::onTogglePlayPause);
    connect(btnStop,      &QPushButton::clicked, this, &MainWindow::onStop);
    connect(btnSkipBack,  &QPushButton::clicked, this, &MainWindow::onSkipBackward);
    connect(btnSkipFwd,   &QPushButton::clicked, this, &MainWindow::onSkipForward);
    connect(btnStepBack,  &QPushButton::clicked, this, &MainWindow::onStepBackward);
    connect(btnStepFwd,   &QPushButton::clicked, this, &MainWindow::onStepForward);
    vlay->addWidget(grpPlay);

    // ── Edit group ────────────────────────────────────────────────────────────
    auto* grpEdit = new QGroupBox(tr("Edit"), panel);
    auto* gEdit   = new QVBoxLayout(grpEdit);
    gEdit->setSpacing(4);

    auto* btnSplit = makeBtn("✂  Split at Playhead    [S]");
    auto* btnDupe  = makeBtn("⧉  Duplicate Selected  [D]");
    auto* btnDel   = makeBtn("🗑  Delete Selected   [Del]");
    btnSplit->setStyleSheet("QPushButton{background:#3a2800;border:1px solid #7a5500;border-radius:3px;}"
                            "QPushButton:hover{background:#4a3500;}");
    connect(btnSplit, &QPushButton::clicked, m_timeline, &Timeline::onSplitAtPlayhead);
    connect(btnDupe,  &QPushButton::clicked, m_timeline, &Timeline::onDuplicateSelected);
    connect(btnDel,   &QPushButton::clicked, [this]{ onDelete(); });
    gEdit->addWidget(btnSplit);
    gEdit->addWidget(btnDupe);
    gEdit->addWidget(btnDel);
    vlay->addWidget(grpEdit);

    // ── Keyframes group ───────────────────────────────────────────────────────
    auto* grpKf = new QGroupBox(tr("Keyframes"), panel);
    auto* gKf   = new QVBoxLayout(grpKf);
    gKf->setSpacing(4);

    auto* btnAddKf = makeBtn("◆  Add Keyframe   [Ctrl+K]");
    btnAddKf->setStyleSheet(
        "QPushButton{background:#3a2800;border:1px solid #d0a020;color:#ffc030;border-radius:3px;}"
        "QPushButton:hover{background:#4a3500;}");
    connect(btnAddKf, &QPushButton::clicked, m_timeline, &Timeline::onAddKeyframe);

    auto* kfHint = new QLabel(
        tr("Select a clip then press Ctrl+K\n"
           "or click above to add a keyframe\nat the playhead position."), panel);
    kfHint->setStyleSheet("color:#777;font-size:10px;");
    kfHint->setWordWrap(true);

    gKf->addWidget(btnAddKf);
    gKf->addWidget(kfHint);
    vlay->addWidget(grpKf);

    // ── Tracks group ──────────────────────────────────────────────────────────
    auto* grpTracks = new QGroupBox(tr("Tracks"), panel);
    auto* gTracks   = new QVBoxLayout(grpTracks);
    gTracks->setSpacing(4);

    auto* btnAddVid = makeBtn("+ Video Track");
    auto* btnAddAud = makeBtn("+ Audio Track");
    connect(btnAddVid, &QPushButton::clicked, [this]{ m_timeline->addVideoTrack(); });
    connect(btnAddAud, &QPushButton::clicked, [this]{ m_timeline->addAudioTrack(); });
    gTracks->addWidget(btnAddVid);
    gTracks->addWidget(btnAddAud);
    vlay->addWidget(grpTracks);

    // ── Project group ─────────────────────────────────────────────────────────
    auto* grpProj = new QGroupBox(tr("Project"), panel);
    auto* gProj   = new QVBoxLayout(grpProj);
    gProj->setSpacing(4);

    auto* btnImport = makeBtn("📂  Import Media  [Ctrl+I]");
    auto* btnExport = makeBtn("📤  Export...     [Ctrl+E]");
    auto* btnCanvas = makeBtn("🎬  Canvas Settings...");
    auto* btnCrop   = makeBtn("✂  Crop Selected...");

    btnExport->setStyleSheet(
        "QPushButton{background:#1a2a5a;border:1px solid #3a5aaf;"
        "font-weight:bold;border-radius:3px;padding:5px;}"
        "QPushButton:hover{background:#223370;}");

    connect(btnImport, &QPushButton::clicked, this, &MainWindow::onImportMedia);
    connect(btnExport, &QPushButton::clicked, this, &MainWindow::onExport);
    connect(btnCanvas, &QPushButton::clicked, this, &MainWindow::onCanvasSettings);
    connect(btnCrop,   &QPushButton::clicked, this, &MainWindow::onCropSelected);
    gProj->addWidget(btnImport);
    gProj->addWidget(btnExport);
    gProj->addWidget(btnCanvas);
    gProj->addWidget(btnCrop);
    vlay->addWidget(grpProj);

    vlay->addStretch();

    // Dock it
    m_commandDock = new QDockWidget(tr("Command Panel"), this);
    m_commandDock->setObjectName("CommandDock");
    m_commandDock->setWidget(panel);
    m_commandDock->setFeatures(QDockWidget::DockWidgetMovable |
                                QDockWidget::DockWidgetFloatable |
                                QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::RightDockWidgetArea, m_commandDock);
}

void MainWindow::buildMenuBar()
{
    auto mkAct = [](QMenu* menu, const QString& text,
                    const QKeySequence& key = {}) -> QAction* {
        auto* a = new QAction(text, menu);
        if (!key.isEmpty()) a->setShortcut(key);
        menu->addAction(a);
        return a;
    };

    // ── File ──────────────────────────────────────────────────────────────────
    auto* fileMenu = menuBar()->addMenu(tr("&File"));
    auto* actNew  = mkAct(fileMenu, tr("&New Project"),       QKeySequence::New);
    connect(actNew, &QAction::triggered, this, &MainWindow::onNewProject);
    auto* actOpen = mkAct(fileMenu, tr("&Open Project..."),   QKeySequence::Open);
    connect(actOpen, &QAction::triggered, this, &MainWindow::onOpenProject);
    fileMenu->addSeparator();
    auto* actSave   = mkAct(fileMenu, tr("&Save Project"),    QKeySequence::Save);
    connect(actSave, &QAction::triggered, this, &MainWindow::onSaveProject);
    auto* actSaveAs = mkAct(fileMenu, tr("Save Project &As..."), QKeySequence::SaveAs);
    connect(actSaveAs, &QAction::triggered, this, &MainWindow::onSaveProjectAs);
    fileMenu->addSeparator();
    m_recentMenu = fileMenu->addMenu(tr("Open &Recent"));
    updateRecentFilesMenu();
    fileMenu->addSeparator();
    auto* actImport = mkAct(fileMenu, tr("&Import Media..."), QKeySequence("Ctrl+I"));
    connect(actImport, &QAction::triggered, this, &MainWindow::onImportMedia);
    fileMenu->addSeparator();
    auto* actExport = mkAct(fileMenu, tr("&Export..."),       QKeySequence("Ctrl+E"));
    connect(actExport, &QAction::triggered, this, &MainWindow::onExport);
    fileMenu->addSeparator();
    auto* actQuit = mkAct(fileMenu, tr("&Quit"), QKeySequence::Quit);
    connect(actQuit, &QAction::triggered, qApp, &QApplication::quit);

    // ── Edit ──────────────────────────────────────────────────────────────────
    auto* editMenu = menuBar()->addMenu(tr("&Edit"));
    auto* actUndo   = mkAct(editMenu, tr("&Undo"),      QKeySequence::Undo);
    connect(actUndo,  &QAction::triggered, this, &MainWindow::onUndo);
    auto* actRedo   = mkAct(editMenu, tr("&Redo"),      QKeySequence::Redo);
    connect(actRedo,  &QAction::triggered, this, &MainWindow::onRedo);
    editMenu->addSeparator();
    auto* actCut   = mkAct(editMenu, tr("Cu&t"),        QKeySequence::Cut);
    connect(actCut,   &QAction::triggered, this, &MainWindow::onCut);
    auto* actCopy  = mkAct(editMenu, tr("&Copy"),       QKeySequence::Copy);
    connect(actCopy,  &QAction::triggered, this, &MainWindow::onCopy);
    auto* actPaste = mkAct(editMenu, tr("&Paste"),      QKeySequence::Paste);
    connect(actPaste, &QAction::triggered, this, &MainWindow::onPaste);
    auto* actDel   = mkAct(editMenu, tr("&Delete"),     QKeySequence::Delete);
    connect(actDel,   &QAction::triggered, this, &MainWindow::onDelete);
    auto* actSelAll= mkAct(editMenu, tr("Select &All"), QKeySequence::SelectAll);
    connect(actSelAll,&QAction::triggered, this, &MainWindow::onSelectAll);

    // ── View ──────────────────────────────────────────────────────────────────
    auto* viewMenu = menuBar()->addMenu(tr("&View"));

    auto* actMediaBin = new QAction(tr("&Media Bin"), viewMenu);
    actMediaBin->setCheckable(true); actMediaBin->setChecked(true);
    viewMenu->addAction(actMediaBin);
    connect(actMediaBin, &QAction::toggled,           m_mediaDock, &QDockWidget::setVisible);
    connect(m_mediaDock, &QDockWidget::visibilityChanged, actMediaBin, &QAction::setChecked);

    auto* actCmdPanel = new QAction(tr("&Command Panel"), viewMenu);
    actCmdPanel->setCheckable(true); actCmdPanel->setChecked(true);
    viewMenu->addAction(actCmdPanel);
    connect(actCmdPanel, &QAction::toggled,              m_commandDock, &QDockWidget::setVisible);
    connect(m_commandDock,&QDockWidget::visibilityChanged,actCmdPanel, &QAction::setChecked);

    viewMenu->addSeparator();
    auto* actZIn    = mkAct(viewMenu, tr("Zoom &In"),    QKeySequence("Ctrl+="));
    connect(actZIn,   &QAction::triggered, m_timeline, &Timeline::onZoomIn);
    auto* actZOut   = mkAct(viewMenu, tr("Zoom &Out"),   QKeySequence("Ctrl+-"));
    connect(actZOut,  &QAction::triggered, m_timeline, &Timeline::onZoomOut);
    auto* actZReset = mkAct(viewMenu, tr("Zoom &Reset"), QKeySequence("Ctrl+0"));
    connect(actZReset,&QAction::triggered, m_timeline, &Timeline::onZoomReset);

    // ── Clip ──────────────────────────────────────────────────────────────────
    auto* clipMenu = menuBar()->addMenu(tr("&Clip"));
    auto* actSplit = mkAct(clipMenu, tr("&Split at Playhead"), QKeySequence("S"));
    connect(actSplit, &QAction::triggered, m_timeline, &Timeline::onSplitAtPlayhead);
    auto* actDupe  = mkAct(clipMenu, tr("&Duplicate"),         QKeySequence("Ctrl+D"));
    connect(actDupe, &QAction::triggered,  m_timeline, &Timeline::onDuplicateSelected);
    auto* actKf    = mkAct(clipMenu, tr("Add &Keyframe"),      QKeySequence("Ctrl+K"));
    connect(actKf,   &QAction::triggered,  m_timeline, &Timeline::onAddKeyframe);
    auto* actCrop  = mkAct(clipMenu, tr("&Crop..."),           QKeySequence("Ctrl+Shift+C"));
    connect(actCrop, &QAction::triggered,  this, &MainWindow::onCropSelected);
    clipMenu->addSeparator();
    auto* actCanvas= mkAct(clipMenu, tr("Canvas &Settings..."),QKeySequence("Ctrl+Shift+S"));
    connect(actCanvas,&QAction::triggered, this, &MainWindow::onCanvasSettings);

    // ── Playback ──────────────────────────────────────────────────────────────
    auto* playMenu = menuBar()->addMenu(tr("&Playback"));
    auto* actPP    = mkAct(playMenu, tr("Play / Pause"),   QKeySequence("Space"));
    connect(actPP,   &QAction::triggered, this, &MainWindow::onTogglePlayPause);
    auto* actStop2 = mkAct(playMenu, tr("Stop"),            QKeySequence("Escape"));
    connect(actStop2,&QAction::triggered, this, &MainWindow::onStop);
    auto* actSFwd  = mkAct(playMenu, tr("Step Forward"),    QKeySequence("Right"));
    connect(actSFwd, &QAction::triggered, this, &MainWindow::onStepForward);
    auto* actSBack = mkAct(playMenu, tr("Step Back"),       QKeySequence("Left"));
    connect(actSBack,&QAction::triggered, this, &MainWindow::onStepBackward);
    auto* actSkFwd = mkAct(playMenu, tr("Skip +10s"),       QKeySequence("Ctrl+Right"));
    connect(actSkFwd,&QAction::triggered, this, &MainWindow::onSkipForward);
    auto* actSkBck = mkAct(playMenu, tr("Skip -10s"),       QKeySequence("Ctrl+Left"));
    connect(actSkBck,&QAction::triggered, this, &MainWindow::onSkipBackward);
    playMenu->addSeparator();
    auto* actGoS = mkAct(playMenu, tr("Go to Start"), QKeySequence("Home"));
    connect(actGoS,  &QAction::triggered, [this]{
        m_player->seekTo(0); m_timeline->setPlayheadMs(0); });
    auto* actGoE = mkAct(playMenu, tr("Go to End"),   QKeySequence("End"));
    connect(actGoE,  &QAction::triggered, [this]{
        qint64 end = 0;
        for (auto& t : m_timeline->tracks())
            for (auto& c : t.clips) end = qMax(end, c.timelineEnd());
        m_player->seekTo(end); m_timeline->setPlayheadMs(end);
    });

    // ── Help ──────────────────────────────────────────────────────────────────
    auto* helpMenu = menuBar()->addMenu(tr("&Help"));
    auto* actAbout = mkAct(helpMenu, tr("&About VideoEditor"));
    connect(actAbout, &QAction::triggered, [this]{
        QMessageBox::about(this, tr("About VideoEditor"),
            tr("<b>VideoEditor v1.0</b><br><br>"
               "Professional non-linear editor — Qt6 + C++17<br><br>"
               "<b>Shortcuts:</b><br>"
               "Space — Play/Pause<br>"
               "Ctrl+Left/Right — Skip 10s<br>"
               "Left/Right — Step frame<br>"
               "S — Split at playhead<br>"
               "D — Duplicate clip<br>"
               "Ctrl+K — Add keyframe<br>"
               "V/C/T — Select/Razor/Trim tools<br>"
               "Ctrl+Z/Y — Undo/Redo<br>"
               "Ctrl+I/E — Import/Export<br>"
               "Ctrl+=/-/0 — Zoom timeline"));
    });
}

void MainWindow::buildStatusBar()
{
    m_lblTimecode   = new QLabel("00:00:00:00", this);
    m_lblResolution = new QLabel("1920x1080",   this);
    m_lblFps        = new QLabel("30.000 fps",  this);
    m_lblTool       = new QLabel("Select",      this);
    for (auto* lbl : {m_lblTimecode, m_lblResolution, m_lblFps, m_lblTool}) {
        lbl->setStyleSheet("color:#ccc;padding:0 8px;font-family:monospace;");
        statusBar()->addPermanentWidget(lbl);
    }
    statusBar()->showMessage(tr("Ready — drag media from the Media Bin onto the timeline tracks"));
}

void MainWindow::buildKeyboardShortcuts()
{
    // Space bar for play/pause (global shortcut in case focus is elsewhere)
    auto* spaceShortcut = new QShortcut(QKeySequence("Space"), this);
    connect(spaceShortcut, &QShortcut::activated, this, &MainWindow::onTogglePlayPause);
}

void MainWindow::connectSignals()
{
    // Toolbar tool changes
    connect(m_toolBar, &ToolBar::toolChanged, [this](EditTool t){
        m_timeline->setCurrentTool(t);
        QString name;
        switch(t){
        case EditTool::Select:    name="Select";    break;
        case EditTool::Razor:     name="Razor";     break;
        case EditTool::Split:     name="Split";     break;
        case EditTool::Duplicate: name="Duplicate"; break;
        case EditTool::Trim:      name="Trim";      break;
        }
        m_lblTool->setText(name);
        if (t == EditTool::Split)     m_timeline->onSplitAtPlayhead();
        if (t == EditTool::Duplicate) m_timeline->onDuplicateSelected();
    });

    connect(m_timeline, &Timeline::playheadMoved,    m_player,   &VideoPlayer::seekTo);
    connect(m_timeline, &Timeline::requestScrub,     m_player,   &VideoPlayer::seekTo);
    connect(m_player,   &VideoPlayer::positionChanged,m_timeline,&Timeline::setPlayheadMs);

    connect(m_player, &VideoPlayer::positionChanged, [this](qint64 ms){
        double fps = m_canvas.frameRate;
        qint64 tf = qint64(ms/1000.0*fps);
        qint64 h=tf/qint64(fps*3600), m2=(tf/qint64(fps*60))%60,
               s=tf/qint64(fps)%60,  f=tf%qint64(fps);
        m_lblTimecode->setText(QString("%1:%2:%3:%4")
            .arg(h,2,10,QChar('0')).arg(m2,2,10,QChar('0'))
            .arg(s,2,10,QChar('0')).arg(f,2,10,QChar('0')));
    });

    connect(m_mediaBin, &MediaBin::mediaItemDoubleClicked,
            this,       &MainWindow::onMediaItemDoubleClicked);
    connect(m_timeline, &Timeline::clipSelected,      this, &MainWindow::onClipSelected);
    connect(m_timeline, &Timeline::clipDoubleClicked, this, &MainWindow::onClipDoubleClicked);
    connect(m_timeline, &Timeline::keyframeAdded,     this, &MainWindow::onKeyframeAdded);

    if (auto* stack = m_timeline->undoStack())
        connect(stack, &QUndoStack::indexChanged, [this]{ setModified(true); });
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Dark Theme
// ═══════════════════════════════════════════════════════════════════════════════

void MainWindow::applyDarkTheme()
{
    qApp->setStyle("Fusion");
    QPalette dark;
    dark.setColor(QPalette::Window,        QColor(28,28,34));
    dark.setColor(QPalette::WindowText,    QColor(220,220,225));
    dark.setColor(QPalette::Base,          QColor(18,18,22));
    dark.setColor(QPalette::AlternateBase, QColor(30,30,38));
    dark.setColor(QPalette::Text,          QColor(215,215,220));
    dark.setColor(QPalette::Button,        QColor(42,42,52));
    dark.setColor(QPalette::ButtonText,    QColor(215,215,220));
    dark.setColor(QPalette::Highlight,     QColor(50,100,200));
    dark.setColor(QPalette::HighlightedText, Qt::white);
    dark.setColor(QPalette::Mid,           QColor(40,40,50));
    dark.setColor(QPalette::Dark,          QColor(20,20,26));
    dark.setColor(QPalette::Shadow,        QColor(8,8,12));
    dark.setColor(QPalette::Light,         QColor(55,55,65));
    qApp->setPalette(dark);

    qApp->setStyleSheet(R"(
        QMainWindow       { background:#1c1c22; }
        QMenuBar          { background:#1c1c22; color:#ddd; }
        QMenuBar::item:selected { background:#2a4a7a; border-radius:3px; }
        QMenu             { background:#252530; border:1px solid #404050; color:#ddd; }
        QMenu::item:selected  { background:#2a4a7a; }
        QMenu::separator  { height:1px; background:#404050; margin:3px 0; }
        QToolBar          { background:#222228; border-bottom:1px solid #333; padding:2px; }
        QToolButton       { background:#2a2a34; border:1px solid #3a3a4a; border-radius:3px;
                            padding:3px 6px; color:#ddd; }
        QToolButton:checked,QToolButton:pressed { background:#2a4a7a; border-color:#4a7abf; }
        QToolButton:hover { background:#33334a; }
        QPushButton       { background:#2a2a36; border:1px solid #404055; border-radius:3px;
                            padding:4px 10px; color:#ddd; }
        QPushButton:hover { background:#333348; }
        QPushButton:pressed{ background:#1c3260; }
        QDockWidget::title{ background:#222230; padding:4px 8px; border-bottom:1px solid #333; }
        QScrollBar:vertical   { width:10px;  background:#1a1a20; border-radius:5px; }
        QScrollBar:horizontal { height:10px; background:#1a1a20; border-radius:5px; }
        QScrollBar::handle    { background:#404050; border-radius:5px; min-length:30px; }
        QScrollBar::handle:hover { background:#505060; }
        QScrollBar::add-line,QScrollBar::sub-line { height:0; width:0; }
        QGroupBox  { border:1px solid #404050; border-radius:4px; margin-top:8px;
                     color:#aaa; padding:6px; }
        QGroupBox::title { subcontrol-origin:margin; left:8px; top:-1px; color:#88aadd; }
        QSplitter::handle { background:#333340; }
        QStatusBar { background:#18181e; border-top:1px solid #333; color:#888; }
        QComboBox  { background:#2a2a36; border:1px solid #404055; border-radius:3px;
                     padding:3px 6px; color:#ddd; }
        QComboBox QAbstractItemView { background:#252530; border:1px solid #505060;
                                      selection-background-color:#2a4a7a; }
        QSpinBox,QDoubleSpinBox,QLineEdit { background:#1e1e28; border:1px solid #404055;
                     border-radius:3px; padding:3px 6px; color:#ddd; }
        QSlider::groove:horizontal { height:4px; background:#404050; border-radius:2px; }
        QSlider::handle:horizontal { width:12px; height:12px; background:#4a9eff;
                                      border-radius:6px; margin:-4px 0; }
        QSlider::sub-page:horizontal { background:#4a7abf; border-radius:2px; }
        QProgressBar { border:1px solid #404050; border-radius:3px; background:#1e1e28;
                       text-align:center; color:#ccc; }
        QProgressBar::chunk { background:#2a6abf; border-radius:3px; }
    )");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Slots
// ═══════════════════════════════════════════════════════════════════════════════

void MainWindow::onNewProject()
{
    if (!confirmDiscardChanges()) return;
    m_timeline->tracks().clear();
    m_timeline->addVideoTrack("Video 1");
    m_timeline->addVideoTrack("Video 2");
    m_timeline->addAudioTrack("Audio 1");
    m_timeline->addAudioTrack("Audio 2");
    m_canvas = ProjectManager::defaultCanvas();
    setProjectPath(QString());
    setModified(false);
    updateStatusBar();
    statusBar()->showMessage(tr("New project created."), 3000);
}

void MainWindow::onOpenProject()
{
    if (!confirmDiscardChanges()) return;
    QString path = QFileDialog::getOpenFileName(
        this, tr("Open Project"), QDir::homePath(),
        tr("VideoEditor Project (*.veproj);;All Files (*)"));
    if (path.isEmpty()) return;
    QVector<Track> tracks; CanvasSettings canvas;
    if (!m_projectMgr->loadProject(path, tracks, canvas)) {
        QMessageBox::critical(this, tr("Open Project"), m_projectMgr->lastError()); return;
    }
    m_timeline->tracks() = tracks;
    m_canvas = canvas;
    setProjectPath(path);
    setModified(false);
    updateStatusBar();
    updateRecentFilesMenu();
    statusBar()->showMessage(tr("Opened: ") + path, 4000);
}

void MainWindow::onSaveProject()
{
    if (m_projectPath.isEmpty()) { onSaveProjectAs(); return; }
    if (!m_projectMgr->saveProject(m_projectPath, m_timeline->tracks(), m_canvas)) {
        QMessageBox::critical(this, tr("Save"), m_projectMgr->lastError()); return;
    }
    setModified(false);
    statusBar()->showMessage(tr("Saved."), 3000);
}

void MainWindow::onSaveProjectAs()
{
    QString path = QFileDialog::getSaveFileName(
        this, tr("Save Project As"), QDir::homePath(),
        tr("VideoEditor Project (*.veproj);;All Files (*)"));
    if (path.isEmpty()) return;
    if (!path.endsWith(".veproj")) path += ".veproj";
    if (!m_projectMgr->saveProject(path, m_timeline->tracks(), m_canvas)) {
        QMessageBox::critical(this, tr("Save"), m_projectMgr->lastError()); return;
    }
    setProjectPath(path); setModified(false);
    updateRecentFilesMenu();
    statusBar()->showMessage(tr("Saved: ") + path, 4000);
}

void MainWindow::onImportMedia() { m_mediaBin->importMedia(); }

void MainWindow::onExport()
{
    ExportDialog dlg(this, m_canvas);
    if (dlg.exec() != QDialog::Accepted) return;
    QString concatPath = exportConcatFile();
    if (concatPath.isEmpty()) {
        QMessageBox::warning(this, tr("Export"), tr("Nothing on the timeline to export."));
        return;
    }
    ExportDialog exportDlg(this, m_canvas);
    exportDlg.startExport(concatPath);
    exportDlg.exec();
}

QString MainWindow::exportConcatFile() const
{
    QVector<TimelineClip> all;
    for (auto& t : m_timeline->tracks())
        for (auto& c : t.clips) all.append(c);
    if (all.isEmpty()) return QString();
    std::sort(all.begin(), all.end(),
              [](const TimelineClip& a, const TimelineClip& b){
                  return a.timelineStart < b.timelineStart; });
    QTemporaryFile tmp;
    tmp.setAutoRemove(false);
    tmp.setFileTemplate(QDir::tempPath() + "/ve_concat_XXXXXX.txt");
    if (!tmp.open()) return QString();
    QTextStream out(&tmp);
    out << "ffconcat version 1.0\n";
    for (auto& c : all) {
        out << QString("file '%1'\n").arg(c.filePath);
        out << QString("inpoint %1\n").arg(c.inPoint/1000.0, 0,'f',3);
        out << QString("outpoint %1\n").arg(c.outPoint/1000.0,0,'f',3);
    }
    tmp.close();
    return tmp.fileName();
}

void MainWindow::onUndo() { if (auto* s = m_timeline->undoStack()) s->undo(); }
void MainWindow::onRedo() { if (auto* s = m_timeline->undoStack()) s->redo(); }
void MainWindow::onCut()  { statusBar()->showMessage(tr("Cut — not yet implemented."),2000); }
void MainWindow::onCopy() { statusBar()->showMessage(tr("Copy — not yet implemented."),2000); }
void MainWindow::onPaste(){ statusBar()->showMessage(tr("Paste — not yet implemented."),2000); }
void MainWindow::onDelete(){
    if (m_timeline && m_timeline->view()) m_timeline->view()->removeSelectedClips();
}
void MainWindow::onSelectAll(){
    if (m_timeline && m_timeline->view()) m_timeline->view()->selectAll();
}

void MainWindow::onCanvasSettings()
{
    CanvasSettingsDialog dlg(this, m_canvas);
    if (dlg.exec() == QDialog::Accepted) {
        m_canvas = dlg.settings();
        setModified(true); updateStatusBar();
        statusBar()->showMessage(
            tr("Canvas: %1x%2 @ %3fps")
            .arg(m_canvas.width).arg(m_canvas.height).arg(m_canvas.frameRate), 4000);
    }
}

void MainWindow::onCropSelected()
{
    CropDialog dlg(this, QPixmap());
    if (dlg.exec() == QDialog::Accepted) {
        auto r = dlg.result();
        statusBar()->showMessage(
            tr("Crop applied — L:%1% T:%2% R:%3% B:%4%")
            .arg(r.left*100,0,'f',1).arg(r.top*100,0,'f',1)
            .arg(r.right*100,0,'f',1).arg(r.bottom*100,0,'f',1), 4000);
        setModified(true);
    }
}

void MainWindow::onToggleMediaBin(bool v) { m_mediaDock->setVisible(v); }

// Playback
void MainWindow::onPlay()
{
    updatePlaybackClip();  // Load and sync the clip at playhead
    m_player->play();
}

void MainWindow::updatePlaybackClip()
{
    // Find the clip at the current playhead position
    qint64 playheadMs = m_timeline->playheadMs();
    
    for (auto& track : m_timeline->tracks()) {
        if (track.type != TrackType::Video) continue;
        for (auto& clip : track.clips) {
            if (playheadMs >= clip.timelineStart && playheadMs < clip.timelineEnd()) {
                // Found the clip at playhead - load it if not already loaded
                if (m_currentPlayingClipId != clip.id) {
                    m_currentPlayingClipId = clip.id;
                    qint64 relativeMs = playheadMs - clip.timelineStart + clip.inPoint;
                    m_player->loadMedia(clip.filePath);
                    m_player->seekTo(relativeMs);
                }
                return;
            }
        }
    }
    
    // No clip at playhead
    m_currentPlayingClipId = QUuid();
}
void MainWindow::onPause()          { m_player->pause(); }
void MainWindow::onStop()           { m_player->stop(); m_timeline->setPlayheadMs(0); }
void MainWindow::onTogglePlayPause(){ m_player->togglePlayPause(); }
void MainWindow::onStepForward()    { m_player->stepForward(); }
void MainWindow::onStepBackward()   { m_player->stepBackward(); }

void MainWindow::onSkipForward()
{
    qint64 newPos = qMin(m_player->position() + 10000, m_player->duration());
    m_player->seekTo(newPos);
    m_timeline->setPlayheadMs(newPos);
}

void MainWindow::onSkipBackward()
{
    qint64 newPos = qMax(m_player->position() - 10000, qint64(0));
    m_player->seekTo(newPos);
    m_timeline->setPlayheadMs(newPos);
}

void MainWindow::onPlayheadMoved(qint64 ms)
{
    // Find the clip currently playing at this playhead position
    TimelineClip* currentClip = nullptr;
    for (auto& track : m_timeline->tracks()) {
        if (track.type != TrackType::Video) continue;
        for (auto& clip : track.clips) {
            if (ms >= clip.timelineStart && ms < clip.timelineEnd()) {
                currentClip = &clip;
                break;
            }
        }
        if (currentClip) break;
    }
    
    if (currentClip) {
        // Calculate position within the source clip
        qint64 offsetInTimeline = ms - currentClip->timelineStart;
        qint64 posInSourceClip = currentClip->inPoint + offsetInTimeline;
        m_player->seekTo(posInSourceClip);
    } else {
        // No clip at playhead, just seek the player directly
        m_player->seekTo(ms);
    }
}

void MainWindow::onMediaItemDoubleClicked(const MediaItem& item)
{
    m_player->loadMedia(item.filePath);
    statusBar()->showMessage(tr("Loaded: ") + item.name, 3000);
}

void MainWindow::onClipAddedToTimeline(const MediaItem& item)
{
    for (int i = 0; i < m_timeline->tracks().size(); ++i) {
        Track& t = m_timeline->tracks()[i];
        bool audioOnly = (item.type == ClipType::Audio);
        if (audioOnly != (t.type == TrackType::Audio)) continue;
        qint64 end = 0;
        for (auto& c : t.clips) end = qMax(end, c.timelineEnd());
        TimelineClip clip;
        clip.filePath      = item.filePath;
        clip.name          = item.name;
        clip.type          = item.type;
        clip.timelineStart = end;
        clip.inPoint       = 0;
        clip.outPoint      = qMax(item.durationMs, qint64(5000));
        clip.sourceDuration= item.durationMs;
        clip.trackIndex    = i;
        m_timeline->addClipToTrack(i, clip);
        setModified(true);
        statusBar()->showMessage(
            tr("Added '%1' to %2").arg(item.name).arg(t.name), 3000);
        return;
    }
    statusBar()->showMessage(tr("No compatible track — add one first."), 3000);
}

void MainWindow::onClipSelected(const TimelineClip& clip)
{
    m_effectsPanel->setSelectedClip(clip);
    statusBar()->showMessage(
        tr("Selected: %1  |  %2ms  |  Keyframes: %3")
        .arg(clip.name).arg(clip.duration()).arg(clip.keyframes.size()), 4000);
}

void MainWindow::onClipDoubleClicked(const TimelineClip& clip)
{
    m_player->loadMedia(clip.filePath);
    m_player->seekTo(clip.inPoint);
    statusBar()->showMessage(tr("Preview: ") + clip.name, 2000);
}

void MainWindow::onKeyframeAdded(qint64 posMs)
{
    statusBar()->showMessage(
        tr("Keyframe added at %1ms").arg(posMs), 3000);
}

void MainWindow::onOpenRecentFile()
{
    auto* act = qobject_cast<QAction*>(sender());
    if (!act) return;
    QString path = act->data().toString();
    if (!QFile::exists(path)) {
        QMessageBox::warning(this, tr("Open Recent"), tr("File not found:\n") + path);
        return;
    }
    if (!confirmDiscardChanges()) return;
    QVector<Track> tracks; CanvasSettings canvas;
    if (m_projectMgr->loadProject(path, tracks, canvas)) {
        m_timeline->tracks() = tracks;
        m_canvas = canvas;
        setProjectPath(path); setModified(false); updateStatusBar();
    } else {
        QMessageBox::critical(this, tr("Open Recent"), m_projectMgr->lastError());
    }
}

void MainWindow::updateStatusBar()
{
    m_lblResolution->setText(QString("%1x%2").arg(m_canvas.width).arg(m_canvas.height));
    m_lblFps->setText(QString("%1 fps").arg(m_canvas.frameRate,0,'f',3));
}

// ─── Drag & Drop ──────────────────────────────────────────────────────────────

void MainWindow::dragEnterEvent(QDragEnterEvent* e)
{
    if (e->mimeData()->hasUrls()) e->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent* e)
{
    for (const QUrl& url : e->mimeData()->urls())
        if (url.isLocalFile())
            m_mediaBin->importMedia();
}

// ─── Helpers ──────────────────────────────────────────────────────────────────

void MainWindow::setProjectPath(const QString& path)
{
    m_projectPath = path;
    setWindowTitle(path.isEmpty()
        ? "VideoEditor"
        : QFileInfo(path).baseName() + " — VideoEditor");
}

void MainWindow::setModified(bool m)
{
    m_modified = m;
    QString t = windowTitle();
    if (m  && !t.startsWith("*")) setWindowTitle("*" + t);
    if (!m &&  t.startsWith("*")) setWindowTitle(t.mid(1));
}

bool MainWindow::confirmDiscardChanges()
{
    if (!m_modified) return true;
    auto btn = QMessageBox::question(this, tr("Unsaved Changes"),
        tr("Discard unsaved changes?"),
        QMessageBox::Discard | QMessageBox::Cancel | QMessageBox::Save);
    if (btn == QMessageBox::Save)    { onSaveProject(); return !m_modified; }
    if (btn == QMessageBox::Discard) return true;
    return false;
}

void MainWindow::updateRecentFilesMenu()
{
    m_recentMenu->clear();
    QStringList recent = m_projectMgr->recentFiles();
    if (recent.isEmpty()) {
        m_recentMenu->addAction(tr("(none)"))->setEnabled(false); return;
    }
    for (const QString& p : recent) {
        auto* a = m_recentMenu->addAction(QFileInfo(p).fileName());
        a->setData(p); a->setToolTip(p);
        connect(a, &QAction::triggered, this, &MainWindow::onOpenRecentFile);
    }
    m_recentMenu->addSeparator();
    m_recentMenu->addAction(tr("Clear Recent"), [this]{
        m_projectMgr->clearRecentFiles(); updateRecentFilesMenu();
    });
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    if (!confirmDiscardChanges()) { e->ignore(); return; }
    QSettings s("VideoEditor","VideoEditor");
    s.setValue("geometry",    saveGeometry());
    s.setValue("windowState", saveState());
    e->accept();
}
