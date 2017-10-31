#include"viewportboxx.h"
#include <QtWidgets>

MyItem::MyItem()
{
    setFlags(ItemIsFocusable|ItemIsMovable);
    boundingRect();
    rect.setRect(-18,-18,36,36);
}
QRectF MyItem::boundingRect() const
{
    return QRectF(rect);
}

void MyItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    prepareGeometryChange();
    rect.moveCenter(event->pos()-offset);
    update();
}
void MyItem::paint(QPainter *painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
    painter->drawRect(rect);
}
void MyItem::keyPressEvent(QKeyEvent* /*event*/)
{
//    switch (event->key())
//    {
//    case Qt::Key_4 :
//        rotate(-30);
//        break;
//    case Qt::Key_6 :
//        rotate(30);
//        break;
//    case Qt::Key_Up:
//        scale(1.2,1.2);
//        break;
//    case Qt::Key_Down:
//        scale(1/1.2,1/1.2);
//        break;
//    }
}
void MyItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setFocus();
    offset=event->pos()-rect.center();
}

