/* ========================================================================== */
/* GEOATTRIBUTE.H -- 基本图形定义头文件
*
* REFERENCE:
*
* COPYRIGHT 2017 Optixera.
* -------------------------------------------------------------------------- */
/* $REVISION: 1.00$ (20171001 Chaohong Liu) */
/* HISTORY: 20171001(v1.00) Initial Version;
*该版本用于定义一些图元的属性结构定义以及相关操作
* ========================================================================== */

#ifndef _GEOATTRIBUTE_H
#define _GEOATTRIBUTE_H

#include "Geo.h"

//属性编号
#define     GEO_ATTRIB_NONE         0      //未定义的任何类型
#define     GEO_ATTRIB_PEN          1      //笔
#define     GEO_ATTRIB_FONT         2	   //字体
#define     GEO_ATTRIB_BRUSH        3      //刷子
//#define     GEO_ATTRIB_FILL         4      //多边形填充模式

//根据RGB或者RGBA分量生成或者获取颜色数据
#define MK_RGB(r,g,b)           (((unsigned char)(r)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned long)(unsigned char)(b))<<16))
#define MK_RGBA(r,g,b,a)        (MK_RGB(r,g,b)|(((unsigned long)(unsigned char)(a))<<24))
#define RGB_R(rgb)              ((unsigned char)((rgb) & 0XFF))
#define RGB_G(rgb)              ((unsigned char)(((rgb)>>8) & 0XFF))
#define RGB_B(rgb)              ((unsigned char)(((rgb)>>16) & 0XFF))
#define RGBA_A(rgba)            ((unsigned char)(((rgba)>>24) & 0XFF))

//常用颜色定义
#define COLOR_BLACK             0
#define COLOR_WHITE             0XFFFFFF
#define COLOR_RED               0X0000FF
#define COLOR_GREEN             0X00FF00
#define COLOR_BLUE              0XFF0000
#define COLOR_YELLOW            0X00FFFF
#define COLOR_ORANGE            0X0080FF

//常用线型的属性
#define LINE_SOLID              0   //实线
#define LINE_DASH               1   //虚线
#define LINE_DOT                2   //点线
#define LINE_DOTDASH            3   //点虚线
#define LINE_DOTDOTDASH         4   //双点虚线

//常用刷子模式
#define BRUSH_SOLID             0   //实心
#define BRUSH_HORIZON           1   //水平线组成的阴影图案
#define BRUSH_VERTICAL          2   //垂直线组成的阴影图案
#define BRUSH_CROSS             3   //水平和垂直交叉组成的阴影图案
#define BRUSH_BDLAGONAL         4   //45度向上斜线组成的阴影图案(自左到右)
#define BRUSH_FDIAGONAL         5   //45度向下斜线组成的阴影图案(自左到右)
#define BRUSH_DIAGCROSS         6   //45度斜线交叉组成的阴影图案

//常用的字体风格
#define FONT_STYLE_NORMAL       0   //正常
#define FONT_STYLE_BOLD         1   //加粗
#define FONT_STYLE_ITALIC       2   //斜体
#define FONT_STYLE_UNDERLINE    4   //下划线
#define FONT_STYLE_STRIKEOUT    8   //删除线


//一下是各种属性定义表
//笔
class CProPen : public CProValue
{
private:
    unsigned short  m_wWidth;      //笔宽
    unsigned short  m_wStyle;      //风格
    unsigned long   m_dwColor;      //颜色，从低位到高位依次为red，green，blue，alpha
    
public:
    CProPen()
    { 
        m_dwID = GEO_ATTRIB_PEN; 
        m_wWidth = 1;
        m_dwColor = 0XFF000000;     //balck
        m_wStyle = LINE_SOLID;      //solid line
    }
    CProPen(unsigned short width, unsigned long color=0, unsigned short style=0)
    { 
        m_dwID = GEO_ATTRIB_PEN; 
        m_wWidth = width<1?1: width;
        m_dwColor = color;
        m_wStyle = style;
    }
    inline void SetWidth(unsigned short wVal) { m_wWidth = wVal<1 ? 1 : wVal; }
    inline unsigned short GetWidth()const { return m_wWidth; }

    inline void SetStyle(unsigned short wVal) { m_wStyle = wVal; }
    inline unsigned short GetStyle()const { return m_wStyle; }

    inline unsigned long GetColor() const { return m_dwColor; }
    inline void SetColor(unsigned long rgba) { m_dwColor = rgba; }
    inline void SetColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255){m_dwColor = MK_RGBA(red,green,blue,alpha);}
};

//画刷
class CProBrush : public CProValue
{
private:
    unsigned long   m_dwColor;          //颜色，从低位到高位依次为red，green，blue，alpha
    unsigned short  m_wPattern;         //填充模式
    unsigned short  m_wDataSize;        //自定义填充模式的填充数据大小,至少大于2字节，具体实现不是必要的
    unsigned char*  m_pFillData;        //填充数据，对于自定义填充模式的填充图形数据，只有在自定义模式下，该字段才有效，否则为0

public:
    CProBrush()
    {
        m_dwID = GEO_ATTRIB_BRUSH;
        m_wPattern = BRUSH_SOLID;
        m_dwColor= 0XFF000000;     //balck
        m_pFillData = 0;
        m_wDataSize = 0;
    }
    CProBrush(unsigned long dwColor, unsigned short wPattern, unsigned short wDataSize=0, unsigned char* pData=0)
    {
        m_dwID = GEO_ATTRIB_BRUSH;
        m_wPattern = wPattern;
        m_dwColor = dwColor;     
        if (pData != 0 && wDataSize > 2) 
        {
            m_pFillData = new unsigned char[wDataSize];
            memcpy(m_pFillData, pData, wDataSize);
        }
    }
    inline unsigned long GetColor() const { return m_dwColor; }
    inline void SetColor(unsigned long rgba) { m_dwColor = rgba; }

    inline unsigned short GetPattern() const { return m_wPattern; }
    inline void SetPattern(unsigned short pattern) { m_wPattern = pattern; }

    //参数pData表示要接受数据的缓冲区，如果为0，则表示希望获取缓冲区大小
    //返回值为实际数据大小，如果返回值为-1表示参数错误
    int GetFillData(unsigned char* pData, unsigned short nBuffSize) const
    { 
        if (0 == pData)
            return m_wDataSize;
        if (nBuffSize < m_wDataSize)
            return -1;
        memcpy(pData, m_pFillData, m_wDataSize);
        return m_wDataSize;
    }
    //参数pData表示要接受数据的缓冲区，如果为0，则表示希望获取缓冲区大小
    //返回值为实际数据大小，如果返回值为-1表示参数错误
    int SetFillData(unsigned char* pData, unsigned short wDataSize)
    {
        if (0 == pData || wDataSize <= 2)
            return -1;
        if (m_wDataSize != wDataSize)
        {
            delete[] m_pFillData;
            m_pFillData = new unsigned char[wDataSize];            
        }
        memcpy(m_pFillData, pData, wDataSize);
        return wDataSize;
    }
};

//字体
//定义时参考了window GDI和QFont
class CProFont : public CProValue
{
private:
    std::string     m_strFamily;    //字体名
    int             m_nHeight;      //字体高度，也表示字体大小
    int             m_mWeight;      //字体笔划的粗细
    unsigned long   m_dwStyle;      //字体风格，每一位表示一个风格，如加粗，斜体等

public:
    CProFont()
    {
        m_nHeight = 12;
        m_mWeight = 50;
        m_dwStyle = FONT_STYLE_NORMAL;
        m_dwID = GEO_ATTRIB_FONT;
    }
    CProFont(const std::string& strName,int nSize=12)
    {
        m_strFamily = strName;
        m_nHeight = nSize;
        m_mWeight = 50;
        m_dwStyle = FONT_STYLE_NORMAL;
        m_dwID = GEO_ATTRIB_FONT;
    }
    //字体名
    inline void SetFamily(const std::string& strName){ m_strFamily = strName; }
    inline const std::string& GetFamily() const { return m_strFamily; }
    //大小
    inline void SetSize(int nSize){m_nHeight = nSize <= 0 ? 12 : nSize;}
    inline int GetSize() const {return m_nHeight;}
    //粗细
    inline void SetWeight(int nVal) { m_mWeight = nVal <= 0 ? 12 : nVal; }
    inline int GetWeight() const { return m_mWeight; }
    //风格
    inline void SetStyle(unsigned long val) { m_dwStyle = val; }
    inline unsigned long GetStyle() const { return m_dwStyle; }

    inline bool IsNormal()const {return FONT_STYLE_NORMAL == m_dwStyle;}
    inline bool IsBold()const {return (m_dwStyle & FONT_STYLE_BOLD) != 0;}
    inline void SetBold(bool bSet=true) 
    { 
        if (bSet)
            m_dwStyle |= FONT_STYLE_BOLD;
        else
            m_dwStyle &= (~FONT_STYLE_BOLD);
    }
    inline bool IsItalic()const { return (m_dwStyle & FONT_STYLE_ITALIC) != 0; }
    inline void SetItalic(bool bSet = true)
    {
        if (bSet)
            m_dwStyle |= FONT_STYLE_ITALIC;
        else
            m_dwStyle &= (~FONT_STYLE_ITALIC);
    }
    inline bool IsUnderline()const { return (m_dwStyle & FONT_STYLE_UNDERLINE) != 0; }
    inline void SetUnderline(bool bSet = true)
    {
        if (bSet)
            m_dwStyle |= FONT_STYLE_UNDERLINE;
        else
            m_dwStyle &= (~FONT_STYLE_UNDERLINE);
    }
    inline bool IsStrikeout()const { return (m_dwStyle & FONT_STYLE_STRIKEOUT) != 0; }
    inline void SetStrikeout(bool bSet = true)
    {
        if (bSet)
            m_dwStyle |= FONT_STYLE_STRIKEOUT;
        else
            m_dwStyle &= (~FONT_STYLE_STRIKEOUT);
    }     
};

#endif