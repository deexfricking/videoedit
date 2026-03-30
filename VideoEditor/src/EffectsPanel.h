#pragma once
#include <QWidget>
#include <QListWidget>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QSlider>
#include "TimelineClip.h"

struct Effect {
    QString name;
    QString type;  // "brightness", "contrast", "saturation", "blur", "opacity", etc.
    double value;
    double minVal, maxVal;
    
    Effect(const QString& n, const QString& t, double v, double mn, double mx)
        : name(n), type(t), value(v), minVal(mn), maxVal(mx) {}
};

class EffectsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit EffectsPanel(QWidget* parent = nullptr);
    
    void setSelectedClip(const TimelineClip& clip);
    void clearSelectedClip();
    
signals:
    void effectApplied(const TimelineClip& clip, const Effect& effect);
    void effectRemoved(const TimelineClip& clip, const QString& effectType);

private slots:
    void onAddEffect();
    void onRemoveEffect();
    void onEffectValueChanged();
    void onEffectTypeChanged(const QString& type);
    void updateEffectPreview();

private:
    void buildUI();
    void refreshEffectsList();
    
    TimelineClip m_selectedClip;
    bool m_clipSelected = false;
    
    // UI elements
    QListWidget* m_effectsList;
    QComboBox* m_effectTypeCombo;
    QSlider* m_valueSlider;
    QDoubleSpinBox* m_valueSpinBox;
    QLabel* m_previewLabel;
    QLabel* m_clipInfoLabel;
    
    // Available effects
    QList<Effect> m_appliedEffects;
};
