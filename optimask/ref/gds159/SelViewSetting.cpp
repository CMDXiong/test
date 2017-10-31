#include "stdafx.h"
#include "gds.h"
#include "SelViewSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////


CSelViewSetting::CSelViewSetting(CWnd* pParent)
	: CDialog(CSelViewSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelViewSetting)
	//}}AFX_DATA_INIT
}


void CSelViewSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelViewSetting)
	DDX_Control(pDX, IDC_SCROLL_VIEW_LIST, m_scrList);
	DDX_Control(pDX, IDC_VIEW_LIST_Y, m_lbViewListY);
	DDX_Control(pDX, IDC_VIEW_LIST_X, m_lbViewListX);
	DDX_Control(pDX, IDC_VIEW_LIST, m_lbViewList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelViewSetting, CDialog)
	//{{AFX_MSG_MAP(CSelViewSetting)
	ON_BN_CLICKED(IDC_DELETE_VIEW, OnDeleteView)
	ON_BN_CLICKED(IDC_SELECT_VIEW, OnSelectView)
	ON_BN_CLICKED(IDC_EDIT_VIEW, OnEditView)
	ON_WM_SHOWWINDOW()
	ON_LBN_SELCHANGE(IDC_VIEW_LIST, OnSelchangeViewList)
	ON_LBN_SELCHANGE(IDC_VIEW_LIST_X, OnSelchangeViewListX)
	ON_LBN_SELCHANGE(IDC_VIEW_LIST_Y, OnSelchangeViewListY)
	ON_LBN_DBLCLK(IDC_VIEW_LIST, OnDblclkViewList)
	ON_LBN_DBLCLK(IDC_VIEW_LIST_X, OnDblclkViewListX)
	ON_LBN_DBLCLK(IDC_VIEW_LIST_Y, OnDblclkViewListY)
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_CLEAR_VIEWS, OnClearViews)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CSelViewSetting::PostNcDestroy() 
{
	delete this;
	//CDialog::PostNcDestroy();
}

#include "ViewSetCommentDlg.h"

void CSelViewSetting::OnEditView() 
{
	int idx = m_lbViewList.GetCurSel();
	if(idx < 0)
		return;
	CViewSetCommentDlg dlg;
	CObArray& array = m_pCaller->GetViewSetting();
	CViewSetting* pSet = (CViewSetting*)array.GetAt(idx);
	dlg.SetComment(pSet->m_strComment);
	if(dlg.DoModal() == IDCANCEL)
		return;
	pSet->m_strComment = dlg.GetComment();
	m_lbViewList.InsertString(idx, pSet->m_strComment);
	m_lbViewList.DeleteString(idx + 1);
	UpdateData(FALSE);
	ChangeSelection(idx);
}

void CSelViewSetting::FillParam()
{
	for(int i = m_lbViewList.GetCount() - 1;i > 0;i--){
		m_lbViewList.DeleteString(i);
		m_lbViewListX.DeleteString(i);
		m_lbViewListY.DeleteString(i);
	}

	CObArray& array = m_pCaller->GetViewSetting();
	for(i = 0;i < array.GetSize();i++){
		CViewSetting* pSet = (CViewSetting*)array.GetAt(i);
		CString str;
		m_lbViewList.AddString(pSet->m_strComment);
		str.Format("%.3f", pSet->m_coordCenter.x * 1e+6);
		m_lbViewListX.AddString(str);
		str.Format("%.3f", pSet->m_coordCenter.y * 1e+6);
		m_lbViewListY.AddString(str);
	}

	SCROLLINFO info;
	m_lbViewList.GetScrollInfo(SB_VERT, &info);
	info.cbSize = sizeof(info);
	info.fMask = SIF_PAGE | SIF_RANGE;
	info.nMin = 0;
	info.nMax = array.GetSize();
	//info.nPage = m_lbViewList.Get;
	m_scrList.SetScrollInfo(&info);
	m_scrList.EnableScrollBar(ESB_ENABLE_BOTH);
}

void CSelViewSetting::AddParam()
{
	CObArray& array = m_pCaller->GetViewSetting();
	CViewSetting* pSet = (CViewSetting*)array.GetAt(array.GetSize() - 1);
	CString str;
	m_lbViewList.AddString(pSet->m_strComment);
	str.Format("%.3f", pSet->m_coordCenter.x * 1e+6);
	m_lbViewListX.AddString(str);
	str.Format("%.3f", pSet->m_coordCenter.y * 1e+6);
	m_lbViewListY.AddString(str);

	SCROLLINFO info;
	m_lbViewList.GetScrollInfo(SB_VERT, &info);
	info.cbSize = sizeof(info);
	info.fMask = SIF_PAGE | SIF_RANGE;
	info.nMin = 0;
	info.nMax = array.GetSize();
	//info.nPage = m_lbViewList.Get;
	m_scrList.SetScrollInfo(&info);
	m_scrList.EnableScrollBar(ESB_ENABLE_BOTH);

	ChangeSelection(m_lbViewList.GetCurSel());
}

void CSelViewSetting::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	FillParam();
	m_lbViewList.ShowScrollBar(SB_VERT, FALSE);
	m_lbViewList.EnableScrollBar(SB_VERT, ESB_DISABLE_BOTH);
}

void CSelViewSetting::OnSelchangeViewList() 
{
	UpdateData(TRUE);
	ChangeSelection(m_lbViewList.GetCurSel());
}

void CSelViewSetting::OnSelchangeViewListX() 
{
	UpdateData(TRUE);
	ChangeSelection(m_lbViewListX.GetCurSel());
}

void CSelViewSetting::OnSelchangeViewListY() 
{
	UpdateData(TRUE);
	ChangeSelection(m_lbViewListY.GetCurSel());
}

void CSelViewSetting::ChangeSelection(int nth, BOOL select)
{
	m_lbViewList.ShowScrollBar(SB_VERT, FALSE);
	if(nth < 0)
		return;
	m_lbViewList.SetCurSel(nth);
	m_lbViewListX.SetCurSel(nth);
	m_lbViewListY.SetCurSel(nth);
	int top = m_lbViewList.GetTopIndex();
	m_scrList.SetScrollPos(top);
	if(!select)
		return;
	m_pCaller->RestoreView(nth);
}

void CSelViewSetting::OnDeleteView() 
{
	int idx = m_lbViewList.GetCurSel();
	if(idx < 0)
		return;
	m_pCaller->DeleteView(idx);
	m_lbViewList.DeleteString(idx);
	m_lbViewListX.DeleteString(idx);
	m_lbViewListY.DeleteString(idx);
	if(m_lbViewList.GetCount() > idx)
		ChangeSelection(idx, TRUE);
	else if(m_lbViewList.GetCount() > 0)
		ChangeSelection(--idx, TRUE);

	m_scrList.SetScrollRange(0, m_lbViewList.GetCount());


	UpdateData(FALSE);
}

void CSelViewSetting::OnClearViews() 
{
	m_pCaller->DeleteAllView();
	m_lbViewList.ResetContent();
	m_lbViewListX.ResetContent();
	m_lbViewListY.ResetContent();

	m_scrList.SetScrollRange(0, 0);

	UpdateData(FALSE);
}

void CSelViewSetting::OnDblclkViewList() 
{
	UpdateData(TRUE);
	ChangeSelection(m_lbViewList.GetCurSel(), TRUE);
}

void CSelViewSetting::OnDblclkViewListX() 
{
	UpdateData(TRUE);
	ChangeSelection(m_lbViewListX.GetCurSel(), TRUE);
}

void CSelViewSetting::OnDblclkViewListY() 
{
	UpdateData(TRUE);
	ChangeSelection(m_lbViewListY.GetCurSel(), TRUE);
}

void CSelViewSetting::OnSelectView() 
{
	UpdateData(TRUE);
	ChangeSelection(m_lbViewList.GetCurSel(), TRUE);
}

void CSelViewSetting::ScrollList()
{
	int idx = m_scrList.GetScrollPos();
	m_lbViewList.SetTopIndex(idx);
	m_lbViewListX.SetTopIndex(idx);
	m_lbViewListY.SetTopIndex(idx);
	m_lbViewList.ShowScrollBar(SB_VERT, FALSE);
}


BOOL CSelViewSetting::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	UINT step;
	if(!SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &step, 0))
		step = 1;
	int min, max;
	int nPos = m_scrList.GetScrollPos();
	m_scrList.GetScrollRange(&min, &max);

	if(zDelta < 0 && nPos < max)
		nPos += step;
	if(zDelta > 0 && nPos > min)
		nPos -= step;

	m_scrList.SetScrollPos(nPos);
	ScrollList();
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CSelViewSetting::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SCROLLINFO info;
	m_lbViewList.GetScrollInfo(SB_VERT, &info);

	switch(nSBCode){
	case SB_LINEUP:
		nPos = m_scrList.GetScrollPos() - 1;
		break;
	case SB_LINEDOWN:
		nPos = m_scrList.GetScrollPos() + 1;
		break;
	case SB_PAGEUP:
		nPos = m_scrList.GetScrollPos() - info.nPage;
		break;
	case SB_PAGEDOWN:
		nPos = m_scrList.GetScrollPos() + info.nPage;
		break;
	case SB_TOP:
		break;
	case SB_BOTTOM:
		break;
	case SB_THUMBPOSITION:
		break;
	case SB_THUMBTRACK:
		break;
	default:
		return;
		break;
	}
	m_scrList.SetScrollPos(nPos);
	ScrollList();

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

