#include "stdafx.h"
#include "gds.h"
#include "AngleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CAngleDlg::CAngleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAngleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAngleDlg)
	m_dblAngle = 0.0;
	//}}AFX_DATA_INIT
}


void CAngleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAngleDlg)
	DDX_Control(pDX, IDC_VIEW_ANGLE, m_editAngle);
	DDX_Text(pDX, IDC_VIEW_ANGLE, m_dblAngle);
	DDV_MinMaxDouble(pDX, m_dblAngle, -360., 360.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAngleDlg, CDialog)
	//{{AFX_MSG_MAP(CAngleDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_ANGLE, OnDeltaposUpdownAngle)
	ON_EN_CHANGE(IDC_VIEW_ANGLE, OnChangeViewAngle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CAngleDlg::OnDeltaposUpdownAngle(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if(pNMUpDown->iDelta < 0){
		m_dblAngle += 1.0;
		UpdateData(FALSE);
	}
	else{
		m_dblAngle -= 1.0;
		UpdateData(FALSE);
	}	
	*pResult = 0;
}

void CAngleDlg::SetAngle(double angle)
{
	m_dblAngle = angle;
}

double CAngleDlg::GetAngle()
{
	return m_dblAngle;
}

void CAngleDlg::OnChangeViewAngle() 
{
	char buf[20];
	m_editAngle.GetLine(0, buf, 10);
	if(strlen(buf) == 1 && *buf == '-')
		return;
	UpdateData(TRUE);
}

