#include "alterdockwidget.h"

#include <QFrame>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QResizeEvent>

#include "./Layer/layertoolbox.h"

//AlterDockWidget
//构层配置工具箱
AlterDockWidget::AlterDockWidget(QWidget *parent, const char *name)
  : QWidget(parent)
{
  setObjectName(QString::fromLatin1(name));

//  // 1. 填充颜色(Fill Color)配置
//  LCPColorPalette *p = new LCPColorPalette(this, "Colors_Fill");
//  mp_palette = p;
//  add_panel(p, AlterDockWidget::to_string(QObject::tr("Fill Color")).c_str());
//  connect(p, SIGNAL(color_selected(QColor)), this, SLOT(fill_color_changed(QColor)));
//  connect(p, SIGNAL(color_brightness_selected(int)), this, SLOT(fill_color_brightness(int)));

  transformWidget = new TransformWidget(this);
  add_panel(transformWidget, "Transform");

  regenerateWidget = new RegenerateWidget(this);
  add_panel(regenerateWidget, "Regenerate");
  booleanWidget = new BooleanWidget(this);
  add_panel(booleanWidget, "Boolean");

  // make the height equal to the computed height
  int h = sizeHint().height();
  setMinimumHeight(h);
  setMaximumHeight(h);
}

AlterDockWidget::~AlterDockWidget()
{

}

void AlterDockWidget::add_panel(QWidget *panel_widget, const char *text)
{
    panel_widget->hide();

    QFrame *f = new QFrame(this);
    f->setAutoFillBackground(true);
    f->setObjectName(QString::fromLatin1("panel"));
    QHBoxLayout *l = new QHBoxLayout(f);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);

    f->setFrameStyle(QFrame::Panel | QFrame::Raised);
    f->setLineWidth(1);
    f->setBackgroundRole(QPalette::Highlight);

    QCheckBox *b = new QCheckBox(f);
    l->addWidget(b);

    b->setFocusPolicy(Qt::NoFocus);
    b->setBackgroundRole(QPalette::Highlight);
    QPalette pl(b->palette());
    pl.setColor(QPalette::Foreground, pl.color(QPalette::Active, QPalette::HighlightedText));
    b->setPalette(pl);
    b->setText(AlterDockWidget::to_qstring(text));
    b->setMaximumSize(QSize(b->maximumSize().width(), b->sizeHint().height() - 4));

    LCPRemitter *e = new LCPRemitter(int(m_tool_panels.size()), this);
    connect(b, SIGNAL(clicked()), e, SLOT(the_slot()));
    connect(e, SIGNAL(the_signal(int)), this, SLOT(panel_button_clicked(int)));

    m_tool_panels.push_back(std::make_pair(f, panel_widget));
}

QSize AlterDockWidget::sizeHint() const
{
    //  override the min width to account for the tree behaviour of Qt 4.5.x:
    int w = 148;
    for (std::vector <std::pair <QWidget *, QWidget *> >::const_iterator i = m_tool_panels.begin(); i != m_tool_panels.end(); ++i) {
        w = std::max (std::max(i->first->sizeHint().width(), i->second->sizeHint().width()), w);
    }

    //  get the required height
    int h = 0;
    for (std::vector <std::pair <QWidget *, QWidget *> >::const_iterator i = m_tool_panels.begin(); i != m_tool_panels.end(); ++i) {
        if (!i->second->isHidden()) {
            h += i->second->sizeHint().height();
        }
        h += i->first->sizeHint().height();
    }

    return QSize(w, h);
}

void AlterDockWidget::resizeEvent(QResizeEvent *re)
{
    rearrange(re->size().width(), re->size().height());
}

void AlterDockWidget::resize(int w, int h)
{
    QWidget::resize(w, h);
    rearrange(w, h);
}

void AlterDockWidget::setGeometry(int x, int y, int w, int h)
{
    QWidget::setGeometry(x, y, w, h);
    rearrange(w, h);
}

void AlterDockWidget::rearrange(int w, int h)
{
  for (std::vector <std::pair <QWidget *, QWidget *> >::iterator i = m_tool_panels.begin(); i != m_tool_panels.end(); ++i) {

    int hh;

    if (!i->second->isHidden()) {
      hh = i->second->sizeHint().height();
      h -= hh;
      i->second->setGeometry(0, h, w, hh);
    }

    hh = i->first->sizeHint().height();
    h -= hh;
    i->first->setGeometry(0, h, w, hh);

  }
}

void AlterDockWidget::panel_button_clicked(int index)
{
    if (index < 0 || index >= int(m_tool_panels.size())) {
      return;
    }

    if (!m_tool_panels[index].second->isHidden()) {
      m_tool_panels[index].second->hide();
    } else {
      m_tool_panels[index].second->show();
    }

    //make the height equal to the computed height
    int h = sizeHint().height();
    setMinimumHeight(h);
    setMaximumHeight(h);
}



//void AlterDockWidget::set_palette(const ColorPalette &p)
//{
//    mp_palette->set_palette(p);
//    mp_frame_palette->set_palette(p);
//}



