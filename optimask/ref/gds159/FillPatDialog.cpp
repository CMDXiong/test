#include "stdafx.h"
#include "gds.h"
#include "FillPatDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFillPatDialog::CFillPatDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFillPatDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFillPatDialog)
	m_bShowColor = FALSE;
	//}}AFX_DATA_INIT
	m_intPattern = -1;
	m_colorLayerColor = RGB(0, 0, 0);
	m_colorLayerBk = GetSysColor(COLOR_MENU);
}


void CFillPatDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFillPatDialog)
	DDX_Control(pDX, IDC_DIA, m_buttonDia);
	DDX_Control(pDX, IDC_RDIAG_DASH, m_buttonRDiagDash);
	DDX_Control(pDX, IDC_FDIAG_DASH, m_buttonFDiagDash);
	DDX_Control(pDX, IDC_LIGHT, m_buttonLight);
	DDX_Control(pDX, IDC_VDASH, m_buttonVDash);
	DDX_Control(pDX, IDC_VWAVE, m_buttonVwave);
	DDX_Control(pDX, IDC_HWAVE, m_buttonHwave);
	DDX_Control(pDX, IDC_HDASH, m_buttonHDash);
	DDX_Control(pDX, IDC_VZIGZAG, m_buttonVZigzag);
	DDX_Control(pDX, IDC_HZIGZAG, m_buttonHZigzag);
	DDX_Control(pDX, IDC_XDIAG, m_buttonXDiag);
	DDX_Control(pDX, IDC_VHATCH, m_buttonVHatch);
	DDX_Control(pDX, IDC_SOLID, m_buttonSolid);
	DDX_Control(pDX, IDC_RDIAG, m_buttonRDiag);
	DDX_Control(pDX, IDC_HHATCH, m_buttonHHatch);
	DDX_Control(pDX, IDC_FRAME, m_buttonFrame);
	DDX_Control(pDX, IDC_FDIAG, m_buttonFDiag);
	DDX_Control(pDX, IDC_XHATCH, m_buttonXHatch);
	DDX_Check(pDX, IDC_CHECK_USE_COLOR, m_bShowColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFillPatDialog, CDialog)
	//{{AFX_MSG_MAP(CFillPatDialog)
	ON_BN_CLICKED(IDC_XHATCH, OnChatch)
	ON_BN_CLICKED(IDC_FDIAG, OnFdiag)
	ON_BN_CLICKED(IDC_FRAME, OnFrame)
	ON_BN_CLICKED(IDC_HHATCH, OnHhatch)
	ON_BN_CLICKED(IDC_RDIAG, OnRdiag)
	ON_BN_CLICKED(IDC_SOLID, OnSolid)
	ON_BN_CLICKED(IDC_VHATCH, OnVhatch)
	ON_BN_CLICKED(IDC_XDIAG, OnXdiag)
	ON_WM_DRAWITEM()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_HZIGZAG, OnHZigzag)
	ON_BN_CLICKED(IDC_VZIGZAG, OnVZigzag)
	ON_BN_CLICKED(IDC_HDASH, OnHDash)
	ON_BN_CLICKED(IDC_VWAVE, OnVwave)
	ON_BN_CLICKED(IDC_HWAVE, OnHwave)
	ON_BN_CLICKED(IDC_VDASH, OnVdash)
	ON_BN_CLICKED(IDC_LIGHT, OnLight)
	ON_BN_CLICKED(IDC_RDIAG_DASH, OnRdiagDash)
	ON_BN_CLICKED(IDC_FDIAG_DASH, OnFdiagDash)
	ON_BN_CLICKED(IDC_DIA, OnDia)
	ON_BN_CLICKED(IDC_CHECK_USE_COLOR, OnCheckUseColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFillPatDialog
#include "GdsDoc.h"
#include "GdsView.h"

void CFillPatDialog::OnChatch() 
{
	m_intPattern = FILL_BMP_XHATCH;
	OnOK();
}

void CFillPatDialog::OnFdiag() 
{
	m_intPattern = FILL_BMP_FDIAG;
	OnOK();
}

void CFillPatDialog::OnFrame() 
{
	m_intPattern = FILL_BMP_NONE ;
	OnOK();
}

void CFillPatDialog::OnHhatch() 
{
	m_intPattern = FILL_BMP_HHATCH;
	OnOK();
}

void CFillPatDialog::OnRdiag() 
{
	m_intPattern = FILL_BMP_RDIAG;
	OnOK();
}

void CFillPatDialog::OnRdiagDash() 
{
	m_intPattern = FILL_BMP_RDIAG_DASH;
	OnOK();
}

void CFillPatDialog::OnFdiagDash() 
{
	m_intPattern = FILL_BMP_FDIAG_DASH;
	OnOK();
}

void CFillPatDialog::OnSolid() 
{
	m_intPattern = FILL_BMP_SOLID;
	OnOK();
}

void CFillPatDialog::OnVhatch() 
{
	m_intPattern = FILL_BMP_VHATCH;
	OnOK();
}

void CFillPatDialog::OnXdiag() 
{
	m_intPattern = FILL_BMP_XDIAG;
	OnOK();
}

void CFillPatDialog::OnHZigzag() 
{
	m_intPattern = FILL_BMP_HZIGZAG;
	OnOK();
}

void CFillPatDialog::OnVZigzag() 
{
	m_intPattern = FILL_BMP_VZIGZAG;
	OnOK();
}

void CFillPatDialog::OnHDash() 
{
	m_intPattern = FILL_BMP_HDASH;
	OnOK();
}

void CFillPatDialog::OnLight() 
{
	m_intPattern = FILL_BMP_LIGHT;
	OnOK();
}

void CFillPatDialog::OnVdash() 
{
	m_intPattern = FILL_BMP_VDASH;
	OnOK();
}

void CFillPatDialog::OnVwave() 
{
	m_intPattern = FILL_BMP_VWAVE;
	OnOK();
}

void CFillPatDialog::OnHwave() 
{
	m_intPattern = FILL_BMP_HWAVE;
	OnOK();
}

void CFillPatDialog::OnDia() 
{
	m_intPattern = FILL_BMP_XHATCH_HD;
	OnOK();
}

void CFillPatDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	switch(lpDrawItemStruct->CtlID){
	case IDC_SOLID:
	case IDC_FRAME:
	case IDC_FDIAG:
	case IDC_RDIAG:
	case IDC_XDIAG:
	case IDC_HHATCH:
	case IDC_VHATCH:
	case IDC_XHATCH:
	case IDC_HZIGZAG:
	case IDC_VZIGZAG:
	case IDC_HDASH:
	case IDC_VDASH:
	case IDC_VWAVE:
	case IDC_HWAVE:
	case IDC_LIGHT:
	case IDC_FDIAG_DASH:
	case IDC_RDIAG_DASH:
	case IDC_DIA:
		lpDrawItemStruct->CtlType = ODT_MENU;
		DrawButtons();
		break;
	default:
		break;
	}
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CFillPatDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
}


int CFillPatDialog::GetPattern()
{
	return m_intPattern;
}

void CFillPatDialog::DrawButtons()
{
	CBitmap bmp;

	bmp.LoadBitmap(IDB_FRAME);
	DrawButton(&m_buttonFrame, &bmp);

	bmp.LoadBitmap(IDB_SOLID);
	DrawButton(&m_buttonSolid, &bmp);

	bmp.LoadBitmap(IDB_XDIAG);
	DrawButton(&m_buttonXDiag, &bmp);

	bmp.LoadBitmap(IDB_FDIAG);
	DrawButton(&m_buttonFDiag, &bmp);

	bmp.LoadBitmap(IDB_RDIAG);
	DrawButton(&m_buttonRDiag, &bmp);

	bmp.LoadBitmap(IDB_XHATCH);
	DrawButton(&m_buttonXHatch, &bmp);

	bmp.LoadBitmap(IDB_HHATCH);
	DrawButton(&m_buttonHHatch, &bmp);

	bmp.LoadBitmap(IDB_VHATCH);
	DrawButton(&m_buttonVHatch, &bmp);

	bmp.LoadBitmap(IDB_HZIGZAG);
	DrawButton(&m_buttonHZigzag, &bmp);

	bmp.LoadBitmap(IDB_VZIGZAG);
	DrawButton(&m_buttonVZigzag, &bmp);

	bmp.LoadBitmap(IDB_HDASH);
	DrawButton(&m_buttonHDash, &bmp);

	bmp.LoadBitmap(IDB_VDASH);
	DrawButton(&m_buttonVDash, &bmp);

	bmp.LoadBitmap(IDB_VWAVE);
	DrawButton(&m_buttonVwave, &bmp);

	bmp.LoadBitmap(IDB_HWAVE);
	DrawButton(&m_buttonHwave, &bmp);

	bmp.LoadBitmap(IDB_LIGHT);
	DrawButton(&m_buttonLight, &bmp);

	bmp.LoadBitmap(IDB_FDIAG_DASH);
	DrawButton(&m_buttonFDiagDash, &bmp);

	bmp.LoadBitmap(IDB_RDIAG_DASH);
	DrawButton(&m_buttonRDiagDash, &bmp);

	bmp.LoadBitmap(IDB_DIA);
	DrawButton(&m_buttonDia, &bmp);
}

void CFillPatDialog::DrawButton(CButton* button, CBitmap* bmp)
{
	CBrush brush, *pOldBrush;
	CRect rect;

	button->GetClientRect(&rect);

	CDC* pDC = button->GetDC();
	if(m_bShowColor)
		pDC->SetTextColor(m_colorLayerColor);
	else
		pDC->SetTextColor(RGB(0, 0, 0));
	pDC->SetBkMode(TRANSPARENT);


	if(m_bShowColor){
		if(m_colorLayerBk == RGB(0, 0, 0)){
			int element = (int)(0.1 * 255);
			pDC->SetBkColor(RGB(element, element, element));
		}
		else
			pDC->SetBkColor(m_colorLayerBk);
	}
	else
		pDC->SetBkColor(GetSysColor(COLOR_MENU));

	brush.CreatePatternBrush(bmp);

	pOldBrush = pDC->SelectObject(&brush);
	pDC->Rectangle(rect);
	pDC->SelectObject(pOldBrush);
	brush.DeleteObject();
	bmp->DeleteObject();

	if(m_bShowColor)
		brush.CreateSolidBrush(m_colorLayerColor);
	else
		brush.CreateSolidBrush(RGB(0, 0, 0));
	pDC->FrameRect(rect, &brush);
	brush.DeleteObject();
}


void CFillPatDialog::SetLayerColor(COLORREF color)
{
	m_colorLayerColor = color;
}

void CFillPatDialog::SetBkColor(COLORREF bkcolor)
{
	m_colorLayerBk = bkcolor;
}

void CFillPatDialog::OnCheckUseColor() 
{
	UpdateData(TRUE);
	UpdateData(FALSE);

	AfxGetApp()->WriteProfileInt("Misc", "ShowColoredPattern", m_bShowColor);

	InvalidateRect(NULL);
}

BOOL CFillPatDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_bShowColor = AfxGetApp()->GetProfileInt("Misc", "ShowColoredPattern", 1);
	UpdateData(FALSE);
	return TRUE;
}
