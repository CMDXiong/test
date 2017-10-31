#if !defined(AFX_GDS_H__38AD7268_B01D_496E_9758_5A3475C12532__INCLUDED_)
#define AFX_GDS_H__38AD7268_B01D_496E_9758_5A3475C12532__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"
#include "Splash.h"
/////////////////////////////////////////////////////////////////////////////

class CGdsApp : public CWinApp
{
	HWND m_hwndGridDlg;
	HWND m_hwndCGdsView;
	CString tecfileForce;

public:
	void SetTecFile(CString tec)
	{
		tecfileForce = tec;
	}
	CString GetTecFile()
	{
		return tecfileForce;
	}

	CString GetExePath();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CGdsApp();
	~CGdsApp();
	void SetGridDlgHWND(HWND wnd){m_hwndGridDlg = wnd;}
	void SetCGdsViewHWND(HWND wnd){m_hwndCGdsView = wnd;}
	//{{AFX_VIRTUAL(CGdsApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);
	virtual BOOL OnIdle(LONG lCount);
	virtual int Run();
	virtual int ExitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	//}}AFX_VIRTUAL

	COleTemplateServer m_server;
	//{{AFX_MSG(CGdsApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFilePrintSetup();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenSameTec();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define DEFAULT_EXTENSION    "tec"

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_GDS_H__38AD7268_B01D_496E_9758_5A3475C12532__INCLUDED_)
