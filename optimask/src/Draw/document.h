#ifndef DOCUMENT
#define DOCUMENT
#include <QWidget>
#include<QGraphicsView>
#include <QFile>
#include "./File/gds2reader.h"
#include"graphicscene.h"
class ItemShape
{
public:
    enum Type {Line,Ellipse,Polygon,Path,Rect, Text, Logo};
    ItemShape(Type type);
    Type type() const;//不修改调用改函数的对象，不允许调用非const成员函数
    QString name() const;
    static QString typeToString(Type type);//静态成员函数，属于类
    static Type stringToType(const QString &s, bool *ok = 0);//静态成员函数，属于类
    static const QSize minSize;//只读的静态变量，整个类中都恒定的常量
    //类的static数据成员不是为每个对象所单独拥有的，而只是在整个类中存在一份，类的static数据成员与类类型
    //相关联，不管有木有对象生成，类的static都存在，可以通过类名访问。
private:
    Type m_type;//形状
    QString m_name;//名字
    friend class Document;//友元函数
};
class Document : public QGraphicsView
{
    Q_OBJECT

public:
    Document();
    void addShape(ItemShape shape);//添加
   // void deleteShape(const QString &shapeName);//删除
   // Shape shape(const QString &shapeName) const;
   // QString currentShapeName() const;//当前shape名字
   // void setShapeRect(const QString &shapeName, const QRect &rect);//设置shape矩形边界
   // void setShapeColor(const QString &shapeName, const QColor &color);//设置shape颜色

    void loadfile(QString filename);//打开GDSII文件

    QList<QString> cell();//图元
    void drawStructure(QString qname,CGDS read);

    QString fileName() const;//文件名
    void setFileName(const QString &fileName);//设置文件名

   // QUndoStack *undoStack() const;//undo栈
   CGDS* GetGDSIRead()
   {
       return &read;
   }
signals:
    void currentShapeChanged(const QString &shapeName);

protected:
    //void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    //void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
   // void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
   // void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    PaintingWidget *paintscene;
    QList<QString> List_structureCell; //获取GDSII中构元名字
    CGDS read;
    CCoord<double> m_coordViewOffset;
    double mag;
    double angle;
   // QVector<QString> imgFile;//获取打开的GDSII文件名字
    CGDSStructures *GDSStructure;
    CGDSStructure* boxStructure;

    void setCurrentShape(int index);
    int indexOf(const QString &shapeName) const;
    //第一个const表示引用参数在函数内不可以改变
    //最后一个const表示为常成员函数，不能改变对象的成员变量，也不能调用类中非const成员函数
    int indexAt(const QPoint &pos) const;
    QString uniqueName(const QString &name) const;
    QList<ItemShape> m_shapeList;//
    int m_currentIndex;
    int m_mousePressIndex;
    QPoint m_mousePressOffset;
    bool m_resizeHandlePressed;
    QString m_fileName;//每个文件有一个名字

};



#endif // DOCUMENT

