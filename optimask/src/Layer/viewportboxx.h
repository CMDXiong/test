#ifndef VIEWPORTBOXX
#define VIEWPORTBOXX
#include <QGraphicsItem>
class MyItem : public QGraphicsItem
{
public:
    MyItem();
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
private:
    QRectF rect;
    QPointF startPoint;
    QPointF offset;

};

#endif // VIEWPORTBOXX




