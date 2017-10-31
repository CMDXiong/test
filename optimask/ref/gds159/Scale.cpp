//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Scale.h"
#include "GdsDoc.h"
#include "GdsView.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CMemoElement::CMemoElement()
{
	pos = CCoord(0, 0);
	memo = "";
 	CWinApp* app = AfxGetApp();
	logfont.lfHeight = 	app->GetProfileInt("Memo", "TextHeight", 0);
	logfont.lfWidth = 0;
	logfont.lfEscapement = 0;
	logfont.lfOrientation = 0;
	logfont.lfWeight = 	app->GetProfileInt("Memo", "TextWeight", FW_DONTCARE);
	CString fontname = app->GetProfileString("Memo", "Font face", "Courier New");
	strncpy(logfont.lfFaceName, fontname, LF_FACESIZE);
	logfont.lfCharSet = app->GetProfileInt("Memo", "Charset", ANSI_CHARSET);
	logfont.lfEscapement = 0;
	logfont.lfOrientation = 0;
	logfont.lfItalic = FALSE;
	logfont.lfUnderline = FALSE;
	logfont.lfStrikeOut = FALSE;
	logfont.lfOutPrecision = OUT_TT_PRECIS;
	logfont.lfClipPrecision = CLIP_TT_ALWAYS;
	logfont.lfQuality = PROOF_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

}


CMemo::CMemo()
{
	coord.x = coord.y = 0;
	memo = "";
	m_bDragging = FALSE;

 	CWinApp* app = AfxGetApp();
	logfont.lfHeight = 	app->GetProfileInt("Memo", "TextHeight", 0);
	logfont.lfWidth = 0;
	logfont.lfEscapement = 0;
	logfont.lfOrientation = 0;
	logfont.lfWeight = 	app->GetProfileInt("Memo", "TextWeight", FW_DONTCARE);
	CString fontname = app->GetProfileString("Memo", "Font face", "Courier New");
	strncpy(logfont.lfFaceName, fontname, LF_FACESIZE);
	logfont.lfCharSet = app->GetProfileInt("Memo", "Charset", ANSI_CHARSET);
	logfont.lfEscapement = 0;
	logfont.lfOrientation = 0;
	logfont.lfItalic = FALSE;
	logfont.lfUnderline = FALSE;
	logfont.lfStrikeOut = FALSE;
	logfont.lfOutPrecision = OUT_TT_PRECIS;
	logfont.lfClipPrecision = CLIP_TT_ALWAYS;
	logfont.lfQuality = PROOF_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
}

CMemo::~CMemo()
{
	DeleteAll();
}

void CMemo::SetFont(LOGFONT font)
{
	logfont = font;

 	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Memo", "TextHeight", logfont.lfHeight);
	app->WriteProfileInt("Memo", "TextWeight", logfont.lfWeight);
	app->WriteProfileString("Memo", "Font face", logfont.lfFaceName);
	app->WriteProfileInt("Memo", "Charset", logfont.lfCharSet);
}

void CMemo::Reset()
{
	coord = CCoord(0, 0);
	memo = "";
	m_bDragging = FALSE;
}

void CMemo::DeleteAll()
{
	POSITION pos = m_listMemos.GetHeadPosition();
	while(pos){
		CMemoElement* ptr = (CMemoElement*)m_listMemos.GetNext(pos);
		delete ptr;
	}
	m_listMemos.RemoveAll();
	m_memoActive = NULL;
}

void CMemo::AddMemo()
{
	if(memo.GetLength() == 0)
		return;
	POSITION pos = m_listMemos.GetTailPosition();
	CMemoElement* ptr = new CMemoElement;
	ptr->memo = memo;
	ptr->pos = coord;
	ptr->logfont = logfont;
	m_listMemos.InsertAfter(pos, ptr);
	m_bDragging = FALSE;
}

CMemoElement* CMemo::GetLastMemo()
{
	if(m_listMemos.GetCount() == 0)
		return NULL;
	POSITION pos = m_listMemos.GetTailPosition();
	if(pos)
		return (CMemoElement*)m_listMemos.GetAt(pos);
	else
		return NULL;
}

BOOL CMemo::DeleteLast(CMemoElement* pDel)
{
	POSITION pos = m_listMemos.GetTailPosition();
	if(pos){
		CMemoElement* ptr = (CMemoElement*)m_listMemos.GetAt(pos);
		pDel->pos = ptr->pos;
		pDel->memo = ptr->memo;
		pDel->logfont = ptr->logfont;
		delete ptr;
		m_listMemos.RemoveAt(pos);
		return TRUE;
	}
	else
		return FALSE;
}

CMemoElement* CMemo::OpenFirstMemo()
{
	if(!m_listMemos.IsEmpty())
		m_memoActive = (CMemoElement*)m_listMemos.GetHead();
	else
		m_memoActive = NULL;
	return m_memoActive;
}

CMemoElement* CMemo::OpenNextMemo()
{
	POSITION pos = m_listMemos.Find(m_memoActive);
	m_listMemos.GetNext(pos);
	if(pos == NULL){
		m_memoActive = NULL;
		return NULL;
	}
	m_memoActive = (CMemoElement*)m_listMemos.GetAt(pos);

	return m_memoActive;
}



CScale::CScale()
{
	m_bMeasuring = FALSE;
	m_pointActive = NULL;
	m_bDragging = FALSE;

	CWinApp* app = AfxGetApp();
	m_intScaleDir =	app->GetProfileInt("Measure", "Measure direction", MEAS_DIR_XY);
}

CScale::~CScale()
{
	DeleteAll();
}


void CScale::DeleteAll()
{
	POSITION pos = m_listPoints.GetHeadPosition();
	while(pos){
		CPoints* ppoints = (CPoints*)m_listPoints.GetNext(pos);
		delete ppoints;
	}
	m_listPoints.RemoveAll();
	m_pointActive = NULL;
}

void CScale::AddScale(CCoord p1, CCoord p2)
{
	POSITION pos = m_listPoints.GetTailPosition();
	CPoints* ppoints = new CPoints;
	ppoints->p1 = p1;
	ppoints->p2 = p2;
	m_listPoints.InsertAfter(pos, ppoints);
	m_bDragging = FALSE;
	m_bMeasuring = FALSE;
}

void CScale::AddScale()
{
	if(p1.x == p2.x && p1.y == p2.y)
		return;
	POSITION pos = m_listPoints.GetTailPosition();
	CPoints* ppoints = new CPoints;
	ppoints->p1 = p1;
	ppoints->p2 = p2;
	m_listPoints.InsertAfter(pos, ppoints);
	m_bDragging = FALSE;
	m_bMeasuring = FALSE;
}

CPoints* CScale::OpenFirstPoints()
{
	if(!m_listPoints.IsEmpty())
		m_pointActive = (CPoints*)m_listPoints.GetHead();
	else
		m_pointActive = NULL;
	return m_pointActive;
}

CPoints* CScale::OpenNextPoints()
{
	POSITION pos = m_listPoints.Find(m_pointActive);
	m_listPoints.GetNext(pos);
	if(pos == NULL){
		m_pointActive = NULL;
		return NULL;
	}
	m_pointActive = (CPoints*)m_listPoints.GetAt(pos);

	return m_pointActive;
}


BOOL CScale::DeleteLast(CPoints* pDel)
{
	POSITION pos = m_listPoints.GetTailPosition();
	if(pos){
		CPoints* ppoints = (CPoints*)m_listPoints.GetAt(pos);
		pDel->p1 = ppoints->p1;
		pDel->p2 = ppoints->p2;
		delete ppoints;
		m_listPoints.RemoveAt(pos);
		return TRUE;
	}
	else
		return FALSE;
}

void CDrawParam::PushParam(CView* view){
	CGdsView* pView = (CGdsView*)view;
	m_coordViewOffset = pView->m_coordViewOffset;
	m_coordMax = pView->m_coordMax;
	m_coordMin = pView->m_coordMin;
	m_dblDrawScale = pView->m_dblDrawScale;
	m_colorBK = pView->m_colorBK;
	m_bFillEnable = pView->m_bFillEnable;
	m_scrollPos = pView->GetScrollPosition();
	m_sizeScroll = pView->GetTotalSize();
	m_bDrawByLayer = pView->GetDrawByLayer();
	m_bOverWrite = pView->GetOverwrite();
	m_realPenWidth = pView->GetPenWidth();
	m_realPenWidthLim = pView->GetPenWidthLim();
	popEnable = TRUE;
}

void CDrawParam::PopParam(CView* view){
	if(!popEnable)
		return;
	popEnable = FALSE;
	CGdsView* pView = (CGdsView*)view;
	pView->m_coordViewOffset = m_coordViewOffset;
	pView->m_coordMax = m_coordMax;
	pView->m_coordMin = m_coordMin;
	pView->m_dblDrawScale = m_dblDrawScale;
	pView->m_colorBK = m_colorBK;
	pView->m_bFillEnable = m_bFillEnable;
	pView->SetScrollSizes(MM_HIMETRIC, m_sizeScroll);
	pView->ScrollToPosition(m_scrollPos);
	pView->SetDrawByLayer(m_bDrawByLayer);
	pView->SetOverwrite(m_bOverWrite);
	pView->SetPenWidth(m_realPenWidth);
	pView->SetPenWidthLim(m_realPenWidthLim);
}
