#include"document.h"
#include <qevent.h>
#include <QPainter>
#include <QTextStream>
static const int resizeHandleWidth = 10;//调整图片大小右下点矩形范围
//不被其他文件使用
//全局静态变量是显示用static修饰的全局变量，作用域是所在的文件，其他的文件即使用extern声明也不能使用。
//非静态全局变量在由多个源文件组成的整个源程序都有效，静态全局变量只在定义了该变量的源文件有效，同一源
//源程序的其他源文件都不能使用它，静态全局变量的作用域在仅限一个源文件内

/******************************************************************************
** Shape
*/

const QSize ItemShape::minSize(80, 50);
//类的静态成员初始化在类外定义，因为它属于这个类而不属于类的对象
//Constructs a size with the given width and height

ItemShape::ItemShape(Type type)
{
     m_type=type;
}

ItemShape::Type ItemShape::type() const
{
    return m_type;
}
QString ItemShape::name() const
{
    return m_name;
}

QString ItemShape::typeToString(Type type)
{
    QString result;

    switch (type) {
        case Line:
            result = QLatin1String("Line");
            break;
        case Ellipse:
            result = QLatin1String("Ellipse");
            break;
        case Polygon:
            result = QLatin1String("Polygon");
            break;
        case Path:
            result = QLatin1String("Path");
            break;
        case Rect:
            result = QLatin1String("Rect");
            break;
        case Text:
            result = QLatin1String("Text");
            break;
        case Logo:
            result = QLatin1String("Logo");
            break;
    }

    return result;
}

ItemShape::Type ItemShape::stringToType(const QString &s, bool *ok)//将Qstring类型变成Type类型
{
    if (ok != 0)
        *ok = true;
    if (s == QLatin1String("Rect"))//QLatin1String可以在任何需要QString对象的地方使用，
        return Rect;//矩形
    if (s == QLatin1String("Line"))//QLatin1String可以在任何需要QString对象的地方使用，
        return Line;//线
    if (s == QLatin1String("Ellipse"))
        //QLatin1String类来更高效的利用const char*的类型，它就是一个关于const char*的一个浅封装。
        return Ellipse;//椭圆
    if (s == QLatin1String("Polygon"))
        return Polygon;//多边形
    if (s == QLatin1String("Path"))//QLatin1String可以在任何需要QString对象的地方使用，
        return Path;//路径
    if (s == QLatin1String("Text"))
        //QLatin1String类来更高效的利用const char*的类型，它就是一个关于const char*的一个浅封装。
        return Text;//文本
    if (s == QLatin1String("Logo"))
        return Logo;//logo

    if (ok != 0)
        *ok = false;//默认矩形
    return Rect;
}

/******************************************************************************
** Document
*/

Document::Document():m_coordViewOffset(0.0,0.0)
{
    setMinimumSize(400,400);
    paintscene=new PaintingWidget();
    setScene(paintscene);
    mag=1.0;
    angle=0.0;

}

void Document::addShape(ItemShape shape)//const被引用的内容不能变，加入一个shape到链表，返回的是加入链表的名字
{
    switch (shape.type()) {
        case ItemShape::Rect:
            paintscene->DrawRectItem();
            break;
        case ItemShape::Line:
            paintscene->DrawLineItem();
            break;
        case ItemShape::Ellipse:
            paintscene->DrawEllipseItem();
            break;
        case ItemShape::Polygon:
            paintscene->DrawPolygonItem();
            break;
        case ItemShape::Path:
            paintscene->DrawPathItem();
            break;
        case ItemShape::Text:
            paintscene->DrawTextItem();
            break;
        case ItemShape::Logo:
            paintscene->slotLogoItem();
            break;
    }
}
QList<QString> Document::cell()
{
    return List_structureCell;
}
void Document::loadfile(QString filename)
{
    List_structureCell.clear();//构元链表清空
    read.m_obaryStructures.clear();
   // imgFile.append(filename);
    read.ReadGDSPlain(filename);
    paintscene->DrawInit(read,this);
    QList<CGDSStructure*>::iterator iter_str;//QList<CGDSStructure*>链表迭代器
    for(iter_str=(read.m_obaryStructures).begin();iter_str!=(read.m_obaryStructures).end();iter_str++)
    {
        string name=(*iter_str)->GetStructureName();//获取每个构元的名字
        QString Name=QString::fromStdString(name);
        //QString Name= QString(QLatin1String(name));
        List_structureCell.push_back(Name);//存每个构元的链表
        paintscene->DrawStructure(*iter_str,m_coordViewOffset,false,angle,mag);//画每个构元
    }
}
void Document::drawStructure(QString qname,CGDS read)
{
    const char *cstrname = (qname.toStdString()).c_str();
    CGDSStructure *iter_str=read.FindStructure(const_cast<char*>(cstrname));
    paintscene->DrawStructure(iter_str,m_coordViewOffset,false,angle,mag);

}

QString Document::fileName() const
{
    return m_fileName;
}

void Document::setFileName(const QString &fileName)
{
    m_fileName = fileName;
}


