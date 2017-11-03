/* ========================================================================== */
/* GEOGDS2FILE.H -- 基本GDSII图形文件定义头文件
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

enum GDS_REC_TYPE {
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

enum strans {
    GDS_STRANS_REFRECT = 0x8000,
    GDS_STRANS_ABS_MAG = 0x0004,
    GDS_STRANS_ABS_ANG = 0x0002,
};

enum element {
    GDS_ELEMENT_TEMPLATE = 0x01,
    GDS_ELEMENT_EXTERNAL = 0x02,
};

enum GDS_DATA_TYPE {
    GDS_NoData = 0,
    GDS_BitArray,
    GDS_Integer2,
    GDS_Integer4,
    GDS_Real4, // not used
    GDS_Real8,
    GDS_Ascii
};

class CGeoGDSRef : public CGeoGDSElement
{
protected:
    std::string     m_strRefStructName;
    unsigned short  m_wTransOption;
    FLOAT_T         m_dbMagnification;
    FLOAT_T         m_dbAngle;
    int             m_nRows, m_nCols;                 //行、列方向的个数，如果这两者为1
    GDS_REC_TYPE    m_recType;
public:
    CGeoGDSRef()
    {
        m_wType = GEO_TYPE_REF_REV;
        m_wTransOption = 0;
        m_dbMagnification = 1.0;
        m_dbAngle = 0;
        m_nRows = m_nCols = 1;
        AddChild(new CGeoLWPolyline);
    }
    inline void SetRefName(const std::string& strName) { m_strRefStructName = strName; }
    inline const std::string& GetRefName() { return m_strRefStructName; }

    inline void SetTransOption(unsigned short wData) { m_wTransOption = wData; }
    inline unsigned short GetTransOption() { return m_wTransOption; }

    inline void SetMagnification(FLOAT_T dbData) { m_dbMagnification = dbData; }
    inline FLOAT_T GetMagnification() { return m_dbMagnification; }

    inline void SetAngle(FLOAT_T dbData) { m_dbAngle = dbData; }
    inline FLOAT_T GetAngle() { return m_dbAngle; }

    inline void SetRecType(GDS_REC_TYPE type) { m_recType = type; }
    inline GDS_REC_TYPE GetRecType() { return m_recType; }

    inline void SetRowAndCol(int nRow, int nCol) { m_nRows = nRow; m_nCols = nCol; }
    inline void GetRowAndCol(int& nRow, int& nCol) { nRow = m_nRows; nCol = m_nCols; }

    virtual void AddCoordPt(FLOAT_T x, FLOAT_T y) { static_cast<CGeoLWPolyline*>(GetChild())->AddPt(x, y); }
    virtual CGeoPt GetCoordPt(int nIndex = 0) { return static_cast<CGeoLWPolyline*>(GetChild())->GetPt(nIndex); }
};

//DXF文件
class CGeoGDS2File : public CGeoFileBase
{
    static bool     s_bSwapByte;
    std::map<std::string, ref_ptr<CGeoLayer> > m_mapNameToLayer;
    
    //数据定义
    unsigned short  m_wVer;                 //次要版本为2位数
    unsigned short  m_wGenerations;         //
    time_t          m_tCreateTime;           //创建修改时间
    struct tm       m_tmLastModifyTime;     //最后修改时间
    struct tm       m_tmLasetAccesTime;     //最后访问时间
    std::string     m_strLibName;         
    double          m_dbUserUnit;
    double          m_dbMeterUnit;

    GeoStructList                                   m_vecStruct;
    //std::map<int, GeoElementList>                   m_mapLayerToElement;    //层号对应着元素
    std::map<std::string, ref_ptr<CGeoGDSStruct> >  m_mapNameToStru;        //结构名对应着结构,假设结构名是唯一的
    std::vector<ref_ptr<CGeoGDSRef> >               m_vecRefElement;        //SREF和AREF的元素表
    std::vector<std::string>                        m_vecRefLibs;           //参考库列表
    std::vector<std::string>                        m_vecfontFile;          //字体定义文件列表
    std::vector<std::string>                        m_vecAttrFile;          //属性定义文件列表

private:
    bool DecodeRecord(GDS_REC_TYPE recType,GDS_DATA_TYPE dataType,unsigned char* lpBuff, unsigned uSize);
    int DecodeInteger(unsigned char* pData, bool bShort = true);
    void DecodeDateTime(struct tm *tmTime, unsigned char* pData);
    time_t DecodeDateTime(unsigned char* pData);
    double DecodeReal(unsigned char* pData, bool bDouble = true);
    bool DecodeCoord(unsigned char* lpBuff, unsigned uSize);

    void AddElementToStruct(CGeoGDSStruct* pStruct, CGeoGDSElement*pElem)
    {
        pElem->SetParent(pStruct);
        pStruct->AddElement(pElem);
    }

    //展开所有关联的数据
    void ExpandData()
    {
        ExpandRefStruct();
        ExpandRefLibs();
        ExpandRefAttrs();
        ExpandRefFonts();
    }
    //把SREF，AREF的数据展开
    void ExpandRefStruct();
    void ExpandRefLibs() {}
    void ExpandRefAttrs() {}
    void ExpandRefFonts() {}

    void CopyDataToScene(CGeoScene* pScene);
    //递归查找所有cell所在的层
    void RecursiveFindLayerNO(CGeoGDSStruct* parent, CGeoGDSStruct* cur);
public:
    CGeoGDS2File();
    virtual CGeoScene* ReadFile(const std::string& strFile);
    virtual bool WriteFile(const std::string& strFile, const CGeoScene* lpData);


DECLARE_FILEFORMAT(CGeoGDS2File)
};

#endif
