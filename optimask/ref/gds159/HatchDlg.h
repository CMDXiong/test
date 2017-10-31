#if !defined(AFX_HATCHDLG_H__CD95849C_F77A_4E55_8D79_993B7671A579__INCLUDED_)
#define AFX_HATCHDLG_H__CD95849C_F77A_4E55_8D79_993B7671A579__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHatchDlg : public CDialog
{
public:
	void SetOmitStructure(BOOL omit){m_bOmitStructure = omit;}
	BOOL GetOmitStructure(){return m_bOmitStructure;}
	void SetHatchLimit(int lim);
	void SetDrawLimit(int lim);
	int GetHatchLimit();
	int GetDrawLimit();
	CHatchDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(CHatchDlg)
	enum { IDD = IDD_FILL_LIMIT_DLG };
	int		m_intHatchLimit;
	int		m_intDrawLimit;
	BOOL	m_bOmitStructure;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CHatchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CHatchDlg)
	afx_msg void OnDeltaposUpdownHatch(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeHatchLimit();
	afx_msg void OnChangeDrawLimit();
	afx_msg void OnDeltaposUpdownDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckOmitStructure();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_HATCHDLG_H__CD95849C_F77A_4E55_8D79_993B7671A579__INCLUDED_)
