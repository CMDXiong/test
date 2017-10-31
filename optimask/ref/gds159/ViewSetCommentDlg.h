#if !defined(AFX_VIEWSETCOMMENTDLG_H__1CFF196A_4F6E_48B7_9611_4942F3712D50__INCLUDED_)
#define AFX_VIEWSETCOMMENTDLG_H__1CFF196A_4F6E_48B7_9611_4942F3712D50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CViewSetCommentDlg : public CDialog
{
public:
	CViewSetCommentDlg(CWnd* pParent = NULL);
	CString GetComment(){return m_strComment;}
	void OnOK()
	{
		UpdateData(TRUE);
		CDialog::OnOK();
	}
	void SetComment(CString str){m_strComment = str;}

	//{{AFX_DATA(CViewSetCommentDlg)
	enum { IDD = IDD_VIEW_SET_COMMENT };
	CString	m_strComment;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CViewSetCommentDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CViewSetCommentDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_VIEWSETCOMMENTDLG_H__1CFF196A_4F6E_48B7_9611_4942F3712D50__INCLUDED_)
