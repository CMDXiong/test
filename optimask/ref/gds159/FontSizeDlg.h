#if !defined(AFX_FONTSIZEDLG_H__00D8803B_4514_44CD_BC87_1C75EECCFB3A__INCLUDED_)
#define AFX_FONTSIZEDLG_H__00D8803B_4514_44CD_BC87_1C75EECCFB3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFontSizeDlg : public CDialog
{
	CDC* pDC;
	CString m_strCurrent;
	int m_intFontStyle;
public:
	void SetDC(CDC* dc){pDC = dc;}
	double GetFontSize();
	int GetFontStyle(){return m_intFontStyle;}
	void SetFontStyle(int style){m_intFontStyle = style;}
	void SetFontSize(double size);
	CFontSizeDlg(CWnd* pParent = NULL);
	void SetCurrentFont(CString current);
	CString GetFontName(){return m_strCurrent;}

	//{{AFX_DATA(CFontSizeDlg)
	enum { IDD = IDD_FONTSIZE_DLG };
	CListBox	m_lbFonts;
	double	m_dblFontSize;
	BOOL	m_bItalic;
	BOOL	m_bBold;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CFontSizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CFontSizeDlg)
	afx_msg void OnDeltaposUpdownRate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeFontSize();
	afx_msg void OnSelchangeListFonts();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCheckBold();
	afx_msg void OnCheckItalic();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_FONTSIZEDLG_H__00D8803B_4514_44CD_BC87_1C75EECCFB3A__INCLUDED_)
