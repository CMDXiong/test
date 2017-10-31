#include "stdafx.h"
#include "gds.h"
#include "StructureInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CStructureInfoDlg::CStructureInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStructureInfoDlg::IDD, pParent)
{
	m_pView = (CGdsView*)pParent;
	//{{AFX_DATA_INIT(CStructureInfoDlg)
	//}}AFX_DATA_INIT
}


void CStructureInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStructureInfoDlg)
	DDX_Control(pDX, IDC_RICHEDIT_STRUCTURE_INFO, m_reditStrInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStructureInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CStructureInfoDlg)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

int CStructureInfoDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AfxInitRichEdit();

	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_STRUCTURE_INFO), TRUE);
	
	return 0;
}

static DWORD CALLBACK callback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
    DWORD ret = 0; // success

    CFile* file = (CFile*)dwCookie;
    *pcb = file->Read(pbBuff, cb);
    return ret;
}

void CStructureInfoDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

    WINDOWPLACEMENT place;

	GetWindowPlacement(&place);
	CWinApp* app = AfxGetApp();
	int x =	app->GetProfileInt("Misc", "Cell info. dialog window X", -1);
	int y = app->GetProfileInt("Misc", "Cell info. dialog window Y", -1);
	if(x > 0 && y > 0)
		SetWindowPos(0, place.rcNormalPosition.left, place.rcNormalPosition.top, 
					x, y, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);


    CHARFORMAT cf;
    cf.cbSize = sizeof(cf); 
    cf.dwMask = CFM_FACE | CFM_CHARSET | CFM_SIZE | CFM_COLOR;
    cf.bCharSet = ANSI_CHARSET;
	cf.crTextColor = RGB(0, 0, 0);
	//cf.dwEffects = CFE_BOLD;
    cf.yHeight = 200;
    strncpy(cf.szFaceName, "Courier New", LF_FACESIZE - 1); 

	m_reditStrInfo.SetBackgroundColor(FALSE, RGB(255, 255, 255));
    m_reditStrInfo.SetDefaultCharFormat(cf);
    m_reditStrInfo.SetEventMask(ENM_NONE);
    m_reditStrInfo.SetReadOnly();

	SetInformation();

}

void CStructureInfoDlg::ClearText()
{
    CMemFile m_FileMemfile;
    m_FileMemfile.SeekToBegin();
    CString message = "Left click SREF or AREF";
    m_FileMemfile.Write(message, message.GetLength());
    m_FileMemfile.SetLength(m_FileMemfile.GetPosition());
    m_FileMemfile.SeekToBegin();

    EDITSTREAM es;
    es.dwCookie = (DWORD)&m_FileMemfile;
    es.dwError = 0;
    es.pfnCallback = callback;
    m_reditStrInfo.StreamIn(SF_TEXT, es);

	UpdateData(FALSE);
}

void CStructureInfoDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
    if(IsWindow(m_reditStrInfo)){
        WINDOWPLACEMENT place;
        place.length = sizeof(place);
        m_reditStrInfo.GetWindowPlacement(&place);
        m_reditStrInfo.SetWindowPos(0, place.rcNormalPosition.left, place.rcNormalPosition.top, 
			cx - place.rcNormalPosition.left, cy - place.rcNormalPosition.top, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);

		GetWindowPlacement(&place);
		CWinApp* app = AfxGetApp();
		app->WriteProfileInt("Misc", "Cell info. dialog window X", place.rcNormalPosition.right - place.rcNormalPosition.left);
		app->WriteProfileInt("Misc", "Cell info. dialog window Y", place.rcNormalPosition.bottom - place.rcNormalPosition.top);
    }
}


void CStructureInfoDlg::ExitDialog()
{
	OnCancel();
}

void CStructureInfoDlg::OnCancel()
{
	*m_pEnabler = FALSE;
	CDialog::OnCancel();
}

void CStructureInfoDlg::PostNcDestroy() 
{
	delete this;	
	//CDialog::PostNcDestroy();
}

void CStructureInfoDlg::SetInformation()
{
    ClearText();

    CMemFile m_FileMemfile;
    m_FileMemfile.SeekToBegin();
	m_pView->WriteStructureInfo(m_FileMemfile);
    m_FileMemfile.SetLength(m_FileMemfile.GetPosition());
    m_FileMemfile.SeekToBegin();

    EDITSTREAM es;
    es.dwCookie = (DWORD)&m_FileMemfile;
    es.dwError = 0;
    es.pfnCallback = callback;
    m_reditStrInfo.StreamIn(SF_TEXT, es);

    UpdateData(FALSE);
}

BOOL CStructureInfoDlg::PreTranslateMessage(MSG* pMsg) 
{
    switch (pMsg->message){
	case WM_KEYDOWN:
		switch(pMsg->wParam){
		case VK_TAB:
			break;
		default:
			//m_pView->OnKeyDown(pMsg->wParam,  0, pMsg->lParam);
			//return TRUE;
			break;
		}
		break;
	default:
        break;
    }
	
	return CDialog::PreTranslateMessage(pMsg);
}


void CStructureInfoDlg::OnTimer(UINT nIDEvent) 
{
	CDialog::OnTimer(nIDEvent);
}
