#include "stdafx.h"
#include "gds.h"
#include "TimeStampDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////


CTimeStampDlg::CTimeStampDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTimeStampDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimeStampDlg)
	m_strCreate = _T("");
	m_strAccess = _T("");
	m_strDBName = _T("");
	//}}AFX_DATA_INIT
}


void CTimeStampDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimeStampDlg)
	DDX_Text(pDX, IDC_EDIT_CREATION_DATE, m_strCreate);
	DDX_Text(pDX, IDC_EDIT_ACCESS_DATE, m_strAccess);
	DDX_Text(pDX, IDC_EDIT_DB_NAME, m_strDBName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimeStampDlg, CDialog)
	//{{AFX_MSG_MAP(CTimeStampDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

#include <time.h>

void CTimeStampDlg::SetCreationDate(tm datetime)
{
	time_t tm_t = mktime(&datetime);
	struct tm* g = localtime(&tm_t);
	m_strCreate = asctime(g);
}

void CTimeStampDlg::SetAccessDate(tm datetime)
{
	time_t tm_t = mktime(&datetime);
	struct tm* g = localtime(&tm_t);
	m_strAccess = asctime(g);
}

void CTimeStampDlg::SetDBName(CString name)
{
	m_strDBName = name;
}
