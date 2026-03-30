#include "EffectsPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>

EffectsPanel::EffectsPanel(QWidget* parent)
    : QWidget(parent)
{
    buildUI();
}

void EffectsPanel::buildUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(8);
    
    // Clip info
    m_clipInfoLabel = new QLabel(tr("No clip selected"), this);
    m_clipInfoLabel->setStyleSheet("color:#999; font-size:10px;");
    mainLayout->addWidget(m_clipInfoLabel);
    
    // Effects list
    auto* effectsGroup = new QGroupBox(tr("Applied Effects"), this);
    auto* effectsLayout = new QVBoxLayout(effectsGroup);
    m_effectsList = new QListWidget(this);
    m_effectsList->setMaximumHeight(120);
    m_effectsList->setStyleSheet(R"(
        QListWidget { background:#1e1e1e; border:1px solid #333; border-radius:4px; }
        QListWidget::item { color:#ccc; padding:4px; }
        QListWidget::item:selected { background:#2a4a7a; border-radius:4px; }
    )");
    effectsLayout->addWidget(m_effectsList);
    
    auto* removeBtn = new QPushButton(tr("Remove Selected"), this);
    removeBtn->setMaximumHeight(28);
    effectsLayout->addWidget(removeBtn);
    mainLayout->addWidget(effectsGroup);
    
    // Effect selection
    auto* addGroup = new QGroupBox(tr("Add Effect"), this);
    auto* addLayout = new QVBoxLayout(addGroup);
    
    auto* typeLabel = new QLabel(tr("Effect Type:"), this);
    m_effectTypeCombo = new QComboBox(this);
    m_effectTypeCombo->addItem(tr("Brightness"), "brightness");
    m_effectTypeCombo->addItem(tr("Contrast"), "contrast");
    m_effectTypeCombo->addItem(tr("Saturation"), "saturation");
    m_effectTypeCombo->addItem(tr("Opacity"), "opacity");
    m_effectTypeCombo->addItem(tr("Blur"), "blur");
    m_effectTypeCombo->addItem(tr("Grayscale"), "grayscale");
    m_effectTypeCombo->addItem(tr("Invert"), "invert");
    m_effectTypeCombo->setStyleSheet("QComboBox { background:#2a2a2a; color:#ccc; border:1px solid #444; }");
    addLayout->addWidget(typeLabel);
    addLayout->addWidget(m_effectTypeCombo);
    
    // Value slider + spinbox
    auto* valueLabel = new QLabel(tr("Value:"), this);
    auto* valueLayout = new QHBoxLayout;
    
    m_valueSlider = new QSlider(Qt::Horizontal, this);
    m_valueSlider->setMinimum(0);
    m_valueSlider->setMaximum(100);
    m_valueSlider->setValue(50);
    m_valueSlider->setStyleSheet(R"(
        QSlider::groove:horizontal { border:1px solid #444; height:4px; background:#333; }
        QSlider::handle:horizontal { background:#2a6a9a; width:12px; margin:-4px 0; border-radius:6px; }
    )");
    
    m_valueSpinBox = new QDoubleSpinBox(this);
    m_valueSpinBox->setMinimum(-100);
    m_valueSpinBox->setMaximum(100);
    m_valueSpinBox->setValue(0);
    m_valueSpinBox->setDecimals(1);
    m_valueSpinBox->setMaximumWidth(80);
    m_valueSpinBox->setStyleSheet("QDoubleSpinBox { background:#2a2a2a; color:#ccc; border:1px solid #444; }");
    
    valueLayout->addWidget(m_valueSlider, 1);
    valueLayout->addWidget(m_valueSpinBox);
    addLayout->addWidget(valueLabel);
    addLayout->addLayout(valueLayout);
    
    // Preview
    m_previewLabel = new QLabel(tr("Preview: Default"), this);
    m_previewLabel->setStyleSheet("color:#999; font-size:9px;");
    addLayout->addWidget(m_previewLabel);
    
    // Add button
    auto* addBtn = new QPushButton(tr("Add Effect"), this);
    addBtn->setMaximumHeight(28);
    addLayout->addWidget(addBtn);
    mainLayout->addWidget(addGroup);
    
    mainLayout->addStretch();
    
    // Connections
    connect(m_effectTypeCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &EffectsPanel::onEffectTypeChanged);
    connect(m_valueSlider, &QSlider::valueChanged, this, &EffectsPanel::onEffectValueChanged);
    connect(m_valueSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &EffectsPanel::onEffectValueChanged);
    connect(addBtn, &QPushButton::clicked, this, &EffectsPanel::onAddEffect);
    connect(removeBtn, &QPushButton::clicked, this, &EffectsPanel::onRemoveEffect);
}

void EffectsPanel::setSelectedClip(const TimelineClip& clip)
{
    m_selectedClip = clip;
    m_clipSelected = true;
    m_clipInfoLabel->setText(tr("Clip: %1").arg(clip.name));
    refreshEffectsList();
}

void EffectsPanel::clearSelectedClip()
{
    m_clipSelected = false;
    m_clipInfoLabel->setText(tr("No clip selected"));
    m_effectsList->clear();
    m_appliedEffects.clear();
}

void EffectsPanel::onAddEffect()
{
    if (!m_clipSelected) return;
    
    QString effectType = m_effectTypeCombo->currentData().toString();
    double value = m_valueSpinBox->value();
    
    // Create effect with appropriate range
    double minVal = 0, maxVal = 100;
    if (effectType == "opacity") {
        minVal = 0; maxVal = 100;
    } else if (effectType == "blur") {
        minVal = 0; maxVal = 50;
    } else if (effectType == "brightness" || effectType == "contrast" || effectType == "saturation") {
        minVal = -100; maxVal = 100;
    }
    
    Effect newEffect(m_effectTypeCombo->currentText(), effectType, value, minVal, maxVal);
    m_appliedEffects.append(newEffect);
    
    emit effectApplied(m_selectedClip, newEffect);
    refreshEffectsList();
}

void EffectsPanel::onRemoveEffect()
{
    int row = m_effectsList->currentRow();
    if (row < 0 || row >= m_appliedEffects.size()) return;
    
    const Effect& effect = m_appliedEffects[row];
    emit effectRemoved(m_selectedClip, effect.type);
    
    m_appliedEffects.removeAt(row);
    refreshEffectsList();
}

void EffectsPanel::onEffectValueChanged()
{
    // Sync slider and spinbox
    QSignalBlocker blocker1(m_valueSlider);
    QSignalBlocker blocker2(m_valueSpinBox);
    
    double value = m_valueSpinBox->value();
    // Convert spinbox value (-100 to 100) to slider (0 to 100)
    if (value < 0) {
        m_valueSlider->setValue(50 + value/2);
    } else {
        m_valueSlider->setValue(50 + value/2);
    }
    
    updateEffectPreview();
}

void EffectsPanel::onEffectTypeChanged(const QString&)
{
    updateEffectPreview();
    
    // Update slider range based on effect type
    QString type = m_effectTypeCombo->currentData().toString();
    if (type == "opacity") {
        m_valueSpinBox->setMinimum(0);
        m_valueSpinBox->setMaximum(100);
        m_valueSpinBox->setValue(100);
    } else if (type == "blur") {
        m_valueSpinBox->setMinimum(0);
        m_valueSpinBox->setMaximum(50);
        m_valueSpinBox->setValue(0);
    } else if (type == "grayscale" || type == "invert") {
        m_valueSlider->setEnabled(false);
        m_valueSpinBox->setEnabled(false);
    } else {
        m_valueSlider->setEnabled(true);
        m_valueSpinBox->setEnabled(true);
        m_valueSpinBox->setMinimum(-100);
        m_valueSpinBox->setMaximum(100);
        m_valueSpinBox->setValue(0);
    }
}

void EffectsPanel::updateEffectPreview()
{
    QString type = m_effectTypeCombo->currentData().toString();
    double value = m_valueSpinBox->value();
    
    QString preview;
    if (type == "brightness") {
        preview = tr("Brightness: %1%").arg(static_cast<int>(value));
    } else if (type == "contrast") {
        preview = tr("Contrast: %1%").arg(static_cast<int>(value));
    } else if (type == "saturation") {
        preview = tr("Saturation: %1%").arg(static_cast<int>(value));
    } else if (type == "opacity") {
        preview = tr("Opacity: %1%").arg(static_cast<int>(value));
    } else if (type == "blur") {
        preview = tr("Blur: %1 px").arg(static_cast<int>(value));
    } else if (type == "grayscale") {
        preview = tr("Grayscale (B&W)");
    } else if (type == "invert") {
        preview = tr("Invert Colors");
    }
    
    m_previewLabel->setText(tr("Preview: %1").arg(preview));
}

void EffectsPanel::refreshEffectsList()
{
    m_effectsList->clear();
    for (const auto& effect : m_appliedEffects) {
        QString text = tr("%1: %2").arg(effect.name, QString::number(effect.value, 'f', 1));
        m_effectsList->addItem(text);
    }
}
