#if !defined(AFX_SETCURROR_H__1391D677_73DD_4D6B_8B39_907EC1595E91__INCLUDED_)
#define AFX_SETCURROR_H__1391D677_73DD_4D6B_8B39_907EC1595E91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CGdsView;

class CSetCursor : public CDialog
{
public:
	void SetCurrentCoord(double x, double y);
	void SetCaller(CGdsView* caller);
	void SetWindowName(CString name);
	void ExitDialog();
	void SetEnabler(BOOL* enabler);
	BOOL* m_pbEnabler;
	CSetCursor(CWnd* pParent = NULL);

	//{{AFX_DATA(CSetCursor)
	enum { IDD = IDD_MOVE_TO };
	CEdit	m_editCoordY;
	CEdit	m_editCoordX;
	double	m_dblCoordX;
	double	m_dblCoordY;
	BOOL	m_bFbCursor;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CSetCursor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnCancel();
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:
	CGdsView* m_pCaller;
	//{{AFX_MSG(CSetCursor)
	afx_msg void OnApply();
	afx_msg void OnChangeCoordX();
	afx_msg void OnChangeCoordY();
	afx_msg void OnFbCursor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_SETCURROR_H__1391D677_73DD_4D6B_8B39_907EC1595E91__INCLUDED_)
