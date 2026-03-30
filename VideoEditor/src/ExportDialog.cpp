#include "ExportDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QRegularExpression>
#include <QDebug>
#include <QDir>

ExportDialog::ExportDialog(QWidget* parent, const CanvasSettings& canvas)
    : QDialog(parent)
{
    m_exportSettings.canvas = canvas;
    setWindowTitle(tr("Export"));
    setMinimumWidth(480);
    buildUi();
}

void ExportDialog::buildUi()
{
    auto* root = new QVBoxLayout(this);

    // ── Output file ───────────────────────────────────────────────────────────
    auto* grpOut = new QGroupBox(tr("Output"), this);
    auto* formOut = new QFormLayout(grpOut);

    auto* outRow = new QHBoxLayout();
    m_outputEdit = new QLineEdit(QDir::homePath() + "/output.mp4", this);
    auto* btnBrowse = new QPushButton(tr("Browse…"), this);
    connect(btnBrowse, &QPushButton::clicked, this, &ExportDialog::onBrowseOutput);
    outRow->addWidget(m_outputEdit);
    outRow->addWidget(btnBrowse);
    formOut->addRow(tr("File:"), outRow);

    m_fmtCombo = new QComboBox(this);
    m_fmtCombo->addItems({"mp4","mov","mkv","webm","avi","gif","mp3","wav"});
    connect(m_fmtCombo, &QComboBox::currentTextChanged,
            this,       &ExportDialog::onFormatChanged);
    formOut->addRow(tr("Format:"), m_fmtCombo);

    root->addWidget(grpOut);

    // ── Video ─────────────────────────────────────────────────────────────────
    auto* grpVid = new QGroupBox(tr("Video"), this);
    auto* formVid = new QFormLayout(grpVid);

    m_vCodecCombo = new QComboBox(this);
    m_vCodecCombo->addItems({"libx264","libx265","libvpx-vp9","prores_ks","copy"});
    formVid->addRow(tr("Codec:"), m_vCodecCombo);

    m_crfSlider = new QSlider(Qt::Horizontal, this);
    m_crfSlider->setRange(0, 51);
    m_crfSlider->setValue(18);
    m_lblCrf    = new QLabel("18 (High Quality)", this);
    connect(m_crfSlider, &QSlider::valueChanged, [this](int v){
        QString quality = v < 15 ? "Lossless" : v < 23 ? "High Quality" :
                          v < 30 ? "Medium"   : "Low Quality";
        m_lblCrf->setText(QString("%1  (%2)").arg(v).arg(quality));
    });
    auto* crfRow = new QHBoxLayout();
    crfRow->addWidget(m_crfSlider);
    crfRow->addWidget(m_lblCrf);
    formVid->addRow(tr("CRF (Quality):"), crfRow);

    m_spVBitrate = new QSpinBox(this);
    m_spVBitrate->setRange(500, 100000);
    m_spVBitrate->setValue(8000);
    m_spVBitrate->setSuffix(" kbps");
    formVid->addRow(tr("Max Bitrate:"), m_spVBitrate);

    m_presetCombo = new QComboBox(this);
    m_presetCombo->addItems({"ultrafast","superfast","veryfast","faster",
                              "fast","medium","slow","slower","veryslow"});
    m_presetCombo->setCurrentText("medium");
    m_presetCombo->setToolTip(tr("Slower = better compression at same quality"));
    formVid->addRow(tr("Speed Preset:"), m_presetCombo);

    root->addWidget(grpVid);

    // ── Audio ─────────────────────────────────────────────────────────────────
    auto* grpAud = new QGroupBox(tr("Audio"), this);
    auto* formAud = new QFormLayout(grpAud);

    m_chkAudio = new QCheckBox(tr("Include audio"), this);
    m_chkAudio->setChecked(true);
    formAud->addRow("", m_chkAudio);

    m_aCodecCombo = new QComboBox(this);
    m_aCodecCombo->addItems({"aac","mp3","opus","flac","pcm_s16le","copy"});
    formAud->addRow(tr("Codec:"), m_aCodecCombo);

    m_spABitrate = new QSpinBox(this);
    m_spABitrate->setRange(64, 1411);
    m_spABitrate->setValue(192);
    m_spABitrate->setSuffix(" kbps");
    formAud->addRow(tr("Bitrate:"), m_spABitrate);

    root->addWidget(grpAud);

    // ── Progress ──────────────────────────────────────────────────────────────
    m_progress    = new QProgressBar(this);
    m_progress->setVisible(false);
    m_statusLabel = new QLabel("", this);
    m_statusLabel->setStyleSheet("color:#888;");
    root->addWidget(m_progress);
    root->addWidget(m_statusLabel);

    // ── Buttons ───────────────────────────────────────────────────────────────
    auto* btnRow = new QHBoxLayout();
    btnRow->addStretch();
    m_btnExport = new QPushButton(tr("Export"), this);
    m_btnExport->setDefault(true);
    connect(m_btnExport, &QPushButton::clicked, this, &ExportDialog::onExportClicked);
    auto* btnCancel = new QPushButton(tr("Close"), this);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    btnRow->addWidget(btnCancel);
    btnRow->addWidget(m_btnExport);
    root->addLayout(btnRow);
}

void ExportDialog::onBrowseOutput()
{
    QString filter;
    QString fmt = m_fmtCombo->currentText();
    filter = QString("Video (*.%1);;All Files (*)").arg(fmt);

    QString path = QFileDialog::getSaveFileName(this, tr("Export To"), m_outputEdit->text(), filter);
    if (!path.isEmpty()) m_outputEdit->setText(path);
}

void ExportDialog::onFormatChanged(const QString& fmt)
{
    updateCodecChoices(fmt);
    // Update file extension
    QString path = m_outputEdit->text();
    int dot = path.lastIndexOf('.');
    if (dot >= 0) path = path.left(dot) + "." + fmt;
    m_outputEdit->setText(path);
}

void ExportDialog::updateCodecChoices(const QString& format)
{
    m_vCodecCombo->clear();
    if (format == "webm") {
        m_vCodecCombo->addItems({"libvpx-vp9","libvpx"});
        m_aCodecCombo->setCurrentText("opus");
    } else if (format == "gif") {
        m_vCodecCombo->addItem("gif");
        m_chkAudio->setChecked(false);
    } else if (format == "mp3" || format == "wav") {
        m_vCodecCombo->addItem("none");
    } else if (format == "mov") {
        m_vCodecCombo->addItems({"prores_ks","libx264","libx265","copy"});
    } else {
        m_vCodecCombo->addItems({"libx264","libx265","copy"});
    }
}

void ExportDialog::onExportClicked()
{
    QString out = m_outputEdit->text().trimmed();
    if (out.isEmpty()) {
        QMessageBox::warning(this, tr("Export"), tr("Please specify an output path."));
        return;
    }

    // Fill export settings from UI
    m_exportSettings.outputPath   = out;
    m_exportSettings.format       = m_fmtCombo->currentText();
    m_exportSettings.videoCodec   = m_vCodecCombo->currentText();
    m_exportSettings.audioCodec   = m_aCodecCombo->currentText();
    m_exportSettings.videoBitrate = m_spVBitrate->value();
    m_exportSettings.audioBitrate = m_spABitrate->value();
    m_exportSettings.crf          = m_crfSlider->value();
    m_exportSettings.preset       = m_presetCombo->currentText();
    m_exportSettings.includeAudio = m_chkAudio->isChecked();

    accept(); // Signal to MainWindow to kick off export
}

void ExportDialog::startExport(const QString& inputSpec)
{
    // Build ffmpeg command
    QString args = buildFFmpegArgs(inputSpec);

    m_ffmpeg = new QProcess(this);
    m_ffmpeg->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_ffmpeg, &QProcess::readyRead,    this, &ExportDialog::onProcessOutput);
    connect(m_ffmpeg, QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),
            [this](int code,QProcess::ExitStatus){ onProcessFinished(code); });

    m_progress->setVisible(true);
    m_progress->setRange(0, 0); // indeterminate
    m_btnExport->setEnabled(false);
    m_statusLabel->setText(tr("Exporting…"));

    m_ffmpeg->start("ffmpeg", args.split(' '));
    if (!m_ffmpeg->waitForStarted(3000)) {
        m_statusLabel->setText(tr("FFmpeg not found. Install FFmpeg and ensure it's in PATH."));
        m_progress->setVisible(false);
        m_btnExport->setEnabled(true);
    }
}

QString ExportDialog::buildFFmpegArgs(const QString& inputSpec) const
{
    const ExportSettings& s = m_exportSettings;
    QString args = QString("-y -f concat -safe 0 -i \"%1\" ").arg(inputSpec);

    // Video
    if (s.videoCodec != "none") {
        args += QString("-c:v %1 ").arg(s.videoCodec);
        if (s.videoCodec != "copy" && s.videoCodec != "prores_ks") {
            args += QString("-crf %1 -preset %2 ").arg(s.crf).arg(s.preset);
            args += QString("-maxrate %1k -bufsize %2k ").arg(s.videoBitrate).arg(s.videoBitrate * 2);
        }
        args += QString("-s %1x%2 -r %3 ")
            .arg(s.canvas.width).arg(s.canvas.height).arg(s.canvas.frameRate);
    }

    // Audio
    if (s.includeAudio) {
        args += QString("-c:a %1 -b:a %2k -ar %3 ")
            .arg(s.audioCodec).arg(s.audioBitrate).arg(s.canvas.sampleRate);
    } else {
        args += "-an ";
    }

    args += QString("\"%1\"").arg(s.outputPath);
    return args;
}

void ExportDialog::onProcessOutput()
{
    if (!m_ffmpeg) return;
    QByteArray data = m_ffmpeg->readAll();
    QString txt = QString::fromUtf8(data);
    // Parse progress from FFmpeg stderr "time=HH:MM:SS.ms"
    static QRegularExpression re("time=(\\d+):(\\d+):(\\d+)\\.(\\d+)");
    QRegularExpressionMatch match = re.match(txt);
    if (match.hasMatch()) {
        m_statusLabel->setText("Encoding: " + match.captured(0));
    }
}

void ExportDialog::onProcessFinished(int code)
{
    m_progress->setRange(0, 100);
    m_progress->setValue(code == 0 ? 100 : 0);
    m_btnExport->setEnabled(true);
    m_statusLabel->setText(code == 0
        ? tr("✅ Export complete: ") + m_exportSettings.outputPath
        : tr("❌ Export failed. Check FFmpeg is installed."));
}
