#include "stdafx.h"
#include "gds.h"
#include "DlgLayer.h"
#include "GDShcy.h"
#include "LayerInfo.h"
#include "GdsView.h"
#include "FillPatDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgLayer::CDlgLayer(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLayer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLayer)
	m_intDensity = 0;
	m_strTecFileName = _T("");
	m_bHideNonExisting = TRUE;
	m_bSaveNonExisting = TRUE;
	m_strNumLayer = _T("");
	m_strEditLayerName = _T("");
	//}}AFX_DATA_INIT
	m_pEnableSwitch = NULL;
	m_pColorColorList = NULL;
	m_pIntFillList = NULL;
	m_pBShowList = NULL;
	m_pIntOrderList = NULL;
	m_pStrNameList = NULL;
	m_pBoolLayerExistList = NULL;

	m_pDragImage = NULL;
	m_pCaller = NULL;
	m_intNumLayer = 0;
	m_pLayerInEdit = NULL;
	m_bDragging = FALSE;
	m_intDragTo = -1;
}

CDlgLayer::~CDlgLayer()
{
}

void CDlgLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLayer)
	DDX_Control(pDX, IDC_EDIT_LAYER_NAME, m_editLayerName);
	DDX_Control(pDX, IDC_LAYER_LIST, m_listCtrlLayer);
	DDX_Control(pDX, IDC_CHECK_HIDE_NONEXIST, m_ctrlHideNonExist);
	DDX_Control(pDX, IDC_CHECK_SAVE_NONEXIST, m_ctrlSaveNonExist);
	DDX_Control(pDX, IDC_EDIT_TEC_FILE_NAME, m_editTecFileName);
	DDX_Control(pDX, IDC_SLIDER_DENSITY, m_sliderDensity);
	DDX_Slider(pDX, IDC_SLIDER_DENSITY, m_intDensity);
	DDX_Text(pDX, IDC_EDIT_TEC_FILE_NAME, m_strTecFileName);
	DDX_Check(pDX, IDC_CHECK_HIDE_NONEXIST, m_bHideNonExisting);
	DDX_Check(pDX, IDC_CHECK_SAVE_NONEXIST, m_bSaveNonExisting);
	DDX_Text(pDX, IDC_NUM_LAYER, m_strNumLayer);
	DDX_Text(pDX, IDC_EDIT_LAYER_NAME, m_strEditLayerName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLayer, CDialog)
	//{{AFX_MSG_MAP(CDlgLayer)
	ON_BN_CLICKED(IDC_ALL_OFF, OnAllOff)
	ON_BN_CLICKED(IDC_ALL_ON, OnAllOn)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_SAVE_LAYER, OnSaveLayer)
	ON_BN_CLICKED(IDC_LOAD_LAYER, OnLoadLayer)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_DENSITY, OnCustomdrawSliderDensity)
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_CHECK_HIDE_NONEXIST, OnCheckHideNonexist)
	ON_BN_CLICKED(IDC_LAYER_APPLY, OnLayerApply)
	ON_NOTIFY(NM_CLICK, IDC_LAYER_LIST, OnClickLayerList)
	ON_EN_CHANGE(IDC_EDIT_LAYER_NAME, OnChangeEditLayerName)
	ON_BN_CLICKED(ID_CLOSE_LAYER_SETUP, OnCloseLayerSetup)
	ON_EN_KILLFOCUS(IDC_EDIT_LAYER_NAME, OnKillfocusEditLayerName)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LAYER_LIST, OnBegindragLayerList)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_LAYER_LIST, OnCustomdrawList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////


void CDlgLayer::OnAllOn() 
{
	for(int i = 0;i < GDS_MAX_LAYER;i++){
		int idx = (m_arrayLayers + i)->m_intNumber;
		if(!m_bHideNonExisting || (m_bHideNonExisting && 
			(*(m_bLayerInTec + idx) || (m_arrayLayers + i)->m_bValid))){
			(m_arrayLayers + i)->m_bShow = TRUE;
		}
	}
	UpdateItemCheck();
}

void CDlgLayer::OnAllOff() 
{
	for(int i = 0;i < GDS_MAX_LAYER;i++){
		int idx = (m_arrayLayers + i)->m_intNumber;
		if(!m_bHideNonExisting || (m_bHideNonExisting && 
			(*(m_bLayerInTec + idx) || (m_arrayLayers + i)->m_bValid))){
			(m_arrayLayers + i)->m_bShow = FALSE;
		}
	}
	UpdateItemCheck();
}


void CDlgLayer::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	m_editTecFileName.SetSel(0, -1);
	m_editTecFileName.SetSel(-1, -1);

	CheckAdditionalLayer();
}

void CDlgLayer::ShowPage()
{
	UpdateData(TRUE);


	m_sliderDensity.SetPos(m_intDensity);

	m_strNumLayer.Format("GDSII contains %d layers", m_intNumLayer);

	UpdateData(FALSE);

	m_listCtrlLayer.RedrawItems(0, m_listCtrlLayer.GetItemCount() - 1);
}




void CDlgLayer::OnSaveLayer() 
{
	UpdateData(TRUE);
	m_layerInfo.SetTecFilePath(m_strTecFileName);
	if(m_bSaveNonExisting)
		m_layerInfo.SaveLayerSetup(m_arrayLayers, "", m_bLayerInTec);
	else{
		BOOL save[GDS_MAX_LAYER];
		for(int i = 0;i < GDS_MAX_LAYER;i++)
			*(save + i) = FALSE;

		for(i = 0;i < GDS_MAX_LAYER;i++){
			int layer = (m_arrayLayers + i)->m_intNumber;
			if((m_arrayLayers + i)->m_bValid)
				*(save + layer) = TRUE;
		}
		m_layerInfo.SaveLayerSetup(m_arrayLayers, "", save);
	}
	m_strTecFileName = m_layerInfo.GetTecFilePath();
	UpdateData(FALSE);
	m_editTecFileName.SetSel(0, -1);
	m_editTecFileName.SetSel(-1, -1);
}

void CDlgLayer::OnLoadLayer() 
{
	BOOL valid[GDS_MAX_LAYER];
	for(int i = 0;i < GDS_MAX_LAYER;i++){
		CLayerSet* pSet = m_arrayLayers + i;
		int layer = pSet->m_intNumber;
		*(valid + layer) = pSet->m_bValid;
		*(m_bLayerInTec + layer) = FALSE;
	}

	if(!m_layerInfo.ReadLayerSetup(m_arrayLayers))
		return;

	m_intNumLayerTec = 0;
	for(i = 0;i < GDS_MAX_LAYER;i++){
		CLayerSet* pSet = m_arrayLayers + i;
		int layer = pSet->m_intNumber;

		if(! *(m_bLayerInTec + layer) && pSet->m_bValid){
			m_intNumLayerTec++;
			*(m_bLayerInTec + layer) = TRUE;
		}
		pSet->m_bValid = *(valid + layer);
	}

	Sort();

	m_strTecFileName = m_layerInfo.GetTecFilePath();

	CheckAdditionalLayer();
	MakeListControl();
	UpdateData(FALSE);
	m_editTecFileName.SetSel(0, -1);
	m_editTecFileName.SetSel(-1, -1);
}

BOOL CDlgLayer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	UINT step;
	if(!SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &step, 0))
		step = 1;

	int min, max;
	int nPos = m_listCtrlLayer.GetScrollPos(SB_VERT);
	m_listCtrlLayer.GetScrollRange(SB_VERT, &min, &max);

	int pagesize = m_listCtrlLayer.GetCountPerPage( );

	if(zDelta < 0 && nPos < max){
		nPos += step;
		m_listCtrlLayer.EnsureVisible(nPos + pagesize - 1, FALSE);
	}
	if(zDelta > 0 && nPos > min){
		nPos -= step;
		m_listCtrlLayer.EnsureVisible(nPos, FALSE);
	}

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}


void CDlgLayer::OnApply()
{
	UpdateData(TRUE);
	for(int i = 0;i < GDS_MAX_LAYER;i++){
		int idx = (m_arrayLayers + i)->m_intNumber;
		*(m_pStrNameList + idx) = (m_arrayLayers + i)->m_strName;
		*(m_pBShowList + idx) = (m_arrayLayers + i)->m_bShow;
		*(m_pColorColorList + idx) = (m_arrayLayers + i)->m_color;
		*(m_pIntFillList + idx) = (m_arrayLayers + i)->m_intFill;
		*(m_pIntOrderList + i) = -1;
	}
	int idx = 0;
	for(i = 0;i < GDS_MAX_LAYER;i++){
		*(m_pBoolLayerInTecList + i) = *(m_bLayerInTec + i); 
		*(m_pIntOrderList + idx++) = (m_arrayLayers + i)->m_intNumber;
	}
	m_pCaller->UpdateLayer();
}

static int sort_to_user_order(const void *arg1, const void *arg2 )
{
	CLayerSet* cmp1 = (CLayerSet*)arg1;
	CLayerSet* cmp2 = (CLayerSet*)arg2;


	// priority 1 : layer order defined in tec file
	if(cmp1->m_intOrder < 0 && cmp2->m_intOrder >= 0)
		return +1;
	if(cmp2->m_intOrder < 0 && cmp1->m_intOrder >= 0)
		return -1;
	if(cmp1->m_intOrder >= 0 && cmp2->m_intOrder >= 0){
		if(cmp1->m_intOrder > cmp2->m_intOrder)
			return +1;
		if(cmp1->m_intOrder < cmp2->m_intOrder)
			return -1;
	}

	// priority 2 : layer exist
    if(cmp1->m_bValid && !(cmp2->m_bValid))
		return -1;
	if(!(cmp1->m_bValid) && cmp2->m_bValid)
		return +1;

	// priority 3 : layer number
	if(cmp1->m_intNumber > cmp2->m_intNumber)
		return +1;
	if(cmp1->m_intNumber < cmp2->m_intNumber)
		return -1;
	return 0;
}

void CDlgLayer::Sort()
{
	size_t len = sizeof(m_arrayLayers)/sizeof(CLayerSet);
	size_t spacing = sizeof(CLayerSet);

	qsort((void*)m_arrayLayers, len, spacing, sort_to_user_order);
}



void CDlgLayer::SetLayerAndColor(int* order, CString* names, 
								 BOOL* stat, COLORREF* colors, int* fill, BOOL* exist, BOOL* tec)
{
	m_intNumLayerTec = 0;
	m_intNumLayer = 0;
	for(int i = 0;i < GDS_MAX_LAYER;i++){
		*(m_bLayerInTec + i) = *(tec + i);

		(m_arrayLayers + i)->m_strName = *(names + i);
		(m_arrayLayers + i)->m_bShow = *(stat + i);
		(m_arrayLayers + i)->m_color = *(colors + i);
		(m_arrayLayers + i)->m_intFill = *(fill + i);
		(m_arrayLayers + i)->m_intNumber = i;
		(m_arrayLayers + i)->m_intOrder = -1;
		(m_arrayLayers + i)->m_bValid = *(exist + i);
		if((m_arrayLayers + i)->m_bValid){
			m_intNumLayer++;
			m_intNumLayerTec++;
		}
		else if(*(tec + i))
			m_intNumLayerTec++;
	}
	int idx = 0;
	for(i = 0;i < GDS_MAX_LAYER;i++){
		int layer = *(order + i);
		if(layer >= 0)
			(m_arrayLayers + layer)->m_intOrder = idx++;
	}
	Sort();

	m_pStrNameList = names;
	m_pBShowList = stat;
	m_pColorColorList = colors;
	m_pIntFillList = fill;
	m_pIntOrderList = order;
	m_pBoolLayerExistList = exist;
	m_pBoolLayerInTecList = tec;
}



BOOL CDlgLayer::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_sliderDensity.SetRange(0, 100);
	m_intDensity = m_sliderDensity.GetPos();

	m_editLayerName.EnableWindow(FALSE);
	m_editLayerName.ShowWindow(SW_HIDE);

	m_listCtrlLayer.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_UNDERLINECOLD );


	MakeListControl();

	WINDOWPLACEMENT place;
	place.length = sizeof(place);
	GetWindowPlacement(&place);
	m_intWindowWidth = place.rcNormalPosition.right - place.rcNormalPosition.left;

	int height = -place.rcNormalPosition.top +place.rcNormalPosition.bottom;

	CGdsApp* app = (CGdsApp*)AfxGetApp();
	int height_store = app->GetProfileInt("Settings", "LayerDialogHeight", 0);

	CRect rect;
	GetClientRect(&rect);
	m_intClientWidth = rect.Width();

	if(height_store > 200){
		height = min(GetSystemMetrics(SM_CYSCREEN), height_store);
		if(place.rcNormalPosition.top + height > GetSystemMetrics(SM_CYSCREEN))
			place.rcNormalPosition.top -= place.rcNormalPosition.top + height - GetSystemMetrics(SM_CYSCREEN);
        SetWindowPos(0, place.rcNormalPosition.left, place.rcNormalPosition.top, 
			m_intWindowWidth, height, SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	return TRUE;
}

void CDlgLayer::MakeListControl()
{
	m_pLayerInEdit = NULL;

	m_listCtrlLayer.DeleteAllItems();

	m_listCtrlLayer.DeleteColumn(3);
	m_listCtrlLayer.DeleteColumn(2);
	m_listCtrlLayer.DeleteColumn(1);
	m_listCtrlLayer.DeleteColumn(0);


	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Layer number";
	lvc.cx = m_listCtrlLayer.GetStringWidth("BBBBBBBB");
	lvc.fmt = LVCFMT_CENTER;
	m_listCtrlLayer.InsertColumn(lvc.iSubItem,&lvc);

	lvc.iSubItem = 1;
	lvc.pszText = "Color";
	lvc.cx = m_listCtrlLayer.GetStringWidth("BBBBB");
	lvc.fmt = LVCFMT_CENTER;
	m_listCtrlLayer.InsertColumn(lvc.iSubItem,&lvc);

	lvc.iSubItem = 2;
	lvc.pszText = "Fill";
	lvc.cx = m_listCtrlLayer.GetStringWidth("BBBBB");
	lvc.fmt = LVCFMT_CENTER;
	m_listCtrlLayer.InsertColumn(lvc.iSubItem,&lvc);


	lvc.iSubItem = 3;
	lvc.pszText = "Name";
	lvc.cx = m_listCtrlLayer.GetStringWidth("BBBBBBBB");
	lvc.fmt = LVCFMT_LEFT;
	m_listCtrlLayer.InsertColumn(lvc.iSubItem,&lvc);

	
	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_STATE ;
	char buf[128];
	int idx = 0;
	for(int i = 0;i < GDS_MAX_LAYER;i++){
		if(!(m_arrayLayers + i)->m_bValid && !*(m_bLayerInTec + (m_arrayLayers + i)->m_intNumber))
			continue;
		if(!(m_arrayLayers + i)->m_bValid && m_bHideNonExisting)
			continue;

		sprintf(buf, "%4d", (m_arrayLayers + i)->m_intNumber);
		m_listCtrlLayer.InsertItem(LVIF_PARAM | LVIF_TEXT, idx, buf, 0, 0, 0, (DWORD)(m_arrayLayers + i));

		sprintf(buf, "%s", (m_arrayLayers + i)->m_strName);
		m_listCtrlLayer.SetItemText(idx, 3, buf);
		idx++;
	}

	UpdateItemCheck();

	AdjustWindowSize();
}

void CDlgLayer::AdjustWindowSize()
{
	CRect rect;
	m_listCtrlLayer.GetClientRect(&rect);

	CRect rect0;
	m_listCtrlLayer.GetItemRect(0, &rect0, LVIR_BOUNDS);

	if(rect.right > rect0.right){
		int width = m_listCtrlLayer.GetColumnWidth(3);
		m_listCtrlLayer.SetColumnWidth(3, width + rect.right - rect0.right);
	}
	m_editTecFileName.SetSel(0, -1);
	m_editTecFileName.SetSel(-1, -1);

	m_editLayerName.EnableWindow(FALSE);
	m_editLayerName.ShowWindow(SW_HIDE);
	m_listCtrlLayer.SetFocus();
}


void CDlgLayer::OnClickLayerList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnKillfocusEditLayerName();
	*pResult = 0;

	NMLISTVIEW* pListView = (NM_LISTVIEW*)pNMHDR;

	LVHITTESTINFO hitinfo;
	hitinfo.pt = pListView->ptAction;

	int item = m_listCtrlLayer.HitTest(&hitinfo); 

	if(item >= 0){
		LVITEM rItem;
		ZeroMemory(&rItem, sizeof(LVITEM));
		rItem.mask = LVIF_PARAM;
		rItem.iItem = item; // row
		rItem.iSubItem = 0;  // column
		m_listCtrlLayer.GetItem(&rItem);
		CLayerSet* pLayer = (CLayerSet*)rItem.lParam;
		if(!pLayer)
			return;


		if((hitinfo.flags & LVHT_ONITEMSTATEICON) > 0){
			pLayer->m_bShow ^= TRUE;
			return;
		}
	}


	item = m_listCtrlLayer.SubItemHitTest(&hitinfo); 
	if(item < 0)
		return;
	// clicked on subitem
	LVITEM rItem;
	ZeroMemory(&rItem, sizeof(LVITEM));
	rItem.mask = LVIF_PARAM;
	rItem.iItem = item; // row
	rItem.iSubItem = 0;  // column
	m_listCtrlLayer.GetItem(&rItem);
	CLayerSet* pLayer = (CLayerSet*)rItem.lParam;
	if(!pLayer)
		return;


	CRect rect;
	m_listCtrlLayer.GetSubItemRect(item, 1, LVIR_BOUNDS, rect);
	if(rect.PtInRect(hitinfo.pt)){ // layer color
		CColorDialog dlg(pLayer->m_color);
		if(dlg.DoModal() == IDOK){
			pLayer->m_color = dlg.GetColor();
			ShowPage();
		}
		m_listCtrlLayer.SetFocus();
		return;
	}

	m_listCtrlLayer.GetSubItemRect(item, 2, LVIR_BOUNDS, rect);
	if(rect.PtInRect(hitinfo.pt)){ // layer fill pattern
		CFillPatDialog dlg;
		dlg.SetLayerColor(pLayer->m_color);
		dlg.SetBkColor(((CGdsView*)m_pCaller)->GetBkColor());
		if(dlg.DoModal() == IDOK){
			pLayer->m_intFill = dlg.GetPattern();
			ShowPage();
		}
		m_listCtrlLayer.SetFocus();
		return;
	}

	m_listCtrlLayer.GetSubItemRect(item, 3, LVIR_BOUNDS, rect);
	if(rect.PtInRect(hitinfo.pt)){ // layer name
		int x = GetSystemMetrics(SM_CXEDGE);
		int y = GetSystemMetrics(SM_CYEDGE);
		WINDOWPLACEMENT place;
		place.length = sizeof(place);
		m_listCtrlLayer.GetWindowPlacement(&place);
		m_editLayerName.SetWindowPos(0,  place.rcNormalPosition.left + rect.left + x,  place.rcNormalPosition.top + rect.top + y,
			rect.Width(), rect.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);

		m_strEditLayerName = pLayer->m_strName;
		UpdateData(FALSE);
		m_editLayerName.EnableWindow(TRUE);
		m_editLayerName.ShowWindow(SW_SHOW);
		m_editLayerName.BringWindowToTop();
		m_editLayerName.SetFocus();
		m_editLayerName.SetSel(0, -1);

		m_pLayerInEdit = pLayer;

		//m_editLayerName.EnableWindow(FALSE);
		//m_editLayerName.ShowWindow(SW_HIDE);
		//m_listCtrlLayer.SetFocus();
		return;
	}
}

void CDlgLayer::OnChangeEditLayerName() 
{
	if(!m_pLayerInEdit)
		return;
	UpdateData();

	m_pLayerInEdit->m_strName = m_strEditLayerName;

	LVFINDINFO findinfo;
	ZeroMemory(&findinfo, sizeof(findinfo));
	findinfo.flags = LVFI_PARAM;
	findinfo.lParam = (LPARAM)m_pLayerInEdit;
	int idx = m_listCtrlLayer.FindItem(&findinfo, -1);
	if(idx >= 0)
		m_listCtrlLayer.SetItemText(idx, 3, m_strEditLayerName);
}

void CDlgLayer::OnKillfocusEditLayerName() 
{
	m_editLayerName.EnableWindow(FALSE);
	m_editLayerName.ShowWindow(SW_HIDE);

	m_pLayerInEdit = NULL;
}

void CDlgLayer::OnCustomdrawList( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMLVCUSTOMDRAW* pLVCD = (NMLVCUSTOMDRAW*)pNMHDR;
    
	*pResult = 0;
	if(pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT){
		*pResult = CDRF_NOTIFYITEMDRAW;
		return;
	}
	if(pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT){
		*pResult = CDRF_NOTIFYPOSTPAINT;
		return;
	}
	if(pLVCD->nmcd.dwDrawStage != CDDS_ITEMPOSTPAINT)
		return;

	*pResult = CDRF_SKIPDEFAULT; // done

	int nItem = (int)pLVCD->nmcd.dwItemSpec;

	char buf[128];

	LVITEM rItem;
	ZeroMemory( &rItem, sizeof(LVITEM) );
	rItem.mask = LVIF_TEXT | LVIF_PARAM;
	rItem.iItem = nItem; // row
	rItem.iSubItem = 0;  // column
	rItem.pszText = buf;
	rItem.cchTextMax = sizeof(buf);
	m_listCtrlLayer.GetItem( &rItem );

	CLayerSet* pLayer = (CLayerSet*)rItem.lParam;

	CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);

	COLORREF color = pLayer->m_color;
	CRect rect;
	m_listCtrlLayer.GetSubItemRect(nItem, 1, LVIR_BOUNDS, rect);
	rect.DeflateRect(1,1);
	pDC->FillSolidRect (rect, color);


	m_listCtrlLayer.GetSubItemRect(nItem, 2, LVIR_BOUNDS, rect);
	rect.DeflateRect(1,1);
	FillPattern(pDC, rect, pLayer);
}

void CDlgLayer::FillPattern(CDC* pDC, CRect rect, CLayerSet* pLayer)
{
	int fill;
	COLORREF color;
	if((pLayer->m_bValid || (!m_bHideNonExisting && *(m_bLayerInTec + pLayer->m_intNumber)))){
		fill = pLayer->m_intFill;
		color = pLayer->m_color;
	}
	else{
		fill = FILL_BMP_NONE ;
		color = GetSysColor(COLOR_WINDOW);
	}

	CBitmap bmp;
	BOOL anyBmp = TRUE;
	switch(fill){
	case FILL_BMP_XDIAG :
		bmp.LoadBitmap(IDB_XDIAG);
		break;
	case FILL_BMP_FDIAG :
		bmp.LoadBitmap(IDB_FDIAG);
		break;
	case FILL_BMP_RDIAG :
		bmp.LoadBitmap(IDB_RDIAG);
		break;
	case FILL_BMP_XHATCH :
		bmp.LoadBitmap(IDB_XHATCH);
		break;
	case FILL_BMP_VHATCH:
		bmp.LoadBitmap(IDB_VHATCH);
		break;
	case FILL_BMP_HHATCH :
		bmp.LoadBitmap(IDB_HHATCH);
		break;
	case FILL_BMP_HZIGZAG:
		bmp.LoadBitmap(IDB_HZIGZAG);
		break;
	case FILL_BMP_VZIGZAG:
		bmp.LoadBitmap(IDB_VZIGZAG);
		break;
	case FILL_BMP_SOLID:
		bmp.LoadBitmap(IDB_SOLID);
		break;
	case FILL_BMP_NONE:
		bmp.LoadBitmap(IDB_FRAME);
		break;
	case FILL_BMP_HDASH:
		bmp.LoadBitmap(IDB_HDASH);
		break;
	case FILL_BMP_VDASH:
		bmp.LoadBitmap(IDB_VDASH);
		break;
	case FILL_BMP_VWAVE:
		bmp.LoadBitmap(IDB_VWAVE);
		break;
	case FILL_BMP_HWAVE:
		bmp.LoadBitmap(IDB_HWAVE);
		break;
	case FILL_BMP_LIGHT:
		bmp.LoadBitmap(IDB_LIGHT);
		break;
	case FILL_BMP_FDIAG_DASH:
		bmp.LoadBitmap(IDB_FDIAG_DASH);
		break;
	case FILL_BMP_RDIAG_DASH:
		bmp.LoadBitmap(IDB_RDIAG_DASH);
		break;
	case FILL_BMP_XHATCH_HD:
		bmp.LoadBitmap(IDB_DIA);
		break;
	default:
		anyBmp = FALSE;
		break;
	}
    color = GetSysColor(COLOR_WINDOW);

	Graphics g(pDC->m_hDC);
	Pen pen(Color(255, 0, 0, 0));
	g.DrawRectangle(&pen, rect.left, rect.top, rect.Width() - 1, rect.Height() - 1);
	if(!anyBmp)
		return;

	CRect one(0, 0, 1, 1);
	pDC->DPtoLP(&one);
	one.NormalizeRect();
	REAL scale = (REAL)(one.Width() * (1.0 - m_intDensity / HATCH_DENSITY_DIV));
	g.SetPageScale(1.0f);
	g.SetPageUnit(UnitPixel);
	g.SetSmoothingMode(SmoothingModeAntiAlias);

	BITMAP bmpobj;
	bmp.GetObject(sizeof(bmpobj), &bmpobj);
	int count = bmpobj.bmWidthBytes * bmpobj.bmHeight;
	BYTE* bmpary = new BYTE[count];
	bmp.GetBitmapBits(count, bmpary);
	BitmapData bitmapData;

	Bitmap bitmap(bmpobj.bmWidth, bmpobj.bmHeight, &g);
	Rect rct(0, 0, bmpobj.bmWidth, bmpobj.bmHeight);
	bitmap.LockBits(&rct, ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);
	UINT* pixels = (UINT*)bitmapData.Scan0;

	for(int row = 0; row < bmpobj.bmHeight; row++){
		for(int col = 0; col < bmpobj.bmWidth; col++){
		int row_pixel = bmpobj.bmHeight - row - 1;
		BOOL bit = *(bmpary +  bmpobj.bmWidthBytes * row + col / 8) & (0x80 >> col % 8);
		if(bit)
			pixels[row_pixel * bitmapData.Stride / 4 + col] = 
			0xff000000 | (GetRValue(color) << 16) | (GetGValue(color) << 8) | GetBValue(color);
		else
			pixels[row_pixel * bitmapData.Stride / 4 + col] = 0xff000000;
	  }
	}
	bitmap.UnlockBits(&bitmapData);
	delete [] bmpary;

	TextureBrush gbrush((&bitmap), WrapModeTile);
	gbrush.ScaleTransform(scale, -scale);

	g.FillRectangle(&gbrush, rect.left, rect.top, rect.Width() - 1, rect.Height() - 1);
	bmp.DeleteObject();
}

void CDlgLayer::OnCustomdrawSliderDensity(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);
	ShowPage();
	
	*pResult = 0;
}


void CDlgLayer::OnCheckHideNonexist() 
{
	UpdateData(TRUE);	
	UpdateData(FALSE);
	MakeListControl();
}


void CDlgLayer::CheckAdditionalLayer()
{
	BOOL saveNonExist = m_ctrlSaveNonExist.IsWindowEnabled();
	BOOL hideNonExist = m_ctrlHideNonExist.IsWindowEnabled();

	m_ctrlSaveNonExist.EnableWindow(FALSE);
	m_ctrlHideNonExist.EnableWindow(FALSE);

	for(int i = 0;i < GDS_MAX_LAYER;i++){
		CLayerSet* pSet = m_arrayLayers + i;
		int layer = pSet->m_intNumber;
		if(! pSet->m_bValid && *(m_bLayerInTec + layer)){
			m_ctrlSaveNonExist.EnableWindow(TRUE);
			m_ctrlHideNonExist.EnableWindow(TRUE);
			break;
		}
	}
	if(!m_ctrlSaveNonExist.IsWindowEnabled())
		m_bSaveNonExisting = FALSE;
	else if(!saveNonExist)
		m_bSaveNonExisting = TRUE;

	if(!m_ctrlHideNonExist.IsWindowEnabled())
		m_bHideNonExisting = FALSE;
	else if(!hideNonExist)
		m_bHideNonExisting = TRUE;

	UpdateData(FALSE);
}

void CDlgLayer::PostNcDestroy() 
{
	delete this;	
	//CDialog::PostNcDestroy();
}

void CDlgLayer::OnLayerApply() 
{
	OnApply();	
}

void CDlgLayer::OnCloseLayerSetup() 
{
	ExitDialog();	
}

void CDlgLayer::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
    if(IsWindow(m_listCtrlLayer)){
        WINDOWPLACEMENT place;
        place.length = sizeof(place);
		GetWindowPlacement(&place);
        SetWindowPos(0, place.rcNormalPosition.left, place.rcNormalPosition.top, 
			m_intWindowWidth, place.rcNormalPosition.bottom - place.rcNormalPosition.top, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);

		CGdsApp* app = (CGdsApp*)AfxGetApp();
		app->WriteProfileInt("Settings", "LayerDialogHeight", place.rcNormalPosition.bottom - place.rcNormalPosition.top);

		CRect rect;
		GetClientRect(&rect);

        m_listCtrlLayer.GetWindowPlacement(&place);
        m_listCtrlLayer.SetWindowPos(0, place.rcNormalPosition.left, place.rcNormalPosition.top, 
			m_intClientWidth - place.rcNormalPosition.left, cy - place.rcNormalPosition.top, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
		AdjustWindowSize();
    }
}

void CDlgLayer::UpdateItemCheck()
{

	for(int i = 0;i < GDS_MAX_LAYER;i++){
		LVFINDINFO findinfo;
		ZeroMemory(&findinfo, sizeof(findinfo));
		findinfo.flags = LVFI_PARAM;
		findinfo.lParam = (LPARAM)(m_arrayLayers + i);

		int idx = m_listCtrlLayer.FindItem(&findinfo, -1);
		if(idx >= 0)
			m_listCtrlLayer.SetCheck(idx, (m_arrayLayers + i)->m_bShow);
	}

}

BOOL CDlgLayer::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message){
	case WM_MOUSEMOVE:
		if(m_bDragging)
			DragItem();
		break;
	case WM_LBUTTONUP:
		if(m_bDragging)
			EndDragItem();
		break;
	default:
		break;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgLayer::OnBegindragLayerList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	CPoint pos = pNMListView->ptAction;
	m_bDragging = FALSE;
	m_intDragTo = -1;

	m_pointDrag = pos;
	if(pNMListView->iItem >= 0){
		m_intDragItem = pNMListView->iItem;
		if(m_pDragImage)
			delete m_pDragImage;

		//m_pDragImage = m_listCtrlLayer.CreateDragImage(pNMListView->iItem, &(pNMListView->ptAction));
		//if(m_pDragImage){
		//	m_pDragImage->BeginDrag(0, CPoint(0, 0));
		//	m_pDragImage->DragEnter(&m_listCtrlLayer, pos);
		//}

		m_bDragging = TRUE;
		m_listCtrlLayer.SetCapture();
		m_listCtrlLayer.SetExtendedStyle(m_listCtrlLayer.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	}
}



void CDlgLayer::DragItem()
{
	if(m_bDragging){
		CPoint point;
		GetCursorPos(&point);
		m_pointDrag = point;
		m_listCtrlLayer.ScreenToClient(&m_pointDrag);

		int to = m_listCtrlLayer.HitTest(m_pointDrag);
		if(to < 0){	
			LVHITTESTINFO hitinfo;
			hitinfo.pt = m_pointDrag;
			to = m_listCtrlLayer.SubItemHitTest(&hitinfo);
		}
		if(to != m_intDragTo){
			CRect rect;
			CClientDC dc(&m_listCtrlLayer);
			dc.SetROP2(R2_COPYPEN);
			CPen pen, *pOldPen;

			if(m_intDragTo >= 0){
				pen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW));
				pOldPen = dc.SelectObject(&pen);

				m_listCtrlLayer.GetItemRect(m_intDragTo, &rect, LVIR_BOUNDS);
				dc.MoveTo(rect.left, rect.bottom);
				dc.LineTo(rect.right, rect.bottom);
				pen.DeleteObject();
				dc.SelectObject(pOldPen);
			}
			if(to >= 0){
				pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
				pOldPen = dc.SelectObject(&pen);

				m_listCtrlLayer.GetItemRect(to, &rect, LVIR_BOUNDS);
				dc.MoveTo(rect.left, rect.bottom);
				dc.LineTo(rect.right, rect.bottom);
				pen.DeleteObject();
				dc.SelectObject(pOldPen);
			}
		}
		//m_pDragImage->DragMove(m_pointDrag);
		m_intDragTo = to;
	}
}

void CDlgLayer::EndDragItem()
{
	if(m_bDragging){
		CPoint point;
		GetCursorPos(&point);
		m_pointDrag = point;
		m_listCtrlLayer.ScreenToClient(&m_pointDrag);

		//m_pDragImage->DragLeave(&m_listCtrlLayer);
		//m_pDragImage->EndDrag();
		//delete m_pDragImage;
		//m_pDragImage = NULL;

		int itemTo = m_listCtrlLayer.HitTest(m_pointDrag);
		if(itemTo >= 0){
			CRect rect;
			CClientDC dc(&m_listCtrlLayer);
			dc.SetROP2(R2_COPYPEN);
			CPen pen, *pOldPen;

			pen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW));
			pOldPen = dc.SelectObject(&pen);

			m_listCtrlLayer.GetItemRect(m_intDragTo, &rect, LVIR_BOUNDS);
			dc.MoveTo(rect.left, rect.bottom);
			dc.LineTo(rect.right, rect.bottom);
			pen.DeleteObject();
			dc.SelectObject(pOldPen);

			m_listCtrlLayer.GetItemRect(itemTo, &rect, LVIR_BOUNDS);

			if(itemTo != m_intDragItem){
				if(itemTo < m_intDragItem)
					itemTo++;
				int start = min(m_intDragItem, itemTo);
				int end = max(m_intDragItem, itemTo);
				CLayerSet* pStart = (CLayerSet*)m_listCtrlLayer.GetItemData(start);
				CLayerSet* pEnd = (CLayerSet*)m_listCtrlLayer.GetItemData(end);

				int start_idx, end_idx;
				for(int i = 0;i < GDS_MAX_LAYER;i++){
					if((m_arrayLayers + i) == pStart)
						start_idx = i;
					if((m_arrayLayers + i) == pEnd)
						end_idx = i;
				}
				CLayerSet layerStore;
				if(itemTo > m_intDragItem){
					layerStore = *(m_arrayLayers + start_idx);
					for(i = start_idx;i < end_idx;i++)
						*(m_arrayLayers + i) = *(m_arrayLayers + i + 1);

					*pEnd = layerStore;
				}
				else{
					layerStore = *(m_arrayLayers + end_idx);
					for(i = end_idx;i > start_idx;i--)
						*(m_arrayLayers + i) = *(m_arrayLayers + i - 1);

					*pStart = layerStore;
				}

				int index = m_listCtrlLayer.GetTopIndex();
				int size = m_listCtrlLayer.GetCountPerPage();
				MakeListControl();
				m_listCtrlLayer.EnsureVisible(index + size - 1, FALSE);
				LVITEM lv;
				ZeroMemory(&lv, sizeof(lv));
				lv.state = LVIS_SELECTED;
				lv.stateMask = LVIS_SELECTED;
				lv.mask = LVIF_STATE;
				m_listCtrlLayer.SetItemState(itemTo, &lv);
			}
		}
	}
	DWORD style = m_listCtrlLayer.GetExtendedStyle();
	style ^= LVS_EX_FULLROWSELECT;
	m_listCtrlLayer.SetExtendedStyle(style);
	m_bDragging = FALSE;
	m_intDragTo = -1;
	ReleaseCapture();
}

void CDlgLayer::OnLButtonDown(UINT nFlags, CPoint point) 
{
	OnKillfocusEditLayerName();
	CDialog::OnLButtonDown(nFlags, point);
}
