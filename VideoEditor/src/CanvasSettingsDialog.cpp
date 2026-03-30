#include "CanvasSettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <cmath>

// Preset table: label → width × height
static const QVector<QPair<QString, QSize>> kPresets = {
    { "720p HD  (1280×720)",    QSize(1280,  720) },
    { "1080p FHD (1920×1080)", QSize(1920, 1080) },
    { "2K (2048×1080)",        QSize(2048, 1080) },
    { "1440p QHD (2560×1440)", QSize(2560, 1440) },
    { "4K UHD  (3840×2160)",   QSize(3840, 2160) },
    { "4K DCI  (4096×2160)",   QSize(4096, 2160) },
    { "Instagram Square (1080×1080)", QSize(1080,1080) },
    { "TikTok/Reels  (1080×1920)",   QSize(1080,1920) },
    { "Twitter/X  (1280×720)",       QSize(1280, 720) },
    { "Custom",                       QSize(0, 0)     },
};

static QString gcd2(int a, int b){ return b ? gcd2(b, a%b) : QString::number(a); }
static QString aspectStr(int w, int h){
    int g = gcd2(w,h).toInt();
    return QString("%1:%2").arg(w/g).arg(h/g);
}

// ─── CanvasSettingsDialog ─────────────────────────────────────────────────────

CanvasSettingsDialog::CanvasSettingsDialog(QWidget* parent,
                                            const CanvasSettings& current)
    : QDialog(parent), m_settings(current)
{
    setWindowTitle(tr("Canvas & Sequence Settings"));
    setMinimumWidth(420);
    buildUi(current);
}

void CanvasSettingsDialog::buildUi(const CanvasSettings& cur)
{
    auto* root = new QVBoxLayout(this);

    // ── Resolution ────────────────────────────────────────────────────────────
    auto* grpRes = new QGroupBox(tr("Resolution"), this);
    auto* formRes = new QFormLayout(grpRes);

    m_presetCombo = new QComboBox(this);
    for (auto& [label, _] : kPresets) m_presetCombo->addItem(label);
    connect(m_presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CanvasSettingsDialog::onPresetChanged);
    formRes->addRow(tr("Preset:"), m_presetCombo);

    // Width / Height
    auto* dimRow = new QHBoxLayout();
    m_spWidth  = new QSpinBox(this);
    m_spHeight = new QSpinBox(this);
    for (auto* sp : {m_spWidth, m_spHeight}) {
        sp->setRange(16, 7680);
        sp->setSingleStep(2);
        sp->setSuffix(" px");
    }
    m_spWidth->setValue(cur.width);
    m_spHeight->setValue(cur.height);

    m_chkLock = new QCheckBox(tr("Lock aspect ratio"), this);
    m_chkLock->setChecked(true);

    m_lblAspect = new QLabel(aspectStr(cur.width, cur.height), this);
    m_lblAspect->setStyleSheet("color:#888;");

    dimRow->addWidget(m_spWidth);
    dimRow->addWidget(new QLabel("×", this));
    dimRow->addWidget(m_spHeight);
    dimRow->addWidget(m_chkLock);
    dimRow->addWidget(m_lblAspect);
    formRes->addRow(tr("Size:"), dimRow);

    connect(m_spWidth,  &QSpinBox::valueChanged, this, &CanvasSettingsDialog::onCustomDimensionChanged);
    connect(m_spHeight, &QSpinBox::valueChanged, this, &CanvasSettingsDialog::onCustomDimensionChanged);

    root->addWidget(grpRes);

    // ── Frame rate ────────────────────────────────────────────────────────────
    auto* grpFps = new QGroupBox(tr("Frame Rate & Audio"), this);
    auto* formFps = new QFormLayout(grpFps);

    m_spFps = new QDoubleSpinBox(this);
    m_spFps->setRange(1.0, 240.0);
    m_spFps->setDecimals(3);
    m_spFps->setSuffix(" fps");
    m_spFps->setValue(cur.frameRate);

    // Common fps presets
    auto* fpsPreset = new QComboBox(this);
    fpsPreset->addItems({"23.976","24","25","29.97","30","50","59.94","60","120"});
    connect(fpsPreset, &QComboBox::currentTextChanged,
            [this](const QString& t){ m_spFps->setValue(t.toDouble()); });

    auto* fpsRow = new QHBoxLayout();
    fpsRow->addWidget(m_spFps);
    fpsRow->addWidget(fpsPreset);
    formFps->addRow(tr("Frame Rate:"), fpsRow);

    // Sample rate
    m_sampleCombo = new QComboBox(this);
    m_sampleCombo->addItems({"44100 Hz","48000 Hz","96000 Hz"});
    m_sampleCombo->setCurrentIndex(1); // 48000
    formFps->addRow(tr("Sample Rate:"), m_sampleCombo);

    // Colour space
    m_colorCombo = new QComboBox(this);
    m_colorCombo->addItems({"Rec.709","Rec.2020","DCI-P3","sRGB"});
    m_colorCombo->setCurrentText(cur.colorSpace);
    formFps->addRow(tr("Colour Space:"), m_colorCombo);

    // Pixel format
    m_pixFmtCombo = new QComboBox(this);
    m_pixFmtCombo->addItems({"yuv420p","yuv422p","yuv444p","rgb24"});
    m_pixFmtCombo->setCurrentText(cur.pixelFormat);
    formFps->addRow(tr("Pixel Format:"), m_pixFmtCombo);

    root->addWidget(grpFps);

    // ── Buttons ───────────────────────────────────────────────────────────────
    auto* bbox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(bbox, &QDialogButtonBox::accepted, this, &CanvasSettingsDialog::onApply);
    connect(bbox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    root->addWidget(bbox);

    // Select matching preset
    m_updatingPreset = true;
    for (int i = 0; i < kPresets.size(); ++i) {
        if (kPresets[i].second == QSize(cur.width, cur.height)) {
            m_presetCombo->setCurrentIndex(i);
            break;
        }
    }
    m_updatingPreset = false;
}

void CanvasSettingsDialog::onPresetChanged(int index)
{
    if (m_updatingPreset) return;
    QSize sz = kPresets[index].second;
    if (sz.isNull()) return; // Custom — let user edit
    m_updatingPreset = true;
    m_spWidth->setValue(sz.width());
    m_spHeight->setValue(sz.height());
    m_lblAspect->setText(aspectStr(sz.width(), sz.height()));
    m_updatingPreset = false;
}

void CanvasSettingsDialog::onCustomDimensionChanged()
{
    if (m_updatingPreset) return;
    m_lblAspect->setText(aspectStr(m_spWidth->value(), m_spHeight->value()));
    // Switch preset combo to "Custom"
    m_updatingPreset = true;
    m_presetCombo->setCurrentIndex(kPresets.size() - 1);
    m_updatingPreset = false;
}

void CanvasSettingsDialog::onApply()
{
    m_settings.width      = m_spWidth->value();
    m_settings.height     = m_spHeight->value();
    m_settings.frameRate  = m_spFps->value();
    m_settings.colorSpace = m_colorCombo->currentText();
    m_settings.pixelFormat= m_pixFmtCombo->currentText();

    QString srText = m_sampleCombo->currentText().split(' ').first();
    m_settings.sampleRate = srText.toInt();

    accept();
}
