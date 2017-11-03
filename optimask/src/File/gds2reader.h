#ifndef GDS2READER_H
#define GDS2READER_H

#include <QObject>
#include <QList>
#include <math.h>
#include <algorithm>
#include <complex>

using namespace std;
//const double PI = std::acos(-1);

/**************************************************************************************/
enum record_type            //Record Types 记录类型
{
    GDS_HEADER = 0,
    GDS_BGNLIB,
    GDS_LIBNAME,
    GDS_UNITS,
    GDS_ENDLIB,
    GDS_BGNSTR,
    GDS_STRNAME,
    GDS_ENDSTR,
    GDS_BOUNDARY,
    GDS_PATH,
    GDS_SREF,
    GDS_AREF,
    GDS_TEXT,
    GDS_LAYER,
    GDS_DATATYPE,
    GDS_WIDTH,
    GDS_XY,
    GDS_ENDEL,
    GDS_SNAME,
    GDS_COLROW,
    GDS_TEXTNODE,
    GDS_NODE,
    GDS_TEXTTYPE,
    GDS_PRESENTATION,
    GDS_SPACING,
    GDS_STRING,
    GDS_STRANS,
    GDS_MAG,
    GDS_ANGLE,
    GDS_UINTEGER,
    GDS_USTRING,
    GDS_REFLIBS,
    GDS_FONTS,
    GDS_PATHTYPE,
    GDS_GENERATIONS,
    GDS_ATTRTABLE,
    GDS_STYPTABLE,
    GDS_STRTYPE,
    GDS_ELFLAGS,
    GDS_ELKEY,
    GDS_LINKTYPE,
    GDS_LINKKEYS,
    GDS_NODETYPE,
    GDS_PROPATTR,
    GDS_PROPVALUE,
    GDS_BOX,
    GDS_BOXTYPE,
    GDS_PLEX,
    GDS_BGNEXTN,
    GDS_ENDEXTN,
    GDS_TAPENUM,
    GDS_TAPECODE,
    GDS_STRCLASS,
    GDS_RESERVED,
    GDS_FORMAT,
    GDS_MASK,
    GDS_ENDMASKS,
    GDS_LIBDIRSIZE,
    GDS_SRFNAME,
    GDS_LIBSECUR,
    GDS_BORDER,
    GDS_SOFTFENCE,
    GDS_HARDFENCE,
    GDS_SOFTWIRE,
    GDS_HARDWIRE,
    GDS_PATHPORT,
    GDS_NODEPORT,
    GDS_USERCONSTRAINT,
    GDS_SPACER_ERROR,
    GDS_CONTACT
};
enum data_type               //数据类型
{
    GDS_NoData = 0,
    GDS_BitArray,           //1
    GDS_Integer2,           //2
    GDS_Integer4,           //3
    GDS_Real4,              // not used
    GDS_Real8,              //5
    GDS_Ascii               //6
};
enum strans                 //STRANS record主要应用在AREF,SREF,text这三种element中
{
    GDS_STRANS_REFRECT = 0x8000,//reflection 坐标需要进行x轴对称变换
    GDS_STRANS_ABS_MAG = 0x0004,//absolute magnification
    GDS_STRANS_ABS_ANG = 0x0002,//absolute angle
};
enum element                    //在ELFLAGS record中，不过这里好像没什么用
{
    GDS_ELEMENT_TEMPLATE = 0x01,//GDSII Reference Manual
    GDS_ELEMENT_EXTERNAL = 0x02,//CustomPlus User's Manual
};

const double PAI                = 3.1415926535897932384626433832795;
const int    GDS_MAX_RECORD_LEN = (65536 + 1); // 16 bit
const int    GDS_MAX_STR_LEN    = (65536 + 1); // 16 bit

const int    GDS_MAX_LAYER      =  1024;       //512 //256
const int    GDS_MAX_COL        =  32767;
const int    GDS_MAX_ROW		=  32767;
const int    GDS_MAX_DATA_TYPE	=  255;

const int    GDS_MAX_COORD      =  8192;
// Record length = 16bit = points up to 65536byte
// XY coord  = pair of 4byte integer= 8byte
// 65536/8 = 8192
/**************************************************************************************/

template<class T>     // 坐标的模板类，已经添加了旋转函数
class CCoord
{
public :
    T x;
    T y;

public:
    CCoord(T x, T y):x(x),y(y) {}

    CCoord():x(0),y(0){}
    CCoord(const CCoord &other):x(other.x),y(other.y){}

    void Offset(T x, T y)
    {
        this->x += x;
        this->y += y;
    }

    void Offset(T* x, T* y)
    {
        this->x += *x;
        this->y += *y;
    }

    void Offset(const CCoord &add)
    {
        x += add.x;
        y += add.y;
    }

    void OffsetNeg(const CCoord &sub)
    {
        x -= sub.x;
        y -= sub.y;
    }

    void NegateY()
    {
        y = -y;
    }

    void NegateX()
    {
        x = -x;
    }

    void Negate()
    {
        x = -x;
        y = -y;
    }

    void Mul(const CCoord &add)
    {
        x *= add.x;
        y *= add.y;
    }

    void Mul(const T &ratio)
    {
        x *= ratio;
        y *= ratio;
    }

    void SetCoord(T x, T y)
    {
        this->x = x;
        this->y = y;
    }

    void SetCoord(T* x, T* y)
    {
        this->x = *x;
        this->y = *y;
    }

    void SetCoord(CCoord& arg)
    {
        this->x = arg.x;
        this->y = arg.y;
    }

    bool operator==(CCoord& arg)
    {
        if(x == arg.x && y == arg.y)
            return true;
        return false;
    }

    bool Equals(CCoord& arg)
    {
        if(x == arg.x && y == arg.y)
            return true;
        return false;
    }

    bool Equals(CCoord* arg)
    {
        if(x == arg->x && y == arg->y)
            return true;
        return false;
    }

    bool operator!=(CCoord& arg)
    {
        if(x != arg.x || y != arg.y)
            return true;
        return false;
    }

    void Copy(CCoord* other)
    {
        x = other->x;
        y = other->y;
    }

    CCoord& operator=(CCoord& arg)
    {
        if(this==&arg)          // 防止自己赋值给自己
            return *this;
        x = arg.x;
        y = arg.y;
        return *this;
    }

    CCoord& operator+=(CCoord& arg)
    {
        x += arg.x;
        y += arg.y;
        return *this;
    }

    CCoord& operator-=(CCoord& arg)
    {
        x -= arg.x;
        y -= arg.y;
        return *this;
    }

    CCoord& operator*=(T ratio)
    {
        x *= ratio;
        y *= ratio;
        return *this;
    }

    CCoord& operator/=(T ratio)
    {
        x /= ratio;
        y /= ratio;
        return *this;
    }

    CCoord operator* (T ratio)
    {
        return CCoord(x * ratio, y * ratio);
    }

    // 这是新加的旋转坐标，但是现在还没有用这个函数，以前的坐标旋转都在其他的函数里完成，
    // 以后会慢慢的更新用这个函数来旋转坐标
    CCoord& RotatePoint(double angle)
    {
        if(angle == 0.0 || (x == 0.0 && y == 0.0))
            return *this;

        if(angle == 90.0 || angle == -270.0){
            double t = x;
            x = -y;
            y = t;
        }
        else if(angle == 180.0 || angle == -180.0){
            x = -x;
            y = -y;
        }
        else if(angle == 270.0 || angle == -90.0){
            double t = x;
            x = y;
            y = -t;
        }
        else{ // 旋转后的点与理论是有误差的，因为PAI不精确
            std::complex<double> P0(x,y);             // 需要旋转的坐标点，用复数表示
            std::complex<double> P1(0,angle*PAI/180); // 旋转的角度，用复数表示
            P1=P0*exp(P1);                       // 旋转的变换公式
            x = P1.real();                       // 旋转后坐标的x值
            y = P1.imag();                       // 旋转后坐标的y值
        }
        return *this;
    }
    template<class pointType>
    void CCoordToQPointF(pointType *qpoint,bool reflect = false) // 将CCoord坐标转换成Qt中的QPoint或Qpoint
    {
        qpoint->setX(x);
        if(reflect){
            qpoint->setY(-y);
        }
        qpoint->setY(y);
    }
};

class CGDSProperty       // GDS属性。Property一般译为属性，而attribute译为特性
{
protected :
    unsigned   m_uintPropAttr;
    char      *m_strPropValue;
public :
    CGDSProperty():m_uintPropAttr(0),m_strPropValue(NULL){}

    ~CGDSProperty(){                     // 因为有指针，所以最后要释放内存
        if(m_strPropValue)
            delete [] m_strPropValue;
    }
    void SetPropAttr(int var){m_uintPropAttr = var;}
    void SetPropValue(char* str){
        if(m_strPropValue)                // 先回收内存空间，以避免内存泄露
            delete [] m_strPropValue;
        int len = strlen(str) + 1;        // 包含一个空格
        m_strPropValue = new char[len];
        strncpy(m_strPropValue, str, len);
    }
    unsigned GetPropAttr(){return m_uintPropAttr;}
    char* GetPropValue(){return m_strPropValue;}

};

class CGDSXY
{   public:
    CCoord<int>*	m_coordList;         // 坐标链表
    unsigned short	m_intNumCoords;      // 存储坐标的数目
protected :

    unsigned long	m_ulSize;            // 边框的长和宽的大者
    CCoord<int>		m_coordPos;          // 边框的中心坐标

public :
    CGDSXY()
    {
        m_coordList = NULL;
        m_intNumCoords = 0;
        m_ulSize = 0;
    }
    virtual ~CGDSXY()
    {
        if(m_coordList)
            delete [] m_coordList;
    }
    virtual void LoadCoords(CCoord<int>* list, int num) // 读取.gds文件时，将各element的坐标记录到链表中
    {
        if(num <= 0)
            return;
        m_intNumCoords = num;
        if(m_coordList)                       // 避免内存泄漏
            delete [] m_coordList;

        if(num == 1){
            m_coordPos = *(list + 0);
            return;
        }

        m_coordList = new CCoord<int>[num];   // 动态分配可以储存num个int型的坐标
        long minx, maxx, miny, maxy;
        for(unsigned short i = 0;i < num;i++){// 找出m_coordList中的x,y的最大最小值，也就是这些点组成图形的边框
            *(m_coordList + i) = *(list + i);
            int x = (m_coordList + i)->x;
            int y = (m_coordList + i)->y;
            if(i == 0){
                minx = maxx = x;
                miny = maxy = y;
            }
            else{
                if(x < minx)
                    minx = x;
                else if(x > maxx)
                    maxx = x;
                if(y < miny)
                    miny = y;
                else if(y > maxy)
                    maxy = y;
            }
        }
        m_ulSize = max(maxx - minx, maxy - miny);
        m_coordPos.x = (maxx + minx)/2L;
        m_coordPos.y = (maxy + miny)/2L;
    }
    unsigned long GetSize()
    {
        return m_ulSize;
    }
    void GetPos(CCoord<double> * pCoord)            // 边框的中心坐标
    {
        pCoord->x = m_coordPos.x;
        pCoord->y = m_coordPos.y;
    }
    CCoord<int> GetPos(){                           // 边框的中心坐标
        return m_coordPos;
    }
    void GetNthCoord(int n, CCoord<double>* pcoord) // 获取第n+1个的点的坐标
    {
        if(m_coordList){
            pcoord->x = (m_coordList + n)->x;
            pcoord->y = (m_coordList + n)->y;
        }
        else{
            pcoord->x = m_coordPos.x;
            pcoord->y = m_coordPos.y;
        }
    }
    unsigned short GetNumCoords()                   // element中图形的点的总数
    {
        return m_intNumCoords;
    }
    CCoord<int>* GetCoordArray()                    // 获取坐标链表
    {
        return m_coordList;
    }
};

class CGDSStructure;     // 声明
class CGDSElement : public QObject, public CGDSXY  //作为一个基类
{
//protected :
public:
    unsigned int	m_uintLayer;
    unsigned char	m_uintElementType;
private :
    short           m_shortElflags;    // 存储ELFLAGS的数据
    int             m_intPlex;         // 存储PLEX的数据
    bool            ELFLAGS_flag;      // 标记是否有ELFLAGS这个记录
    bool            PLEX_flag;         // 标记是否有PLEX这个记录    
    bool            StransFlag;        // 标记是否有STRANS这个记录
    bool            MagFlag;           // 标记是否有MAG这个记录
    bool            AngleFlag;         // 标记是否有ANGLE这个记录
    bool            PathTypeFlag;      // 标记是否有ANGLE这个记录
    bool            WidthFlag;         // 标记是否有ANGLE这个记录
    bool            BgnextnFlag;       // 标记是否有ANGLE这个记录
    bool            EdextnFlag;        // 标记是否有ANGLE这个记录
    bool            PresentationFlag;  // 标记是否有PRESENTATION这个记录

public :
    CGDSElement();
    ~CGDSElement();

    virtual void SetDataType(int ){}         // 设置数据类型
    virtual void SetWidth(int ){}
    virtual void SetSname(std::string ){}
    virtual void SetColRow(int, int){}       // 设置行列
    virtual void SetTextType(int){}          // 设置文本类型
    virtual void SetPresentation(int){}
    virtual void SetString(std::string ){}
    virtual void SetStrans(int ){}
    virtual void SetMag(double ){}
    virtual void SetAngle(double ){}
    virtual void SetPathType(int ){}
    virtual void SetNodeType(int ){}
    virtual void SetBoxType(int ){}
    virtual void SetBgnExtn(int ){}
    virtual void SetEndExtn(int ){}
    virtual string GetSname(){return "";}                 // 获取被引用构元的名字
    //virtual char* GetSname(){return NULL;}
    virtual void SetRefStructure(CGDSStructure* ){}        // 设置为被引用的构元。如名字为box的构元被设置为被引用的构元
    virtual CGDSStructure* GetRefStructure(){return NULL;} // 获取被引用的构元
    void SetLayer(unsigned int layer);                     // 设置层


    void SetElflags(int elflags);
    void SetELFLAGS_flag (bool flag){ ELFLAGS_flag = flag; }
    bool GetELFLAGS_flag ()         { return ELFLAGS_flag; }
    short GetElflags ()             { return m_shortElflags;}

    void SetPlex (int plex);
    void SetPLEX_flag (bool flag)   {ELFLAGS_flag = flag;}
    bool GetPLEX_flag ()            {return PLEX_flag;}
    int  GetPlex ()                 {return m_intPlex;}

    void SetStransFlag  (bool flag)  { StransFlag= flag;}
    void SetMagFlag     (bool flag)  { MagFlag= flag;}
    void SetAngleFlag   (bool flag)  { AngleFlag= flag;}
    void SetPathTypeFlag(bool flag)  { PathTypeFlag = flag;}
    void SetWidthFlag   (bool flag)  { WidthFlag = flag;}
    void SetBgnextnFlag (bool flag)  { BgnextnFlag = flag;}
    void SetEdextnFlag  (bool flag)  { EdextnFlag = flag;}
    void SetPresentationFlag (bool flag) { PresentationFlag = flag;}

    bool GetStransFlag   () { return StransFlag;}
    bool GetMagFlag      () { return MagFlag;}
    bool GetAngleFlag    () { return AngleFlag;}
    bool GetPathTypeFlag () { return PathTypeFlag;}
    bool GetWidthFlag    () { return WidthFlag;}
    bool GetBgnextnFlag  () { return BgnextnFlag;}
    bool GetEdextnFlag   () { return EdextnFlag;}
    bool GetPresentationFlag  () { return PresentationFlag;}

    void SetElementType(int type){m_uintElementType = type;}
    unsigned int GetLayer(){return m_uintLayer;}
    int GetElementType(){return m_uintElementType;}

    virtual int GetDataType(){return 0;}
    virtual unsigned short GetStrans(){return 0;}
    virtual int GetWidth(){return 0;}
    virtual int GetPathType(){return 0;}
    virtual int GetBgnExtn(){return 0;}
    virtual int GetEndExtn(){return 0;}
    virtual double GetMag(){return 1.0;}
    virtual double GetAngle(){return 0.0;}
    //  void AddProperty(int var, string str)
    // {
    //        /*CGDSProperty* p = new CGDSProperty[m_intNumProperties + 1];
    //        if(m_pProperties){
    //            for(unsigned i = 0;i < m_intNumProperties;i++){
    //                (p + i)->SetPropAttr((m_pProperties + i)->GetPropAttr());
    //                (p + i)->SetPropValue((m_pProperties + i)->GetPropValue());
    //            }
    //            delete [] m_pProperties;
    //        }
    //        (p + m_intNumProperties)->SetPropAttr(var);
    //        (p + m_intNumProperties)->SetPropValue(str);
    //        m_pProperties = p;
    //        m_intNumProperties++; */
    // }
};

class CGDSStrans : public CGDSElement
{
protected :
    double m_dblMag;
    double m_dblAng;
    unsigned short m_intAttrib;  //其值主要由SetStrans()函数来设计
public :
    CGDSStrans():m_dblMag(1.0),m_dblAng(0.0),m_intAttrib(0){}

    void SetStrans(int bits){m_intAttrib = bits;}
    void SetAngle(double ang){m_dblAng = ang;}
    void SetMag(double mag){m_dblMag = mag;}
    virtual unsigned short GetStrans(){return m_intAttrib;}

    bool IsReflect()
    {
        return (m_intAttrib & GDS_STRANS_REFRECT) > 0 ? true : false;
    }
    bool IsAbsAng()
    {
        return (m_intAttrib & GDS_STRANS_ABS_ANG) > 0 ? true : false;
    }
    bool IsAbsMag()
    {
        return (m_intAttrib & GDS_STRANS_ABS_MAG) > 0 ? true : false;
    }
    double GetMag(){return m_dblMag;}
    void GetMag(double* pmag){*pmag = m_dblMag;}
    double GetAngle(){return m_dblAng;}
    void GetAngle(double* pang){*pang = m_dblAng;}
};


/****************************七种需要画的element介绍开始***********************************/
class CGDSBoundary : public CGDSElement
{
protected :
public:
    unsigned char m_uintDataType;

public :
    CGDSBoundary(){m_uintDataType = 0;}
    ~CGDSBoundary(){}
    void SetDataType(int type);
    virtual int GetDataType(){return m_uintDataType;}
};

class CGDSPath : public CGDSElement
{
protected :
    unsigned char m_uintDataType;
    unsigned char m_uintPathType;
    int		  m_intBgnExtn;
    int		  m_intEndExtn;
    int       m_intWidth;

public :
    CGDSPath()
    {
        m_uintDataType = 0;
        m_uintPathType = m_intBgnExtn = m_intEndExtn = 0;
        m_intWidth = 0;
    }
    ~CGDSPath(){}
    void SetDataType(int type);
    void SetWidth(int width);
    void SetPathType(int type);
    void SetBgnExtn(int len);
    void SetEndExtn(int len);
    virtual int GetDataType(){return m_uintDataType;}
    int  GetPathType(){return m_uintPathType & 0x0f;}
    int  GetBgnExtn(){return m_intBgnExtn;}
    int  GetEndExtn(){return m_intEndExtn;}
    int  GetWidth(){return m_intWidth;}
    void LoadCoords(CCoord<int>* list, int num);};
class CGDSSREF : public CGDSStrans
{
protected :
    char*           m_strSname;
    //string            m_strSname;
    CGDSStructure*  m_pStructure;

public :
    CGDSSREF()//:m_strSname(0)
    {
        m_strSname=NULL;
        m_pStructure = NULL;
    }
    ~CGDSSREF()
    {
        if(m_strSname)
            delete [] m_strSname;
    }
    void SetSname(string str)
    {
//        if(str.size()!=0)
//            str.clear();
//        m_strSname=str;
        if(m_strSname)
            delete []m_strSname;
        int len = str.size() + 1;
        m_strSname = new char[len];
        strncpy(m_strSname, str.c_str(), len);
    }
    string GetSname(){return m_strSname;}
    //char* GetSname(){return m_strSname;}
    void SetRefStructure(CGDSStructure* ref)
    {
        m_pStructure = ref;
//        if(m_strSname.size()!=0)
//            m_strSname.clear();
//        if(m_strSname){
//            delete [] m_strSname;
//            m_strSname = NULL;
//        }
    }
    CGDSStructure* GetRefStructure(){return m_pStructure;}
};


class CGDSAREF : public CGDSSREF
{
protected :
    unsigned   m_uintCol;
    unsigned   m_uintRow;
public :
    CGDSAREF():m_uintCol(0),m_uintRow(0){}

    ~CGDSAREF(){}
    void SetColRow(int col, int row)
    {
        if(col >= 0 && col <= GDS_MAX_COL && row >= 0 && row <= GDS_MAX_ROW){
            m_uintCol = col;
            m_uintRow = row;
            return;
        }
        return;
    }
    unsigned GetNumCol(){return m_uintCol;}
    unsigned GetNumRow(){return m_uintRow;}
};

class CGDSText : public CGDSStrans
{
protected :
    unsigned int    m_uintPresebtation;
    unsigned  m_uintFont;
    unsigned  m_uintVertJust;
    unsigned  m_uintHorzJust;

    unsigned char m_uintPathType;
    unsigned char m_uintTextType;
    int       m_intWidth;

    char*     m_strString;
public :
    CGDSText()
    {
        m_uintFont = m_uintVertJust = m_uintHorzJust = 0;
        m_uintPathType = m_intWidth = 0;
        m_uintTextType = 0;
        m_strString = NULL;
    }
    ~CGDSText()
    {
        if(m_strString)
            delete [] m_strString;
    }
    void SetWidth(int width);
    void SetTextType(int type)
    {
        if(type >= 0 && type <= GDS_MAX_DATA_TYPE){
            m_uintTextType = type;
            return;
        }
        return;
    }
    void SetPresentation(int bits);
    int GetPresentation(){return m_uintPresebtation;}
    void SetString(string str)
    {
        if(m_strString)
            delete [] m_strString;
        int len = str.size() + 1;
        m_strString = new char[len];
        strncpy(m_strString, str.c_str(), len);
    }
    void SetPathType(int type);
    char* GetString(){return m_strString;}
    int GetWidth(){return m_intWidth;}
    int GetFont(){return m_uintFont;}
    int GetVertJust(){return m_uintVertJust;}
    int GetHorzJust(){return m_uintHorzJust;}
    int GetPathType(){return m_uintPathType;}
    int GetTextType(){return m_uintTextType;}
};
class CGDSNode : public CGDSElement
{
protected :
    unsigned char m_uintNodeType;
public:
    CGDSNode(){m_uintNodeType = 0;}
    ~CGDSNode(){}
    void SetNodeType(int type);
};
class CGDSBox : public CGDSElement
{
protected :
    unsigned char m_uintBoxType;
public:
    CGDSBox(){m_uintBoxType = 0;}
    ~CGDSBox(){}
    void SetBoxType(int type);
};
/****************************七种需要画的element介绍结束***********************************/

//所有的元素都存储在m_obaryElements的链表中
class CGDSElements
{
protected :
public:
        QList<CGDSElement*> m_obaryElements;
public:
    CGDSElements(){}
    ~CGDSElements();
    void AddElement(int type);
    void DeleteElement(CGDSElement* p);
    //QList::iterator GetElementList(){return m_obaryElements.begin();}
    CGDSElement* GetLastElement()
    {
        QList<CGDSElement*>::iterator iter=m_obaryElements.end();
        if(iter==m_obaryElements.begin())
            return NULL;
        iter--;
        return (*iter);

    }
};


class CGDSStructures;
class CGDSRefStructure;
class CGDSStructure : public CGDSElements, public QObject
{
protected :
    CCoord<double>			m_coordDimMin;           // 存储structure的最小坐标。注：structure中所有element的minx,miny
    CCoord<double>			m_coordDimMax;           // 存储structure的最大坐标。注：structure中所有element的maxx,maxy
    char*			m_strStrName;                    // 结构体名STRNAME
    bool			m_bRefered;                      // 此构元是否是被引用
    QList<CGDSRefStructure*> m_obaryRefStructures;   // 被引用构元链表
    unsigned char	m_charLayerList[GDS_MAX_LAYER/8];// 最大是128层，为何不是63？

public :
    CGDSStructure()
    {
        m_bRefered = false;
        m_strStrName = NULL;
        for(int i = 0;i < GDS_MAX_LAYER/8;i++)
            *(m_charLayerList + i) = 0;
    }
    ~CGDSStructure();
    void SetVisibleOnLayer(int layer)
    {
        unsigned char val = (0x01 << (layer % 8));
        *(m_charLayerList + layer / 8) |= val;
    }
    bool IsVisibleOnLayer(int layer)
    {
        unsigned char val = (0x01 << (layer % 8));
        return (*(m_charLayerList + layer / 8) & val) > 0 ? true : false;
    }
    void MarkRefered(){m_bRefered = true;}          // 标记此构元被引用
    bool IsRefered(){return m_bRefered;}            // 判断此构元会不会被引用
    void SetStructureName(string name)              // 设置此structurer的STRNAME
    {
        if(m_strStrName)
            delete [] m_strStrName;

        int len = name.size() + 1;
        m_strStrName = new char[len];
        strncpy(m_strStrName, name.c_str(), len);
    }

    string GetStructureName(){return m_strStrName;} // 获取此structurer的STRNAME

    // 获取structure的最大，最小坐标
    void  GetDimMin(CCoord<double>* dim)
    {
        dim->x = m_coordDimMin.x;
        dim->y = m_coordDimMin.y;
    }
    void  GetDimMax(CCoord<double>* dim)
    {
        dim->x = m_coordDimMax.x;
        dim->y = m_coordDimMax.y;
    }

    // 获取指向structure的最大，最小坐标的指针
    CCoord<double>*  GetDimMinP(){return &m_coordDimMin;}
    CCoord<double>*  GetDimMaxP(){return &m_coordDimMax;}

    // 设置structure的最大，最小坐标
    void SetDimMin(CCoord<double> coord){m_coordDimMin = coord;}
    void SetDimMax(CCoord<double> coord){m_coordDimMax = coord;}

    // 1：找到structure的SREF,AREF，TEXT element，根据其SName打到对应的构元添加到被引用构元的链表中；
    // 2：计算得到structure的最大，最小坐标。
    void CalcDimension(CGDSStructures* strlist, bool* broken_link/*, char** err = NULL*/);

    // 获取element的最小x,y，最大x,y。min(min.x,min.y),max(max.x,max.y)
    void GetElementMinMax(CGDSElement* element, CCoord<double>& min, CCoord<double>& max,double angle = 0.0);

    // 将str构元添加到被引用构元的链表中
    void AddRefStructure(CGDSStructure* str);

    // 依据其structure指针在被引用构元的链表中找到对应的被引用的构元
    CGDSRefStructure* FindRefStructure(CGDSStructure* structure);

    // 依据其name在被引用构元的链表中找到对应的被引用的构元
    CGDSRefStructure* FindRefStructure(string name);

    // 获取被引用构元的数目
    int GetNumRefStructure(){return m_obaryRefStructures.size();}
};

class CGDSRefStructure : public QObject
{
protected:
    CGDSStructure* m_pRefStructure;
    char*          m_strStrName;
public:
    CGDSRefStructure()
    {
        m_pRefStructure = NULL;
        m_strStrName = NULL;
    }
    ~CGDSRefStructure()
    {
        if(m_strStrName)
            delete [] m_strStrName;
    }
    CGDSStructure* GetStructure(){return m_pRefStructure;}
    void SetStructure(CGDSStructure* str)
    {
        m_pRefStructure = str;
        if(m_strStrName){
            delete [] m_strStrName;
            m_strStrName = NULL;
        }
    }
    void SetName(string name){
        if(m_strStrName)
            delete [] m_strStrName;
        int len = name.size() + 1;
        m_strStrName = new char[len];
        strncpy(m_strStrName, name.c_str(), len);
    }
    char* GetName()
    {
        return m_strStrName;
    }
};
class CGDSStructures
{
public:
    QList<CGDSStructure*> m_obaryStructures; // 需要被外界用到
protected :
    CGDSStructure* m_structActive;
public :
    CGDSStructure* FindTopStructure();   
    void MakeCrossRef();                     // 找到被引用的构元，并标记此构元为被引用构元
    CGDSStructures()
    {
        m_structActive = NULL;
    }
    ~CGDSStructures();

    void AddStructure();
    CGDSStructure* FindStructure(string name);
    int GetNumStructure(){return m_obaryStructures.size();} // 获得构元的数目
    void ClearActiveStructure(){m_structActive = NULL;}
    CGDSStructure* GetActiveStructure(){return m_structActive;}
};

class CMask : public QObject
{
protected :
    string m_strMask;
public :
    void SetMaskName(string str){m_strMask = str;}
    string GetMaskName(){return m_strMask;}
};


class CGDSFormatType
{
protected :
    QList<CMask*> m_obaryMasks;
public :
    CGDSFormatType(){}
    ~CGDSFormatType();
    void AddFormatType(string str);
};


class CGDShcy :  public CGDSStructures, public CGDSFormatType
{
protected :
    struct tm      m_tmCreationDate;
    struct tm      m_tmLastAccessDate;

    unsigned       m_uintStreamVersion;
    unsigned       m_uintLibDirSize;
    string 	       m_strSrfName;
    string 	       m_strLibName;
    string 	       m_strFonts;
    string 	       m_strRefLibs;
    string 	       m_strAttrTable;
    unsigned       m_uintGenerations;

    double         m_dblUserUnit;
    double         m_dblDBUnit;
    double         maxxvalue;           // 整个.gds文件图形边框maxX
    double         maxyvalue;           // 整个.gds文件图形边框maxY
    double         minxvalue;           // 整个.gds文件图形边框minX
    double         minyvalue;           // 整个.gds文件图形边框minY

protected :
    CCoord<double> m_coordMax;
    CCoord<double> m_coordMin;

public :
    CGDShcy();
    virtual ~CGDShcy();

    // 得到.gds文件的FONTS
    string GetFonts(){return m_strFonts;}
    // 得到.gds文件的version numbers
    unsigned int GetStreamVersion(){return m_uintStreamVersion;}
    // 得到.gds文件的库名
    string GetLibName(){return m_strLibName;}
    // 得到.gds文件的user unit
    double  GetUserUnit(){return m_dblUserUnit;}
    // 得到.gds文件的database unit
    double  GetDBUnit(){return m_dblDBUnit;}

    // 设置好所有structure的最大，最小坐标。
    bool CalculateDimension();
    // 设置好一个structure的最大，最小坐标
    void CalcDimensionRecurse(CGDSStructure* structure, CCoord<double> pos, bool reflect, double angle, double mag);
    // 旋转点angle度
    void RotatePoint(CCoord<double>* point, double& angle);
    // 调整angle在正负360之间
    void fmod360(double& angle);

    struct tm GetCreationDate(){return m_tmCreationDate;}
    struct tm GetAccessDate(){return m_tmLastAccessDate;}

    double GetFrameSize(){return max((maxxvalue - minxvalue),(maxyvalue - minyvalue));} //modified by Bamboo 2017/08/05
    double GetMaxX(){return maxxvalue;}
    double GetMaxY(){return maxyvalue;}
    double GetMinX(){return minxvalue;}
    double GetMinY(){return minyvalue;}

};

class CGDS : public CGDShcy
{
private :
    bool            FontsFlag;               // 标记是否有FONTS这个记录
public:
    CCoord<
    int>       m_aryCoordBuf[GDS_MAX_COORD];
protected :
    unsigned long m_ulongCacheSize;//用于缓存区
    unsigned int  m_uintRecordLength;//记录长度
    unsigned int  m_uintRecordType;//记录类型
    unsigned int  m_uintDataType;//数据类型
    unsigned int  m_uintBufPtr;
    unsigned char m_ucharBuf[GDS_MAX_RECORD_LEN]; //数据存放在m_ucharBuf
    char          m_charBuf[GDS_MAX_STR_LEN];  //用于存放一些字符串如，构元名字，引用构元的名字

    string font_name;// 专用来存放Font字体
    bool font;       // 来标记是否用font_name作为输出

    bool ValidateXY(int elementtype, int num_xy);
    unsigned      DecodeBitArray();
    inline double DecodeReal();
    inline int    DecodeInteger();
    bool          DecodeDateTime(struct tm *tm_set);

    bool          m_bLayerList[GDS_MAX_LAYER];//初始化为false

    int           m_intCoordIdx;

    int           m_intPropAttr;
    string        m_strPropVal;
    unsigned long m_ulNumStructure;
    unsigned long m_ulNumXY;
    unsigned long m_ulNumXYCoord;

public :
    CGDS();
    bool ReadGDSPlain(QString filename);
    bool DecodeRecord();
    bool DecodeAscii(string* set_to, unsigned len = 0);
    void AddLayer(int layer);
    bool DoesLayerExist(int layer);
    string GetFontName(){return font_name;}

    void SetFontsFlag(bool flag) {FontsFlag = flag;}
    bool GetFontsFlag()          {return FontsFlag;}

};


#endif // GDS2READER_H
