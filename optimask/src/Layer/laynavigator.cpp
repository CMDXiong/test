#include "laynavigator.h"
#include<QDebug>
Navigator::Navigator()
    :m_bMouseTranslate(false)
{
    paintscene=new QGraphicsScene();
    this->setScene(paintscene);
    this->setSceneRect(-40,-40,80,80);
   // setSceneRect(INT_MIN/2, INT_MIN/2, INT_MAX, INT_MAX);//显示的范围
    //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//去掉水平的滚动条
   // setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//去掉垂直滚动条
    createViewBox();
    createRect();
    connect(viewBox,SIGNAL(currentViewBoxZoom(float)),this,SLOT(viewBoxZoom(float)));
    //this->resize(80,80);
    //setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}
void Navigator::SetGdsDocument(CGdsDocument* doc)
{
    m_gdsdoc=doc;
}
CGdsDocument* Navigator::GetGdsDocument()const
{
    return m_gdsdoc;
}
void Navigator::createViewBox()
{
    viewBox = new NavigatorViewBox(QRect(-31,-31, 62, 62), paintscene);
    viewBox->setPos(QPoint(0,0));
}
void Navigator::createRect()
{
    QGraphicsRectItem *item = new QGraphicsRectItem(QRectF(-30,-30,60, 60));
    QPen pen;
    pen.setWidth(1);
    item->setPen(pen);
    item->setPos(0,0);
    paintscene->addItem(item);

}
void Navigator::viewBoxZoom(const float &scaleFactor)
{
    emit viewBoxchanged(scaleFactor);
}
void Navigator::setBoxzoom(const float &scaleFactor)
{
    viewBox->zoom(scaleFactor);
}
//-----------------------平移--------------------------------
void Navigator::mousePressEvent(QMouseEvent *event)
{
    QPointF point=mapToScene(event->pos());
    if(viewBox->contains(point))
    {
        startPoint=event->pos();
        m_bMouseTranslate=true;
    }
    QGraphicsView::mousePressEvent(event);
}
void Navigator::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bMouseTranslate){
       // QPointF mouseDelta = mapToScene(event->pos()) - mapToScene(startPoint);
        QPointF mouseDelta = mapToScene(startPoint)- mapToScene(event->pos());
        emit tranSlate(mouseDelta);
    }
    startPoint = event->pos();

    QGraphicsView::mouseMoveEvent(event);
}
void Navigator::mouseReleaseEvent(QMouseEvent *event)
{
    m_bMouseTranslate = false;
    QGraphicsView::mouseReleaseEvent(event);
}
//void Navigator::slotTranslate(const QPointF &point)
//{
//    QPointF Point=-point;
//    tranSlate(Point);
//}
