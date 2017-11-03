//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GDS2MULTIDOCTEMPLATE_H__7A5928C9_23B6_4D7C_9C35_CAE8401D17DF__INCLUDED_)
#define AFX_GDS2MULTIDOCTEMPLATE_H__7A5928C9_23B6_4D7C_9C35_CAE8401D17DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGDS2MultiDocTemplate : public CMultiDocTemplate  
{
public:
	CGDS2MultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
        CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass ) :
        CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
		{}
	virtual ~CGDS2MultiDocTemplate();
	BOOL GetDocString(CString& rString, enum DocStringIndex i) const;
	CDocTemplate::Confidence MatchDocType(const char* pszPathName, CDocument*& rpDocMatch);
};

#endif // !defined(AFX_GDS2MULTIDOCTEMPLATE_H__7A5928C9_23B6_4D7C_9C35_CAE8401D17DF__INCLUDED_)
