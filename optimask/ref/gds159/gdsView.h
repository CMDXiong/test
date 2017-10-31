// gdsView.h : CGdsView
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_GDSVIEW_H__E6BB1E8A_6F5B_4EA2_9C08_5332EF3CE356__INCLUDED_)
#define AFX_GDSVIEW_H__E6BB1E8A_6F5B_4EA2_9C08_5332EF3CE356__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CStructureDlg;
class CSetCursor;
class CGridDlg;
class CMemoDlg;
class CStructureInfoDlg;
class CSelViewSetting;
class CDlgLayer;

#include "MainFrm.h"
#include "Scale.h"
#include "Grid.h"
#include "TBComboBox.h"
#include "MTRender.h"

#define TAN225              0.4142135623730950488016887242097
#define TAN675              2.4142135623730950488016887242097

#define HIMETRIC					0.01e-3 // meter
#define SCROLL_WIDTH				1.5     // meter
#define SCROLL_HEIGHT				1.5     // meter
#define TEXT_HEIGHT					5
#define DEFAULT_PRINT_PEN_WIDTH		0.1e-3 // meter
#define DEFAULT_PRINT_HATCH_SPACE	3.0e-3 // meter
#define NUM_POLYGON_POINT   GDS_MAX_COORD + 1




#define HATCH_DENSITY_DIV	400.0


#define FOOTER_MARGIN		(4.0e-3 / HIMETRIC)
#define FOOTER_HEIGHT		(3.5e-3 / HIMETRIC)
#define LEGEND_HEIGHT  		(3.0e-3)
#define SCALE_DIV_ONE		(0.75e-3 / HIMETRIC)
#define SCALE_DIV_FIV		(1.5e-3 / HIMETRIC)

#define HD_HATCH_RATIO		0.5
#define SCALE_FIT_RATIO     0.90
#define SCALE_TEXT_RATIO    0.04
#define SCALE_MEMO_RATIO    0.1
#define SCALE_GRID_RATIO    0.5
#define SCALE_PEN_WIDTH     20
#define GRID_MARKER_SIZE    5
#define SCALE_MARKER_RATIO  0.02
#define CENTER_MARK_RATIO   0.05

#define ZOOM_AREA_MIN       4
#define GRID_LIMIT          6
#define GRID_SIZE			2
#define DRAW_LIMIT          4
#define FILL_LIMIT          4
#define RESIZE_RECT_WIDTH   2
#define RECURSE_LEVEL       1023
#define DIAGONAL_MIN_RATIO  0.1   

#define COLOR_INVALID		  (0xffffffff)
#define COLOR_BLACK           RGB(  0,   0,   0)
#define COLOR_WHITE           RGB(255, 255, 255)
#define COLOR_RED             RGB(255,   0,   0)
#define COLOR_AXIS            RGB(150, 150, 150)
#define COLOR_BOUNDARY_FRAME  RGB(200, 200, 200)
#define COLOR_DUMMY_STRUCTURE RGB(120, 120, 120)
#define COLOR_BK_AT_PRINT     RGB(255, 255, 255)
#define COLOR_CURSOR          RGB(  0,   0,   0) //RGB(255, 255, 255)
#define COLOR_STRUCTURE_FRAME RGB(255,   0,   0)
#define COLOR_SCALE_FLOAT     RGB(255, 255, 255)
#define COLOR_SCALE_FIX       RGB(255, 200, 128)
#define COLOR_MEMO_FLOAT      RGB(255, 255, 255)
#define COLOR_MEMO_DEFAULT    RGB(255, 200, 128)
#define COLOR_GRID_DOT        RGB(100, 100, 100)
#define COLOR_GRID_LINE       RGB( 70,  70,  70)
#define COLOR_GRID_HIGHLIGHT  RGB(255, 255, 255)
#define COLOR_GRID_MARKER     RGB(255, 255, 255)
#define COLOR_HLT_STRUCTURE   RGB(  0, 255, 255)
#define TRANSPARENT_ALPHA     160

#define WM_USER_END_RENDER (WM_USER + 0x1000)

#define DEFAULT_GDS2_FONT     "Courier New"


enum {
	FILL_BMP_NONE = 0,
	FILL_BMP_XDIAG,
	FILL_BMP_FDIAG,
	FILL_BMP_RDIAG,
	FILL_BMP_XHATCH,
	FILL_BMP_VHATCH,
	FILL_BMP_HHATCH,
	FILL_BMP_SOLID,
	FILL_BMP_HZIGZAG,
	FILL_BMP_VZIGZAG,
	FILL_BMP_HDASH,
	FILL_BMP_VDASH,
	FILL_BMP_VWAVE,
	FILL_BMP_HWAVE,
	FILL_BMP_LIGHT,
	FILL_BMP_FDIAG_DASH,
	FILL_BMP_RDIAG_DASH,
	FILL_BMP_XHATCH_HD,
	FILL_BMP_SIZE,
};

class CViewSetting : public CObject
{
public:
	CViewSetting()
	{
		m_pStructure = NULL;
	}
	CCoord m_coordCenter;
	CCoord m_coordSize;
	CGDSStructure* m_pStructure;
	CString m_strComment;

	CViewSetting& operator = (CViewSetting& other)
	{
		m_coordCenter = other.m_coordCenter;
		m_coordSize = other.m_coordSize;
		m_pStructure = other.m_pStructure;
		return *this; 
	}
};

class CStrFrame : public CObject
{
	CGDSStructure*	structure;
	CGDSElement*    element;
	CCoord			area[2];
	double			angle;
	BOOL			m_bIsStructure;
	BOOL			m_bExpandThis;
	HENHMETAFILE    m_emfMetaFile;
public:
	inline BOOL operator==(CStrFrame& arg)
	{
		if(structure != arg.structure)
			return FALSE;
		if(element != arg.element)
			return FALSE;
		if(area[0] != arg.area[0])
			return FALSE;
		if(area[1] != arg.area[1])
			return FALSE;
		return TRUE;
	}

	CStrFrame()
	{
		m_bExpandThis = FALSE;
		angle = 0.0;
		structure = NULL;
		m_bIsStructure = TRUE;
		m_emfMetaFile = NULL;
	}
	~CStrFrame()
	{
		if(m_emfMetaFile)
			::DeleteEnhMetaFile(m_emfMetaFile);
	}
	void SetMetaFile(HENHMETAFILE emf){m_emfMetaFile = emf;}
	HENHMETAFILE GetMetaFile(){return m_emfMetaFile;}
	void SetExpand(BOOL expand)
	{
		m_bExpandThis = expand;
		if(!m_bExpandThis){
			if(m_emfMetaFile)
				::DeleteEnhMetaFile(m_emfMetaFile);
			m_emfMetaFile = NULL;
		}
	}
	BOOL GetExpand(){return m_bExpandThis;}
	void SetAngle(double agl){angle = agl;}
	double GetAngle(){return angle;}
	void SetArea(CCoord p1, CCoord p2)
	{
		area[0] = p1;
		area[1] = p2;
	}
	void GetArea(CCoord* p)
	{
		(p + 0)->Copy(area + 0);
		(p + 1)->Copy(area + 1);
	}
	CGDSStructure* GetStructure(){return structure;}
	void SetStructure(CGDSStructure* pstructure){structure = pstructure;}

	void SetElement(CGDSElement* pelement){element = pelement;}
	CGDSElement* GetElement(){return element;}
	void SetAref(BOOL aref = TRUE){m_bIsStructure = !aref;}
	BOOL IsSref(){return m_bIsStructure;}

	inline int compare_size(CStrFrame* other)
	{
		double s_this = fabs((area[1].x - area[0].x) * (area[1].y - area[0].y));
		double s_other = fabs((other->area[1].x - other->area[0].x) * (other->area[1].y - other->area[0].y));
		if(s_this > s_other)
			return 1;
		else if(s_this <  s_other)
			return -1;
		return 0;
	}
	inline BOOL operator > (CStrFrame& other)
	{
		double s_this = fabs((area[1].x - area[0].x) * (area[1].y - area[0].y));
		double s_other = fabs((other.area[1].x - other.area[0].x) * (other.area[1].y - other.area[0].y));
		if(s_this >= s_other)
			return TRUE;
		return FALSE; 
	}
	inline BOOL operator < (CStrFrame& other)
	{
		double s_this = fabs((area[1].x - area[0].x) * (area[1].y - area[0].y));
		double s_other = fabs((other.area[1].x - other.area[0].x) * (other.area[1].y - other.area[0].y));
		if(s_this >= s_other)
			return FALSE;
		return TRUE; 
	}
};

class CPointer : public CObject
{
	DWORD	m_pPointer;
public:
	CPointer(){m_pPointer = NULL;}
	~CPointer(){}
	void SetPointer(DWORD addr){m_pPointer = addr;}
	DWORD GetPointer(){return m_pPointer;}
};


class CHatchStyle : public CObject
{
public:
	double	m_dblPenWidth;
	double  m_dblSpace;
	double	m_dblAngle;
	double  m_dblDashRatio;
	double  m_dblOffsetX;
	double  m_dblOffsetY;
	BOOL    m_bPhaseShift;
	int     m_intLineShape;
	BOOL    m_bCross;
public:
	enum {
		HatchLineStraight,
		HatchLineZigzag,
		HatchLineWave,
	};
	CHatchStyle()
	{
		m_bPhaseShift = FALSE;
		m_dblPenWidth = 0.1e-3;
		m_dblSpace = 5e-3;
		m_dblAngle = 0.0;
		m_dblDashRatio = 0.5;
		m_dblOffsetX = 0.0;
		m_dblOffsetY = 0.0;
		m_intLineShape = HatchLineStraight;
		m_bCross = FALSE;
	}
	~CHatchStyle(){}
};

class CGdsView : public CScrollView
{
	COleDropTarget m_oleDropTarget;

	PointF points[NUM_POLYGON_POINT];

	friend class CStructureInfoDlg;
	friend class CGridDlg;
	friend class CMemoDlg;
	friend class CDrawParam;

protected:
	CGdsView();
	DECLARE_DYNCREATE(CGdsView)
	virtual ~CGdsView();

public:
	//{{AFX_VIRTUAL(CGdsView)
	public:
	virtual void OnScrollLeft();
	virtual void OnScrollDown();
	virtual void OnScrollRight();
	virtual void OnScrollUp();
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnRenderAllFormats(); 
	virtual void OnRenderFormat(UINT format); 
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

public:
	CGdsDoc* GetDocument();

public:
	afx_msg void OnUpdateEditCopyCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg BOOL OnNotifyNeedText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );

public:
	void LPtoDBUnit(REAL* x, REAL* y);
	inline BOOL NeedClip(PointF* p);
	BOOL IsGridVisible();
	void UpdateLayer();
	void SyncToAnotherView(CGdsView* pView);
	BOOL IsHatchEnabled(){return m_bFillEnable;}

	CObArray& GetViewSetting(){return m_arrayViewSetting;}
	void ReorderLayer(CLayerSet* layers);
	void ReadTecFile();
	COLORREF ReverseGray(COLORREF rgb);
	CGDS* GetGDShcy(){return GetDocument()->GetGDS();}
	void SortSkelton();
	BOOL ReadWriteIniFile(BOOL read = TRUE);
	void ShowBufferBmp();
	void UnlockWindowUpdate();
	void LockWindowUpdate();
	void DrawPrintFooter();
	void DrawPrintScale();
	int  DrawPrintLegend();
	REAL GetPenWidth(){return m_realPenWidth;}
	REAL GetPenWidthLim(){return m_realPenWidthLim;}
	void SetPenWidth(REAL width){m_realPenWidth = width;}
	void SetPenWidthLim(REAL width){m_realPenWidthLim = width;}
	void ScrollToPosition(CPoint pos);
	int GetEncoderClsid(CString format, CLSID* pClsid);
	BOOL SaveImageAsFile(BOOL whole);
	BOOL GetOverwrite(){return m_bOverWrite;}
	void SetOverwrite(BOOL val){m_bOverWrite = val;}
	BOOL GetDrawByLayer(){return m_bDrawByLayer;}
	void SetDrawByLayer(BOOL val){m_bDrawByLayer = val;}
	void LPtoMicron(CPoint point, double* x, double* y);
	void OnEndRender();
	void InvalidateRect(BOOL stop_draw, LPCRECT lpRect, BOOL bErase = TRUE);
	void StopDraw(){OnButtonStopDraw();}
	void Render(CDC* pDC = NULL);
	void KillRenderThread();
	void CreateRenderThread();
	void DrawSingleStructure(CStrFrame* pFrame);
	BOOL ExpandCell();
	void WriteAllCellInfo(CFile &file);

	void DeleteDottedFrame(CDC *pDC);
	void WriteStructureInfo(CFile &file, CStrFrame* frame = NULL);
	void OnStructureContext(CPoint point);
	CGDSStructure* GetActiveStructure(){return m_pStrActive;}
	void DrawMemos(CDC* pDC = NULL);
	void DrawMemo(CMemoElement* ptr, CDC* pDC = NULL);
	void OnGridNone();
	void OnGridDot();
	void OnGridLine();
	void SetMeasureDirection(int dir){m_scale.SetScaleDirection(dir);}
	int  GetMeasureDirection(){return m_scale.GetScaleDirection();}
	void OnKeyEscape();
	void DrawScales(CDC* pDC = NULL);
	void DrawScale(CCoord p1, CCoord p2, CDC* pDC = NULL);
	void AbortMeasuring(BOOL del = FALSE);
	void DrawComboBox(BOOL on = TRUE);
	void DrawRubberScale(CPoint point);
	void DrawGridMarker();
	void DrawGridMarker(CPoint point);
	void DrawGrid(CDC* pDC);
	void ZoomFitWindow(BOOL redraw = TRUE);
	BOOL IsSelected();
	void DoCutCopyPaste(CArchive &ar, BOOL bCut);
	CPoint PanToPosMeter(CCoord point);
	CPoint PanToPos(CCoord point, BOOL panto = TRUE);
	void UpdateSubStructureLevel(int level);
	void DlgStructureChanged();
	void DrawAxis(BOOL draw = TRUE);
	void CreateStrLvlBox(CRect rect, CToolBar* tb);
	HENHMETAFILE WriteToEmf(BOOL whole = FALSE);
	double OnFitView(BOOL examine = FALSE, BOOL view_to_paper = FALSE, BOOL optshrink = FALSE);
	void CreateComboBox(CRect rect, CToolBar* tb);
	void DrawCenterMark();
	void SetDefaultLayerColor();
    void SetOrigin(CDC* pDC);
    void OnZoomArea();
    void OnZoom(int zoomUp);
	BOOL PanTo(BOOL client = TRUE, CPoint pos = CPoint(0, 0));
	void DisableStatusBarDisplay(BOOL drwtime = FALSE);
	int GetROP(CDC* pDC);

	void AddSkeltonStructure(CGDSStructure* structure, double angle,
								CCoord* area, BOOL isSref, CGDSElement* element);

	void DrawSkeltonStructure(CDC* pDC, CStrFrame* pFrame, int level);
	void RoundToUserGrid(double* x, double* y, BOOL snap = TRUE);

	void FillBackground(BOOL screen = TRUE);
	BOOL IsDrawing(){return m_bCancelDraw;}
	void UpdateXYCoord(CPoint cursor, CDC* pDC = NULL);
	void CalcStructureArea(CStrFrame* pFrame, CRect& rect);


	int GetLayerStatus(int layer);

	inline void ClipPath(GraphicsPath* pPath, BOOL horizon = FALSE, int posneg = 0);
	inline void DrawStructureFrame(CCoord* pframe, CGDSStructure* structure);

	inline BOOL IsOutOfView(CDC* pDC, CRect rect);
	inline BOOL IsOutOfView(CRect rect);

	inline BOOL DrawGDSElement(CGDSElement* element, 
		                       CCoord& pos,  BOOL reflect, double& angle, double& mag);


	inline void CalcStructureFrame(CCoord* rect4, CGDSStructure* structure, 
							   CCoord& pos,  BOOL reflec, double& angle, double& mag);

	inline void CalcStructureArea(CCoord* rect, CGDSStructure* structure, 
							   CCoord& pos,  BOOL reflec, double& angle, double& mag);

	inline void DrawDummyStructure(CGDSStructure* structure, 
							 CCoord& pos, BOOL reflect, double& angle, double& mag);

	inline BOOL CheckStructureFrame(int level, CGDSStructure* structure, 
							 CCoord& pos, BOOL reflect, double& angle, double& mag,
							 CGDSElement* element = NULL);

	void DrawStructure(int level, CGDSStructure* structure, 
					   CCoord& pos,  BOOL reflect, double& angle, double& mag);

	void DrawAref(int level, CGDSAREF* element, CCoord& pos, BOOL reflect, double& angle, double& mag);

	inline void DrawDummySkelton(CDC* pDC, CRect* rect, COLORREF color);

	inline void fmod360(double& angle);
	inline void RotatePoint(CCoord* point, double& angle);
	inline void RotatePoints(CCoord* point, double& angle, unsigned num);
	inline void RotatePoints(PointF* point, double& angle, unsigned num);
	inline void RotatePoints(CPoint* point, double& angle, unsigned num);
	inline void RotateRect(CRect* rect, double& angle);

	inline void DrawRect(CRect* pRect);
	inline void FillPolygon(GraphicsPath* path, BOOL legend = FALSE);
	inline void FillPolygonScreen(GraphicsPath* path);
	inline void FillPolygonPrint(GraphicsPath* path, BOOL legend = FALSE);

	inline int DrawDummyElement(CGDSElement* element, 
							   CCoord& pos,  BOOL reflect, double& angle, double& mag);
	inline BOOL DrawGDSPath(CGDSPath *element, 
						   CCoord pos, BOOL reflect, double angle, double mag);
	inline BOOL DrawGDSBoundary(CGDSBoundary* element, 
							   CCoord pos,  BOOL reflect, double angle, double mag);
	inline void DrawGDSText(CGDSText* element,
							   CCoord pos,  BOOL reflect, double angle, double mag);

	//void DrawGDSNode(CGDSNode* element,
	//						   CCoord pos,  BOOL reflect, double angle, double mag);
	//void DrawGDSContact(CGDSNode* element,
	//						   CCoord pos,  BOOL reflect, double angle, double mag);

	void CalcStructureAreaTop(double angle);
	void AdjustPathEnds(PointF* points, int num, double bgnext, double endext);
	void DeleteCursor();
	void DrawCursor(BOOL draw = TRUE);
	void CheckMeasureDirection(double* x, double* y);
	void ChangeStructure(CGDSStructure* str, BOOL init, BOOL fromDlg = FALSE);
	CGDSStructure* PopHistory();
	void AddHistory();
	void ClearHistory();
	CStrFrame* HitTestStructure(CDC* pDC, CRect rect, BOOL select, BOOL area);
	CStrFrame* HitTestStructure(CDC* pDC, CPoint point, BOOL select);
	void DrawSkelton(CDC* pDC, BOOL redraw_structure = TRUE);
	void ClearHitTestArray();
	void WriteToBmp(CBitmap* bmp);
	void MakeBufferBmp();
	void DrawRubberMemo(CPoint point);
	void CreateModelessDialogs();
	void ExitModeDialogs(BOOL desctractor = FALSE);
	void ChangeEditMode(unsigned newmode);
	void SetCommandGuide(int id);
	void MoveViewOffset(double x, double y);
	void DecrementScale();
	void RecalcNumActiveLayer();

	void SetLayerStatus(int layer, int value);
	void OnOmitSmallStructure();


	CDC* SetDC(CDC* pDC)
	{
		m_pDC = pDC;
		m_pDC->SelectClipRgn(&m_rgnUpdate);
		return m_pScreenDC;
	}
	void ZoomAreaCoord(CCoord center, CCoord size);
	void ClearViewHistory();
	void GetViewArea(double* left, double* right, double* top, double* bottom);
	void GetViewArea(CViewSetting* pSet = NULL);
	void RestoreView(CViewSetting* pRestore);
	void RestoreView(int idx);
	void DeleteView(int idx);
	void DeleteAllView();
	COLORREF GetBkColor()
	{
		return m_colorBK;
	}

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

protected:
	//{{AFX_MSG(CGdsView)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetLayer();
	afx_msg void OnSelectStr();
	afx_msg void OnSetHatchLimit();
	afx_msg void OnZoomUp();
	afx_msg void OnZoomDown();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSetShrinkRate();
	afx_msg void OnDecreaseFontSize();
	afx_msg void OnIncreaseFontSize();
	afx_msg void OnSetFontSize();
	afx_msg void OnViewText();
	afx_msg void OnUpdateViewText(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSetFontSize(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDecreaseFontSize(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIncreaseFontSize(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSetHatchLimit(CCmdUI* pCmdUI);
	afx_msg void OnFitWindow();
	afx_msg void OnUpdateSetLayer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectStr(CCmdUI* pCmdUI);
	afx_msg void OnSetAngle();
	afx_msg void OnMirrorX();
	afx_msg void OnUpdateMirrorX(CCmdUI* pCmdUI);
	afx_msg void OnMirrorY();
	afx_msg void OnUpdateMirrorY(CCmdUI* pCmdUI);
	afx_msg void OnFill();
	afx_msg void OnUpdateFill(CCmdUI* pCmdUI);
	afx_msg void OnShowCenterCross();
	afx_msg void OnUpdateShowCenterCross(CCmdUI* pCmdUI);
	afx_msg void OnShowGridDlg();
	afx_msg void OnUpdateShowGridDlg(CCmdUI* pCmdUI);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnUpdateStructureCb(CCmdUI* pCmdUI);
	afx_msg void OnWriteToEmf();
	afx_msg void OnWritePartToEmf();
	afx_msg void OnUpdateStrLevelCb(CCmdUI* pCmdUI);
	afx_msg void OnStructureLevel();
	afx_msg void OnMouseCrossWindow();
	afx_msg void OnUpdateMouseCrossWindow(CCmdUI* pCmdUI);
	afx_msg void OnMouseCross();
	afx_msg void OnUpdateMouseCross(CCmdUI* pCmdUI);
	afx_msg void OnMouseStandard();
	afx_msg void OnUpdateMouseStandard(CCmdUI* pCmdUI);
	afx_msg void OnHelp();
	afx_msg void OnHelpFinder();
	afx_msg void OnContextHelp();
	afx_msg void OnSetCursorTo();
	afx_msg void OnUpdateSetCursorTo(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnZoomSwitch();
	afx_msg void OnUpdateZoomSwitch(CCmdUI* pCmdUI);
	afx_msg void OnCycleGrid();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMeasureSwitch();
	afx_msg void OnUpdateMeasureSwitch(CCmdUI* pCmdUI);
	afx_msg void OnDeleteScale();
	afx_msg void OnUpdateDeleteScale(CCmdUI* pCmdUI);
	afx_msg void OnCycleDirection();
	afx_msg void OnMemoSwitch();
	afx_msg void OnUpdateMemoSwitch(CCmdUI* pCmdUI);
	afx_msg void OnDeleteMemo();
	afx_msg void OnUpdateDeleteMemo(CCmdUI* pCmdUI);
	afx_msg void OnBackgroundBlack();
	afx_msg void OnUpdateBackgroundBlack(CCmdUI* pCmdUI);
	afx_msg void OnBackgroundWhite();
	afx_msg void OnUpdateBackgroundWhite(CCmdUI* pCmdUI);
	afx_msg void OnMousePlusWindow();
	afx_msg void OnUpdateMousePlusWindow(CCmdUI* pCmdUI);
	afx_msg void OnShowAref();
	afx_msg void OnUpdateShowAref(CCmdUI* pCmdUI);
	afx_msg void OnAxisNone();
	afx_msg void OnUpdateAxisNone(CCmdUI* pCmdUI);
	afx_msg void OnAxisStructureOrigin();
	afx_msg void OnUpdateAxisStructureOrigin(CCmdUI* pCmdUI);
	afx_msg void OnAxisWindowCenter();
	afx_msg void OnUpdateAxisWindowCenter(CCmdUI* pCmdUI);
	afx_msg void OnSkelton();
	afx_msg void OnUpdateSkelton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStructureLevel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSetAngle(CCmdUI* pCmdUI);
	afx_msg void OnButtonBack();
	afx_msg void OnUpdateButtonBack(CCmdUI* pCmdUI);
	afx_msg void OnStructureInfo();
	afx_msg void OnForceSkeltonOn();
	afx_msg void OnForceSkeltonOff();
	afx_msg void OnSelectAllCell();
	afx_msg void OnDestroy();
	afx_msg void OnZoomSwitchByKey();
	afx_msg void OnMemoSwitchByKey();
	afx_msg void OnMeasureSwitchByKey();
	afx_msg void OnDeleteAllRuler();
	afx_msg void OnButtonStopDraw();
	afx_msg void OnPaint();
	afx_msg void OnDrawByLayer();
	afx_msg void OnUpdateDrawByLayer(CCmdUI* pCmdUI);
	afx_msg void OnOverwriteMode();
	afx_msg void OnUpdateOverwriteMode(CCmdUI* pCmdUI);
	afx_msg void OnOutlinePath();
	afx_msg void OnUpdateOutlinePath(CCmdUI* pCmdUI);
	afx_msg void OnExpandCell();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPanToByHomeKey();
	afx_msg void OnHatchExact();
	afx_msg void OnUpdateHatchExact(CCmdUI* pCmdUI);
	afx_msg void OnRulerColor();
	afx_msg void OnColorMemo();
	afx_msg void OnShowTimeStamp();
	afx_msg void OnLayerSetupAtBoot();
	afx_msg void OnAutoRedraw();
	afx_msg void OnUpdateRedraw(CCmdUI* pCmdUI);
	afx_msg void OnForceRedraw();
	afx_msg void OnDebugSwArefArea();
	afx_msg void OnUpdateDebugSwArefArea(CCmdUI* pCmdUI);
	afx_msg void OnZoomAreaCoord();
	afx_msg void OnShowSpotElement();
	afx_msg void OnUpdateShowSpotElement(CCmdUI* pCmdUI);
	afx_msg void GoInToCell();
	afx_msg void OnShowBoundary();
	afx_msg void OnUpdateShowBoundary(CCmdUI* pCmdUI);
	afx_msg void OnShowPath();
	afx_msg void OnUpdateShowPath(CCmdUI* pCmdUI);
	afx_msg void OnViewStore();
	afx_msg void OnViewBack();
	afx_msg void OnViewList();
	afx_msg void OnUpdateViewList(CCmdUI* pCmdUI);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSyncView();
	afx_msg void OnSyncViewAlways();
	afx_msg void OnUpdateSyncViewAlways(CCmdUI* pCmdUI);
	afx_msg void OnClipPolygon();
	afx_msg void OnUpdateClipPolygon(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL			m_bShowPath;
	BOOL			m_bShowBoundary;
	BOOL			m_bShowAREFArea;
	BOOL			m_bClipPolygon;
	BOOL			m_bViewInitialized;
	BOOL			m_bSpotSmall;
	BOOL			m_bEverythingInView;
	BOOL			m_bAutoRedraw;
	BOOL			m_bForceRedraw;
	BOOL			m_bHatchExactly;
	BOOL			m_bRedrawPart;
	BOOL			m_bShowAREF;
	BOOL			m_bAbortDraw;
	BOOL			m_bInitView;
	BOOL			m_bCancelDraw;
    BOOL			m_bDragging;
	BOOL			m_bButtonDown;
    BOOL			m_bCursorState;
	BOOL			m_bShowText;
	BOOL			m_bFillEnable;
	BOOL			m_bShowCenterCross;
	BOOL			m_bDlgView;
	BOOL			m_bShowGridDlg;
	BOOL			m_bStructureDialog;
	BOOL			m_bSetCursorDialog;
	BOOL			m_bMemoDialog;
	BOOL			m_bUpdateDrawTime;
	BOOL			m_bStructureInfoDlg;
	BOOL			m_bDrawByLayer;
	BOOL			m_bOverWrite;
	BOOL			m_bOutlinePath;
	BOOL			m_bSkelton;
	BOOL			m_bSkeltonSorted;
	BOOL			m_bSelectAllCell;
	BOOL			m_bOmitSmallStructure;
	BOOL			m_bLockUpdate;
    BOOL            m_bMouseInView;
	BOOL			m_bAxisState;
	BOOL			m_bOptShrink;
	BOOL			m_bPrintLegend;
	BOOL			m_bPrintScale;
	BOOL			m_bPrintFooter;
	BOOL			m_bPrintPenColor;
	BOOL			m_bDlgLayer;

	int				m_intFontStyle;
	BOOL			m_bLayerInTec[GDS_MAX_LAYER];
	BOOL			m_bLayerExist[GDS_MAX_LAYER];
	BOOL			m_bShowList[GDS_MAX_LAYER];
 	int				m_aryIntFillType[GDS_MAX_LAYER];
	COLORREF		m_aryColorLayerRGB[GDS_MAX_LAYER];
	Color			m_aryColorLayer[GDS_MAX_LAYER];
	CString			m_aryLayerName[GDS_MAX_LAYER];
	int				m_aryIntLayerOrder[GDS_MAX_LAYER];

	int				m_intNumActiveLayer;


	CGDSStructure*	m_pStrActive;

	double			m_dblDrawScale;
	double			m_dblDrawScaleFit;
    double			m_dblShrinkRate;
	double			m_dblTextHeight;
	double			m_dblViewRotation;

	unsigned		m_intZoomAreaMode;
	unsigned		m_intDrawMode;
	unsigned		m_intPrintMode;
	unsigned		m_intAxisAt;
	unsigned		m_intEditMode;
	unsigned		m_intMirror;

	int				m_intHatchLimitD;
	int				m_intHatchLimitL;
	int				m_intDrawLimitD;
	int				m_intDrawLimitL;
	int				m_intDPInvisibleLP;
	int				m_intDPoneLP;
	int				m_intPathLimit;
	int				m_intExpandLevel;
	int				m_intCursorType;
	int				m_intGridMarkerSize;

	CString			m_strFontName;
	WCHAR*			m_pwcharFontName;

	CRect			m_rectView;
	CRect			m_rectDrag;
    CPoint			m_pCursor;

	CCoord			m_coordMin;
	CCoord			m_coordMax;
	CCoord			m_coordViewOffset;

	CRect			m_rectClip;
	RectF			m_rectfClip;
	CRgn			m_rgnUpdate;

	CPoint			m_pToolTip;
	CToolTipCtrl	m_toolTipCtrl;
	CString			m_strToolTip;
	
	CSelViewSetting* m_pDlgView;
	CMemoDlg*		m_pDlgMemo;
	CGridDlg*		m_pDlgGrid;
	CStructureDlg*	m_pDlgStructure;
	CSetCursor*		m_pDlgSetCursor;
	CStructureInfoDlg* m_pDlgStructureInfo;
	CDlgLayer*		m_pDlgLayer;

	CTBComboBox		m_cmbboxStructure;
	CTBComboBox		m_cmbboxStructureLvl;
	CTBComboBox		m_comboDummy1;
	CTBComboBox		m_comboDummy2;

	HCURSOR			m_cOldCursor;
	HCURSOR			m_cStandardCursor;
	HCURSOR			m_cZoomCursor;
	HCURSOR			m_cZoomUpCursor;
	HCURSOR			m_cZoomDownCursor;
	HCURSOR			m_cCross;

	CBitmap         m_bmpAryFill[FILL_BMP_SIZE];
	int				m_intLayerNow;
	int				m_intLayerDraw;
	unsigned		m_uintDrawingAREF;
	CBitmap			m_bmpScreen;
	BOOL			m_bBufferScreen;
	CBitmap			m_bmpCopy;

	CScale			m_scale;
	CMemo			m_memo;
	CGrid*			m_pGrid;

	CDrawParam		m_drawParam;

	CObArray        m_arrayViewSetting;
	CViewSetting    m_viewSetting;

	CObArray        m_arrayHitTest;
	CStrFrame*		m_pFrameActive;
	CStrFrame*		m_pFrameTemp;
	CObArray		m_arrayStructureHist;
	CMTRender*      m_pThreadRender;
	clock_t			m_clocktStart;

	double			m_dblPrintScale;
	double			m_dblPrintScaleArea;
	double			m_dblHatchDensity;
	double			m_dblLegendSize;
	double			m_dblLegendAngle;

	REAL			m_realHatchPenWidthPrint;
	REAL			m_realHatchSpacingPrint;
	REAL			m_realPenWidth;
	REAL			m_realPenWidthLim;
	REAL			m_realPenWidthPrint;
	REAL			m_realScaleBmpX;
	REAL			m_realScaleBmpY;

	Color			m_colorPen;
	Color			m_colorLayer;
	COLORREF		m_colorPenRGB;
	COLORREF		m_colorLayerRGB;

	COLORREF		m_colorMemo;
	COLORREF        m_colorScale;
    COLORREF		m_colorBK;

	CDC*			m_pDC;
	CDC*			m_pScreenDC;
	Graphics*       m_pGraphics;

	Pen*			m_pPenDraw;

	Metafile*		m_pMetafile;
	HENHMETAFILE    m_hMetafile;

	CMainFrame*     m_pMainFrame;

	BYTE			m_byteTransparent;

};

enum {
	ELEMENT_OMITTED,
	ELEMENT_DUMMY,
	ELEMENT_TO_BE_DRAWN,
};

enum {
	DRAW_MODE_DISPLAY,
	DRAW_MODE_PRINT,
	DRAW_MODE_CLIPBOARD,
};

enum {
	CMD_GUIDE_UPDATE,
	CMD_GUIDE_NONE,
	CMD_GUIDE_MEASURE_P1,
	CMD_GUIDE_MEASURE_P2,
	CMD_GUIDE_ZOOM,
	CMD_GUIDE_MEMO,
	CMD_GUIDE_STOP_DRAW,
};

enum {
	PRINT_MODE_UNKNOWN,
	PRINT_MODE_WYSIWYG,
	PRINT_MODE_SCALED_FULL,
	PRINT_MODE_SCALED_AREA,
};

enum dir {
	MEAS_DIR_XY = 0,
	MEAS_DIR_DIAGONAL,
	MEAS_DIR_ANY,
};

enum {
	EDIT_MODE_NONE = 0,
	EDIT_MODE_ZOOM,
	EDIT_MODE_MEASURE,
	EDIT_MODE_MEMO,
};

enum {
	MIRROR_NONE = 0,
	MIRROR_TO_X = 1,
	MIRROR_TO_Y = 2
};

enum {
	CURSOR_CROSS_WINDOW = 0,
	CURSOR_PLUS_WINDOW,
	CURSOR_CROSS,
	CURSOR_STANDARD
};

enum {
	GRID_TYPE_NONE = 0,
	GRID_TYPE_DOT,
	GRID_TYPE_LINE,
};

enum {
	AXIS_AT_NONE = 0,
	AXIS_AT_STRUCTURE_ORIGIN = 1, // must be 1 for backward compatibility
	AXIS_AT_WINDOW_CENTER,
};

#ifndef _DEBUG
inline CGdsDoc* CGdsView::GetDocument()
   { return (CGdsDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_GDSVIEW_H__E6BB1E8A_6F5B_4EA2_9C08_5332EF3CE356__INCLUDED_)
