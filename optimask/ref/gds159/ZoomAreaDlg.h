#if !defined(AFX_ZOOMAREADLG_H__18583734_3E15_4EA8_96F7_E0DD439C6B5D__INCLUDED_)
#define AFX_ZOOMAREADLG_H__18583734_3E15_4EA8_96F7_E0DD439C6B5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//

/////////////////////////////////////////////////////////////////////////////

class CZoomAreaDlg : public CDialog
{
public:
	int GetZoomAreaMode();
	void SetZoomAreaMode(int mode);
	void Recalc();
	void GetCoord(double* left, double* top, double* right, double* bottom);
	void SetCurrentArea(double left, double top, double right, double bottom);
	void OnOK();
	BOOL ValidateArea();
	CZoomAreaDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(CZoomAreaDlg)
	enum { IDD = IDD_AREA_DLG };
	CEdit	m_editTop;
	CEdit	m_editRight;
	CEdit	m_editRadius;
	CEdit	m_editLeft;
	CEdit	m_editCenterY;
	CEdit	m_editCenterX;
	CEdit	m_editBottom;
	double	m_dblBottom;
	double	m_dblLeft;
	double	m_dblRight;
	double	m_dblTop;
	double	m_dblCenterX;
	double	m_dblCenterY;
	double	m_dblRadius;
	int		m_intStyle;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CZoomAreaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CZoomAreaDlg)
	afx_msg void OnRadioArea();
	afx_msg void OnRadioCenter();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnKillfocusEditLeft();
	afx_msg void OnKillfocusEditTop();
	afx_msg void OnKillfocusEditBottom();
	afx_msg void OnKillfocusEditCenterX();
	afx_msg void OnKillfocusEditCenterY();
	afx_msg void OnKillfocusEditRadius();
	afx_msg void OnKillfocusEditRight();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

enum {
	ZOOM_AREA_MODE_SIDE,
	ZOOM_AREA_MODE_AREA,
};

//{{AFX_INSERT_LOCATION}}


#endif // !defined(AFX_ZOOMAREADLG_H__18583734_3E15_4EA8_96F7_E0DD439C6B5D__INCLUDED_)
