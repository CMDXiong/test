// gds.cpp 
//

#include "stdafx.h"
#include "gds.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "gdsDoc.h"
#include "gdsView.h"
#include "Splash.h"
#include "GDS2MultiDocTemplate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGdsApp

BEGIN_MESSAGE_MAP(CGdsApp, CWinApp)
	//{{AFX_MSG_MAP(CGdsApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_OPEN_SAME_TEC, OnFileOpenSameTec)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CGdsApp::CGdsApp()
{
	m_hwndGridDlg = NULL;
	m_hwndCGdsView = NULL;
}

CGdsApp::~CGdsApp()
{
}
/////////////////////////////////////////////////////////////////////////////

CGdsApp theApp;
static ULONG_PTR m_gdiplusToken;
static Gdiplus::GdiplusStartupInput gdiplusStartupInput;
static Gdiplus::GdiplusStartupOutput gdiplusStartupOutput;

// {70071046-426D-4470-8158-0FFBD0B94A98}
static const CLSID clsid =
{ 0x70071046, 0x426d, 0x4470, { 0x81, 0x58, 0xf, 0xfb, 0xd0, 0xb9, 0x4a, 0x98 } };

/////////////////////////////////////////////////////////////////////////////
int CGdsApp::ExitInstance() 
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);	
	return CWinApp::ExitInstance();
}

BOOL CGdsApp::InitInstance()
{
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, &gdiplusStartupOutput);

	OSVERSIONINFO verinfo;
	verinfo.dwOSVersionInfoSize = sizeof(verinfo);
	::GetVersionEx(&verinfo);

	if(verinfo.dwPlatformId != VER_PLATFORM_WIN32_NT){
		// Not available on non-NT versions of Windows.
		AfxMessageBox(IDS_CANNOT_RUN_ON_16BIT_WINDOWS_LT_4);
		return FALSE;
	}
	else if(verinfo.dwPlatformId == VER_PLATFORM_WIN32_NT && verinfo.dwMajorVersion < 4){
		// Not available on Windows less than NT version 4.0.
		AfxMessageBox(IDS_CANNOT_RUN_ON_16BIT_WINDOWS_LT_4);
		return FALSE;
	}

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	if (!AfxOleInit())	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

#ifdef _AFXDLL
	Enable3dControls();
#else
	Enable3dControlsStatic();
#endif

	SetRegistryKey(_T("K.Yuda"));

	LoadStdProfileSettings();
	CGDS2MultiDocTemplate* pDocTemplate;
	pDocTemplate = new CGDS2MultiDocTemplate(
		IDR_GDSTYPE,
		RUNTIME_CLASS(CGdsDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CGdsView));
	AddDocTemplate(pDocTemplate);

	m_server.ConnectTemplate(clsid, pDocTemplate, FALSE);

	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;


	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
		return TRUE;
	CSplashWnd::EnableSplashScreen();
	CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
	if(cmdInfo.m_strFileName.GetLength() > 0){
		WIN32_FIND_DATA find_dat;
		if(FindFirstFile(cmdInfo.m_strFileName, &find_dat) != INVALID_HANDLE_VALUE){
			char path[_MAX_PATH], *ptr;

			if(GetFullPathName(cmdInfo.m_strFileName, _MAX_PATH, path, &ptr) > 0){
				cmdInfo.m_strFileName = cmdInfo.m_strFileName.Left(ptr - path);
				cmdInfo.m_strFileName += find_dat.cFileName;
				CSplashWnd::EnableSplashScreen(FALSE);
			}
		}
	}
	// Do not display MDI child window
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	m_server.UpdateRegistry(OAT_DISPATCH_OBJECT);
	COleObjectFactory::UpdateRegistryAll();

	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_strVersion;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_strVersion = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_VERSION_STRING, m_strVersion);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CGdsApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////

BOOL CGdsApp::PreTranslateMessage(MSG* pMsg)
{
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;


	return CWinApp::PreTranslateMessage(pMsg);
}

void CAboutDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	CString path_exe = ((CGdsApp*)AfxGetApp())->GetExePath();
    DWORD tmp;
    int size = GetFileVersionInfoSize(path_exe, &tmp);

    if(size > 0){
        char* buf = new char[size];
        GetFileVersionInfo(path_exe, 0, size, buf);

        char *recv;
        unsigned len;
	    if(VerQueryValue(buf, TEXT("\\StringFileInfo\\040904b0\\ProductVersion"), (void**)&recv, &len))
            m_strVersion = "Version " + CString(recv);
        else
            m_strVersion ="Version unknown";
        delete [] buf;
    }
    else
        m_strVersion = path_exe;//"Version N/A";

    UpdateData(FALSE);
}

#include "GdsView.h"
#include "GridDlg.h"
BOOL CGdsApp::ProcessMessageFilter(int code, LPMSG lpMsg) 
{
    if(m_hwndGridDlg != NULL){
        if ((lpMsg->hwnd == m_hwndGridDlg) || ::IsChild(m_hwndGridDlg, lpMsg->hwnd)){
            if(lpMsg->message == WM_MOUSELEAVE)
				((CGridDlg*)CWnd::FromHandle(m_hwndGridDlg))->OnMouseLeave();
		}
    }
    if(m_hwndCGdsView != NULL){
        if ((lpMsg->hwnd == m_hwndCGdsView) || ::IsChild(m_hwndCGdsView, lpMsg->hwnd)){
            if(lpMsg->message == WM_KEYDOWN)
				((CGdsView*)CWnd::FromHandle(m_hwndCGdsView))->SendMessage(lpMsg->message,
					lpMsg->wParam, lpMsg->lParam);
		}
    }
	return CWinApp::ProcessMessageFilter(code, lpMsg);
}

BOOL CGdsApp::OnIdle(LONG lCount) 
{
	return CWinApp::OnIdle(lCount);
}

int CGdsApp::Run() 
{
	return CWinApp::Run();
}


CString CGdsApp::GetExePath()
{
	CString path;

    char* name = GetCommandLine();
    char* path_exe = strtok(name, "\"");
    if(CString(path_exe).Find('/') > 0)
        path_exe = strtok(path_exe, "/");
	path = path_exe;
	return path;
}


void CGdsApp::OnFilePrintSetup() 
{
	CWinApp::OnFilePrintSetup();	
}

CDocument* CGdsApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	return CWinApp::OpenDocumentFile(lpszFileName);
}

void CGdsApp::OnFileOpen() 
{
	CWinApp::OnFileOpen();
}

void CGdsApp::OnFileOpenSameTec() 
{
	POSITION template_pos = GetFirstDocTemplatePosition();

	CGdsView* pActiveView = NULL;
	while(template_pos){
		CMultiDocTemplate* pDocTemplate = (CMultiDocTemplate*)GetNextDocTemplate(template_pos);
		POSITION posDoc = pDocTemplate->GetFirstDocPosition();
		while(posDoc){
			CGdsDoc* pDoc = (CGdsDoc*)pDocTemplate->GetNextDoc(posDoc);
			POSITION posView = pDoc->GetFirstViewPosition();
			while(posView){
				CGdsView* pView = (CGdsView*)pDoc->GetNextView(posView);
				((CMainFrame*)AfxGetMainWnd())->IsViewActive(pView);
				pActiveView = pView;
				break;
			}
			if(pActiveView)
				break;
		}
		if(pActiveView)
			break;
	}

	if(pActiveView)
		SetTecFile(pActiveView->GetDocument()->GetTecFilePath());
	CWinApp::OnFileOpen();
	SetTecFile("");
}

