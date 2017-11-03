#if !defined(AFX_GRIDDLG_H__04E64580_3158_42F6_AC7E_CA9175C29C9A__INCLUDED_)
#define AFX_GRIDDLG_H__04E64580_3158_42F6_AC7E_CA9175C29C9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include "GdsDoc.h"
#include "GdsView.h"

class CGridDlg : public CDialog
{
public:
	void SetGridType(int type);
	void SetDirection(int dir);
	virtual void OnOK();
	virtual void OnCancel();

	void ValidateSettings();
	void OnMouseLeave();
	void ExitMeasure();
	void SetEnabler(BOOL* enabler){m_pEnabler = enabler;}
	CGridDlg(CWnd* pParent);

	//{{AFX_DATA(CGridDlg)
	enum { IDD = IDD_GRID_DLG };
	int		m_intRadioDirection;
	int		m_intMajorX;
	int		m_intMajorY;
	double	m_dblMinorX;
	double	m_dblMinorY;
	double	m_dblSnapX;
	double	m_dblSnapY;
	CString	m_strGridSize;
	int		m_intRadioStyle;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CGridDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CGridDlg)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCheckGrid();
	afx_msg void OnChangeEditDiv();
	afx_msg void OnKillfocusEditSnapX();
	afx_msg void OnKillfocusEditSnapY();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnChangeEditSnapX();
	afx_msg void OnChangeEditSnapY();
	afx_msg void OnRadioXy();
	afx_msg void OnRadioFree();
	afx_msg void OnRadioDiagonal();
	afx_msg void OnButtonClearScale();
	afx_msg void OnChangeEditMajorX();
	afx_msg void OnKillfocusEditMajorX();
	afx_msg void OnChangeEditMajorY();
	afx_msg void OnKillfocusEditMajorY();
	afx_msg void OnChangeEditMinorX();
	afx_msg void OnKillfocusEditMinorX();
	afx_msg void OnChangeEditMinorY();
	afx_msg void OnKillfocusEditMinorY();
	afx_msg void OnRadioGridDot();
	afx_msg void OnRadioGridLine();
	afx_msg void OnRadioGridNone();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL   *m_pEnabler;
	CWnd*  m_pParent;
	BOOL   m_bMouseInView;
};

//{{AFX_INSERT_LOCATION}}


#endif // !defined(AFX_GRIDDLG_H__04E64580_3158_42F6_AC7E_CA9175C29C9A__INCLUDED_)
