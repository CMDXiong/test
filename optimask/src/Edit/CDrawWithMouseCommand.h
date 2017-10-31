#ifndef CDRAWWITHMOUSECOMMAND_H
#define CDRAWWITHMOUSECOMMAND_H

#include "./Edit/ccommandbase.h"
#include "./base/Geo.h"
#include <QLine>
#include <QRect>
#include <QPoint>

class CCommandReceiverBase;
/*****CDrawLineWithMouseCommand Class***************************************/
class CDrawLineWithMouseCommand : public CCommandBase
{
public:
    CDrawLineWithMouseCommand();
    CDrawLineWithMouseCommand(CCommandReceiverBase* receiver, CGeoLine *line);
    virtual ~CDrawLineWithMouseCommand();

    virtual bool Execute(CCommandBase* command);
    virtual bool UnExecute(CCommandBase*command);
    virtual void Draw(QPaintDevice* device);
    virtual void UnDraw(QPaintDevice * device);

    void SetReceiver(CCommandReceiverBase * pReceiver);
    void SetLineData(CGeoLine *line);
    ref_ptr<CGeoLine> GetLineData();

    int GetType();
private:
    CDrawLineWithMouseCommand(const CDrawLineWithMouseCommand& rhs);
    CDrawLineWithMouseCommand& operator=(const CDrawLineWithMouseCommand& rhs);

private:
    CCommandReceiverBase*  m_pReceiver;
    ref_ptr<CGeoLine> m_line;
};

/*****CDrawRectWithMouseCommand Class***************************************/
class CDrawRectWithMouseCommand : public CCommandBase
{
public:
    CDrawRectWithMouseCommand();
    CDrawRectWithMouseCommand(CCommandReceiverBase* receiver, CGeoRectangle* rect);
    virtual ~CDrawRectWithMouseCommand();

    virtual bool Execute(CCommandBase* command);
    virtual bool UnExecute(CCommandBase*command);
    virtual void Draw(QPaintDevice* device);
    virtual void UnDraw(QPaintDevice * device);

    void SetReceiver(CCommandReceiverBase * pReceiver);
    void SetRectData(ref_ptr<CGeoRectangle> rect);
    ref_ptr<CGeoRectangle> GetRectData();

    int GetType();

private:
    CDrawRectWithMouseCommand(const CDrawRectWithMouseCommand& rhs);
    CDrawRectWithMouseCommand& operator=(const CDrawRectWithMouseCommand& rhs);

private:
    CCommandReceiverBase*  m_pReceiver;
    ref_ptr<CGeoRectangle> m_rect;
};

/*****CDrawEllipseWithMouseCommand Class***************************************/
class CDrawEllipseWithMouseCommand : public CCommandBase
{
public:
    CDrawEllipseWithMouseCommand();
    CDrawEllipseWithMouseCommand(CCommandReceiverBase* receiver, CGeoEllipse* ellipse);
    virtual ~CDrawEllipseWithMouseCommand();

    virtual bool Execute(CCommandBase* command);
    virtual bool UnExecute(CCommandBase*command);
    virtual void Draw(QPaintDevice* device);
    virtual void UnDraw(QPaintDevice * device);

    void SetReceiver(CCommandReceiverBase * pReceiver);
    ref_ptr<CGeoEllipse> GetEllipseData();

    int GetType();

private:
    CDrawEllipseWithMouseCommand(const CDrawEllipseWithMouseCommand& rhs);
    CDrawEllipseWithMouseCommand& operator=(const CDrawEllipseWithMouseCommand& rhs);

private:
    CCommandReceiverBase*  m_pReceiver;
    ref_ptr<CGeoEllipse> m_ellipse;
};

/*****CDrawCircleWithMouseCommand Class***************************************/
class CDrawCircleWithMouseCommand : public CCommandBase
{
public:
    CDrawCircleWithMouseCommand();
    CDrawCircleWithMouseCommand(CCommandReceiverBase* receiver, CGeoCircle *circle);
    virtual ~CDrawCircleWithMouseCommand();

    virtual bool Execute(CCommandBase* command);
    virtual bool UnExecute(CCommandBase*command);
    virtual void Draw(QPaintDevice* device);
    virtual void UnDraw(QPaintDevice * device);

    inline void SetReceiver(CCommandReceiverBase * pReceiver);
    ref_ptr<CGeoCircle> GetCircleData();

    int GetType();

private:
    CDrawCircleWithMouseCommand(const CDrawCircleWithMouseCommand& rhs);
    CDrawCircleWithMouseCommand& operator=(const CDrawCircleWithMouseCommand& rhs);

private:
    CCommandReceiverBase*  m_pReceiver;
    ref_ptr<CGeoCircle> m_circle;
};

/*****CDrawArcWithMouseCommand Class***************************************/
class CDrawArcWithMouseCommand : public CCommandBase
{
public:
    CDrawArcWithMouseCommand();
    CDrawArcWithMouseCommand(CCommandReceiverBase* receiver, CGeoArc* arc);
    virtual ~CDrawArcWithMouseCommand();

    virtual bool Execute(CCommandBase* command);
    virtual bool UnExecute(CCommandBase*command);
    virtual void Draw(QPaintDevice* device);
    virtual void UnDraw(QPaintDevice * device);

    inline void SetReceiver(CCommandReceiverBase * pReceiver);
    ref_ptr<CGeoArc> GetArcData();

    int GetType();
private:
    CDrawArcWithMouseCommand(const CDrawArcWithMouseCommand& rhs);
    CDrawArcWithMouseCommand& operator=(const CDrawArcWithMouseCommand& rhs);

private:
    CCommandReceiverBase*  m_pReceiver;
    ref_ptr<CGeoArc> m_arc;
};

/*****CDrawLWPolylineWithMouseCommand Class***************************************/
class CDrawLWPolylineWithMouseCommand : public CCommandBase
{
public:
    CDrawLWPolylineWithMouseCommand();
    CDrawLWPolylineWithMouseCommand(CCommandReceiverBase* receiver,CGeoLWPolyline *LWPolyline);
    virtual ~CDrawLWPolylineWithMouseCommand();

    virtual bool Execute(CCommandBase* command);
    virtual bool UnExecute(CCommandBase*command);
    virtual void Draw(QPaintDevice* device);
    virtual void UnDraw(QPaintDevice * device);

    void SetReceiver(CCommandReceiverBase * pReceiver);

    ref_ptr<CGeoLWPolyline> GetLWPolylineData();
    int GetType();

private:
    CDrawLWPolylineWithMouseCommand(const CDrawLWPolylineWithMouseCommand& rhs);
    CDrawLWPolylineWithMouseCommand& operator=(const CDrawLWPolylineWithMouseCommand& rhs);

private:
    CCommandReceiverBase*  m_pReceiver;
    ref_ptr<CGeoLWPolyline> m_LWPolyline;
};

#endif // CDRAWWITHMOUSECOMMAND_H
