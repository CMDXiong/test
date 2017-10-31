#if !defined(AFX_TBCOMBOBOX_H__1EBDB150_B6BF_4476_A12F_D57402CE9299__INCLUDED_)
#define AFX_TBCOMBOBOX_H__1EBDB150_B6BF_4476_A12F_D57402CE9299__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TBComboBox.h : Header file
//

class CGdsView;

/////////////////////////////////////////////////////////////////////////////
// CTBComboBox window
class CTBComboBox : public CComboBox
{
	CGdsView* m_pView;
	BOOL	  m_bControl;
public:
	CTBComboBox();
	void SetView(CGdsView* view){m_pView = view;}

	//{{AFX_VIRTUAL(CTBComboBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

public:
	void SetCurSel(CString str);
	virtual ~CTBComboBox();

protected:
	//{{AFX_MSG(CTBComboBox)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();
	afx_msg void OnSelchange();
	afx_msg void OnDropdown();
	afx_msg void OnSelendcancel();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_TBCOMBOBOX_H__1EBDB150_B6BF_4476_A12F_D57402CE9299__INCLUDED_)
