#include "CDrawWithMouseCommand.h"
#include "./Edit/ccommandrecevierbase.h"
#include <QPainter>

/*****CDrawLineWithMouseCommand Class***************************************/
CDrawLineWithMouseCommand::CDrawLineWithMouseCommand()
    :m_pReceiver(NULL), m_line(0){}

CDrawLineWithMouseCommand::CDrawLineWithMouseCommand(CCommandReceiverBase *receiver, CGeoLine *line)
    :m_pReceiver(receiver),m_line(line){}

CDrawLineWithMouseCommand::~CDrawLineWithMouseCommand()
{
    if (m_pReceiver)
    {
        //delete m_pReceiver;// 不能在这里删除
        m_pReceiver = NULL;
    }
}
bool CDrawLineWithMouseCommand::Execute(CCommandBase* command)
{
    m_pReceiver->Action(command);
    return true;
}

bool CDrawLineWithMouseCommand::UnExecute(CCommandBase* command)
{
    m_pReceiver->UnAction(command);
    return true;
}

void CDrawLineWithMouseCommand::Draw(QPaintDevice *device)
{
    QPainter painter(device);
    painter.setPen(Qt::black);
    painter.drawLine(m_line->GetFirstPt().dx, m_line->GetFirstPt().dy,
                     m_line->GetSecondPt().dx,m_line->GetSecondPt().dy);
}

void CDrawLineWithMouseCommand::UnDraw(QPaintDevice *device)
{
    QPainter painter(device);
    painter.setPen(Qt::white);
    painter.drawLine(m_line->GetFirstPt().dx, m_line->GetFirstPt().dy,
                     m_line->GetSecondPt().dx,m_line->GetSecondPt().dy);
}

void CDrawLineWithMouseCommand::SetReceiver(CCommandReceiverBase *pReceiver)
{
    m_pReceiver = pReceiver;
}

void CDrawLineWithMouseCommand::SetLineData(CGeoLine *line)
{
    m_line = line;
}

ref_ptr<CGeoLine> CDrawLineWithMouseCommand::GetLineData()
{
    return m_line;
}

int CDrawLineWithMouseCommand::GetType()
{
    return GEO_TYPE_LINE;
}

/*****CDrawRectWithMouseCommand Class***************************************/
CDrawRectWithMouseCommand::CDrawRectWithMouseCommand()
    :m_pReceiver(NULL), m_rect(0) {}

CDrawRectWithMouseCommand::CDrawRectWithMouseCommand(CCommandReceiverBase *receiver, CGeoRectangle *rect)
    :m_pReceiver(receiver), m_rect(rect)  {}

CDrawRectWithMouseCommand::~CDrawRectWithMouseCommand()
{
    if (m_pReceiver)
    {
        //delete m_pReceiver;// 不能在这里删除
        m_pReceiver = NULL;
    }
}
bool CDrawRectWithMouseCommand::Execute(CCommandBase* command)
{
    m_pReceiver->Action(command);
    return true;
}

bool CDrawRectWithMouseCommand::UnExecute(CCommandBase* command)
{
    m_pReceiver->UnAction(command);
    return true;
}

void CDrawRectWithMouseCommand::Draw(QPaintDevice *device)
{
    QPainter painter(device);
    painter.setPen(Qt::black);
    QPointF topLeft(m_rect->GetTopLeftPt().dx, m_rect->GetTopLeftPt().dy);
    QPointF bottomRight(m_rect->GetRightBottomPt().dx, m_rect->GetRightBottomPt().dy);
    painter.drawRect(QRectF(topLeft, bottomRight));
}

void CDrawRectWithMouseCommand::UnDraw(QPaintDevice *device)
{
    QPainter painter(device);
    painter.setPen(Qt::white);
    QPointF topLeft(m_rect->GetTopLeftPt().dx, m_rect->GetTopLeftPt().dy);
    QPointF bottomRight(m_rect->GetRightBottomPt().dx, m_rect->GetRightBottomPt().dy);
    painter.drawRect(QRectF(topLeft, bottomRight));
}

void CDrawRectWithMouseCommand::SetReceiver(CCommandReceiverBase *pReceiver)
{
    m_pReceiver = pReceiver;
}

void CDrawRectWithMouseCommand::SetRectData(ref_ptr<CGeoRectangle> rect)
{
    m_rect = rect;
}

ref_ptr<CGeoRectangle> CDrawRectWithMouseCommand::GetRectData()
{
    return m_rect;
}

int CDrawRectWithMouseCommand::GetType()
{
    return GEO_TYPE_RECT;
}

/*****CDrawEllipseWithMouseCommand Class***************************************/
CDrawEllipseWithMouseCommand::CDrawEllipseWithMouseCommand()
    :m_pReceiver(NULL), m_ellipse(0){}

CDrawEllipseWithMouseCommand::CDrawEllipseWithMouseCommand(CCommandReceiverBase *receiver, CGeoEllipse *ellipse)
    :m_pReceiver(receiver), m_ellipse(ellipse){}

CDrawEllipseWithMouseCommand::~CDrawEllipseWithMouseCommand()
{
    if (m_pReceiver)
    {
        //delete m_pReceiver;// 不能在这里删除
        m_pReceiver = NULL;
    }
}
bool CDrawEllipseWithMouseCommand::Execute(CCommandBase* command)
{
    m_pReceiver->Action(command);
    return true;
}

bool CDrawEllipseWithMouseCommand::UnExecute(CCommandBase* command)
{
    m_pReceiver->UnAction(command);
    return true;
}

void CDrawEllipseWithMouseCommand::Draw(QPaintDevice *device)
{
        QPainter painter(device);
        painter.setPen(Qt::black);
        painter.drawEllipse(QPointF(m_ellipse->GetCenterPt().dx, m_ellipse->GetCenterPt().dy),
                          m_ellipse->GetWidth(), m_ellipse->GetHeight());
}

void CDrawEllipseWithMouseCommand::UnDraw(QPaintDevice *device)
{
    QPainter painter(device);
    painter.setPen(Qt::white);
    painter.drawEllipse(QPointF(m_ellipse->GetCenterPt().dx, m_ellipse->GetCenterPt().dy),
                      m_ellipse->GetWidth(), m_ellipse->GetHeight());
}

void CDrawEllipseWithMouseCommand::SetReceiver(CCommandReceiverBase *pReceiver)
{
    m_pReceiver = pReceiver;
}

ref_ptr<CGeoEllipse> CDrawEllipseWithMouseCommand::GetEllipseData()
{
    return m_ellipse;
}

int CDrawEllipseWithMouseCommand::GetType()
{
    return GEO_TYPE_ELLIPSE;
}

/*****CDrawCircleWithMouseCommand Class***************************************/
CDrawCircleWithMouseCommand::CDrawCircleWithMouseCommand()
    :m_pReceiver(NULL), m_circle(0){}

CDrawCircleWithMouseCommand::CDrawCircleWithMouseCommand(CCommandReceiverBase *receiver, CGeoCircle *circle)
    :m_pReceiver(receiver), m_circle(circle){}

CDrawCircleWithMouseCommand::~CDrawCircleWithMouseCommand()
{
    if (m_pReceiver)
    {
        //delete m_pReceiver;// 不能在这里删除
        m_pReceiver = NULL;
    }
}
bool CDrawCircleWithMouseCommand::Execute(CCommandBase* command)
{
    m_pReceiver->Action(command);
    return true;
}

bool CDrawCircleWithMouseCommand::UnExecute(CCommandBase* command)
{
    m_pReceiver->UnAction(command);
    return true;
}

void CDrawCircleWithMouseCommand::Draw(QPaintDevice *device)
{
    QPainter painter(device);
    painter.setPen(Qt::black);
    painter.drawEllipse(QPointF(m_circle->GetCenterPt().dx, m_circle->GetCenterPt().dy),
                        m_circle->GetRadius(), m_circle->GetRadius());
}

void CDrawCircleWithMouseCommand::UnDraw(QPaintDevice *device)
{
    QPainter painter(device);
    painter.setPen(Qt::white);
    painter.drawEllipse(QPointF(m_circle->GetCenterPt().dx, m_circle->GetCenterPt().dy),
                        m_circle->GetRadius(), m_circle->GetRadius());
}

void CDrawCircleWithMouseCommand::SetReceiver(CCommandReceiverBase *pReceiver)
{
    m_pReceiver = pReceiver;
}

ref_ptr<CGeoCircle> CDrawCircleWithMouseCommand::GetCircleData()
{
    return m_circle;
}

int CDrawCircleWithMouseCommand::GetType()
{
    return GEO_TYPE_CIRCLE;
}

/*****CDrawArcWithMouseCommand Class***************************************/
CDrawArcWithMouseCommand::CDrawArcWithMouseCommand():
    m_pReceiver(NULL), m_arc(0){}

CDrawArcWithMouseCommand::CDrawArcWithMouseCommand(CCommandReceiverBase *receiver, CGeoArc *arc)
    :m_pReceiver(receiver), m_arc(arc){}



CDrawArcWithMouseCommand::~CDrawArcWithMouseCommand()
{
    if (m_pReceiver)
    {
        //delete m_pReceiver;// 不能在这里删除
        m_pReceiver = NULL;
    }
}
bool CDrawArcWithMouseCommand::Execute(CCommandBase* command)
{
    m_pReceiver->Action(command);
    return true;
}

bool CDrawArcWithMouseCommand::UnExecute(CCommandBase* command)
{
    m_pReceiver->UnAction(command);
    return true;
}

void CDrawArcWithMouseCommand::Draw(QPaintDevice *device)
{
    QPainter painter(device);
    painter.setPen(Qt::black);
    FLOAT_T startAngle = m_arc->GetAngleS();
    FLOAT_T endAngle = m_arc->GetAngleE();
    CGeoPt center = m_arc->GetCenterPt();
    FLOAT_T rad = m_arc->GetRadius();
    QRectF rectf(center.dx-rad, center.dy - rad, 2*rad, 2*rad);
    painter.drawArc(rectf,-startAngle*16, -(endAngle-startAngle)*16);

}

void CDrawArcWithMouseCommand::UnDraw(QPaintDevice *device)
{
    QPainter painter(device);
    painter.setPen(Qt::white);
    FLOAT_T startAngle = m_arc->GetAngleS();
    FLOAT_T endAngle = m_arc->GetAngleE();
    CGeoPt center = m_arc->GetCenterPt();
    FLOAT_T rad = m_arc->GetRadius();
    QRectF rectf(center.dx-rad, center.dy - rad, 2*rad, 2*rad);
    painter.drawArc(rectf,-startAngle*16, -(endAngle-startAngle)*16);
}

ref_ptr<CGeoArc> CDrawArcWithMouseCommand::GetArcData()
{
    return m_arc;
}

int CDrawArcWithMouseCommand::GetType()
{
    return GEO_TYPE_ARC;
}


/*****CDrawLWPolylineWithMouseCommand Class***************************************/
CDrawLWPolylineWithMouseCommand::CDrawLWPolylineWithMouseCommand()
    :m_pReceiver(NULL), m_LWPolyline(0){}


CDrawLWPolylineWithMouseCommand::CDrawLWPolylineWithMouseCommand(CCommandReceiverBase *receiver, CGeoLWPolyline *LWPolyline)
    :m_pReceiver(receiver), m_LWPolyline(LWPolyline){}


CDrawLWPolylineWithMouseCommand::~CDrawLWPolylineWithMouseCommand()
{
    if (m_pReceiver)
    {
        //delete m_pReceiver;// 不能在这里删除
        m_pReceiver = NULL;
    }
}
bool CDrawLWPolylineWithMouseCommand::Execute(CCommandBase* command)
{
    m_pReceiver->Action(command);
    return true;
}

bool CDrawLWPolylineWithMouseCommand::UnExecute(CCommandBase* command)
{
    m_pReceiver->UnAction(command);
    return true;
}

void CDrawLWPolylineWithMouseCommand::Draw(QPaintDevice *device)
{
    QPainter painter(device);
    painter.setPen(Qt::black);
    QVector<QPointF> QvecPoint;
    std::vector<CGeoPt> allPoint = m_LWPolyline->GetPtList();
    for(std::vector<CGeoPt>::iterator itr = allPoint.begin();itr !=  allPoint.end(); ++itr){
        CGeoPt pt = (*itr);
        QvecPoint.push_back(QPoint(pt.dx, pt.dy));
    }
    QPolygonF polygon(QvecPoint);

    if(m_LWPolyline->IsClosed() == true){
        painter.drawPolygon(polygon);
    }else{
        painter.drawPolyline(polygon);
    }
}

void CDrawLWPolylineWithMouseCommand::UnDraw(QPaintDevice *device)
{
    QPainter painter(device);
    painter.setPen(Qt::white);
    QVector<QPointF> QvecPoint;
    std::vector<CGeoPt> allPoint = m_LWPolyline->GetPtList();
    for(std::vector<CGeoPt>::iterator itr = allPoint.begin();itr !=  allPoint.end(); ++itr){
        CGeoPt pt = (*itr);
        QvecPoint.push_back(QPoint(pt.dx, pt.dy));
    }
    QPolygonF polygon(QvecPoint);

    if(m_LWPolyline->IsClosed() == true){
        painter.drawPolygon(polygon);
    }else{
        painter.drawPolyline(polygon);
    }
}

void CDrawLWPolylineWithMouseCommand::SetReceiver(CCommandReceiverBase *pReceiver)
{
    m_pReceiver = pReceiver;
}

ref_ptr<CGeoLWPolyline> CDrawLWPolylineWithMouseCommand::GetLWPolylineData()
{
    return m_LWPolyline;
}

int CDrawLWPolylineWithMouseCommand::GetType()
{

    return GEO_TYPE_LWPOLYLINE;
}
