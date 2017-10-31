#if !defined(AFX_SELVIEWSETTING_H__519D6588_6253_41BB_B29E_80452745EB56__INCLUDED_)
#define AFX_SELVIEWSETTING_H__519D6588_6253_41BB_B29E_80452745EB56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GdsDoc.h"
#include "GdsView.h"

class CSelViewSetting : public CDialog
{
private:
	CGdsView* m_pCaller;
	BOOL* m_pEnabler;
public:
	CSelViewSetting(CWnd* pParent = NULL);
	void ExitDialog(){OnCancel();}
	void ChangeSelection(int nth, BOOL select = FALSE);
	void OnCancel()
	{
		*m_pEnabler = FALSE;
		CDialog::OnCancel();
	}
	void OnOK()
	{
		OnSelectView();
	}
	void FillParam();
	void SetEnabler(BOOL *pEnabler)
	{
		m_pEnabler = pEnabler;
	}
	void SetCaller(CGdsView* pView)
	{
		m_pCaller = pView;
	}
	void ScrollList();
	void AddParam();


	//{{AFX_DATA(CSelViewSetting)
	enum { IDD = IDD_VIEW_SELECT };
	CScrollBar	m_scrList;
	CListBox	m_lbViewListY;
	CListBox	m_lbViewListX;
	CListBox	m_lbViewList;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CSelViewSetting)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CSelViewSetting)
	afx_msg void OnDeleteView();
	afx_msg void OnSelectView();
	afx_msg void OnEditView();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelchangeViewList();
	afx_msg void OnSelchangeViewListX();
	afx_msg void OnSelchangeViewListY();
	afx_msg void OnDblclkViewList();
	afx_msg void OnDblclkViewListX();
	afx_msg void OnDblclkViewListY();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnClearViews();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_SELVIEWSETTING_H__519D6588_6253_41BB_B29E_80452745EB56__INCLUDED_)
