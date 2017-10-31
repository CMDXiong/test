#include "stdafx.h"
#include "gds.h"
#include "PrintScaleDlg.h"

#include "GdsDoc.h"
#include "GdsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////


CPrintScaleDlg::CPrintScaleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintScaleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintScaleDlg)
	m_intRadioMode = -1;
	m_uintRatio = 0;
	m_intFull = 0;
	m_intViewArea = -1;
	m_uintRatioArea = 0;
	m_bEnableShrink = FALSE;
	m_strShrinkRate = _T("");
	m_dblLineWidth = 0.0;
	m_strShrink = _T("");
	m_bPrintScale = FALSE;
	m_bPrintLegend = FALSE;
	m_bFooter = FALSE;
	m_dblLegendSize = 0.0;
	m_dblHatchPenWidth = 0.0;
	m_dblHatchSpacing = 0.0;
	//}}AFX_DATA_INIT
	this->pParent = pParent;
	m_dblShrink = 0.0;
}


void CPrintScaleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintScaleDlg)
	DDX_Control(pDX, IDC_EDIT_LINE_WIDTH, m_ctrlLineWidth);
	DDX_Control(pDX, IDC_SPIN_PENWIDTH, m_ctrlPenWidth);
	DDX_Control(pDX, IDC_COMBO_LEG_LOC, m_comboLegendPos);
	DDX_Control(pDX, IDC_EDIT_LEG_SIZE, m_editLegendSize);
	DDX_Control(pDX, IDC_SPIN_LEG_SIZE, m_spinLegendSize);
	DDX_Control(pDX, IDC_COMBO_PEN_COLOR, m_comboPenColor);
	DDX_Control(pDX, IDC_CHECK_SHRINK, m_buttonShrink);
	DDX_Control(pDX, IDC_SPIN_RATIO_AREA, m_spinRatioArea);
	DDX_Control(pDX, IDC_EDIT_RATIO_AREA, m_editRatioArea);
	DDX_Control(pDX, IDC_BUTTON_AUTO_AREA, m_buttonAutoArea);
	DDX_Control(pDX, IDC_RADIO_FULL, m_radioFull);
	DDX_Control(pDX, IDC_RADIO_VIEW_AREA, m_radioViewArea);
	DDX_Control(pDX, IDC_BUTTON_AUTO, m_buttonAuto);
	DDX_Control(pDX, IDC_EDIT_RATIO, m_editRatio);
	DDX_Control(pDX, IDC_SPIN_RATIO, m_spinRatio);
	DDX_Radio(pDX, IDC_RADIO_WYSIWYG, m_intRadioMode);
	DDX_Text(pDX, IDC_EDIT_RATIO, m_uintRatio);
	DDX_Radio(pDX, IDC_RADIO_FULL, m_intFull);
	DDX_Radio(pDX, IDC_RADIO_VIEW_AREA, m_intViewArea);
	DDX_Text(pDX, IDC_EDIT_RATIO_AREA, m_uintRatioArea);
	DDX_Check(pDX, IDC_CHECK_SHRINK, m_bEnableShrink);
	DDX_Text(pDX, IDC_EDIT_LINE_WIDTH, m_dblLineWidth);
	DDV_MinMaxDouble(pDX, m_dblLineWidth, 0., 10.);
	DDX_Text(pDX, IDC_EDIT_SHRINK, m_strShrink);
	DDX_Check(pDX, IDC_CHECK_SCALE, m_bPrintScale);
	DDX_Check(pDX, IDC_CHECK_LEGEND, m_bPrintLegend);
	DDX_Check(pDX, IDC_CHECK_FOOTER, m_bFooter);
	DDX_Text(pDX, IDC_EDIT_LEG_SIZE, m_dblLegendSize);
	DDV_MinMaxDouble(pDX, m_dblLegendSize, 0., 10.);
	DDX_Text(pDX, IDC_EDIT_HATCH_PEN_WIDTH, m_dblHatchPenWidth);
	DDV_MinMaxDouble(pDX, m_dblHatchPenWidth, 0., 10.);
	DDX_Text(pDX, IDC_EDIT_HATCH_SPACING, m_dblHatchSpacing);
	DDV_MinMaxDouble(pDX, m_dblHatchSpacing, 0.5, 10.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintScaleDlg, CDialog)
	//{{AFX_MSG_MAP(CPrintScaleDlg)
	ON_BN_CLICKED(IDC_RADIO_WYSIWYG, OnRadioWysiwyg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_RATIO, OnDeltaposSpinRatio)
	ON_BN_CLICKED(IDC_RADIO_FULL, OnRadioFull)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_AUTO, OnButtonAuto)
	ON_BN_CLICKED(IDC_RADIO_SCALING, OnRadioScaling)
	ON_BN_CLICKED(IDC_RADIO_VIEW_AREA, OnRadioViewArea)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_AREA, OnButtonAutoArea)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_RATIO_AREA, OnDeltaposSpinRatioArea)
	ON_BN_CLICKED(IDC_CHECK_SHRINK, OnCheckShrink)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PENWIDTH, OnDeltaposSpinPenwidth)
	ON_BN_CLICKED(IDC_CHECK_LEGEND, OnCheckLegend)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LEG_SIZE, OnDeltaposSpinLegSize)
	ON_CBN_SELCHANGE(IDC_COMBO_LEG_LOC, OnSelchangeComboLegLoc)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HATCHPENWIDTH, OnDeltaposSpinHatchpenwidth)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HATCHSPACING, OnDeltaposSpinHatchspacing)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CPrintScaleDlg::OnButtonAuto() 
{
	SetFitScale(-1.0, 0.0);
}

void CPrintScaleDlg::OnButtonAutoArea() 
{
	SetFitScale(0.0, -1.0);
}

void CPrintScaleDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	ShowRatio(m_intRadioMode == PRINT_MODE_WYSIWYG ? TRUE : FALSE);
	m_comboPenColor.SetCurSel(m_bUseColorPen ? 1 : 0);
	m_strShrink.Format("%d", (int)(m_dblShrink * 100.0));
	if(m_dblShrink == 0)
		m_buttonShrink.EnableWindow(FALSE);
		
	int pos = 0;
	if(m_dblLegendDir == 0.0)
		pos = 0;
	else if(m_dblLegendDir == -90.0)
		pos = 2;
	else if(m_dblLegendDir == 90.0)
		pos = 1;
	m_comboLegendPos.SetCurSel(pos);

	m_spinLegendSize.EnableWindow(m_bPrintLegend);
	m_editLegendSize.EnableWindow(m_bPrintLegend);

	if(!((CGdsView*)pParent)->IsHatchEnabled()){
		m_comboPenColor.EnableWindow(FALSE);
		m_ctrlLineWidth.EnableWindow(FALSE);
		m_ctrlPenWidth.EnableWindow(FALSE);
	}


	UpdateData(FALSE);
	GetClientRect(pRect);
	ClientToScreen(pRect);
	CDialog::OnShowWindow(bShow, nStatus);
}

void CPrintScaleDlg::ShowRatio(BOOL enable)
{
	BOOL full = m_intFull == 0 ? TRUE : FALSE;
	BOOL area = m_intViewArea == 0 ? TRUE : FALSE;

	if(!enable)
		full = area = FALSE;


	m_radioFull.EnableWindow(enable);
	m_radioViewArea.EnableWindow(enable);

	m_editRatio.EnableWindow(full);
	m_spinRatio.EnableWindow(full);
	m_buttonAuto.EnableWindow(full);

	m_editRatioArea.EnableWindow(area);
	m_spinRatioArea.EnableWindow(area);
	m_buttonAutoArea.EnableWindow(area);
}

int CPrintScaleDlg::GetPrintMode()
{
	switch(m_intRadioMode){
	case 0:
		return PRINT_MODE_WYSIWYG;
		break;
	case 1:
		if(m_intFull == 0)
			return PRINT_MODE_SCALED_FULL;
		if(m_intViewArea == 0)
			return PRINT_MODE_SCALED_AREA;
		break;
	default:
		return -1;
		break;
	}
	return -1;
}

double CPrintScaleDlg::GetPrintScale(BOOL whole)
{
	if(whole)
		return (double)(m_uintRatio);
	else
		return (double)(m_uintRatioArea);
}

void CPrintScaleDlg::SetPrintMode(int mode)
{
	if(mode == PRINT_MODE_UNKNOWN)
		mode = PRINT_MODE_WYSIWYG;

	switch(mode){
	case PRINT_MODE_WYSIWYG:
		m_intRadioMode = 0;
		break;
	case PRINT_MODE_SCALED_FULL:
		m_intRadioMode = 1;
		m_intFull = 0;
		m_intViewArea = -1;
		break;
	case PRINT_MODE_SCALED_AREA:
		m_intRadioMode = 1;
		m_intFull = -1;
		m_intViewArea = 0;
		break;
	default:
		break;
	}

	if(::IsWindow(m_hWnd))
		UpdateData(FALSE);
}

void CPrintScaleDlg::SetFitScale(double ratio, double ratio_area)
{
	if(::IsWindow(m_hWnd))
		UpdateData(TRUE);

	if(ratio < 0.0)
		ratio = ((CGdsView*)pParent)->OnFitView(TRUE, FALSE, m_bEnableShrink);
	double r = ratio * 100.0;
	if(ratio > 0.0)
		m_uintRatio = (UINT)(r / 100.0);

	if(ratio_area < 0.0)
		ratio_area = ((CGdsView*)pParent)->OnFitView(TRUE, TRUE, m_bEnableShrink);
	r = ratio_area * 100.0;
	if(ratio_area > 0.0)
		m_uintRatioArea = (UINT)(r / 100.0);
	
	
	if(::IsWindow(m_hWnd))
		UpdateData(FALSE);
}

void CPrintScaleDlg::OnDeltaposSpinRatio(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if(pNMUpDown->iDelta > 0){
		if(m_uintRatio > 0)
			m_uintRatio--;
	}
	else
		m_uintRatio++;

	UpdateData(FALSE);
	*pResult = 0;
}
void CPrintScaleDlg::OnDeltaposSpinRatioArea(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if(pNMUpDown->iDelta > 0){
		if(m_uintRatioArea > 0)
			m_uintRatioArea--;
	}
	else
		m_uintRatioArea++;

	UpdateData(FALSE);
	*pResult = 0;
}

void CPrintScaleDlg::OnRadioScaling() 
{
	UpdateData(TRUE);
	ShowRatio(TRUE);
}

void CPrintScaleDlg::OnRadioWysiwyg() 
{
	UpdateData(TRUE);
	ShowRatio(FALSE);
}

void CPrintScaleDlg::OnRadioViewArea() 
{
	m_intViewArea = 0;
	m_intFull = -1;
	ShowRatio(TRUE);
	UpdateData(FALSE);
}
void CPrintScaleDlg::OnRadioFull() 
{
	m_intViewArea = -1;
	m_intFull = 0;
	ShowRatio(TRUE);
	UpdateData(FALSE);
}


void CPrintScaleDlg::OnCheckShrink() 
{
	BOOL prev = m_bEnableShrink;
	UpdateData(TRUE);
	if(prev != m_bEnableShrink){
		double ratio = (double)m_uintRatio / 100.0;
		double ratio_area = (double)m_uintRatioArea / 100.0;
		if(m_bEnableShrink){
			ratio /= (1.0 - m_dblShrink);
			ratio_area /= (1.0 - m_dblShrink);
		}
		else{
			ratio *= (1.0 - m_dblShrink);
			ratio_area *= (1.0 - m_dblShrink);
		}
		m_uintRatio = (int)(ratio * 100);
		m_uintRatioArea = (int)(ratio_area * 100);
		UpdateData(FALSE);
	}
}

void CPrintScaleDlg::OnDeltaposSpinPenwidth(NMHDR* pNMHDR, LRESULT* pResult) 
{
	double step = 0.01;
	UpdateData(TRUE);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if(pNMUpDown->iDelta > 0){
		if(m_dblLineWidth >= step)
			m_dblLineWidth -= step;
	}
	else
		m_dblLineWidth += step;

	m_dblLineWidth = ((int)(m_dblLineWidth * 1000.0 + 0.5))/1000.0;
	UpdateData(FALSE);
	*pResult = 0;
}

void CPrintScaleDlg::OnCheckLegend() 
{
	UpdateData(TRUE);
	m_spinLegendSize.EnableWindow(m_bPrintLegend);
	m_editLegendSize.EnableWindow(m_bPrintLegend);
	m_comboLegendPos.EnableWindow(m_bPrintLegend);

}

void CPrintScaleDlg::OnDeltaposSpinLegSize(NMHDR* pNMHDR, LRESULT* pResult) 
{
	double step = 1.0;
	UpdateData(TRUE);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if(pNMUpDown->iDelta > 0){
		if(m_dblLegendSize >= step)
			m_dblLegendSize -= step;
	}
	else
		m_dblLegendSize += step;

	UpdateData(FALSE);
	*pResult = 0;
}

void CPrintScaleDlg::OnSelchangeComboLegLoc() 
{
	switch(m_comboLegendPos.GetCurSel()){
	case 0:
		m_dblLegendDir = 0.0;
		break;
	case 1:
		m_dblLegendDir =  90.0;
		break;
	case 2:
		m_dblLegendDir =  -90.0;
		break;
	default:
		m_dblLegendDir =  0.0;
		break;
	}
}

void CPrintScaleDlg::OnDeltaposSpinHatchpenwidth(NMHDR* pNMHDR, LRESULT* pResult) 
{
	double step = 0.01;
	UpdateData(TRUE);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if(pNMUpDown->iDelta > 0){
		if(m_dblHatchPenWidth >= step)
			m_dblHatchPenWidth -= step;
	}
	else
		m_dblHatchPenWidth += step;

	m_dblHatchPenWidth = ((int)(m_dblHatchPenWidth * 1000.0 + 0.5))/1000.0;
	UpdateData(FALSE);
	*pResult = 0;
}

void CPrintScaleDlg::OnDeltaposSpinHatchspacing(NMHDR* pNMHDR, LRESULT* pResult) 
{
	double step = 0.1;
	UpdateData(TRUE);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if(pNMUpDown->iDelta > 0){
		if(m_dblHatchSpacing >= step)
			m_dblHatchSpacing -= step;
	}
	else
		m_dblHatchSpacing += step;

	m_dblHatchSpacing = ((int)(m_dblHatchSpacing * 10.0 + 0.5))/10.0;
	UpdateData(FALSE);
	*pResult = 0;
}

void CPrintScaleDlg::OnOK() 
{
	UpdateData(TRUE);
	m_bUseColorPen = m_comboPenColor.GetCurSel() == 0 ? FALSE : TRUE;
	CDialog::OnOK();
}

