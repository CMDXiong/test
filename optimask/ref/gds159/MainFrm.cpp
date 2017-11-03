// MainFrm.cpp
//

#include "stdafx.h"
#include "gds.h"

#include "MainFrm.h"
#include "Splash.h"
#include "GdsDoc.h"
#include "GdsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_SHOW_SPLASH_SCREEN, OnShowSplashScreen)
	ON_WM_SHOWWINDOW()
	ON_WM_DROPFILES()
	ON_WM_MOVE()
	ON_WM_MOVING()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_ACTIVATE()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_INITMENUPOPUP()
	//}}AFX_MSG_MAP
    ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)
END_MESSAGE_MAP()


#define SB_WIDTH_COMMON 200
#define SB_WIDTH_GUIDE  400
#define SB_WIDTH_TIME   110
#define SB_WIDTH_COORD  220

static UINT indicators[] =
{
	ID_SEPARATOR, 
	ID_SB_CMD_MESSAGE,
	ID_SB_DRAW_TIME,
	ID_STATUSBAR,
    //ID_INDICATOR_KANA,
	//ID_INDICATOR_CAPS,
	//ID_INDICATOR_NUM,
	//ID_INDICATOR_SCRL,
};
enum paneid{
	PANE_ID_CMD_GUIDE = 1,
	PANE_ID_DRAW_TIME,
	PANE_ID_COORD,
};

static unsigned uintTimerID = 0;
static void CALLBACK timercallback(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	if(uTimerID == uintTimerID){
		if(((CMainFrame*)dwUser)->IsKindOf(RUNTIME_CLASS(CMainFrame)))
			((CMainFrame*)dwUser)->PollStopDraw();
	}
}

void CMainFrame::PollStopDraw()
{
	if(GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState('C')){
		CPointer* pPtr;
		CGdsView* pView;
		for(int i = 0;i < m_arrayView.GetSize();i++){
			pPtr = (CPointer*)m_arrayView.GetAt(i);
			pView = (CGdsView*)pPtr->GetPointer();
			pView->StopDraw();
		}
	}
}

void CMainFrame::CreateAbortDrawTimer()
{
	// Clear key state
	GetAsyncKeyState(VK_CONTROL);
	GetAsyncKeyState('C');

	if(uintTimerID > 0)
		return;
	uintTimerID = timeSetEvent(100, 100, timercallback, (DWORD)this, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
}

void CMainFrame::KillAbortDrawTimer()
{
	if(uintTimerID > 0){
		timeKillEvent(uintTimerID);
		uintTimerID = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

CMainFrame::CMainFrame()
{
	m_bSyncViewAlways = FALSE;
	m_bToolTipEnabled = TRUE;
	m_bToolBarCreated = FALSE;
	m_strTitleOrg = "";
	m_pViewActive = NULL;
	m_bPreview = FALSE;
}

CMainFrame::~CMainFrame()
{
	KillAbortDrawTimer();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;
	}
	m_wndStatusBar.SetPaneInfo(0, 0, SBPS_STRETCH | SBPS_NOBORDERS, SB_WIDTH_COMMON);
	m_wndStatusBar.SetPaneInfo(PANE_ID_CMD_GUIDE, ID_SB_CMD_MESSAGE, SBPS_NORMAL, SB_WIDTH_GUIDE);
	m_wndStatusBar.SetPaneInfo(PANE_ID_DRAW_TIME, ID_SB_DRAW_TIME, SBPS_NORMAL, SB_WIDTH_TIME);
	m_wndStatusBar.SetPaneInfo(PANE_ID_COORD, ID_STATUSBAR, SBPS_NORMAL, SB_WIDTH_COORD);
	SetStatusBarDrawTime(-1.0);
	SetStatusBarXY(0.0, 0.0, FALSE);
	

	m_wndToolBar.EnableDocking(/*CBRS_ALIGN_TOP); //*/CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_TOP); //CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

    DragAcceptFiles(TRUE);

	if(m_strTitleOrg.GetLength() == 0){
		CString version_str;

		char *path_exe, path[_MAX_PATH];
		DWORD tmp;
		const char* name = GetCommandLine();
		strcpy(path, name);
		path_exe = strtok(path, "\"");
		if(CString(path_exe).Find('/') > 0)
			path_exe = strtok(path_exe, "/");

		int size = GetFileVersionInfoSize(path_exe, &tmp);

		if(size > 0){
			char* buf = new char[size];
			GetFileVersionInfo(path_exe, 0, size, buf);

			char *recv;
			unsigned len;
			if(VerQueryValue(buf, TEXT("\\StringFileInfo\\040904b0\\ProductVersion"), (void**)&recv, &len))
				version_str = " version" + CString(recv);
			else
				version_str ="";
			delete [] buf;
		}
		else
			version_str = "";
	
		m_strTitleOrg = CMDIFrameWnd::m_strTitle + version_str;
		OnUpdateFrameTitle(TRUE);
	}


	CreateToolBar();
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetStatusBarXY(double x, double y, BOOL enable)
{
	CString message;
	CString str;
	if(enable){
		str.Format(" (%.3f", x);
		str.TrimRight('0');
		str.TrimRight('.');
		message.Format("%sum, ", str);

		str.Format("%.3f", y);
		str.TrimRight('0');
		str.TrimRight('.');
		str.Format("%sum)", str);
		message += str;
		m_wndStatusBar.SetPaneText(PANE_ID_COORD, message);
	}
	else
		m_wndStatusBar.SetPaneText(PANE_ID_COORD, "");

	return;
}


void CMainFrame::SetStatusBarDrawTime(double elasped, BOOL drawtime)
{
	elasped /= CLOCKS_PER_SEC;
	elasped *= 1000.0; // convert to msec
	CString message, frmt;

	if(drawtime)
		frmt = "Draw ";
	else
		frmt = "Read ";

	if(elasped > 1000.0){
		frmt += "%.2fsec";
		elasped /= 1000.0;
	}
	else
		frmt += "%.0fms";

	if(elasped >= 0.0)
		message.Format(frmt, elasped);
	else
		message.Format("");//Draw        msec");
	m_wndStatusBar.SetPaneText(PANE_ID_DRAW_TIME, message);
}

void CMainFrame::CreateToolBar()
{
	if(m_bToolBarCreated)
		return;

	if (!m_tbToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_tbToolBar.LoadToolBar(IDR_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
	}
	m_tbToolBar.SetButtonInfo(0, ID_STRUCTURE_CB, TBBS_SEPARATOR, 180);
	m_tbToolBar.SetButtonInfo(1, ID_STR_LEVEL_CB, TBBS_SEPARATOR, 75);


	ShowControlBar(&m_tbToolBar, TRUE, FALSE);
	m_tbToolBar.EnableDocking(CBRS_ALIGN_TOP); //CBRS_ALIGN_ANY);
	CRect rect;
	m_wndToolBar.GetWindowRect(&rect);
	rect.OffsetRect(abs(rect.Width()), 0);
	DockControlBar(&m_tbToolBar,AFX_IDW_DOCKBAR_TOP,rect);
	RecalcLayout();

	m_bToolBarCreated = TRUE;
	CreateDummyComboBoxes();
}

void CMainFrame::ShowDummyComboBoxes()
{
	m_comboDummy1.ShowWindow(SW_SHOW);
	m_comboDummy2.ShowWindow(SW_SHOW);
}

void CMainFrame::HideDummyComboBoxes()
{
	m_comboDummy1.ShowWindow(SW_HIDE);
	m_comboDummy2.ShowWindow(SW_HIDE);
}

void CMainFrame::CreateDummyComboBoxes()
{
	CRect rect;
	GetDropDownRect(&rect);
	m_comboDummy1.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL |
			CBS_DROPDOWNLIST | CBS_SORT, rect, &m_tbToolBar, ID_STRUCTURE_CB);
	GetStrLvlRect(&rect);
	m_comboDummy2.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL |
			CBS_DROPDOWNLIST | CBS_SORT, rect, &m_tbToolBar, ID_STR_LEVEL_CB);

	m_comboDummy1.EnableWindow(FALSE);
	m_comboDummy2.EnableWindow(FALSE);
}

void CMainFrame::GetDropDownRect(CRect *rect)
{
	if(!m_bToolBarCreated)
		return;
	m_tbToolBar.GetItemRect(0, rect);
	rect->bottom = rect->top + 200;
}

void CMainFrame::GetStrLvlRect(CRect *rect)
{
	if(!m_bToolBarCreated)
		return;
	m_tbToolBar.GetItemRect(1, rect);
	rect->bottom = rect->top + 200;
}


void CMainFrame::EnableToolBarButton(int id)
{
	CToolBarCtrl& control = m_tbToolBar.GetToolBarCtrl();
	control.SetState(id, TBSTATE_ENABLED);
}



int CMainFrame::GetToolBarStatus(int id)
{
	CToolBarCtrl& control = m_tbToolBar.GetToolBarCtrl();
	return control.IsButtonPressed(id);
}


void CMainFrame::ShowSplashScreen()
{
	CSplashWnd::EnableSplashScreen();
	CSplashWnd::ShowSplashScreen(this);
}


void CMainFrame::OnShowSplashScreen() 
{
	ShowSplashScreen();
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	CMDIFrameWnd::m_strTitle = m_strTitleOrg;
	CMDIFrameWnd::OnUpdateFrameTitle(bAddToTitle);
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CMDIFrameWnd::OnShowWindow(bShow, nStatus);
	CSplashWnd::ShowSplashScreen(this);
	if(bShow){
		SetForegroundWindow();
		BringWindowToTop();
	}
}

void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
	SetForegroundWindow();
	BringWindowToTop();
	
	CMDIFrameWnd::OnDropFiles(hDropInfo);
}

void CMainFrame::OnDropFiles(HDROP hDropInfo, CString tec)
{
	((CGdsApp*)AfxGetApp())->SetTecFile(tec);

	CMDIFrameWnd::OnDropFiles(hDropInfo);

	((CGdsApp*)AfxGetApp())->SetTecFile("");
}

void CMainFrame::SetStatusBarMessage(CString message)
{
	m_wndStatusBar.SetPaneText(PANE_ID_CMD_GUIDE, message);
}

CString CMainFrame::GetStatusBarMessage()
{
	return m_wndStatusBar.GetPaneText(PANE_ID_CMD_GUIDE);
}



void CMainFrame::AddView(CGdsView* pView)
{
	CPointer* pNew = new CPointer;
	pNew->SetPointer((DWORD)pView);
	m_arrayView.Add(pNew);

}

void CMainFrame::RemoveView(CGdsView* pView)
{
	CPointer* pPtr;
	CGdsView* pViewNow;
	for(int i = 0;i < m_arrayView.GetSize();i++){
		pPtr = (CPointer*)m_arrayView.GetAt(i);
		pViewNow = (CGdsView*)pPtr->GetPointer();
		if(pView == pViewNow){
			m_arrayView.RemoveAt(i);
			delete pPtr;
		}
	}
	if(m_arrayView.GetSize() == 0)
		ShowDummyComboBoxes();
}


void CMainFrame::RedrawToolBar()
{
	CToolBarCtrl& control = m_tbToolBar.GetToolBarCtrl();
	control.RedrawWindow(NULL);
}

void CMainFrame::ShowToolBar()
{
	if(!m_bToolBarCreated)
		return;
	m_tbToolBar.ShowWindow(TRUE);
}

void CMainFrame::HideToolBar()
{
	if(!m_bToolBarCreated)
		return;
	m_tbToolBar.ShowWindow(FALSE);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	if(::IsWindow(m_tbToolBar.m_hWnd)){
		_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
		CToolTipCtrl* ptt = pThreadState->m_pToolTip;
		if(ptt && ::IsWindow(ptt->m_hWnd))
			ptt->Activate(m_bToolTipEnabled);
	}

	return CMDIFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::ActivateToolTip(BOOL activate)
{
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	CToolTipCtrl* ptt = pThreadState->m_pToolTip;

	if(!activate)
		m_bToolTipEnabled = FALSE;
	else{
		CPointer* pPtr;
		CGdsView* pView;
		for(int i = 0;i < m_arrayView.GetSize();i++){
			pPtr = (CPointer*)m_arrayView.GetAt(i);
			pView = (CGdsView*)pPtr->GetPointer();
			if(pView->IsDrawing())
				break;
		}
		if(i >= m_arrayView.GetSize())
			m_bToolTipEnabled = TRUE;
	}
	if(ptt && ::IsWindow(ptt->m_hWnd))
		ptt->Activate(m_bToolTipEnabled);
}

void CMainFrame::OnMove(int x, int y) 
{
	LockView(FALSE);
	CMDIFrameWnd::OnMove(x, y);
}

void CMainFrame::OnMoving(UINT fwSide, LPRECT pRect) 
{
	LockView(TRUE);
	CMDIFrameWnd::OnMoving(fwSide, pRect);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	LockView(FALSE);
	CRect rect;
	m_wndStatusBar.GetClientRect(&rect);
	int total = SB_WIDTH_COMMON + SB_WIDTH_GUIDE + SB_WIDTH_TIME + SB_WIDTH_COORD;
	int width = rect.Width();
	m_wndStatusBar.SetPaneInfo(PANE_ID_CMD_GUIDE, ID_SB_CMD_MESSAGE, SBPS_NORMAL, (int)((double)SB_WIDTH_GUIDE/total * width));
	m_wndStatusBar.SetPaneInfo(PANE_ID_DRAW_TIME, ID_SB_DRAW_TIME, SBPS_NORMAL, (int)((double)SB_WIDTH_TIME/total * width));
	m_wndStatusBar.SetPaneInfo(PANE_ID_COORD, ID_STATUSBAR, SBPS_NORMAL, (int)((double)SB_WIDTH_COORD/total * width));
	m_wndStatusBar.SetPaneInfo(0, 0, SBPS_STRETCH | SBPS_NOBORDERS, (int)((double)SB_WIDTH_COMMON/total * width));
	CMDIFrameWnd::OnSize(nType, cx, cy);
}

void CMainFrame::OnSizing(UINT fwSide, LPRECT pRect) 
{
	LockView(TRUE);
	CMDIFrameWnd::OnSizing(fwSide, pRect);
}

void CMainFrame::LockView(BOOL lock)
{
	CPointer* pPtr;
	CGdsView* pView;
	for(int i = 0;i < m_arrayView.GetSize();i++){
		pPtr = (CPointer*)m_arrayView.GetAt(i);
		pView = (CGdsView*)pPtr->GetPointer();
		if(pView->IsDrawing()){
			if(lock)
				pView->LockWindowUpdate();
			else
				pView->UnlockWindowUpdate();
		}
	}
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CMDIFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	if(nState == WA_INACTIVE)
		LockView(TRUE);
}

void CMainFrame::OnSetPreviewMode(BOOL bPreview, CPrintPreviewState* pState) 
{
	m_bPreview = bPreview;
	CMDIFrameWnd::OnSetPreviewMode(bPreview, pState);
}

void CMainFrame::RecalcLayout(BOOL bNotify) 
{
	
	CMDIFrameWnd::RecalcLayout(bNotify);
}

BOOL CMainFrame::IsThereMoreView(CGdsView* pView)
{
	CWinApp* app = AfxGetApp();
	POSITION template_pos = app->GetFirstDocTemplatePosition();

	while(template_pos){
		CMultiDocTemplate* pDocTemplate = (CMultiDocTemplate*)app->GetNextDocTemplate(template_pos);
		POSITION posDoc = pDocTemplate->GetFirstDocPosition();
		while(posDoc){
			CGdsDoc* pDoc = (CGdsDoc*)pDocTemplate->GetNextDoc(posDoc);
			POSITION posView = pDoc->GetFirstViewPosition();
			while(posView){
				CGdsView* pViewCmpr = (CGdsView*)pDoc->GetNextView(posView);
				if(pView != pViewCmpr)
					return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CMainFrame::IsViewActive(CGdsView* pView)
{
	CMDIFrameWnd *pFrame = 
				 (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;

	CMDIChildWnd *pChild = 
				 (CMDIChildWnd *) pFrame->GetActiveFrame();


	CGdsView *pChildView = (CGdsView *) pChild->GetActiveView();
	if(pChildView == pView)
		return TRUE;
	return FALSE;
}


void CMainFrame::SyncViews(CGdsView* pViewNow)
{
	CWinApp* app = AfxGetApp();
	POSITION template_pos = app->GetFirstDocTemplatePosition();

	while(template_pos){
		CMultiDocTemplate* pDocTemplate = (CMultiDocTemplate*)app->GetNextDocTemplate(template_pos);
		POSITION posDoc = pDocTemplate->GetFirstDocPosition();
		while(posDoc){
			CGdsDoc* pDoc = (CGdsDoc*)pDocTemplate->GetNextDoc(posDoc);
			POSITION posView = pDoc->GetFirstViewPosition();
			while(posView){
				CGdsView* pView = (CGdsView*)pDoc->GetNextView(posView);
				if(pView == pViewNow)
					continue;
				pView->SyncToAnotherView(pViewNow);
			}
		}
	}

}


