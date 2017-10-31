#include "stdafx.h"
#include "gds.h"
#include "ZipSelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////


CZipSelDlg::CZipSelDlg(unzFile file, int numentry, CWnd* pParent /*=NULL*/)
	: CDialog(CZipSelDlg::IDD, pParent)
{
	zfile = file;
	numfiles = numentry;
	namesel = "";
	//{{AFX_DATA_INIT(CZipSelDlg)
	//}}AFX_DATA_INIT
}


void CZipSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZipSelDlg)
	DDX_Control(pDX, IDOK, m_buttonOK);
	DDX_Control(pDX, IDC_LIST_FILE, m_listFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CZipSelDlg, CDialog)
	//{{AFX_MSG_MAP(CZipSelDlg)
	ON_WM_SHOWWINDOW()
	ON_LBN_SELCHANGE(IDC_LIST_FILE, OnSelchangeListFile)
	ON_LBN_DBLCLK(IDC_LIST_FILE, OnDblclkListFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CZipSelDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	

	char filename[_MAX_PATH];
    unzGoToFirstFile(zfile);
	unz_file_info file_info;

	for(int i = 0;i < numfiles;i++){
		unzGetCurrentFileInfo(zfile, &file_info, filename, _MAX_PATH, NULL, 0, NULL, 0);
		unzGoToNextFile(zfile);
		char last = *(filename + strlen(filename) - 1);
		if((file_info.external_fa & 32) && last == '/')
			continue;
		m_listFile.AddString(filename);
	}
	unzGoToFirstFile(zfile);
	m_buttonOK.EnableWindow(FALSE);
}

void CZipSelDlg::OnSelchangeListFile() 
{
	int sel_idx	= m_listFile.GetCurSel();
	if(sel_idx >= 0)
		m_listFile.GetText(sel_idx, namesel);
	m_buttonOK.EnableWindow(TRUE);
}

void CZipSelDlg::OnDblclkListFile() 
{
	OnSelchangeListFile();
	OnOK();
}
