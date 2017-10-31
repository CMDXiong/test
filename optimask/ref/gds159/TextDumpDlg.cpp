#include "stdafx.h"
#include "gds.h"
#include "TextDumpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////


CTextDumpDlg::CTextDumpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextDumpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextDumpDlg)
	//}}AFX_DATA_INIT
}


void CTextDumpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextDumpDlg)
	DDX_Control(pDX, IDC_EDIT_DUMP, m_editDump);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextDumpDlg, CDialog)
	//{{AFX_MSG_MAP(CTextDumpDlg)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_SELECT_ALL_CELL, OnSelectAllCell)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_SAVE_TEXT_DUMP, OnSaveTextDump)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CTextDumpDlg::PostNcDestroy() 
{
	delete this;
	//CDialog::PostNcDestroy();
}

void CTextDumpDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
    if(IsWindow(m_editDump)){
        WINDOWPLACEMENT place;
        place.length = sizeof(place);
        m_editDump.GetWindowPlacement(&place);
        m_editDump.SetWindowPos(0, place.rcNormalPosition.left, place.rcNormalPosition.top, 
			cx - place.rcNormalPosition.left, cy - place.rcNormalPosition.top, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
    }
	
}

int CTextDumpDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_DUMP), TRUE);
	
	return 0;
}

void CTextDumpDlg::OnSelectAllCell() 
{
	m_editDump.SetSel(0, -1);
}

BOOL CTextDumpDlg::PreTranslateMessage(MSG* pMsg) 
{
    switch (pMsg->message){
	case WM_KEYDOWN:
		GetAsyncKeyState(VK_CONTROL);
		if(GetAsyncKeyState(VK_CONTROL) && (pMsg->wParam == 'A' || pMsg->wParam == 'a')){
			OnSelectAllCell();
			return TRUE;
		}
		break;
	default:
        break;
    }
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CTextDumpDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTextDumpDlg::OnSaveTextDump() 
{
	CFileDialog dlg(FALSE, "txt", NULL, OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "text file (*.txt)|*.txt||", NULL);

	if(dlg.DoModal() == IDCANCEL)
		return;

	CString path = dlg.GetPathName();
	CFile file;
	if(!file.Open(path, CFile::modeCreate | CFile::modeWrite))
		return;

	CArchive ar(&file, CArchive::store);

	int line = m_editDump.GetLineCount();
	char buf[65536];
	for(int i = 0;i < line;i++){
		int num = m_editDump.GetLine(i, buf, sizeof(buf) - 1);
		*(buf + num) = '\0';
		ar.WriteString(buf);
		ar.WriteString("\r\n");
	}

	ar.Close();
	file.Close();
}
