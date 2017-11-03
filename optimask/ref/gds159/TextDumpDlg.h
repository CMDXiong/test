#if !defined(AFX_TEXTDUMPDLG_H__D3056BA2_74B4_465C_AFF1_81A501262787__INCLUDED_)
#define AFX_TEXTDUMPDLG_H__D3056BA2_74B4_465C_AFF1_81A501262787__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//

/////////////////////////////////////////////////////////////////////////////

class CTextDumpDlg : public CDialog
{
	BOOL* enabler;
public:
	CTextDumpDlg(CWnd* pParent = NULL);
	void ShowDump(CString str)
	{
		if(m_hWnd){
			m_editDump.SetLimitText(0xFFFFFFFF);
			m_editDump.SetSel(0, -1);
			m_editDump.ReplaceSel(str);
			m_editDump.LineScroll(-m_editDump.GetLineCount());
			m_editDump.SetSel(0, 0);
		}
	}
	void SetEnabler(BOOL* pEnabler)
	{
		enabler = pEnabler;
	}
	void OnCancel()
	{
		*enabler = FALSE;
		CDialog::OnCancel();
	}
	void OnOK()
	{
	}
	void ExitDialog()
	{
		OnCancel();
	}

	//{{AFX_DATA(CTextDumpDlg)
	enum { IDD = IDD_TEXT_DUMP };
	CEdit	m_editDump;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CTextDumpDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CTextDumpDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelectAllCell();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSaveTextDump();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_TEXTDUMPDLG_H__D3056BA2_74B4_465C_AFF1_81A501262787__INCLUDED_)
