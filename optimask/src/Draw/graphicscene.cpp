#include "graphicscene.h"
#include <QGraphicsObject>

PaintingWidget::PaintingWidget(QWidget *parent):QGraphicsScene(parent),FitSclae(0.9),offsetSceneToView(0.0,0.0)
{
    ODBDrawScale = 1.0;
    //setSceneRect(-200,-200,400,400);//设置scene大小
}

PaintingWidget::~PaintingWidget()
{

}

void PaintingWidget::DrawLineItem(QRect rect)
{
    QGraphicsLineItem *item = new QGraphicsLineItem(QLineF(rect.topLeft(),rect.bottomRight()));
    QPen pen(Qt::red,1,Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin);
    item->setPen(pen);
    item->setFlag(QGraphicsItem::ItemIsMovable);
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    this->addItem(item);
    item->setPos(10,10);
}
void PaintingWidget::slotClear()                   //清除场景中所有的图元
{
    QList<QGraphicsItem*> listItem = items();

    while(!listItem.empty())
    {
        removeItem(listItem.at(0));
        listItem.removeAt(0);
    }
}
void PaintingWidget::DrawEllipseItem(QRect rect)    //在场景中加入一个椭圆形图元
{
    QGraphicsEllipseItem *item = new QGraphicsEllipseItem(rect);
    item->setPen(Qt::SolidLine);
    item->setFlag(QGraphicsItem::ItemIsMovable,true);
    item->setFlag(QGraphicsItem::ItemIsSelectable,true);
    addItem(item);
    item->setPos(20,20);
}
void PaintingWidget::DrawPolygonItem(QRect rect)    //在场景中加入一个多边形图元
{
    QVector<QPoint> v;
    //v<<QPoint(30,-15)<<QPoint(0,-30)<<QPoint(0,30)<<QPoint(30,15)<<QPoint(30,-15);
    v<<rect.topLeft()<<rect.topRight()<<rect.center()<<rect.bottomRight()<<rect.bottomLeft();
    QGraphicsPolygonItem *item = new QGraphicsPolygonItem(QPolygonF(v));
    item->setPen(Qt::SolidLine);
    //item->setBrush(QColor(qrand()%256,qrand()%256,qrand()%256));
    item->setFlag(QGraphicsItem::ItemIsMovable);
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    addItem(item);
    item->setPos(-50,-50);
}
void PaintingWidget::DrawPathItem(QRect rect)
{
    QPainterPath path;//路径
    path.moveTo(0,50);
    path.lineTo(50,0);
    path.lineTo(100,50);
    path.lineTo(150,0);
   // path.setFillRule(fillRule);
    QGraphicsPathItem *item=new QGraphicsPathItem(path);

    QPen pen(Qt::blue,2,Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin);
    item->setPen(pen);
    //item->setBrush(QColor(qrand()%256,qrand()%256,qrand()%256));
    item->setFlag(QGraphicsItem::ItemIsMovable);
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    addItem(item);
    item->setPos(rect.topRight());
}
void PaintingWidget::DrawTextItem(QRect rect)       // 在场景中加入一个文字图元
{
    QFont font("Times",16);
    QGraphicsTextItem *item = new QGraphicsTextItem("Hi, PaintingWidget!");
    item->setFont(font);
    item->setFlag(QGraphicsItem::ItemIsMovable);
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    item->setDefaultTextColor(QColor(qrand()%256,qrand()%256,qrand()%256));
    addItem(item);
    item->setPos(rect.center());
}

void PaintingWidget::DrawRectItem(/*QRect rect*/)       // 在场景中加入一个矩形图元
{
    QGraphicsRectItem *item = new QGraphicsRectItem(0,0,50,50);

    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::red);
    item->setPen(pen);

    item->setFlag(QGraphicsItem::ItemIsMovable);
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    addItem(item);
    item->setPos(0,0);

    QRectF rect1(m_startPot,m_endPot);
    QGraphicsRectItem *item1 = new QGraphicsRectItem(rect1);
    addItem(item1);

}

void PaintingWidget::slotLogoItem(QRect rect)          //在场景中加入一个logo
{
    QGraphicsPixmapItem *item =addPixmap(QPixmap("../Image/logo.png"));
    item->setFlag(QGraphicsItem::ItemIsMovable);
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    item->setPos(rect.center());
}


void PaintingWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button()&Qt::LeftButton){
        m_endPot = event->scenePos();
        //update();
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void PaintingWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
      if (event->button() == Qt::LeftButton){
          m_startPot = event->scenePos();
      }
      QGraphicsScene::mousePressEvent(event);
}

void PaintingWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        m_endPot = event->scenePos();
//        QRectF rect(m_startPot, m_endPot);
//        QGraphicsRectItem* rectItem = new QGraphicsRectItem(rect);
//        addItem(rectItem);
    }
    QGraphicsScene::mouseReleaseEvent(event);
}


QPointF PaintingWidget::getScenePoint()
{
    return   scenePoint;
}

/******************根据.gds文件读取画******************************/
void PaintingWidget::DrawInit(CGDS& read,QGraphicsView *view)
{
    ODBDrawScale  =  read.GetDBUnit()*1000000;     // 单位为微米
    double   FrameSize = read.GetFrameSize()*ODBDrawScale;

    double   maxxvalue = read.GetMaxX();                  // 整个.gds文件图形边框maxX
    double   maxyvalue = read.GetMaxY();                  // 整个.gds文件图形边框maxY
    double   minxvalue = read.GetMinX();                  // 整个.gds文件图形边框minX
    double   minyvalue = read.GetMinY();                  // 整个.gds文件图形边框minY

    QPointF FrameCenter((maxxvalue + minxvalue)/2,(maxyvalue + minyvalue)/2);// 在场景中的中心坐标
    QPoint  FrameCenterOnView = view->mapFromScene(FrameCenter);               // 从场景中的中心坐标变换到视图中的坐标
    offsetSceneToView.setX(view->width()/2-FrameCenterOnView.x());     // 按中心位置平移，场景和视图坐标的偏移x值，
    offsetSceneToView.setY(view->height()/2-FrameCenterOnView.y());    // 按中心位置平移，场景和视图坐标的偏移y值

    ODBDrawScale *= static_cast<double>(std::min(view->width(),view->height()))/(FrameSize)*FitSclae;// 固定pymq口的大小的为500后，图形的合适大小

    offsetSceneToView *= ODBDrawScale;
}

void PaintingWidget::DrawStructure(CGDSStructure *structure, CCoord<double> &pos, bool reflect, double &angle, double &mag)
{
    if(!structure)
        return;
    CGDSStructure *refStructure;
    bool ereflect;

    for(QList<CGDSElement*>::iterator iter = (structure->m_obaryElements).begin();
        iter != (structure->m_obaryElements).end();iter++)
    {

        CCoord<double> coord;
        CGDSElement *element = *iter;
        switch (element->GetElementType()) {
        case GDS_SREF:
            refStructure = element->GetRefStructure();    // 得到被引用的structure
            element->GetPos(&coord);                      // 得到引用的偏移坐标
            coord.Mul(mag);
            if(reflect)
                coord.y = -coord.y;
            RotatePoint(&coord, angle);                   // 这个是旋转偏移点，对引用的引用有用
            coord += pos;

            double emag;
            ((CGDSSREF*)element)->GetMag(&emag);
            emag *= mag;
             ereflect = reflect ^ (((CGDSStrans*)element)->IsReflect());

            double eangle;
            if(ereflect != ((CGDSStrans*)element)->IsReflect())
                eangle = angle - ((CGDSSREF*)element)->GetAngle();
            else
                eangle = angle + ((CGDSSREF*)element)->GetAngle();

            fmod360(eangle);

            DrawStructure(refStructure, coord, ereflect, eangle, emag);
            break;
        case GDS_AREF:
            DrawAref((CGDSAREF*)element, pos, reflect, angle, mag);
            break;
        case NULL:
            continue;
            break;
        default:
            DrawGDSElement(element, pos, reflect, angle, mag);
            break;
        }
    }
    return;
}

void PaintingWidget::DrawAref(CGDSAREF *element, CCoord<double> &pos, bool reflect, double &angle, double &mag)
{

    if(!element)
        return;

    CCoord<double> coordOrg, coordCol, coordRow;
    element->GetNthCoord(0, &coordOrg);//原始坐标
    element->GetNthCoord(1, &coordCol);//列坐标
    element->GetNthCoord(2, &coordRow);//行坐标

    coordOrg.Mul(mag);
    coordCol.Mul(mag);
    coordRow.Mul(mag);
    if(reflect){
        coordCol.y = -coordCol.y;
        coordOrg.y = -coordOrg.y;
        coordRow.y = -coordRow.y;
    }

    unsigned numCol = element->GetNumCol();// 获得列数
    unsigned numRow = element->GetNumRow();// 获得行数

//    double xstep, ystep; // 这是日本人的原程序，这里是有问题的
//    if(coordCol.x == coordOrg.x && coordRow.y == coordOrg.y){
//        numCol = element->GetNumRow();
//        numRow = element->GetNumCol();
//        xstep = (coordRow.x - coordOrg.x)/(double)numCol; // 向x方向的步进距离
//        ystep = (coordCol.y - coordOrg.y)/(double)numRow; // 向y方向的步进距离
//    }
//    else{
//        xstep = (coordCol.x - coordOrg.x)/(double)numCol;
//        ystep = (coordRow.y - coordOrg.y)/(double)numRow;
//    }

    double emag;
    element->GetMag(&emag);
    emag *= mag;
    bool ereflect = reflect ^ element->IsReflect();
    double eangle;
    if(ereflect != element->IsReflect())
        eangle = angle - element->GetAngle();
    else
        eangle = angle + element->GetAngle();
    fmod360(eangle);

    double xstep,ystep;                    // Dx Dy
    coordCol -= coordOrg;
    coordRow -= coordOrg;
    if(eangle<1e-6){                        // 等于0时不需要旋转
        xstep = coordCol.x/(double)numCol;
        ystep = coordRow.y/(double)numRow;
    }else{
        coordCol.RotatePoint(-eangle);
        coordRow.RotatePoint(-eangle);
        xstep = coordCol.x/(double)numCol;
        ystep = coordRow.y/(double)numRow;
    }

    for(unsigned i = 0;i < numRow;i++){    // 后画的是行
        for(unsigned j = 0;j < numCol;j++){// 先画的是列
            //CCoord<double> coord(coordOrg.x + j * xstep, coordOrg.y + i * ystep);
//            if(j == 0){ // draw outline only
//                if(numCol > 2 && i > 0 && i < numRow - 1)
//                    j = numCol - 2;
//            }
//            RotatePoint(&coord, angle);
//            coord += pos;
            CCoord<double> coord(j * xstep,i * ystep);  // 这里与原文不一样
            coord.RotatePoint(eangle);                  // 偏移坐标的旋转

            //RotatePoint(&coord, eangle);                // 这里的旋转只是
            coord+=coordOrg;                              // AREF不同单元需要平移的不同坐标向量
            coord+=pos;                                   // 引用中的引用有用
            DrawStructure(element->GetRefStructure(), coord, ereflect, eangle, emag);
        }
    }
}



void PaintingWidget::DrawGDSElement(CGDSElement *element, CCoord<double> &pos, bool reflect, double &angle, double &mag)
{
    //int m_intLayerNow = element->GetLayer();//获得在第几层
    switch(element->GetElementType()){//开始画不同的元素
    case GDS_BOUNDARY:
    case GDS_BOX:
        DrawGDSBoundary((CGDSBoundary*)element, pos, reflect, angle, mag);
        break;
    case GDS_PATH:
        DrawGDSPath((CGDSPath*)element, pos, reflect, angle, mag);
        break;
    case GDS_TEXT:
        DrawGDSText((CGDSText*)element, pos, reflect, angle, mag);
        break;
    /*case GDS_NODE:
        // Not supported yet
        DrawGDSNode((CGDSNode*)element, pos, reflect, angle, mag);
        break;
    case GDS_CONTACT:
        // Not supported yet
        DrawGDSContact((CGDSNode*)element, pos, reflect, angle, mag);
         break;*/
    default:
        break;
    }

}

void PaintingWidget::DrawGDSPath(CGDSPath *element, CCoord<double> pos, bool reflect, double angle, double mag)
{
    unsigned numP = ((CGDSXY*)element)->GetNumCoords();         // path element中的坐标点的数目
    CCoord<int> *pCoord = ((CGDSXY*)element)->GetCoordArray();  // 坐标链表
    if(numP == 0)                                               // 数目为零则失败
       return;

    mag *= ODBDrawScale;
    pos.Mul(ODBDrawScale);

    for(unsigned int i=0;i<numP;i++)
    {
        (pCoord+i)->CCoordToQPointF(&points[i],reflect);
        points[i] *= ODBDrawScale;
        RotatePoints(&points[i], angle,1);                    // 旋转一个点
        (points[i]).setX((double)points[i].x()+(double)pos.x);// 加上偏移量。
        (points[i]).setY((double)points[i].y()+(double)pos.y);
        points[i] += offsetSceneToView;
        //point.push_back(*(points+i));
    }

//    CCoord<int> *pCoord = ((CGDSXY*)element)->GetCoordArray();  //获得坐标链表
//    for(unsigned int i=0;i<numP;i++)
//    {
//        points[i].setX(mag*((*(pCoord+i)).x)/50000);
//        if(reflect)
//            points[i].setY(-mag*((*(pCoord+i)).y)/50000);
//        else
//            points[i].setY(mag*((*(pCoord+i)).y)/50000);
//    }
//    RotatePoints(points, angle, numP);//旋转这些点

//    for(unsigned int i = 0;i < numP;i++){
//        (points[i]).setX((double)points[i].x()+(double)pos.x/50000);//加上偏移量。
//        (points[i]).setY((double)points[i].y()+(double)pos.y/50000);
//    }



    QPainterPath path;
    path.moveTo(points[0]);
    for(unsigned int i=1;i<numP;i++)
    {
        path.lineTo(points[i]);
    }

    QGraphicsPathItem * item = new QGraphicsPathItem(path);

    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::red);
    item->setPen(pen);
    addItem(item);
    item->setPos(0,0);
    return;
}

void PaintingWidget::DrawGDSBoundary(CGDSBoundary *element, CCoord<double> pos, bool reflect, double angle, double mag)
{
    unsigned numP = ((CGDSXY*)element)->GetNumCoords();
    CCoord<int> *pCoord = ((CGDSXY*)element)->GetCoordArray();  //获得坐标链表

    QVector<QPointF> point;
    QPointF points[NUM_POLYGON_POINT];

    mag *= ODBDrawScale;
    pos.Mul(ODBDrawScale);


    for(unsigned int i=0;i<numP;i++)
    {
        (pCoord+i)->CCoordToQPointF(&points[i],reflect);
        points[i] *= ODBDrawScale;
        RotatePoints(&points[i], angle,1);                    // 旋转一个点
        (points[i]).setX((double)points[i].x()+(double)pos.x);// 加上偏移量。
        (points[i]).setY((double)points[i].y()+(double)pos.y);
        points[i] += offsetSceneToView;
        point.push_back(*(points+i));
    }


    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::red);

    QGraphicsPolygonItem *item = new QGraphicsPolygonItem(QPolygonF(point));
    item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    item->setPen(pen);
    addItem(item);
    item->setPos(0,0);
    point.clear();
}

void PaintingWidget::DrawGDSText(CGDSText *element, CCoord<double> pos, bool reflect, double angle, double /*mag*/)
{
    CCoord<double> coord;
    element->GetPos(&coord);
//	coord.Mul(m_dblDrawScale);
//	coord.Mul(mag);

    if(reflect)
        coord.y = -coord.y;
    RotatePoint(&coord, angle);
    coord += pos;
//	if(m_intMirror > 0)
//		coord.y = -coord.y;


    QFont font("Times",16);
    QString name(element->GetString());
    QGraphicsTextItem *item = new QGraphicsTextItem(name);
    item->setFont(font);
    item->setFlag(QGraphicsItem::ItemIsMovable);
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    item->setDefaultTextColor(Qt::red);
    item->setPos(0,0);
    addItem(item);

}




/**********************角度的调整函数上边界************************************/
//修正angle使其在(-360，360)
void PaintingWidget::fmod360(double& angle)
{
    if(angle >= 360.0)
        angle -= 360.0;
    else if(angle <= -360.0)
        angle += 360.0;
}
/**********************角度的调整函数下边界************************************/


/**********************坐标的调整函数上边界************************************/

//将1个CCoord类型坐标点旋转angle/180.0*PAI
void PaintingWidget::RotatePoint(CCoord<double>* point, double& angle)
{
    if(angle == 0.0 || (point->x == 0.0 && point->y == 0.0))
        return;

    static double t;
    if(angle == 90.0 || angle == -270.0){
        t = point->x;
        point->x = -point->y;
        point->y = t;
    }
    else if(angle == 180.0 || angle == -180.0){
        point->x = -point->x;
        point->y = -point->y;
    }
    else if(angle == 270.0 || angle == -90.0){
        t = point->x;
        point->x = point->y;
        point->y = -t;
    }
    else{
        t = sqrt(point->x * point->x + point->y * point->y);
        double newang =(double)(atan2(point->y, point->x) + angle*PAI/180.0);
        point->SetCoord(t*cos(newang), t*sin(newang));
    }
    return;
}

//将num个PointF类型坐标点旋转angle/180.0*PAI
void PaintingWidget::RotatePoints(QPointF* point, double& angle, unsigned num)
{
    if(angle == 0.0)
        return;
    static int dir;
    if(angle == 90.0 || angle == -270.0)
        dir = 0;
    else if(angle == 180.0 || angle == -180.0)
        dir = 1;
    else if(angle == 270.0 || angle == -90.0)
        dir = 2;
    else
        dir = 3;

    static double t;
    for(unsigned i = 0;i < num;i++){
        if((point + i)->x() == 0.0 && (point + i)->y() == 0.0)
            continue;
        switch(dir){
        case 0:
            t = (point + i)->x();
            (point + i)->setX(-(point + i)->y());
            (point + i)->setY(t);
            break;
        case 1:
            (point + i)->setX(-(point + i)->x());
            (point + i)->setY(-(point + i)->y());
            break;
        case 2:
            t = (point + i)->x();
            (point + i)->setX((point + i)->y());
            (point + i)->setY(-t);
            break;
        case 3:
        default:
        {
            t = (double)sqrt((point + i)->x() * (point + i)->x() + (point + i)->y() * (point + i)->y());
            double newang = (double)(atan2((point + i)->y(), (point + i)->x()) + angle*PAI/180.0);
            (point + i)->setX((double)(t*cos(newang)));
            (point + i)->setY((double)(t*sin(newang)));
        }
            break;
        }
    }
    return;
}



//将num个CCoord类型坐标点旋转angle/180.0*PAI
/*
void PaintingWidget::RotatePoints(CCoord<double>* point, double& angle, unsigned num)
{
    if(angle == 0.0)
        return;
    static int dir;
    if(angle == 90.0 || angle == -270.0)
        dir = 0;
    else if(angle == 180.0 || angle == -180.0)
        dir = 1;
    else if(angle == 270.0 || angle == -90.0)
        dir = 2;
    else
        dir = 3;

    static double t;
    for(unsigned i = 0;i < num;i++){
        if((point + i)->x == 0.0 && (point + i)->y == 0.0)
            continue;
        switch(dir){
        case 0:
            t = (point + i)->x;
            (point + i)->x = -(point + i)->y;
            (point + i)->y = t;
            break;
        case 1:
            (point + i)->x = -(point + i)->x;
            (point + i)->y = -(point + i)->y;
            break;
        case 2:
            t = (point + i)->x;
            (point + i)->x = (point + i)->y;
            (point + i)->y = -t;
            break;
        case 3:
        default:
        {
            t = sqrt((point + i)->x * (point + i)->x + (point + i)->y * (point + i)->y);
            double newang = atan2((point + i)->y, (point + i)->x) + angle/180.0*PAI;
            (point + i)->x = t*cos(newang);
            (point + i)->y = t*sin(newang);
        }
            break;
        }
    }
    return;
}
//将num个点旋转angle/180.0*PAI
void PaintingWidget::RotatePoints(QPoint* point, double& angle, unsigned num)//将num个点旋转angle/180.0*PAI
{
    if(angle == 0.0)
        return;
    static int dir;
    if(angle == 90.0 || angle == -270.0)
        dir = 0;
    else if(angle == 180.0 || angle == -180.0)
        dir = 1;
    else if(angle == 270.0 || angle == -90.0)
        dir = 2;
    else
        dir = 3;

    static long t;
    for(unsigned i = 0;i < num;i++){
        if((point + i)->x == 0.0 && (point + i)->y == 0.0)//如果点的坐标是在原点，则忽略该点
            continue;
        switch(dir){
        case 0:
            t = (point + i)->x;
            (point + i)->x = -(point + i)->y;
            (point + i)->y = t;
            break;
        case 1:
            (point + i)->x = -(point + i)->x;
            (point + i)->y = -(point + i)->y;
            break;
        case 2:
            t = (point + i)->x;
            (point + i)->x = (point + i)->y;
            (point + i)->y = -t;
            break;
        case 3:
        default:
        {
            double t = sqrt((point + i)->x * (point + i)->x + (point + i)->y * (point + i)->y);
            double newang = atan2((point + i)->y, (point + i)->x) + angle/180.0*PAI;
            (point + i)->x = (long)(t*cos(newang));
            (point + i)->y = (long)(t*sin(newang));
        }
            break;
        }
    }
    return;
}

//矩形绕原点angle/180.0*PAI度

void PaintingWidget::RotateRect(QRect* rect, double& angle)//矩形绕原点angle/180.0*PAI度
{
    if(angle == 0.0)
        return;
    static int dir;
    if(angle == 90.0 || angle == -270.0)
        dir = 0;
    else if(angle == 180.0 || angle == -180.0)
        dir = 1;
    else if(angle == 270.0 || angle == -90.0)
        dir = 2;
    else
        dir = 3;//任意度

    static QPoint point[4];//四个顶点按逆时针转
    point[0].x = point[1].x = rect->left();
    point[0].y = point[3].y = rect->top();
    point[2].x = point[3].x = rect->right();
    point[1].y = point[2].y = rect->bottom();
    static long t;
    for(unsigned i = 0;i < 4;i++){
        switch(dir){
        case 0://(x,y)变成(-y,x)
            t = (point + i)->x;
            (point + i)->x = -(point + i)->y;
            (point + i)->y = t;
            break;
        case 1://(x,y)变成(-x,-y)
            (point + i)->x = -(point + i)->x;
            (point + i)->y = -(point + i)->y;
            break;
        case 2://(x,y)变成(y,-x)
            t = (point + i)->x;
            (point + i)->x = (point + i)->y;
            (point + i)->y = -t;
            break;
        case 3:
        default:
        {
            double t = sqrt((point + i)->x * (point + i)->x + (point + i)->y * (point + i)->y);//直线的距离
            double newang = atan2((point + i)->y, (point + i)->x) + angle/180.0*PAI;
            //atan2(y,x)求的是反正切
            (point + i)->x = (long)(t*cos(newang));
            (point + i)->y = (long)(t*sin(newang));
        }
            break;
        }
    }
    for(i = 0;i < 4;i++){//重新设置rect的left,right,bottom,top四个数据成员
        if(i == 0){//left左上角X坐标，top左上角Y坐标，right右下角X坐标，bottom右下角Y坐标。
            rect->setLeft((point + i)->x) ;
            rect->setRight((point + i)->x);
            rect->setBottom((point + i)->y);
            rect->setTop((point + i)->y);
        }
        else{
            if((point + i)->x < rect->left())//左上角x是最小的
                rect->setLeft((point + i)->x);
            else if((point + i)->x > rect->right())//右下角x是最大的
                rect->setRight((point + i)->x);
            if((point + i)->y < rect->bottom())//右下角y是最小的
                rect->setBottom((point + i)->y);
            else if((point + i)->y > rect->top())//左上角y是最大的
                rect->setTop((point + i)->y);
        }
    }
    return;
}
*/

// CGraphicsItem
#include <QDebug>
CGraphicsItem::CGraphicsItem(QGraphicsScene *scene)
{
    scene->addItem(this);
    pix = QPixmap(1024, 768);
    pix.fill(Qt::white);
    isDrawing = false;
    isDoubleClick = false;
}

CGraphicsItem::~CGraphicsItem()
{

}

void CGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //qDebug()<<"Hello PanXiong";
//    painter->drawRect(lasetPoint.x(),lasetPoint.y(), endPoint.x()-lasetPoint.x(),endPoint.y()-lasetPoint.y());
//    painter->drawRect(20,20,40,40);

    int x, y, w, h;
        x = lasetPoint.x();
        y = lasetPoint.y();
        w = endPoint.x() - x;
        h = endPoint.y() - y;

        //QPainter painter(this->viewport());
        if (isDrawing)
        {
            tempPix = pix;
            QPainter pp(&tempPix);
            pp.drawRect(x, y, w, h);
            painter->drawPixmap(0, 0, tempPix);
        }
        else
        {
            QPainter pp(&pix);
            pp.drawRect(x, y, w, h);
            painter->drawPixmap(0, 0, pix);
        }

        //QGraphicsItem::paint(painter, option, widget);

}

QRectF CGraphicsItem::boundingRect() const
{
    //return QRectF(20,20,100,100);
    return QRectF(lasetPoint,QSizeF(endPoint.x() - lasetPoint.x()+1,
                                       endPoint.y() - lasetPoint.y()+1));
}

void CGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
//    setCursor(Qt::CrossCursor);
//    static int n =0;
//    if(event->button() == Qt::LeftButton){
//        if(n==0){
//            lasetPoint = event->pos();
//            ++n;
//        }else{
//            endPoint = event->pos();
//        }
//    }

    if (event->button() == Qt::LeftButton)
        {
            lasetPoint = event->pos();
            isDrawing = true;
            isDoubleClick = false;
        }

    QGraphicsObject::mousePressEvent(event);

}

void CGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        {
            if (!isDoubleClick)
            {
                endPoint = event->pos();
                isDrawing = false;
                update();//this->viewport()->
            }
        }
    //setCursor(Qt::ArrowCursor);
    QGraphicsObject::mouseReleaseEvent(event);
}

void CGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
//    static int n =0;
//    if(event->button() == Qt::LeftButton){
//        if(n==0){
//            lasetPoint = event->pos();
//            ++n;
//        }else{
//            endPoint = event->pos();
//        }
//    }
    if (event->buttons() & Qt::LeftButton)
        {
            if (!isDoubleClick)
            {
                endPoint = event->pos();
                update();
            }//this->viewport()->
        }

    QGraphicsObject::mouseMoveEvent(event);
}

void CGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        {
            isDoubleClick = true;
        }
    QGraphicsObject::mouseDoubleClickEvent(event);
}


