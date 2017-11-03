#include "stdafx.h"
#include "gds.h"

#include "gdsDoc.h"
#include "gdsView.h"
#include "GDShcy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CGdsDoc

IMPLEMENT_DYNCREATE(CGdsDoc, CDocument)

BEGIN_MESSAGE_MAP(CGdsDoc, CDocument)
	//{{AFX_MSG_MAP(CGdsDoc)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN_SAME_TEC, OnUpdateFileOpenSameTec)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CGdsDoc::CGdsDoc()
{
}

CGdsDoc::~CGdsDoc()
{
}

BOOL CGdsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////

void CGdsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())	{
	}
	else	{
	}
}

/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void CGdsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGdsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

#include "MainFrm.h"
#include "ProgDlg.h"

static CProgressDlg* pDlg = NULL;
static clock_t start_t;

static void StepCancelDlg()
{
	if(!pDlg)
		return;
	pDlg->OnCancel();
}

static void StepProgDlg()
{
	if(!pDlg)
		return;
	pDlg->StepIt();

	char str[256];
	pDlg->GetStatus(str, sizeof(str));
	CString msg = str;

	int idx = msg.Find("...");
	if(idx >= 0)
		msg = msg.Left(idx + 3);

	clock_t end_t = clock();
	double duration = (double)(end_t - start_t) / CLOCKS_PER_SEC;

	CString tm;
	tm.Format("%12.1fsec passed", duration);
	msg += tm;
	pDlg->SetStatus(msg);
}

static BOOL CheckStepProgDlgCancel()
{
	if(pDlg)
		return pDlg->CheckCancelButton();
	return FALSE;
}

BOOL CGdsDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	POSITION pos = GetFirstViewPosition();
	while(pos){
      CGdsView* pView = (CGdsView*)GetNextView(pos);
	  pView->LockWindowUpdate();
	}


	int step = 5;
	CProgressDlg dlg(0);
	dlg.Create(AfxGetApp()->GetMainWnd());
	dlg.SetStep(step);
	dlg.SetStatus("Reading GDSII file ...");
	pDlg = &dlg;

	clock_t end_t;
	start_t = clock();

	CString name;

	if(!m_gdsGDS.ReadGDS(lpszPathName, name, StepCancelDlg, StepProgDlg, step, CheckStepProgDlgCancel)){
		pDlg = NULL;
		return FALSE;
	}

	SetPathName(name);

	end_t = clock();

	((CMainFrame*)(AfxGetApp()->GetMainWnd()))->SetStatusBarDrawTime((double)(end_t - start_t), FALSE);
	m_strLibTitle = m_gdsGDS.GetLibName();

	pos = GetFirstViewPosition();
	while(pos){
      CGdsView* pView = (CGdsView*)GetNextView(pos);
	  pView->UnlockWindowUpdate();
	}
	if(pDlg == NULL)
		return FALSE;
	else
		pDlg = NULL;

	return TRUE;
}

BOOL CGdsDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	//return CDocument::OnSaveDocument(lpszPathName);
	return TRUE;
}



void CGdsDoc::SetTitle(LPCTSTR lpszTitle) 
{
	CString title;

	if(m_strDocTitle.GetLength() == 0){
		m_strDocTitle = this->m_strPathName;
		if(m_strDocTitle.GetLength() > 32){
			int idx = m_strDocTitle.ReverseFind('\\');
			if(idx > 0){
				CString tmp = m_strDocTitle.Left(idx);
				idx = tmp.ReverseFind('\\');
				if(idx > 0){
					tmp = tmp.Right(tmp.GetLength() - idx - 1);
					m_strDocTitle = "...\\" + tmp + "\\" + lpszTitle;
				}
			}
		}
		//m_strDocTitle = lpszTitle;
	}

	title = m_strDocTitle;
	if(m_strTecFile.GetLength() > 0){
		int idx = m_strTecFile.ReverseFind('\\');
		CString tec = m_strTecFile;
		if(idx > 0){
			tec = tec.Right(tec.GetLength() - idx - 1);
			title += ": " + tec;
		}
	}
	CDocument::SetTitle(title);
}



void CGdsDoc::OnUpdateFileOpenSameTec(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}
