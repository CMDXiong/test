#include "stdafx.h"
#include "gds.h"
#include "MTRender.h"

#include "GdsDoc.h"
#include "GdsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMTRender

CRITICAL_SECTION CMTRender::m_csGDILock;

IMPLEMENT_DYNAMIC(CMTRender, CWinThread)

CMTRender::CMTRender(HWND hwnd, HDC hdc)
{
	m_bAutoDelete = FALSE;
	m_pMainWnd = AfxGetMainWnd();
	m_hWndView = hwnd;
	m_hDC = hdc;
	m_pView = (CGdsView*)CWnd::FromHandle(m_hWndView);
	needDraw = FALSE;

	m_hEventKill = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventStop = CreateEvent(NULL, TRUE, TRUE, NULL);
}

CMTRender::~CMTRender()
{
	CloseHandle(m_hEventKill);
	CloseHandle(m_hEventDead);
	CloseHandle(m_hEventStop);
}

void CMTRender::Delete()
{
	//CWinThread::Delete();

	SetEvent(m_hEventDead);
}

BOOL CMTRender::WaitUntilDrawn()
{
	if(!m_pView->IsDrawing())
		return TRUE;
	if(WaitForSingleObject(m_hEventStop, 1000) == WAIT_TIMEOUT)//INFINITE);
		return FALSE;
	return TRUE;
}

BOOL CMTRender::InitInstance()
{
	// Initialize static members of CGDIThread
	InitializeCriticalSection(&CMTRender::m_csGDILock);

	// loop but check for kill notification
	while (WaitForSingleObject(m_hEventKill, 0) == WAIT_TIMEOUT){
		//EnterCriticalSection(&CMTRender::m_csGDILock);
		if(m_hWndView)
			DrawLoop();
		//LeaveCriticalSection(&CMTRender::m_csGDILock);
	}
	// avoid entering standard message loop by returning FALSE
	return FALSE;
}

int CMTRender::ExitInstance()
{
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CMTRender, CWinThread)
	//{{AFX_MSG_MAP(CMTRender)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CMTRender::KillThread()
{
	SetEvent(m_hEventKill);

	SetThreadPriority(THREAD_PRIORITY_NORMAL);
	WaitForSingleObject(m_hEventDead, INFINITE);
	WaitForSingleObject(m_hThread, INFINITE);

	delete this;
}

void CMTRender::AskDrawStructure()
{
	ResetEvent(m_hEventStop);
	needDraw = TRUE;
}

void CMTRender::DrawLoop()
{
	if(needDraw){
		needDraw = FALSE;
		ResetEvent(m_hEventStop);
		if(m_pView){
			CClientDC dc(m_pView);
			m_pView->OnPrepareDC(&dc);
			m_pView->SetOrigin(&dc);
			CDC* pDC = m_pView->SetDC(&dc);
			m_pView->Render();
			m_pView->SetDC(pDC);
			SetEvent(m_hEventStop);
		}
	}
	else
		Sleep(100);
}

