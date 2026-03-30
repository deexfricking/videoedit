#pragma once
#include <QDialog>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QPixmap>
#include <QRect>

// ─── CropWidget ───────────────────────────────────────────────────────────────
// Interactive preview showing the crop rectangle with drag handles.

class CropWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CropWidget(QWidget* parent = nullptr);

    void setPreviewPixmap(const QPixmap& px);
    void setCropRect(const QRectF& normalised); // 0.0-1.0 values
    QRectF cropRect() const { return m_crop; }

signals:
    void cropChanged(QRectF normalised);

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    enum Handle { None, TopLeft, TopRight, BotLeft, BotRight,
                  Top, Bottom, Left, Right, Center };
    Handle hitHandle(const QPoint& pos) const;
    QRect  handleRect(Handle h) const;
    QRect  previewRect() const;
    QRectF  toNormal(const QRect& r) const;
    QRect   fromNormal(const QRectF& nf) const;

    QPixmap m_pixmap;
    QRectF  m_crop      = QRectF(0, 0, 1, 1); // normalised 0-1
    bool    m_dragging  = false;
    Handle  m_handle    = None;
    QPoint  m_dragStart;
    QRectF  m_cropAtDragStart;
    static constexpr int kHandleSize = 10;
};

// ─── CropDialog ───────────────────────────────────────────────────────────────

class CropDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CropDialog(QWidget* parent = nullptr,
                        const QPixmap& preview = QPixmap());

    // Returns normalised crop rect (0-1) for each side
    struct CropResult {
        double left   = 0;
        double top    = 0;
        double right  = 0;   // fraction from the RIGHT (not from left)
        double bottom = 0;   // fraction from the BOTTOM
        bool   accepted = false;
    };
    CropResult result() const { return m_result; }

private slots:
    void onCropWidgetChanged(QRectF r);
    void onSpinChanged();
    void onResetClicked();
    void onApplyClicked();

private:
    void buildUi(const QPixmap& preview);
    void syncSpinsFromRect(const QRectF& r);
    void syncRectFromSpins();

    CropWidget*      m_cropWidget = nullptr;
    QDoubleSpinBox*  m_spLeft     = nullptr;
    QDoubleSpinBox*  m_spTop      = nullptr;
    QDoubleSpinBox*  m_spRight    = nullptr;
    QDoubleSpinBox*  m_spBottom   = nullptr;
    QCheckBox*       m_chkAspect  = nullptr;

    CropResult m_result;
    bool       m_updating = false;
};
