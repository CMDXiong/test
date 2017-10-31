/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GDSDOC_H__CFCC480E_D0C4_4C50_B163_E58CA4DE96AB__INCLUDED_)
#define AFX_GDSDOC_H__CFCC480E_D0C4_4C50_B163_E58CA4DE96AB__INCLUDED_

#include "GDShcy.h"	

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLayerSet : public CObject
{
public:
	BOOL		m_bShow;
	int			m_bValid;
	int			m_intOrder;
	int			m_intNumber;
	CString		m_strName;
	int			m_intFill;
	COLORREF	m_color;

	CLayerSet()
	{
		m_bValid = FALSE;
		m_intOrder = -1;
		m_intNumber = -1;
		m_bShow = FALSE;
		m_strName = "";
		m_intFill = 0;
		m_color = 0;
	}
	CLayerSet& operator=(CLayerSet& arg)
	{
		m_intOrder = arg.m_intOrder;
		m_intNumber = arg.m_intNumber;
		m_bShow = arg.m_bShow;
		m_strName = arg.m_strName;
		m_intFill = arg.m_intFill;
		m_color = arg.m_color;
		m_bValid = arg.m_bValid;
		return *this;
	}
};

class CGdsDoc : public CDocument
{
protected:
	CGdsDoc();
	DECLARE_DYNCREATE(CGdsDoc)

// Attribute
public:

public:
	//{{AFX_VIRTUAL(CGdsDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void SetTitle(LPCTSTR lpszTitle);
	//}}AFX_VIRTUAL

public:
	CGDS* GetGDS(){return &m_gdsGDS;}
	CString GetTecFilePath(){return m_strTecFile;}
	void SetTecFilePath(CString path)
	{
		m_strTecFile = path;
		SetTitle(NULL);
	}

	virtual ~CGdsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

protected:
	//{{AFX_MSG(CGdsDoc)
	afx_msg void OnUpdateFileOpenSameTec(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private :
	CGDS       m_gdsGDS;
	CString    m_strLibTitle;
	CString    m_strDocTitle;
	CString    m_strTecFile;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_GDSDOC_H__CFCC480E_D0C4_4C50_B163_E58CA4DE96AB__INCLUDED_)
