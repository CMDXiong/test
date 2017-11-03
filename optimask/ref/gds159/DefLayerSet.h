#if !defined(AFX_DEFLAYERSET_H__2CB63FCF_CD38_4272_A12B_587544BFA601__INCLUDED_)
#define AFX_DEFLAYERSET_H__2CB63FCF_CD38_4272_A12B_587544BFA601__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////

class CDefLayerSet : public CDialog
{
public:
	CDefLayerSet(CWnd* pParent = NULL);
	CString GetFileName(){return m_strFile;}
	void SetFileName(CString name){m_strFile = name;}

	//{{AFX_DATA(CDefLayerSet)
	enum { IDD = IDD_DEFAULT_TEC_DLG };
	CString	m_strFile;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CDefLayerSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CDefLayerSet)
	afx_msg void OnButtonClear();
	afx_msg void OnButtonBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DEFLAYERSET_H__2CB63FCF_CD38_4272_A12B_587544BFA601__INCLUDED_)
