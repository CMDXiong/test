//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LAYERINFO_H__D51D05BC_27AE_406B_820A_236379287618__INCLUDED_)
#define AFX_LAYERINFO_H__D51D05BC_27AE_406B_820A_236379287618__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GdsDoc.h"

class CLayerInfo  
{
private:
	CString m_strTecFilePath;
public:
	CLayerInfo();
	~CLayerInfo();
	CString GetTecFilePath(){return m_strTecFilePath;}
	void SetTecFilePath(CString path){m_strTecFilePath = path;}
	BOOL SaveLayerSetup(CLayerSet* pSet, CString path = "", BOOL* storeList = NULL);
	BOOL ReadLayerSetup(CLayerSet* pSet, CString path = "");
};

#endif // !defined(AFX_LAYERINFO_H__D51D05BC_27AE_406B_820A_236379287618__INCLUDED_)
