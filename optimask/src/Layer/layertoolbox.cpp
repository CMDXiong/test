/* ========================================================================== */
/* LAYERTOOLBOX.CPP -- 构层配置(Layer Palette)(色板,格纹,动画,样式,能见度)构建
 *   构层色板(Layer Palette)
 *       构层边框颜色(Frame Color)
 *       构层填充颜色(Fill Color)
 *       构层填充格纹(Stipple, Fill Pattern)
 *       构层动画(Animation)
 *       构层样式(Style)
 *       构层能见度(Visibility)
 *
 * REFERENCE:
 *
 * COPYRIGHT 2017 Optixera.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.01$ (20171002) */
/* HISTORY: 20161027(v1.00) Initial Version;
 * ========================================================================== */

#include "./Layer/layertoolbox.h"
#include "./Item/brushwidget.hpp"
#include "./Item/penwidget.hpp"

#include <QVBoxLayout>
#include <QColorDialog>
#include <QMouseEvent>
#include <QCheckBox>

//#include <QGraphicsRectItem>
//#include <QGraphicsScene>
//#include <QGraphicsView>

LCPRemitter::LCPRemitter(int index, QObject *parent, const char *name)
  : QObject(parent),
    m_index(index)
{
    setObjectName(QString::fromLatin1(name));
}

void LCPRemitter::the_slot()
{
    emit the_signal(m_index);
}
#include <QDebug>
void LCPColorPalette::slotColor(int color)       // 在场景中加入一个矩形图元
{
    QColor colorInf;
    colorInf = QColor(m_palette.color_by_index(color));
    emit frameColor(colorInf);
}


LCPColorPalette::LCPColorPalette(QWidget *parent, const char *name)
  : QFrame(parent)
{
    setObjectName(QString::fromLatin1(name));
    QSizePolicy sp(QSizePolicy::Ignored, QSizePolicy::Ignored);
    sp.setHorizontalStretch(0);
    sp.setVerticalStretch(0);

    QVBoxLayout *l = new QVBoxLayout(this);
    l->setMargin(0);
    l->setSpacing(0);
    for (unsigned int i = 0; i < 6; ++i) {
        QFrame *f = new QFrame(this);
        f->setObjectName(QString::fromLatin1("color_f"));
        l->addWidget(f);
        QHBoxLayout *ll = new QHBoxLayout(f);
        ll->setMargin(0);
        ll->setSpacing(0);
        for (unsigned int j = 0; j < 7; ++j) {
            unsigned int n = j * 6 + i;
            LCPActiveLabel *b = new LCPActiveLabel(n, f);
            while (m_color_buttons.size() <= n) {
                m_color_buttons.push_back(0);
            }
            m_color_buttons [n] = b;
            b->setMinimumSize(16, 16);
            b->setLineWidth(1);
            b->setSizePolicy(sp);
            b->setText(QString());
            connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
            connect(b, SIGNAL(clicked(int)), this, SLOT(slotColor(int)));

            ll->addWidget(b);
        }
    }

  {
    //  darker and brighter colors
    QFrame *f = new QFrame(this);
    f->setObjectName(QString::fromLatin1("color_l1"));
    l->addWidget(f);
    QHBoxLayout *ll = new QHBoxLayout(f);
    ll->setMargin(0);
    ll->setSpacing(0);

    LCPActiveLabel *b;

    //  No color
    b = new LCPActiveLabel(-1, f);
    b->setFrameStyle(QFrame::Panel | QFrame::Raised);
    b->setLineWidth(1);
    b->setText(QObject::tr("None"));
    b->setBackgroundRole(QPalette::Button);
    connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
    ll->addWidget(b);

    //  More colors ..
    b = new LCPActiveLabel(-2, f);
    b->setFrameStyle(QFrame::Panel | QFrame::Raised);
    b->setLineWidth(1);
    b->setText(QObject::tr("More .."));
    b->setBackgroundRole(QPalette::Button);
    connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
    ll->addWidget(b);
  }

  {
    //  darker and brighter colors
    QFrame *f = new QFrame(this);
    f->setObjectName(QString::fromLatin1("color_l2"));
    l->addWidget(f);
    QHBoxLayout *ll = new QHBoxLayout(f);
    ll->setMargin(0);
    ll->setSpacing(0);

    LCPActiveLabel *b;

    QLabel *lbl = new QLabel(QObject::tr("S/V"), f);
    ll->addWidget(lbl);

    b = new LCPActiveLabel(-10, f);
    b->setFrameStyle(QFrame::Panel | QFrame::Raised);
    b->setLineWidth(1);
    b->setPixmap(QPixmap(QString::fromLatin1(":dark.png")));
    b->setBackgroundRole(QPalette::Button);
    b->setAlignment(Qt::AlignHCenter);
    connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
    ll->addWidget(b);

    b = new LCPActiveLabel(-11, f);
    b->setFrameStyle(QFrame::Panel | QFrame::Raised);
    b->setLineWidth(1);
    b->setPixmap(QPixmap(QString::fromLatin1(":bright.png")));
    b->setBackgroundRole(QPalette::Button);
    b->setAlignment(Qt::AlignHCenter);
    connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
    ll->addWidget(b);

    b = new LCPActiveLabel(-12, f);
    b->setFrameStyle(QFrame::Panel | QFrame::Raised);
    b->setLineWidth(1);
    b->setPixmap(QPixmap(QString::fromLatin1(":neutral.png")));
    b->setBackgroundRole(QPalette::Button);
    b->setAlignment(Qt::AlignHCenter);
    connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
    ll->addWidget(b);
  }

}

void LCPColorPalette::set_palette(const ColorPalette &palette)
{
  if (palette != m_palette) {

    m_palette = palette;

    for (unsigned int i = 0; i < m_color_buttons.size(); ++i) {
      QColor color;
      if (i < m_palette.colors()) {
        color = QColor(m_palette.color_by_index(i));
      }
      if (m_color_buttons [i]) {
        QPalette pl;
        pl.setColor(QPalette::Window, color);
        m_color_buttons [i]->setPalette(pl);
      }
    }
  }

}

void LCPColorPalette::button_clicked(int index)
{
  if (index >= 0) {
    QColor color;
    if (index < int(m_palette.colors())) {
      color = QColor(m_palette.color_by_index(index));
    }
    emit color_selected(color);
  } else if (index == -1) {
    //  no color
    emit color_selected(QColor());
  } else if (index == -2) {
    QColor c = QColorDialog::getColor();
    if (c.isValid()) {
      emit color_selected(c);
    }
  } else if (index == -10) {
    emit color_brightness_selected(-16); // darker
  } else if (index == -11) {
    emit color_brightness_selected(16); // brighter
  } else if (index == -12) {
    emit color_brightness_selected(0); // brighter
  }
}

LCPActiveLabel::LCPActiveLabel(int index, QWidget *parent, const char *name)
  : QLabel(parent), m_index(index), m_grabbed(false)
{
  setAutoFillBackground(true);
  setObjectName(QString::fromLatin1(name));
  setFrameStyle(QFrame::Panel | QFrame::Raised);
  setBackgroundRole(QPalette::Window);
}

void LCPActiveLabel::mousePressEvent(QMouseEvent *e)
{
  if (! m_grabbed && e->button() == Qt::LeftButton) {
    setFrameShadow(QFrame::Sunken);
    m_grabbed = true;
  }
}

void LCPActiveLabel::mouseReleaseEvent(QMouseEvent *e)
{
  if (m_grabbed) {

    setFrameShadow(QFrame::Raised);
    m_grabbed = false;

    if (e->button() == Qt::LeftButton && rect().contains(e->pos())) {
      emit clicked(m_index);
    }

  }
}

// LCPAnimationPalette implementation
LCPAnimationPalette::LCPAnimationPalette(QWidget *parent, const char *name)
  : QFrame(parent)
{
  setObjectName(QString::fromLatin1(name));

  QVBoxLayout *l = new QVBoxLayout(this);
  l->setMargin(0);
  l->setSpacing(0);

  QFrame *f;
  LCPActiveLabel *b;
  QHBoxLayout *ll;

  QSizePolicy sp(QSizePolicy::Ignored, QSizePolicy::Ignored);
  sp.setHorizontalStretch(0);
  sp.setVerticalStretch(0);

  f = new QFrame(this);
  f->setObjectName(QString::fromLatin1("anim_f"));
  l->addWidget(f);
  ll = new QHBoxLayout(f);
  ll->setMargin(0);
  ll->setSpacing(0);

  b = new LCPActiveLabel(0, f);
  b->setMinimumSize(50, 16);
  b->setAlignment(Qt::AlignCenter);
  b->setLineWidth(1);
  b->setSizePolicy(sp);
  b->setText(QObject::tr("None"));
  connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
  ll->addWidget(b);

  b = new LCPActiveLabel(1, f);
  b->setMinimumSize(50, 16);
  b->setAlignment(Qt::AlignCenter);
  b->setLineWidth(1);
  b->setSizePolicy(sp);
  b->setText(QObject::tr("Scroll"));
  connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
  ll->addWidget(b);

  f = new QFrame(this);
  l->addWidget(f);
  ll = new QHBoxLayout(f);
  ll->setMargin(0);
  ll->setSpacing(0);

  b = new LCPActiveLabel(2, f);
  b->setMinimumSize(50, 16);
  b->setAlignment(Qt::AlignCenter);
  b->setLineWidth(1);
  b->setSizePolicy(sp);
  b->setText(QObject::tr("Blink"));
  connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
  ll->addWidget(b);

  b = new LCPActiveLabel(3, f);
  b->setMinimumSize(50, 16);
  b->setAlignment(Qt::AlignCenter);
  b->setLineWidth(1);
  b->setSizePolicy(sp);
  b->setText(QObject::tr("/Blink"));
  connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
  ll->addWidget(b);
}

void LCPAnimationPalette::button_clicked(int index)
{
  emit animation_selected(index);
}

//  LCPStylePalette implementation
LCPStylePalette::LCPStylePalette(QWidget *parent, const char *name)
  : QFrame(parent)
{
  setObjectName(QString::fromLatin1(name));

  QVBoxLayout *l = new QVBoxLayout(this);
  l->setMargin(0);
  l->setSpacing(0);

  QFrame *f;
  LCPActiveLabel *b;
  QHBoxLayout *ll;

  QSizePolicy sp(QSizePolicy::Ignored, QSizePolicy::Ignored);
  sp.setHorizontalStretch(0);
  sp.setVerticalStretch(0);

  f = new QFrame(this);
  f->setObjectName(QString::fromLatin1("style_f"));
  l->addWidget(f);
  ll = new QHBoxLayout(f);
  ll->setMargin(0);
  ll->setSpacing(0);

  b = new LCPActiveLabel(0, f);
  b->setMinimumSize(25, 16);
  b->setAlignment(Qt::AlignCenter);
  b->setLineWidth(1);
  b->setSizePolicy(sp);
  b->setText(QObject::tr("0px"));
  connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
  ll->addWidget(b);

  b = new LCPActiveLabel(1, f);
  b->setMinimumSize(25, 16);
  b->setAlignment(Qt::AlignCenter);
  b->setLineWidth(1);
  b->setSizePolicy(sp);
  b->setText(QObject::tr("1px"));
  connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
  ll->addWidget(b);

  b = new LCPActiveLabel(2, f);
  b->setMinimumSize(25, 16);
  b->setAlignment(Qt::AlignCenter);
  b->setLineWidth(1);
  b->setSizePolicy(sp);
  b->setText(QObject::tr("2px"));
  connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
  ll->addWidget(b);

  b = new LCPActiveLabel(3, f);
  b->setMinimumSize(25, 16);
  b->setAlignment(Qt::AlignCenter);
  b->setLineWidth(1);
  b->setSizePolicy(sp);
  b->setText(QObject::tr("3px"));
  connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
  ll->addWidget(b);

  f = new QFrame(this);
  l->addWidget(f);
  ll = new QHBoxLayout(f);
  ll->setMargin(0);
  ll->setSpacing(0);

  b = new LCPActiveLabel(100, f);
  b->setMinimumSize(50, 16);
  b->setAlignment(Qt::AlignCenter);
  b->setLineWidth(1);
  b->setSizePolicy(sp);
  b->setText(QObject::tr("Simple"));
  connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
  ll->addWidget(b);

  b = new LCPActiveLabel(101, f);
  b->setMinimumSize(50, 16);
  b->setAlignment(Qt::AlignCenter);
  b->setLineWidth(1);
  b->setSizePolicy(sp);
  b->setText(QObject::tr("Marked"));
  connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
  ll->addWidget(b);
}

void LCPStylePalette::button_clicked(int index)
{
  if (index >= 0 && index < 16) {
    emit width_selected (index);
  } else if (index == 100) {
    emit marked_selected(false);
  } else if (index == 101) {
    emit marked_selected(true);
  }
}

//  LCPDitherPalette implementation
LCPDitherPalette::LCPDitherPalette(QWidget *parent, const char *name)
  : QFrame(parent)
{
  setObjectName(QString::fromLatin1(name));

  int n = 0;
  QVBoxLayout *l = new QVBoxLayout(this);
  l->setMargin(0);
  l->setSpacing(0);

  for (unsigned int i = 0; i < 4; ++i) {

    QFrame *f = new QFrame(this);
    f->setObjectName(QString::fromLatin1("dp_f"));
    l->addWidget(f);
    QHBoxLayout *ll = new QHBoxLayout(f);
    ll->setMargin(0);
    ll->setSpacing(0);

    for (unsigned int j = 0; j < 4; ++j) {

      LCPActiveLabel *b = new LCPActiveLabel(n, f);

      b->setMinimumSize(28, 28);
      b->setAlignment(Qt::AlignCenter);
      b->setLineWidth(1);

      QSizePolicy sp(QSizePolicy::Ignored, QSizePolicy::Ignored);
      sp.setHorizontalStretch(0);
      sp.setVerticalStretch(0);
      b->setSizePolicy(sp);

      create_pixmap_for(b, n);
      m_stipple_buttons.push_back(b);

      connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));

      ll->addWidget(b);

      ++n;

    }

  }

  {
    QFrame *f = new QFrame(this);
    f->setObjectName(QString::fromLatin1("dp_ll"));
    l->addWidget(f);
    QHBoxLayout *ll = new QHBoxLayout(f);
    ll->setMargin(0);
    ll->setSpacing(0);

    LCPActiveLabel *b;

    //  No pattern
    b = new LCPActiveLabel(-3, f);
    b->setFrameStyle(QFrame::Panel | QFrame::Raised);
    b->setLineWidth(1);
    b->setText(QObject::tr("None"));
    b->setBackgroundRole(QPalette::Button);
    connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
    ll->addWidget(b);

    //  More pattern
    b = new LCPActiveLabel(-2, f);
    b->setFrameStyle(QFrame::Panel | QFrame::Raised);
    b->setLineWidth(1);
    b->setText(QObject::tr("More .."));
    b->setBackgroundRole(QPalette::Button);
    connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
    ll->addWidget(b);
  }

  {
    //  Edit pattern
    LCPActiveLabel *b;
    b = new LCPActiveLabel(-1, this, "dp_l2");
    b->setFrameStyle(QFrame::Panel | QFrame::Raised);
    b->setLineWidth(1);
    b->setText(QObject::tr("Custom Pattern .."));
    b->setBackgroundRole(QPalette::Button);
    connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
    l->addWidget(b);
  }

}
//为Stipple里面填充pixmap
void LCPDitherPalette::create_pixmap_for(LCPActiveLabel *b, int n)
{
  //const DitherPattern &pattern = !mp_view ? lay::DitherPattern::default_pattern() : mp_view->dither_pattern();
  const DitherPattern &pattern=DitherPattern::default_pattern() ;
  color_t color0 = b->palette().color(QPalette::Normal, b->backgroundRole()).rgb();
  color_t color1 = b->palette().color(QPalette::Normal, b->foregroundRole()).rgb();

  const unsigned int h = 24;
  const unsigned int w = 24;

  QImage image(w, h, QImage::Format_RGB32);
  const uint32_t *dp = pattern.pattern(n);
  for (unsigned int l = 0; l < h; ++l, ++dp) {
    uint32_t m = *dp;
    if (l == 0 || l == h - 1) {
      m |= ((1 << w) - 1);
    } else {
      m |= ((1 << (w - 1)) | 1);
    }
    color_t *pt = (color_t *) image.scanLine(h - 1 - l);
    for (unsigned int b = 0; b < w; ++b) {
      *pt++ = (m & 1) ? color1 : color0;
      m >>= 1;
    }
  }

  QPixmap pixmap = QPixmap::fromImage(image); // Qt 4.6.0 workaround
  b->setPixmap(pixmap);
}

void LCPDitherPalette::set_palette(const StipplePalette &palette)
{
  if (palette != m_palette) {

    m_palette = palette;

    for (unsigned int i = 0; i < m_stipple_buttons.size (); ++i) {
      unsigned int n = i;
      if (i < m_palette.stipples()) {
        n = m_palette.stipple_by_index(i);
      }
      if (m_stipple_buttons [i]) {
        create_pixmap_for(m_stipple_buttons[i], n);
      }
    }

  }
}

void
LCPDitherPalette::button_clicked(int /*index*/)
{
//  if (! mp_view) {
//    return;
//  }

//  if (index == -1) {

//    //  edit pattern
//    lay::DitherPattern pattern(mp_view->dither_pattern());
//    lay::EditStipplesForm stipples_form(mp_view, pattern);
//    if (stipples_form.exec() && stipples_form.pattern() != pattern) {
//      emit pattern_changed(stipples_form.pattern());
//    }

//  } else if (index == -2) {

//    //  edit pattern
//    lay::SelectStippleForm stipples_form(0, mp_view->dither_pattern());
//    if (stipples_form.exec() && stipples_form.selected() >= 0) {
//      emit dither_selected(int(stipples_form.selected()));
//    }

//  } else if (index == -3) {

//    emit dither_selected(-1);

//  } else {
//    if (index < int(m_palette.stipples())) {
//      emit dither_selected(m_palette.stipple_by_index(index));
//    } else {
//      emit dither_selected(index);
//    }
//  }
}

//void
//LCPDitherPalette:: dither_selected(int /*dither_index*/)
//{

//}

//void
//LCPDitherPalette::pattern_changed(const DitherPattern &)
//{

//}


//  LCPVisibilityPalette implementation
LCPVisibilityPalette::LCPVisibilityPalette(QWidget *parent, const char *name)
  : QFrame(parent)
{
  setObjectName(QString::fromLatin1(name));

  QVBoxLayout *l = new QVBoxLayout(this);
  l->setMargin(0);
  l->setSpacing(0);

  QFrame *f;
  LCPActiveLabel *b;
  QHBoxLayout *ll;

  QSizePolicy sp(QSizePolicy::Ignored, QSizePolicy::Ignored);
  sp.setHorizontalStretch(0);
  sp.setVerticalStretch(0);

  f = new QFrame(this);
  f->setObjectName(QString::fromLatin1("vis_f"));
  l->addWidget(f);
  ll = new QHBoxLayout(f);
  ll->setMargin(0);
  ll->setSpacing(0);

  b = new LCPActiveLabel(0, f);
  b->setMinimumSize(50, 16);
  b->setAlignment(Qt::AlignCenter);
  b->setLineWidth(1);
  b->setSizePolicy(sp);
  b->setText(QObject::tr("Show"));
  connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
  ll->addWidget(b);

  b = new LCPActiveLabel(1, f);
  b->setMinimumSize(50, 16);
  b->setAlignment(Qt::AlignCenter);
  b->setLineWidth(1);
  b->setSizePolicy(sp);
  b->setText(QObject::tr("Hide"));
  connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
  ll->addWidget(b);

  f = new QFrame(this);
  l->addWidget(f);
  ll = new QHBoxLayout(f);
  ll->setMargin(0);
  ll->setSpacing(0);

  b = new LCPActiveLabel(2, f);
  b->setMinimumSize(50, 16);
  b->setAlignment(Qt::AlignCenter);
  b->setLineWidth(1);
  b->setSizePolicy(sp);
  b->setText(QObject::tr("Transp."));
  connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
  ll->addWidget(b);

  b = new LCPActiveLabel(3, f);
  b->setMinimumSize(50, 16);
  b->setAlignment(Qt::AlignCenter);
  b->setLineWidth(1);
  b->setSizePolicy(sp);
  b->setText(QObject::tr("Opaque"));
  connect(b, SIGNAL(clicked(int)), this, SLOT(button_clicked(int)));
  ll->addWidget(b);
}

void LCPVisibilityPalette::button_clicked(int index)
{
  if (index == 0) {
    emit visibility_change(true);
  } else if (index == 1) {
    emit visibility_change(false);
  } else if (index == 2) {
    emit transparency_change(true);
  } else if (index == 3) {
    emit transparency_change(false);
  }
}

//构层配置工具箱
LayerToolbox::LayerToolbox(QWidget *parent, const char *name)
  : QWidget(parent)
{
  setObjectName(QString::fromLatin1(name));

  //CHECK!! 可否顺序从上到下？目前必须是反序添加配置栏。

  // 7. Brush and Pen
  BrushWidget *brushWidget = new BrushWidget(this);
  add_panel (brushWidget, LayerToolbox::to_string (QObject::tr ("Brush")).c_str ());
  PenWidget   *penWidget   = new PenWidget(this);
  add_panel (penWidget, LayerToolbox::to_string (QObject::tr ("Pen")).c_str ());

  // 6. 能见度(Visibility)配置
 // setObjectName (QString::fromLatin1 (name));
  LCPVisibilityPalette *vp = new LCPVisibilityPalette (this, "vis");
  add_panel (vp, LayerToolbox::to_string (QObject::tr ("Visibility")).c_str ());
  connect (vp, SIGNAL(visibility_change (bool)), this, SLOT (visibility_changed (bool)));

  connect (vp, SIGNAL(transparency_change (bool)), this, SLOT (transparency_changed (bool)));
  // 5. 样式(Style)配置(线条样式： 粗细, 虚实)
  LCPStylePalette *sp = new LCPStylePalette (this, "styles");
  add_panel (sp, LayerToolbox::to_string (QObject::tr ("Style")).c_str ());
  connect (sp, SIGNAL(width_selected (int)), this, SLOT (width_changed (int)));
  connect (sp, SIGNAL(marked_selected (bool)), this, SLOT (marked_changed (bool)));

  // 4. 动画(Animation)配置
  LCPAnimationPalette *ap = new LCPAnimationPalette (this, "anim");
  add_panel (ap, LayerToolbox::to_string(QObject::tr ("Animation")).c_str ());
  connect (ap, SIGNAL(animation_selected (int)), this, SLOT (animation_changed (int)));
  LCPDitherPalette *dp = new LCPDitherPalette (this, "dither");
  mp_dither_palette = dp;

  // 3. 格纹(Stipple, Pattern)配置 (KLayout ： Stipple)
  add_panel (dp, LayerToolbox::to_string (QObject::tr ("Stipple")).c_str ());
  connect (dp, SIGNAL(dither_selected (int)), this, SLOT (dither_changed (int)));
  //connect (dp, SIGNAL(pattern_changed (const lay::DitherPattern &)), this, SLOT (dither_pattern_changed (const lay::DitherPattern &)));

  // 2. 框线颜色(Frame Color)配置
  LCPColorPalette *pf = new LCPColorPalette(this, "Colors_Frame");
  mp_frame_palette = pf;
  add_panel(pf, LayerToolbox::to_string(QObject::tr("Frame Color")).c_str());
  connect(pf, SIGNAL(color_selected(QColor)), this, SLOT(frame_color_changed(QColor)));
  connect(pf, SIGNAL(color_brightness_selected(int)), this, SLOT(frame_color_brightness(int)));
  connect(pf, SIGNAL(frameColor(QColor &)), this, SLOT(slotframeColorInf(QColor &)));


  // 1. 填充颜色(Fill Color)配置
  LCPColorPalette *p = new LCPColorPalette(this, "Colors_Fill");
  mp_palette = p;
  add_panel(p, LayerToolbox::to_string(QObject::tr("Fill Color")).c_str());
  connect(p, SIGNAL(color_selected(QColor)), this, SLOT(fill_color_changed(QColor)));
  connect(p, SIGNAL(color_brightness_selected(int)), this, SLOT(fill_color_brightness(int)));
  connect(p, SIGNAL(frameColor(QColor &)), this, SLOT(slotfillColorInf(QColor &)));


  // make the height equal to the computed height
  int h = sizeHint().height();
  setMinimumHeight(h);
  setMaximumHeight(h);
}

LayerToolbox::~LayerToolbox()
{

}

void LayerToolbox::add_panel(QWidget *panel_widget, const char *text)
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
    b->setText(LayerToolbox::to_qstring(text));
    b->setMaximumSize(QSize(b->maximumSize().width(), b->sizeHint().height() - 4));

    LCPRemitter *e = new LCPRemitter(int(m_tool_panels.size()), this);
    connect(b, SIGNAL(clicked()), e, SLOT(the_slot()));
    connect(e, SIGNAL(the_signal(int)), this, SLOT(panel_button_clicked(int)));

    m_tool_panels.push_back(std::make_pair(f, panel_widget));
}

void LayerToolbox::slotframeColorInf(QColor &color)
{
    emit  frameColorInf(color);
}

void LayerToolbox::slotfillColorInf(QColor &fillcolor)
{
    emit  fillColorInf(fillcolor);
}

QSize LayerToolbox::sizeHint() const
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

void LayerToolbox::resizeEvent(QResizeEvent *re)
{
    rearrange(re->size().width(), re->size().height());
}

void LayerToolbox::resize(int w, int h)
{
    QWidget::resize(w, h);
    rearrange(w, h);
}

void LayerToolbox::setGeometry(int x, int y, int w, int h)
{
    QWidget::setGeometry(x, y, w, h);
    rearrange(w, h);
}

void LayerToolbox::rearrange(int w, int h)
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

void LayerToolbox::panel_button_clicked(int index)
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

void LayerToolbox::fill_color_changed(QColor /*c*/)
{

}

void LayerToolbox::frame_color_changed(QColor /*c*/)
{

}

void LayerToolbox::fill_color_brightness(int /*delta*/)
{

}

void LayerToolbox::frame_color_brightness(int /*delta*/)
{

}

void LayerToolbox::dither_changed(int /*di*/)
{

}

void LayerToolbox::dither_pattern_changed(const DitherPattern &/*pattern*/)
{

}

void LayerToolbox::visibility_changed(bool /*visible*/)
{

}

void LayerToolbox::transparency_changed(bool /*transparent*/)
{

}

void LayerToolbox::width_changed(int /*width*/)
{

}

void LayerToolbox::marked_changed(bool /*marked*/)
{

}

void LayerToolbox::animation_changed(int /*mode*/)
{

}

void LayerToolbox::set_palette(const ColorPalette &p)
{
    mp_palette->set_palette(p);
    mp_frame_palette->set_palette(p);
}
