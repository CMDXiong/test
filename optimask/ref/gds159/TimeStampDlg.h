#if !defined(AFX_TIMESTAMPDLG_H__1AA3EA86_31EA_42E3_AD1A_7790B7C38C8F__INCLUDED_)
#define AFX_TIMESTAMPDLG_H__1AA3EA86_31EA_42E3_AD1A_7790B7C38C8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////

class CTimeStampDlg : public CDialog
{
public:
	void SetDBName(CString name);
	void SetAccessDate(struct tm datetime);
	void SetCreationDate(struct tm datetime);
	CTimeStampDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(CTimeStampDlg)
	enum { IDD = IDD_TIME_STAMP_DLG };
	CString	m_strCreate;
	CString	m_strAccess;
	CString	m_strDBName;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CTimeStampDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CTimeStampDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_TIMESTAMPDLG_H__1AA3EA86_31EA_42E3_AD1A_7790B7C38C8F__INCLUDED_)
