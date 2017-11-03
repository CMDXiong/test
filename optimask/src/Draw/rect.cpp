#include "rect.h"

Rect::Rect()
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

void Rect::startDraw(QGraphicsSceneMouseEvent * event)
{
        setRect(QRectF(event->scenePos(), QSizeF(0, 0)));//setRect(qreal x, qreal y, qreal width, qreal height)
        //is equivalent to calling setRect(QRectF(x, y, width, height))
}

void Rect::drawing(QGraphicsSceneMouseEvent * event)
{
        QRectF r(rect().topLeft(),
                 QSizeF(event->scenePos().x() - rect().topLeft().x(), event->scenePos().y() - rect().topLeft().y()));
        //QRectF::​QRectF(const QPointF & topLeft, const QPointF & bottomRight)构建一个矩形，给定左上和右下点
        //	QSizeF(qreal width, qreal height)
       // Constructs a rectangle with the given topLeft and bottomRight corners.
        //QRectF QGraphicsRectItem::​rect() const=Returns the item's rectangle
        setRect(r);
}
