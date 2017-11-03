#include "stdafx.h"
#include "gds.h"
#include "HatchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CHatchDlg::CHatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHatchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHatchDlg)
	m_intHatchLimit = 0;
	m_intDrawLimit = 0;
	m_bOmitStructure = FALSE;
	//}}AFX_DATA_INIT
}


void CHatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHatchDlg)
	DDX_Text(pDX, IDC_HATCH_LIMIT, m_intHatchLimit);
	DDV_MinMaxInt(pDX, m_intHatchLimit, 0, 500);
	DDX_Text(pDX, IDC_DRAW_LIMIT, m_intDrawLimit);
	DDX_Check(pDX, IDC_CHECK_OMIT_STRUCTURE, m_bOmitStructure);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHatchDlg, CDialog)
	//{{AFX_MSG_MAP(CHatchDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_HATCH, OnDeltaposUpdownHatch)
	ON_EN_CHANGE(IDC_HATCH_LIMIT, OnChangeHatchLimit)
	ON_EN_CHANGE(IDC_DRAW_LIMIT, OnChangeDrawLimit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_DRAW, OnDeltaposUpdownDraw)
	ON_BN_CLICKED(IDC_CHECK_OMIT_STRUCTURE, OnCheckOmitStructure)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHatchDlg
void CHatchDlg::OnDeltaposUpdownHatch(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if(pNMUpDown->iDelta > 0){
		if(m_intHatchLimit > 0)
			m_intHatchLimit--;
	}
	else{
		if(m_intHatchLimit < 500)
			m_intHatchLimit++;
	}

	UpdateData(FALSE);
	*pResult = 0;
}
void CHatchDlg::OnDeltaposUpdownDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if(pNMUpDown->iDelta > 0){
		if(m_intDrawLimit > 0)
			m_intDrawLimit--;
	}
	else{
		if(m_intDrawLimit < 500)
			m_intDrawLimit++;
	}

	UpdateData(FALSE);
	*pResult = 0;
}

void CHatchDlg::OnChangeHatchLimit() 
{
	UpdateData(TRUE);
}
void CHatchDlg::OnChangeDrawLimit() 
{
	UpdateData(TRUE);
}

int CHatchDlg::GetHatchLimit()
{
	return m_intHatchLimit;
}

int CHatchDlg::GetDrawLimit()
{
	return m_intDrawLimit;
}

void CHatchDlg::SetHatchLimit(int lim)
{
	if(lim >= 0 && lim <= 500)
		m_intHatchLimit = lim;
}

void CHatchDlg::SetDrawLimit(int lim)
{
	if(lim >= 0 && lim <= 500)
		m_intDrawLimit = lim;
}



void CHatchDlg::OnCheckOmitStructure() 
{
	UpdateData(TRUE);
}
