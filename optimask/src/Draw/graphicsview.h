#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QRect>
#include <vector>

#include "./File/gds2reader.h"
#include "graphicscene.h"
#include "gdsdocument.h"
#include "./base/SceneRender.h"        //added by Bamboo 2017/08/05
#include "./base/Geo.h"
#include "./Edit/ccommandrecevierbase.h"

class QWheelEvent;
class PaintingWidget;
class CGdsDocument;


class COptimaskView:public QGraphicsView, public CSceneRender, public CCommandReceiverBase         //modified by Bamboo 2017/08/05
{
    Q_OBJECT

public:
    COptimaskView(CGdsDocument* documentData = NULL);
    ~COptimaskView();
    void             SetGdsDocument(CGdsDocument*gds);              // 设置view所引用的gdsdocument
    void             SetViewName(const QString &viewName);          // 设置文件名
    QString          ViewName() const;                              // 返回画板名
    CGdsDocument*    GetGdsDocument()const;
    PaintingWidget*  getGraphicsScene();

    inline QPointF   GeoPtToQPoint(const CGeoPt& point) const;         // 将CGeoPt类型的点转换成QPointF
    inline QRectF    GeoRectToQRectF(const CGeoRect& rect) const;   // 将CGeoRect类型的点转换成QRectF
    inline CGeoPt    QPointFToGeoPt(QPointF& point) const;                 // 将QPointF类型的点转换成CGeoPt
    inline CGeoPt    QPointFToGeoPt(QPoint& point) const;                 // 将QPoint类型的点转换成CGeoPt
signals:
    void             CurrentViewChanged();                           // 当前画板添加或者删除了新的图形
    void             currentShapeChanged(const QString &shapeName);
public slots:
    void             drawShape(const int type);
protected:
    void             DrawArc(QPainter painter);
public:
    void             drawType(const int type);
    void             isSelect(bool select);
    void             DrawWithMouse(const int type);  // Draw All Shape With mouse

    //Begin Added by Bamboo 2017/08/05 --------------------------
    //virtual void    DrawScene();
    virtual void    DrawBasePrimitive(CGeoBase* pData);
    virtual void    DrawPrimitive(CGeoPoint* pData) ;
    virtual void    DrawPrimitive(CGeoLine* pData) ;
    virtual void    DrawPrimitive(CGeoCircle* pData);
    virtual void    DrawPrimitive(CGeoRectangle* pData);
    virtual void    DrawPrimitive(CGeoArc* pData);
    virtual void    DrawPrimitive(CGeoLWPolyline* pData);
    virtual void    DrawPrimitive(CGeoEllipse* pData);
    virtual void    DrawPrimitive(CGeoText* pData);
    virtual void    DrawPrimitive(CGeoMulripler* pData);
    virtual void    DrawPrimitive(CGeoTransform* pData);
    virtual void    DrwaPrimitive(CGeoArray* pData);
    void            DrawItemFrame(CGeoBase* pData);//画图元的包围盒

    void            DrawStructure(CGeoGDSStruct* pCell);
    void            DrawStructure(std::string sname);
    CGeoGDSStruct*  FindStructureByName(std::string sname);
    unsigned int    GeoTransformReferenceLevel(CGeoTransform* trans);        // 一种CGeoTransform类型图元的引用层次
    unsigned int    GeoStructureReferenceLevel(CGeoGDSStruct* trans);        // 一种CGeoTransform类型图元的引用层次

    //End -------------------------------------------------------
    void            DrawMainStructure();                                     // 画主构元
    void            DrawMainStructureByName(std::string sname);              // 根据主构无的名字画主构元
    CGeoGDSStruct*  FindMainStructure();                                     // 找主构元
    QRect           setViewPort();                                           // 设置显示整个Frame的视口
    void            SetMap(CGeoGDSStruct* pData);                            // 建立映射关系
    void            SetGeoStructureMap(CGeoGDSStruct* pData);                // 设置GeoGDSStructure的Map映射
    void            SetGeoTransformMap(ref_ptr<CGeoTransform> pData);        // 设置GeoTransform图元的Map映射

    void TestData();
private:
    FLOAT_T multiple;                       // 1个物理坐标单位对应multiple个逻辑坐标单位
    QRect   viewFrame;                      // 整个构元包围盒对应的物理坐标范围
    QRect  windowFrame;                    // 构元包围盒范围

    //Begin Added by Xiong 2017/09/05 有关选中的函数--------------------------
public:
    void         SetSceneFrame();                  // 场景中所有图形区域边框
    CGeoRect     LayerFrame(const int& n) const;                  // 第n层的图形区域边框
    CGeoRect     Frame(const std::vector<FLOAT_T>& xVector, const std::vector<FLOAT_T>& yVector) const;
    //Begin Added by Xiong 2017/09/12-------------------------
    inline void  SetRow(int row);      //行暂时这样设置，最后根据图元的数据来调整
    inline void  SetColumn(int column);//列暂时这样设置，最后根据图元的数据来调整
    void         SetHeight(const CGeoRect& frame);
    void         SetWidth(const CGeoRect& frame);
    unsigned int GetID( QPoint point);// 得到点所在的分区

    bool         RectContainPoint(const ref_ptr<CGeoRectangle> rect, const QPoint& point);                  //判断一个矩形是否包含一个点
    bool         LWPolyContainPoint(ref_ptr<CGeoLWPolyline> LWPolyline, const QPoint &point);
    bool         LineContainPoint(const ref_ptr<CGeoLine>& line, const QPoint& point);
    bool         CircleContainPoint(const ref_ptr<CGeoCircle>& circle, const QPoint& point);
    bool         ArcContainPoint(const ref_ptr<CGeoArc>& arc, const QPoint& point);
    bool         EllipseContainPoint(const ref_ptr<CGeoEllipse>& ellipse, const QPoint& point);
    bool         TransformContainPoint(const ref_ptr<CGeoTransform>& trans, const QPoint& point);           //这个CGeoTransform中子节点一定要是基础元素

    void         GetListcontainPointAndMinArea();

    //Begin Added by Xiong 2017/09/18-------------------------
    FLOAT_T FrameArea(CGeoRect rect) const;
    void GetAreaMinItem(CGeoBase* base,  std::list<ref_ptr<CGeoBase> >& list,FLOAT_T& minFrameArea) const;

    //End Added by Xiong 2017/09/18-------------------------
    void UpdateSceneFrame(ref_ptr<CGeoBase> base);
    void SetItemMap(ref_ptr<CGeoBase> base);

    void setCurrentItem(ref_ptr<CGeoBase> item);
private:
    typedef std::multimap<unsigned int , ref_ptr<CGeoBase> >::iterator MultiMapIterator;
    std::multimap<unsigned int, ref_ptr<CGeoBase> > m_mapItem; // 分区ID与其包含图元的对应关系
    bool m_bSelectEvent ;    // 中心窗口中的选中事件
    CGeoRect m_sceneFrame;
    QPen m_pen;
    ref_ptr<CGeoBase> m_rCurrentItem;  // 当前选中的图元

    int m_nrow;                // 初始化的m_nrow行栅格
    int m_ncolumn;             // 初始化的m_ncolumn列栅格
    int idWidth;               // 初始化一个栅格的宽度(图元边框坐标（逻辑坐标）计算得到)
    int idHeight;              // 初始化一个栅格的高度


    // 以下两个变量的初始化应该为什么值？
    FLOAT_T m_dboriginMapItemX; // 算一个点的ID都需要与这个值相减，即图元边框的左上点x值
    FLOAT_T m_dboriginMapItemY; // 算一个点的ID都需要与这个值相减，即图元边框的左上点y值

    //End -------------------------------------------------------
public:
     ref_ptr<CGeoBase> GetSelectItem();  //  得到当前选中图元
     void SetPen(QPen pen);              //  设置笔

public:
    // 与坐标位置有关
    //Begin Added by Xiong 2017/09/28-------------------------
    QPointF mapToWindow(const QPoint point);            // viewport(物理坐标)映射到window(逻辑坐标)
    QPointF mapToWindow(const QPointF point);            // viewport(物理坐标)映射到window(逻辑坐标)
    QPointF mapToWindow(const int dx, const int dy);    // viewport(物理坐标)映射到window(逻辑坐标)
    QPointF mapToWindow(const double dx, const double dy);    // viewport(物理坐标)映射到window(逻辑坐标)
    QPointF mapFromWindow(const QPointF point);         // window(逻辑坐标)映射到viewport(物理坐标)
    QPointF mapFromWindow(FLOAT_T dx, FLOAT_T dy);      // window(逻辑坐标)映射到viewport(物理坐标)

    //End Added by Xiong 2017/09/28-------------------------

    //Begin Added by Xiong 2017/09/14-------------------------
public:
    virtual void DrawScene();
    //End -------------------------------------------------------

protected:
    void             paintEvent(QPaintEvent *event)            Q_DECL_OVERRIDE;
    void             mousePressEvent(QMouseEvent *event)       Q_DECL_OVERRIDE;
    void             mouseReleaseEvent(QMouseEvent *event)     Q_DECL_OVERRIDE;
    void             mouseMoveEvent(QMouseEvent *event)        Q_DECL_OVERRIDE;
    void             mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void             setCurrentShape(int index);                   // 当前栈在的第一个图元
    int              indexOf(const QString &shapeName) const;      // 搜索图元
    int              indexAt(const QPoint &pos) const;             // 选中事件中，反回在点pos处的图元的索引
    QString          uniqueName(const QString &name) const;        // 给图元设置一个独一无二的名字

private:
    int distance(const QPoint &point1, const QPoint &point2); //计算两点之间的距离
    double distance(const QPointF &point1, const QPointF &point2); //计算两点之间的距离
    PaintingWidget*     paintscene;                       // 场景容器，画图
    CCoord<double>      m_coordViewOffset;                // 画gds图需要的参数变量
    double              mag;
    double              angle;
    QString             m_ViewName;                       // 每个文件有一个名字
    CGdsDocument*       m_gdsdocument;                    // 引用的gdsdocument的指针变量
    int                 m_currentIndex;                   // 当前图元的索引
    int                 m_mousePressIndex;
    QPoint              m_mousePressOffset;
    bool                m_resizeHandlePressed;
    CCoord<int>         m_aryCoordBuf[GDS_MAX_COORD];     // 图元的坐标数组
    CGDSBoundary        boxstructure;                     // 矩形图元转成在的CGDSBoundary
    // 用鼠标画图中需要的变量
    QPixmap        pix;                       // 双缓冲绘制图元
    QPixmap        tempPix;              // 双缓冲拖动时绘制图元
    QPainter       painter;
    bool           isDrawing;           // 一个图元的绘制过程中
    bool           isDoubleClick;

    //start----------------------Add by xiong 17/09/05
    CGeoPt  m_geoStartPos;
    CGeoPt  m_geoLasetPos;
    CGeoPt m_geoEndPos;
    std::vector<CGeoPt> m_allPoint;   // 存储多义线的点
    //end----------------------Add by xiong 17/09/05

    QPoint           m_startPos;           // 起始点
    QPoint           m_lasetPos;           // 上一个点
    QPoint           m_endPos;             // 结束点
    bool             m_bFirstPoint;        // 标记是否是所画图元的第一个点
    int               m_nNumPoint;         // 记录跟踪一个图元点的个数

    int  m_shapeType; //所画图元的类型
    bool m_bDrawEvent;       // 中心窗口中的画图元事件
    int oldIndex;

public:
    void SetDrawEvent(bool drawEvent);
    void SetSelectEvent(bool selectEvent);
    QList<CGDSElement*> shapeStructureList1;            // 栈中图元转化成的element链表
    bool m_parent;      // 标记是子结构图还是父结构图
    //-------------------------缩放-----------------------------------------------------------

public:
    // 缩放的增量
    void setZoomDelta(qreal delta);
    qreal zoomDelta() const;
    // 画比例尺
    void DrawRuler();
protected:
    // 放大/缩小
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *);
public Q_SLOTS:
    void zoomIn();                   // 放大
    void zoomOut();                  // 缩小
    void zoom(float scaleFactor);    // 缩放 - scaleFactor：缩放的比例因子

private:
    qreal m_zoomDelta;               // 缩放的增量
    //QPoint m_lastMousePos;         // 鼠标最后按下的位置
    qreal m_scale;                   // 缩放值
    qreal m_oldscale;
    qreal m_ruler;
    qreal m_oldruler;
    QString ruler;

    double startAngle;
    double endAngle;

    std::vector<QPoint> m_vecPoint;

    //---------平移-----------------------------------------------------------------------
public:
    // 平移速度
    void setTranslateSpeed(qreal speed);
    qreal translateSpeed() const;
public Q_SLOTS:
    void tranSlate(QPointF delta);  // 平移
signals:
    // void signalTranslate(QPointF delta);
private:
    Qt::MouseButton m_translateButton;  // 平移按钮
    qreal m_translateSpeed;  // 平移速度
    bool m_bMouseTranslate;  // 平移标识
    QPoint m_lastMousePos;  // 鼠标最后按下的位置

public slots:
    void DrawRectWithCommand(CGeoPt ,CGeoPt);
    void DrawLineWithCommand(CGeoPt ,CGeoPt);
    void DrawPolygonWithCommand(std::vector<CGeoPt>,bool);
    void DrawEllipseWithCommand(CGeoPt ,double ,double );
    void DrawArcWithCommand(CGeoPt, double,double,double);

public:
    bool UnAction(CCommandBase* command);    // Undo对应的操作
    bool Action(CCommandBase* command);      //  Redo对应的操作
};

#endif // GRAPHICSVIEW_H

