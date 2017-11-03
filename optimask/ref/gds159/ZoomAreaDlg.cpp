#include "stdafx.h"
#include "gds.h"
#include "ZoomAreaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////


CZoomAreaDlg::CZoomAreaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CZoomAreaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CZoomAreaDlg)
	m_dblBottom = 0.0;
	m_dblLeft = 0.0;
	m_dblRight = 0.0;
	m_dblTop = 0.0;
	m_dblCenterX = 0.0;
	m_dblCenterY = 0.0;
	m_dblRadius = 0.0;
	m_intStyle = -1;
	//}}AFX_DATA_INIT
}


void CZoomAreaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZoomAreaDlg)
	DDX_Control(pDX, IDC_EDIT_TOP, m_editTop);
	DDX_Control(pDX, IDC_EDIT_RIGHT, m_editRight);
	DDX_Control(pDX, IDC_EDIT_RADIUS, m_editRadius);
	DDX_Control(pDX, IDC_EDIT_LEFT, m_editLeft);
	DDX_Control(pDX, IDC_EDIT_CENTER_Y, m_editCenterY);
	DDX_Control(pDX, IDC_EDIT_CENTER_X, m_editCenterX);
	DDX_Control(pDX, IDC_EDIT_BOTTOM, m_editBottom);
	DDX_Text(pDX, IDC_EDIT_BOTTOM, m_dblBottom);
	DDX_Text(pDX, IDC_EDIT_LEFT, m_dblLeft);
	DDX_Text(pDX, IDC_EDIT_RIGHT, m_dblRight);
	DDX_Text(pDX, IDC_EDIT_TOP, m_dblTop);
	DDX_Text(pDX, IDC_EDIT_CENTER_X, m_dblCenterX);
	DDX_Text(pDX, IDC_EDIT_CENTER_Y, m_dblCenterY);
	DDX_Text(pDX, IDC_EDIT_RADIUS, m_dblRadius);
	DDX_Radio(pDX, IDC_RADIO_AREA, m_intStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CZoomAreaDlg, CDialog)
	//{{AFX_MSG_MAP(CZoomAreaDlg)
	ON_BN_CLICKED(IDC_RADIO_AREA, OnRadioArea)
	ON_BN_CLICKED(IDC_RADIO_CENTER, OnRadioCenter)
	ON_WM_SHOWWINDOW()
	ON_EN_KILLFOCUS(IDC_EDIT_LEFT, OnKillfocusEditLeft)
	ON_EN_KILLFOCUS(IDC_EDIT_TOP, OnKillfocusEditTop)
	ON_EN_KILLFOCUS(IDC_EDIT_BOTTOM, OnKillfocusEditBottom)
	ON_EN_KILLFOCUS(IDC_EDIT_CENTER_X, OnKillfocusEditCenterX)
	ON_EN_KILLFOCUS(IDC_EDIT_CENTER_Y, OnKillfocusEditCenterY)
	ON_EN_KILLFOCUS(IDC_EDIT_RADIUS, OnKillfocusEditRadius)
	ON_EN_KILLFOCUS(IDC_EDIT_RIGHT, OnKillfocusEditRight)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CZoomAreaDlg::ValidateArea()
{
	UpdateData(TRUE);

	if(m_dblBottom >= m_dblTop)
		return FALSE;
	if(m_dblLeft >= m_dblRight)
		return FALSE;
	return TRUE;
}

void CZoomAreaDlg::OnOK()
{
	if(ValidateArea())
		CDialog::OnOK();
}

void CZoomAreaDlg::SetCurrentArea(double left, double top, double right, double bottom)
{
	m_dblLeft  = left;
	m_dblTop = top;
	m_dblRight = right;
	m_dblBottom = bottom;

	m_dblCenterX = (m_dblLeft + m_dblRight)/2.0;
	m_dblCenterY = (m_dblTop + m_dblBottom)/2.0;
	m_dblRadius = min(m_dblRight - m_dblLeft, m_dblTop - m_dblBottom);
}

void CZoomAreaDlg::GetCoord(double *left, double *top, double *right, double *bottom)
{
	*left = m_dblLeft * 1e-6;
	*top = m_dblTop * 1e-6;
	*right = m_dblRight * 1e-6;
	*bottom = m_dblBottom * 1e-6;
}

void CZoomAreaDlg::OnRadioArea() 
{
	UpdateData(TRUE);

	m_editLeft.EnableWindow(TRUE);
	m_editTop.EnableWindow(TRUE);
	m_editRight.EnableWindow(TRUE);
	m_editBottom.EnableWindow(TRUE);

	m_editCenterX.EnableWindow(FALSE);
	m_editCenterY.EnableWindow(FALSE);
	m_editRadius.EnableWindow(FALSE);
	UpdateData(FALSE);
}

void CZoomAreaDlg::OnRadioCenter() 
{
	UpdateData(TRUE);

	m_editLeft.EnableWindow(FALSE);
	m_editTop.EnableWindow(FALSE);
	m_editRight.EnableWindow(FALSE);
	m_editBottom.EnableWindow(FALSE);

	m_editCenterX.EnableWindow(TRUE);
	m_editCenterY.EnableWindow(TRUE);
	m_editRadius.EnableWindow(TRUE);
	UpdateData(FALSE);
}

void CZoomAreaDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	switch(m_intStyle){
	case 0:
		OnRadioArea();
		break;
	case 1:
		OnRadioCenter();
		break;
	default:
		break;
	}


	Recalc();
}

void CZoomAreaDlg::OnKillfocusEditLeft() 
{
	if(m_intStyle != 0)
		return;
	UpdateData(TRUE);
	Recalc();
}

void CZoomAreaDlg::OnKillfocusEditRight() 
{
	if(m_intStyle != 0)
		return;
	UpdateData(TRUE);
	Recalc();
}

void CZoomAreaDlg::OnKillfocusEditTop() 
{
	if(m_intStyle != 0)
		return;
	UpdateData(TRUE);
	Recalc();
}

void CZoomAreaDlg::OnKillfocusEditBottom() 
{
	if(m_intStyle != 0)
		return;
	UpdateData(TRUE);
	Recalc();
}

void CZoomAreaDlg::OnKillfocusEditCenterX() 
{
	if(m_intStyle != 1)
		return;
	UpdateData(TRUE);
	Recalc();
}

void CZoomAreaDlg::OnKillfocusEditCenterY() 
{
	if(m_intStyle != 1)
		return;
	UpdateData(TRUE);
	Recalc();
}

void CZoomAreaDlg::OnKillfocusEditRadius() 
{
	if(m_intStyle != 1)
		return;
	UpdateData(TRUE);
	Recalc();
}

void CZoomAreaDlg::Recalc()
{
	m_dblCenterX = (double)(int)(m_dblCenterX * 10.0)/10.0;
	m_dblCenterY = (double)(int)(m_dblCenterY * 10.0)/10.0;
	m_dblRadius = (double)(int)(m_dblRadius * 10.0)/10.0;
	m_dblLeft = (double)(int)(m_dblLeft * 10.0)/10.0;
	m_dblRight = (double)(int)(m_dblRight * 10.0)/10.0;
	m_dblTop = (double)(int)(m_dblTop * 10.0)/10.0;
	m_dblBottom = (double)(int)(m_dblBottom * 10.0)/10.0;
	if(m_intStyle == 0){
		m_dblCenterX = (m_dblLeft + m_dblRight)/2.0;
		m_dblCenterY = (m_dblTop + m_dblBottom)/2.0;
		m_dblRadius = min(m_dblRight - m_dblLeft, m_dblTop - m_dblBottom);
	}
	else if(m_intStyle == 1){
		m_dblLeft = m_dblCenterX - m_dblRadius/2.0;
		m_dblRight = m_dblCenterX + m_dblRadius/2.0;
		m_dblTop = m_dblCenterY + m_dblRadius/2.0;
		m_dblBottom = m_dblCenterY - m_dblRadius/2.0;
	}
	m_dblCenterX = (double)(int)(m_dblCenterX * 10.0)/10.0;
	m_dblCenterY = (double)(int)(m_dblCenterY * 10.0)/10.0;
	m_dblRadius = (double)(int)(m_dblRadius * 10.0)/10.0;
	m_dblLeft = (double)(int)(m_dblLeft * 10.0)/10.0;
	m_dblRight = (double)(int)(m_dblRight * 10.0)/10.0;
	m_dblTop = (double)(int)(m_dblTop * 10.0)/10.0;
	m_dblBottom = (double)(int)(m_dblBottom * 10.0)/10.0;
	UpdateData(FALSE);
}


void CZoomAreaDlg::SetZoomAreaMode(int mode)
{
	switch(mode){
	case ZOOM_AREA_MODE_AREA:
		m_intStyle = 1;
		break;
	case ZOOM_AREA_MODE_SIDE:
	default:
		m_intStyle = 0;
		break;
	}
}

int CZoomAreaDlg::GetZoomAreaMode()
{
	switch(m_intStyle){
	case 1:
		return ZOOM_AREA_MODE_AREA;
		break;
	case 0:
		return ZOOM_AREA_MODE_SIDE;
	default:
		break;
	}
	return -1;
}
