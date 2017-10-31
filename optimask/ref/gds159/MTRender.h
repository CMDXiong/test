#if !defined(AFX_MTRENDER_H__EB79A9C2_1731_4444_83BB_9E7884FE528E__INCLUDED_)
#define AFX_MTRENDER_H__EB79A9C2_1731_4444_83BB_9E7884FE528E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
class CGdsView;
class CMTRender : public CWinThread
{
	BOOL   needDraw;
public:
	DECLARE_DYNCREATE(CMTRender)
	CMTRender(HWND hwnd, HDC hdc);
	HWND    m_hWndView;
	HDC		m_hDC;
	CGdsView* m_pView;
	HANDLE	m_hEventKill;
	HANDLE	m_hEventDead;
	HANDLE  m_hEventStop;
	static CRITICAL_SECTION m_csGDILock;

public:
	void KillThread();

	void AskDrawStructure();

	//{{AFX_VIRTUAL(CMTRender)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

public:
	BOOL WaitUntilDrawn();
	void DrawLoop();
	virtual ~CMTRender();
	virtual void Delete();

	//{{AFX_MSG(CMTRender)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_MTRENDER_H__EB79A9C2_1731_4444_83BB_9E7884FE528E__INCLUDED_)
