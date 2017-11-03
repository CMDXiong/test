/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__31DAE01B_8552_4EFF_883F_94D70A1F49A1__INCLUDED_)
#define AFX_MAINFRM_H__31DAE01B_8552_4EFF_883F_94D70A1F49A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Splash.h"

#include <mmsystem.h>

class CGdsView;

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)

	CString		m_strTitleOrg;
	CObArray	m_arrayView;
	BOOL		m_bPreview;
	BOOL		m_bSyncViewAlways;
public:
	CMainFrame();
	void SetSyncView(BOOL val)
	{
		m_bSyncViewAlways = val;
	}
	BOOL GetSyncView()
	{
		return m_bSyncViewAlways;
	}


	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnSetPreviewMode(BOOL bPreview, CPrintPreviewState* pState);
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	//}}AFX_VIRTUAL

public:
	BOOL IsThereMoreView(CGdsView* pView);
	BOOL IsViewActive(CGdsView* pView);
	void SyncViews(CGdsView* pViewNow);
	void OnDropFiles(HDROP hDropInfo, CString tec);
	void LockView(BOOL lock);
	void ActivateToolTip(BOOL activate);
	void KillAbortDrawTimer();
	void CreateAbortDrawTimer();
	void PollStopDraw();
	void HideDummyComboBoxes();
	void ShowDummyComboBoxes();
	void CreateDummyComboBoxes();
	void HideToolBar();
	void ShowToolBar();
	void RedrawToolBar();
	void AddView(CGdsView* pView);
	void RemoveView(CGdsView* pView);
	void ShowSplashScreen();
	void SetStatusBarMessage(CString message);
	CString GetStatusBarMessage();
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	int GetToolBarStatus(int id);
    void SetStatusBarXY(double x, double y, BOOL enable = TRUE);
	void EnableToolBarButton(int id);
	void SetStatusBarDrawTime(double elasped, BOOL drawtime = TRUE);
	void GetStrLvlRect(CRect *rect);
	void GetDropDownRect(CRect* rect);
	void CreateToolBar();
	CToolBar* GetToolBar(){return &m_tbToolBar;}
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CStatusBar		m_wndStatusBar;
	CToolBar		m_wndToolBar;
	CToolBar		m_tbToolBar;
	BOOL			m_bToolBarCreated;
	BOOL			m_bToolTipEnabled;
	CComboBox		m_comboDummy1;
	CComboBox		m_comboDummy2;
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowSplashScreen();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////


#endif // !defined(AFX_MAINFRM_H__31DAE01B_8552_4EFF_883F_94D70A1F49A1__INCLUDED_)
