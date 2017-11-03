/* ========================================================================== */
/* GeoAttribute.CPP -- 基本图形定义
*
* REFERENCE:
*
* COPYRIGHT 2017 Optixera.
* -------------------------------------------------------------------------- */
/* $REVISION: 1.00$ (20171001 Chaohong Liu) */
/* HISTORY: 20171001(v1.00) Initial Version;
* 该文件为各种属性的操作具体的实现。包括位于Geo.h件中的CGeoProperty类的实现
* ========================================================================== */

#include"GeoAttribute.h"

bool CGeoProperty::AddProValue(CProValue* val)
{
    return SetProValue(val, false);
}

bool CGeoProperty::SetProValue(CProValue* val, bool bAdd)
{
    if (val != 0)
    {
        for (unsigned i=0; i<m_vecProValueList.size();i++)
        {
            if (m_vecProValueList[i]->GetProValuID() == val->GetProValuID())
            {
                m_vecProValueList[i] = val;
                return true;
            }
        }
        if (bAdd)
        {
            m_vecProValueList.push_back(val);
            return true;
        }
    }
    return false;
}

CProValue* CGeoProperty::GetProValue(unsigned long dwID)
{
//    for (auto& x : m_vecProValueList)
//    {
//        if (x->GetProValuID() == dwID)
//            return x.get();
//    }
//    return 0;
    for (unsigned i=0; i<m_vecProValueList.size();i++)
    {
        if (m_vecProValueList[i]->GetProValuID() == dwID)
            return m_vecProValueList[i].get();
    }
    return 0;
}

bool CGeoProperty::HasProValur(unsigned long dwID)
{
    return GetProValue(dwID) != 0;
}
