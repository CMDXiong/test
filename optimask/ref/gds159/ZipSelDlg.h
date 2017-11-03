#if !defined(AFX_ZIPSELDLG_H__F07C9CC1_A972_4D97_B8C6_F8A2EEB814D0__INCLUDED_)
#define AFX_ZIPSELDLG_H__F07C9CC1_A972_4D97_B8C6_F8A2EEB814D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
#include "zlib.h"
#include "unzip.h"

class CZipSelDlg : public CDialog
{
	CString namesel;
	int numfiles;
	unzFile zfile;
public:
	CString GetSelected(){return namesel;}
	CZipSelDlg(unzFile file, int numentry, CWnd* pParent = NULL);

	//{{AFX_DATA(CZipSelDlg)
	enum { IDD = IDD_ZIPSEL_DIALOG };
	CButton	m_buttonOK;
	CListBox	m_listFile;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CZipSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CZipSelDlg)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelchangeListFile();
	afx_msg void OnDblclkListFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_ZIPSELDLG_H__F07C9CC1_A972_4D97_B8C6_F8A2EEB814D0__INCLUDED_)
