/* ========================================================================== */
/* GEODXFFILE.H -- 基本DXF图形定义头文件
 *
 * REFERENCE: 
 *
 * COPYRIGHT 2017 Optixera.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.00$ (20171001 Chaohong Liu) */
/* HISTORY: 20171001(v1.00) Initial Version;
 * ========================================================================== */

#ifndef CGEODXFFILE_H_LIU
#define CGEODXFFILE_H_LIU

#include "GeoFile.h"
#include<map>

#define GEO_TYPE_ELLIPSE_DXF     GEO_TYPE_USER+5

class CGeoDxfEllipse : public CGeoBase
{
protected:
    CGeoPt	  ptCenter;                 // 圆心点坐标
    CGeoPt    ptAxis;                   //长轴端点的坐标
    CGeoPt    ptAngle;                  //起始角度和终止角度
    FLOAT_T   m_dbRate;                 //短轴与长轴的比例

public:
    CGeoDxfEllipse()
    {
        m_wType = GEO_TYPE_ELLIPSE_DXF;
        m_dbRate = 1;
    }
    CGeoPt& GetCenterPt() { return ptCenter; }
    CGeoPt& GetAxisPt() { return ptAxis; }
    CGeoPt& GetAngle() { return ptAngle; }
    void SetEllipticity(FLOAT_T val) { m_dbRate = val; }
    FLOAT_T GetEllipticity() { return m_dbRate; }
    virtual int GetObjDataLen() const { return 0; }
    virtual CGeoPt GetCentrePoint() const { return ptCenter; }
    virtual CGeoRect GetBoundBox() const            //Add by xiong 17/09/07
    {
        return CGeoRect();
    }
};

//DXF文件
class CGeoDXFFile : public CGeoFileBase
{
    struct Code_Val{
        int nCode;
        std::string strVar;

        Code_Val(){}
        Code_Val(const Code_Val& val)
        {
            nCode = val.nCode;
            strVar= val.strVar;
        }

        Code_Val(int code, const std::string& val)
        {
            nCode = code;
            strVar= val;
        }
    };

     struct Code_Section{
        std::string strName;    //段名
        std::vector<Code_Val> allVal;   //所有的值
        bool        bInit;
        Code_Section() { bInit = false;}
        void clear() { bInit = false;strName.clear(); }
    };

    struct DXF_HEAD
    {
        double  dbExtX1,dbExtY1,dbExtZ1;//左下角
        double  dbExtX2,dbExtY2,dbExtZ2;
        double  dbLimitX1,dbLimitY1;//左下角
        double  dbLimitX2,dbLimitY2;
        double  dbBaseAngle;
        unsigned short    wVer;       //12表示R12 ，2009表示CAD2009
        bool    bCCWAngle;  //逆时针方向角度为正
    };

    std::map<std::string, ref_ptr<CGeoLayer> > m_mapNameToLayer;
    std::map<std::string,std::string>    m_mapLayerFilter;   //层过滤列表，列表中的项采用UTF8格式保存 utf8字符串对应着ANSI

public:
    typedef enum { eDXFNull, eDXFPoint, eDXFLine, eDXFPolyline, eDXFArc, eDXFCircle, eDXFEllipse, eDXFText, eDXFLWPolyline, eDXFMText, eDXFDimension } enumDXFObjType;

private:
    bool PhaseObj(CGeoBase *pObj,enumDXFObjType type, Code_Val& val);
    bool ReadDxfSection(Code_Section& section, const std::string& strFirst, const std::string& strSecond);
    void PhaseHeadData(std::vector<Code_Val>& allVal,CGeoLayer* pLayer=0);
    void PhaseClassData(std::vector<Code_Val>& allVal,CGeoLayer* pLayer=0);
    void PhaseTableData(std::vector<Code_Val>& allVal,CGeoLayer* pLayer=0);
    void PhaseBlockData(std::vector<Code_Val>& allVal,CGeoLayer* pLayer=0);
    void PhaseEntityData(std::vector<Code_Val>& allVal,CGeoLayer* pLayer=0);
    void PhaseObjectData(std::vector<Code_Val>& allVal,CGeoLayer* pLayer=0);
    void PhaseThumbData(std::vector<Code_Val>& allVal,CGeoLayer* pLayer=0);

    //分析标注信息
    //bool PhaseDimensionGeo(CGeoBase *pObj,Code_Val& val);
    void TransferLayerToScene(CGeoScene* pScene);

    bool IsValidPoint(double x,double y)
    {
        if(x < m_headDXF.dbLimitX1 || x > m_headDXF.dbLimitX2 || y < m_headDXF.dbLimitY1 || y > m_headDXF.dbLimitY2)
            return false;
        else
            return true;
    }
    //把图形转化为基本图元图形
    CGeoGDSElement* ConvertPrimitiveToElement(CGeoBase *pObj);
public:
    CGeoDXFFile() {}
    virtual CGeoScene* ReadFile(const std::string& strFile);
    virtual bool WriteFile(const std::string& strFile, const CGeoScene* lpData);

    //添加层过滤器
    //返回标志总共有多少层过滤器
    int AddLayerFilter(const std::string& strFilter,const std::string& strLayer);
protected:
    char szCurLine[256];
	enumDXFObjType curDXFObjType;
    DXF_HEAD   m_headDXF;           //dxf文件头数据

DECLARE_FILEFORMAT(CGeoDXFFile)
};

#endif
