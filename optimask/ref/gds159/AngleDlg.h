#if !defined(AFX_ANGLEDLG_H__3C3A9C31_5DE1_436F_9115_3CA1DEC1E136__INCLUDED_)
#define AFX_ANGLEDLG_H__3C3A9C31_5DE1_436F_9115_3CA1DEC1E136__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAngleDlg : public CDialog
{
public:
	double GetAngle();
	void SetAngle(double angle);
	CAngleDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(CAngleDlg)
	enum { IDD = IDD_ANGLE_DLG };
	CEdit	m_editAngle;
	double	m_dblAngle;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CAngleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CAngleDlg)
	afx_msg void OnDeltaposUpdownAngle(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeViewAngle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_ANGLEDLG_H__3C3A9C31_5DE1_436F_9115_3CA1DEC1E136__INCLUDED_)
