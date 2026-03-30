#pragma once
#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>

struct CanvasSettings {
    int    width        = 1920;
    int    height       = 1080;
    double frameRate    = 30.0;
    int    sampleRate   = 48000;   // Hz
    QString colorSpace  = "Rec.709";
    QString pixelFormat = "yuv420p";
};

// ─── CanvasSettingsDialog ─────────────────────────────────────────────────────

class CanvasSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CanvasSettingsDialog(QWidget* parent             = nullptr,
                                  const CanvasSettings& current = CanvasSettings());

    CanvasSettings settings() const { return m_settings; }

private slots:
    void onPresetChanged(int index);
    void onCustomDimensionChanged();
    void onApply();

private:
    void buildUi(const CanvasSettings& cur);
    void applyPreset(const QString& preset);

    QComboBox*      m_presetCombo   = nullptr;
    QSpinBox*       m_spWidth       = nullptr;
    QSpinBox*       m_spHeight      = nullptr;
    QDoubleSpinBox* m_spFps         = nullptr;
    QComboBox*      m_sampleCombo   = nullptr;
    QComboBox*      m_colorCombo    = nullptr;
    QComboBox*      m_pixFmtCombo   = nullptr;
    QCheckBox*      m_chkLock       = nullptr;
    QLabel*         m_lblAspect     = nullptr;

    CanvasSettings  m_settings;
    bool            m_updatingPreset = false;
};
