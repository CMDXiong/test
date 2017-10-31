/* ========================================================================== */
/* GEOGDS2FILE.CPP -- 基本GDSII图形文件定义
 *
 * REFERENCE: 
 *
 * COPYRIGHT 2017 Optixera.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.00$ (20171001 Chaohong Liu) */
/* HISTORY: 20171001(v1.00) Initial Version;
 * ========================================================================== */

#include "GeoGDS2File.h"
#include "GeoAttribute.h"

#include<math.h>
#include <fstream>
#include <sstream>
#include <list>
#include <ctime>


BEGIN_REGISTRATION(CGeoGDS2File)
REGISTRATION_EXT(CGeoGDS2File, "GDS")
END_REGISTRATION(CGeoGDS2File)


#define GDS_HEADER_LEN        (unsigned long int)4
#define BUFF_SIZE              64*1024

bool IsSwap()
{
    union AAA {
        char sz[2];
        unsigned short sData;
    };
    AAA testtt;
    testtt.sz[0] = '1';
    testtt.sz[1] = '2';

    return testtt.sData == 0X3231;
}

bool     CGeoGDS2File::s_bSwapByte = IsSwap();

CGeoGDS2File::CGeoGDS2File() 
{
    m_wVer = 600;
    m_strLibName = "LIB.DB";
    time_t tCur = time(NULL);
    struct tm * timeinfo= localtime(&tCur);
    memcpy(&m_tmLastModifyTime, timeinfo, sizeof(struct tm));
    memcpy(&m_tmLasetAccesTime, timeinfo, sizeof(struct tm));

    m_dbUserUnit = 0.001;
    m_dbMeterUnit = 1E-9;
}
//读取数据
CGeoScene* CGeoGDS2File::ReadFile(const std::string& strFile)
{
    if (strFile.empty())
        return 0;

    //获取文件的创建时间
    if (!GetTimeOfFile(strFile.c_str(),&m_tCreateTime))
        return 0;

    std::ifstream inFile(strFile.c_str(), std::ifstream::binary);
    if (!inFile)
        return 0;       //文件不存在

    unsigned char szHeadBuf[GDS_HEADER_LEN];
    unsigned char* pszDataBuf = new unsigned char[BUFF_SIZE];
    unsigned  uRecordLength;
    GDS_REC_TYPE recType;
    GDS_DATA_TYPE dataType;
    while (true) 
    {
        inFile.read((char*)szHeadBuf, GDS_HEADER_LEN);
        if (inFile)
        {
            uRecordLength = szHeadBuf[0];
            uRecordLength = (uRecordLength <<8)+ szHeadBuf[1];
            recType = static_cast<GDS_REC_TYPE>(szHeadBuf[2]);
            dataType = static_cast<GDS_DATA_TYPE>(szHeadBuf[3]);

            uRecordLength -= GDS_HEADER_LEN;
        }
        else
            break;

        inFile.read((char*)pszDataBuf, uRecordLength);
        if (!inFile)
        {
            if (!inFile.eof() || inFile.gcount() != uRecordLength)
                break;
        }

        if (!DecodeRecord(recType, dataType,pszDataBuf, uRecordLength))
            break;
    }
    inFile.close();

    //展开数据
    ExpandData();
    //转换为
    CGeoScene* pScene = new CGeoScene;
    CopyDataToScene(pScene);
    return pScene;
}

int CGeoGDS2File::DecodeInteger(unsigned char* pData, bool bShort)
{
    int nResult = 0;
    if (bShort)
    {
        short sTemp;
        unsigned char* ptr = (unsigned char*)&sTemp;
        if (s_bSwapByte)
        {
            ptr[1] = pData[0];
            ptr[0] = pData[1];
        }
        else
        {
            ptr[0] = pData[0];
            ptr[1] = pData[1];
        }
        nResult= sTemp;
    }
    else
    {
        unsigned char* ptr = (unsigned char*)&nResult;
        if (s_bSwapByte)
        {
            for (unsigned i = 0;i < 4;i++)
                *(ptr + 3 - i) = *(pData++);
        }
        else
        {
            for (unsigned i = 0;i < 4;i++)
                *(ptr++) = *(pData++);
        }
    }
    return nResult;
}

void CGeoGDS2File::DecodeDateTime(struct tm *tmTime, unsigned char* pData)
{
    tmTime->tm_year = DecodeInteger(pData);
    if (tmTime->tm_year > 1970) // if incorrectly encoded
        tmTime->tm_year -= 1900;
    if (tmTime->tm_year < 70)   // if incorrectly encoded
        tmTime->tm_year += 100;

    tmTime->tm_mon = DecodeInteger(pData+2) - 1;
    if (tmTime->tm_mon < 0)
        tmTime->tm_mon = 0;
    if (tmTime->tm_mon > 11)
        tmTime->tm_mon = 11;

    tmTime->tm_mday = DecodeInteger(pData+4);
    if (tmTime->tm_mday < 0)
        tmTime->tm_mday = 0;
    if (tmTime->tm_mday > 31)
        tmTime->tm_mday = 31;

    tmTime->tm_hour = DecodeInteger(pData+6);
    if (tmTime->tm_hour < 0)
        tmTime->tm_hour = 0;
    if (tmTime->tm_hour > 23)
        tmTime->tm_hour = 23;

    tmTime->tm_min = DecodeInteger(pData+8);
    if (tmTime->tm_min < 0)
        tmTime->tm_min = 0;
    if (tmTime->tm_min > 59)
        tmTime->tm_min = 59;

    tmTime->tm_sec = DecodeInteger(pData+10);
    if (tmTime->tm_sec < 0)
        tmTime->tm_sec = 0;
    if (tmTime->tm_sec > 59)
        tmTime->tm_sec = 59;

    tmTime->tm_isdst = 0;
    tmTime->tm_wday = 0;
    tmTime->tm_yday = 0;
}

time_t CGeoGDS2File::DecodeDateTime(unsigned char* pData)
{
    struct tm tmTime;
    DecodeDateTime(&tmTime, pData);

    return mktime(&tmTime);
}
double CGeoGDS2File::DecodeReal(unsigned char* pData, bool bDouble)
{
    long long mantissa = 0;           //尾数
    int exp16 = (pData[0] & 0x7F)-64; //真正的指数值
    mantissa = unsigned(DecodeInteger(pData, false)) & 0xffffff;
    if (bDouble)
    {
        mantissa <<= 32;
        mantissa += unsigned(DecodeInteger(pData + 4, false));
        exp16 -= 14;                 //把尾数调整为小数，14*4=56位
    }
    else
    {
        exp16 -= 6;                 //把尾数调整为小数，6*4=24位
    }

    if ((pData[0] & 0x80) != 0) {mantissa = -mantissa;}

    if (exp16 != 0)
        return pow(16.0, exp16)*mantissa;
    else
        return mantissa*1.0;
}

bool CGeoGDS2File::DecodeCoord(unsigned char* lpBuff, unsigned uSize)
{
    CGeoGDSElement* pElement = m_vecStruct.back()->BackElement();
    int nNum = uSize / 8;
    unsigned short wType = pElement->GetObjType();

    //检查最基本的坐标数
    bool bOk = false;
    switch (wType)
    {
    case    GEO_TYPE_BOUNDARY:
        if(nNum >=4)
            bOk=true;
        nNum--;         //去掉重复的点
        break;
    case    GEO_TYPE_PATH:
        if (nNum >= 2)
            bOk = true;
        break;
    case    GEO_TYPE_REF_REV:
        if (nNum >= 1)
            bOk = true;
        break;
    case    GEO_TYPE_AREF:
        if (nNum == 3)
            bOk = true;
        break;
    case    GEO_TYPE_SREF:
    case    GEO_TYPE_GDS_TEXT:
        if (nNum == 1)
            bOk = true;
        break;
    case    GEO_TYPE_BOX:
        if (nNum == 5)
            bOk = true;
        nNum--;         //去掉重复的点
        break;

    case    GEO_TYPE_NODE:
        if (nNum >= 1)
            bOk = true;
        break;
    default:
        return false;
    }

    if (!bOk)
        return false;

    //注意：目前假设最后一个点和第一个点位重复的，另外也没有针对BOX类型做优化

    for (int i = 0; i < nNum;i++)
    {
        int x = DecodeInteger(lpBuff + i*8,false);
        int y = DecodeInteger(lpBuff + i * 8 +4, false);
        pElement->AddCoordPt(x,y);
    }
    return true;
}

bool CGeoGDS2File::DecodeRecord(GDS_REC_TYPE recType,GDS_DATA_TYPE dataType, unsigned char* lpBuff, unsigned uSize)
{
    unsigned short wProNum = 0;
    switch (recType) 
    {
    //File information-------------------------------------
    case GDS_HEADER:
        m_wVer = DecodeInteger(lpBuff, dataType== GDS_Integer2);
        if (m_wVer < 600) //对于6.0版本以及之后的，次要版本为两位数，对于6.0之前的，没有次要版本,则统一乘以100
            m_wVer *= 100;
        break;

    case GDS_BGNLIB:
        if(uSize >= 12)
            DecodeDateTime(&m_tmLastModifyTime, lpBuff);
        if(uSize >= 24)
            DecodeDateTime(&m_tmLasetAccesTime, lpBuff+12);
        break;
    case GDS_LIBNAME:
        m_strLibName.assign((char*)lpBuff, uSize);
        break;

    case GDS_UNITS:
        if (uSize >= 8)
            m_dbUserUnit = DecodeReal(lpBuff);
        if (uSize >= 16)
            m_dbMeterUnit = DecodeReal(lpBuff + 8);
        break;

    case GDS_REFLIBS:
        if (uSize >= 44)
        {
            int nNum = uSize / 44;
            for (int i = 0;i < nNum;i++)
            {
                char szBuf[45];
                memcpy(szBuf, lpBuff + i * 44, 44);
                szBuf[44] = 0;
                if(szBuf[0] != 0)
                    m_vecRefLibs.push_back(szBuf);
            }
        }
        break;

    case GDS_FONTS:
        if (uSize >= 44)
        {
            int nNum = uSize / 44;
            for (int i = 0;i < nNum;i++)
            {
                char szBuf[45];
                memcpy(szBuf, lpBuff + i * 44, 44);
                szBuf[44] = 0;
                if (szBuf[0] != 0)
                    m_vecfontFile.push_back(szBuf);
            }
        }
        break;

    case GDS_ATTRTABLE:
        if (uSize >= 44)
        {
            int nNum = uSize / 44;
            for (int i = 0;i < nNum;i++)
            {
                char szBuf[45];
                memcpy(szBuf, lpBuff + i * 44, 44);
                szBuf[44] = 0;
                if (szBuf[0] != 0)
                    m_vecAttrFile.push_back(szBuf);
            }
        }
        break;

    case GDS_GENERATIONS:
        if (uSize >= 2)
            m_wGenerations = static_cast<unsigned short>(DecodeInteger(lpBuff));
        break;

    case GDS_ENDLIB: // End of library
        break;
    //End File information-----------------------------------

    // not handled-------------------------------------------
    case GDS_FORMAT:
        break;

    case GDS_MASK:
        break;

    case GDS_ENDMASKS:
        break;

    case GDS_LIBDIRSIZE:
        break;

    case GDS_SRFNAME:
        break;

    case GDS_LIBSECUR:
        break;

    case GDS_BGNEXTN:
        break;

    case GDS_ENDEXTN:
        break;
    //-------------------------------------------------------
    //Struct information-------------------------------------
    case GDS_BGNSTR:
        if (uSize >= 12)
        {
            CGeoGDSStruct* pTemp = new CGeoGDSStruct;
            m_vecStruct.push_back(pTemp);
            pTemp->SetCreateTime(DecodeDateTime(lpBuff));
            if (uSize >= 24)
                pTemp->SetLastModifyTime(DecodeDateTime(lpBuff + 12));
        }
        break;

    case GDS_STRNAME:
        if (!m_vecStruct.empty() && uSize > 1)
        {
            CGeoGDSStruct* pTemp = m_vecStruct.back().get();
            if (*(lpBuff + uSize - 1) == 0)
                uSize--;
            std::string strTemp((char*)lpBuff, uSize);
            pTemp->SetName(strTemp);
            m_mapNameToStru.insert(std::pair<std::string, ref_ptr<CGeoGDSStruct> >(strTemp, pTemp));
        }
            
        break;
    case GDS_STRCLASS:      //not use
        break;
    case GDS_ENDSTR:
        break;
    //End Struct information---------------------------------

    //Element Type information-------------------------------------
    case GDS_BOUNDARY:
        if (!m_vecStruct.empty())
            AddElementToStruct(m_vecStruct.back().get(), new CGeoGDSBoundary());
        break;

    case GDS_PATH:
        if (!m_vecStruct.empty())
            AddElementToStruct(m_vecStruct.back().get(), new CGeoGDSPath());
        break;

    case GDS_SREF:
    case GDS_AREF:
        if (!m_vecStruct.empty())
        {
            CGeoGDSRef* lpTemp = new CGeoGDSRef();
            AddElementToStruct(m_vecStruct.back().get(), lpTemp);
            m_vecRefElement.push_back(lpTemp);
            lpTemp->SetRecType(recType);
        }
        break;

    case GDS_TEXT:
        if (!m_vecStruct.empty())
            AddElementToStruct(m_vecStruct.back().get(), new CGeoGDSText());
        break;

    case GDS_NODE:
        if (!m_vecStruct.empty())
            AddElementToStruct(m_vecStruct.back().get(), new CGeoGDSNode());
        break;

    case GDS_BOX:
        if (!m_vecStruct.empty())
            AddElementToStruct(m_vecStruct.back().get(), new CGeoGDSBox());
        break;

    case GDS_ENDEL:
        break;
    //End Element Type information---------------------------------

    //Element Data information-------------------------------------
    case GDS_LAYER:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty())
        {
            ref_ptr<CGeoGDSElement> element = m_vecStruct.back()->BackElement();
            int nLayer = DecodeInteger(lpBuff, dataType == GDS_Integer2);
            element->SetLayerNo(static_cast<unsigned short>(nLayer));
        }
        break;

    case GDS_DATATYPE:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty())
            m_vecStruct.back()->BackElement()->SetDataType(static_cast<unsigned short>(DecodeInteger(lpBuff, dataType == GDS_Integer2)));
        break;

    case GDS_XY:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty())
            DecodeCoord(lpBuff, uSize);
        break;

    case GDS_PLEX:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty())
            m_vecStruct.back()->BackElement()->SetPlex(static_cast<unsigned>(DecodeInteger(lpBuff, false)));
        break;

    case GDS_ELFLAGS:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty())
            m_vecStruct.back()->BackElement()->SetFlags(static_cast<unsigned short>(DecodeInteger(lpBuff)));
        break;

    case GDS_WIDTH:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty())
        {
            CGeoGDSElement* pElement = m_vecStruct.back()->BackElement();
            unsigned short wType = pElement->GetObjType();
            int nWidth = DecodeInteger(lpBuff, false);

            if (GEO_TYPE_PATH == wType || GEO_TYPE_GDS_TEXT == wType)
            {
                //修改为最新的属性结构操作 2017/10/14 by bamboo
                //--------------------------------------------
                static_cast<CGeoLWPolyline*>(pElement->GetChild())->GetForcedProperty()->SetProValue(new CProPen(nWidth));
                //--------------------------------------------
               // static_cast<CGeoLWPolyline*>(pElement->GetChild())->SetPenSize(nWidth); bamboo 2017/09/29暂时屏蔽
            }               
        }
        break;
    //End Element Data information---------------------------------

    //Element Property Start---------------------------
    case GDS_PROPATTR:
        if(uSize >= 2)
            wProNum = static_cast<unsigned short>(DecodeInteger(lpBuff));
        break;

    case GDS_PROPVALUE:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty() && uSize > 1 && wProNum > 0)
        {
            CGeoGDSElement* pElement = m_vecStruct.back()->BackElement();
            if (pElement != 0)
            {
                if (*(lpBuff + uSize - 1) == 0)
                    uSize--;
                std::string strTemp((char*)lpBuff, uSize);
                pElement->AddElemntProperty(wProNum, strTemp);
                wProNum = 0; //复位
            }
        }
        break;

    //Element Property End-----------------------------
    case GDS_PATHTYPE:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty())
        {
            CGeoGDSElement* pElement = m_vecStruct.back()->BackElement();
            if (GEO_TYPE_PATH == pElement->GetObjType())
            {
                int nPathType = DecodeInteger(lpBuff);
                CGeoGDSPath::PATHTYPE eType;
//                if (nPathType < 0 || nPathType >= CGeoGDSPath::PATHTYPE::PATHTYPE_MAX)
//                    eType = CGeoGDSPath::PATHTYPE::SQUARE_END_TYPE;
//                else
//                    eType = static_cast<CGeoGDSPath::PATHTYPE>(nPathType);
                if (nPathType < 0 || nPathType >= (CGeoGDSPath::PATHTYPE_MAX))
                    eType = CGeoGDSPath::SQUARE_END_TYPE;
                else
                    eType = static_cast<CGeoGDSPath::PATHTYPE>(nPathType);
                static_cast<CGeoGDSPath*>(pElement)->SetPathType(eType);
            }    //其他忽略,特别是对于文本，以后可能要考虑
        }
        break;

    case GDS_NODETYPE:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty())
        {
            CGeoGDSElement* pElement = m_vecStruct.back()->BackElement();
            if (GEO_TYPE_NODE == pElement->GetObjType())
            {
                static_cast<CGeoGDSNode*>(pElement)->SetNodeType(DecodeInteger(lpBuff));
            }    //其他忽略,特别是对于文本，以后可能要考虑
        }
        break;

    case GDS_BOXTYPE:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty() && uSize >= 2)
        {
            CGeoGDSElement* pElement = m_vecStruct.back()->BackElement();
            if (GEO_TYPE_BOX == pElement->GetObjType())
            {
                static_cast<CGeoGDSBox*>(pElement)->SetBoxType(static_cast<unsigned short>(DecodeInteger(lpBuff)));
            }
        }
        break;

    //////////////////////////////////////////////////
    // SREF和AREF的矩阵参数
    case GDS_STRANS:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty() && uSize>=2)
        {
            CGeoGDSElement* pElement = m_vecStruct.back()->BackElement();
            unsigned short wType = pElement->GetObjType();
            int nData = DecodeInteger(lpBuff);

            if (GEO_TYPE_REF_REV == wType || GEO_TYPE_GDS_TEXT == wType)
            {
                static_cast<CGeoGDSRef*>(pElement)->SetTransOption(static_cast<unsigned short>(nData));
            }
        }
        break;

    case GDS_MAG:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty() && uSize >= 8)
        {
            CGeoGDSElement* pElement = m_vecStruct.back()->BackElement();
            unsigned short wType = pElement->GetObjType();
            double dbData = DecodeReal(lpBuff);

            if (GEO_TYPE_REF_REV == wType || GEO_TYPE_GDS_TEXT == wType)
            {
                static_cast<CGeoGDSRef*>(pElement)->SetMagnification(dbData);
            }
        }
        break;

    case GDS_ANGLE:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty() && uSize >= 8)
        {
            CGeoGDSElement* pElement = m_vecStruct.back()->BackElement();
            unsigned short wType = pElement->GetObjType();
            double dbData = DecodeReal(lpBuff);

            if (GEO_TYPE_REF_REV == wType|| GEO_TYPE_GDS_TEXT== wType)
            {
                static_cast<CGeoGDSRef*>(pElement)->SetAngle(dbData);
            }
        }
        break;

    case GDS_SNAME:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty() && uSize >= 1)
        {
            CGeoGDSElement* pElement = m_vecStruct.back()->BackElement();
            unsigned short wType = pElement->GetObjType();

            if (GEO_TYPE_REF_REV == wType)
            {
                if (*(lpBuff + uSize - 1) == 0)
                    uSize--;
                std::string strTemp((char*)lpBuff, uSize);
                static_cast<CGeoGDSRef*>(pElement)->SetRefName(strTemp);
            }
        }
        break;

    case GDS_COLROW:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty() && uSize >= 4)
        {
            CGeoGDSElement* pElement = m_vecStruct.back()->BackElement();
            int nRow = DecodeInteger(lpBuff);
            int nCol = DecodeInteger(lpBuff+2);

            if (GEO_TYPE_REF_REV == pElement->GetObjType())
            {
                static_cast<CGeoGDSRef*>(pElement)->SetRowAndCol(nRow, nCol);
            }
        }
        break;
    //////////////////////////////////////////////////

    //////////////////////////////////////////////////
    //Text
    case GDS_TEXTTYPE:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty() && uSize >= 2)
        {
            CGeoGDSElement* pElement = m_vecStruct.back()->BackElement();
            if (GEO_TYPE_GDS_TEXT == pElement->GetObjType())
            {
                static_cast<CGeoGDSText*>(pElement)->SetTextType(static_cast<unsigned short>(DecodeInteger(lpBuff)));
            }
        }
        break;

    case GDS_PRESENTATION:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty() && uSize >= 2)
        {
            CGeoGDSElement* pElement = m_vecStruct.back()->BackElement();
            if (GEO_TYPE_GDS_TEXT == pElement->GetObjType())
            {
                static_cast<CGeoGDSText*>(pElement)->SetPresentation(static_cast<unsigned short>(DecodeInteger(lpBuff)));
            }
        }
        break;

    case GDS_STRING:
        if (!m_vecStruct.empty() && !m_vecStruct.back()->IsEmpty() && uSize >= 1)
        {
            CGeoGDSElement* pElement = m_vecStruct.back()->BackElement();
            if (GEO_TYPE_GDS_TEXT == pElement->GetObjType())
            {
                if (*(lpBuff + uSize - 1) == 0)
                    uSize--;
                std::string strTemp((char*)lpBuff, uSize);
                static_cast<CGeoGDSText*>(pElement)->SetText(strTemp);
            }
        }
        break;
     //////////////////////////////////////////////////
    default:
        return FALSE;
        break;
    }

    return TRUE;
}

bool CGeoGDS2File::WriteFile(const std::string& /*strFile*/, const CGeoScene* /*lpData*/)
{
    return false;
}


//把SREF，AREF的数据展开
void CGeoGDS2File::ExpandRefStruct()
{
    if (m_vecRefElement.empty())
        return;

    //遍历引用节点，然后逐一处理
    for (unsigned i = 0; i < m_vecRefElement.size();i++)
    {
        CGeoGDSRef* pRefTemp = m_vecRefElement.at(i).get();
        std::map<std::string, ref_ptr<CGeoGDSStruct> >::iterator pos = m_mapNameToStru.find(pRefTemp->GetRefName());
        if (pos == m_mapNameToStru.end())   //该名字找不到对应的结构，则表示该SREF或者AREF的数据是错误的，则应该从包含该元素的结构里面删除该对象，目前并未实现删除
        {
            continue;
        }
        if (pos->second->GetChildCount() == 0)
            continue;

        CGeoGDSSRef* pNewRefElement = 0;
        if (pRefTemp->GetRecType() == GDS_AREF)
        {
            pNewRefElement = new CGeoGDSARef;
        }
        else
        {
            pNewRefElement = new CGeoGDSSRef;
        }

        dynamic_cast<CGeoGDSStruct*>(pRefTemp->GetParent())->AddElement(pNewRefElement); //新增的添加到结构中
        pNewRefElement->SetElementChild(pos->second.get()); //把cell节点设置为元素节点
        //pNewRefElement->SetLayerNo(pRefTemp->GetLayerNo()); //对于REF节点，层号无效
        pNewRefElement->SetDataType(pRefTemp->GetDataType());
        pNewRefElement->SetFlags(pRefTemp->GetFlags());
        pNewRefElement->SetPlex(pRefTemp->GetPlex());
        pNewRefElement->SetElemntProperty(pRefTemp->GetElemntProperty());

        if (pRefTemp->GetProperty() != 0)
        {
            pNewRefElement->SetProperty(pRefTemp->GetProperty());
        }

        //设置矩阵参数
        CMatrix matCur;
        CGeoPt ptTemp = pRefTemp->GetCoordPt(0);    //参考点
        matCur.MakeTranslate(ptTemp.dx, ptTemp.dy);
        pNewRefElement->SetReferencePt(ptTemp.dx, ptTemp.dy);

        unsigned short wTrans = pRefTemp->GetTransOption();
        if ((1 & wTrans) != 0)
        {
            matCur *= CMatrix::Scale(1,-1);
        }

        double dbAngle = pRefTemp->GetAngle();
        if(dbAngle > MIN_PRECISION)
            matCur *= CMatrix::Rotate(dbAngle*M_PI/180);

        double dbMag = pRefTemp->GetMagnification();
        if (dbMag > MIN_PRECISION && fabs(dbMag-1.0) > MIN_PRECISION)
            matCur *= CMatrix::Scale(dbMag, dbMag);

        pNewRefElement->SetTransformMatrix(matCur);

        if (pRefTemp->GetRecType() == GDS_AREF) //计算行与列
        {
            CGeoArray* pArray = dynamic_cast<CGeoArray*>(pNewRefElement->GetChild());
            if (pArray != 0)
            {
                int nRow, nCol;
                pRefTemp->GetRowAndCol(nRow, nCol);
                pArray->SetRowAndCol(nRow, nCol);
                //计算行间距和列间距

                double dbInterCol, dbInterRow;
                CGeoPt ptTemp1 =  pRefTemp->GetCoordPt(1);
                dbInterCol = sqrt((ptTemp1.dx- ptTemp.dx)*(ptTemp1.dx - ptTemp.dx) + (ptTemp1.dy - ptTemp.dy)*(ptTemp1.dy - ptTemp.dy))/ nCol;
                ptTemp1 = pRefTemp->GetCoordPt(2);
                dbInterRow = sqrt((ptTemp1.dx - ptTemp.dx)*(ptTemp1.dx - ptTemp.dx) + (ptTemp1.dy - ptTemp.dy)*(ptTemp1.dy - ptTemp.dy)) / nRow;
                pArray->SetRowAndColInterDist(dbInterRow, dbInterCol);
            }
        }
    }

    //删除所有的CGeoGDSRef节点
    for (unsigned i = 0; i < m_vecRefElement.size();i++)
    {
        CGeoGDSRef* pRefTemp = m_vecRefElement.at(i).get();
        dynamic_cast<CGeoGDSStruct*>(pRefTemp->GetParent())->DelElemet(pRefTemp);
    }
}

void CGeoGDS2File::CopyDataToScene(CGeoScene* pScene)
{
    pScene->SetVersion(m_wVer);
    if (!m_strLibName.empty())
        pScene->SetSceneName(m_strLibName);
    pScene->SetSysTime(m_tCreateTime, mktime(&m_tmLastModifyTime), mktime(&m_tmLasetAccesTime));
    pScene->SetUnit(m_dbUserUnit, m_dbMeterUnit);

    GeoLayerList& lstLayer = pScene->GetLayerList();
    GeoStructList& lstCell = pScene->GetCellList();

    //找出Cell所在的所有层数据
    for (unsigned i = 0;i < m_vecStruct.size();i++)
    {
        CGeoGDSStruct* pCell = m_vecStruct.at(i).get();
        //不是根cell或者cell没有子节点，则忽略
        if (pCell->GetChildCount() == 0 || pCell->GetRefCount() != 0)
            continue;
        
        RecursiveFindLayerNO(0, pCell);
    }

    std::map<unsigned short, CGeoLayer*> mapLayer;
    for (unsigned i = 0;i < m_vecStruct.size();i++)
    {
        CGeoGDSStruct* pCell = m_vecStruct.at(i).get();
        lstCell.push_back(pCell);

        //从cell中读取所属的层，然后把该cell添加到层列表中
        const std::set<unsigned short>& lstLayer = pCell->GetLayerList();
        std::set<unsigned short>::const_iterator posLayer = lstLayer.begin();
        while (posLayer != lstLayer.end())
        {
            unsigned short wLay = *posLayer;
            std::map<unsigned short, CGeoLayer*>::iterator pos = mapLayer.find(wLay);
            CGeoLayer* pLayer = 0;
            if (pos != mapLayer.end())
            {
                pLayer = pos->second;
            }
            else
            {
                pLayer = new CGeoLayer;
                mapLayer[wLay] = pLayer;
                pLayer->SetLayerNo(wLay);
                //缺省把层号变成这个层的名字
                std::ostringstream strOf;
                strOf << "GDS_" << wLay;
                pLayer->SetName(strOf.str());
            }
            pLayer->AddObj(pCell);

            posLayer++;
        }
    }

    std::map<unsigned short, CGeoLayer*>::iterator pos = mapLayer.begin();
    while (pos != mapLayer.end())
    {
        lstLayer.push_back(pos->second);
        pos++;
    }
}

//递归查找所有cell所在的层
void CGeoGDS2File::RecursiveFindLayerNO(CGeoGDSStruct* parent, CGeoGDSStruct* cur)
{
    for (unsigned i = 0; i < cur->GetChildCount(); i++)
    {
        CGeoGDSElement* pTemp = static_cast<CGeoGDSElement*>(cur->GetChild(i));
        if (pTemp->GetObjType() == GEO_TYPE_REF_REV)
            continue;

        unsigned short wType = pTemp->GetObjType();
        if (GEO_TYPE_AREF == wType || GEO_TYPE_SREF == wType)
        {
            RecursiveFindLayerNO(cur, static_cast<CGeoGDSStruct*>(pTemp->GetElementChild()));
        }
        else
        {
            unsigned short wLlayerNo = pTemp->GetLayerNo();
            cur->AddLayerNO(wLlayerNo);
            if (parent != 0)
                parent->AddLayerNO(wLlayerNo);
        }
    }    
}
