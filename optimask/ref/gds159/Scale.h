//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCALE_H__0F9174DE_FA9B_45F8_B2A5_F1DC805B0856__INCLUDED_)
#define AFX_SCALE_H__0F9174DE_FA9B_45F8_B2A5_F1DC805B0856__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Gdshcy.h"

class CPoints : public CObject
{
public:
	CCoord p1;
	CCoord p2;
	CPoints& operator=(CPoints& arg)
	{
		p1 = arg.p1;
		p2 = arg.p2;
		return *this;
	}
	CPoints(const CPoints &other)
	{
		p1.x = other.p1.x;
		p1.y = other.p1.y;
		p2.x = other.p2.x;
		p2.y = other.p2.y;
	}
	CPoints()
	{
		p1 = CCoord(0, 0);
		p2 = CCoord(0, 0);
	}
};

class CDrawParam
{
public:
	REAL   m_realPenWidth;
	REAL   m_realPenWidthLim;
	CCoord m_coordViewOffset;
	CCoord m_coordMax;
	CCoord m_coordMin;
	double m_dblDrawScale;
	COLORREF m_colorBK;
	BOOL m_bFillEnable;
	CPoint m_scrollPos;
	SIZE   m_sizeScroll;
	BOOL popEnable;
	BOOL m_bDrawByLayer;
	BOOL m_bOverWrite;
public:
	CDrawParam(){popEnable = FALSE;}
	~CDrawParam(){}

	void PushParam(CView* view);
	void PopParam(CView* view);
};

class CMemoElement : public CObject
{
public:
	CCoord pos;
	CString memo;
	LOGFONT logfont;
	CMemoElement();
};

class CMemo
{
	BOOL	 m_bDragging;
	CCoord   coord;
	CString  memo;
	CObList  m_listMemos;
	CMemoElement* m_memoActive;
	LOGFONT  logfont;
public:
	CMemo();
	virtual ~CMemo();

	void Reset();
	CMemoElement* GetLastMemo();
	void DeleteAll();
	BOOL DeleteLast(CMemoElement* pDel);
	void AddMemo();
	int  GetNumMemos(){return m_listMemos.GetCount();}
	CMemoElement* OpenFirstMemo();
	CMemoElement* OpenNextMemo();
	void CloseMemo(){m_memoActive = NULL;}
	void SetDragging(BOOL set = TRUE){m_bDragging = set;}
	BOOL IsDragging(){return m_bDragging;}

	void SetFont(LOGFONT font);
	LOGFONT* GetFont(){return &logfont;}
	void SetPosition(CCoord pos){coord = pos;}
	CCoord GetPosition(){return coord;}
	CString GetMemoStr(){return memo;}
	void SetMemoStr(CString str){memo = str;}
};

class CScale  
{
	BOOL	 m_bDragging;
	BOOL     m_bMeasuring;
	int		 m_intScaleDir;
	CCoord   p1, p2;
	CObList  m_listPoints;
	CPoints* m_pointActive;
public:
	BOOL DeleteLast(CPoints* del);
	void AddScale(CCoord p1, CCoord p2);
	void AddScale();
	CPoints* OpenFirstPoints();
	CPoints* OpenNextPoints();
	void ClosePoints(){m_pointActive = NULL;}
	CScale();
	virtual ~CScale();
	void DeleteAll();
	void SetPoint1(CCoord pos)
	{
		p1 = pos;
		m_bMeasuring = TRUE;
	}
	void SetPoint2(CCoord pos, BOOL clear = TRUE)
	{
		p2 = pos;
		if(clear){
			m_bMeasuring = FALSE;
			m_bDragging = FALSE;
		}
	}
	CCoord GetPoint1(){return p1;}
	CCoord GetPoint2(){return p2;}
	BOOL IsMeasuring(){return m_bMeasuring;}
	void Reset(){m_bMeasuring = FALSE;}
	void SetDragging(BOOL set = TRUE){m_bDragging = set;}
	BOOL IsDragging(){return m_bDragging;}
	int  GetScaleDirection(){return m_intScaleDir;}
	void SetScaleDirection(int dir)
	{
		m_intScaleDir = dir;
		CWinApp* app = AfxGetApp();
		app->WriteProfileInt("Measure", "Measure direction", m_intScaleDir);
	}
	int  GetNumScales(){return m_listPoints.GetCount();}
};

#endif // !defined(AFX_SCALE_H__0F9174DE_FA9B_45F8_B2A5_F1DC805B0856__INCLUDED_)
