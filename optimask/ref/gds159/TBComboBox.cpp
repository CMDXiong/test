// TBComboBox.cpp : Implementation file
//

#include "stdafx.h"
#include "gds.h"
#include "TBComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTBComboBox

CTBComboBox::CTBComboBox()
{
	m_pView = NULL;
	m_bControl = FALSE;
}

CTBComboBox::~CTBComboBox()
{
}


BEGIN_MESSAGE_MAP(CTBComboBox, CComboBox)
	//{{AFX_MSG_MAP(CTBComboBox)
	ON_WM_KEYDOWN()
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_CONTROL_REFLECT(CBN_SELENDCANCEL, OnSelendcancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTBComboBox message handler

void CTBComboBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CTBComboBox::PreTranslateMessage(MSG* pMsg) 
{
    switch (pMsg->message){
	case WM_KEYDOWN:
		switch(pMsg->wParam){
		case VK_CONTROL:
			m_bControl = TRUE;
			return FALSE;
			break;
		default:
			if(m_bControl)
				return TRUE;
			break;
		}
	case WM_KEYUP:
		switch(pMsg->wParam){
		case VK_CONTROL:
			m_bControl = FALSE;
			return FALSE;
			break;
		default:
			break;
		}
		break;
	}
	return CComboBox::PreTranslateMessage(pMsg);
}

void CTBComboBox::OnKillfocus() 
{
	m_bControl = FALSE;
}

void CTBComboBox::OnSetfocus() 
{
	m_bControl = FALSE;
}

#include "GdsDoc.h"
#include "GdsView.h"
void CTBComboBox::OnSelchange() 
{
	UpdateData(FALSE);
	if(m_pView){
		m_pView->LockWindowUpdate();
		m_pView->SetFocus();
	}
	return;
}


void CTBComboBox::SetCurSel(CString str)
{
	int idx = 0;
	while(1){
		CString name;
		idx = FindStringExact(idx, str);
		if(idx == CB_ERR)
			return;
		GetLBText(idx, name);
		if(name == str)
			break;
	}
	CComboBox::SetCurSel(idx);
}

void CTBComboBox::OnDropdown() 
{
	
}

void CTBComboBox::OnSelendcancel() 
{
	if(m_pView)
		m_pView->SetFocus();
}
