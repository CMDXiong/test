#ifndef AlterDockWidget_H
#define AlterDockWidget_H

#include <QWidget>

#include "./Item/boxitem.hpp"
#include "./Item/textitem.hpp"
#include "./Item/textitemdialog.hpp"
#include "./Item/alt_key.hpp"
#include "./Item/aqp.hpp"
#include "./Item/brushwidget.hpp"
#include "./Item/penwidget.hpp"
#include "./Alter/transformwidget.hpp"
#include "./Alter/booleanwidget.h"
#include "./Alter/regeneratewidget.h"

//class BrushWidget;
//class TransformWidget;
//class PenWidget;

class AlterDockWidget : public QWidget
{
    Q_OBJECT
  public:
    AlterDockWidget(QWidget *parent, const char *name);
    ~AlterDockWidget();

    QString to_qstring(const std::string &s)
    {
      return QString::fromUtf8(s.c_str());
    }

    std::string to_string(const QString &s)
    {
      return std::string(s.toUtf8().constData());
    }

    virtual QSize sizeHint() const;
    virtual void resizeEvent(QResizeEvent *re);
    virtual void resize(int w, int h);
    virtual void setGeometry(int x, int y, int w, int h);

//    BrushWidget* getbrushWidget() {return brushWidget;}
    TransformWidget* getTransformWidget() {return transformWidget;}
//    PenWidget* getPenWidget() {return penWidget;}

  protected slots:
    void panel_button_clicked(int index);

  private:
    std::vector <std::pair <QWidget *, QWidget *> > m_tool_panels;
    void rearrange(int w, int h);
    void add_panel(QWidget *panel_widget, const char *text);

    TransformWidget *transformWidget;
    RegenerateWidget   *regenerateWidget;
    BooleanWidget   *booleanWidget;
};

#endif // AlterDockWidget_H
