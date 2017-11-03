#ifndef _SPLASH_SCRN_
#define _SPLASH_SCRN_


/////////////////////////////////////////////////////////////////////////////

class CSplashWnd : public CWnd
{
public:
	CSplashWnd();


public:
	CBitmap m_bitmap;

public:
	static void EnableSplashScreen(BOOL bEnable = TRUE);
	static void ShowSplashScreen(CWnd* pParentWnd = NULL);
	static BOOL PreTranslateAppMessage(MSG* pMsg);
	void KillSplashScreen();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashWnd)
	//}}AFX_VIRTUAL

public:
	~CSplashWnd();
	virtual void PostNcDestroy();

protected:
	BOOL Create(CWnd* pParentWnd = NULL);
	static BOOL c_bShowSplashWnd;
	static CSplashWnd* c_pSplashWnd;
	void HideSplashScreen();

protected:
	//{{AFX_MSG(CSplashWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
