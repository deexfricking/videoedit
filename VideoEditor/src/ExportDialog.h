#pragma once
#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QProgressBar>
#include <QPushButton>
#include <QProcess>
#include "CanvasSettingsDialog.h"

// ─── ExportSettings ───────────────────────────────────────────────────────────

struct ExportSettings {
    QString outputPath;
    QString format      = "mp4";      // container
    QString videoCodec  = "libx264";
    QString audioCodec  = "aac";
    int     videoBitrate= 8000;       // kbps
    int     audioBitrate= 192;        // kbps
    int     crf         = 18;         // constant rate factor (lower = better)
    QString preset      = "medium";   // encoder speed preset
    bool    includeAudio = true;
    CanvasSettings canvas;
};

// ─── ExportDialog ─────────────────────────────────────────────────────────────

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(QWidget* parent             = nullptr,
                          const CanvasSettings& canvas = CanvasSettings());

    ExportSettings exportSettings() const { return m_exportSettings; }

    // Start the actual FFmpeg export (call after dialog accepted)
    void startExport(const QString& inputSpec);   // inputSpec = ffmpeg concat demuxer file

private slots:
    void onBrowseOutput();
    void onFormatChanged(const QString& fmt);
    void onExportClicked();
    void onProcessOutput();
    void onProcessFinished(int code);

private:
    void buildUi();
    void updateCodecChoices(const QString& format);
    QString buildFFmpegArgs(const QString& inputSpec) const;

    QLineEdit*      m_outputEdit    = nullptr;
    QComboBox*      m_fmtCombo      = nullptr;
    QComboBox*      m_vCodecCombo   = nullptr;
    QComboBox*      m_aCodecCombo   = nullptr;
    QSpinBox*       m_spVBitrate    = nullptr;
    QSpinBox*       m_spABitrate    = nullptr;
    QSlider*        m_crfSlider     = nullptr;
    QLabel*         m_lblCrf        = nullptr;
    QComboBox*      m_presetCombo   = nullptr;
    QCheckBox*      m_chkAudio      = nullptr;
    QProgressBar*   m_progress      = nullptr;
    QLabel*         m_statusLabel   = nullptr;
    QPushButton*    m_btnExport     = nullptr;

    QProcess*       m_ffmpeg        = nullptr;
    ExportSettings  m_exportSettings;
};
