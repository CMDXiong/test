#ifndef VIEWPORTBOX
#define VIEWPORTBOX
#include <QtWidgets>
#include <QGraphicsRectItem>
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
class QKeyEvent;
class QWheelEvent;

class NavigatorViewBox : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
   // enum {Type = BoxItemType};
    explicit NavigatorViewBox(const QRect &rect, QGraphicsScene *scene);
    double angle() const { return m_angle; }
    double shearHorizontal() const { return m_shearHorizontal; }
    double shearVertical() const { return m_shearVertical; }

signals:
    void currentViewBoxZoom(const float &scaleFactor);


public slots:
    void setAngle(double angle);
    void setShearHorizontal(double shearHorizontal)
        { setShear(shearHorizontal, m_shearVertical); }
    void setShearVertical(double shearVertical)
        { setShear(m_shearHorizontal, shearVertical); }
    void setShear(double shearHorizontal, double shearVertical);

    void zoomIn();                   // 放大
    void zoomOut();                  // 缩小
    void zoom(float scaleFactor);    // 缩放 - scaleFactor：缩放的比例因子

protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
private:
    void updateTransform();

    bool m_resizing;
    double m_angle;
    double m_shearHorizontal;
    double m_shearVertical;
    qreal m_zoomDelta;               // 缩放的增量
};

#endif // VIEWPORTBOX



