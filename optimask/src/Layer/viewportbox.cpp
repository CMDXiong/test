#include"viewportbox.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
NavigatorViewBox::NavigatorViewBox(const QRect &rect, QGraphicsScene *scene)
    : m_resizing(false), m_angle(0.0), m_shearHorizontal(0.0), m_shearVertical(0.0),m_zoomDelta(0.1)
{
    setFlags(QGraphicsItem::ItemIsSelectable|
             QGraphicsItem::ItemSendsGeometryChanges|
             QGraphicsItem::ItemIsMovable|
             QGraphicsItem::ItemIsFocusable);
    setPos(rect.center());
    setRect(QRectF(QPointF(-rect.width() / 2.0, -rect.height() / 2.0), rect.size()));
    scene->clearSelection();
    scene->addItem(this);
    setSelected(true);
    setFocus();

}

void NavigatorViewBox::setAngle(double angle)
{
    if (isSelected() && !qFuzzyCompare(m_angle, angle)) {
        m_angle = angle;
        updateTransform();
    }
}

void NavigatorViewBox::setShear(double shearHorizontal, double shearVertical)
{
    if (isSelected() &&
        (!qFuzzyCompare(m_shearHorizontal, shearHorizontal) ||
         !qFuzzyCompare(m_shearVertical, shearVertical))) {
        m_shearHorizontal = shearHorizontal;
        m_shearVertical = shearVertical;
        updateTransform();
    }
}

void NavigatorViewBox::updateTransform()
{
    QTransform transform;
    transform.shear(m_shearHorizontal, m_shearVertical);
    transform.rotate(m_angle);
    setTransform(transform);
}

QVariant NavigatorViewBox::itemChange(GraphicsItemChange change,
                             const QVariant &value)
{
    return QGraphicsRectItem::itemChange(change, value);
}

void NavigatorViewBox::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier ||
        event->modifiers() & Qt::ControlModifier) {
        bool move = event->modifiers() & Qt::ControlModifier;
        bool changed = true;
        double dx1 = 0.0;
        double dy1 = 0.0;
        double dx2 = 0.0;
        double dy2 = 0.0;
        switch (event->key()) {
            case Qt::Key_Left:
                if (move)
                    dx1 = -1.0;
                dx2 = -1.0;
                break;
            case Qt::Key_Right:
                if (move)
                    dx1 = 1.0;
                dx2 = 1.0;
                break;
            case Qt::Key_Up:
                if (move)
                    dy1 = -1.0;
                dy2 = -1.0;
                break;
            case Qt::Key_Down:
                if (move)
                    dy1 = 1.0;
                dy2 = 1.0;
                break;
            default:
                changed = false;
        }
        if (changed) {
            setRect(rect().adjusted(dx1, dy1, dx2, dy2));
            event->accept();
            //emit dirty();
            return;
        }
    }
    QGraphicsRectItem::keyPressEvent(event);
}

void NavigatorViewBox::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier) {
        m_resizing = true;//改变大小标志
        setCursor(Qt::SizeAllCursor);//设置鼠标箭头方向
    }
    else
        QGraphicsRectItem::mousePressEvent(event);
}
void NavigatorViewBox::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_resizing) {
#ifdef ALTERNATIVE_RESIZING
        qreal dx = event->pos().x() - event->lastPos().x();
        qreal dy = event->pos().y() - event->lastPos().y();//鼠标的偏移量
        setRect(rect().adjusted(0, 0, dx, dy).normalized());
        //QRect QRect::adjusted ( int dx1, int dy1, int dx2, int dy2 ) const
       // 返回一个新的矩形用dx1，dy1,dx2,dy2
#else
        QRectF rectangle(rect());
        if (event->pos().x() < rectangle.x())
            rectangle.setBottomLeft(event->pos());
        else
            rectangle.setBottomRight(event->pos());
        setRect(rectangle);
#endif
        scene()->update();
    }
    else
        QGraphicsRectItem::mouseMoveEvent(event);
}

void NavigatorViewBox::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_resizing) {
        m_resizing = false;
        setCursor(Qt::ArrowCursor);
    }
    else
        QGraphicsRectItem::mouseReleaseEvent(event);
}
void NavigatorViewBox::wheelEvent(QGraphicsSceneWheelEvent *event)
{

    qreal scrollAmount=event->delta();
     scrollAmount> 0 ? zoomOut() : zoomIn();

    QGraphicsRectItem::wheelEvent(event);
}

void NavigatorViewBox::zoomIn()
{
    zoom(1 + m_zoomDelta);//m_zoomDelta初始值为0.1,每次放大1.1
}
 void NavigatorViewBox::zoomOut()
 {
     zoom(1 - m_zoomDelta);//每次缩小0.9
 }
 void NavigatorViewBox::zoom(float scaleFactor)
 {
     QRectF rectangle(rect());
     qreal width=rectangle.width()*scaleFactor;
     qreal height=rectangle.height()*scaleFactor;
     setRect(QRectF(-width/2,-height/2,width,height));
     scene()->update();
     float reverseFactor;
     reverseFactor=(float)1/scaleFactor;
     emit currentViewBoxZoom(reverseFactor);
 }
