#if !defined(AFX_SHRINKDLG_H__F91FF2C5_87B4_11D5_A700_009027DFFDCF__INCLUDED_)
#define AFX_SHRINKDLG_H__F91FF2C5_87B4_11D5_A700_009027DFFDCF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////

class CShrinkDlg : public CDialog
{
public:
	void SetShrinkRate(double rate);
	double GetShrinkRate();
	CShrinkDlg(CWnd* pParent = NULL); 

	//{{AFX_DATA(CShrinkDlg)
	enum { IDD = IDD_SHRINK_DLG };
	int		m_intShrinkRate;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CShrinkDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CShrinkDlg)
	afx_msg void OnDeltaposUpdownRate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeShrinkRate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_SHRINKDLG_H__F91FF2C5_87B4_11D5_A700_009027DFFDCF__INCLUDED_)
