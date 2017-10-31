#include "stdafx.h"
#include "gds.h"
#include "StructureDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////


CStructureDlg::CStructureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStructureDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStructureDlg)
	m_intSubLevel = 0;
	//}}AFX_DATA_INIT
	m_strPrev = "";
	m_gdsViewCaller = NULL;
	m_pTextDumpDlg = NULL;
	m_bTextDumpDlg = FALSE;
}


void CStructureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStructureDlg)
	DDX_Control(pDX, IDC_STRUCTURE, m_comboStructure);
	DDX_Control(pDX, IDC_STR_TREE, m_ctrlStructure);
	DDX_Text(pDX, IDC_SUB_LEVEL, m_intSubLevel);
	DDV_MinMaxInt(pDX, m_intSubLevel, 0, 1023);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStructureDlg, CDialog)
	//{{AFX_MSG_MAP(CStructureDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_STR_TREE, OnSelchangedStrTree)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_CLICK, IDC_STR_TREE, OnClickStrTree)
	ON_WM_PAINT()
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_STR_TREE, OnItemexpandedStrTree)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_STR_TREE, OnItemexpandingStrTree)
	ON_NOTIFY(NM_KILLFOCUS, IDC_STR_TREE, OnKillfocusStrTree)
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_CBN_SELCHANGE(IDC_STRUCTURE, OnSelchangeStructure)
	ON_EN_CHANGE(IDC_SUB_LEVEL, OnChangeSubLevel)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_BN_CLICKED(IDC_BUTTON_DUMP_HIERARCHY, OnButtonDumpHierarchy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStructureDlg
void CStructureDlg::OnSelchangedStrTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM item = (pNMTreeView->itemNew).hItem;
	CString namesel = m_ctrlStructure.GetItemText(item);

	CString name;
	int idx = 0;
	while(1){
		idx = m_comboStructure.FindStringExact(idx, namesel);
		if(idx == CB_ERR)
			break;
		m_comboStructure.GetLBText(idx, name);
		if(name == namesel)
			break;
	}

	if(idx != CB_ERR){
		m_comboStructure.SetCurSel(idx);
		CGDSStructure* structure = m_gdsGDS->FindStructure(name.GetBuffer(0));
		if(structure != m_gdsViewCaller->GetActiveStructure())
			m_gdsViewCaller->AddHistory();
		m_gdsViewCaller->ChangeStructure(structure, FALSE, TRUE);
	}

	*pResult = 0;
}

void CStructureDlg::SetStructure(CGDS* pGDS)
{
	//ASSERT_VALID(pGDS);
	m_gdsGDS = pGDS;
}

void CStructureDlg::SelectActive()
{
	int idx = 0;
	while(1){
		CString name;
		idx = m_comboStructure.FindStringExact(idx, m_strPrev);
		if(idx == CB_ERR)
			break;
		m_comboStructure.GetLBText(idx, name);
		if(name == m_strPrev)
			break;
	}
	if(idx != CB_ERR)
		m_comboStructure.SetCurSel(idx);

	HTREEITEM item = m_ctrlStructure.GetRootItem();
	while(item){
		CString strname = m_ctrlStructure.GetItemText(item);
		if(strname == m_strPrev){
			m_ctrlStructure.SelectItem(item);
			m_ctrlStructure.Select(item, TVGN_CARET);
			m_ctrlStructure.Select(item, TVGN_FIRSTVISIBLE);
			m_ctrlStructure.Expand(item, TVE_EXPAND);
			//m_ctrlStructure.SetFocus();
			break;
		}
		item = m_ctrlStructure.GetNextItem(item, TVGN_NEXT);
	}
}

void CStructureDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	SetList();
	CDialog::OnShowWindow(bShow, nStatus);
	if(m_strPrev.GetLength() > 0)
		SelectActive();
}

void CStructureDlg::MakeTree(CGDSStructure* pStr, HTREEITEM parent)
{
	CString strname;
	HTREEITEM item;

	CObArray* refstructures =  pStr->GetRefStructureList();
	for(int idx = 0;idx < refstructures->GetSize();idx++){
		CGDSRefStructure* ref = (CGDSRefStructure*)refstructures->GetAt(idx);
		CGDSStructure* structure = ref->GetStructure();
		strname = structure->GetStructureName();
		item = m_ctrlStructure.InsertItem(strname, parent);
		MakeTree(structure, item);
	}
}

void CStructureDlg::SetList()
{
	HTREEITEM item;

	CObArray* structures = m_gdsGDS->GetStructureList();
	for(int idx = 0;idx < structures->GetSize();idx++){
		CGDSStructure* pStr = (CGDSStructure*)structures->GetAt(idx);
		CString strname = pStr->GetStructureName();
		item = m_ctrlStructure.InsertItem(strname);
		m_comboStructure.AddString(strname);
		MakeTree(pStr, item);
		UpdateData(FALSE);
	}
}


void CStructureDlg::OnClickStrTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}

void CStructureDlg::OnPaint() 
{
	CPaintDC dc(this);
}

BOOL CStructureDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	return CDialog::OnCommand(wParam, lParam);
}

void CStructureDlg::OnItemexpandedStrTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
}

void CStructureDlg::OnItemexpandingStrTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM item = (pNMTreeView->itemNew).hItem;
	*pResult = 0;
}


BOOL CStructureDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CStructureDlg::OnKillfocusStrTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}

int CStructureDlg::DoModal() 
{
	return CDialog::DoModal();
}

void CStructureDlg::OnDestroy() 
{
	CDialog::OnDestroy();
}

void CStructureDlg::OnKillFocus(CWnd* pNewWnd) 
{
	CDialog::OnKillFocus(pNewWnd);
}

void CStructureDlg::OnOK() 
{
	//*m_pBoolEnabler = FALSE;
	//CDialog::OnOK();
}

void CStructureDlg::OnCancel() 
{
	*m_pBoolEnabler = FALSE;
	if(m_bTextDumpDlg)
		m_pTextDumpDlg->ExitDialog();

	CDialog::OnCancel();
}



void CStructureDlg::OnSelchangeStructure() 
{
	int idx = m_comboStructure.GetCurSel();
	CString name;
	m_comboStructure.GetLBText(idx, name);
	CGDSStructure* structure = m_gdsGDS->FindStructure(name.GetBuffer(0));
	if(structure != m_gdsViewCaller->GetActiveStructure())
		m_gdsViewCaller->AddHistory();
	m_gdsViewCaller->ChangeStructure(structure, FALSE, TRUE);
 
	
	HTREEITEM item = m_ctrlStructure.GetRootItem();
	while(item){
		CString strname = m_ctrlStructure.GetItemText(item);
		if(strname == name){
			m_ctrlStructure.SelectItem(item);
			m_ctrlStructure.Select(item, TVGN_CARET);
			m_ctrlStructure.Select(item, TVGN_FIRSTVISIBLE);
			m_ctrlStructure.Expand(item, TVE_EXPAND);
			m_ctrlStructure.SetFocus();
			break;
		}
		item = m_ctrlStructure.GetNextItem(item, TVGN_NEXT);
	}

}

void CStructureDlg::SetActive(CString name)
{
	m_strPrev = name;
}


void CStructureDlg::SetEnableSwitch(BOOL *enabler)
{
	m_pBoolEnabler = enabler;
}


void CStructureDlg::ExitDialog()
{
	OnCancel();
}


void CStructureDlg::SetCaller(CGdsView *caller)
{
	m_gdsViewCaller = caller;
}

void CStructureDlg::SetWindowName(CString name)
{
	CString title;
	GetWindowText(title);
	title += " ";
	title += name;
	SetWindowText(title);
}

void CStructureDlg::OnChangeSubLevel() 
{
	UpdateData(TRUE);
	m_gdsViewCaller->UpdateSubStructureLevel(m_intSubLevel);
}

void CStructureDlg::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if(pNMUpDown->iDelta < 0){
		if(m_intSubLevel < 1023){
			m_intSubLevel ++;
			UpdateData(FALSE);
		}
	}
	else{
		if(m_intSubLevel > 0){
			m_intSubLevel --;
			UpdateData(FALSE);
		}
	}
	m_gdsViewCaller->UpdateSubStructureLevel(m_intSubLevel);
	*pResult = 0;
}

void CStructureDlg::SetExpandLevel(int level)
{
	m_intSubLevel = level;
	UpdateData(FALSE);
}

BOOL CStructureDlg::PreTranslateMessage(MSG* pMsg) 
{
	/*if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_ESCAPE)
		return FALSE;*/

	return CDialog::PreTranslateMessage(pMsg);
}

void CStructureDlg::PostNcDestroy() 
{

	delete this;
	//CDialog::PostNcDestroy();
}

#define NEWLINE      "\r\n"
#define SEPARATE_PLS " -+- "
#define SEPARATE     " --- "
#define SEPARATE_CNT "  +- "
#define SEPARATE_VT  "  |  "

void CStructureDlg::OnButtonDumpHierarchy() 
{
	CWaitCursor cursor;

	int idx = m_comboStructure.GetCurSel();
	CString name;
	m_comboStructure.GetLBText(idx, name);
	CGDSStructure* structure = m_gdsGDS->FindStructure(name.GetBuffer(0));
	if(!structure)
		return;

	CString edit = name;

	MakeTreeText(structure, 0, "", &edit);

	if(!m_bTextDumpDlg){
		m_pTextDumpDlg = new CTextDumpDlg;
		m_pTextDumpDlg->SetEnabler(&m_bTextDumpDlg);
		m_pTextDumpDlg->Create(IDD_TEXT_DUMP, this);
		m_bTextDumpDlg = TRUE;
	}
	CString title;
	title.Format("Hierarchy of %s", name);
	m_pTextDumpDlg->SetWindowText(title);
	m_pTextDumpDlg->ShowDump(edit);
}


void CStructureDlg::MakeTreeText(CGDSStructure* pStr, int level, CString header, CString* edit)
{
	CString strname;
	BOOL init = TRUE;
	CObArray* refstructures =  pStr->GetRefStructureList();
	
	if(refstructures->GetSize() == 0){
		*edit += NEWLINE;
		return;
	}

	CString fmt;
	fmt.Format("%%%ds", strlen(pStr->GetStructureName()));
	CString buf;
	buf.Format(fmt, "");
	header += buf + SEPARATE_VT;


	for(int idx = 0;idx < refstructures->GetSize();idx++){
		CGDSRefStructure* ref = (CGDSRefStructure*)refstructures->GetAt(idx);
		CGDSRefStructure* reftmp = NULL;
		if(idx < refstructures->GetSize() - 1)
			reftmp = (CGDSRefStructure*)refstructures->GetAt(idx + 1);
		if(!reftmp){
			header = header.Left(header.GetLength() - strlen(SEPARATE));
			CString fmt;
			fmt.Format("%%%ds", strlen(SEPARATE));
			CString buf;
			buf.Format(fmt, "");
			header += buf;
		}
		CGDSStructure* structure = ref->GetStructure();
		strname = structure->GetStructureName();
		int count = pStr->GetNumRefStructure();
		if(!init){
			*edit += header.Left(header.GetLength() - strlen(SEPARATE));
			*edit += SEPARATE_CNT;
		}
		else{
			if(count == 1)
				*edit += SEPARATE;
			else if(count > 1)
				*edit += SEPARATE_PLS;

		}
		init = FALSE;
		*edit += strname;
		MakeTreeText(structure, level + 1, header, edit);
	}
}

