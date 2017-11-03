// GridDlg.cpp : Implementation file
//

#include "stdafx.h"
#include "gds.h"
#include "GridDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define		GetView() ((CGdsView*)m_pParent)
#define     GRID_MAX_RATIO     1000
/////////////////////////////////////////////////////////////////////////////
// CGridDlg dialogue


CGridDlg::CGridDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGridDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGridDlg)
	m_intRadioDirection = -1;
	m_intMajorX = 0;
	m_intMajorY = 0;
	m_dblMinorX = 0.0;
	m_dblMinorY = 0.0;
	m_dblSnapX = 0.0;
	m_dblSnapY = 0.0;
	m_strGridSize = _T("");
	m_intRadioStyle = -1;
	//}}AFX_DATA_INIT
	m_pParent = pParent;
	m_bMouseInView = FALSE;
}


void CGridDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGridDlg)
	DDX_Radio(pDX, IDC_RADIO_XY, m_intRadioDirection);
	DDX_Text(pDX, IDC_EDIT_MAJOR_X, m_intMajorX);
	DDX_Text(pDX, IDC_EDIT_MAJOR_Y, m_intMajorY);
	DDX_Text(pDX, IDC_EDIT_MINOR_X, m_dblMinorX);
	DDX_Text(pDX, IDC_EDIT_MINOR_Y, m_dblMinorY);
	DDX_Text(pDX, IDC_EDIT_SNAP_X, m_dblSnapX);
	DDX_Text(pDX, IDC_EDIT_SNAP_Y, m_dblSnapY);
	DDX_Text(pDX, IDC_GRDI_SIZE, m_strGridSize);
	DDX_Radio(pDX, IDC_RADIO_GRID_DOT, m_intRadioStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGridDlg, CDialog)
	//{{AFX_MSG_MAP(CGridDlg)
	ON_WM_SHOWWINDOW()
	ON_EN_KILLFOCUS(IDC_EDIT_SNAP_X, OnKillfocusEditSnapX)
	ON_EN_KILLFOCUS(IDC_EDIT_SNAP_Y, OnKillfocusEditSnapY)
	ON_WM_KILLFOCUS()
	ON_WM_CAPTURECHANGED()
	ON_EN_CHANGE(IDC_EDIT_SNAP_X, OnChangeEditSnapX)
	ON_EN_CHANGE(IDC_EDIT_SNAP_Y, OnChangeEditSnapY)
	ON_BN_CLICKED(IDC_RADIO_XY, OnRadioXy)
	ON_BN_CLICKED(IDC_RADIO_FREE, OnRadioFree)
	ON_BN_CLICKED(IDC_RADIO_DIAGONAL, OnRadioDiagonal)
	ON_EN_CHANGE(IDC_EDIT_MAJOR_X, OnChangeEditMajorX)
	ON_EN_KILLFOCUS(IDC_EDIT_MAJOR_X, OnKillfocusEditMajorX)
	ON_EN_CHANGE(IDC_EDIT_MAJOR_Y, OnChangeEditMajorY)
	ON_EN_KILLFOCUS(IDC_EDIT_MAJOR_Y, OnKillfocusEditMajorY)
	ON_EN_CHANGE(IDC_EDIT_MINOR_X, OnChangeEditMinorX)
	ON_EN_KILLFOCUS(IDC_EDIT_MINOR_X, OnKillfocusEditMinorX)
	ON_EN_CHANGE(IDC_EDIT_MINOR_Y, OnChangeEditMinorY)
	ON_EN_KILLFOCUS(IDC_EDIT_MINOR_Y, OnKillfocusEditMinorY)
	ON_BN_CLICKED(IDC_RADIO_GRID_DOT, OnRadioGridDot)
	ON_BN_CLICKED(IDC_RADIO_GRID_LINE, OnRadioGridLine)
	ON_BN_CLICKED(IDC_RADIO_GRID_NONE, OnRadioGridNone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CGridDlg::PostNcDestroy() 
{
	delete this;
	//CDialog::PostNcDestroy();
}

void CGridDlg::ExitMeasure()
{
	OnCancel();
}


void CGridDlg::OnCancel()
{
	ValidateSettings();
	((CGdsApp*)AfxGetApp())->SetGridDlgHWND(NULL);
	*m_pEnabler = FALSE;
	CDialog::OnCancel();
}

void CGridDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	CGdsView* pView = GetView();
	double dbunit = pView->GetGDShcy()->GetDBUnit();
	double scaleum =  dbunit * 1e+6; // convert to [um]
	if(scaleum > 0.001)
		m_strGridSize.Format("Database unit = %.3fum", scaleum);
	else
		m_strGridSize.Format("Database unit = %.3fnm", scaleum * 1e+3);

	switch(pView->GetMeasureDirection()){
	case MEAS_DIR_XY:
		m_intRadioDirection = 0;
		break;
	case MEAS_DIR_DIAGONAL:
		m_intRadioDirection = 1;
		break;
	case MEAS_DIR_ANY:
		m_intRadioDirection = 2;
		break;
	default:
		break;
	}

	CGrid* grid = pView->m_pGrid;
	switch(grid->GetGridType()){
	case GRID_TYPE_DOT:
		m_intRadioStyle = 0;
		break;
	case GRID_TYPE_LINE:
		m_intRadioStyle = 1;
		break;
	case GRID_TYPE_NONE:
		m_intRadioStyle = 2;
		break;
	default:
		break;
	}

	m_dblSnapX = grid->GetSnapGridX() * dbunit * 1e+6;
	m_dblSnapY = grid->GetSnapGridY() * dbunit * 1e+6;
	m_dblMinorX = grid->GetMinorX() * dbunit * 1e+6;
	m_dblMinorY = grid->GetMinorY() * dbunit * 1e+6;
	m_intMajorX = grid->GetMajorX();
	m_intMajorY = grid->GetMajorY();
	UpdateData(FALSE);
	((CGdsApp*)AfxGetApp())->SetGridDlgHWND(m_hWnd);
}

static VOID CALLBACK TrackMouseLeave(HWND hWnd,UINT uMsg,UINT idEvent,DWORD dwTime)
{
	RECT rect;
	POINT pt;

	GetClientRect(hWnd,&rect);
	MapWindowPoints(hWnd,NULL,(LPPOINT)&rect,2);
	GetCursorPos(&pt);
	if (!PtInRect(&rect,pt) || (WindowFromPoint(pt) != hWnd)) {
		 if (!KillTimer(hWnd,idEvent)) {
		 }
		 PostMessage(hWnd,WM_MOUSELEAVE,0,0);
	}
}

static BOOL TrackMouseEvent(LPTRACKMOUSEEVENT ptme)
{
	if(!ptme || ptme->cbSize < sizeof(TRACKMOUSEEVENT))
		return FALSE;

	if(!IsWindow(ptme->hwndTrack))
		return FALSE;

	if(ptme->dwFlags & TME_LEAVE) {
		return SetTimer(ptme->hwndTrack, ptme->dwFlags,
					  100,(TIMERPROC)TrackMouseLeave);
	}
	return FALSE;
}

BOOL CGridDlg::PreTranslateMessage(MSG* pMsg) 
{
	TRACKMOUSEEVENT tme;


    switch (pMsg->message){
	case WM_KEYDOWN:
		switch(pMsg->wParam){
		case VK_TAB:
			break;
		default:
			GetView()->OnKeyDown(pMsg->wParam,  0, pMsg->lParam);
			//return TRUE;
			break;
		}
		break;
    case WM_MOUSEMOVE:
		if(!m_bMouseInView){
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = m_hWnd;
			if(!TrackMouseEvent(&tme)) {
				//::MessageBox(m_hWnd, TEXT("TrackMouseEvent Failed"), TEXT("Mouse Leave"),MB_OK);
			}
			m_bMouseInView = TRUE;
		}
        break;
	case WM_MOUSELEAVE:
		//OnMouseLeave();
		m_bMouseInView = FALSE;
		break;
	default:
        break;
    }
	return CDialog::PreTranslateMessage(pMsg);
}

void CGridDlg::OnOK()
{
	ValidateSettings();
}

void CGridDlg::OnKillfocusEditSnapX() 
{
	//ValidateSettings();
}

void CGridDlg::OnKillfocusEditSnapY() 
{
	//ValidateSettings();
}


void CGridDlg::OnKillFocus(CWnd* pNewWnd) 
{
	ValidateSettings();
	CDialog::OnKillFocus(pNewWnd);
}

void CGridDlg::OnCaptureChanged(CWnd *pWnd) 
{
	//ValidateSettings();
	CDialog::OnCaptureChanged(pWnd);
}

void CGridDlg::OnChangeEditSnapX() 
{
}

void CGridDlg::OnChangeEditSnapY() 
{
}

void CGridDlg::OnMouseLeave()
{
	OnOK();	
}

void CGridDlg::OnRadioXy() 
{
	GetView()->SetMeasureDirection(MEAS_DIR_XY);
	GetView()->SetCommandGuide(CMD_GUIDE_UPDATE);
}

void CGridDlg::OnRadioFree() 
{
	GetView()->SetMeasureDirection(MEAS_DIR_ANY);
	GetView()->SetCommandGuide(CMD_GUIDE_UPDATE);
}

void CGridDlg::OnRadioDiagonal() 
{
	GetView()->SetMeasureDirection(MEAS_DIR_DIAGONAL);
	GetView()->SetCommandGuide(CMD_GUIDE_UPDATE);
}

void CGridDlg::OnChangeEditMajorX() 
{
}

void CGridDlg::OnKillfocusEditMajorX() 
{
	//ValidateSettings();
}

void CGridDlg::OnChangeEditMajorY() 
{
}

void CGridDlg::OnKillfocusEditMajorY() 
{
	//ValidateSettings();
}

void CGridDlg::OnChangeEditMinorX() 
{
}

void CGridDlg::OnKillfocusEditMinorX() 
{
	//ValidateSettings();
}

void CGridDlg::OnChangeEditMinorY() 
{
}

void CGridDlg::OnKillfocusEditMinorY() 
{
	//ValidateSettings();
}

void CGridDlg::OnRadioGridDot() 
{
	UpdateData(TRUE);
	GetView()->OnGridDot();
}

void CGridDlg::OnRadioGridLine() 
{
	UpdateData(TRUE);
	GetView()->OnGridLine();
}

void CGridDlg::OnRadioGridNone() 
{
	UpdateData(TRUE);
	GetView()->OnGridNone();
}

void CGridDlg::ValidateSettings()
{
	UpdateData(TRUE);

	CGdsView* pView = GetView();
	CGrid* grid = pView->m_pGrid;

	double dbunit = pView->GetGDShcy()->GetDBUnit();
	int    user = (int)(1.0 / pView->GetGDShcy()->GetUserUnit());
	double val = m_dblSnapX * 1e-6;



	double mulsnap = (val / dbunit);
	if(mulsnap <= 0)
		mulsnap = 1;
	else if(mulsnap > user * GRID_MAX_RATIO)
		mulsnap = user * GRID_MAX_RATIO;
	else if(mulsnap < 0.001)
		mulsnap = 0.001;
	grid->SetSnapGridX(mulsnap);

	val = m_dblSnapY * 1e-6;
	mulsnap = (val / dbunit);
	if(mulsnap <= 0)
		mulsnap = 1;
	else if(mulsnap > user * GRID_MAX_RATIO)
		mulsnap = user * GRID_MAX_RATIO;
	else if(mulsnap < 0.001)
		mulsnap = 0.001;
	grid->SetSnapGridY(mulsnap);


	val = m_dblMinorX * 1e-6;
	mulsnap = (val / dbunit);
	if(mulsnap <= 0)
		mulsnap = 1;
	else if(mulsnap > user * GRID_MAX_RATIO)
		mulsnap = user * GRID_MAX_RATIO;
	else if(mulsnap < 0.001)
		mulsnap = 0.001;
	grid->SetMinorX(mulsnap);


	val = m_dblMinorY * 1e-6;
	mulsnap = (val / dbunit);
	if(mulsnap <= 0)
		mulsnap = 1;
	else if(mulsnap > user * GRID_MAX_RATIO)
		mulsnap = user * GRID_MAX_RATIO;
	else if(mulsnap < 0.001)
		mulsnap = 0.001;
	grid->SetMinorY(mulsnap);


	if(m_intMajorX <= 0)
		m_intMajorX = 1;
	else if(m_intMajorX > 1000)
		m_intMajorX = 1000;
	grid->SetMajorX(m_intMajorX);

	if(m_intMajorY <= 0)
		m_intMajorY = 1;
	else if(m_intMajorY > 1000)
		m_intMajorY = 1000;
	grid->SetMajorY(m_intMajorY);

	m_dblSnapX = grid->GetSnapGridX() * dbunit * 1e+6;
	m_dblSnapY = grid->GetSnapGridY() * dbunit * 1e+6;
	m_dblMinorX = grid->GetMinorX() * dbunit * 1e+6;
	m_dblMinorY = grid->GetMinorY() * dbunit * 1e+6;
	m_intMajorX = grid->GetMajorX();
	m_intMajorY = grid->GetMajorY();
	
	UpdateData(FALSE);
}




void CGridDlg::SetDirection(int dir)
{
	m_intRadioDirection = dir;
	UpdateData(FALSE);
}

void CGridDlg::SetGridType(int type)
{
	switch(type){
	case GRID_TYPE_DOT:
		m_intRadioStyle = 0;
		break;
	case GRID_TYPE_LINE:
		m_intRadioStyle = 1;
		break;
	case GRID_TYPE_NONE:
		m_intRadioStyle = 2;
		break;
	default:
		break;
	}
	UpdateData(FALSE);
}
