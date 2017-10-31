#include "stdafx.h"
#include "gds.h"
#include "ShrinkDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////


CShrinkDlg::CShrinkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShrinkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShrinkDlg)
	m_intShrinkRate = 0;
	//}}AFX_DATA_INIT
}


void CShrinkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShrinkDlg)
	DDX_Text(pDX, IDC_SHRINK_RATE, m_intShrinkRate);
	DDV_MinMaxInt(pDX, m_intShrinkRate, 0, 80);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShrinkDlg, CDialog)
	//{{AFX_MSG_MAP(CShrinkDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_RATE, OnDeltaposUpdownRate)
	ON_EN_CHANGE(IDC_SHRINK_RATE, OnChangeShrinkRate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

double CShrinkDlg::GetShrinkRate()
{
    return m_intShrinkRate/100.0;

}

void CShrinkDlg::SetShrinkRate(double rate)
{
    if(rate >= 0.0 && rate <= 0.8)
        m_intShrinkRate = (int)(rate * 100.0 + 0.5);
    else
        m_intShrinkRate = 0;
}

void CShrinkDlg::OnDeltaposUpdownRate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if(pNMUpDown->iDelta > 0){
		if(m_intShrinkRate > 0)
			m_intShrinkRate--;
	}
	else{
		if(m_intShrinkRate < 80)
			m_intShrinkRate++;
	}

	UpdateData(FALSE);
	*pResult = 0;
}

void CShrinkDlg::OnChangeShrinkRate() 
{
	UpdateData(TRUE);
}
