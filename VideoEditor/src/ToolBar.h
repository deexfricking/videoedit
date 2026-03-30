#pragma once
#include <QWidget>
#include <QToolBar>
#include <QActionGroup>

// ─── EditTool ─────────────────────────────────────────────────────────────────

enum class EditTool {
    Select,     // Arrow — move/resize clips
    Razor,      // Cut clip at click position
    Split,      // Split clip at playhead
    Duplicate,  // Clone selected clip(s)
    Trim        // Drag clip edges to trim
};

// ─── ToolBar ──────────────────────────────────────────────────────────────────

class ToolBar : public QToolBar
{
    Q_OBJECT

public:
    explicit ToolBar(QWidget* parent = nullptr);

    EditTool currentTool() const { return m_currentTool; }

signals:
    void toolChanged(EditTool tool);

private slots:
    void onSelectTriggered();
    void onRazorTriggered();
    void onSplitTriggered();
    void onDuplicateTriggered();
    void onTrimTriggered();

private:
    void buildActions();

    EditTool      m_currentTool = EditTool::Select;
    QActionGroup* m_group       = nullptr;
};
