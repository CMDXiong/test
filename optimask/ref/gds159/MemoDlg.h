#if !defined(AFX_MEMODLG_H__350F4ED2_ACC7_487B_8D0F_7E65A11447C7__INCLUDED_)
#define AFX_MEMODLG_H__350F4ED2_ACC7_487B_8D0F_7E65A11447C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////

class CMemoDlg : public CDialog
{
public:
	CString UpdateMemoString();
	void OnApply(){OnOK();}
	void ExitDialog();
	void SetEnabler(BOOL* enabler);
	CMemoDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(CMemoDlg)
	enum { IDD = IDD_MEMO_DLG };
	CEdit	m_editMemo;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CMemoDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CMemoDlg)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnButtonFont();
	afx_msg void OnChangeEditMemo();
	afx_msg void OnUpdateEditMemo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL* m_pEnabler;
	CWnd*  m_pParent;
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_MEMODLG_H__350F4ED2_ACC7_487B_8D0F_7E65A11447C7__INCLUDED_)
