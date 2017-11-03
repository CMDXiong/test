#include "stdafx.h"
#include "gds.h"
#include "FontSizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CFontSizeDlg::CFontSizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFontSizeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFontSizeDlg)
	m_dblFontSize = 0.0;
	m_bItalic = FALSE;
	m_bBold = FALSE;
	//}}AFX_DATA_INIT
}


void CFontSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFontSizeDlg)
	DDX_Control(pDX, IDC_LIST_FONTS, m_lbFonts);
	DDX_Text(pDX, IDC_FONT_SIZE, m_dblFontSize);
	DDX_Check(pDX, IDC_CHECK_ITALIC, m_bItalic);
	DDX_Check(pDX, IDC_CHECK_BOLD, m_bBold);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFontSizeDlg, CDialog)
	//{{AFX_MSG_MAP(CFontSizeDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_RATE, OnDeltaposUpdownRate)
	ON_EN_CHANGE(IDC_FONT_SIZE, OnChangeFontSize)
	ON_LBN_SELCHANGE(IDC_LIST_FONTS, OnSelchangeListFonts)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHECK_BOLD, OnCheckBold)
	ON_BN_CLICKED(IDC_CHECK_ITALIC, OnCheckItalic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontSizeDlg
#include <math.h>
void CFontSizeDlg::OnDeltaposUpdownRate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int n10 = (int)log10(m_dblFontSize);
	int n = (int)(m_dblFontSize/pow(10.0, n10));
	if(n == 0){
		n10--;
		n = (int)(m_dblFontSize/pow(10.0, n10));
	}

	if(pNMUpDown->iDelta < 0)
		n++;
	else{
		n--;
		if(n <= 0){
			n = 9;
			n10--;
		}
	}
	m_dblFontSize = pow(10.0, n10) * n; 
	UpdateData(FALSE);

	*pResult = 0;
}


void CFontSizeDlg::SetFontSize(double size)
{
	if(size > 0.0)
		m_dblFontSize = size;
}

double CFontSizeDlg::GetFontSize()
{
	return m_dblFontSize;
}


void CFontSizeDlg::OnChangeFontSize() 
{
	UpdateData(TRUE);
}

void CFontSizeDlg::OnSelchangeListFonts() 
{
	UpdateData(TRUE);
	int idx = m_lbFonts.GetCurSel();
	if(idx >= 0)
		 m_lbFonts.GetText(idx, m_strCurrent);
}

void CFontSizeDlg::SetCurrentFont(CString current)
{
	m_strCurrent = current;
}

static int CALLBACK EnumFontFamProc(ENUMLOGFONT FAR *lpelf, NEWTEXTMETRIC FAR *lpntm, int FontType, LPARAM lParam)
{
	if(FontType != TRUETYPE_FONTTYPE)
		return TRUE;
	if(*(lpelf->elfFullName + 0) == '@')
		return TRUE;
	((CListBox*)lParam)->AddString((const char*)lpelf->elfFullName);
	return TRUE;
}

void CFontSizeDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	LOGFONT logfont;
	logfont.lfCharSet = ANSI_CHARSET;
	strcpy(logfont.lfFaceName, "");
	logfont.lfPitchAndFamily = 0;

	EnumFontFamiliesEx(pDC->m_hDC, &logfont, (FONTENUMPROC)EnumFontFamProc, (LPARAM)&m_lbFonts, 0);

	m_lbFonts.SelectString(0, m_strCurrent);
	if((m_intFontStyle & FontStyleBold) > 0)
		m_bBold = TRUE;
	if((m_intFontStyle & FontStyleItalic) > 0)
		m_bItalic = TRUE;

	UpdateData(FALSE);
}

void CFontSizeDlg::OnCheckBold() 
{
	UpdateData(TRUE);
	if(m_bBold)
		m_intFontStyle |= FontStyleBold;
	else
		m_intFontStyle &= ~FontStyleBold;
}

void CFontSizeDlg::OnCheckItalic() 
{
	UpdateData(TRUE);
	if(m_bItalic)
		m_intFontStyle |= FontStyleItalic;
	else
		m_intFontStyle &= ~FontStyleItalic;
}
