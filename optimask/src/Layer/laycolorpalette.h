/* 构层色板配置 */

#ifndef LAYCOLORPALETTE
#define LAYCOLORPALETTE

#include <vector>
#include <string>

typedef unsigned int color_t;

/* Class Constructor for ColorPalette */
class ColorPalette
{
  public:
    ColorPalette();

    //ColorPalette运算符号集
    ColorPalette operator = (const ColorPalette &d);                 // =  赋值
    bool operator == (const ColorPalette &d) const;                  // == 相等
    bool operator != (const ColorPalette &d) const                   // != 不等
    {
        return ! operator == (d);
    }

    //设定或清除 颜色或索引
    void set_color(unsigned int n, color_t c);                       //设定颜色
    void clear_colors();                                             //清除颜色
    void set_luminous_color_index(unsigned int n, unsigned int ci);  //设定亮色索引
    void clear_luminous_colors();                                    //清除亮色

    color_t color_by_index(unsigned int n) const;
    unsigned int colors() const;
    color_t luminous_color_by_index(unsigned int n) const
    {
        return color_by_index(luminous_color_index_by_index(n));
    }
    unsigned int luminous_color_index_by_index(unsigned int n) const;
    unsigned int luminous_colors() const;

    //std::string to_string() const;//这个函数暂时还用不到
    //void from_string(const std::string &s, bool simple = false);
    void from_string(const char *&s, bool simple = false);

    static ColorPalette default_palette();                           //缺省调色面板

  private:
    std::vector <color_t> m_colors;                                  //颜色向量
    std::vector <unsigned int> m_luminous_color_indices;             //亮色索引向量

};

#endif // LAYCOLORPALETTE
