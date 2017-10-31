/* ========================================================================== */
/* GEODXFFILE.CPP -- 基本DXF图形定义
 *
 * REFERENCE: 
 *
 * COPYRIGHT 2017 Optixera.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.00$ (20171001 Chaohong Liu) */
/* HISTORY: 20171001(v1.00) Initial Version;
 * ========================================================================== */

#include "GeoDxfFile.h"

#include<math.h>
#include <fstream>
#include <sstream>
#include <list>


BEGIN_REGISTRATION(CGeoDXFFile)
REGISTRATION_EXT(CGeoDXFFile, "DXF")
END_REGISTRATION(CGeoDXFFile)


void  Utf8ToAnsi(std::string& strData)
{
    wchar_t* pwszBuf = new wchar_t[strData.size()+2];
    char* pszBuf = new char[strData.size()+2];

    int nLen =	MultiByteToWideChar(CP_UTF8,0,strData.c_str(),-1,pwszBuf,strData.size()+1);
    pwszBuf[nLen] = 0;

    nLen = WideCharToMultiByte(CP_ACP,0,pwszBuf,nLen,pszBuf,strData.size()+1,0,0);
    pszBuf[nLen] = 0;
    strData = pszBuf;
    delete[] pwszBuf;
    delete[] pszBuf;
}

void  AnsiToUtf8(std::string& strData)
{
    wchar_t* pwszBuf = new wchar_t[strData.size()+2];

    int nLen =	MultiByteToWideChar(CP_ACP,0,strData.c_str(),-1,pwszBuf,strData.size()+2);
    //pwszBuf[nLen] = 0;

    nLen = WideCharToMultiByte(CP_UTF8,0,pwszBuf,-1,0,0,0,0);
    char* pszBuf = new char[nLen+1];

    WideCharToMultiByte(CP_UTF8,0,pwszBuf,-1,pszBuf,nLen+1,0,0);
    //pszBuf[nLen] = 0;
    strData = pszBuf;
    delete[] pwszBuf;
    delete[] pszBuf;
}


//读取数据
CGeoScene* CGeoDXFFile::ReadFile(const std::string& strFile)
{
    if (strFile.empty())
        return 0;

    std::ifstream inFile(strFile.c_str());
    if (!inFile)
        return 0;       //文件不存在

    CGeoScene* pScene = new CGeoScene;

    CGeoDXFFile::Code_Section section;
    std::string  strFirst,strSecond;
//    bool bNewSection = true;

    bool bStartSection = true;

    while (getline(inFile, strFirst))
    {
        if (!getline(inFile, strSecond))
            break;  //fail 

        if (bStartSection)
        {
            section.allVal.clear();
            section.clear();
            bStartSection = false;
        }

        if (!ReadDxfSection(section, strFirst, strSecond))
            continue;  //uncompleted

        bStartSection = true;   //ready to read next section

        if (section.strName == "HEADER")
        {
            PhaseHeadData(section.allVal);
            continue;
        }

        if (section.strName == "CLASSES")
        {
            PhaseClassData(section.allVal);
            continue;
        }

        if (section.strName == "TABLES")
        {
            PhaseTableData(section.allVal);
            continue;
        }

        if (section.strName == "BLOCKS")
        {
            PhaseBlockData(section.allVal);
            continue;
        }

        if (section.strName == "ENTITIES")  //phase data
        {
            PhaseEntityData(section.allVal);
            continue;
        }

        if (section.strName == "OBJECTS")
        {
            PhaseObjectData(section.allVal);
            continue;
        }

        if (section.strName == "THUMBNAILIMAGE")
        {
            PhaseThumbData(section.allVal);
        }
    }

    inFile.close();

    //写入时间和文件名
    time_t tCreate, tModify, tAccess;;
    if (GetTimeOfFile(strFile.c_str(), &tCreate, &tModify, &tAccess))
    {
        pScene->SetSysTime(tCreate, tModify, tAccess);

        unsigned nIndex = strFile.rfind('/');
        if (std::string::npos == nIndex)
            nIndex = strFile.rfind('\\');
        if (std::string::npos == nIndex)
            pScene->SetSceneName(strFile);
        else
            pScene->SetSceneName(strFile.substr(nIndex + 1));
    }

    //分析各层数据
    TransferLayerToScene(pScene);
    return pScene;
}

bool CGeoDXFFile::WriteFile(const std::string& /*strFile*/, const CGeoScene* /*lpData*/)
{
    return false;
}

//添加层过滤器
//返回
int CGeoDXFFile::AddLayerFilter(const std::string& strFilter,const std::string& strLayer)
{
    std::string strTemp = strFilter;
    AnsiToUtf8(strTemp);
    m_mapLayerFilter.insert(std::pair<std::string,std::string>(strTemp,strLayer));
    return m_mapLayerFilter.size();
}

//读取dxf文件中的一个段,每个段的数据都保存
bool CGeoDXFFile::ReadDxfSection(Code_Section& section, const std::string& strFirst, const std::string& strSecond)
{
    int nNewGroupID = atoi(strFirst.c_str()); //还应该检查文本串都是全都是数字，这里我就不检查了
    //读取节的标志符号
    if (!section.bInit)
    {
        if (0 == nNewGroupID && strSecond=="SECTION")   //表示段开始
        {
            section.bInit = true;
        }
    }
    else
    {
        if(0 == nNewGroupID && strSecond == "ENDSEC")
            return true;  //表示段结束

        if (2 == nNewGroupID && section.strName.empty())
        {
            section.strName = strSecond;
            return false;  //这个段的名字
        }

        section.allVal.push_back(CGeoDXFFile::Code_Val(nNewGroupID, strSecond)); 
    }
    return false;
}

//分析DXF文件的头信息
void CGeoDXFFile::PhaseHeadData(std::vector<Code_Val>& allVal,CGeoLayer* /*pLayer*/)
{
    //分析数据
    for(unsigned i=0; i < allVal.size(); i++)
    {
        if(allVal[i].nCode == 9)  //版本变量名
        {
            if(allVal[i].strVar == "$ACADVER" && allVal.size() > i+ 2)  //版本变量
            {
                int nTmp = atoi(allVal[++i].strVar.substr(2).c_str());
                switch(nTmp)
                {
                case    1006:
                    m_headDXF.wVer = 10;
                    break;

                case    1009:
                    m_headDXF.wVer = 12;
                    break;

                case    1012:
                    m_headDXF.wVer = 13;
                    break;

                case    1014:
                    m_headDXF.wVer = 14;
                break;

                case    1015:
                    m_headDXF.wVer = 2000;
                break;

                case    1018:
                    m_headDXF.wVer = 2004;
                break;

                case    1021:
                    m_headDXF.wVer = 2007;
                break;

                case    1024:
                    m_headDXF.wVer = 2010;
                break;

                default:
                    m_headDXF.wVer = 2011;
                    break;
                }

                continue;
            }

            //基线角度
            if(allVal[i].strVar == "$ANGBASE" && allVal.size() > i+ 2) 
            {
                m_headDXF.dbBaseAngle = atof(allVal[++i].strVar.c_str());
                continue;
            }

            //角度正方向
            if(allVal[i].strVar == "$ANGDIR" && allVal.size() > i+ 2) 
            {
                m_headDXF.bCCWAngle = atoi(allVal[++i].strVar.c_str()) == 0;
                continue;
            }

            //图形范围右上角（在 WCS 中）
            if(allVal[i].strVar == "$EXTMAX" && allVal.size() > i+ 4) 
            {
                m_headDXF.dbExtX2 = atof(allVal[++i].strVar.c_str());
                m_headDXF.dbExtY2 = atof(allVal[++i].strVar.c_str());
                m_headDXF.dbExtZ2 = atof(allVal[++i].strVar.c_str());
                continue;
            }

            //图形范围左下角（在 WCS 中）
            if(allVal[i].strVar == "$EXTMIN" && allVal.size() > i+ 4) 
            {
                m_headDXF.dbExtX1 = atof(allVal[++i].strVar.c_str());
                m_headDXF.dbExtY1 = atof(allVal[++i].strVar.c_str());
                m_headDXF.dbExtZ1 = atof(allVal[++i].strVar.c_str());
                continue;
            }

            //图形界限右上角（在 WCS 中）
            if(allVal[i].strVar == "$LIMMAX" && allVal.size() > i+ 3) 
            {
                m_headDXF.dbLimitX2 = atof(allVal[++i].strVar.c_str());
                m_headDXF.dbLimitY2 = atof(allVal[++i].strVar.c_str());
                continue;
            }

            //图形图形界限左下角（在 WCS 中） 
            if(allVal[i].strVar == "$LIMMIN" && allVal.size() > i+ 3) 
            {
                m_headDXF.dbLimitX1 = atof(allVal[++i].strVar.c_str());
                m_headDXF.dbLimitY1 = atof(allVal[++i].strVar.c_str());
                continue;
            }
            
        }
    }
}

void CGeoDXFFile::PhaseClassData(std::vector<Code_Val>& /*allVal*/,CGeoLayer* /*pLayer*/)
{
}
void CGeoDXFFile::PhaseTableData(std::vector<Code_Val>& /*allVal*/,CGeoLayer* /*pLayer*/)
{
}
void CGeoDXFFile::PhaseBlockData(std::vector<Code_Val>& /*allVal*/,CGeoLayer* /*pLayer*/)
{
}
void CGeoDXFFile::PhaseEntityData(std::vector<Code_Val>& allVal,CGeoLayer* /*pLayer*/)
{
    enumDXFObjType type;
    ref_ptr<CGeoBase> pObj;
    bool bContinue = false;
    for(unsigned i=0; i < allVal.size(); i++)
    {
        if(0 == allVal[i].nCode)
        {
            if(allVal[i].strVar == "TEXT")
            {
                type = eDXFText;
                pObj = new CGeoText;
                bContinue = true;
                continue;
            }
           /* 
            if(allVal[i].strVar == "MTEXT")
            {
                continue;
            }*/
		
			if(allVal[i].strVar == "LWPOLYLINE")
			{
				type = eDXFLWPolyline;
				pObj = new CGeoLWPolyline;				
                bContinue = true;
				continue;
			}
            if(allVal[i].strVar == "ARC")
            {
                type = eDXFArc;
                pObj = new CGeoArc;
                bContinue = true;
                continue;
            }

            if(allVal[i].strVar == "CIRCLE")
            {
                type = eDXFCircle;
                pObj = new CGeoCircle;
                bContinue = true;
                continue;
            }

            if(allVal[i].strVar == "ELLIPSE")
            {
                type = eDXFEllipse;
                pObj = new CGeoDxfEllipse;
                bContinue = true;
                continue;
            }

            if(allVal[i].strVar == "LINE")
            {
                type = eDXFLine;
                pObj = new CGeoLine;
                bContinue = true;
                continue;
            }

            

            if(allVal[i].strVar == "POINT")
            {
                type = eDXFPoint;
                pObj = new CGeoPoint;
		        bContinue = true;
                continue;
            }

   /*         
            if(allVal[i].strVar == "POLYLINE")
            {
                continue;
            }

            if(allVal[i].strVar == "SPLINE")
            {
                continue;
            }
            if(allVal[i].strVar == "DIMENSION" || allVal[i].strVar == "LARGE_RADIAL_DIMENSION" || allVal[i].strVar == "ARC_DIMENSION")
            {
                type = eDXFDimension;
                pObj = new CGeoDimension;                
                
                bContinue = true;
                continue; 
            }*/

            type = eDXFNull;
            
        }

        if(!pObj.valid() || !bContinue || eDXFNull == type)   //如果无效
            continue;

        bContinue = PhaseObj(pObj.get(), type, allVal[i]);             
    }
}

void CGeoDXFFile::PhaseObjectData(std::vector<Code_Val>& /*allVal*/,CGeoLayer* /*pLayer*/)
{
}
void CGeoDXFFile::PhaseThumbData(std::vector<Code_Val>& /*allVal*/,CGeoLayer* /*pLayer*/)
{
}
//分析标注信息
//bool CGeoDXFFile::PhaseDimensionGeo(CGeoBase *pObj,Code_Val& val)
//{
//    if (1 == val.nCode) //标注文字
//    {
//        Utf8ToAnsi(val.strVar);
//        pObj->m_strObjAnnot = val.strVar;
//        return true;
//
//        //PhaseDimText(val.strVar, m_mapReactorToGeo[dwID].get());
//        //return false;  //表示注释到此为止
//    }
//
//    if(42 == val.nCode)  //真实标注
//    {
//        pObj->GetPt(0)->dx = atof(val.strVar.c_str());
//        return true;
//    }
//
//    if(100 == val.nCode)    //标注样式
//    {
//        CGeoProRail* pTmp = (CGeoProRail*)(pObj->GetPro(0));
//        if(val.strVar == "AcDbAlignedDimension")
//        {
//            pTmp->SetInt(CGeoDimension::eDimAligned);
//        }
//        else
//        {
//            if(val.strVar == "AcDbRadialDimension")
//            {
//                pTmp->SetInt(CGeoDimension::eDimRadial);
//            }
//            else
//                if(val.strVar == "AcDbDiametricDimension")
//                    pTmp->SetInt(CGeoDimension::eDimDiametric);
//        }
//        return true;
//    }
//
//    int nIndex = val.nCode - 9;
//    if(nIndex < 1 || nIndex > 7)
//    {
//        nIndex = val.nCode - 19;
//    }
//
//    CGeoPt* pt=0;
//    if(nIndex >= 1 && nIndex <= 7)
//    {
//        pt = pObj->GetPt(nIndex);
//    }
//
//    if(pt != 0)
//    {
//        if(val.nCode < 20)
//            pt->dx = atof(val.strVar.c_str());
//        else
//            pt->dy = atof(val.strVar.c_str());
//    }
//    return true;
//}


bool CGeoDXFFile::PhaseObj(CGeoBase *pObj,enumDXFObjType type, Code_Val& val)
{
    if(8 == val.nCode) //对不是注释的则添加到层中
    {
        CGeoLayer* pTmp;
        std::map<std::string, ref_ptr<CGeoLayer> >::iterator pos = m_mapNameToLayer.find(val.strVar);
        if(pos != m_mapNameToLayer.end())
        {
            pTmp = pos->second.get();
        }
        else
        {
            pTmp = new CGeoLayer;
            pTmp->SetName(val.strVar);
            m_mapNameToLayer.insert(std::pair<std::string, ref_ptr<CGeoLayer> >(val.strVar, pTmp));
        }
        pTmp->AddObj(pObj);
        return true;
    }

    if(102 == val.nCode || 330 == val.nCode/* && m_bReactors*/ || 5 == val.nCode)
        return true;

    //if(5 == val.nCode && pObj->m_tidGeoID == 0)  //表示编号
    //{
    //    std::istringstream sIn(val.strVar);
    //    DWORD dwID;
    //    sIn>>std::hex>>dwID;
    //    pObj->m_tidGeoID = dwID;
    //    return true;
    //}


    //由于标注全部采用半径标注，所有的几何体标注，采用圆与之相交，然后再圆上标注，除了几何体本身是圆或者圆弧外
    //所以也就不需要下面的两个数据
    //if(102 == val.nCode)    //表示属性编辑器
    //{
    //    if(val.strVar == "{ACAD_REACTORS")
    //    {
    //        m_nDimID = 0;
    //        m_bReactors = true;
    //    }
    //    else
    //    {
    //        if(m_bReactors && val.strVar == "}")
    //        {
    //            m_bReactors = false;

    //            if(m_nDimID == 1 && type != eDXFDimension)   //表示只有唯一的一个结点
    //            {
    //                m_mapTmpReactorToGeo.erase(m_dwLastDim);
    //                m_mapReactorToGeo.insert(std::pair<DWORD, ref_ptr<CGeoBase>>(m_dwLastDim,pObj));
    //            }
    //            m_nDimID = 0;
    //        }
    //    }
    //    return true;
    //}

    // if(330 == val.nCode && m_bReactors)    //表示需要提取关键字
    //{
    //    std::istringstream sIn(val.strVar);
    //    DWORD dwID;
    //    sIn>>std::hex>>dwID;
    //    if(type != eDXFDimension)
    //    {
    //        m_mapTmpReactorToGeo.insert(std::pair<DWORD, ref_ptr<CGeoBase>>(dwID,pObj));
    //        m_dwLastDim = dwID;
    //        m_nDimID++;
    //    }
    //    else
    //    {
    //        pObj->m_tidGeoID = dwID;    //如果是注释则，保存ID
    //    }
    //    return true;
    //}

    switch(type)
    {
    /*case    eDXFDimension:
        return PhaseDimensionGeo(pObj,val);*/

    case    eDXFText:
        {
            CGeoText* temp = static_cast<CGeoText*>(pObj);

            switch(val.nCode)
            {
            case    1:  //文字
                Utf8ToAnsi(val.strVar);
                temp->SetText(val.strVar);
                break;

            case    10: //x
            case    20: //y
            {
                CGeoPt pt = temp->GetStartPt();
                if (10 == val.nCode)
                    pt.dx = atof(val.strVar.c_str());
                else
                    pt.dy = atof(val.strVar.c_str());
                temp->SetStartPt(pt);
            }
                break;

            case    40: //文字高度
                temp->SetHeight(atof(val.strVar.c_str()));
                break;

            default:
                break;
            }
            
        }
        break;

    case    eDXFPoint:
        if(10 == val.nCode || 20 == val.nCode)
        {
            CGeoPoint* temp = static_cast<CGeoPoint*>(pObj);
            double x, y;
            temp->GetPt(x,y);
            
            if (10 == val.nCode)
                x = atof(val.strVar.c_str());
            else
                y = atof(val.strVar.c_str());
            temp->SetPt(x, y);
        }
        break;

    case  eDXFLine:
        {
            CGeoPt pt;
            CGeoLine* temp = static_cast<CGeoLine*>(pObj);
            int nFlag = 0;
            if(10 == val.nCode || 20 == val.nCode)
            {
                pt = temp->GetFirstPt();
                nFlag = 1;
            }

            if(11 == val.nCode || 21 == val.nCode)
            {
                pt = temp->GetSecondPt();
                nFlag=2;
            }

            switch(val.nCode)
            {
            case    10: //x
            case    11:
                pt.dx = atof(val.strVar.c_str());
                break;

            case    20: //y
            case    21:
                pt.dy = atof(val.strVar.c_str());
                break;
            }
            if (1 == nFlag)
                temp->SetFirstPt(pt);
            else if (2 == nFlag)
                temp->SetSecondPt(pt);
        }
        break;

    case eDXFArc:
	    {
            CGeoPt pt;
            CGeoArc* temp = static_cast<CGeoArc*>(pObj);

            switch(val.nCode)
            {
            case    10: //x
            case    20: //y
                pt = temp->GetCenterPt();
                if (10 == val.nCode)
                {
                    pt.dx = atof(val.strVar.c_str());
                }
                else
                {
                    pt.dy= atof(val.strVar.c_str());
                }
                temp->SetCenterPt(pt);
                break;

            case    40:
                temp->SetRadius(atof(val.strVar.c_str()));
                break;
            case    50:
                temp->SetAngleS(atof(val.strVar.c_str()));
                break;         
            case    51:
                temp->SetAngleE(atof(val.strVar.c_str()));
                break;
            }

	    }//end case eDXFArc
        break;

    case eDXFCircle:
	    {
             CGeoPt pt;
             CGeoCircle* temp = static_cast<CGeoCircle*>(pObj);
            switch(val.nCode)
            {
            case    10: //x
            case    20: //y
                pt = temp->GetCenterPt();
                if (10 == val.nCode)
                {
                    pt.dx = atof(val.strVar.c_str());
                }
                else
                {
                    pt.dy = atof(val.strVar.c_str());
                }
                temp->SetCenterPt(pt);
                break;

            case    40:
                temp->SetRadius(atof(val.strVar.c_str()));
                break;
            }
	    }//end case eDXFCircle
        break;

    case    eDXFLWPolyline:
    {
        CGeoLWPolyline* temp = static_cast<CGeoLWPolyline*>(pObj);
        if (10 == val.nCode)
        {
            temp->AddPt(atof(val.strVar.c_str()),0);
        }
        else if (20 == val.nCode)
        {
            int nIndex = temp->GetPtNum();
            if (nIndex > 0)
            {
                nIndex--;
                CGeoPt pt = temp->GetPt(nIndex);
                pt.dy = atof(val.strVar.c_str());
                temp->UpdatePt(nIndex,pt);
            }
        }
        else if (70 == val.nCode)
        {
            if (1 == atoi(val.strVar.c_str())) //表示封闭，则起始点和终止点必须重新组成一个对象
            {
                temp->SetClose(true);
            }
            else
                temp->SetClose(false);
        }
    }
        break;

    case eDXFEllipse:
        {
            CGeoPt* pt=0;
            CGeoDxfEllipse* temp = static_cast<CGeoDxfEllipse*>(pObj);

            switch (val.nCode)
            {
            case    10:
            case    20:
                pt = &(temp->GetCenterPt());
                break;

            case    11:
            case    21:
                pt = &(temp->GetAxisPt());
                break;

            case    41:
            case    42:
                pt = &(temp->GetAngle());
                break;

            }
            
            switch (val.nCode)
            {
            case    10: //中心点
            case    11: //端点
            case    41://角度
                pt->dx = atof(val.strVar.c_str());
                break;

            case    20: //y
            case    21:
            case    42: //终点参数
                pt->dy = atof(val.strVar.c_str());
                break;

            case    40: //短轴与长轴的比例
                temp->SetEllipticity(atof(val.strVar.c_str()));
                break;
            }
        }
        break;
    default:
        break;
    }

    return true;
}

//剔除无用的节点，整合成线路
void CGeoDXFFile::TransferLayerToScene(CGeoScene* pScene)
{
    std::map<std::string, ref_ptr<CGeoLayer> >::iterator pos = m_mapNameToLayer.begin();
    unsigned short nLayer = 1; //手动设置层的编号从1开始
    
    while(pos != m_mapNameToLayer.end())
    {
        std::string strName = pos->first;
        Utf8ToAnsi(strName);
        CGeoLayer* lpCurLayer = new CGeoLayer;
        lpCurLayer->SetName(strName);
        lpCurLayer->SetLayerNo(nLayer);
        pScene->AddLayer(lpCurLayer);        

        CGeoGDSStruct* pCell = new CGeoGDSStruct;
        //遍历所有层的元素，转换所有元素到cell中
        GeoObjList& objList = pos->second.get()->GetObjList();
        for (unsigned i = 0;i < objList.size();i++)
        {
            CGeoGDSElement* pElement = ConvertPrimitiveToElement(objList.at(i).get());
            pCell->AddElement(pElement);
            lpCurLayer->AddObj(pElement);
            pElement->SetLayerNo(nLayer);
        }
        time_t tCreate, tModify, tAccess;
        pScene->GetSysTime(tCreate, tModify, tAccess);
        pCell->SetCreateTime(tCreate);
        pCell->SetLastModifyTime(tModify);
        pCell->AddLayerNO(nLayer);

        std::ostringstream strOf;
        strOf << "DXF_CELL_" << nLayer;
        pCell->SetName(strOf.str());
        pScene->AddCell(pCell);

        nLayer++; //层号加一
        pos++; //下一层
    }    
}

CGeoGDSElement* CGeoDXFFile::ConvertPrimitiveToElement(CGeoBase *pObj)
{
    CGeoGDSElement* pElement = 0;
    unsigned short wType = pObj->GetObjType();
    FLOAT_T x, y;
    if (GEO_TYPE_CIRCLE == wType || GEO_TYPE_ARC == wType)
    {
        CGeoCircle* pTemp = static_cast<CGeoCircle*>(pObj);
        
        pTemp->GetCenterPt(x,y);
        pTemp->SetCenterPt(0,0);
        
        pElement = new CGeoGDSOther;
        CMatrix mat;
        mat.MakeTranslate(x, y);
        pElement->SetTransformMatrix(mat);
        pElement->SetChild(pObj);
    }
    else if (GEO_TYPE_ELLIPSE_DXF == wType)
    {
        CGeoDxfEllipse* pTemp = static_cast<CGeoDxfEllipse*>(pObj);
        CGeoPt& ptCent = pTemp->GetCenterPt();
        CGeoPt& ptAxis = pTemp->GetAxisPt();
        FLOAT_T dbLen = sqrt(ptAxis.dx * ptAxis.dx + ptAxis.dy *ptAxis.dy);

        CGeoEllipse* pEllipse = new CGeoEllipse;
        pEllipse->SetWidth(dbLen);
        pEllipse->SetHeight(dbLen*pTemp->GetEllipticity());

        CMatrix mat1;
        mat1.MakeTranslate(ptCent.dx, ptCent.dy);
        CMatrix mat2;
        mat2.MakeRotate(ptAxis.dx / dbLen, ptAxis.dy / dbLen);

        pElement = new CGeoGDSOther;        
        pElement->SetTransformMatrix(mat1*mat2);        
        pElement->SetChild(pEllipse);
    }
    else if (GEO_TYPE_LWPOLYLINE == wType)
    {
        CGeoLWPolyline* pTemp = static_cast<CGeoLWPolyline*>(pObj);
        if (pTemp->IsClosed())
        {
            pElement = new CGeoGDSBoundary;
        }
        else
        {
            pElement = new CGeoGDSPath;
        }
        pElement->SetChild(pTemp);
    }
    else if (GEO_TYPE_TEXT == wType)
    {
        pElement = new CGeoGDSText;
        pElement->SetChild(pObj);
    }
    else if (GEO_TYPE_LINE == wType)
    {
        CGeoLine* pTemp = static_cast<CGeoLine*>(pObj);
        pElement = new CGeoGDSPath;

        CGeoPt first, second;
        pTemp->GetLinePt(first, second);
        pElement->AddCoordPt(first.dx, first.dy);
        pElement->AddCoordPt(second.dx, second.dy);
    }
    else
    {
        pElement  = new CGeoGDSOther;
        pElement->SetChild(pObj);
    }
    return pElement;
}