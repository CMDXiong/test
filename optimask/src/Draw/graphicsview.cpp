#include "graphicsview.h"
#include "./File/gds2reader.h"
#include<QDebug>
#include <qevent.h>
#include <QPainter>
#include <QTextStream>
#include "./Edit/commands.h"

#include "./Edit/CDrawWithMouseCommand.h"
#include "./Edit/CommandManager.h"

#define VIEW_CENTER viewport()->rect().center()
#define VIEW_WIDTH  viewport()->rect().width()
#define VIEW_HEIGHT viewport()->rect().height()
#define M 10
#define FieldValueLine 200

/******************************************************************************
** COptimaskView
*/
COptimaskView::COptimaskView(CGdsDocument* document):m_coordViewOffset(0.0,0.0),m_currentIndex(-1),
    m_zoomDelta(0.1),
    m_scale(1.0),
    m_oldscale(1.0),
    m_ruler(10),
    m_oldruler(10)
{
    resize(450,450);
    this->viewport()->resize(444,419);
    //setDragMode(RubberBandDrag);
    //setDragMode(ScrollHandDrag);
    paintscene  = new PaintingWidget();
    mag         = 1.0;
    angle       = 0.0;
    setScene(paintscene);
    pix         = QPixmap(size());
    pix.fill(Qt::white);
    isDrawing   = false;
    isDoubleClick = false;

    m_gdsdocument = NULL;
    m_bDrawEvent       = false;
    m_bSelectEvent     = false;

    m_translateButton = Qt::LeftButton;
    m_translateSpeed = 1.0;
    m_bMouseTranslate = false;

    m_nNumPoint = 0;

    m_nrow = 10;                // 初始化为10行栅格
    m_ncolumn = 10;             // 初始化的10列栅格
    idWidth = std::ceil( this->viewport()->width()/m_nrow);    // 初始化一个栅格的宽度
    idHeight =std::ceil( this->viewport()->width()/m_ncolumn); // 初始化一个栅格的高度

    setTransformationAnchor(QGraphicsView::NoAnchor);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//去掉水平的滚动条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//去掉垂直滚动条
    // setCursor(Qt::PointingHandCursor);
    setRenderHint(QPainter::Antialiasing);
    setSceneRect(INT_MIN/2, INT_MIN/2, INT_MAX, INT_MAX);//scene的范围
    centerOn(0, 0);

    m_gdsdocument = document;
    if(document != 0){
        m_CurScene = document->GetSceneData();
    }
    m_pen.setColor(Qt::black);
    m_pen.setStyle(Qt::SolidLine);
    m_pen.setWidth(1);
    multiple = 1.0;
}

COptimaskView::~COptimaskView()
{

}

void COptimaskView::SetGdsDocument(CGdsDocument*gds)
{
    m_gdsdocument=gds;
    m_CurScene = m_gdsdocument->GetSceneData();
}


PaintingWidget *COptimaskView::getGraphicsScene()
{
    return paintscene;
}

inline QRectF COptimaskView::GeoRectToQRectF(const CGeoRect &rect) const
{
    return QRectF(rect.m_dLeft, rect.m_dTop, rect.m_dRight, rect.m_dBottom);
}

inline CGeoPt COptimaskView::QPointFToGeoPt(QPointF &point) const
{
    return CGeoPt(point.x(),point.y());
}

inline CGeoPt COptimaskView::QPointFToGeoPt(QPoint &point) const
{
    return CGeoPt(point.x(),point.y());
}

inline QPointF COptimaskView::GeoPtToQPoint(const CGeoPt &point) const
{
    return QPointF(point.dx, point.dy);
}

void COptimaskView::drawShape(const int shapetype)
{
    m_shapeType = shapetype;
    m_bDrawEvent  = true;
    m_bFirstPoint   = true;
    this->viewport()->update();
}


void COptimaskView::drawType(const int type1)
{
    m_shapeType         = type1;
    m_bDrawEvent      = true;
    m_bSelectEvent    = false;
}

void COptimaskView::isSelect(bool select)
{
    m_bSelectEvent = select;
    m_bDrawEvent   = false;
}

void COptimaskView::DrawWithMouse(const int type)
{
    QPainter painter(this->viewport());

    switch (type) {
    case GEO_TYPE_LINE:
    case GEO_TYPE_RECT:
    case GEO_TYPE_CIRCLE:
    case GEO_TYPE_ELLIPSE:
    {
        if (isDrawing)
        {
            tempPix = pix;                                          // 将先前画的图复制给temPix
            QPainter pp(&tempPix);
            if(type == GEO_TYPE_LINE){
                pp.drawLine(m_lasetPos,m_endPos);
            }
            if(type == GEO_TYPE_RECT){
                QRect rect(m_lasetPos,m_endPos);
                pp.drawRect(rect);
            }
            if(type == GEO_TYPE_ELLIPSE){
                QRect rect(m_lasetPos,m_endPos);
                pp.drawEllipse(rect);
            }
            if(type == GEO_TYPE_CIRCLE){
                int radius = distance(m_lasetPos, m_endPos); // 圆的半径
                pp.save();
                pp.setPen(Qt::DotLine);
                pp.drawLine(m_endPos, m_startPos);  // Draw an radius
                pp.restore();

                pp.drawEllipse(m_lasetPos, radius, radius);
            }
            painter.drawPixmap(0, 0, tempPix);
        }
        else
        {
            painter.drawPixmap(0, 0, pix);
        }
    }
        break;
    case GEO_TYPE_POLYGON:
    case GEO_TYPE_PATH:
    {
        if (isDrawing)
        {
            tempPix = pix;                    // 将先前画的图复制给temPix
            QPainter pp(&tempPix);
            pp.drawLine(m_lasetPos,m_endPos);
            if(type == GEO_TYPE_POLYGON){
                pp.save();
                pp.setPen(Qt::DotLine);
                pp.drawLine(m_endPos, m_startPos);
                pp.restore();
            }
            painter.drawPixmap(0, 0, tempPix);
        }
        else
        {
            painter.drawPixmap(0, 0, pix);
        }
    }
        break;
    case GEO_TYPE_ARC:
        if (isDrawing)
        {
            tempPix = pix;                                          // 将先前画的图复制给temPix
            QPainter pp(&tempPix);
            if(type == GEO_TYPE_ARC){
                if(m_nNumPoint == 1){
                    int radius = distance(m_startPos, m_endPos); // 圆的半径
                    pp.save();
                    pp.setPen(Qt::DotLine);
                    pp.drawLine(m_endPos, m_startPos);  // Draw an radius
                    pp.drawEllipse(m_startPos, radius, radius);
                    pp.restore();
                }
                if(m_nNumPoint == 2){
                    pp.save();
                    pp.setPen(Qt::DotLine);
                    pp.drawLine(m_endPos, m_startPos);  // Draw an radius
                    pp.restore();

                    double startAngle;
                    double endAngle;
                    int radiusStart = distance(m_startPos, m_lasetPos);
                    QRect rect(m_startPos.x() - radiusStart, m_startPos.y() - radiusStart, 2*radiusStart, 2*radiusStart);
                    double yValueStart = static_cast<double>(m_lasetPos.y() - m_startPos.y());
                    double xValueStart = static_cast<double>(m_lasetPos.x() - m_startPos.x());
                    double yValueEnd  = static_cast<double>(m_endPos.y() - m_startPos.y());
                    double xValueEnd = static_cast<double>(m_endPos.x() - m_startPos.x());

                    if(xValueStart>=0){
                        if(xValueStart > 0){
                            if(yValueStart>=0){
                                startAngle = std::asin(yValueStart/radiusStart)*180/PAI;          // 第一象限
                            }else{
                                startAngle = 360 - std::asin(-yValueStart/radiusStart)*180/PAI;// 第四象限
                            }
                        }else{
                            if(yValueStart>0){
                                startAngle = 90;      // y轴正半轴
                            }else if(yValueStart<0){
                                startAngle = 270;    // y轴负半轴
                            }else{
                                QMessageBox::information(NULL, tr("Warning"),tr("Two point overlap"));
                                return;
                            }
                        }
                    }else{
                        if(yValueStart>=0){
                            startAngle = 180 - std::asin(yValueStart/radiusStart)*180/PAI;        // 第二象限
                        }else{
                            startAngle = 180 + std::asin(-yValueStart/radiusStart)*180/PAI;      // 第三象限
                        }
                    }

                    int radiusEnd = distance(m_startPos, m_endPos);
                    if(xValueEnd>=0){
                        if(xValueEnd > 0){
                            if(yValueEnd>=0){
                                endAngle = std::asin(yValueEnd/radiusEnd)*180/PAI;          // 第一象限或x正半轴
                            }else{
                                endAngle = 360 - std::asin(-yValueEnd/radiusEnd)*180/PAI;// 第四象限
                            }
                        }else{
                            if(yValueEnd>0){
                                endAngle = 90;      // y轴正半轴
                            }else if(yValueEnd<0){
                                endAngle = 270;    // y轴负半轴
                            }else{
                                QMessageBox::information(NULL, tr("Warning"),tr("Two point overlap"));
                                return;
                            }
                        }
                    }else{
                        if(yValueEnd>=0){
                            endAngle = 180 - std::asin(yValueEnd/radiusEnd)*180/PAI;        // 第二象限或x负半轴
                        }else{
                            endAngle = 180 + std::asin(-yValueEnd/radiusEnd)*180/PAI;      // 第三象限
                        }
                    }

                    pp.drawArc(rect, -startAngle*16, - (endAngle - startAngle)*16);


                }
            }
            painter.drawPixmap(0, 0, tempPix);
        }
        else
        {
            painter.drawPixmap(0, 0, pix);
        }
        break;
    default:
        break;
    }
}

void COptimaskView::DrawPrimitive(CGeoPoint *pData)
{
    if(pData == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("No Data"));
        return;
    }
    QPainter pp(&pix);
    pp.setViewport(viewFrame);
    pp.setWindow(windowFrame);
    pp.translate(0, windowFrame.bottom() + windowFrame.top());
    m_pen.setWidthF(multiple);
    pp.setPen(m_pen);
    pp.setRenderHint(QPainter::Antialiasing, true);

    QPointF pointF(pData->GetPt().dx, -pData->GetPt().dy);
    pp.drawPoint(pointF);
    this->viewport()->update();
}

void COptimaskView::DrawPrimitive(CGeoLine *pData)
{
    if(pData == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("No Data"));
        return;
    }
    QPainter pp(&pix);
    pp.setViewport(viewFrame);
    pp.setWindow(windowFrame);
    pp.translate(0, windowFrame.bottom() + windowFrame.top());
    m_pen.setWidthF(multiple);
    pp.setPen(m_pen);
    pp.setRenderHint(QPainter::Antialiasing, true);

    pp.drawLine(pData->GetFirstPt().dx, -pData->GetFirstPt().dy,
                pData->GetSecondPt().dx, -pData->GetSecondPt().dy);
    this->viewport()->update();
}

void COptimaskView::DrawPrimitive(CGeoCircle *pData)
{
    if(pData == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("No Data"));
        return;
    }

    QPainter pp(&pix);
    pp.setViewport(viewFrame);
    pp.setWindow(windowFrame);
    m_pen.setWidthF(multiple);
    pp.setPen(m_pen);
    pp.setRenderHint(QPainter::Antialiasing, true);

    pp.translate(0, windowFrame.bottom() + windowFrame.top());
    QPointF center1(pData->GetCenterPt().dx, -pData->GetCenterPt().dy);
    pp.drawEllipse(center1, pData->GetRadius(),pData->GetRadius());

    //pp.drawRect(m_sceneFrame.m_dLeft, m_sceneFrame.m_dTop, m_sceneFrame.Width(),m_sceneFrame.Height());
    this->viewport()->update();
}

void COptimaskView::DrawPrimitive(CGeoRectangle *pData)
{
    if(pData == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("No Data"));
        return;
    }
    QPainter pp(&pix);
    pp.setViewport(viewFrame);
    pp.setWindow(windowFrame);
    m_pen.setWidthF(multiple);
    pp.setPen(m_pen);
    pp.setRenderHint(QPainter::Antialiasing, true);

    pp.translate(0, windowFrame.bottom() + windowFrame.top());
    pp.drawRect(pData->GetTopLeftPt().dx, -pData->GetTopLeftPt().dy, pData->Width(), pData->Height());
    this->viewport()->update();
}

void COptimaskView::DrawPrimitive(CGeoArc *pData)
{
    if(pData == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("No Data"));
        return;
    }
    QPainter pp(&pix);
    pp.setViewport(viewFrame);
    pp.setWindow(windowFrame);
    m_pen.setWidthF(multiple);
    pp.setPen(m_pen);
    pp.setRenderHint(QPainter::Antialiasing, true);

    FLOAT_T rad = pData->GetRadius();
    pp.translate(0, windowFrame.bottom() + windowFrame.top());
    QRectF rectf1(pData->GetCenterPt().dx - rad, -pData->GetCenterPt().dy - rad, 2*rad, 2*rad);
    pp.drawArc(rectf1, pData->GetAngleS()*16,(pData->GetAngleE()*16-pData->GetAngleS()*16));
    this->viewport()->update();
}

void COptimaskView::DrawPrimitive(CGeoLWPolyline *pData)
{
    if(pData == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("No Data"));
        return;
    }
    QPainter pp(&pix);
    pp.setViewport(viewFrame);
    pp.setWindow(windowFrame);
    pp.translate(0, windowFrame.bottom() + windowFrame.top());
    m_pen.setWidthF(multiple);
    pp.setPen(m_pen);
    pp.setRenderHint(QPainter::Antialiasing, true);

    std::vector<CGeoPt> allPoint = pData->GetPtList();
    int numPoint = pData->GetPtNum();
    QPointF* points = new QPointF[numPoint]; //C++不允许以变量作为数组的大小
    for(int i = 0; i<numPoint; ++i ){
        points[i]  = GeoPtToQPoint(allPoint[i]);
        points[i].setY(-points[i].y());
    }
    if(pData->IsClosed()){//封闭
        pp.drawPolygon(points, numPoint);
    }else{
        QPainterPath path;
        path.moveTo(points[0]);
        for(int i = 1; i<numPoint; ++i ){
            path.lineTo(points[i]);
        }
        pp.drawPath(path);
    }
    delete[] points;
    this->viewport()->update();

}

void COptimaskView::DrawPrimitive(CGeoEllipse *pData)
{
    if(pData == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("No Data"));
        return;
    }
    QPainter pp(&pix);
    pp.setViewport(viewFrame);
    pp.setWindow(windowFrame);
    m_pen.setWidthF(multiple);
    pp.setPen(m_pen);
    pp.setRenderHint(QPainter::Antialiasing, true);
    this->viewport()->update();
}

void COptimaskView::DrawPrimitive(CGeoText *pData)
{
    if(pData == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("No Data"));
        return;
    }
    QPainter pp(&pix);
    pp.setViewport(viewFrame);
    pp.setWindow(windowFrame);
    m_pen.setWidthF(multiple);
    pp.setPen(m_pen);
    pp.setRenderHint(QPainter::Antialiasing, true);
    pp.translate(0, windowFrame.bottom() + windowFrame.top());
    CGeoPt Pt = pData->GetStartPt();
    Pt.dy *=(-1);
    pp.drawText(GeoPtToQPoint(Pt),QString::fromLocal8Bit(pData->GetText().c_str()));
    this->viewport()->update();
}

void COptimaskView::DrawPrimitive(CGeoMulripler *pData)
{
    if(pData == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("No Data"));
        return;
    }
    for(unsigned int i = 0; i<pData->GetChildCount(); ++i)
    {
        DrawBasePrimitive(pData->GetChild(i));
    }
}

void COptimaskView::DrawPrimitive(CGeoTransform *pData)
{
    CMatrix  mat;
    pData->GetTransformMatrix(mat);
    CGeoBase *geoChild = pData->GetChild();

    CGeoTransform* pTran = dynamic_cast<CGeoTransform*>(geoChild);
    if (pTran != 0)
    {
        CGeoTransform trans = *pTran;
        mat*= pTran->GetTransformMatrix();   //更新矩阵
        trans.SetTransformMatrix(mat);
        DrawPrimitive(&trans);
        return;
    }

    CGeoMulripler* pMulti = dynamic_cast<CGeoMulripler*>(geoChild);
    if (pMulti != 0)
    {
        if(pMulti->GetObjType() == GEO_TYPE_MULRIPLER){
            for (unsigned i = 0; i < pMulti->GetChildCount(); i++)
            {
                geoChild = pMulti->GetChild(i);
                CGeoTransform transform;
                transform.SetTransformMatrix(mat);
                transform.SetChild(geoChild);
                DrawPrimitive(&transform);
            }
        }
        if(pMulti->GetObjType() == GEO_TYPE_STRUCT){
            CGeoGDSStruct *pStruct = static_cast<CGeoGDSStruct*>(geoChild);
            for (unsigned i = 0; i < pStruct->GetChildCount(); i++)
            {
                geoChild = pStruct->GetChild(i);
                CGeoTransform transform;
                transform.SetTransformMatrix(mat);
                transform.SetChild(geoChild);
                DrawPrimitive(&transform);
            }
        }
        if(pMulti->GetObjType() == GEO_TYPE_ARRAY){
            CGeoArray *pArray = static_cast<CGeoArray*>(geoChild);

            int     rowNum, colNum;
            FLOAT_T interRow, interCol;
            pArray->GetRowAndCol(rowNum, colNum);
            pArray->GetRowAndColInterDist(interRow, interCol);
            for(int i = 0; i<rowNum; ++i)
                for(int j = 0; j<colNum; ++j)
                    for (unsigned k = 0; k < pArray->GetChildCount(); ++k)
                    {
                        //这种方法好像不行
                        //                        CGeoTransform transform;
                        //                        CGeoPt pt(j*interCol, i*interRow);
                        //                        CMatrix   invMat;
                        //                        mat.GetInvMatrix(invMat);
                        //                        CGeoPt newPt = invMat * pt;

                        //                        transform.SetTransformMatrix(mat);
                        //                        transform.Translate(newPt.dx,newPt.dy);
                        //                        transform.SetChild(pArray->GetChild(k));
                        //                        DrawPrimitive(&transform);

                        CGeoTransform transform;
                        FLOAT_T angle = mat.GetAngle(); //角度是弧度
                        CGeoPt pt(j*interCol, i*interRow);

                        CMatrix rotateMat =CMatrix::Rotate(angle);
                        CGeoPt newPt = rotateMat * pt;
                        CMatrix translateMat = CMatrix::Translate(newPt.dx,newPt.dy);
                        translateMat *= mat;
                        transform.SetTransformMatrix(translateMat);
                        transform.SetChild(pArray->GetChild(k));
                        DrawPrimitive(&transform);
                    }
        }
        return;
    }

    switch(geoChild->GetObjType())
    {
    case GEO_TYPE_POINT:
    {
        CGeoPoint* point = static_cast<CGeoPoint* >(geoChild);
        CGeoPt pt = mat *point->GetPt();
        CGeoPoint newPoint;
        newPoint.SetPt(pt.dx, pt.dy);
        DrawPrimitive(&newPoint);
    }
        break;
    case GEO_TYPE_LINE:
    {
        CGeoLine* line = static_cast<CGeoLine* >(geoChild);
        CGeoPt newFirstPt  = mat * line->GetFirstPt();
        CGeoPt newSecondPt = mat * line->GetSecondPt();
        CGeoLine newLine(newFirstPt, newSecondPt);
        DrawPrimitive(&newLine);
    }
        break;
    case GEO_TYPE_CIRCLE:
    {
        CGeoCircle* circle = static_cast<CGeoCircle* >(geoChild);
        CGeoCircle newCircle;
        CMatrix rotateTranslateMaT;
        mat.GetRotateAndTranslate(rotateTranslateMaT);
        newCircle.SetRadius(mat.GetScale() * circle->GetRadius());
        newCircle.SetCenterPt(rotateTranslateMaT * circle->GetCenterPt());
        DrawPrimitive(&newCircle);
    }
        break;
    case GEO_TYPE_ARC:
    {
        CGeoArc* arc = static_cast<CGeoArc* >(geoChild);
        CGeoArc newArc;
        CMatrix rotateTranslateMaT;
        mat.GetRotateAndTranslate(rotateTranslateMaT);
        newArc.SetRadius(mat.GetScale() * arc->GetRadius());                 // 设置新的半径
        newArc.SetCenterPt(rotateTranslateMaT * arc->GetCenterPt());         // 设置新的中心点
        newArc.SetAngleS(mat.GetAngle() + arc->GetAngleS());                 // 设置新的起始角度
        newArc.SetAngleE(mat.GetAngle() + arc->GetAngleE());                 // 设置新的终止角度

        DrawPrimitive(&newArc);
    }
        break;
    case GEO_TYPE_LWPOLYLINE:
    {
        const CGeoLWPolyline* pPloy = static_cast<const CGeoLWPolyline*>(geoChild);
        const std::vector<CGeoPt>& ptList = pPloy->GetPtList();
        CGeoLWPolyline lwPolyline;
        lwPolyline.SetClose(pPloy->IsClosed());
        lwPolyline.SetFill(pPloy->IsFill());

        //qDebug("point [%d]:\n", ptList.size());

        for (unsigned i = 0;i < ptList.size();i++)
        {
            const CGeoPt& pt = ptList.at(i);
            CGeoPt ptNew = mat*pt;
            //qDebug("%03d(%.1lf,%.1lf)\t(%.1lf,%.1lf)\n", i+1,pt.dx, pt.dy, ptNew.dx, ptNew.dy);
            lwPolyline.AddPt(ptNew);
        }
        DrawPrimitive(&lwPolyline);
    }
        break;
    case GEO_TYPE_ELLIPSE:
    {
        CGeoEllipse* pEllipse = static_cast<CGeoEllipse* >(geoChild);

        FLOAT_T width  = mat.GetScale() * pEllipse->GetWidth();      // 新的长轴一半
        FLOAT_T height = mat.GetScale() * pEllipse->GetHeight();     // 新的短轴一半

        CMatrix rotateTranslateMaT;
        mat.GetRotateAndTranslate(rotateTranslateMaT);
        CGeoPt center = rotateTranslateMaT * pEllipse->GetCenterPt(); // 新的圆心
        FLOAT_T angle = mat.GetAngle();                               // 弧度
        //QRectF ellipseFrame(center.dx - width, center.dy - height, 2*width, 2*height);


        QPainter pp(&pix);
        pp.setViewport(viewFrame);
        pp.setWindow(windowFrame);
        m_pen.setWidthF(multiple);
        pp.setPen(m_pen);
        pp.setRenderHint(QPainter::Antialiasing, true);
        pp.translate(0, windowFrame.bottom() + windowFrame.top());


        pp.save();
        pp.translate(center.dx, -center.dy);              // 使用逻辑坐标
        pp.rotate(angle*180/M_PI);
        QRectF ellipseFrame1(-width, -height, 2*width, 2*height);
        pp.drawEllipse(ellipseFrame1);
        pp.restore();
        this->viewport()->update();
    }
        break;
    case GEO_TYPE_TEXT:
    {
        CGeoText* pText = static_cast<CGeoText* >(geoChild);
        CMatrix rotateTranslateMaT;
        mat.GetRotateAndTranslate(rotateTranslateMaT);
        CGeoText newText;
        newText.SetStartPt(mat * pText->GetStartPt());
        newText.SetText(pText->GetText());
        DrawPrimitive(&newText);
    }
        break;
    case GEO_TYPE_RECT:
    {
        CGeoRectangle* pRect = static_cast<CGeoRectangle*>(geoChild);
        CGeoRectangle newRect;
        newRect.SetRectPt(mat*pRect->GetTopLeftPt(), mat*pRect->GetRightBottomPt());
        DrawPrimitive(&newRect);
    }
        break;
    default:
        break;
    }
}

void COptimaskView::DrwaPrimitive(CGeoArray *pData)
{
    if(pData == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("CGeoArray: No Data"));
        return;
    }
    int     rowNum, colNum;
    FLOAT_T interRow, interCol;
    pData->GetRowAndCol(rowNum, colNum);
    pData->GetRowAndColInterDist(interRow, interCol);
    for(int i = 0; i<rowNum; ++i)
        for(int j = 0; j<colNum; ++j)
        {
            for (unsigned k = 0; k < pData->GetChildCount(); ++k)
            {
                CGeoBase* pNode = pData->GetChild(k);
                CGeoTransform transform;
                transform.Translate(i*interRow, j*interCol);
                transform.SetChild(pNode);
                DrawPrimitive(&transform);
            }
            //下面是对于子节点多于一个的情况下，更高效的做法，而对于只有一个子节点的情况下，效率会低一些
            //            CGeoMulripler tmpMul;
            //            for (unsigned k = 0; k < pData->GetChildCount(); ++k)
            //            {
            //                tmpMul.AddChild(pData->GetChild(k));
            //            }
            //            CGeoTransform transform;
            //            transform.Translate(i*interRow, j*interCol);
            //            transform.SetChild(&tmpMul);
            //            DrawPrimitive(&transform);
        }
}

void COptimaskView::DrawItemFrame(CGeoBase* pData)
{
    if(pData == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("No Data"));
        return;
    }
    QPainter pp(&pix);
    pp.setViewport(viewFrame);
    pp.setWindow(windowFrame);
    pp.translate(0, windowFrame.bottom() + windowFrame.top());
    m_pen.setWidthF(multiple);
    pp.setPen(m_pen);
    pp.setRenderHint(QPainter::Antialiasing, true);

    CGeoRect itemframe = pData->GetBoundBox(); //画图元包围盒
    pp.drawRect(itemframe.m_dLeft, -itemframe.m_dBottom, itemframe.Width(), itemframe.Height());
    this->viewport()->update();
}

void COptimaskView::DrawStructure(CGeoGDSStruct *pCell)
{
    if(pCell == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("CGeoGDSStruct: 没有构元数据"));
        return;
    }
    for (unsigned int j = 0; j < pCell->GetChildCount();j++)
    {
        DrawBasePrimitive(pCell->GetChild(j));
//        CGeoBase* pChild = pCell->GetChild(j);
//        CGeoTransform* pTran = dynamic_cast<CGeoTransform*>(pChild);
//        if (pTran != 0)
//        {
//            DrawPrimitive(pTran);
//        }else{
//            DrawBasePrimitive(pChild);
//        }
    }
}

void COptimaskView::DrawStructure(std::string sname)
{
    DrawStructure(FindStructureByName(sname));
}

CGeoGDSStruct* COptimaskView::FindStructureByName(std::string sname)
{
    const GeoStructList& lstStru = m_CurScene->GetCellList();
    for (unsigned int i = 0; i < lstStru.size(); i++)
    {
        CGeoGDSStruct* pCell = lstStru.at(i).get();
        std::string name = pCell->GetName();
        if(name == sname){
            return pCell;
        }
    }
    return 0;
}

CGeoGDSStruct *COptimaskView::FindMainStructure()
{
    const GeoStructList& lstStru = m_CurScene->GetCellList();
    ref_ptr<CGeoGDSStruct>  mainStruct;
    for (unsigned int i = 0; i < lstStru.size(); i++)
    {
        CGeoGDSStruct* pCell = lstStru.at(i).get();
        unsigned int referenceLevel = 0;
        unsigned int childNum = 0;
        time_t       cellTime = 0;
        if(pCell->GetRefCount()== 0)                 // 主构元
        {
            if(referenceLevel < GeoStructureReferenceLevel(pCell)){
                referenceLevel = GeoStructureReferenceLevel(pCell);
                mainStruct = pCell;
            }else if(childNum < pCell->GetChildCount()) {  // 比较子孩子数目
                childNum = pCell->GetChildCount();
                mainStruct = pCell;
            }else if(childNum == pCell->GetChildCount()){ //比较最后的修改时间
                if(cellTime < pCell->GetModifyTime()){
                    cellTime = pCell->GetModifyTime();
                    mainStruct = pCell;
                }
            }
        }
    }
    return mainStruct.get();
}

void COptimaskView::DrawMainStructure()
{
    CGeoGDSStruct *mainStruct = FindMainStructure();       // 寻找主构元
    if(mainStruct == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("MainStruct: No Found"));
        return;
    }
    m_sceneFrame = mainStruct->GetBoundBox();              // 计算主构元的包围盒
    setViewPort();                                         // 计算主构元包围盒所对应的物理坐标范围
    m_dboriginMapItemX = windowFrame.x();
    m_dboriginMapItemY = windowFrame.y();
    SetMap(mainStruct);                                    // 建立图元选中的映射关系
    DrawStructure(mainStruct);                             // 画主构元
}

void COptimaskView::DrawMainStructureByName(std::string sname)
{
    CGeoGDSStruct *mainStruct = FindStructureByName(sname);       // 寻找主构元
        if(mainStruct == 0){
            QMessageBox::information(NULL, tr("Warning"),tr("MainStruct: No Found"));
            return;
        }
        m_sceneFrame = mainStruct->GetBoundBox();              // 计算主构元的包围盒
        setViewPort();                                         // 计算主构元包围盒所对应的物理坐标范围
        m_dboriginMapItemX = windowFrame.x();
        m_dboriginMapItemY = windowFrame.y();
        SetMap(mainStruct);                                    // 建立图元选中的映射关系
        DrawStructure(mainStruct);                             // 画主构元
}

unsigned int COptimaskView::GeoTransformReferenceLevel(CGeoTransform* trans)
{
    if(trans == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("CGeoTransform: No ReferenceLevel"));
        return 0;
    }
    int referencelevel = 0;
    CGeoBase* geoChild = trans->GetChild();
    if(geoChild->GetObjType() == GEO_TYPE_STRUCT){
        CGeoGDSStruct* struc = static_cast<CGeoGDSStruct*>(geoChild);
        referencelevel++;
        referencelevel += GeoStructureReferenceLevel(struc);
        return referencelevel;
    }
    return 0;
}

unsigned int COptimaskView::GeoStructureReferenceLevel(CGeoGDSStruct *struc)
{
    if(struc == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("CGeoGDSStruct: No ReferenceLevel"));
        return 0;
    }
    unsigned int referencelevel = 0;
    for (unsigned j = 0; j < struc->GetChildCount();j++)
    {
        unsigned int childreferencelevel = 0;
        CGeoBase* pChild = struc->GetChild(j);
        CGeoTransform* trans = static_cast<CGeoTransform*>(pChild);
        if(trans != 0){
            childreferencelevel += GeoTransformReferenceLevel(trans);
            referencelevel = std::max(referencelevel, childreferencelevel);
        }

        CGeoMulripler* pMulti = dynamic_cast<CGeoMulripler*>(pChild);
        if (pMulti != 0)
        {
            if(pMulti->GetObjType() == GEO_TYPE_STRUCT){
                CGeoGDSStruct *pStruc = static_cast<CGeoGDSStruct*>(pMulti);
                childreferencelevel++;
                childreferencelevel += GeoStructureReferenceLevel(pStruc);
                referencelevel = std::max(referencelevel, childreferencelevel);
            }
        }
    }
    return referencelevel;
}

void COptimaskView::TestData()
{
    const GeoStructList& lstStru = m_CurScene->GetCellList();
    for (int i = 0; i < lstStru.size(); i++)
    {
        const CGeoGDSStruct* pCell = lstStru.at(i).get();
        //qDebug("Struct(Name,refcount) [%s]--->%d\n", pCell->GetName().c_str(), pCell->GetRefCount());
        for (int j = 0; j < pCell->GetChildCount();j++)
        {
            CMatrix mat;
            CGeoBase* pChild =const_cast<CGeoBase*>(pCell->GetChild(j));
            CGeoTransform* pTran = dynamic_cast<CGeoTransform*>(pChild);
            if (pTran != 0)
            {
                pTran->GetTransformMatrix(mat);
                pChild = const_cast<CGeoBase*>(pTran->GetChild());
                DrawPrimitive(pTran);
            }else{
                DrawBasePrimitive(pChild);
            }
        }
        //qDebug("Struct [%s] End\n\n", pCell->GetName().c_str());
    }
}

void COptimaskView::DrawBasePrimitive(CGeoBase *pData)
{
    CGeoTransform* pTran = dynamic_cast<CGeoTransform*>(pData);
    if (pTran != 0)
    {
        DrawPrimitive(pTran);
        return ;
    }

    CGeoMulripler* pMulti = dynamic_cast<CGeoMulripler*>(pData);
    if (pMulti != 0)
    {
        CMatrix mat;
        if(pMulti->GetObjType() == GEO_TYPE_MULRIPLER){
            for (unsigned i = 0; i < pMulti->GetChildCount(); i++)
            {
                CGeoBase* geoChild = pMulti->GetChild(i);
                CGeoTransform transform;
                transform.SetTransformMatrix(mat);
                transform.SetChild(geoChild);
                DrawPrimitive(&transform);
            }
        }
        if(pMulti->GetObjType() == GEO_TYPE_STRUCT){
            CGeoGDSStruct *pStruct = static_cast<CGeoGDSStruct*>(pData);
            for (unsigned i = 0; i < pStruct->GetChildCount(); i++)
            {
                CGeoBase* geoChild = pStruct->GetChild(i);
                CGeoTransform transform;
                transform.SetTransformMatrix(mat);
                transform.SetChild(geoChild);
                DrawPrimitive(&transform);
            }
        }
        if(pMulti->GetObjType() == GEO_TYPE_ARRAY){
            CGeoArray *pArray = static_cast<CGeoArray*>(pData);

            int     rowNum, colNum;
            FLOAT_T interRow, interCol;
            pArray->GetRowAndCol(rowNum, colNum);
            pArray->GetRowAndColInterDist(interRow, interCol);
            for(int i = 0; i<rowNum; ++i)
                for(int j = 0; j<colNum; ++j)
                    for (unsigned k = 0; k < pArray->GetChildCount(); ++k)
                    {
                        CGeoTransform transform;
                        FLOAT_T angle = mat.GetAngle(); //角度是弧度
                        CGeoPt pt(j*interCol, i*interRow);

                        CMatrix rotateMat =CMatrix::Rotate(angle);
                        CGeoPt newPt = rotateMat * pt;
                        CMatrix translateMat = CMatrix::Translate(newPt.dx,newPt.dy);
                        translateMat *= mat;
                        transform.SetTransformMatrix(translateMat);
                        transform.SetChild(pArray->GetChild(k));
                        DrawPrimitive(&transform);
                    }
        }
        return;
    }

    switch(pData->GetObjType()){
    case GEO_TYPE_POINT:
        break;
    case GEO_TYPE_LWPOLYLINE:
        DrawPrimitive(dynamic_cast<CGeoLWPolyline*>(pData));
        break;
    case GEO_TYPE_LINE:
        DrawPrimitive(dynamic_cast<CGeoLine*>(pData));
        break;
    case GEO_TYPE_CIRCLE:
        DrawPrimitive(dynamic_cast<CGeoCircle*>(pData));
        break;
    case GEO_TYPE_ARC:
        DrawPrimitive(dynamic_cast<CGeoArc*>(pData));
        break;
    case GEO_TYPE_RECT:
        DrawPrimitive(dynamic_cast<CGeoRectangle*>(pData));
        break;
    case GEO_TYPE_ELLIPSE:
        DrawPrimitive(dynamic_cast<CGeoEllipse*>(pData));
        break;
    case GEO_TYPE_TEXT:
        DrawPrimitive(dynamic_cast<CGeoText*>(pData));
        break;
    case GEO_TYPE_MULRIPLER:
        DrawPrimitive(dynamic_cast<CGeoMulripler*>(pData));
        break;
    case GEO_TYPE_TRANSFORM:
        DrawPrimitive(dynamic_cast<CGeoTransform*>(pData));
        break;
        //    case GEO_TYPE_STRUCT:
        //        DrawPrimitive(dynamic_cast<CGeoGDSStruct*>(pData));
    default:
        break;
    }
}

void COptimaskView::SetSceneFrame()
{
    std::vector<FLOAT_T> xVector;
    std::vector<FLOAT_T> yVector;

    std::vector<ref_ptr<CGeoLayer> > layerList = m_CurScene->GetLayerList();
    for(std::vector<ref_ptr<CGeoLayer> >::iterator iterLayer = layerList.begin(); iterLayer != layerList.end(); ++iterLayer){
        ref_ptr<CGeoLayer> layer = *iterLayer;
        GeoObjList objList = layer->GetObjList();
        for(std::vector<ref_ptr<CGeoBase> >::iterator iterObj = objList.begin(); iterObj != objList.end(); ++iterObj){

            CGeoBase* pData = (*iterObj).get();
            CGeoRect rect = pData->GetBoundBox();
            xVector.push_back(rect.m_dLeft);
            xVector.push_back(rect.m_dRight);
            yVector.push_back(rect.m_dTop);
            yVector.push_back(rect.m_dBottom);

        }
    }
    m_sceneFrame =  Frame(xVector, yVector);
    m_dboriginMapItemX = *(std::min_element(xVector.begin(), xVector.end()));
    m_dboriginMapItemY = *(std::min_element(yVector.begin(), yVector.end()));
}

CGeoRect COptimaskView::LayerFrame(const int& n) const
{
    std::vector<FLOAT_T> xVector;
    std::vector<FLOAT_T> yVector;

    CGeoLayer* layer = m_CurScene->GetLayerByIndex(n);
    GeoObjList objList = layer->GetObjList();
    for(std::vector<ref_ptr<CGeoBase> >::iterator iterObj = objList.begin(); iterObj != objList.end(); ++iterObj){
        CGeoBase* pData = (*iterObj).get();
        xVector.push_back(pData->GetBoundBox().m_dLeft);
        xVector.push_back(pData->GetBoundBox().m_dRight);
        yVector.push_back(pData->GetBoundBox().m_dTop);
        yVector.push_back(pData->GetBoundBox().m_dBottom);
    }
    return Frame(xVector, yVector);
}

CGeoRect COptimaskView::Frame(const std::vector<double> &xVector, const std::vector<double> &yVector) const
{
    FLOAT_T dbMinX = *(std::min_element(xVector.begin(), xVector.end()));
    FLOAT_T dbMaxX = *(std::max_element(xVector.begin(), xVector.end()));
    FLOAT_T dbMinY = *(std::min_element(yVector.begin(), yVector.end()));
    FLOAT_T dbMaxY = *(std::max_element(yVector.begin(), yVector.end()));

    return CGeoRect(dbMinX-2, dbMinY-2, dbMaxX+2, dbMaxY+2);//冗余两个单位
}

void COptimaskView::SetMap(CGeoGDSStruct* pData)
{
    if(pData == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("无主构元"));
        return;
    }
    SetRow(10);
    SetColumn(10);
    SetHeight(m_sceneFrame);
    SetWidth(m_sceneFrame);

    SetGeoStructureMap(pData);
}

void COptimaskView::SetGeoTransformMap(ref_ptr<CGeoTransform> pData)
{
    if(pData == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("No Map Data"));
        return;
    }

    CGeoBase* child = pData->GetChild();
    CMatrix mat;
    pData->GetTransformMatrix(mat);

    CGeoTransform* pTran = dynamic_cast<CGeoTransform*>(child); // 判断子节点是否是CGeoTransform类或其派生类
    if (pTran != 0)
    {
//        CGeoTransform trans/* = *pTran*/;
//        mat *= pTran->GetTransformMatrix();//更新矩阵
//        trans.SetTransformMatrix(mat);
//        trans.SetChild(pTran->GetChild());
//        SetGeoTransformMap(&trans);
//        return ;
        ref_ptr<CGeoTransform> trans(new CGeoTransform());
        mat *= pTran->GetTransformMatrix();//更新矩阵
        trans->SetTransformMatrix(mat);
        trans->SetChild(pTran->GetChild());
        SetGeoTransformMap(trans);
        return ;
    }

    CGeoMulripler* pMulti = dynamic_cast<CGeoMulripler*>(child);
    if (pMulti != 0)
    {
        if (pMulti->GetObjType() == GEO_TYPE_MULRIPLER || pMulti->GetObjType() == GEO_TYPE_STRUCT) {
            for (unsigned i = 0; i < pMulti->GetChildCount(); ++i)
            {
                CGeoBase *geoChild = pMulti->GetChild(i);
                ref_ptr<CGeoTransform> transform(new CGeoTransform());
                transform->SetTransformMatrix(mat);
                transform->SetChild(geoChild);
                SetGeoTransformMap(transform);
            }
            return ;
        }
        if (pMulti->GetObjType() == GEO_TYPE_ARRAY) {
            CGeoArray *pArray = static_cast<CGeoArray*>(child);

            int     rowNum, colNum;
            FLOAT_T interRow, interCol;
            pArray->GetRowAndCol(rowNum, colNum);
            pArray->GetRowAndColInterDist(interRow, interCol);
            for (int i = 0; i<rowNum; ++i)
                for (int j = 0; j<colNum; ++j)
                    for (unsigned k = 0; k < pArray->GetChildCount(); ++k)
                    {
//                        CGeoTransform transform;
//                        FLOAT_T angle = mat.GetAngle();
//                        CGeoPt pt(j*interCol, i*interRow);

//                        CMatrix rotateMat = CMatrix::Rotate(angle);
//                        CGeoPt newPt = rotateMat * pt;
//                        CMatrix translateMat = CMatrix::Translate(newPt.dx, newPt.dy);
//                        translateMat *= mat;
//                        transform.SetTransformMatrix(translateMat);
//                        transform.SetChild(pArray->GetChild(k));

//                        SetGeoTransformMap(&transform);
                        ref_ptr<CGeoTransform> transform(new CGeoTransform());
                        FLOAT_T angle = mat.GetAngle();
                        CGeoPt pt(j*interCol, i*interRow);

                        CMatrix rotateMat = CMatrix::Rotate(angle);
                        CGeoPt newPt = rotateMat * pt;
                        CMatrix translateMat = CMatrix::Translate(newPt.dx, newPt.dy);
                        translateMat *= mat;
                        transform->SetTransformMatrix(translateMat);
                        transform->SetChild(pArray->GetChild(k));

                        SetGeoTransformMap(transform);
                    }
            return ;
        }
    }

    CGeoRect rect;
    switch (child->GetObjType()) {
    case GEO_TYPE_POINT:
    {
        CGeoPoint* point = static_cast<CGeoPoint* >(child);
        CGeoPt newPt = mat * point->GetPt();
        CGeoPoint newPoint;
        newPoint.SetPt(newPt.dx, newPt.dy);
        rect = newPoint.GetBoundBox();
        break;
    }

    case GEO_TYPE_LINE:
    {
        CGeoLine* line = static_cast<CGeoLine* >(child);
        CGeoPt newFirstPt = mat * line->GetFirstPt();
        CGeoPt newSecondPt = mat * line->GetSecondPt();
        CGeoLine newLine(newFirstPt, newSecondPt);
        rect = newLine.GetBoundBox();
        break;
    }
    case GEO_TYPE_CIRCLE:
    {
        CGeoCircle* circle = static_cast<CGeoCircle* >(child);
        CGeoCircle newCircle;
        CMatrix rotateTranslateMaT;                                         // 旋转平移矩阵
        mat.GetRotateAndTranslate(rotateTranslateMaT);
        newCircle.SetRadius(mat.GetScale() * circle->GetRadius());          // 半径只能放缩
        newCircle.SetCenterPt(rotateTranslateMaT * circle->GetCenterPt());  // 圆心旋转平移
        rect = newCircle.GetBoundBox();
        break;
    }
    case GEO_TYPE_ARC:
    {
        CGeoArc* arc = static_cast<CGeoArc* >(child);
        CGeoArc newArc;
        CMatrix rotateTranslateMaT;
        mat.GetRotateAndTranslate(rotateTranslateMaT);

        newArc.SetRadius(mat.GetScale() * arc->GetRadius());                 // 设置新的半径
        newArc.SetCenterPt(rotateTranslateMaT * arc->GetCenterPt());         // 设置新的中心点
        newArc.SetAngleS(mat.GetAngle() + arc->GetAngleS());                 // 设置新的起始角度
        newArc.SetAngleE(mat.GetAngle() + arc->GetAngleE());                 // 设置新的终止角度
        rect = newArc.GetBoundBox();
        break;
    }
    case GEO_TYPE_LWPOLYLINE:
    {
        const CGeoLWPolyline* pPloy = static_cast<const CGeoLWPolyline*>(child);
        const std::vector<CGeoPt>& ptList = pPloy->GetPtList();
        CGeoLWPolyline newLwPolyline;
        newLwPolyline.SetClose(pPloy->IsClosed());
        newLwPolyline.SetFill(pPloy->IsFill());
        for (unsigned i = 0;i < ptList.size();i++)
        {
            const CGeoPt& pt = ptList.at(i);
            CGeoPt ptNew = mat*pt;
            newLwPolyline.AddPt(ptNew);
        }
        rect = newLwPolyline.GetBoundBox();
        break;
    }
    case GEO_TYPE_ELLIPSE:
    {
        CGeoEllipse* pEllipse = static_cast<CGeoEllipse* >(child);
        FLOAT_T width  = mat.GetScale() * pEllipse->GetWidth();
        FLOAT_T height = mat.GetScale() * pEllipse->GetHeight();
        CMatrix rotateTranslateMaT;
        mat.GetRotateAndTranslate(rotateTranslateMaT);
        CGeoPt center = rotateTranslateMaT * pEllipse->GetCenterPt(); // 圆心
        FLOAT_T angle = mat.GetAngle();  // 弧度

        CGeoRect   FrameRect ;
        FLOAT_T   a = width;                         // 椭圆长轴一半
        FLOAT_T   b = height;                        // 椭圆短轴一半
        FLOAT_T angleXT = atan((-b * tan(angle))/a);
        FLOAT_T xFrame1 = center.dx + a * cos(angleXT) * cos(angle) - b * sin(angleXT) *sin(angle);
        angleXT += M_PI;
        FLOAT_T xFrame2 = center.dx + a * cos(angleXT) * cos(angle) - b * sin(angleXT) *sin(angle);
        if(xFrame1 > xFrame2){
            FrameRect.m_dLeft  = xFrame2;
            FrameRect.m_dRight = xFrame1;
        }else{
            FrameRect.m_dLeft  = xFrame1;
            FrameRect.m_dRight = xFrame2;
        }

        FLOAT_T angleYT = atan(b/(a * tan(angle)));
        FLOAT_T yFrame1 = center.dy + b * sin(angleYT) * cos(angle) + a * cos(angleYT) *sin(angle);
        angleYT += M_PI;
        FLOAT_T yFrame2 = center.dy + b * sin(angleYT) * cos(angle) + a * cos(angleYT) *sin(angle);
        if(yFrame1 > yFrame2){
            FrameRect.m_dBottom  = yFrame1;
            FrameRect.m_dTop     = yFrame2;
        }else{
            FrameRect.m_dBottom  = yFrame2;
            FrameRect.m_dTop     = yFrame1;
        }
        rect = FrameRect;
        break;
    }
    case GEO_TYPE_TEXT:
    {
        CGeoRect   FrameRect = child->GetBoundBox();
        CGeoPt leftTop(FrameRect.m_dLeft, FrameRect.m_dTop);
        CGeoPt rightBottom(FrameRect.m_dRight, FrameRect.m_dBottom);
        CGeoPt rightTop(FrameRect.m_dRight, FrameRect.m_dTop);
        CGeoPt leftBottom(FrameRect.m_dLeft, FrameRect.m_dBottom);

        leftTop = (mat) * leftTop;
        rightBottom = (mat) * rightBottom;
        rightTop = (mat) * rightTop;
        leftBottom = (mat) * leftBottom;

        FrameRect.m_dLeft = std::min(std::min(leftTop.dx, rightBottom.dx), std::min(rightTop.dx, leftBottom.dx));
        FrameRect.m_dTop = std::min(std::min(leftTop.dy, rightBottom.dy), std::min(rightTop.dy, leftBottom.dy));
        FrameRect.m_dRight = std::max(std::max(leftTop.dx, rightBottom.dx), std::max(rightTop.dx, leftBottom.dx));
        FrameRect.m_dBottom = std::max(std::max(leftTop.dy, rightBottom.dy), std::max(rightTop.dy, leftBottom.dy));

        rect = FrameRect;
        break;
    }
    case GEO_TYPE_RECT:
    {
        CGeoRect   FrameRect = child->GetBoundBox();
        CGeoPt leftTop(FrameRect.m_dLeft, FrameRect.m_dTop);
        CGeoPt rightBottom(FrameRect.m_dRight, FrameRect.m_dBottom);
        CGeoPt rightTop(FrameRect.m_dRight, FrameRect.m_dTop);
        CGeoPt leftBottom(FrameRect.m_dLeft, FrameRect.m_dBottom);

        leftTop = (mat) * leftTop;
        rightBottom = (mat) * rightBottom;
        rightTop = (mat) * rightTop;
        leftBottom = (mat) * leftBottom;

        FrameRect.m_dLeft = std::min(std::min(leftTop.dx, rightBottom.dx), std::min(rightTop.dx, leftBottom.dx));
        FrameRect.m_dTop = std::min(std::min(leftTop.dy, rightBottom.dy), std::min(rightTop.dy, leftBottom.dy));
        FrameRect.m_dRight = std::max(std::max(leftTop.dx, rightBottom.dx), std::max(rightTop.dx, leftBottom.dx));
        FrameRect.m_dBottom = std::max(std::max(leftTop.dy, rightBottom.dy), std::max(rightTop.dy, leftBottom.dy));
        rect = FrameRect;
        break;
    }
    default://到这里应该是不知道的类型
        break;
    }

    rect.m_dTop = windowFrame.bottom() + windowFrame.top() - rect.m_dTop;
    rect.m_dBottom = windowFrame.bottom() + windowFrame.top() - rect.m_dBottom;
    std::swap(rect.m_dTop, rect.m_dBottom);

    if(rect.m_dLeft==0 && rect.m_dTop == 0 && rect.m_dRight == 0 && rect.m_dBottom == 0)
        return;
    else{
        int itemLeftTopX     = std::floor((rect.m_dLeft    - windowFrame.x())/idWidth);
        int itemLeftTopY     = std::floor((rect.m_dTop     - windowFrame.y())/idHeight);
        int itemRightBottomX = std::floor((rect.m_dRight   - windowFrame.x())/idWidth);
        int itemRightBottomY = std::floor((rect.m_dBottom  - windowFrame.y())/idHeight);

        for(int i = itemLeftTopX; i <= itemRightBottomX; ++i )
            for(int j = itemLeftTopY; j <= itemRightBottomY; ++j )
            {
                unsigned int itemID = ((i<<16) + j);
                m_mapItem.insert(std::pair<unsigned int, ref_ptr<CGeoBase> >(itemID, pData));
            }
    }
}

void COptimaskView::SetGeoStructureMap(CGeoGDSStruct *pData)
{
    if(pData == 0){
        QMessageBox::information(NULL, tr("Warning"),tr("No Map Data"));
        return;
    }
    CMatrix mat;
    for (unsigned i = 0; i < pData->GetChildCount(); ++i)
    {
        CGeoBase *geoChild = pData->GetChild(i);
        ref_ptr<CGeoTransform> transform(new CGeoTransform());
        transform->SetTransformMatrix(mat);
        transform->SetChild(geoChild);
        SetGeoTransformMap(transform);
    }

}

inline void COptimaskView::SetRow(int row)
{
    m_nrow = row;
}

inline void  COptimaskView::SetColumn(int column)
{
    m_ncolumn = column;
}

void  COptimaskView::SetHeight(const CGeoRect& frame)
{
    if(frame.m_dBottom - frame.m_dTop  > 0){
        idHeight = std::ceil((std::ceil(frame.m_dBottom) - std::floor(frame.m_dTop))/m_nrow);
    }else{
        idHeight = std::ceil((std::ceil(frame.m_dTop) - std::floor(frame.m_dBottom))/m_nrow);
    }
}

void COptimaskView::SetWidth(const CGeoRect& frame)
{
    if(frame.m_dRight - frame.m_dLeft  > 0){
        idWidth = std::ceil((std::ceil(frame.m_dRight) - std::floor(frame.m_dLeft))/m_ncolumn);
    }else{
        idWidth = std::ceil((std::ceil(frame.m_dLeft) - std::floor(frame.m_dRight))/m_ncolumn);
    }
}

unsigned int COptimaskView::GetID(QPoint point)
{
    QPointF pointf = mapToWindow(point);//转换成逻辑坐标
    unsigned int nx = std::floor((pointf.x() - std::floor(m_dboriginMapItemX))/idWidth);
    unsigned int ny = std::floor((pointf.y() - std::floor(m_dboriginMapItemY))/idHeight);
    return ((nx<<16) + ny);
}

bool COptimaskView::RectContainPoint(const ref_ptr<CGeoRectangle> rect, const QPoint &point)
{
    double left= rect->GetTopLeftPt().dx;
    double top = rect->GetTopLeftPt().dy;
    double right = rect->GetRightBottomPt().dx;
    double bottom = rect->GetRightBottomPt().dy;
    if(left > right) {std::swap(left, right);} //保证right 大于左边
    if(top > bottom) {std::swap(top, bottom);}
    return (point.x() >= left && point.x() <= right && point.y() >= top && point.y() <= bottom);
}

bool COptimaskView::LWPolyContainPoint(ref_ptr<CGeoLWPolyline> LWPolyline, const QPoint &point)
{
    FLOAT_T x = static_cast<FLOAT_T>(point.x());
    FLOAT_T y = static_cast<FLOAT_T>(point.y());

    std::vector<CGeoPt> ptPolygon = LWPolyline->GetPtList();
    unsigned int poly_sides =  ptPolygon.size();
    std::vector<double> poly_X;
    std::vector<double> poly_Y;
    for(std::vector<CGeoPt>::iterator itr = ptPolygon.begin(); itr != ptPolygon.end(); ++itr){
        CGeoPt pt = (*itr);
        poly_X.push_back(pt.dx);
        poly_Y.push_back(pt.dy);
    }

    unsigned int i =0;
    unsigned int j =0;

    if(LWPolyline->IsClosed() == true){ //多边形
        j = poly_sides-1;
        int res = 0;
        for (i = 0; i<poly_sides; i++)   // 遍历每一条边
        {
            if(((poly_Y[i]<y && poly_Y[j]>=y) || (poly_Y[j]<y && poly_Y[i]>=y)) && (poly_X[i]<=x || poly_X[j]<=x))  //前半部分是y值在两个端点之间，后半部分是高效的做法
            {
                res ^= ((poly_X[i] + (y-poly_Y[i])/(poly_Y[j]-poly_Y[i])*(poly_X[j]-poly_X[i])) < x);  // 得到与线段之间的交点
            }
            j=i;
        }
        return res;
    }else{ //处理Path
        for (j = 1; j<poly_sides; i = j, j++)   // 遍历每一条边
        {
            FLOAT_T x1 = poly_X[i];
            FLOAT_T y1 = poly_Y[i];
            FLOAT_T x2 = poly_X[j];
            FLOAT_T y2 = poly_Y[j];

            FLOAT_T cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
            if (cross < 0)  continue;
            if((cross == 0) && (std::sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1)) <= FieldValueLine)){   //域值为2
                return true;
            }
            FLOAT_T d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
            if (cross > d2) continue;
            if((cross == d2) && (std::sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2)) <= FieldValueLine)){
                return true;
            }

            FLOAT_T r = cross / d2;
            FLOAT_T px = x1 + (x2 - x1) * r;
            FLOAT_T py = y1 + (y2 - y1) * r;
            if(std::sqrt((x - px) * (x - px) + (py - y1) * (py - y1)) <= FieldValueLine){
                return true;
            }else{
                continue;
            }
        }
        return false;
    }
}

bool COptimaskView::LineContainPoint(const ref_ptr<CGeoLine> &line, const QPoint &point)
{
    FLOAT_T x = static_cast<FLOAT_T>(point.x());
    FLOAT_T y = static_cast<FLOAT_T>(point.y());
    FLOAT_T x1 = line->GetFirstPt().dx;
    FLOAT_T y1 = line->GetFirstPt().dy;
    FLOAT_T x2 = line->GetSecondPt().dx;
    FLOAT_T y2 = line->GetSecondPt().dy;

    FLOAT_T cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
    if (cross < 0) return false;
    if(cross == 0)   return (std::sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1))<FieldValueLine);//域值为2

    FLOAT_T d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
    if (cross > d2) return false;
    if (cross == d2) return (std::sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2))<FieldValueLine);

    FLOAT_T r = cross / d2;
    FLOAT_T px = x1 + (x2 - x1) * r;
    FLOAT_T py = y1 + (y2 - y1) * r;
    //double value = std::sqrt((x - px) * (x - px) + (py - y1) * (py - y1));
    return (std::sqrt((x - px) * (x - px) + (py - y1) * (py - y1))<FieldValueLine);
}

bool COptimaskView::CircleContainPoint(const ref_ptr<CGeoCircle> &circle, const QPoint &point)
{
    FLOAT_T x = static_cast<FLOAT_T>(point.x());
    FLOAT_T y = static_cast<FLOAT_T>(point.y());

    FLOAT_T centerX = circle->GetCenterPt().dx;
    FLOAT_T centerY = circle->GetCenterPt().dy;

    return (std::sqrt((centerX - x)*(centerX - x)+ (centerY - y )*(centerY - y)) <= circle->GetRadius());
}

bool COptimaskView::ArcContainPoint(const ref_ptr<CGeoArc> &arc, const QPoint &point)
{
    FLOAT_T x = static_cast<FLOAT_T>(point.x());
    FLOAT_T y = static_cast<FLOAT_T>(point.y());

    FLOAT_T centerX = arc->GetCenterPt().dx;
    FLOAT_T centerY = arc->GetCenterPt().dy;

    FLOAT_T angle= std::atan2((y - centerY), (x - centerX))*180/PAI;
    angle = angle<0? (angle + 360):angle;
    if(angle >= (arc->GetAngleS()) && angle <= (arc->GetAngleE())){
        FLOAT_T distance = std::sqrt((centerX - x)*(centerX - x)+ (centerY - y )*(centerY - y));
        return ((distance<= arc->GetRadius() + FieldValueLine)&&(distance >= arc->GetRadius() - FieldValueLine));
    }else{
        return false;
    }
}

bool COptimaskView::EllipseContainPoint(const ref_ptr<CGeoEllipse> &ellipse, const QPoint &point)
{
    FLOAT_T x = static_cast<FLOAT_T>(point.x());
    FLOAT_T y = static_cast<FLOAT_T>(point.y());

    FLOAT_T halfLong = ellipse->GetWidth();
    FLOAT_T halfShort = ellipse->GetHeight();

    return ((x*x)/(halfLong*halfLong) + (y*y)/(halfShort*halfShort) <=1);
}


// 此trans中包含的子节点一定是基本图元
// 基本图元包含...
bool COptimaskView::TransformContainPoint(const ref_ptr<CGeoTransform> &trans, const QPoint &point)
{
    CMatrix   mat;
    trans->GetTransformMatrix(mat);
    CGeoBase* geoChild = trans->GetChild();

    switch (geoChild->GetObjType()) {
    case GEO_TYPE_POINT:
    {
        CGeoPoint* pt = static_cast<CGeoPoint* >(geoChild);
        CGeoPt newPt = mat * pt->GetPt();
        if(point.x() <= (multiple*2+newPt.dx) && point.x() >= (newPt.dx-multiple*2) && point.y() <= (multiple*2+newPt.dy) && point.y() >= (newPt.dy-multiple*2))
        {
            return true;
        }
        return false;
    }
    case GEO_TYPE_LINE:
    {
        CGeoLine* line = static_cast<CGeoLine* >(geoChild);
        CGeoPt newFirstPt = mat * line->GetFirstPt();
        CGeoPt newSecondPt = mat * line->GetSecondPt();
        ref_ptr<CGeoLine> newLine(new CGeoLine(newFirstPt, newSecondPt));
        return LineContainPoint(newLine, point);
    }
    case GEO_TYPE_CIRCLE:
    {
        CGeoCircle* circle = static_cast<CGeoCircle* >(geoChild);
        ref_ptr<CGeoCircle> newCircle(new CGeoCircle());
        CMatrix rotateTranslateMaT;                                         // 旋转平移矩阵
        mat.GetRotateAndTranslate(rotateTranslateMaT);
        newCircle->SetRadius(mat.GetScale() * circle->GetRadius());          // 半径只能放缩
        newCircle->SetCenterPt(rotateTranslateMaT * circle->GetCenterPt());  // 圆心旋转平移
        return CircleContainPoint(newCircle, point);

    }
    case GEO_TYPE_ARC:
    {
        CGeoArc* arc = static_cast<CGeoArc* >(geoChild);
        ref_ptr<CGeoArc> newArc(new CGeoArc());
        CMatrix rotateTranslateMaT;
        mat.GetRotateAndTranslate(rotateTranslateMaT);

        newArc->SetRadius(mat.GetScale() * arc->GetRadius());                 // 设置新的半径
        newArc->SetCenterPt(rotateTranslateMaT * arc->GetCenterPt());         // 设置新的中心点
        newArc->SetAngleS(mat.GetAngle() + arc->GetAngleS());                 // 设置新的起始角度
        newArc->SetAngleE(mat.GetAngle() + arc->GetAngleE());                 // 设置新的终止角度

        return ArcContainPoint(newArc, point);
    }
    case GEO_TYPE_LWPOLYLINE:
    {
        const CGeoLWPolyline* pPloy = static_cast<const CGeoLWPolyline*>(geoChild);
        const std::vector<CGeoPt>& ptList = pPloy->GetPtList();
        ref_ptr<CGeoLWPolyline> newLwPolyline(new CGeoLWPolyline());
        newLwPolyline->SetClose(pPloy->IsClosed());
        newLwPolyline->SetFill(pPloy->IsFill());
        for (unsigned i = 0;i < ptList.size();i++)
        {
            const CGeoPt& pt = ptList.at(i);
            CGeoPt ptNew = mat*pt;
            newLwPolyline->AddPt(ptNew);
        }

        return LWPolyContainPoint(newLwPolyline, point);
    }

    case GEO_TYPE_ELLIPSE:
    {
        CGeoEllipse* pEllipse = static_cast<CGeoEllipse* >(geoChild);
        FLOAT_T width  = mat.GetScale() * pEllipse->GetWidth();
        FLOAT_T height = mat.GetScale() * pEllipse->GetHeight();
        CMatrix rotateTranslateMaT;
        mat.GetRotateAndTranslate(rotateTranslateMaT);
        CGeoPt center = rotateTranslateMaT * pEllipse->GetCenterPt(); // 圆心
        FLOAT_T angle = mat.GetAngle();  // 弧度

        double calu1 = ((point.x() - center.dx)*cos(angle) - (point.y() - center.dy)*sin(angle)) *((point.x() - center.dx)*cos(angle) - (point.y() - center.dy)*sin(angle)) / (width * width);
        double calu2 = ((point.x() - center.dx)*sin(angle) + (point.y() - center.dy)*cos(angle)) *((point.x() - center.dx)*sin(angle) + (point.y() - center.dy)*cos(angle)) / (height * height);
        return ((calu1 +calu2) <=1);
    }
    case GEO_TYPE_TEXT:
    case GEO_TYPE_RECT:
    {
        ref_ptr<CGeoLWPolyline> lwPolyline(new CGeoLWPolyline());
        lwPolyline->SetClose(true);

        CGeoRect   FrameRect = geoChild->GetBoundBox();
        CGeoPt leftTop(FrameRect.m_dLeft, FrameRect.m_dTop);
        CGeoPt rightBottom(FrameRect.m_dRight, FrameRect.m_dBottom);
        CGeoPt rightTop(FrameRect.m_dRight, FrameRect.m_dTop);
        CGeoPt leftBottom(FrameRect.m_dLeft, FrameRect.m_dBottom);

        lwPolyline->AddPt(mat * leftTop);
        lwPolyline->AddPt(mat * rightBottom);
        lwPolyline->AddPt(mat * rightTop);
        lwPolyline->AddPt(mat * leftBottom);

        return LWPolyContainPoint(lwPolyline, point);
    }
    case GEO_TYPE_POLYGON:
    {
        return false;
        // 数据中还没有设置这一类型
    }
    default:
        break;
    }
    return false;
}

FLOAT_T COptimaskView::FrameArea(CGeoRect rect) const
{
    return std::fabs(rect.Height()*rect.Width());
}

void COptimaskView::GetAreaMinItem(CGeoBase *base, std::list<ref_ptr<CGeoBase> >& list, FLOAT_T &minFrameArea) const
{
    FLOAT_T boxArea = FrameArea(base->GetBoundBox());
    if(boxArea < minFrameArea){
        minFrameArea = boxArea;
        if(!list.empty()){
            list.clear();
            list.push_back(base);
        }
    }
    if(boxArea == minFrameArea){
        list.push_back(base);
    }
}

void COptimaskView::UpdateSceneFrame(ref_ptr<CGeoBase> base)
{
    CGeoRect itemframe = base->GetBoundBox();
    // 更新边框
    m_sceneFrame.m_dLeft = (m_sceneFrame.m_dLeft < itemframe.m_dLeft) ? m_sceneFrame.m_dLeft:itemframe.m_dLeft;
    m_sceneFrame.m_dRight = (m_sceneFrame.m_dRight > itemframe.m_dRight)?m_sceneFrame.m_dRight:itemframe.m_dRight;
    m_sceneFrame.m_dTop = (m_sceneFrame.m_dTop < itemframe.m_dTop)?m_sceneFrame.m_dTop:itemframe.m_dTop;
    m_sceneFrame.m_dBottom = (m_sceneFrame.m_dBottom > itemframe.m_dBottom)?m_sceneFrame.m_dBottom:itemframe.m_dBottom;
}

void COptimaskView::SetItemMap(ref_ptr<CGeoBase> base)
{
    UpdateSceneFrame(base);
    CGeoRect itemframe = base->GetBoundBox();
    int itemLeftTopX = std::floor((itemframe.m_dLeft - std::floor(m_dboriginMapItemX))/idWidth);
    int itemLeftTopY = std::floor((itemframe.m_dTop  - std::floor(m_dboriginMapItemY))/idHeight);
    int itemRightBottomX = std::floor((itemframe.m_dRight  - std::floor(m_dboriginMapItemX))/idWidth);
    int itemRightBottomY = std::floor((itemframe.m_dBottom  - std::floor(m_dboriginMapItemY))/idHeight);
    for(int i = itemLeftTopX; i <= itemRightBottomX; ++i )
        for(int j = itemLeftTopY; j <= itemRightBottomY; ++j )
        {
            unsigned int itemID = ((i<<16) + j);
            m_mapItem.insert(std::pair<unsigned int, ref_ptr<CGeoBase> >(itemID, base));
        }

    QPainter pp(&pix);
    pp.save();
    //pp.setPen(Qt::DotLine);
    pp.drawRect(m_sceneFrame.m_dLeft, m_sceneFrame.m_dTop, m_sceneFrame.Width(), m_sceneFrame.Height());
    pp.restore();
    this->viewport()->update();
}

void COptimaskView::setCurrentItem(ref_ptr<CGeoBase> item)
{
    m_rCurrentItem = item;
}

ref_ptr<CGeoBase> COptimaskView::GetSelectItem()
{
    return m_rCurrentItem;
}

void COptimaskView::SetPen(QPen pen)
{
    m_pen.setColor(pen.color()) ;
}

QPointF COptimaskView::mapToWindow(const QPoint point)
{
    FLOAT_T x = m_sceneFrame.m_dLeft + (point.x() - setViewPort().left())*m_sceneFrame.Width()/setViewPort().width();
    FLOAT_T y = m_sceneFrame.m_dTop  + (point.y() - setViewPort().top())*m_sceneFrame.Height()/setViewPort().height();
    return QPointF(x, y);
}

QPointF COptimaskView::mapToWindow(const QPointF point)
{
    FLOAT_T x = m_sceneFrame.m_dLeft + (point.x() - setViewPort().left())*m_sceneFrame.Width()/setViewPort().width();
    FLOAT_T y = m_sceneFrame.m_dTop  + (point.y() - setViewPort().top())*m_sceneFrame.Height()/setViewPort().height();
    return QPointF(x, y);
}

QPointF COptimaskView::mapToWindow(const int dx, const int dy)
{
    FLOAT_T x = m_sceneFrame.m_dLeft + ( dx - setViewPort().left())*m_sceneFrame.Width()/setViewPort().width();
    FLOAT_T y = m_sceneFrame.m_dTop  + ( dy - setViewPort().top())*m_sceneFrame.Height()/setViewPort().height();
    return QPointF(x, y);
}

QPointF COptimaskView::mapToWindow(const double dx, const double dy)
{
     FLOAT_T x = m_sceneFrame.m_dLeft + ( dx - setViewPort().left())*m_sceneFrame.Width()/setViewPort().width();
     FLOAT_T y = m_sceneFrame.m_dTop  + ( dy - setViewPort().top())*m_sceneFrame.Height()/setViewPort().height();
     return QPointF(x, y);
}

QPointF COptimaskView::mapFromWindow(const QPointF point)
{

    FLOAT_T x = setViewPort().left() + (point.x() - m_sceneFrame.m_dLeft)*setViewPort().width()/m_sceneFrame.Width();
    FLOAT_T y = setViewPort().top()  + (point.y() - m_sceneFrame.m_dTop)*setViewPort().height()/m_sceneFrame.Height();
    return QPointF(x, y);
}

QPointF COptimaskView::mapFromWindow(double dx, double dy)
{
    FLOAT_T x = setViewPort().left() + ( dx - m_sceneFrame.m_dLeft)*setViewPort().width()/m_sceneFrame.Width();
    FLOAT_T y = setViewPort().top()  + ( dy - m_sceneFrame.m_dTop)*setViewPort().height()/m_sceneFrame.Height();
    return QPointF(x, y);
}

QRect COptimaskView::setViewPort()
{
    int viewWidth;
    int viewHeight;
    if(m_sceneFrame.Height() >= m_sceneFrame.Width()){
        viewWidth  = std::ceil(0.9*static_cast<double>(this->viewport()->height())*m_sceneFrame.Width()/m_sceneFrame.Height());
        viewHeight = std::ceil(0.9*this->viewport()->height());
        viewFrame = QRect(std::floor(this->viewport()->width()/2 - viewWidth/2), std::floor(0.05*this->viewport()->height()),viewWidth, viewHeight);
    }else{
        viewHeight = std::ceil(0.9*this->viewport()->width()*m_sceneFrame.Height()/m_sceneFrame.Width());
        viewWidth  = std::ceil(0.9*this->viewport()->width());
        viewFrame = QRect(std::floor(0.05*this->viewport()->width()), std::floor(this->viewport()->height()/2 - viewHeight/2), viewWidth, viewHeight);
    }

    multiple   = std::max(m_sceneFrame.Height()/viewHeight, m_sceneFrame.Width()/viewWidth);
    if(multiple < 1.0)
        multiple = 1.0;
    // 修正m_sceneFrame的宽高比与setViewPort一样,都为viewWidth/viewHeight。
    m_sceneFrame.m_dLeft = std::floor(m_sceneFrame.m_dLeft);
    m_sceneFrame.m_dTop  = std::floor(m_sceneFrame.m_dTop);
    m_sceneFrame.m_dRight = m_sceneFrame.m_dLeft + multiple * viewWidth;
    m_sceneFrame.m_dBottom = m_sceneFrame.m_dTop + multiple * viewHeight;
    windowFrame = QRect(m_sceneFrame.m_dLeft, m_sceneFrame.m_dTop, std::ceil(multiple * viewWidth), std::ceil(multiple * viewHeight));

    return viewFrame;
}

void COptimaskView::DrawScene()
{
    if (!m_CurScene.valid() && m_CurScene->GetLayerNum() == 0)
        return;
    //遍历所有图层中的所有图元，然后分别画出图元
    std::vector<ref_ptr<CGeoLayer> > layerList = m_CurScene->GetLayerList();
    for(std::vector<ref_ptr<CGeoLayer> >::iterator iterLayer = layerList.begin(); iterLayer != layerList.end(); ++iterLayer){
        ref_ptr<CGeoLayer> layer = *iterLayer;
        GeoObjList objList = layer->GetObjList();
        for(std::vector<ref_ptr<CGeoBase> >::iterator iterObj = objList.begin(); iterObj != objList.end(); ++iterObj){
            CGeoBase* pData = (*iterObj).get();
            DrawBasePrimitive(pData);
        }
    }
}

void COptimaskView::paintEvent(QPaintEvent *event)
{
    setViewPort();
    QPainter painter(this->viewport());
    painter.drawPixmap(0, 0, pix);
    //----------比例尺-----------------------------
    DrawRuler();

    // 中心窗口中的用鼠标画图元事件
    //QRegion paintRegion = event->region();  //Returns the region that needs to be updated
    if(m_bDrawEvent == true){
        DrawWithMouse(m_shapeType);
    }

    QGraphicsView::paintEvent(event);       // 保证基本的绘图
}

void COptimaskView::mousePressEvent(QMouseEvent *event)
{
    //在中心窗口的画图元事件
    if(m_bDrawEvent == true){
        if (event->button() == Qt::LeftButton)
        {
            this->setMouseTracking(true);
            setCursor(Qt::CrossCursor);
            switch (m_shapeType) {
            case GEO_TYPE_LINE:
            case GEO_TYPE_RECT:
            case GEO_TYPE_CIRCLE:
            case GEO_TYPE_ELLIPSE:
                if(m_bFirstPoint){
                    m_bFirstPoint = false;
                    m_startPos = event->pos();
                    m_lasetPos = event->pos();
                    m_endPos   = event->pos();
                    m_geoStartPos = QPointFToGeoPt(m_startPos);
                    m_geoLasetPos = QPointFToGeoPt(m_lasetPos);
                    m_geoEndPos = QPointFToGeoPt(m_endPos);
                    isDrawing = true;
                }else{
                    m_bFirstPoint = true;
                    setMouseTracking(false);
                    setCursor(Qt::ArrowCursor);
                    m_endPos = event->pos();
                    m_geoEndPos = QPointFToGeoPt(m_endPos);
                    if(m_shapeType == GEO_TYPE_LINE){
                        CGeoLine *pLine = new CGeoLine(m_geoStartPos, m_geoEndPos);
                        CDrawLineWithMouseCommand *drawLineCommand = new CDrawLineWithMouseCommand(this,pLine);
                        CALLCOMMAND(drawLineCommand);
                        SetItemMap(pLine);
                        AddObjToScene(pLine);
                    }
                    if(m_shapeType == GEO_TYPE_RECT){
                        CGeoRectangle *pRect = new CGeoRectangle(m_geoStartPos, m_geoEndPos);
                        CDrawRectWithMouseCommand *drawRectCommand = new CDrawRectWithMouseCommand(this,pRect);
                        CALLCOMMAND(drawRectCommand);
                        SetItemMap(pRect);
                        if(m_CurScene !=0){//已经打开了DXF的文件的处理方法
                            AddObjToScene(pRect);
                        }else{}
                    }

                    if(m_shapeType == GEO_TYPE_ELLIPSE){
                        CGeoEllipse *pEllipse = new CGeoEllipse(m_geoStartPos, m_geoEndPos);
                        CDrawEllipseWithMouseCommand *drawEllipseCommand = new CDrawEllipseWithMouseCommand(this,pEllipse);
                        CALLCOMMAND(drawEllipseCommand);
                        SetItemMap(pEllipse);

                        if(m_CurScene !=0){//已经打开了DXF的文件的处理方法
                            AddObjToScene(pEllipse);
                        }else{}                            //还没有打开任务DXF的文件的处理方法
                    }
                    if(m_shapeType ==GEO_TYPE_CIRCLE){
                        CGeoCircle *pCircle = new CGeoCircle(m_geoStartPos, distance(m_startPos, m_endPos));
                        CDrawCircleWithMouseCommand *drawCircleCommand = new CDrawCircleWithMouseCommand(this, pCircle);
                        CALLCOMMAND(drawCircleCommand);
                        SetItemMap(pCircle);

                        if(m_CurScene !=0){//已经打开了DXF的文件的处理方法
                            AddObjToScene(pCircle);
                        }else{}//还没有打开任务DXF的文件的处理方法
                    }
                    isDoubleClick = false;
                    isDrawing = false;
                }
                break;
            case GEO_TYPE_POLYGON :
                if(m_bFirstPoint == true){
                    m_bFirstPoint  = false;
                    isDrawing = true;
                    m_nNumPoint = 1;
                    m_startPos = event->pos();
                    m_lasetPos = event->pos();
                    m_endPos   = event->pos();
                    m_vecPoint.push_back(m_startPos);
                    m_allPoint.push_back(QPointFToGeoPt(m_startPos));
                }else{
                    QPainter pp(&pix);
                    ++m_nNumPoint;
                    m_endPos   = event->pos();
                    pp.drawLine(m_lasetPos,m_endPos);
                    m_lasetPos = m_endPos;
                    m_vecPoint.push_back(m_lasetPos);
                    m_allPoint.push_back(QPointFToGeoPt(m_lasetPos));
                }
                isDoubleClick = false;
                break;
            case GEO_TYPE_PATH :
                if(m_bFirstPoint == true){
                    m_bFirstPoint  = false;
                    isDrawing = true;
                    m_nNumPoint = 1;
                    m_startPos = event->pos();
                    m_lasetPos = event->pos();
                    m_endPos   = event->pos();
                    m_geoStartPos = QPointFToGeoPt(m_startPos);
                    m_geoLasetPos = QPointFToGeoPt(m_lasetPos);
                    m_geoEndPos = QPointFToGeoPt(m_endPos);

                    m_vecPoint.push_back(m_startPos);
                    m_allPoint.push_back(QPointFToGeoPt(m_startPos));
                }else{
                    ++m_nNumPoint;
                    m_endPos   = event->pos();
                    m_geoEndPos = QPointFToGeoPt(m_endPos);
                    CGeoLine *pLine = new CGeoLine(m_geoLasetPos, m_geoEndPos);
                    CDrawLineWithMouseCommand *drawLineCommand = new CDrawLineWithMouseCommand(this,pLine);
                    CALLCOMMAND(drawLineCommand);
                    m_lasetPos = m_endPos;
                    m_geoLasetPos = QPointFToGeoPt(m_lasetPos);
                    m_vecPoint.push_back(m_lasetPos);
                    m_allPoint.push_back(m_geoLasetPos);
                }
                isDoubleClick = false;
                break;
            case GEO_TYPE_ARC:
                if(m_bFirstPoint){
                    m_bFirstPoint = false;
                    m_startPos = event->pos();
                    m_lasetPos = event->pos();
                    m_endPos   = event->pos();
                    isDrawing = true;
                    m_nNumPoint = 1;
                }else{

                    ++m_nNumPoint;
                    if(m_nNumPoint == 2){
                        QPainter pp(&pix);
                        m_lasetPos = event->pos();
                        m_endPos = event->pos();
                        int radius = distance(m_startPos, m_lasetPos);
                        pp.save();
                        pp.setPen(Qt::DotLine);
                        pp.drawLine(m_startPos, m_lasetPos);
                        pp.drawEllipse(m_startPos, radius, radius);
                        pp.restore();
                        //isDrawing = false;
                        //this->viewport()->update();
                        //isDrawing = true;
                    }
                    if(m_nNumPoint == 3){
                        m_endPos == event->pos();
                        m_bFirstPoint = true;

                        int radiusStart = distance(m_startPos, m_lasetPos);

                        QRect rect(m_startPos.x() - radiusStart, m_startPos.y() - radiusStart, 2*radiusStart, 2*radiusStart);
                        double yValueStart = static_cast<double>(m_lasetPos.y() - m_startPos.y());
                        double xValueStart = static_cast<double>(m_lasetPos.x() - m_startPos.x());
                        double yValueEnd  = static_cast<double>(m_endPos.y() - m_startPos.y());
                        double xValueEnd = static_cast<double>(m_endPos.x() - m_startPos.x());

                        if(xValueStart>=0){
                            if(xValueStart > 0){
                                if(yValueStart>=0){
                                    startAngle = std::asin(yValueStart/radiusStart)*180/PAI;          // 第一象限
                                }else{
                                    startAngle = 360 - std::asin(-yValueStart/radiusStart)*180/PAI;// 第四象限
                                }
                            }else{
                                if(yValueStart>0){
                                    startAngle = 90;      // y轴正半轴
                                }else if(yValueStart<0){
                                    startAngle = 270;    // y轴负半轴
                                }else{
                                    QMessageBox::information(NULL, tr("Warning"),tr("Two point overlap"));
                                    return;
                                }
                            }
                        }else{
                            if(yValueStart>=0){
                                startAngle = 180 - std::asin(yValueStart/radiusStart)*180/PAI;        // 第二象限
                            }else{
                                startAngle = 180 + std::asin(-yValueStart/radiusStart)*180/PAI;      // 第三象限
                            }
                        }

                        int radiusEnd = distance(m_startPos, m_endPos);
                        if(xValueEnd>=0){
                            if(xValueEnd > 0){
                                if(yValueEnd>=0){
                                    endAngle = std::asin(yValueEnd/radiusEnd)*180/PAI;          // 第一象限或x正半轴
                                }else{
                                    endAngle = 360 - std::asin(-yValueEnd/radiusEnd)*180/PAI;// 第四象限
                                }
                            }else{
                                if(yValueEnd>0){
                                    endAngle = 90;      // y轴正半轴
                                }else if(yValueEnd<0){
                                    endAngle = 270;    // y轴负半轴
                                }else{
                                    QMessageBox::information(NULL, tr("Warning"),tr("Two point overlap"));
                                    return;
                                }
                            }
                        }else{
                            if(yValueEnd>=0){
                                endAngle = 180 - std::asin(yValueEnd/radiusEnd)*180/PAI;        // 第二象限或x负半轴
                            }else{
                                endAngle = 180 + std::asin(-yValueEnd/radiusEnd)*180/PAI;      // 第三象限
                            }
                        }

                        QPainter *pp = new QPainter;
                        pp->begin(&pix);
                        //pp.save();
                        QPen pen(Qt::SolidLine);
                        pen.setColor(Qt::white);
                        pen.setWidth(2);
                        pp->setPen(pen);

                        pp->drawEllipse(m_startPos, radiusStart, radiusStart);
                        pp->drawLine(m_startPos, m_lasetPos);
                        //pp.restore();
                        pp->end();


                        CGeoArc *pArc = new CGeoArc(QPointFToGeoPt(m_startPos),distance(m_startPos, m_endPos), startAngle, endAngle);
                        CDrawArcWithMouseCommand *drawArcCommand = new CDrawArcWithMouseCommand(this,pArc);
                        CALLCOMMAND(drawArcCommand);
                        SetItemMap(pArc);
                        if(m_CurScene !=0){//已经打开了DXF的文件的处理方法
                            AddObjToScene(pArc);
                            //delete pLine;
                        }else{}//还没有打开任务DXF的文件的处理方法

                        isDrawing = false;

                        setMouseTracking(false);
                        setCursor(Qt::ArrowCursor);
                    }
                    isDoubleClick = false;
                }
                break;
            default:
                break;
            }
        }
    }
    // 中心窗口的选中图元拖动事件
    if(m_bSelectEvent == true){
        event->accept();
        typedef std::multimap<unsigned int , ref_ptr<CGeoBase> >::iterator MultiMapIterator;
        std::pair<MultiMapIterator, MultiMapIterator> iterPair = m_mapItem.equal_range(GetID(event->pos()));
        FLOAT_T minFrameArea = std::numeric_limits<FLOAT_T>::max();

        QPoint eventpos = event->pos();
        QPointF LogPos = mapToWindow(eventpos);

        LogPos.setY(-LogPos.y() + windowFrame.top() + windowFrame.bottom());
        std::list<ref_ptr<CGeoBase> > list;
        for (MultiMapIterator it = iterPair.first; it != iterPair.second; ++it)
        {
            CGeoTransform* trans = static_cast<CGeoTransform*>(((*it).second).get());
            if(trans != 0){
                if(TransformContainPoint(trans, LogPos.toPoint())){
                     GetAreaMinItem((*it).second.get(), list, minFrameArea);
                }
            }
            switch((*it).second->GetObjType()){
            case GEO_TYPE_LWPOLYLINE:
                if(LWPolyContainPoint( static_cast<CGeoLWPolyline*>((*it).second.get()), LogPos.toPoint())){
                    GetAreaMinItem((*it).second.get(), list, minFrameArea);
                }
                break;
            case GEO_TYPE_LINE:
                if(LineContainPoint( static_cast<CGeoLine*>((*it).second.get()), LogPos.toPoint())){
                    GetAreaMinItem((*it).second.get(), list, minFrameArea);
                }
                break;
            case GEO_TYPE_CIRCLE:
                if(CircleContainPoint( static_cast<CGeoCircle*>((*it).second.get()), LogPos.toPoint())){
                    GetAreaMinItem((*it).second.get(), list, minFrameArea);
                }
                break;
            case GEO_TYPE_ARC:
                if(ArcContainPoint( static_cast<CGeoArc*>((*it).second.get()), LogPos.toPoint())){
                    GetAreaMinItem((*it).second.get(), list, minFrameArea);
                }
                break;
            case GEO_TYPE_RECT:
                if(RectContainPoint( static_cast<CGeoRectangle*>((*it).second.get()), LogPos.toPoint())){
                    GetAreaMinItem((*it).second.get(), list, minFrameArea);
                }
                break;
            case GEO_TYPE_ELLIPSE:
                if(EllipseContainPoint( static_cast<CGeoEllipse*>((*it).second.get()), LogPos.toPoint())){
                    GetAreaMinItem((*it).second.get(), list, minFrameArea);
                }
            case GEO_TYPE_OTHER:
                if(TransformContainPoint( static_cast<CGeoTransform*>((*it).second.get()), LogPos.toPoint())){
                    GetAreaMinItem((*it).second.get(), list, minFrameArea);
                }
                break;
            default:
                break;
            }
        }
        if(list.size() ==1){// 如果list中只有一个元素，直接画出
            CGeoBase* pData = list.back().get();
            if(m_rCurrentItem.get() != NULL){       // 先前有选中的图元
                if(pData != m_rCurrentItem.get()) { // 当前选中的图元不为先前选中的图元
                    m_pen.setColor(Qt::white);       // 先前选中图元的边框消失
                    DrawItemFrame(m_rCurrentItem.get());

                    m_pen.setStyle(Qt::SolidLine);  // 先前选中的图元还原
                    m_pen.setColor(Qt::black);
                    DrawBasePrimitive(m_rCurrentItem.get());

                    m_pen.setStyle(Qt::DotLine);    // 新选中的图元画上虚线以表示选中
                    m_pen.setColor(Qt::red);
                    DrawBasePrimitive(pData);
                    DrawItemFrame(pData);           // 画新选中图元的边框
                    m_rCurrentItem = pData;
                }
            }else{                                  // 先前没有选中图元时，直接在新选中的图元上画上虚线以表示选中
                m_pen.setStyle(Qt::DotLine);
                m_pen.setColor(Qt::red);
                DrawBasePrimitive(pData);
                DrawItemFrame(pData);               // 画新选中图元的边框
                m_rCurrentItem = pData;
            }
        }
        if(list.size() >1){
            for(std::list<ref_ptr<CGeoBase> >::iterator iter = list.begin(); iter != list.end(); ++iter){
                //如果有多个元素，选择当前层的元素。
                //因为当前层还没有设置好，暂时还无法判断
                CGeoBase* pData = (*iter).get();
                //m_pen.setStyle(Qt::DotLine);
                m_pen.setColor(Qt::red);
                DrawBasePrimitive(pData);
            }
        }
    }
    //------------平移------------------------
    if(cursor().shape() == Qt::OpenHandCursor){
        setCursor(Qt::ClosedHandCursor);
        if (event->button() == m_translateButton) {
            // 当光标底下没有 item 时，才能移动
            QPointF point = mapToScene(event->pos());
            if (scene()->itemAt(point, transform()) == NULL)  {
                m_bMouseTranslate = true;
                m_lastMousePos = event->pos();
            }
        }
    }


    QGraphicsView::mousePressEvent(event);
}

void COptimaskView::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    m_mousePressIndex = -1;
    //--------------平移--------------------------
    if(cursor().shape() == Qt::ClosedHandCursor){
        setCursor(Qt::OpenHandCursor);
        if (event->button() == m_translateButton)
            m_bMouseTranslate = false;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void COptimaskView::mouseMoveEvent(QMouseEvent *event)
{
    // 在中心窗口的画图元事件
    if(m_bDrawEvent == true && isDrawing == true){
        switch (m_shapeType) {
        case  GEO_TYPE_PATH:
        case  GEO_TYPE_POLYGON:
            m_endPos = event->pos();
            this->viewport()->update();
            break;
        case GEO_TYPE_LINE:
        case GEO_TYPE_RECT:
        case GEO_TYPE_CIRCLE:
        case GEO_TYPE_ELLIPSE:
        case GEO_TYPE_ARC:
            m_endPos = event->pos();
            this->viewport()->update();
            break;
        default:
            if (event->buttons() & Qt::LeftButton)
            {
                if (!isDoubleClick)
                {
                    m_endPos = event->pos();
                    this->viewport()->update();
                }
            }
            break;
        }
    }
    //-----------------------------平移----------------------------
    if(cursor().shape() == Qt::ClosedHandCursor){
        if (m_bMouseTranslate){
            QPointF mouseDelta = mapToScene(event->pos()) - mapToScene(m_lastMousePos);
            tranSlate(mouseDelta);
            // emit signalTranslate(mouseDelta);
        }
        m_lastMousePos = event->pos();
    }

    QGraphicsView::mouseMoveEvent(event);
}

void COptimaskView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        isDoubleClick = true;
        m_endPos = event->pos();
        if(m_shapeType == GEO_TYPE_POLYGON){
            if(m_nNumPoint < 3){        // 双击的时候也是一次单击。
                QMessageBox::information(NULL, tr("Error"),tr("A Polygon must have  at least 3 points"));
                return;
            }else{
                CGeoLWPolyline *pLWPolyline = new CGeoLWPolyline(m_allPoint, true);
                CDrawLWPolylineWithMouseCommand *drawPolygonCommand = new CDrawLWPolylineWithMouseCommand(this,pLWPolyline);
                CALLCOMMAND(drawPolygonCommand);
                SetItemMap(pLWPolyline);
                if(m_CurScene !=0){//已经打开了DXF的文件的处理方法
                    AddObjToScene(pLWPolyline);
                }else{}//还没有打开任务DXF的文件的处理方法
                m_vecPoint.clear();
                m_allPoint.clear();
            }
        }
        if(m_shapeType == GEO_TYPE_PATH){
            CGeoLWPolyline *pLWPolyline = new CGeoLWPolyline(m_allPoint, false);
            SetItemMap(pLWPolyline);
            if(m_CurScene !=0){//已经打开了DXF的文件的处理方法
                AddObjToScene(pLWPolyline);
            }else{}//还没有打开任务DXF的文件的处理方法
            m_vecPoint.clear();
            m_allPoint.clear();
        }
        if(m_shapeType == GEO_TYPE_ARC){
            if(m_nNumPoint < 3){        // 双击的时候也是一次单击。
                QMessageBox::information(NULL, tr("Error"),tr("at least 3 points"));
                return;
            }
        }
        setMouseTracking(false);
        isDrawing = false;
        m_bFirstPoint   = true;
        //this->viewport()->update();
        setCursor(Qt::ArrowCursor);
    }

    QGraphicsView::mouseDoubleClickEvent(event);
}

//void COptimaskView::setCurrentShape(int index)
//{
//    QString currentName;
//    if (m_currentIndex != -1)
//        this->viewport()->update();//m_shapeList.at(m_currentIndex).rect()
//    m_currentIndex = index;
//    if(m_currentIndex != -1) {
//        const ItemShape &current = m_shapeList.at(m_currentIndex);
//        this->viewport()->update();//current.rect()
//        currentName = current.name();
//    }
//    emit currentShapeChanged(currentName);
//}

//int COptimaskView::indexOf(const QString &shapeName) const
//{
//    for (int i = 0; i < m_shapeList.count(); ++i) {
//        if (m_shapeList.at(i).name() == shapeName)
//            return i;
//    }
//    return -1;
//}

//QString COptimaskView::uniqueName(const QString &name) const
//{
//    QString unique;
//    for (int i = 0; ; ++i) {
//        unique = name;
//        if (i > 0)
//            unique += QString::number(i);
//        if (indexOf(unique) == -1)
//            break;
//    }
//    return unique;
//}

int COptimaskView::distance(const QPoint &point1, const QPoint &point2)
{
    int x = point1.x() - point2.x();
    int y = point1.y() - point2.y();
    return (std::sqrt(x*x +y*y));
}

double COptimaskView::distance(const QPointF &point1, const QPointF &point2)
{
    double x = point1.x() - point2.x();
    double y = point1.y() - point2.y();
    return (std::sqrt(x*x +y*y));
}

void COptimaskView::SetDrawEvent(bool drawEvent)
{
    m_bDrawEvent = drawEvent;
}

void COptimaskView::SetSelectEvent(bool selectEvent)
{
    m_bSelectEvent = selectEvent;
}


void COptimaskView::SetViewName(const QString &viewName)
{
    m_ViewName=viewName;
}
QString COptimaskView::ViewName() const//返回画板名
{
    return m_ViewName;
}
CGdsDocument* COptimaskView::GetGdsDocument()const
{
    return m_gdsdocument;
}
//----------------缩放----------------------
// 缩放的增量
void COptimaskView::setZoomDelta(qreal delta)
{
    // 建议增量范围
    Q_ASSERT_X(delta >= 0.0 && delta <= 1.0,
               "InteractiveView::setZoomDelta", "Delta should be in range [0.0, 1.0].");
    m_zoomDelta = delta;
}

qreal COptimaskView::zoomDelta() const
{
    return m_zoomDelta;
}

void COptimaskView::DrawRuler()
{
    QPainter pp(&pix);
    // 清除旧的标尺
    pp.setPen(Qt::white);
    pp.setBrush(Qt::white);
    QString oldruler=QString::number(m_oldruler)+QString("um");
    pp.drawText(20,viewport()->height()-25,oldruler);
    pp.drawRect(QRect(20,viewport()->height()-20,m_oldscale*20*2,10));

    // 画新的标尺
    pp.setPen(Qt::black);
    ruler=QString::number(m_ruler)+QString("um");
    pp.drawText(20,viewport()->height()-25,ruler);
    pp.setBrush(Qt::black);
    pp.drawRect(QRect(20,viewport()->height()-20,m_scale*20,10));
    pp.setBrush(Qt::white);
    pp.drawRect(QRect(20+m_scale*20,viewport()->height()-20,m_scale*20,10));
}

// 放大/缩小
void COptimaskView::wheelEvent(QWheelEvent *event)
{
    // 滚轮的滚动量
    QPoint scrollAmount = event->angleDelta();
    m_oldruler = m_ruler;
    // 正值表示滚轮远离使用者（放大），负值表示朝向使用者（缩小）
    scrollAmount.y() > 0 ? zoomIn() : zoomOut();
}

// 放大
void COptimaskView::zoomIn()
{
    zoom(1 + m_zoomDelta);
    m_ruler=m_ruler+5;
}

// 缩小
void COptimaskView::zoomOut()
{
    zoom(1 - m_zoomDelta);
    m_ruler=m_ruler-5;
    if(m_ruler<0)
        m_ruler=-m_ruler;
}

// 缩放 - scaleFactor：缩放的比例因子
void COptimaskView::zoom(float scaleFactor)
{
    // 防止过小或过大
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
    m_oldscale = m_scale;
    m_scale *= scaleFactor;
    if(m_scale > 3.0){
        m_scale = 1.0;
    }
    if(m_scale < 1.0){
        m_scale = 3.0;
    }
    this->viewport()->update();
}
//-----------------------平移---------------------------------------
void COptimaskView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        tranSlate(QPointF(0, -2));  // 上移
        break;
    case Qt::Key_Down:
        tranSlate(QPointF(0, 2));  // 下移
        break;
    case Qt::Key_Left:
        tranSlate(QPointF(-2, 0));  // 左移
        break;
    case Qt::Key_Right:
        tranSlate(QPointF(2, 0));  // 右移
        break;
    case Qt::Key_Plus:  // 放大
        zoomIn();
        break;
    case Qt::Key_Minus:  // 缩小
        zoomOut();
        break;
    case Qt::Key_Space:  // 逆时针旋转
        rotate(-5);
        break;
    case Qt::Key_Enter:  // 顺时针旋转
    case Qt::Key_Return:
        rotate(5);
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
    this->viewport()->update();
}

// 平移速度
void COptimaskView::setTranslateSpeed(qreal speed)
{
    // 建议速度范围
    Q_ASSERT_X(speed >= 0.0 && speed <= 2.0,
               "InteractiveView::setTranslateSpeed", "Speed should be in range [0.0, 2.0].");
    m_translateSpeed = speed;
}

qreal COptimaskView::translateSpeed() const
{
    return m_translateSpeed;
}

// 平移
void COptimaskView::tranSlate(QPointF delta)
{
    // 根据当前 zoom 缩放平移数
    delta *= m_scale;
    delta *= m_translateSpeed;

    // view 根据鼠标下的点作为锚点来定位 scene
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    QPoint newCenter(VIEW_WIDTH / 2 - delta.x(),  VIEW_HEIGHT / 2 - delta.y());
    centerOn(mapToScene(newCenter));

    // scene 在 view 的中心点作为锚点
    //delete by bamboo 2017/09/03
    /*setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    this->viewport()->update();*/
}

void COptimaskView::DrawRectWithCommand(CGeoPt begin, CGeoPt end)
{
    CGeoRectangle *pRect = new CGeoRectangle(begin,end);
    CDrawRectWithMouseCommand *drawRectCommand = new CDrawRectWithMouseCommand(this,pRect);
    CALLCOMMAND(drawRectCommand);
    if(m_CurScene !=0){//已经打开了DXF的文件的处理方法
        AddObjToScene(pRect);
    }else{}
}

void COptimaskView::DrawLineWithCommand(CGeoPt begin, CGeoPt end)
{
    CGeoLine *pLine = new CGeoLine(begin,end);
    CDrawLineWithMouseCommand *drawLineCommand = new CDrawLineWithMouseCommand(this,pLine);
    CALLCOMMAND(drawLineCommand);
    if(m_CurScene !=0){//已经打开了DXF的文件的处理方法
        AddObjToScene(pLine);
    }else{}
}

void COptimaskView::DrawPolygonWithCommand(std::vector<CGeoPt> allpoints,bool b_poAndPa)
{
    CGeoLWPolyline *pLWPolyline = new CGeoLWPolyline(allpoints, b_poAndPa);
    CDrawLWPolylineWithMouseCommand *drawPolygonCommand = new CDrawLWPolylineWithMouseCommand(this,pLWPolyline);
    CALLCOMMAND(drawPolygonCommand);
    if(m_CurScene !=0){//已经打开了DXF的文件的处理方法
        AddObjToScene(pLWPolyline);
    }else{}
}

void COptimaskView::DrawEllipseWithCommand(CGeoPt center,double axisX,double axisY )
{
    CGeoEllipse *pEllipse = new CGeoEllipse(center,axisX,axisY);
    CDrawEllipseWithMouseCommand *drawEllipseCommand = new CDrawEllipseWithMouseCommand(this,pEllipse);
    CALLCOMMAND(drawEllipseCommand);

    if(m_CurScene !=0){//已经打开了DXF的文件的处理方法
        AddObjToScene(pEllipse);
    }else{}                            //还没有打开任务DXF的文件的处理方法
}

void COptimaskView::DrawArcWithCommand(CGeoPt center, double rad,double angleS,double angleE)
{
    CGeoArc *pArc = new CGeoArc(center, rad,angleS,angleE);
    CDrawArcWithMouseCommand *drawArcCommand = new CDrawArcWithMouseCommand(this,pArc);
    CALLCOMMAND(drawArcCommand);
    if(m_CurScene !=0){//已经打开了DXF的文件的处理方法
        AddObjToScene(pArc);
    }else{}

}

bool COptimaskView::UnAction(CCommandBase* command)
{
    command->UnDraw(&pix);
    viewport()->update();
    return true; //added by bamboo 2017/09/03
}

bool COptimaskView::Action(CCommandBase *command)
{
    command->Draw(&pix);
    viewport()->update();
    return true;
}

void COptimaskView::resizeEvent(QResizeEvent* /*event*/)
{
    QRectF bounds = paintscene->itemsBoundingRect();
    this->fitInView(bounds, Qt::KeepAspectRatio);
    this->centerOn(0, 0);
    //QGraphicsView::resizeEvent(event);
}
