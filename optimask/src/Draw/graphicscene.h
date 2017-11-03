#ifndef GRAPHICSCENE
#define GRAPHICSCENE
#include <QtGui>
#include <QtWidgets>
#include <QGraphicsObject>
#include <set>
#include "./File/gds2reader.h"


const unsigned NUM_POLYGON_POINT = GDS_MAX_COORD + 1;

class PaintingWidget : public QGraphicsScene
{
    Q_OBJECT
public:
        PaintingWidget(QWidget *parent = 0);
        ~PaintingWidget();
        QPointF getScenePoint();
public slots:
        void slotClear();                         // 清除场景中所有的图元
        void DrawLineItem(QRect rect);            // 在场景中加入线
        void DrawEllipseItem(QRect rect);         // 在场景中加入一个椭圆形图元
        void DrawPolygonItem(QRect rect);         // 在场景中加入一个多边形图元
        void DrawPathItem(QRect rect = QRect());  // 在场景中加入一个多边线图元
        void DrawTextItem(QRect rect);            // 在场景中加入一个文字图元
        void DrawRectItem(/*QRect rect*/);
        void slotLogoItem(QRect rect = QRect());  // 在场景中加入一个Logo

protected:
        void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
        void mousePressEvent(QGraphicsSceneMouseEvent *event);
        void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:
      const double FitSclae;               // 图形与View窗口的合适比例
      QPointF      scenePoint;
      QPointF      offsetSceneToView;      // 场景与视图的偏移坐标
      double       ODBDrawScale;           // 每个像素的单位，化成了微米
      QPointF      m_startPot;             // 鼠标按下的起始点
      QPointF      m_endPot;               // 鼠标释放的结束点

      QPointF      points[NUM_POLYGON_POINT];//用QPointF点类型存放元素(如boundary,path)中所有点

public:
      void fmod360(double& angle);                                    // 将angle调整在正负360之间
      void RotatePoint(CCoord<double>* point, double& angle);                 // 将point所指示的CCoord类型点旋转angle度
      void RotatePoints(CCoord<double>* point, double& angle, unsigned num);  // 将point所指示的num个CCoord类型的点旋转angle度
      void RotatePoints(QPointF* point, double& angle, unsigned num); // 将point所指示的num个QPointF类型的点旋转angle度
      void RotatePoints(QPoint* point, double& angle, unsigned num);  // 将point所指示的num个QPoint类型的点旋转angle度
      void RotateRect(QRect* rect, double& angle);                    // 将rect所指示的矩形旋转angle度

      // 初始化画的参数
      void DrawInit(CGDS& read,QGraphicsView *view);


      // 以下的6个画函数的关系为：
      // DrawStructure调用包含有DrawAref(对应的为画AREF),DrawGDSElement(对应的为画Path,Boundary,Text),DrawStructure(递归调用自己画SREF)
      void DrawStructure(CGDSStructure* structure, CCoord<double>& pos, bool reflect, double& angle, double& mag);
      void DrawAref(CGDSAREF* element, CCoord<double>& pos, bool reflect, double& angle, double& mag);
      void DrawGDSElement(CGDSElement* element,CCoord<double>& pos,  bool reflect, double& angle, double& mag);

      //DrawGDSElement调用包含有DrawGDSPath(对应画Path),DrawGDSBoundary(对应画Boundary),DrawGDSText(对应画Text)
      void DrawGDSPath(CGDSPath *element,CCoord<double> pos, bool reflect, double angle, double mag);
      void DrawGDSBoundary(CGDSBoundary* element,CCoord<double> pos, bool  reflect, double angle, double mag);
      void DrawGDSText(CGDSText* element,CCoord<double> pos, bool reflect, double angle, double mag);

};

class CGraphicsItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit CGraphicsItem(QGraphicsScene *scene);
    ~CGraphicsItem();
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
protected:
    void             mousePressEvent(QGraphicsSceneMouseEvent *event);
    void             mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void             mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void             mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
private:
    QPointF lasetPoint;
    QPointF endPoint;
    QPixmap pix;
//    QPoint lasetPoint;
//    QPoint endPoint;
    QPixmap tempPix;
    bool isDrawing;
    bool isDoubleClick;
};

#endif // GRAPHICSCENE

