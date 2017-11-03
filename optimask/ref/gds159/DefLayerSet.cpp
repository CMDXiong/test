#include "stdafx.h"
#include "gds.h"
#include "DefLayerSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////


CDefLayerSet::CDefLayerSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDefLayerSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDefLayerSet)
	m_strFile = _T("");
	//}}AFX_DATA_INIT
}


void CDefLayerSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefLayerSet)
	DDX_Text(pDX, IDC_EDIT_FILE, m_strFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefLayerSet, CDialog)
	//{{AFX_MSG_MAP(CDefLayerSet)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDefLayerSet::OnButtonClear() 
{
	m_strFile =  "";
	UpdateData(FALSE);
}

void CDefLayerSet::OnButtonBrowse() 
{
	CString filt;
	filt.Format("TechnologyMap Files (*.%s)|*.%s|All Files (*.*)|*.*||", DEFAULT_EXTENSION, DEFAULT_EXTENSION);


	CFileDialog dlg(TRUE, DEFAULT_EXTENSION, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filt, NULL);
	CString path;

	if(dlg.DoModal() == IDCANCEL)
		return;
	m_strFile =  dlg.GetPathName();
	UpdateData(FALSE);
}
