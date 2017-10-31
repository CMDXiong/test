#include "stdafx.h"
#include "gds.h"
#include "ViewSetCommentDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CViewSetCommentDlg::CViewSetCommentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CViewSetCommentDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewSetCommentDlg)
	m_strComment = _T("");
	//}}AFX_DATA_INIT
}


void CViewSetCommentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewSetCommentDlg)
	DDX_Text(pDX, IDC_EDIT_VIEW_COMMENT, m_strComment);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewSetCommentDlg, CDialog)
	//{{AFX_MSG_MAP(CViewSetCommentDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

