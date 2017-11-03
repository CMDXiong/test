#if !defined(AFX_STRLVLDLG_H__A4087D15_98BA_4669_AA2A_A7D031A1A7A3__INCLUDED_)
#define AFX_STRLVLDLG_H__A4087D15_98BA_4669_AA2A_A7D031A1A7A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////

class CStrLvlDlg : public CDialog
{
public:
	int GetLevel();
	void SetLevel(int level);
	CStrLvlDlg(CWnd* pParent = NULL); 

	//{{AFX_DATA(CStrLvlDlg)
	enum { IDD = IDD_STR_LEVEL_DLG };
	int		m_intLevel;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CStrLvlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CStrLvlDlg)
	afx_msg void OnDeltaposUpdownLevel(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeStrLevel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_STRLVLDLG_H__A4087D15_98BA_4669_AA2A_A7D031A1A7A3__INCLUDED_)
