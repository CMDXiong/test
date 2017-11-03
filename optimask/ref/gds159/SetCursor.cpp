#include "stdafx.h"
#include "gds.h"
#include "SetCursor.h"

#include "gdsDoc.h"
#include "gdsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CSetCursor::CSetCursor(CWnd* pParent /*=NULL*/)
	: CDialog(CSetCursor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetCursor)
	m_dblCoordX = 0.0;
	m_dblCoordY = 0.0;
	m_bFbCursor = FALSE;
	//}}AFX_DATA_INIT
}


void CSetCursor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetCursor)
	DDX_Control(pDX, IDC_COORD_Y, m_editCoordY);
	DDX_Control(pDX, IDC_COORD_X, m_editCoordX);
	DDX_Text(pDX, IDC_COORD_X, m_dblCoordX);
	DDX_Text(pDX, IDC_COORD_Y, m_dblCoordY);
	DDX_Check(pDX, IDC_FB_CURSOR, m_bFbCursor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetCursor, CDialog)
	//{{AFX_MSG_MAP(CSetCursor)
	ON_BN_CLICKED(ID_APPLY, OnApply)
	ON_EN_CHANGE(IDC_COORD_X, OnChangeCoordX)
	ON_EN_CHANGE(IDC_COORD_Y, OnChangeCoordY)
	ON_BN_CLICKED(IDC_FB_CURSOR, OnFbCursor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetCursor

void CSetCursor::OnApply() 
{
	CCoord point;
	point.x = m_dblCoordX * 1e-6; // um
	point.y = m_dblCoordY * 1e-6;
	m_pCaller->PanToPosMeter(point);
}

void CSetCursor::OnCancel()
{
	*m_pbEnabler = FALSE;
	CDialog::OnCancel();
}

void CSetCursor::SetEnabler(BOOL *enabler)
{
	m_pbEnabler = enabler;
}

void CSetCursor::ExitDialog()
{
	OnCancel();
}

void CSetCursor::SetWindowName(CString name)
{
	CString title;
	GetWindowText(title);
	title += " ";
	title += name;
	SetWindowText(title);
}

void CSetCursor::SetCaller(CGdsView *caller)
{
	m_pCaller = caller;
}

void CSetCursor::PostNcDestroy() 
{
	delete this;
	//CDialog::PostNcDestroy();
}

void CSetCursor::OnChangeCoordX() 
{
	char buf[20];
	m_editCoordX.GetLine(0, buf, 10);
	if(strlen(buf) == 1 && *buf == '-')
		return;
	if(strlen(buf) == 0)
		return;
	UpdateData(TRUE);
}

void CSetCursor::OnChangeCoordY() 
{
	char buf[20];
	m_editCoordY.GetLine(0, buf, 10);
	if(strlen(buf) == 1 && *buf == '-')
		return;
	if(strlen(buf) == 0)
		return;
	UpdateData(TRUE);
}

void CSetCursor::SetCurrentCoord(double x, double y)
{
	if(!m_bFbCursor)
		return;

	m_dblCoordX = x;
	m_dblCoordY = y;
	UpdateData(FALSE);
}

void CSetCursor::OnFbCursor() 
{
	m_bFbCursor ^= TRUE;
}
