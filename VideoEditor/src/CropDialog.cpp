#include "CropDialog.h"
#include <QPainter>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDialogButtonBox>

// ═══════════════════════════════════════════════════════════════════════════════
//  CropWidget
// ═══════════════════════════════════════════════════════════════════════════════

CropWidget::CropWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(360, 220);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
}

void CropWidget::setPreviewPixmap(const QPixmap& px)
{
    m_pixmap = px;
    update();
}

void CropWidget::setCropRect(const QRectF& normalised)
{
    m_crop = normalised;
    update();
}

QRect CropWidget::previewRect() const
{
    if (m_pixmap.isNull()) return rect().adjusted(10, 10, -10, -10);
    QSize pxSz  = m_pixmap.size().scaled(size() - QSize(20, 20),
                                          Qt::KeepAspectRatio);
    int   ox    = (width()  - pxSz.width())  / 2;
    int   oy    = (height() - pxSz.height()) / 2;
    return QRect(QPoint(ox, oy), pxSz);
}

QRect CropWidget::fromNormal(const QRectF& nf) const
{
    QRect pr = previewRect();
    return QRect(
        int(pr.x() + nf.x()      * pr.width()),
        int(pr.y() + nf.y()      * pr.height()),
        int(nf.width()            * pr.width()),
        int(nf.height()           * pr.height()));
}

QRectF CropWidget::toNormal(const QRect& r) const
{
    QRect pr = previewRect();
    if (pr.width() == 0 || pr.height() == 0) return QRectF(0,0,1,1);
    return QRectF(
        double(r.x() - pr.x()) / pr.width(),
        double(r.y() - pr.y()) / pr.height(),
        double(r.width())       / pr.width(),
        double(r.height())      / pr.height());
}

QRect CropWidget::handleRect(Handle h) const
{
    QRect cr = fromNormal(m_crop);
    int s = kHandleSize;
    int hs = s / 2;
    switch (h) {
    case TopLeft:   return QRect(cr.left()-hs,  cr.top()-hs,   s, s);
    case TopRight:  return QRect(cr.right()-hs, cr.top()-hs,   s, s);
    case BotLeft:   return QRect(cr.left()-hs,  cr.bottom()-hs,s, s);
    case BotRight:  return QRect(cr.right()-hs, cr.bottom()-hs,s, s);
    case Top:       return QRect(cr.center().x()-hs, cr.top()-hs,   s, s);
    case Bottom:    return QRect(cr.center().x()-hs, cr.bottom()-hs,s, s);
    case Left:      return QRect(cr.left()-hs, cr.center().y()-hs,  s, s);
    case Right:     return QRect(cr.right()-hs,cr.center().y()-hs,  s, s);
    default:        return QRect();
    }
}

CropWidget::Handle CropWidget::hitHandle(const QPoint& pos) const
{
    for (Handle h : {TopLeft,TopRight,BotLeft,BotRight,Top,Bottom,Left,Right})
        if (handleRect(h).adjusted(-3,-3,3,3).contains(pos)) return h;
    if (fromNormal(m_crop).contains(pos)) return Center;
    return None;
}

void CropWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Checkerboard background
    p.fillRect(rect(), QColor(40, 40, 45));

    QRect pr = previewRect();
    if (!m_pixmap.isNull())
        p.drawPixmap(pr, m_pixmap.scaled(pr.size(), Qt::IgnoreAspectRatio,
                                          Qt::SmoothTransformation));
    else
        p.fillRect(pr, QColor(60, 60, 70));

    // Dimmed overlay outside crop
    QRect cr = fromNormal(m_crop);
    p.fillRect(QRect(pr.left(), pr.top(), cr.left()-pr.left(), pr.height()),
               QColor(0,0,0,120));
    p.fillRect(QRect(cr.right(), pr.top(), pr.right()-cr.right(), pr.height()),
               QColor(0,0,0,120));
    p.fillRect(QRect(cr.left(), pr.top(), cr.width(), cr.top()-pr.top()),
               QColor(0,0,0,120));
    p.fillRect(QRect(cr.left(), cr.bottom(), cr.width(), pr.bottom()-cr.bottom()),
               QColor(0,0,0,120));

    // Rule-of-thirds grid
    p.setPen(QPen(QColor(255,255,255,50), 1, Qt::DashLine));
    for (int i = 1; i < 3; ++i) {
        int x = cr.left() + cr.width()  * i / 3;
        int y = cr.top()  + cr.height() * i / 3;
        p.drawLine(x, cr.top(), x, cr.bottom());
        p.drawLine(cr.left(), y, cr.right(), y);
    }

    // Crop rectangle border
    p.setPen(QPen(QColor(255, 200, 50), 2));
    p.setBrush(Qt::NoBrush);
    p.drawRect(cr);

    // Handles
    p.setPen(QPen(Qt::white, 1));
    p.setBrush(QColor(255, 200, 50));
    for (Handle h : {TopLeft,TopRight,BotLeft,BotRight,Top,Bottom,Left,Right})
        p.drawRect(handleRect(h));
}

void CropWidget::mousePressEvent(QMouseEvent* e)
{
    m_handle        = hitHandle(e->pos());
    m_dragging      = (m_handle != None);
    m_dragStart     = e->pos();
    m_cropAtDragStart = m_crop;
}

void CropWidget::mouseMoveEvent(QMouseEvent* e)
{
    if (!m_dragging) {
        Handle h = hitHandle(e->pos());
        switch (h) {
        case TopLeft: case BotRight: setCursor(Qt::SizeFDiagCursor); break;
        case TopRight:case BotLeft:  setCursor(Qt::SizeBDiagCursor); break;
        case Top: case Bottom:       setCursor(Qt::SizeVerCursor);   break;
        case Left:case Right:        setCursor(Qt::SizeHorCursor);   break;
        case Center:                 setCursor(Qt::SizeAllCursor);   break;
        default:                     setCursor(Qt::ArrowCursor);     break;
        }
        return;
    }

    QRect  pr   = previewRect();
    if (pr.width() == 0 || pr.height() == 0) return;
    double dx   = double(e->pos().x() - m_dragStart.x()) / pr.width();
    double dy   = double(e->pos().y() - m_dragStart.y()) / pr.height();
    QRectF orig = m_cropAtDragStart;

    auto clamp = [](double v, double lo, double hi){ return qBound(lo, v, hi); };

    QRectF nr = orig;
    switch (m_handle) {
    case TopLeft:  nr.setLeft(clamp(orig.left()+dx,0,orig.right()-0.05));
                   nr.setTop( clamp(orig.top()+dy, 0,orig.bottom()-0.05)); break;
    case TopRight: nr.setRight( clamp(orig.right()+dx, orig.left()+0.05, 1));
                   nr.setTop(   clamp(orig.top()+dy,   0, orig.bottom()-0.05)); break;
    case BotLeft:  nr.setLeft(  clamp(orig.left()+dx,  0, orig.right()-0.05));
                   nr.setBottom(clamp(orig.bottom()+dy, orig.top()+0.05, 1)); break;
    case BotRight: nr.setRight( clamp(orig.right()+dx,  orig.left()+0.05, 1));
                   nr.setBottom(clamp(orig.bottom()+dy, orig.top()+0.05, 1)); break;
    case Top:      nr.setTop(clamp(orig.top()+dy, 0, orig.bottom()-0.05)); break;
    case Bottom:   nr.setBottom(clamp(orig.bottom()+dy, orig.top()+0.05, 1)); break;
    case Left:     nr.setLeft(clamp(orig.left()+dx, 0, orig.right()-0.05)); break;
    case Right:    nr.setRight(clamp(orig.right()+dx, orig.left()+0.05, 1)); break;
    case Center: {
        double nx = clamp(orig.x()+dx, 0, 1 - orig.width());
        double ny = clamp(orig.y()+dy, 0, 1 - orig.height());
        nr.moveTo(nx, ny); break;
    }
    default: break;
    }

    m_crop = nr;
    update();
    emit cropChanged(m_crop);
}

void CropWidget::mouseReleaseEvent(QMouseEvent*)
{
    m_dragging = false;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CropDialog
// ═══════════════════════════════════════════════════════════════════════════════

CropDialog::CropDialog(QWidget* parent, const QPixmap& preview)
    : QDialog(parent)
{
    setWindowTitle(tr("Crop"));
    setMinimumSize(520, 420);
    buildUi(preview);
}

void CropDialog::buildUi(const QPixmap& preview)
{
    auto* root = new QVBoxLayout(this);

    m_cropWidget = new CropWidget(this);
    if (!preview.isNull()) m_cropWidget->setPreviewPixmap(preview);
    connect(m_cropWidget, &CropWidget::cropChanged,
            this,         &CropDialog::onCropWidgetChanged);
    root->addWidget(m_cropWidget, 1);

    // Numeric crop controls
    auto* grp = new QGroupBox(tr("Crop (%)"), this);
    auto* grid = new QGridLayout(grp);

    auto mkSpin = [](QWidget* parent) {
        auto* sp = new QDoubleSpinBox(parent);
        sp->setRange(0, 99);
        sp->setDecimals(1);
        sp->setSuffix("%");
        return sp;
    };

    m_spLeft   = mkSpin(this);
    m_spTop    = mkSpin(this);
    m_spRight  = mkSpin(this);
    m_spBottom = mkSpin(this);

    grid->addWidget(new QLabel("Left",   this), 0, 0);
    grid->addWidget(m_spLeft,                   0, 1);
    grid->addWidget(new QLabel("Top",    this), 0, 2);
    grid->addWidget(m_spTop,                    0, 3);
    grid->addWidget(new QLabel("Right",  this), 1, 0);
    grid->addWidget(m_spRight,                  1, 1);
    grid->addWidget(new QLabel("Bottom", this), 1, 2);
    grid->addWidget(m_spBottom,                 1, 3);

    for (auto* sp : {m_spLeft, m_spTop, m_spRight, m_spBottom})
        connect(sp, &QDoubleSpinBox::valueChanged, this, &CropDialog::onSpinChanged);

    root->addWidget(grp);

    // Buttons
    auto* btnRow = new QHBoxLayout();
    auto* btnReset = new QPushButton(tr("Reset"), this);
    connect(btnReset, &QPushButton::clicked, this, &CropDialog::onResetClicked);
    btnRow->addWidget(btnReset);
    btnRow->addStretch();

    auto* bbox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(bbox, &QDialogButtonBox::accepted, this, &CropDialog::onApplyClicked);
    connect(bbox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    btnRow->addWidget(bbox);
    root->addLayout(btnRow);
}

void CropDialog::onCropWidgetChanged(QRectF r)
{
    m_updating = true;
    syncSpinsFromRect(r);
    m_updating = false;
}

void CropDialog::onSpinChanged()
{
    if (m_updating) return;
    syncRectFromSpins();
}

void CropDialog::syncSpinsFromRect(const QRectF& r)
{
    m_spLeft->setValue(r.left() * 100.0);
    m_spTop->setValue(r.top() * 100.0);
    m_spRight->setValue((1.0 - r.right()) * 100.0);
    m_spBottom->setValue((1.0 - r.bottom()) * 100.0);
}

void CropDialog::syncRectFromSpins()
{
    double l = m_spLeft->value()   / 100.0;
    double t = m_spTop->value()    / 100.0;
    double r = 1.0 - m_spRight->value()  / 100.0;
    double b = 1.0 - m_spBottom->value() / 100.0;
    if (r > l + 0.05 && b > t + 0.05) {
        m_cropWidget->setCropRect(QRectF(l, t, r-l, b-t));
    }
}

void CropDialog::onResetClicked()
{
    m_cropWidget->setCropRect(QRectF(0, 0, 1, 1));
    syncSpinsFromRect(QRectF(0, 0, 1, 1));
}

void CropDialog::onApplyClicked()
{
    QRectF r = m_cropWidget->cropRect();
    m_result.left     = r.left();
    m_result.top      = r.top();
    m_result.right    = 1.0 - r.right();
    m_result.bottom   = 1.0 - r.bottom();
    m_result.accepted = true;
    accept();
}
