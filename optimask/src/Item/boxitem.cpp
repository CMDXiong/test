/*
    Copyright (c) 2009-10 Qtrac Ltd. All rights reserved.

    This program or module is free software: you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version. It is provided
    for educational purposes and is distributed in the hope that it will
    be useful, but WITHOUT ANY WARRANTY; without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
    the GNU General Public License for more details.
*/

#include "./Item/alt_key.hpp"
#include "./Item/aqp.hpp"
#include "./Item/boxitem.hpp"
#include "./Item/global.hpp"
#include "./Item/brushwidget.hpp"
#include "./Item/penwidget.hpp"
#include "./Alter/transformwidget.hpp"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QAction>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

BoxItem::BoxItem(const QRect &rect_, QGraphicsScene *scene)
    : QObject(), QGraphicsRectItem(), m_resizing(false),
      m_angle(0.0), m_shearHorizontal(0.0), m_shearVertical(0.0),m_showPenWidget(false),
      m_showBrushWidget(false),m_showTransfromWidget(false)
{
    setFlags(QGraphicsItem::ItemIsSelectable|
#if QT_VERSION >= 0x040600
             QGraphicsItem::ItemSendsGeometryChanges|
#endif
             QGraphicsItem::ItemIsMovable|
             QGraphicsItem::ItemIsFocusable);
    setPos(rect_.center());
    setRect(QRectF(QPointF(-rect_.width() / 2.0,
                           -rect_.height() / 2.0), rect_.size()));
    scene->clearSelection();
    scene->addItem(this);
    setSelected(true);
    setFocus();

 ////////////////////////////////
    //setZValue(0);
    //setAcceptDrops(true);
    //setFlags(ItemIsSelectable);
    setAcceptHoverEvents(true);
    cScale=false;
    m_cursor=new QCursor;
    direction=0;
}


void BoxItem::setPen(const QPen &pen_)
{
    if (isSelected() && pen_ != pen()) {
        QGraphicsRectItem::setPen(pen_);
        emit dirty();
    }
}


void BoxItem::setBrush(const QBrush &brush_)
{
    if (isSelected() && brush_ != brush()) {
        QGraphicsRectItem::setBrush(brush_);
        emit dirty();
    }
}


void BoxItem::setAngle(double angle)
{
    if (isSelected() && !qFuzzyCompare(m_angle, angle)) {
        m_angle = angle;
        updateTransform();
    }
}


void BoxItem::setShear(double shearHorizontal, double shearVertical)
{
    if (isSelected() &&
        (!qFuzzyCompare(m_shearHorizontal, shearHorizontal) ||
         !qFuzzyCompare(m_shearVertical, shearVertical))) {
        m_shearHorizontal = shearHorizontal;
        m_shearVertical = shearVertical;
        updateTransform();
    }
}

void BoxItem::setShowPenWidget(bool /*on*/)
{
    PenWidget *penWidget = new PenWidget;
    penWidget->show();
}

void BoxItem::setShowBrushWidget(bool /*on*/)
{
    BrushWidget *brushWidget = new BrushWidget;
    brushWidget->show();
}

void BoxItem::setshowTransfromWidget(bool /*on*/)
{
    TransformWidget *transfrom = new TransformWidget;
    transfrom->show();
}

void BoxItem::edit()
{
//    QMenu menu;
//    QAction *showHatAction = createMenuAction(&menu, QIcon(),
//            tr("Show Hat"), m_showHat);
//    connect(showHatAction, SIGNAL(triggered(bool)),
//            this, SLOT(setShowHat(bool)));
//    menu.addSeparator();
//    QActionGroup *group = new QActionGroup(this);
//    createMenuAction(&menu, QIcon(":/smileysmile.png"),
//            tr("Happy"), m_face == Happy, group, Happy);
//    createMenuAction(&menu, QIcon(":/smileyneutral.png"),
//            tr("Neutral"), m_face == Neutral, group, Neutral);
//    createMenuAction(&menu, QIcon(":/smileysad.png"),
//            tr("Sad"), m_face == Sad, group, Sad);
//    AQP::accelerateMenu(&menu);
//    QAction *chosen = menu.exec(QCursor::pos());
//    if (chosen && chosen != showHatAction)
//        setFace(static_cast<Face>(chosen->data().toInt()));
    QMenu menu;
    QAction *showPenAction       = createMenuAction(&menu, QIcon(), tr("Show Pen"), m_showPenWidget);
    QAction *showBrushAction     = createMenuAction(&menu, QIcon(), tr("Show Brush"), m_showBrushWidget);
    QAction *showTransfromAction = createMenuAction(&menu, QIcon(), tr("Show Transfrom"), m_showTransfromWidget);

    connect(showPenAction,       SIGNAL(triggered(bool)),this, SLOT(setShowPenWidget(bool)));
    connect(showBrushAction,     SIGNAL(triggered(bool)),this, SLOT(setShowBrushWidget(bool)));
    connect(showTransfromAction, SIGNAL(triggered(bool)),this, SLOT(setshowTransfromWidget(bool)));

    menu.addSeparator();
    AQP::accelerateMenu(&menu);
    QAction *chosen = menu.exec(QCursor::pos());
//    if (chosen && chosen != showHatAction)
//        setFace(static_cast<Face>(chosen->data().toInt()));
    if(chosen){;}//暂时还没用到chosen
}


QAction *BoxItem::createMenuAction(QMenu *menu, const QIcon &icon,
                                   const QString &text, bool checked, QActionGroup *group,
                                   const QVariant &data)
{
    QAction *action = menu->addAction(icon, text);
    action->setCheckable(true);
    action->setChecked(checked);
    if (group)
        group->addAction(action);
    action->setData(data);
    return action;
}

void BoxItem::updateTransform()
{
    QTransform transform;
    transform.shear(m_shearHorizontal, m_shearVertical);
    transform.rotate(m_angle);
    setTransform(transform);
}




QVariant BoxItem::itemChange(GraphicsItemChange change,
                             const QVariant &value)
{
    if (isDirtyChange(change))
        emit dirty();
    return QGraphicsRectItem::itemChange(change, value);
}


void BoxItem::keyPressEvent(QKeyEvent *event)
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
            emit dirty();
            return;
        }
    }
    QGraphicsRectItem::keyPressEvent(event);
}


void BoxItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier) {
        m_resizing = true;
        setCursor(Qt::SizeAllCursor);
    }else if(event->button()==Qt::LeftButton){
        start=event->scenePos();
        cScale=true;
    }
    update();
    QGraphicsRectItem::mousePressEvent(event);
}


void BoxItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_resizing) {
#ifdef ALTERNATIVE_RESIZING
        qreal dx = event->pos().x() - event->lastPos().x();
        qreal dy = event->pos().y() - event->lastPos().y();
        setRect(rect().adjusted(0, 0, dx, dy).normalized());
#else
        QRectF rectangle(rect());
        if (event->pos().x() < rectangle.x())
            rectangle.setBottomLeft(event->pos());
        //rectangle.setBottom(event->pos().y());
        else
            rectangle.setBottomRight(event->pos());
        setRect(rectangle);
#endif
        scene()->update();
    }else  if(cScale && (cursor().shape() == Qt::SizeFDiagCursor||
                         cursor().shape() == Qt::SizeBDiagCursor  ||
                         cursor().shape() == Qt::SizeHorCursor||
                         cursor().shape() == Qt::SizeVerCursor)){
        setFlags(QGraphicsItem::ItemIsSelectable|
    #if QT_VERSION >= 0x040600
                 QGraphicsItem::ItemSendsGeometryChanges|
    #endif
//                 QGraphicsItem::ItemIsMovable|
                 QGraphicsItem::ItemIsFocusable);
        {
        QPointF dis;
        end=event->scenePos();
        dis=end-start;
        start=end;
        switch(direction){
            case 1:
            {
                QRectF tem=QRectF(rect().x(),rect().y()+dis.y()/2,rect().width(),rect().height()-dis.y());
                this->ResetRect(tem);
                update(boundingRect());
                this->moveBy(0,dis.y()/2);
                break;
            }
            case 2:
            {
                QRectF tem=QRectF(rect().x()+dis.x()/2,rect().y()+dis.y()/2,
                                  rect().width()+dis.x(),rect().height()-dis.y());
                this->ResetRect(tem);
                update(boundingRect());
                this->moveBy(-dis.x()/2,-dis.y()/2);
                break;
            }
            case 3:
            {
                QRectF tem=QRectF(rect().x()+dis.x()/2,rect().y(),rect().width()+dis.x(),rect().height());
                this->ResetRect(tem);
                update(boundingRect());
                this->moveBy(-dis.x()/2,0);
                break;
            }
            case 4:
            {
                QRectF tem=QRectF(rect().x()+dis.x()/2,rect().y()+dis.y()/2,
                                  rect().width()+dis.x(),rect().height()+dis.y());
                this->ResetRect(tem);
                update(boundingRect());
                this->moveBy(-dis.x()/2,-dis.y()/2);
                break;
            }
            case 5:
            {
                QRectF tem=QRectF(rect().x(),rect().y()+dis.y()/2,rect().width(),rect().height()+dis.y());
                this->ResetRect(tem);
                update(boundingRect());
                this->moveBy(0,-dis.y()/2);
                break;
            }
            case 6:
            {
                QRectF tem=QRectF(rect().x()+dis.x()/2,rect().y()+dis.y()/2,
                                  rect().width()-dis.x(),rect().height()+dis.y());
                this->ResetRect(tem);
                update(boundingRect());
                this->moveBy(-dis.x()/2,-dis.y()/2);
                break;
            }
            case 7:
            {
                QRectF tem=QRectF(rect().x()+dis.x()/2,rect().y(),rect().width()-dis.x(),rect().height());
                this->ResetRect(tem);
                update(boundingRect());
                this->moveBy(dis.x()/2,0);
                break;
            }
            case 8:
            {
                QRectF tem=QRectF(rect().x()+dis.x()/2,rect().y()+dis.y()/2,
                                  rect().width()-dis.x(),rect().height()-dis.y());
                this->ResetRect(tem);
                update(boundingRect());
                this->moveBy(dis.x()/2,dis.y()/2);
                break;
            }
        }
    }
    update();}else{
        QGraphicsRectItem::mouseMoveEvent(event);
    }
    setFlags(QGraphicsItem::ItemIsSelectable|
#if QT_VERSION >= 0x040600
             QGraphicsItem::ItemSendsGeometryChanges|
#endif
             QGraphicsItem::ItemIsMovable|
             QGraphicsItem::ItemIsFocusable);
}


void BoxItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_resizing) {
        m_resizing = false;
        setCursor(Qt::ArrowCursor);
        emit dirty();
    }
    cScale=false;
    update();
    QGraphicsRectItem::mouseReleaseEvent(event);
}

void BoxItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
    event->setAccepted(false);
}


QDataStream &operator<<(QDataStream &out, const BoxItem &boxItem)
{
    out << boxItem.pos() << boxItem.angle()
        << boxItem.shearHorizontal() << boxItem.shearVertical()
        << boxItem.zValue() << boxItem.rect() << boxItem.pen()
        << boxItem.brush();
    return out;
}


QDataStream &operator>>(QDataStream &in, BoxItem &boxItem)
{
    QPointF position;
    double angle;
    double shearHorizontal;
    double shearVertical;
    double z;
    QRectF rect;
    QPen pen;
    QBrush brush;
    in >> position >> angle >> shearHorizontal >> shearVertical >> z
       >> rect >> pen >> brush;
    boxItem.setPos(position);
    boxItem.setAngle(angle);
    boxItem.setShear(shearHorizontal, shearVertical);
    boxItem.setZValue(z);
    boxItem.setRect(rect);
    boxItem.setPen(pen);
    boxItem.setBrush(brush);
    return in;
}




////////////////////////////////////////////
void BoxItem::ResetRect(QRectF rect){
   // m_rect=rect;
    setRect(rect);
    update(boundingRect());
}

//QRectF BoxItem::boundingRect()const{
//    //return QRectF(rect().x()-2,rect().y()-2,rect().width()+4,rect().height()+4);
//    return QGraphicsRectItem::boundingRect();
//}

//QPainterPath BoxItem::shape()const{
//    QPainterPath path;
//    path.addRect(rect());
//    return path;
//}

//void BoxItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
//    //Q_UNUSED(option);
//    Q_UNUSED(widget);
//    if(option->state & QStyle::State_Selected){
//            painter->setPen(QPen(Qt::red,2,Qt::DotLine));
//            //painter->drawRect(shapeRectF.x(),shapeRectF.y(),
//            //shapeRectF.width(),shapeRectF.height());
//            painter->drawRect(rect());
//        }else{
//        painter->setPen(QPen(Qt::red,2,Qt::SolidLine));
//        painter->drawRect(rect());
//    }

//    setSelected(true);
//}

void BoxItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event){
    QPointF pos1=event->scenePos();//光标在scene中的位置
    QPointF lt=this->scenePos()+QPointF(rect().x(),rect().y());//左上顶点
    QPointF lb=this->scenePos()+QPointF(rect().x(),rect().y()+rect().height());//左下顶点
    QPointF rt=this->scenePos()+QPointF(rect().x()+rect().width(),rect().y());//右上顶点
    QPointF rb=this->scenePos()+QPointF(rect().x()+rect().width(),rect().y()+rect().height());//右下顶点
    if((pos1.x()<=lt.x()+4&&pos1.y()<=lt.y()+4)
     ||(pos1.x()>=rb.x()-4&&pos1.y()>=rb.y()-4)){
        m_cursor->setShape(Qt::SizeFDiagCursor);
        if(pos1.x()<=lt.x()+4)direction=8;
        else direction=4;
    }else if((pos1.x()<=lb.x()+4&&pos1.y()>=lb.y()-4)
             ||(pos1.x()>=rt.x()-4&&pos1.y()<=rt.y()+4)){
        m_cursor->setShape(Qt::SizeBDiagCursor);
        if(pos1.x()<=lb.x()+4)direction=6;
        else direction=2;
    }else if((pos1.x()<=lt.x()+4||pos1.x()>=rt.x()-4)
             &&(pos1.y()<=lb.y()&&pos1.y()>=lt.y())){
        m_cursor->setShape(Qt::SizeHorCursor);
        if(pos1.x()<=lt.x()+4)direction=7;
        else direction=3;
    }else if((pos1.y()<=lt.y()+4||pos1.y()>=lb.y()-4)
             &&(pos1.x()>=lt.x()&&pos1.x()<=rt.x())){
        m_cursor->setShape(Qt::SizeVerCursor);
        if(pos1.y()<=lt.y()+4)direction=1;
        else direction=5;
    }else{
        cScale=false;
        m_cursor->setShape(Qt::ArrowCursor);
    }
    this->setCursor(*m_cursor);
    update();
    QGraphicsItem::hoverEnterEvent(event);
}

void BoxItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event){
    QPointF pos1=event->scenePos();
    QPointF lt=this->scenePos()+QPointF(rect().x(),rect().y());
    QPointF lb=this->scenePos()+QPointF(rect().x(),rect().y()+rect().height());
    QPointF rt=this->scenePos()+QPointF(rect().x()+rect().width(),rect().y());
    QPointF rb=this->scenePos()+QPointF(rect().x()+rect().width(),rect().y()+rect().height());
    if((pos1.x()<=lt.x()+4&&pos1.y()<=lt.y()+4)
     ||(pos1.x()>=rb.x()-4&&pos1.y()>=rb.y()-4)){
        m_cursor->setShape(Qt::SizeFDiagCursor);
        if(pos1.x()<=lt.x()+4)direction=8;
        else direction=4;
    }else if((pos1.x()<=lb.x()+4&&pos1.y()>=lb.y()-4)
             ||(pos1.x()>=rt.x()-4&&pos1.y()<=rt.y()+4)){
        m_cursor->setShape(Qt::SizeBDiagCursor);
        if(pos1.x()<=lb.x()+4)direction=6;
        else direction=2;
    }else if((pos1.x()<=lt.x()+4||pos1.x()>=rt.x()-4)
             &&(pos1.y()<=lb.y()&&pos1.y()>=lt.y())){
        m_cursor->setShape(Qt::SizeHorCursor);
        if(pos1.x()<=lt.x()+4)direction=7;
        else direction=3;
    }else if((pos1.y()<=lt.y()+4||pos1.y()>=lb.y()-4)
             &&(pos1.x()>=lt.x()&&pos1.x()<=rt.x())){
        m_cursor->setShape(Qt::SizeVerCursor);
        if(pos1.y()<=lt.y()+4)direction=1;
        else direction=5;
    }else{
        cScale=false;
        m_cursor->setShape(Qt::ArrowCursor);
    }
    this->setCursor(*m_cursor);
    update();
    QGraphicsItem::hoverMoveEvent(event);
}

void BoxItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event){
    m_cursor->setShape(Qt::ArrowCursor);
    cScale=false;
    this->setCursor(*m_cursor);
    update();
    QGraphicsItem::hoverLeaveEvent(event);
}

//void BoxItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
//    if(event->button()==Qt::LeftButton){
//        start=event->scenePos();
//        cScale=true;
//    }
//    update();
//    QGraphicsItem::mousePressEvent(event);
//}

//void BoxItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
////    int t=0;
////    t++;
////    t--;
//    if(cScale){
//        QPointF dis;
//        end=event->scenePos();
//        dis=end-start;
//        start=end;
//        switch(direction){
//            case 1:
//            {
//                QRectF tem=QRectF(m_rect.x(),m_rect.y()+dis.y()/2,m_rect.width(),m_rect.height()-dis.y());
//                this->ResetRect(tem);
//                update(boundingRect());
//                this->moveBy(0,dis.y()/2);
//                break;
//            }
//            case 2:
//            {
//                QRectF tem=QRectF(m_rect.x()+dis.x()/2,m_rect.y()+dis.y()/2,
//                                  m_rect.width()+dis.x(),m_rect.height()-dis.y());
//                this->ResetRect(tem);
//                update(boundingRect());
//                this->moveBy(-dis.x()/2,-dis.y()/2);
//                break;
//            }
//            case 3:
//            {
//                QRectF tem=QRectF(m_rect.x()+dis.x()/2,m_rect.y(),m_rect.width()+dis.x(),m_rect.height());
//                this->ResetRect(tem);
//                update(boundingRect());
//                this->moveBy(-dis.x()/2,0);
//                break;
//            }
//            case 4:
//            {
//                QRectF tem=QRectF(m_rect.x()+dis.x()/2,m_rect.y()+dis.y()/2,
//                                  m_rect.width()+dis.x(),m_rect.height()+dis.y());
//                this->ResetRect(tem);
//                update(boundingRect());
//                this->moveBy(-dis.x()/2,-dis.y()/2);
//                break;
//            }
//            case 5:
//            {
//                QRectF tem=QRectF(m_rect.x(),m_rect.y()+dis.y()/2,m_rect.width(),m_rect.height()+dis.y());
//                this->ResetRect(tem);
//                update(boundingRect());
//                this->moveBy(0,-dis.y()/2);
//                break;
//            }
//            case 6:
//            {
//                QRectF tem=QRectF(m_rect.x()+dis.x()/2,m_rect.y()+dis.y()/2,
//                                  m_rect.width()-dis.x(),m_rect.height()+dis.y());
//                this->ResetRect(tem);
//                update(boundingRect());
//                this->moveBy(-dis.x()/2,-dis.y()/2);
//                break;
//            }
//            case 7:
//            {
//                QRectF tem=QRectF(m_rect.x()+dis.x()/2,m_rect.y(),m_rect.width()-dis.x(),m_rect.height());
//                this->ResetRect(tem);
//                update(boundingRect());
//                this->moveBy(dis.x()/2,0);
//                break;
//            }
//            case 8:
//            {
//                QRectF tem=QRectF(m_rect.x()+dis.x()/2,m_rect.y()+dis.y()/2,
//                                  m_rect.width()-dis.x(),m_rect.height()-dis.y());
//                this->ResetRect(tem);
//                update(boundingRect());
//                this->moveBy(dis.x()/2,dis.y()/2);
//                break;
//            }
//        }
//    }
//    update();
//    QGraphicsItem::mouseMoveEvent(event);
//}

//void BoxItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
//    cScale=false;
//    update();
//    QGraphicsItem::mouseReleaseEvent(event);
//}
