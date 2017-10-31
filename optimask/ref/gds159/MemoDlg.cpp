#include "stdafx.h"
#include "gds.h"
#include "MemoDlg.h"
#include "GdsDoc.h"
#include "GdsView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GetView() ((CGdsView*)m_pParent)
/////////////////////////////////////////////////////////////////////////////


CMemoDlg::CMemoDlg(CWnd* pParent)
	: CDialog(CMemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMemoDlg)
	//}}AFX_DATA_INIT
	m_pParent = pParent;
}


void CMemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMemoDlg)
	DDX_Control(pDX, IDC_EDIT_MEMO, m_editMemo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMemoDlg, CDialog)
	//{{AFX_MSG_MAP(CMemoDlg)
	ON_BN_CLICKED(IDC_BUTTON_FONT, OnButtonFont)
	ON_EN_CHANGE(IDC_EDIT_MEMO, OnChangeEditMemo)
	ON_EN_UPDATE(IDC_EDIT_MEMO, OnUpdateEditMemo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CMemoDlg::ExitDialog()
{
	*m_pEnabler = FALSE;
	CDialog::OnCancel();
}

void CMemoDlg::OnCancel() 
{
	GetView()->ChangeEditMode(EDIT_MODE_NONE);
	*m_pEnabler = FALSE;
	CDialog::OnCancel();
}

void CMemoDlg::OnOK() 
{
	UpdateData(TRUE);
	GetView()->m_memo.SetMemoStr(UpdateMemoString());
	//CDialog::OnOK();
}

void CMemoDlg::OnButtonFont() 
{
	LOGFONT *pfont = GetView()->m_memo.GetFont();
	CFontDialog dialog(pfont);

	dialog.m_cf.Flags |= CF_TTONLY;
	dialog.m_cf.Flags &= ~CF_EFFECTS;

	if(dialog.DoModal() != IDCANCEL){
		LOGFONT font;
		dialog.GetCurrentFont(&font);
		GetView()->m_memo.SetFont(font);
	}
}

CString CMemoDlg::UpdateMemoString()
{
	CString str = "";
	char buf[512];
	for(int idx = 0;idx < m_editMemo.GetLineCount();idx++){
		int len = m_editMemo.GetLine(idx, buf, sizeof(buf) - 1);
		*(buf + len) = '\0';
		str += buf;
		if(idx < m_editMemo.GetLineCount() - 1)
			str += "\n";
	}
	return str;
}

void CMemoDlg::OnChangeEditMemo() 
{
	UpdateData(TRUE);
	GetView()->m_memo.SetMemoStr(UpdateMemoString());
}

void CMemoDlg::SetEnabler(BOOL *enabler)
{
	m_pEnabler = enabler;
}


void CMemoDlg::OnUpdateEditMemo() 
{
}

BOOL CMemoDlg::PreTranslateMessage(MSG* pMsg) 
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CMemoDlg::PostNcDestroy() 
{
	delete this;
	//CDialog::PostNcDestroy();
}
