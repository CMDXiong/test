#if !defined(AFX_STRUCTUREINFODLG_H__DD50C36B_E43A_440A_AF28_71BE7557F98D__INCLUDED_)
#define AFX_STRUCTUREINFODLG_H__DD50C36B_E43A_440A_AF28_71BE7557F98D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
#include "GDSDoc.h"
#include "GDSView.h"

class CStructureInfoDlg : public CDialog
{
	BOOL*		m_pEnabler;
	CGdsView*	m_pView;
public:
	void SetInformation();
	void	SetEnabler(BOOL* p){m_pEnabler = p;}
	virtual void OnCancel();
	void ExitDialog();
	void ClearText();
	CStructureInfoDlg(CWnd* pParent);


	//{{AFX_DATA(CStructureInfoDlg)
	enum { IDD = IDD_STRUCTURE_INFO };
	CRichEditCtrl	m_reditStrInfo;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CStructureInfoDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CStructureInfoDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_STRUCTUREINFODLG_H__DD50C36B_E43A_440A_AF28_71BE7557F98D__INCLUDED_)
