/* ========================================================================== */
/* GEO.H -- 基本图形定义头文件
 *
 * REFERENCE: 
 *
 * COPYRIGHT 2017 Optixera.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.00$ (20171001 Chaohong Liu) */
/* HISTORY: 20171001(v1.00) Initial Version;
 * ========================================================================== */

#ifndef _GEOOBJ_H
#define _GEOOBJ_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <math.h>
#include <time.h>
#include <limits>

#include "Referenced.h"


#ifndef M_PI
#define   M_PI      3.1415926535897932
#endif

//地物类型
#define     GEO_TYPE_NONE       0      //未定义的任何类型
#define     GEO_TYPE_POINT      1      //点
#define     GEO_TYPE_LINE       2	   //线
#define     GEO_TYPE_CIRCLE     3      //圆
#define     GEO_TYPE_ARC        4      //圆弧
#define     GEO_TYPE_LWPOLYLINE 5      //折线
#define     GEO_TYPE_ELLIPSE    6      //椭圆
#define     GEO_TYPE_TEXT       7      //纯显示文本
#define     GEO_TYPE_RECT       8     // 矩形
//#define     GEO_TYPE_PATH       9    // 路径
#define     GEO_TYPE_POLYGON    10    // 多边形

#define     GEO_TYPE_MULRIPLER  20    //复合对象
#define     GEO_TYPE_TRANSFORM  21     //transform节点
#define     GEO_TYPE_ARRAY      22     //阵列节点
//以下针对GDS格式数据/////////////////////
#define     GEO_TYPE_STRUCT     100     //GDS模块Struct节点

#define     GEO_TYPE_ELEMENT    110     //Base Element type begin
#define     GEO_TYPE_BOUNDARY   111     //BOUNDARY
#define     GEO_TYPE_PATH       112     //PATH
#define     GEO_TYPE_SREF       113     
#define     GEO_TYPE_AREF       114     
#define     GEO_TYPE_GDS_TEXT   115     
#define     GEO_TYPE_NODE       116     
#define     GEO_TYPE_BOX        117 
#define     GEO_TYPE_OTHER      118     //其他没法在GDS类型找到定义的所有节点类型，如矢量圆弧 
#define     GEO_TYPE_REF_REV    119     //保留的ref节点 
#define     GEO_TYPE_ELEM_END   120     //Element Type Define end

#define     GEO_TYPE_USER       1000    //用户自定义元素开始
/////////////////////////////////////////



//层类型
#define     LAYER_TYPE_LINE	   1		//线路层

//层标志
#define     LAYER_FLAG_NONE         0       //初始状态
#define     LAYER_FLAG_LOCK         1       //锁定，不能修改
#define     LAYER_FLAG_HIDE         2       //隐藏
#define     LAYER_FLAG_PROTECT      4       //保护，不能选择
#define     LAYER_FLAG_FILL         8       //填充

#ifdef EMBED_ARM
#define FLOAT_T	float
#define ULLong  unsigned long long
#else
#define FLOAT_T	double
#define ULLong  unsigned __int64
#endif

#define     MIN_PRECISION       1e-6

#pragma pack(4)//设定为4字节对齐 

//编译器在解构对齐时采用四字节
//地理范围结构
typedef struct TAG_RECT
{
	FLOAT_T m_dLeft, m_dTop, m_dRight, m_dBottom;
	TAG_RECT(){ m_dLeft = m_dRight = m_dTop = m_dBottom = 0; }
    TAG_RECT(FLOAT_T dLeft, FLOAT_T dTop, FLOAT_T dRight, FLOAT_T dBottom)
    { 
        m_dLeft = dLeft;
        m_dRight = dRight;
        m_dTop = dTop;
        m_dBottom = dBottom;
    }
	FLOAT_T Width(){ return m_dRight - m_dLeft; }
    FLOAT_T Height(){ return  m_dBottom - m_dTop ; }
} CGeoRect;

typedef struct TAG_PT
{
	FLOAT_T dx, dy;
	TAG_PT(){ dx = dy = 0; }
	TAG_PT(FLOAT_T x, FLOAT_T y)
	{ 
		dx = x; dy = y; 
	}
	TAG_PT(const TAG_PT& pt)
	{
		if (this == &pt)
			return;

		dx = pt.dx; dy = pt.dy;
	}
	TAG_PT& operator=(const TAG_PT& pt)
	{
		if (this != &pt)
		{
			dx = pt.dx; dy = pt.dy;
		}
		return *this;
	}

    bool operator<(const TAG_PT& pt) const
    {
        if (dx < pt.dx)
            return true;
        else if (fabs(dx - pt.dx) < MIN_PRECISION)
        {
            if (dy < pt.dy)
                return true;
            else
                return false;
        }
        else
            return false;
    }

} CGeoPt;

class CGeoArc;
extern bool GetLineProjectPoint(const CGeoPt& first, const CGeoPt& second, const CGeoPt& pt, CGeoPt* ptDist=0, CGeoPt* projPt=0);
extern bool GetArcProjectPoint(const CGeoArc& arc, const CGeoPt& pt, CGeoPt* ptDist=0, CGeoPt* projPt=0);
extern bool GetCircleProjectPoint(const CGeoPt& center, FLOAT_T radius, const CGeoPt& pt, FLOAT_T& dbDist, CGeoPt* projPt = 0);

//具体的属性值,如果颜色，笔宽等
//每个具体的属性值得编码ID列表在GeoAttribute.h头文件定义，编码保存在m_dwID变量中
class CProValue : public Referenced
{
protected:
    unsigned long		m_dwID;			//属性编号
public:
    CProValue() { m_dwID=0; }
    inline unsigned long GetProValuID() const { return m_dwID;}
};
//定义图元的属性，具体的属性值必须唯一
class CGeoProperty: public Referenced
{
private:
    std::vector<ref_ptr<CProValue> > m_vecProValueList;

public:
    std::vector<ref_ptr<CProValue> >& GetProValueList() { return m_vecProValueList;}
    const std::vector<ref_ptr<CProValue> >& GetProValueList() const { return m_vecProValueList; }
    unsigned GetProValueNum() const { return m_vecProValueList.size(); }
    //添加具体的属性值，如果存在则返回false，否则返回true
    bool AddProValue(CProValue* val);
    //设置具体属性值，val表示具体的属性值
    //bAdd表示如果设置的属性不存在，是否添加，true表示添加，false表示不添加
    //返回值true表示成功的设置或者添加，false表示失败
    bool SetProValue(CProValue* val,bool bAdd=true);
    //根据属性编号返回具体的属性值
    //如果找不到则返回0，否则返回具体的属性值
    CProValue* GetProValue(unsigned long dwID);
    //检查是否存在指定的属性值
    bool HasProValur(unsigned long dwID);
};

//基础对象类，方法多为虚函数，在派生类中实现
class CGeoBase : public Referenced
{
protected:
	unsigned long		m_dwID;			//编号
	unsigned short		m_wType;		//类型
    //mutable unsigned long  m_dwLen;		//对象占用空间大小。该字段也许会取消
    //笔对象大小，如果对于点，则表示点的大小，如果对于线，就是表示线粗，如果为0，则表示采用系统默认的，如果为负数，则采用绝对大小
    //对于复合对象，如果下面的子对象没有设置，则采用符合对象的值，否则采用自己专有的。
    //int                 m_nPenSize;   
    ref_ptr<CGeoProperty>   m_Attribute;    //该对象的属性数据
public:
    CGeoBase() { m_dwID = 0; m_wType = GEO_TYPE_NONE;/* m_dwLen = 0;m_nPenSize = 0; */}
    inline void SetObjID(unsigned long ID){ m_dwID = ID; }
	inline unsigned long GetObjID() const { return m_dwID; }                       //得到对象ID
    inline unsigned short GetObjType() const { return m_wType; }                   //得到对象类型
    CGeoProperty* GetProperty()const { return m_Attribute.get();}                   //获取属性数据，如果返回值为0，表示没有设置属性
    //强制获取属性数据，如果没有设置属性,则自动生成
    CGeoProperty* GetForcedProperty() 
    { 
        if (!m_Attribute.valid())
            m_Attribute = new CGeoProperty;
        return m_Attribute.get(); 
    }                  
    void SetProperty(CGeoProperty* val) { m_Attribute = val;}                       //设置属性数据
	virtual int GetObjDataLen() const                                              //返回值表示写入到文件中数据大小
    {
        return sizeof(FLOAT_T)+ sizeof(unsigned long)+ sizeof(unsigned short);
    }                    
    virtual CGeoPt GetCentrePoint() const = 0;                                     //得到对象的中心点坐标
    virtual CGeoRect GetBoundBox() const { return CGeoRect(); }                    //得到对象的包围盒
};
		
typedef std::vector<ref_ptr<CGeoBase> >         GeoObjList;                        //地物列表
//点地物
class CGeoPoint : public CGeoBase
{
protected:
	FLOAT_T m_dbX,m_dbY;		//点坐标
public:
    CGeoPoint() 
    {
        m_wType = GEO_TYPE_POINT; 
        m_dbX   = m_dbY = 0; 
    }
	virtual int GetObjDataLen() const {return sizeof(FLOAT_T) * 2 + CGeoBase::GetObjDataLen();}
    virtual CGeoPt GetCentrePoint() const
    {
       return  CGeoPt(m_dbX, m_dbY);
    }
    virtual CGeoRect GetBoundBox() const
    {
        return CGeoRect(m_dbX, m_dbY, m_dbX, m_dbY);
    }

    inline void SetPt(FLOAT_T x, FLOAT_T y){m_dbX = x;m_dbY = y;}
    inline void GetPt(FLOAT_T& x, FLOAT_T& y) const { x=m_dbX;y=m_dbY; }
    inline CGeoPt GetPt() const { return CGeoPt(m_dbX, m_dbY); } 
};

//线地物
class CGeoLine : public CGeoBase
{
protected:
	FLOAT_T	  m_dbBeginX,m_dbBeginY; //第一个点坐标
	FLOAT_T   m_dbEndX,m_dbEndY;     //第二个点坐标
public:	
	CGeoLine()
    {
        m_wType = GEO_TYPE_LINE;
        m_dbBeginX = m_dbBeginY = m_dbEndX = m_dbEndY = 0;
    }
	CGeoLine(CGeoPt begin, CGeoPt end) //Add by xiong 17/09/05
    {
        m_wType = GEO_TYPE_LINE;
        m_dbBeginX = begin.dx;
        m_dbBeginY = begin.dy;
        m_dbEndX  = end.dx;
        m_dbEndY = end.dy;
    }
	
	virtual int GetObjDataLen() const {return sizeof(FLOAT_T) * 4 + CGeoBase::GetObjDataLen();}
    virtual  CGeoPt GetCentrePoint() const
    {
        return CGeoPt((m_dbBeginX + m_dbEndX) / 2, (m_dbBeginY + m_dbEndY)/2);
    }

    virtual CGeoRect GetBoundBox() const
    {
        FLOAT_T dbMinX = m_dbBeginX > m_dbEndX ? m_dbEndX : m_dbBeginX;
        FLOAT_T dbMaxX = m_dbBeginX < m_dbEndX ? m_dbEndX : m_dbBeginX;
        FLOAT_T dbMinY = m_dbBeginY > m_dbEndY ? m_dbEndY : m_dbBeginY;
        FLOAT_T dbMaxY = m_dbBeginY < m_dbEndY ? m_dbEndY : m_dbBeginY;
        return CGeoRect(dbMinX, dbMinY, dbMaxX, dbMaxY);
    }

    //计算点到直线上的映射点
    //参数pt表示实际的点，projPt参数表示映射点
    //ptDist中的X分量表示映射点到该对象第一个点的距离（表示轴向距离），Y分量表示到该对象的距离
    //如果点没法映射到地物上或者该函数没法计算，则返回值为false
    //如果参数projPt为0，则表示不需要知道映射点，ptDist为0，表示不需要知道距离
    virtual bool GetProjectPoint(const CGeoPt& pt, CGeoPt* ptDist = 0, CGeoPt* projPt = 0) const
    {
        return GetLineProjectPoint(CGeoPt(m_dbBeginX, m_dbBeginY), CGeoPt(m_dbEndX, m_dbEndY), pt, ptDist, projPt);
    }

    //设置坐标
    inline void SetLinePt(const CGeoPt& first, const CGeoPt& second)
    {
        m_dbBeginX = first.dx;
        m_dbBeginY = first.dy;
        m_dbEndX = second.dx;
        m_dbEndY = second.dy;
    }
    inline void SetFirstPt(const CGeoPt& pt)
    {
        m_dbBeginX = pt.dx;
        m_dbBeginY = pt.dy;
    }
    inline void SetSecondPt(const CGeoPt& pt)
    { 
        m_dbEndX = pt.dx;
        m_dbEndY = pt.dy;
    }

    inline void GetLinePt(CGeoPt& first, CGeoPt& second) const
    {
         first.dx= m_dbBeginX;
         first.dy= m_dbBeginY;
         second.dx= m_dbEndX;
         second.dy= m_dbEndY;
    }
    inline CGeoPt GetFirstPt() const { return CGeoPt(m_dbBeginX, m_dbBeginY); }
    inline CGeoPt GetSecondPt() const { return CGeoPt(m_dbEndX, m_dbEndY); }
};

//圆地物
class CGeoCircle : public CGeoBase
{
protected:
    FLOAT_T	  m_dbX, m_dbY; //圆心点坐标
    FLOAT_T   m_dbRad;     //半径

public:
    CGeoCircle()
    {
        m_wType = GEO_TYPE_CIRCLE; 
        m_dbX = m_dbY = m_dbRad = 0;
    }
	CGeoCircle(CGeoPt center,FLOAT_T rad )   //Add by xiong 17/09/05
    {
        m_wType = GEO_TYPE_CIRCLE;
        m_dbX = center.dx;
        m_dbY = center.dy;
        m_dbRad = rad;
    }
    virtual int GetObjDataLen() const{return sizeof(FLOAT_T) * 3 + CGeoBase::GetObjDataLen(); }
    virtual CGeoPt GetCentrePoint() const { return CGeoPt(m_dbX, m_dbY); }
    virtual CGeoRect GetBoundBox() const
    {
        return CGeoRect(m_dbX- m_dbRad, m_dbY- m_dbRad, m_dbX + m_dbRad, m_dbY + m_dbRad);
    }
    //计算点到地物的距离和映射到地物上的映射点
    //参数pt表示实际的点，projPt参数表示映射点
    //ptDist中的X分量表示映射点到该对象第一个点的距离（圆弧为到第一个点的弧长），Y分量表示到该对象的距离
    //如果点没法映射到地物上或者该函数没法计算，则返回值为false
    //如果参数projPt为0，则表示不需要知道映射点，ptDist为0，表示不需要知道距离
    virtual bool GetProjectPoint(const CGeoPt& pt, CGeoPt* ptDist = 0, CGeoPt* projPt = 0) const
    {
        FLOAT_T dbDist;
        bool bRet = GetCircleProjectPoint(CGeoPt(m_dbX, m_dbY), m_dbRad, pt, dbDist, projPt);
        if (bRet && ptDist != 0)
        {
            ptDist->dy = dbDist;
            ptDist->dx = 0;
        }
        return bRet;
    }

    //设置坐标
    inline void SetCenterPt(const CGeoPt& pt){m_dbX = pt.dx;m_dbY = pt.dy;}
    inline void SetCenterPt(FLOAT_T x, FLOAT_T y) { m_dbX = x; m_dbY = y;}
    inline void SetRadius(FLOAT_T tmp){ m_dbRad = tmp;}

    inline CGeoPt GetCenterPt() const{ return CGeoPt(m_dbX,m_dbY); }
    inline void GetCenterPt(FLOAT_T& x, FLOAT_T& y) const { x= m_dbX; y= m_dbY; }
    inline FLOAT_T GetRadius() const { return m_dbRad; }
};

//圆弧地物
class CGeoArc : public CGeoCircle
{
protected:
	FLOAT_T   m_dbAngleS,m_dbAngleE;			//起始角度和终止角度,单位为度，不是弧度

public:
	CGeoArc()
    {
        m_wType = GEO_TYPE_ARC;
        m_dbAngleS = m_dbAngleE = 0;
    }

	CGeoArc(CGeoPt center, FLOAT_T rad, FLOAT_T angleS, FLOAT_T angleE) //Add by xiong 17/09/05
    {
        m_dbX = center.dx;
        m_dbY = center.dy;
        m_dbRad = rad;

        m_dbAngleS = angleS;
        m_dbAngleE = angleE;

        while(m_dbAngleS<0 || m_dbAngleS>360){
            if(m_dbAngleS<0){
                m_dbAngleS += 360;
            }
            if(m_dbAngleS>360){
                m_dbAngleS -= 360;
            }
        }
        while(m_dbAngleE<0 || m_dbAngleE>360){
            if(m_dbAngleE<0){
                m_dbAngleE += 360;
            }
            if(m_dbAngleE>360){
                m_dbAngleE -= 360;
            }
        }
    }
    virtual int GetObjDataLen() const {return CGeoCircle::GetObjDataLen() + sizeof(FLOAT_T) * 2; }
    virtual CGeoPt GetCentrePoint() const;
    virtual CGeoRect GetBoundBox() const;                 // Add by xiong 17/09/07
    //计算点到地物的距离和映射到地物上的映射点
    //参数pt表示实际的点，projPt参数表示映射点
    //ptDist中的X分量表示映射点到该对象第一个点的距离（圆弧为到第一个点的弧长），Y分量表示到该对象的距离
    //如果点没法映射到地物上或者该函数没法计算，则返回值为false
    //如果参数projPt为0，则表示不需要知道映射点，ptDist为0，表示不需要知道距离
    virtual bool GetProjectPoint(const CGeoPt& pt, CGeoPt* ptDist = 0, CGeoPt* projPt = 0) const
    {
        return GetArcProjectPoint(*this, pt, ptDist,projPt);
    }

    //设置坐标
    inline void SetAngleS(FLOAT_T tmp){ m_dbAngleS = tmp; }
	inline FLOAT_T GetAngleS()const {return m_dbAngleS;}
    inline void SetAngleE(FLOAT_T tmp){ m_dbAngleE = tmp; }
	inline FLOAT_T GetAngleE() const {return m_dbAngleE;}
    inline void GetFirstPt(CGeoPt& pt) const
    {
        pt.dx = m_dbX + m_dbRad*cos(m_dbAngleS / 180 * M_PI);
        pt.dy = m_dbY + m_dbRad*sin(m_dbAngleS / 180 * M_PI);
    }
    inline void GetSecondPt(CGeoPt& pt) const
    {
        pt.dx = m_dbX + m_dbRad*cos(m_dbAngleE / 180 * M_PI);
        pt.dy = m_dbY + m_dbRad*sin(m_dbAngleE / 180 * M_PI);
    }
};

//多段线，保存所有点
class CGeoLWPolyline : public CGeoBase
{
protected:
	std::vector<CGeoPt> m_allPoint; //所有点
    bool                m_bClose;   //表示起始点与结束点是否相连，如果为true，则画图时必须额外画一直线从起始点到终止点
    bool                m_bFill;    //是否填充，只有封闭多边形才使用这个值
public:
    CGeoLWPolyline(){ m_wType = GEO_TYPE_LWPOLYLINE; m_bFill = m_bClose = false; }
	virtual int GetObjDataLen() const {return sizeof(FLOAT_T)* m_allPoint.size() * 2 + CGeoBase::GetObjDataLen()+sizeof(bool);}
	CGeoLWPolyline(std::vector<CGeoPt> allPoint, bool close = false)                        //Add by xiong 17/09/05
     {
         m_allPoint = allPoint;
         m_bClose = close;
     }
    virtual CGeoPt GetCentrePoint() const
    {
        return CGeoPt(); //目前没有实现，有时间必须实现
    }

    virtual CGeoRect GetBoundBox() const;       // Add by xiong 17/09/07

    //设置坐标
    void AddPt(FLOAT_T x, FLOAT_T y){ AddPt(CGeoPt(x,y)); }
    void AddPt(const CGeoPt& pt){ m_allPoint.push_back(pt);}
    bool GetPt(unsigned nIndex, CGeoPt& pt) const
    {
        if (nIndex > m_allPoint.size())
            return false;
        else
        {
            pt = m_allPoint[nIndex];
            return true;
        }
    }
    CGeoPt GetPt(unsigned nIndex) const
    {
        if (nIndex >= m_allPoint.size())
            return CGeoPt();
        else
            return  m_allPoint[nIndex];
    }
    //update point
    bool UpdatePt(unsigned nIndex, const CGeoPt& pt)
    {
        if (nIndex >= m_allPoint.size())
            return false;
        m_allPoint[nIndex] = pt;
        return true;
    }

    inline void SetClose(bool bClose){ m_bClose = bClose; }
    inline bool IsClosed() const { return m_bClose; }

    inline void SetFill(bool bFill) { m_bFill = bFill; }
    inline bool IsFill() const { return m_bFill; }

    inline unsigned GetPtNum() const { return m_allPoint.size(); }
    inline const std::vector<CGeoPt>& GetPtList() const { return m_allPoint; }
};

//矩形地物
class CGeoEllipse : public CGeoBase
{
protected:
    FLOAT_T	  m_dbX, m_dbY;        // 圆心点坐标
    FLOAT_T    m_dbwidth;          // 长轴的一半
    FLOAT_T    m_dbheight;         // 短轴的一半

public:
    CGeoEllipse()
    {
        m_wType = GEO_TYPE_ELLIPSE;
        m_dbX = m_dbY = m_dbwidth = m_dbheight = 0;
    }
    CGeoEllipse(CGeoPt center, FLOAT_T width, FLOAT_T height)  //Add by xiong 17/09/05
    {
        m_wType = GEO_TYPE_ELLIPSE;
        m_dbX = center.dx;
        m_dbY = center.dy;
        m_dbwidth = width;
        m_dbheight = height;
    }
    CGeoEllipse(CGeoPt startPt , CGeoPt endPt)  //Add by xiong 17/09/05
    {
        m_wType = GEO_TYPE_ELLIPSE;
        m_dbX = (startPt.dx + endPt.dx)/2;
        m_dbY = (startPt.dy + endPt.dy)/2;
        m_dbwidth = (endPt.dx - startPt.dx)/2;
        m_dbheight = (endPt.dy - startPt.dy)/2;
    }
    virtual int GetObjDataLen() const{return sizeof(FLOAT_T) * 5 + CGeoBase::GetObjDataLen();}
    virtual CGeoPt GetCentrePoint() const { return CGeoPt(m_dbX, m_dbY); }
    virtual CGeoRect GetBoundBox() const            //Add by xiong 17/09/07
    {
        return CGeoRect(m_dbX - m_dbwidth, m_dbY - m_dbheight, m_dbX + m_dbwidth, m_dbY + m_dbheight);
    }

    //设置坐标
    inline void SetCenterPt(FLOAT_T x, FLOAT_T y) { m_dbX = x; m_dbY = y; }
    inline void SetCenterPt(const CGeoPt& pt){ m_dbX = pt.dx; m_dbY = pt.dy; }
    inline void SetWidth(FLOAT_T width){ m_dbwidth = width; }
    inline void SetHeight(FLOAT_T height) { m_dbheight = height; }
    inline CGeoPt GetCenterPt() const { return CGeoPt(m_dbX, m_dbY); }
    inline FLOAT_T GetWidth() const {return m_dbwidth;}    //Add by xiong 17/09/05
    inline FLOAT_T GetHeight() const {return m_dbheight;}   //Add by xiong 17/09/05
};

//单行文本地物
class CGeoText : public CGeoBase
{
protected:
	FLOAT_T	  m_dbX, m_dbY; 					//文本起始坐标
	FLOAT_T   m_dbDirX, m_dbDirY; 				//文本的方向
    FLOAT_T   m_dbHeight;                       //文本高度，暂不采用
    int       m_nFont;                          //字体编号，目前针对GDS
    unsigned char m_byAlignVer;                 //垂直对齐方式，0->top, 1->middle, 2->bottom
    unsigned char m_byAlignHor;                 //水平对齐方式，0->left, 1->center, 2->right
	std::string  m_strName;                     //文本数据
public:
	CGeoText()
    { 
        m_wType = GEO_TYPE_TEXT;
        m_dbX = m_dbY = m_dbDirX = m_dbDirY = /*m_dbHeight =*/ 0;
        m_nFont = 0;
        m_byAlignHor= m_byAlignVer = 0;
    }
	virtual int GetObjDataLen() const{return sizeof(FLOAT_T) * 4 + m_strName.size() + 1;}
    virtual CGeoPt GetCentrePoint() const 
    { 
        return CGeoPt(); //目前没有实现，有时间必须实现
    }
    virtual CGeoRect GetBoundBox() const
    {
        // 注：一个字母只占文本高度的一半，但是汉字却是宽度等于高度，目前字母当作汉字来计算
        double right = m_dbX + m_dbHeight*m_strName.size();
        double bottom = m_dbY + m_dbHeight;
        return CGeoRect(m_dbX, m_dbY, right,bottom);
    }

    inline void SetText(const std::string& strText){ m_strName = strText; }
    const std::string& GetText() {return m_strName;}

    void SetAlignHor(unsigned char byVal) { m_byAlignHor = byVal;}
    void SetAlignVer(unsigned char byVal) { m_byAlignVer = byVal;}
    void SetFontIndex(int nVal) {m_nFont = nVal; }

    inline void SetStartPt(FLOAT_T x, FLOAT_T y) { m_dbX = x; m_dbY = y; }
    inline void SetStartPt(const CGeoPt& pt) { m_dbX = pt.dx; m_dbY = pt.dy; }
    CGeoPt GetStartPt() { return CGeoPt(m_dbX, m_dbY);}
    void GetStartPt(FLOAT_T& x, FLOAT_T& y) { x= m_dbX; y= m_dbY; }
    inline void SetDir(FLOAT_T x, FLOAT_T y){ m_dbDirX = x; m_dbDirY = y; }
    inline void SetHeight(FLOAT_T tmp){ m_dbHeight = tmp; }
    FLOAT_T GetHeight() {return m_dbHeight;}
};

//2017/09/22 by bamboo
//修改为两个点表示的矩形，这样的矩形如果要旋转则只能挂载矩阵节点下，然后取出四个点进行选择变换
//或者转换为多边形或者多义线，对多边形进行具体操作
// 
class CGeoRectangle : public CGeoBase
{
protected:
    FLOAT_T   m_dbLeft, m_dbRight;
    FLOAT_T   m_dbTop, m_dbBottom;
public:
    CGeoRectangle() { m_wType = GEO_TYPE_RECT; m_dbLeft = m_dbRight = m_dbTop = m_dbBottom = 0;}
    //指定左下右上（也可指定任意对角点）
    CGeoRectangle(const CGeoPt& begin, const CGeoPt& end){m_wType = GEO_TYPE_RECT;SetRectPt(begin.dx, end.dx, end.dy, begin.dy); }
    //根据左右上下四个数据生成矩形
    CGeoRectangle(FLOAT_T dbLeft, FLOAT_T dbRight, FLOAT_T dbTop, FLOAT_T dbBottom){ m_wType = GEO_TYPE_RECT; SetRectPt(dbLeft, dbRight, dbTop, dbBottom);}
    //根据点列表生成包围盒矩形
    CGeoRectangle(const std::vector<CGeoPt>& lstPt) { m_wType = GEO_TYPE_RECT; SetRectPt(lstPt); }
    //根据多义线或者多边形生成包围盒矩形
    CGeoRectangle(const CGeoLWPolyline& poly) { m_wType = GEO_TYPE_RECT; SetRectPt(poly);}

    virtual int GetObjDataLen() const { return sizeof(FLOAT_T) * 4 + CGeoBase::GetObjDataLen(); }
    virtual  CGeoPt GetCentrePoint() const { return CGeoPt((m_dbLeft + m_dbRight) / 2, (m_dbBottom + m_dbTop) / 2);}
    virtual CGeoRect GetBoundBox() const{ return CGeoRect(m_dbLeft, m_dbTop, m_dbRight, m_dbBottom);}

    //设置矩形数据，指定左下右上（也可指定任意对角点）
    void SetRectPt(const CGeoPt& begin, const CGeoPt& end) { SetRectPt(begin.dx, end.dx, end.dy, begin.dy); }
    //设置矩形数据，根据左右上下四个数据生成矩形
    void SetRectPt(FLOAT_T dbLeft, FLOAT_T dbRight, FLOAT_T dbTop, FLOAT_T dbBottom);
    //设置矩形数据，根据点列表生成包围盒矩形
    void SetRectPt(const std::vector<CGeoPt>& lstPt);
    //设置矩形数据，根据多义线或者多边形生成包围盒矩形
    void SetRectPt(const CGeoLWPolyline& poly);

    //获取矩形的左下右上的对角点
    void GetRectPt(CGeoPt& begin, CGeoPt& end) const
    {
        begin.dx = m_dbLeft;begin.dy = m_dbBottom;
        end.dx = m_dbRight;end.dy = m_dbTop;
    }
    //获取矩形的四个范围
    void GetRectPt(FLOAT_T&dbLeft, FLOAT_T& dbRight, FLOAT_T& dbTop, FLOAT_T& dbBottom) const
    {
        dbLeft = m_dbLeft;dbBottom = m_dbBottom;
        dbRight = m_dbRight;dbTop = m_dbTop;
    }
    //获取矩形的四个顶点的数据，该函数会把矩形的两个对角点转换成逆时针方向的四个顶点，第一个点位左下角顶点
    //注意： 当对矩形进行旋转时，必须先调用该函数获取四个顶点，然后对顶点进行旋转，然后采用旋转后的坐标画图。
    void GetRectPt(std::vector<CGeoPt>& lstPt) const
    {
        lstPt.clear();
        lstPt.push_back(CGeoPt(m_dbLeft, m_dbBottom));
        lstPt.push_back(CGeoPt(m_dbRight, m_dbBottom));
        lstPt.push_back(CGeoPt(m_dbRight, m_dbTop));
        lstPt.push_back(CGeoPt(m_dbLeft, m_dbTop));
    }
    //把矩形转换成多边形
    CGeoLWPolyline ToLWPolyline()
    {
        CGeoLWPolyline temp;
        temp.SetClose(true);
        temp.AddPt(m_dbLeft, m_dbBottom);
        temp.AddPt(m_dbRight, m_dbBottom);
        temp.AddPt(m_dbRight, m_dbTop);
        temp.AddPt(m_dbLeft, m_dbTop);
        return temp;
    }

    inline CGeoPt GetTopLeftPt() const { return CGeoPt(m_dbLeft, m_dbTop); }
    inline CGeoPt GetRightBottomPt() const { return CGeoPt(m_dbRight, m_dbBottom); }
    inline FLOAT_T Width() const { return m_dbRight - m_dbLeft; }
    inline FLOAT_T Height() const{ return  m_dbTop - m_dbBottom; }
};

//复合地物
class CGeoMulripler : public CGeoBase
{
protected:
    GeoObjList          m_vecChildren;       //所有子地物列表
public:
	CGeoMulripler(){ m_wType = GEO_TYPE_MULRIPLER; }
	virtual int GetObjDataLen() const
	{
        int nLen = 0;
		for (unsigned i = 0; i < m_vecChildren.size(); i++)
            nLen += m_vecChildren.at(i)->GetObjDataLen();
        nLen += CGeoBase::GetObjDataLen();
        return  nLen;
	}
    virtual CGeoPt GetCentrePoint() const 
    { 
        return CGeoPt(); //目前没有实现，有时间必须实现
    }
    virtual CGeoRect GetBoundBox() const;
    unsigned GetChildCount() const { return m_vecChildren.size(); } 
    bool IsEmpty() const { return m_vecChildren.empty(); }
    //添加子节点，成功返回true，失败返回false
    virtual bool AddChild(CGeoBase* pElement);
    //设置子节点，成功返回true，失败返回false
    virtual bool SetChild(unsigned nIndex, CGeoBase* pElement);
    CGeoBase* GetChild(unsigned uIndex)
    {
        if (uIndex >= m_vecChildren.size())
            return 0;
        else
            return m_vecChildren.at(uIndex).get();
    }
    const CGeoBase* GetChild(unsigned uIndex) const
    {
        if (uIndex >= m_vecChildren.size())
            return 0;
        else
            return m_vecChildren.at(uIndex).get();
    }

    CGeoBase* FrontChild()
    {
        if (m_vecChildren.empty())
            return 0;
        else
            return m_vecChildren.front().get();
    }
    CGeoBase* BackChild()
    {
        if (m_vecChildren.empty())
            return 0;
        else
            return m_vecChildren.back().get();
    }
};

//阵列节点
class CGeoArray : public CGeoMulripler
{
protected:
    //FLOAT_T        m_dbRefX, m_dbRefY;                //参考点
    //FLOAT_T        m_dbRowEndX, m_dbRowEndY;          //行方向的端点
    //FLOAT_T        m_dbColEndX, m_dbColEndY;          //列方向的端点
    FLOAT_T        m_dbInterRow, m_dbInterCol;          //行间距和列间距
    int            m_nRows, m_nCols;                    //行、列方向的个数，如果这两者为1
public:
    CGeoArray()
    {
        m_wType = GEO_TYPE_ARRAY;
        m_nRows = m_nCols = 1;
        m_dbInterRow = m_dbInterCol = 0;
    }
    void SetRowAndCol(int nRow, int nCol) { m_nRows = nRow; m_nCols = nCol; }
    inline void GetRowAndCol(int& nRow, int& nCol) { nRow= m_nRows; nCol = m_nCols; }
    inline int GetRowNum() const { return m_nRows; }
    inline int GetColNum() const { return m_nCols; }

    void SetRowAndColInterDist(FLOAT_T dbRow, FLOAT_T dbCol) { m_dbInterRow = dbRow; m_dbInterCol = dbCol; }
    inline void GetRowAndColInterDist(FLOAT_T& dbRow, FLOAT_T& dbCol) { dbRow = m_dbInterRow; dbCol = m_dbInterCol; }
    inline FLOAT_T GetInterRowDist() const { return m_dbInterRow; }
    inline FLOAT_T GetInterColDist() const { return m_dbInterCol; }
    virtual CGeoRect GetBoundBox() const
    {
        CGeoRect FrameRect   = CGeoMulripler::GetBoundBox();
        FrameRect.m_dRight  = FrameRect.m_dLeft + m_dbInterRow *(m_nRows - 1) + FrameRect.Width();
        FrameRect.m_dBottom = FrameRect.m_dTop  + m_dbInterCol *(m_nCols - 1) + FrameRect.Height();

        return FrameRect;
    }
};

//矩阵
//该矩阵的计算与定义仅仅用坐标转换，不是完整的数据定义
class CMatrix : public Referenced
{
protected:
    FLOAT_T                            m_mat[3][3];        //3*3矩阵
public:
    CMatrix() { MakeIdentity(); }
    CMatrix(const CMatrix& mat) { memcpy(m_mat, mat.m_mat,sizeof(m_mat)); }
    //设置为单位矩阵
    void MakeIdentity();
    //设置为缩放矩阵
    void MakeScale(FLOAT_T x, FLOAT_T y);
    //设置为平移矩阵
    void MakeTranslate(FLOAT_T x, FLOAT_T y);
    //设置为旋转矩阵，angle为弧度
    void MakeRotate(FLOAT_T angle);
    //根据角度的余弦和正弦设置为旋转矩阵
    //注意：如果传入参数不是真正的余弦和正弦，则会导致矩阵出现不可预料的情况
    void MakeRotate(FLOAT_T cos, FLOAT_T sin);
    //矩阵相乘
    void Mult(const CMatrix& lhs, const CMatrix& rhs);
    //左乘
    void PreMult(const CMatrix& other);
    //右乘
    void PostMult(const CMatrix& other);
    //点与矩阵左乘
    inline CGeoPt PreMult(const CGeoPt& v) const
    {
        FLOAT_T d = 1.0f / (m_mat[0][2] * v.dx + m_mat[1][2] * v.dy + m_mat[2][2]);
        return CGeoPt((m_mat[0][0] * v.dx + m_mat[1][0] * v.dy + m_mat[2][0])*d,
            (m_mat[0][1] * v.dx + m_mat[1][1] * v.dy +  m_mat[2][1])*d);
    }
    //点与矩阵右乘
    inline CGeoPt postMult(const CGeoPt& v) const
    {
        FLOAT_T d = 1.0f / (m_mat[2][0] * v.dx + m_mat[2][1] * v.dy + m_mat[2][2]);
        return CGeoPt((m_mat[0][0] * v.dx + m_mat[0][1] * v.dy + m_mat[0][2])*d,
            (m_mat[1][0] * v.dx + m_mat[1][1] * v.dy + m_mat[1][2])*d);
    }
    //点与矩阵乘，就是把坐标/向量经过变换矩阵变换后得到新的坐标/向量
    inline CGeoPt operator* (const CGeoPt& v) const
    {
        return PreMult(v);
    }
    inline const CMatrix& operator* (const CMatrix& other)
    {
        PreMult(other);
        return *this;
    }
    inline void operator *= (const CMatrix& other)
    {
        if (this == &other)
            return;
        PreMult(other);
    }
    inline const CMatrix& operator= (const CMatrix& other)
    {
        if (this == &other)
            return *this;
        else
        {
            memcpy(m_mat, other.m_mat, sizeof(m_mat));
            return *this;
        }
    }
    //目前针对版图，好像支持x/y同样的缩放因子,所以我们假设为同一比例因子
    //如果缩放比例为复数，则表示进行了reflection
    FLOAT_T GetScale() const;
    void GetScale(FLOAT_T& x, FLOAT_T& y)const;    //该函数目前不支持x,y不同缩放比例因子
    void GetTranslate(FLOAT_T& x, FLOAT_T& y)const;  //得到平移量
    FLOAT_T GetAngle()const;
    void GetRotateAndTranslate(CMatrix& mat) const;   //得到旋转与平移矩阵
    void GetInvMatrix(CMatrix& mat) const;            //得到矩阵的逆

    //返回一个单位矩阵
    inline static CMatrix Identity(void)
    {
        CMatrix m;
        return m;
    }
    //返回一个缩放矩阵
    inline static CMatrix Scale(FLOAT_T x, FLOAT_T y)
    {
        CMatrix m;
        m.MakeScale(x, y);
        return m;
    }
    //返回一个平移矩阵
    inline static CMatrix Translate(FLOAT_T x, FLOAT_T y)
    {
        CMatrix m;
        m.MakeTranslate(x, y);
        return m;
    }
    //返回一个旋转矩阵，角度为弧度
    inline static CMatrix Rotate(FLOAT_T angle)
    {
        CMatrix m;
        m.MakeRotate(angle);
        return m;
    }
};

//矩阵节点
class CGeoTransform : public CGeoBase
{
protected:
    ref_ptr<CMatrix>    m_mat;               //3*3矩阵
    ref_ptr<CGeoBase>   m_geoChild;          //子节点
public:
    /*CGeoTransform()
    {
        m_wType = GEO_TYPE_TRANSFORM;
        m_mat = new CMatrix;
    }*/
    CGeoTransform(CMatrix* pMat=0){m_wType = GEO_TYPE_TRANSFORM; m_mat = (0 == pMat) ? new CMatrix : pMat;}
    virtual int GetObjDataLen() const {return 0; } //未实现
    virtual CGeoPt GetCentrePoint() const {return CGeoPt();}    //未实现

    //设置矩阵参数，该函数会替换原先的矩阵，替换后与替换者共享同一矩阵数据
    //如果替换后，修改了替换者的矩阵参数数据，则这里面的矩阵参数也跟随变化
    void SetSetTransformMatrix(CMatrix* pMat) {if (pMat != 0) m_mat = pMat;}
    //设置矩阵参数，该函数会复制现有的矩阵，替换后与替换者不共享同一矩阵数据，但是数据值相同
    //如果替换后，修改了替换者的矩阵参数数据，这里面的矩阵参数不会发生变化
    void SetTransformMatrix(const CMatrix& mat){ m_mat = new CMatrix(mat);}
    //获取矩阵参数
    CMatrix* GetTransformMatrixPtr() { return m_mat.get(); }
    //获取矩阵参数
    const CMatrix& GetTransformMatrix() const { return *m_mat; }
    void GetTransformMatrix(CMatrix& mat)const { mat= *m_mat; }

    //平移
    void Translate(FLOAT_T x, FLOAT_T y) { (*m_mat) *= CMatrix::Translate(x,y);}
    //旋转，角度为度
    void Rotate(FLOAT_T angle) { (*m_mat) *= CMatrix::Rotate(angle*M_PI/180); }
    //缩放
    void Scale(FLOAT_T x, FLOAT_T y) { (*m_mat) *= CMatrix::Scale(x, y); }
    void Scale(FLOAT_T mag) { (*m_mat) *= CMatrix::Scale(mag, mag); }

    //设置子节点，设置成功返回为true
    bool SetChild(CGeoBase* child);
    //添加子节点，添加成功返回为true
    bool AddChild(CGeoBase* child) { return SetChild(child); }
    inline CGeoBase* GetChild() { return m_geoChild.get(); }
    inline const CGeoBase* GetChild() const { return m_geoChild.get(); }
    virtual CGeoRect GetBoundBox() const;
};


//GDS文件的中的图元
class CGeoGDSElement : public CGeoTransform
{
protected:
    const static int    GDS_MAX_LAYER = 65535;
    const static int    GDS_MAX_COL = 32767;
    const static int    GDS_MAX_ROW = 32767;
    const static int    GDS_MAX_DATA_TYPE = 64;
protected:
    unsigned short  m_wLayer;
    unsigned short  m_wDataType;
    unsigned short  m_wFlags;
    unsigned        m_dwPlex;
    std::map<unsigned short, std::string> m_mapProperty;
    ref_ptr<CGeoBase>   m_parentStruct;

public:
    CGeoGDSElement()
    {
        m_dwPlex = 0;
        m_wFlags = 0;
        m_wLayer = m_wDataType = 0;
        m_wType = GEO_TYPE_ELEMENT;
    }
    inline CGeoBase* GetParent() { return m_parentStruct.get(); }
    inline const CGeoBase* GetParent() const { return m_parentStruct.get(); }
    inline void SetParent(CGeoBase* pParent) {m_parentStruct = pParent; }

    inline void SetLayerNo(unsigned short  wLayer) { m_wLayer = wLayer < GDS_MAX_LAYER ? wLayer : 0; }
    inline unsigned short GetLayerNo()const { return m_wLayer; }

    inline void SetDataType(unsigned short  wType) { m_wDataType = wType < GDS_MAX_DATA_TYPE ? wType : 0; }
    inline unsigned short GetDataType()const { return m_wDataType; }

    inline void SetPlex(unsigned  dwPlex) { m_dwPlex = dwPlex; }
    inline unsigned GetPlex()const { return m_dwPlex; }

    inline void SetFlags(unsigned short  wFlag) { m_wFlags = wFlag; }
    inline unsigned short GetFlags()const { return m_wFlags; }
    //添加GDS文件中的属性数据
    inline void AddElemntProperty(unsigned short wNum, const std::string& value) { m_mapProperty.insert(std::pair<unsigned short, std::string>(wNum, value)); }
    void SetElemntProperty(const std::map<unsigned short, std::string>& prolist) { m_mapProperty = prolist; }
    const std::map<unsigned short, std::string>& GetElemntProperty()const { return m_mapProperty; }
    const std::string& GetElemntProperty(unsigned short wNum)const
    {
        std::map<unsigned short, std::string>::const_iterator pos = m_mapProperty.find(wNum);
        if (pos != m_mapProperty.end())
            return pos->second;
        else
            return " ";
    }
    bool GetElemntProperty(unsigned short wNum, std::string& value)const
    {
        bool bOk = false;
        std::map<unsigned short, std::string>::const_iterator pos = m_mapProperty.find(wNum);
        if (pos != m_mapProperty.end())
        {
            value=pos->second;
            bOk = true;
        }
        return  bOk;
    }
    //设置Element子节点，该函数针对特殊的Element节点做特殊的处理，对于SREF和AREF节点要求采用该接口，它会自动添加cell节点的计数
    //该函数与SetChild函数的区别是针对阵列节点设置节点时，对于AREF节点，该函数会直接设构元为阵列节点的子节点，对于其他节点，与SetChild函数一样
    bool SetElementChild(CGeoBase* child);
    //该函数与SetElementChild函数对应，对于非AREF节点，等同于GetChild函数，如果是AREF节点，该函数会直接获取阵列节点的子节点，
    CGeoBase* GetElementChild();

    virtual unsigned GetPtNum()const { return 0; }
    virtual void AddCoordPt(FLOAT_T /*x*/, FLOAT_T /*y*/) {}
    virtual CGeoPt GetCoordPt(int /*nIndex=0*/) { return CGeoPt(); }
};

//GDS2 Struce
class CGeoGDSStruct : public CGeoMulripler
{
protected:
    std::string                        m_strName; 
    time_t                             m_tCreateTime;
    time_t                             m_tLastModifyTime;
    //int                                m_nRefCount;         //被引用次数，如果为0，则表示主cell
    //引用该构元的图元列表
    std::vector<ref_ptr<CGeoGDSElement> > m_vecRefElement;  //如果大小为0，则表示主cell
    std::set<unsigned short>           m_setLayNo;          //该cell属于的层
private:
    void InitData();

public:
    CGeoGDSStruct()
    {
        m_wType = GEO_TYPE_STRUCT; 
        InitData();
    }
    virtual int GetObjDataLen() const
    {
        return  CGeoMulripler::GetObjDataLen();
    }
    virtual CGeoPt GetCentrePoint() const
    {
        return CGeoPt(); //目前没有实现，有时间必须实现
    }
    virtual CGeoRect GetBoundBox() const
    {
        return CGeoMulripler::GetBoundBox();
    }

    //set name, time
    void SetName(const std::string& name) { m_strName = name; }
    const std::string& GetName() const {return m_strName;}
    void SetCreateTime(time_t tTime) { m_tCreateTime = tTime;}
    void SetLastModifyTime(time_t tTime) { m_tLastModifyTime = tTime; }
    void GetTime(time_t& tCreate, time_t& tModify) const {tCreate = m_tCreateTime;tModify = m_tLastModifyTime;}
    time_t GetCreateTime() const { return m_tCreateTime;}
    time_t GetModifyTime() const { return m_tLastModifyTime; }

    //该函数以后会被删除，用AddChild函数代替
    bool AddElement(CGeoGDSElement* pElement) { return AddChild(pElement);}
    //构元添加子节点(图元)，该函数限制添加的子节点必须是从CGeoGDSElement类派生的
    virtual bool AddChild(CGeoBase* pElement);
    //构元设置子节点(图元)，该函数限制子节点必须是从CGeoGDSElement类派生的
    virtual bool SetChild(unsigned nIndex, CGeoBase* pElement);
    //返回值得到子节点的类型数目
    //pType表示要求各类型数目的数组，每一个元素的高32为表示类型值如GEO_TYPE_PATH，低32为表示具体的数目
    //如果该参数为零或者nSize参数为零，则返回具体的类型数目，忽略其他参数
    //nSize参数表示pType数组大小，如果该大小小于实际的类型数据，则按照类型值得大小返回前面nSize个数据
    int GetAllChildTypeCount(ULLong* pType, int nSize) const;
    CGeoGDSElement* BackElement()
    { 
        CGeoBase* pTemp = BackChild();
        if (0== pTemp)
            return 0;
        else
            return dynamic_cast<CGeoGDSElement*>(pTemp);
    }
    CGeoGDSElement* FrontElement()
    {
        CGeoBase* pTemp = FrontChild();
        if (0 == pTemp)
            return 0;
        else
            return dynamic_cast<CGeoGDSElement*>(pTemp);
    }
    void DelElemet(CGeoGDSElement* pElement)
    {
        for (unsigned i = 0;i < m_vecChildren.size();i++)
        {
            if (m_vecChildren.at(i) == pElement)
                m_vecChildren.erase(m_vecChildren.begin()+i);
        }
    }

    //以下函数为REF图元引用该构元的信息存取函数
    //得到该构元被引用的次数
    inline int GetRefCount()const { return m_vecRefElement.size(); }
    //添加应用该构元的图元
    //引用该构元的图元数目等于GetRefCount()函数获取的数目
    //如果要获取引用该构元的构元，首先获取引用该构元的图元，然后获取图元所属的构元
    bool AddRefElement(CGeoGDSElement* pElement);
    //删除引用该构元的图元
    //成功删除返回true，否则返回false
    bool DelRefElement(CGeoGDSElement* pElement);
    //根据索引获取引用该构元的图元
    //找不到返回0
    CGeoGDSElement* GetRefElement(unsigned nIndex);
    //根据索引获取引用该构元的图元所属的构元
    //找不到返回0
    CGeoGDSStruct* GetRefCell(unsigned nIndex);
    //得到引用该构元的图元列表
    std::vector<ref_ptr<CGeoGDSElement> >& GetRefElementList() { return m_vecRefElement; }
    //得到该构元里面各种类型的子节点的数据
    
    //以下为构元的层操作函数
    //添加层号
    void AddLayerNO(WORD wNo) { m_setLayNo.insert(wNo); }
    //删除层号
    void DelLayerNO(WORD wNo) { m_setLayNo.erase(wNo); }
    //获取层号列表
    const std::set<unsigned short>& GetLayerList()const { return m_setLayNo; }
};

class CGeoGDSBoundary : public CGeoGDSElement
{
public:
    CGeoGDSBoundary()
    {
        m_wType = GEO_TYPE_BOUNDARY;
        CGeoLWPolyline *pTemp = new CGeoLWPolyline;
        pTemp->SetClose(true);  //必须闭合
        AddChild(pTemp);
    }
    virtual void AddCoordPt(FLOAT_T x, FLOAT_T y) {static_cast<CGeoLWPolyline*>(GetChild())->AddPt(x,y);}
    virtual CGeoRect GetBoundBox() const;
};

class CGeoGDSPath : public CGeoGDSBoundary
{
public:
    enum PATHTYPE { SQUARE_END_TYPE, ROUND_END_TYPE, SQUARE_END_EXT_TYPE, PATHTYPE_MAX };

protected:
    PATHTYPE    m_ePathType;

public:
    CGeoGDSPath()
    {
        m_ePathType = SQUARE_END_TYPE;
        m_wType = GEO_TYPE_PATH;
        static_cast<CGeoLWPolyline*>(GetChild())->SetClose(false);
    }
    void SetPathType(PATHTYPE eType) { m_ePathType = eType; }
    PATHTYPE GetPathType() const { return m_ePathType; }
    virtual CGeoRect GetBoundBox() const { return CGeoGDSBoundary::GetBoundBox(); }
};

class CGeoGDSBox : public CGeoGDSBoundary
{
protected:
    unsigned    m_uBoxType;
public:
    CGeoGDSBox()
    {
        m_uBoxType = 0;
        m_wType = GEO_TYPE_BOX;
    }
    void SetBoxType(unsigned uType) { m_uBoxType = uType; }
    unsigned GetBoxType() const { return m_uBoxType; }
    virtual CGeoRect GetBoundBox() const { return CGeoGDSBoundary::GetBoundBox(); }
};

class CGeoGDSNode : public CGeoGDSElement
{
protected:
    unsigned    m_uNodeType;
public:
    CGeoGDSNode()
    {
        m_uNodeType = 0;
        m_wType = GEO_TYPE_NODE;
    }
    void SetNodeType(unsigned uType) { m_uNodeType = uType; }
    unsigned GetNodeType() const { return m_uNodeType; }
    virtual CGeoRect GetBoundBox() const   // 如果是点，则边框是以点为中点向外扩展两个单位
    {
        CGeoRect   rect = m_geoChild->GetBoundBox();//点的数据没有设置
        if(rect.Width()>0){
            rect.m_dLeft   -= 2;
            rect.m_dRight  += 2;
        }else{
            rect.m_dLeft   += 2;
            rect.m_dRight  -= 2;
        }

        if(rect.Height()>0){
            rect.m_dTop     -= 2;
            rect.m_dBottom  += 2;
        }else{
            rect.m_dTop     += 2;
            rect.m_dBottom  -= 2;
        }

        return rect;
    }
};

//该子节点包含的为cell结构
class CGeoGDSSRef : public CGeoGDSElement
{
protected:
    //std::string     m_strRefStructName;   //根据讨论得出新的GDS规范修改
    FLOAT_T         m_dbRefX, m_dbRefY;     //参考点坐标，也许以后不需要

public:
    CGeoGDSSRef()
    {
        m_wType = GEO_TYPE_SREF;
        m_dbRefX = m_dbRefY = 0;
    }
    //void SetRefName(const std::string& strName) { m_strRefStructName = strName; }
    //const std::string& GetRefName() { return m_strRefStructName; }

    void SetReferencePt(FLOAT_T x, FLOAT_T y) { m_dbRefX = x;m_dbRefY=y; }
    CGeoPt GetReferencePt() const { return CGeoPt(m_dbRefX, m_dbRefY); } 
    virtual CGeoRect GetBoundBox() const;
};

//该子节点的子节点包含的为cell结构
class CGeoGDSARef : public CGeoGDSSRef
{
public:
    CGeoGDSARef()
    {
        m_wType = GEO_TYPE_AREF;
        AddChild(new CGeoArray);    //以后可能需要把阵列节点的数据直接集成到这里，这样可能达到更高的效率
    }
    //设置阵列的行数和列数
    void SetRowAndCol(int nRow, int nCol) {static_cast<CGeoArray*>(m_geoChild.get())->SetRowAndCol(nRow, nCol); }
    //获取阵列的行数和列数
    inline void GetRowAndCol(int& nRow, int& nCol) { static_cast<CGeoArray*>(m_geoChild.get())->GetRowAndCol(nRow, nCol); }
    //获取阵列的行数
    inline int GetRowNum() const { return static_cast<CGeoArray*>(m_geoChild.get())->GetRowNum(); }
    //获取阵列的列数
    inline int GetColNum() const { return static_cast<CGeoArray*>(m_geoChild.get())->GetColNum(); }

    //设置阵列的行间距和列间距
    void SetRowAndColInterDist(FLOAT_T dbRow, FLOAT_T dbCol) { static_cast<CGeoArray*>(m_geoChild.get())->SetRowAndColInterDist(dbRow, dbCol); }
    //获取阵列的行间距和列间距
    inline void GetRowAndColInterDist(FLOAT_T& dbRow, FLOAT_T& dbCol) { static_cast<CGeoArray*>(m_geoChild.get())->GetRowAndColInterDist(dbRow, dbCol); }
    //获取阵列的行间距
    inline FLOAT_T GetInterRowDist() const { return static_cast<CGeoArray*>(m_geoChild.get())->GetInterRowDist(); }
    //获取阵列的列间距
    inline FLOAT_T GetInterColDist() const { return static_cast<CGeoArray*>(m_geoChild.get())->GetInterColDist(); }

    virtual CGeoRect GetBoundBox() const;//对于子节点中包含椭圆的图元来说，这个边框计算的不够准确
};


class CGeoGDSText : public CGeoGDSElement
{
protected:
    unsigned short  m_wTextType;

public:
    CGeoGDSText()
    {
        m_wType = GEO_TYPE_GDS_TEXT;
        m_wTextType = 0;
        CGeoText *pTemp = new CGeoText;
        AddChild(pTemp);
    }
    void SetText(const std::string& strName) { static_cast<CGeoText*>(GetChild())->SetText(strName); }
    const std::string& GetText() { return static_cast<CGeoText*>(GetChild())->GetText(); }

    void SetTextType(unsigned short wData) { m_wTextType = wData; }
    unsigned short GetTextType() { return m_wTextType; }

    void SetPresentation(unsigned short wVal)
    {
        CGeoText* pTemp = static_cast<CGeoText*>(GetChild());
        pTemp->SetFontIndex(wVal & 0XC00);
        pTemp->SetAlignVer(wVal & 0X3000);
        pTemp->SetAlignHor(wVal & 0XC000);
    }

    virtual CGeoRect GetBoundBox() const;
};

//其他未定义的GDS图元，主要用于矢量图形格式
class CGeoGDSOther : public CGeoGDSElement
{
public:
    CGeoGDSOther()
    {
        m_wType = GEO_TYPE_OTHER;
    }
};

typedef std::vector<ref_ptr<CGeoGDSStruct> >    GeoStructList;
//图层
//图层保存的是element图元，不保存cell数据，但是从任何一个element都可以获取他所在的cell
class CGeoLayer : public Referenced
{
protected:
	//层类型，0表示未定义，1表示线路层
    unsigned long		m_dwType;
    unsigned long       m_dwLayerFlag;  //层标志，具体标志含义参考LAYER_FLAG_*的宏定义
    unsigned long       m_dwSize;       //在保存数据时，该数据充当长度，其他情况不用
    unsigned long       m_dwLayerNo;    //层号
    FLOAT_T		m_dbMinX, m_dbMinY, m_dbMaxX, m_dbMaxY; //层范围
    std::string m_strName;                              //层名字
    std::string m_strAnnotation;                        //层注记
    GeoObjList m_lstChildList;                          //数据列表
    ref_ptr<CGeoProperty>   m_Attribute;                //该层的属性数据
public:
    CGeoLayer()
    {
        m_dwType = 1; m_dwSize = 0;
        m_dbMinX = m_dbMinY = m_dbMaxX = m_dbMaxY = 0;
        m_dwLayerFlag = LAYER_FLAG_NONE;
    }
    virtual ~CGeoLayer(){}
public:
    unsigned long GetObjDataLen() const
    {
        unsigned long dwLen = m_strName.size() + m_strAnnotation.size() + sizeof(FLOAT_T)* 4 + 2 + sizeof(unsigned long)*3;
        for (unsigned i = 0; i < m_lstChildList.size(); i++)
        {
            dwLen += m_lstChildList.at(i)->GetObjDataLen();
        }
        return dwLen;
    }
    unsigned GetChildCount()const { return m_lstChildList.size(); }
	GeoObjList& GetObjList(){ return m_lstChildList; }
	unsigned GetLayperType(){ return m_dwType; }

    //添加属于该层的element图元
    virtual void AddObj(CGeoBase* lpObj){ m_lstChildList.push_back(lpObj); }    //以后添加删除更新图元时要自动计算出中心点和包围盒,注意，不要添加局部变量的指针
    virtual bool DelObj(unsigned nIndex);    //按照索引删除
    inline void SetType(unsigned long dwType) { m_dwType = dwType;}
    inline void SetName(const std::string& strName){ m_strName = strName; }
    inline const std::string& GetName(){ return m_strName; }
    CGeoProperty* GetProperty()const { return m_Attribute.get(); }                   //获取属性数据，如果返回值为0，表示没有设置属性
    //获取属性数据，如果没有设置属性,则自动生成
    CGeoProperty* GetForcedProperty()
    {
        if (!m_Attribute.valid())
            m_Attribute = new CGeoProperty;
        return m_Attribute.get();
    }
    void SetProperty(CGeoProperty* val) { m_Attribute = val; }                       //设置属性数据

    //标志相关
    inline void SetLock(bool bSet = true) { m_dwLayerFlag = bSet ? m_dwLayerFlag | LAYER_FLAG_LOCK : m_dwLayerFlag &(~LAYER_FLAG_LOCK);}
    inline bool IsLock() const { return (m_dwLayerFlag & LAYER_FLAG_LOCK) != 0;}
    inline void SetHide(bool bSet = true) { m_dwLayerFlag = bSet ? m_dwLayerFlag | LAYER_FLAG_HIDE : m_dwLayerFlag &(~LAYER_FLAG_HIDE); }
    inline bool IsHide() const { return (m_dwLayerFlag & LAYER_FLAG_HIDE) != 0; }

    inline void SetSelect(bool bSet = true) { m_dwLayerFlag = bSet ? m_dwLayerFlag | LAYER_FLAG_PROTECT : m_dwLayerFlag &(~LAYER_FLAG_PROTECT); }
    inline bool IsSelectable() const { return (m_dwLayerFlag & LAYER_FLAG_PROTECT) != 0; }
    inline void SetFill(bool bSet = true) { m_dwLayerFlag = bSet ? m_dwLayerFlag | LAYER_FLAG_FILL : m_dwLayerFlag &(~LAYER_FLAG_FILL); }
    inline bool IsFill() const { return (m_dwLayerFlag & LAYER_FLAG_FILL) != 0; }

    inline void SetLayerNo(unsigned long dwNo) { m_dwLayerNo = dwNo; }
    unsigned long GetLayerNo() { return m_dwLayerNo;}
};

typedef std::vector<ref_ptr<CGeoLayer> >        GeoLayerList;   //层列表
typedef std::vector<ref_ptr<CGeoGDSElement> >   GeoElementList;

//地图类型
class CGeoScene : public Referenced
{
protected:
	CGeoRect mapRect;                           //地图范围
    unsigned long   m_nDataVer;                   //地图文件数据版本
    unsigned long   m_dwLastNo;                   //最后的编号
    std::string     m_strName;                    //地图名
    FLOAT_T         m_dbUserUnit,m_dbMeterUnit;   //单位
    time_t          m_tCreate;
    time_t          m_tLastModify;
    time_t          m_tLastAcess;
    GeoStructList   m_vecGdsStructList;
    GeoLayerList    m_layerList;

public:
    CGeoScene();
    virtual ~CGeoScene(){}
    unsigned long GetObjDataLen() const
    {
        return 0;   //未实现
    }
    //modifed by bamoo 2017/09/13  为了提高效率
    GeoLayerList& GetLayerList() {return m_layerList;}
    const GeoLayerList& GetLayerList() const { return m_layerList; }

    GeoStructList& GetCellList() { return m_vecGdsStructList; }
    const GeoStructList& GetCellList() const { return m_vecGdsStructList; }

    //添加构元，成功返回true，失败返回false。
    //如果添加的构元与保存的有同名的，则会失败
    bool AddCell(CGeoGDSStruct* pCell);
    //设置构元，成功返回true，失败返回false。
    //如果bAdd为false，则如果找不到同名的构元，则返回失败。true则会添加。
    bool SetCell(CGeoGDSStruct* pCell, bool bAdd = true);
    //添加层
    void AddLayer(CGeoLayer* pLayer){m_layerList.push_back(pLayer);}
	int GetLayerNum() const{ return m_layerList.size(); }
	//获取图层
    CGeoLayer* GetLayerByIndex(unsigned nIndex)
	{
        if (nIndex >= m_layerList.size())
			return 0;
		else
			return m_layerList.at(nIndex).get();
	}

    //version data
    unsigned long GetVersion() const {return m_nDataVer;}
    void SetVersion(unsigned long dwVer) { m_nDataVer= dwVer; }
    //name
    const std::string& GetSceneName() const { return m_strName; }
    void SetSceneName(const std::string& name) { m_strName = name; }
    //time
    void SetSysTime(time_t tCreate, time_t tModify, time_t m_tAcess){ m_tCreate = tCreate; m_tLastModify = tModify; m_tLastAcess = m_tAcess; }
    void GetSysTime(time_t& tCreate, time_t& tModify, time_t& m_tAcess) const { tCreate= m_tCreate; tModify= m_tLastModify; m_tAcess= m_tLastAcess; }
    void SetUnit(FLOAT_T dbUser, FLOAT_T dbMeter) { m_dbUserUnit = dbUser; m_dbMeterUnit = dbMeter;}
};

#pragma pack()//恢复对齐状态

#endif

