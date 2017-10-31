/* ========================================================================== */
/* LAYERTOOLBOX.H -- 构层配置(Layer Palette)(色板,格纹,动画,样式,能见度)构建头文件
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
/* HISTORY: 20161027(v1.00) Initial Version; 20171001 Optimization;
 * ========================================================================== */

#ifndef LAYERTOOLBOX_H
#define LAYERTOOLBOX_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QString>

#include "./Layer/laycolorpalette.h"   //构层色板(Layer Palette)
#include "./Layer/layStipplePalette.h" //构层格纹(Stipple, Pattern)
#include "./Layer/layDitherPattern.h"
#include "./Layer/layDitherPattern.h"

#include <stdint.h>
class LCPRemitter;
class LCPColorPalette;
class LCPActiveLabel;
class ColorPalette;
class StipplePalette;
class DitherPattern;

class LCPRemitter : public QObject
{
  Q_OBJECT
  
  public:
    LCPRemitter (int index, QObject *parent, const char *name = 0);
  
  public slots:
    void the_slot();
  
  signals:
    void the_signal(int index);
  
  private:
    int m_index;
};

class LCPColorPalette : public QFrame
{
  Q_OBJECT
  
  public:
    LCPColorPalette(QWidget *parent, const char *name = 0);
    void set_palette(const ColorPalette &p);
  
  public slots:
    void button_clicked(int index);

     void slotColor(int);
  
  signals:
    void color_selected(QColor);
    void color_brightness_selected(int);

    void frameColor(QColor &);
  
  private:
    ColorPalette m_palette;
    std::vector <LCPActiveLabel *> m_color_buttons;
};

class LCPActiveLabel : public QLabel
{
  Q_OBJECT
  
  public:
    LCPActiveLabel(int index, QWidget *parent, const char *name = "button");
  
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
  
  signals:
    void clicked(int);
  
  private:
    int m_index;
    bool m_grabbed;
};

class LCPAnimationPalette : public QFrame
{
  Q_OBJECT

  public:
    LCPAnimationPalette(QWidget *parent, const char *name = 0);

  public slots:
    void button_clicked(int index);

  signals:
    void animation_selected(int mode);
};

class LCPStylePalette : public QFrame
{
  Q_OBJECT

  public:
    LCPStylePalette(QWidget *parent, const char *name = 0);

  public slots:
    void button_clicked(int index);

  signals:
    void width_selected(int width);
    void marked_selected(bool marked);
};

class LCPDitherPalette : public QFrame
{
  Q_OBJECT

  public:
    LCPDitherPalette(QWidget *parent, const char *name = 0);

    void set_palette(const StipplePalette &p);

//  void set_view (LayoutView *view)
//  {
//    mp_view = view;
//  }

  public slots:
    void button_clicked (int index);

  signals:
    void dither_selected(int dither_index);
    void pattern_changed(const DitherPattern &);
  
  private:
    StipplePalette m_palette;
   // lay::LayoutView *mp_view;
    std::vector <LCPActiveLabel *> m_stipple_buttons;
  
    void create_pixmap_for(LCPActiveLabel *b, int n);
};

class LayerToolbox : public QWidget
{
    Q_OBJECT
  public:
    LayerToolbox(QWidget *parent, const char *name);
    ~LayerToolbox();

    QString to_qstring(const std::string &s)
    {
      return QString::fromUtf8(s.c_str());
    }

    std::string to_string(const QString &s)
    {
      return std::string(s.toUtf8().constData());
    }

    //void set_view (LayoutView *view);
    void set_palette(const ColorPalette &p);
    //void set_palette (const lay::StipplePalette &p);
    virtual QSize sizeHint() const;
    virtual void resizeEvent(QResizeEvent *re);
    virtual void resize(int w, int h);
    virtual void setGeometry(int x, int y, int w, int h);

  protected slots:
    void panel_button_clicked(int index);
    void fill_color_changed (QColor c);
    void frame_color_changed (QColor c);
    void fill_color_brightness (int delta);
    void frame_color_brightness (int delta);
    void dither_changed (int di);
    void dither_pattern_changed (const DitherPattern &pattern);
    void visibility_changed (bool visible);
    void transparency_changed (bool transparent);
    void width_changed (int width);
    void marked_changed (bool marked);
    void animation_changed (int mode);

  private:
    //LayoutView *mp_view;
    std::vector <std::pair <QWidget *, QWidget *> > m_tool_panels;
    LCPDitherPalette *mp_dither_palette;
    LCPColorPalette *mp_palette;
    LCPColorPalette *mp_frame_palette;

    //template <class Op>
    //void foreach_selected (const Op &op);
    void rearrange(int w, int h);
    void add_panel(QWidget *panel_widget, const char *text);

private slots:
    void  slotframeColorInf(QColor &);
    void  slotfillColorInf(QColor &);
signals:
  void  frameColorInf(QColor &);
  void  fillColorInf(QColor &);

};

class LCPVisibilityPalette : public QFrame
{
  Q_OBJECT
  
  public:
    LCPVisibilityPalette (QWidget *parent, const char *name = 0);
  
  public slots:
    void button_clicked (int index);
  
  signals:
    void visibility_change (bool visible);
    void transparency_change (bool transparent);
};

#endif // LAYERTOOLBOX_H
