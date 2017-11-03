#ifndef LAYNAVIGATOR
#define LAYNAVIGATOR
#include <QWidget>
#include <QGraphicsView>
#include "viewportbox.h"
class CGdsDocument;
class Navigator : public QGraphicsView
{
    Q_OBJECT
public:
    Navigator();
    void createRect();
    void createViewBox();
    void SetGdsDocument(CGdsDocument* doc);
    CGdsDocument* GetGdsDocument()const;

signals:
    void viewBoxchanged(const float &scaleFactor);
public slots:
    void viewBoxZoom(const float &scaleFactor);
    void setBoxzoom(const float &scaleFactor);

private:
    QGraphicsScene *paintscene;//场景容器，画图
    NavigatorViewBox *viewBox ;
    CGdsDocument *m_gdsdoc;
//---------------------平移------------------------------------------------------
protected:
    void             mousePressEvent(QMouseEvent *event)       Q_DECL_OVERRIDE;
    void             mouseReleaseEvent(QMouseEvent *event)     Q_DECL_OVERRIDE;
    void             mouseMoveEvent(QMouseEvent *event)        Q_DECL_OVERRIDE;
signals:
    void             tranSlate(const QPointF &point);
  //  void             slotTranslate(const QPointF &point);
private:
    QPoint           startPoint;
    bool             m_bMouseTranslate;
};


#endif // LAYNAVIGATOR

