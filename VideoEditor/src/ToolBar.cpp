#include "ToolBar.h"
#include <QAction>
#include <QIcon>
#include <QKeySequence>

ToolBar::ToolBar(QWidget* parent)
    : QToolBar(tr("Edit Tools"), parent)
{
    setObjectName("EditToolBar");
    setMovable(false);
    setIconSize(QSize(22, 22));
    buildActions();
}

void ToolBar::buildActions()
{
    m_group = new QActionGroup(this);
    m_group->setExclusive(true);

    // ── Select ────────────────────────────────────────────────────────────────
    auto* actSelect = addAction(QIcon::fromTheme("edit-select"), tr("Select (V)"));
    actSelect->setCheckable(true);
    actSelect->setChecked(true);
    actSelect->setShortcut(QKeySequence("V"));
    actSelect->setToolTip(tr("Select & Move clips"));
    m_group->addAction(actSelect);
    connect(actSelect, &QAction::triggered, this, &ToolBar::onSelectTriggered);

    // ── Razor (Cut) ───────────────────────────────────────────────────────────
    auto* actRazor = addAction(QIcon::fromTheme("edit-cut"), tr("Razor (C)"));
    actRazor->setCheckable(true);
    actRazor->setShortcut(QKeySequence("C"));
    actRazor->setToolTip(tr("Cut clip at click position"));
    m_group->addAction(actRazor);
    connect(actRazor, &QAction::triggered, this, &ToolBar::onRazorTriggered);

    addSeparator();

    // ── Split at Playhead ─────────────────────────────────────────────────────
    auto* actSplit = addAction(QIcon::fromTheme("edit-find"), tr("Split (S)"));
    actSplit->setCheckable(false);
    actSplit->setShortcut(QKeySequence("S"));
    actSplit->setToolTip(tr("Split selected clip at playhead"));
    connect(actSplit, &QAction::triggered, this, &ToolBar::onSplitTriggered);

    // ── Duplicate ─────────────────────────────────────────────────────────────
    auto* actDupe = addAction(QIcon::fromTheme("edit-copy"), tr("Duplicate (D)"));
    actDupe->setCheckable(false);
    actDupe->setShortcut(QKeySequence("D"));
    actDupe->setToolTip(tr("Duplicate selected clip(s) and place after original"));
    connect(actDupe, &QAction::triggered, this, &ToolBar::onDuplicateTriggered);

    addSeparator();

    // ── Trim ──────────────────────────────────────────────────────────────────
    auto* actTrim = addAction(QIcon::fromTheme("transform-scale"), tr("Trim (T)"));
    actTrim->setCheckable(true);
    actTrim->setShortcut(QKeySequence("T"));
    actTrim->setToolTip(tr("Drag clip edges to trim in/out points"));
    m_group->addAction(actTrim);
    connect(actTrim, &QAction::triggered, this, &ToolBar::onTrimTriggered);
}

void ToolBar::onSelectTriggered()
{
    m_currentTool = EditTool::Select;
    emit toolChanged(m_currentTool);
}

void ToolBar::onRazorTriggered()
{
    m_currentTool = EditTool::Razor;
    emit toolChanged(m_currentTool);
}

void ToolBar::onSplitTriggered()
{
    // Momentary action — stays in current tool mode
    emit toolChanged(EditTool::Split);
}

void ToolBar::onDuplicateTriggered()
{
    emit toolChanged(EditTool::Duplicate);
}

void ToolBar::onTrimTriggered()
{
    m_currentTool = EditTool::Trim;
    emit toolChanged(m_currentTool);
}
