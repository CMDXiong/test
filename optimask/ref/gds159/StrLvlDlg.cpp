#include "stdafx.h"
#include "gds.h"
#include "StrLvlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////


CStrLvlDlg::CStrLvlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStrLvlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStrLvlDlg)
	m_intLevel = 0;
	//}}AFX_DATA_INIT
}


void CStrLvlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStrLvlDlg)
	DDX_Text(pDX, IDC_STR_LEVEL, m_intLevel);
	DDV_MinMaxInt(pDX, m_intLevel, 0, 1023);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStrLvlDlg, CDialog)
	//{{AFX_MSG_MAP(CStrLvlDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_LEVEL, OnDeltaposUpdownLevel)
	ON_EN_CHANGE(IDC_STR_LEVEL, OnChangeStrLevel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStrLvlDlg

void CStrLvlDlg::OnDeltaposUpdownLevel(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if(pNMUpDown->iDelta < 0){
		if(m_intLevel < 1023){
			m_intLevel++;
			UpdateData(FALSE);
		}
	}
	else{
		if(m_intLevel > 0){
			m_intLevel--;
			UpdateData(FALSE);
		}
	}	
	*pResult = 0;
}

void CStrLvlDlg::SetLevel(int level)
{
	m_intLevel = level;
}

int CStrLvlDlg::GetLevel()
{
	return m_intLevel;
}

void CStrLvlDlg::OnChangeStrLevel() 
{
	UpdateData(TRUE);
}
