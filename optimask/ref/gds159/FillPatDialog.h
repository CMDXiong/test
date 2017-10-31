#if !defined(AFX_FILLPATDIALOG_H__62145061_986B_11D5_A708_009027DFFDCF__INCLUDED_)
#define AFX_FILLPATDIALOG_H__62145061_986B_11D5_A708_009027DFFDCF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////

class CFillPatDialog : public CDialog
{
public:
	void SetBkColor(COLORREF bkcolor);
	void SetLayerColor(COLORREF color);
	void DrawButton(CButton* button, CBitmap* bmp);
	void DrawButtons();
	int m_intPattern;
	int GetPattern();
	CFillPatDialog(CWnd* pParent = NULL);

	//{{AFX_DATA(CFillPatDialog)
	enum { IDD = IDD_FILL_PAT_DLG };
	CButton	m_buttonDia;
	CButton	m_buttonRDiagDash;
	CButton	m_buttonFDiagDash;
	CButton	m_buttonLight;
	CButton	m_buttonVDash;
	CButton	m_buttonVwave;
	CButton	m_buttonHwave;
	CButton	m_buttonHDash;
	CButton	m_buttonVZigzag;
	CButton	m_buttonHZigzag;
	CButton	m_buttonXDiag;
	CButton	m_buttonVHatch;
	CButton	m_buttonSolid;
	CButton	m_buttonRDiag;
	CButton	m_buttonHHatch;
	CButton	m_buttonFrame;
	CButton	m_buttonFDiag;
	CButton	m_buttonXHatch;
	BOOL	m_bShowColor;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CFillPatDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CFillPatDialog)
	afx_msg void OnChatch();
	afx_msg void OnFdiag();
	afx_msg void OnFrame();
	afx_msg void OnHhatch();
	afx_msg void OnRdiag();
	afx_msg void OnSolid();
	afx_msg void OnVhatch();
	afx_msg void OnXdiag();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHZigzag();
	afx_msg void OnVZigzag();
	afx_msg void OnHDash();
	afx_msg void OnVwave();
	afx_msg void OnHwave();
	afx_msg void OnVdash();
	afx_msg void OnLight();
	afx_msg void OnRdiagDash();
	afx_msg void OnFdiagDash();
	afx_msg void OnDia();
	afx_msg void OnCheckUseColor();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	COLORREF m_colorLayerColor;
	COLORREF m_colorLayerBk;
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_FILLPATDIALOG_H__62145061_986B_11D5_A708_009027DFFDCF__INCLUDED_)
