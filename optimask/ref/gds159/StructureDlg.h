#if !defined(AFX_STRUCTUREDLG_H__A23401DD_D678_41D5_BC69_B96D9CDF95B2__INCLUDED_)
#define AFX_STRUCTUREDLG_H__A23401DD_D678_41D5_BC69_B96D9CDF95B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
#include "gdsDoc.h"
#include "gdsView.h"
#include "TextDumpDlg.h"

class CStructureDlg : public CDialog
{
public:
	void SetExpandLevel(int level);
	void SetWindowName(CString name);
	void SelectActive();
	void SetCaller(CGdsView* caller);
	void ExitDialog();
	BOOL* m_pBoolEnabler;
	void SetEnableSwitch(BOOL* enabler);
	void SetActive(CString name);
	void MakeTree(CGDSStructure* pStr, HTREEITEM parent);
	void MakeTreeText(CGDSStructure* pStr, int level, CString header, CString* edit);
	void SetStructure(CGDS* pGDS);
	CStructureDlg(CWnd* pParent = NULL);
	void SetList();
	
	//{{AFX_DATA(CStructureDlg)
	enum { IDD = IDD_STRUCTURE_DLG };
	CComboBox	m_comboStructure;
	CTreeCtrl	m_ctrlStructure;
	int		m_intSubLevel;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CStructureDlg)
	public:
	virtual int DoModal();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CStructureDlg)
	afx_msg void OnSelchangedStrTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClickStrTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnItemexpandedStrTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpandingStrTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusStrTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	virtual void OnOK();
	afx_msg void OnSelchangeStructure();
	virtual void OnCancel();
	afx_msg void OnChangeSubLevel();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonDumpHierarchy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CGdsView* m_gdsViewCaller;
	CGDS* m_gdsGDS;
	CString m_strPrev;
	CTextDumpDlg* m_pTextDumpDlg;
	BOOL m_bTextDumpDlg;
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_STRUCTUREDLG_H__A23401DD_D678_41D5_BC69_B96D9CDF95B2__INCLUDED_)
