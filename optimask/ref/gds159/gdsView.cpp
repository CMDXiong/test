#include "stdafx.h"
#include "gds.h"

#include "MainFrm.h"
#include "ChildFrm.h"

#include "gdsDoc.h"
#include "gdsView.h"
#include "GDShcy.h"
#include "SetCursor.h"
#include "StructureDlg.h"
#include "GridDlg.h"
#include "Scale.h"
#include "MemoDlg.h"
#include "StructureInfoDlg.h"
#include "ZoomAreaDlg.h"
#include "SelViewSetting.h"
#include "DlgLayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define PROFILE

/////////////////////////////////////////////////////////////////////////////
// CGdsView

IMPLEMENT_DYNCREATE(CGdsView, CScrollView)

BEGIN_MESSAGE_MAP(CGdsView, CScrollView)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopyCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCopyCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnNotifyNeedText )
	//{{AFX_MSG_MAP(CGdsView)
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_SET_LAYER, OnSetLayer)
	ON_COMMAND(ID_SELECT_STR, OnSelectStr)
	ON_COMMAND(ID_SET_HATCH_LIMIT, OnSetHatchLimit)
	ON_COMMAND(ID_ZOOM_UP, OnZoomUp)
	ON_COMMAND(ID_ZOOM_DOWN, OnZoomDown)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_SET_SHRINK_RATE, OnSetShrinkRate)
	ON_COMMAND(ID_DECREASE_FONT_SIZE, OnDecreaseFontSize)
	ON_COMMAND(ID_INCREASE_FONT_SIZE, OnIncreaseFontSize)
	ON_COMMAND(ID_SET_FONT_SIZE, OnSetFontSize)
	ON_COMMAND(ID_VIEW_TEXT, OnViewText)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TEXT, OnUpdateViewText)
	ON_UPDATE_COMMAND_UI(ID_SET_FONT_SIZE, OnUpdateSetFontSize)
	ON_UPDATE_COMMAND_UI(ID_DECREASE_FONT_SIZE, OnUpdateDecreaseFontSize)
	ON_UPDATE_COMMAND_UI(ID_INCREASE_FONT_SIZE, OnUpdateIncreaseFontSize)
	ON_UPDATE_COMMAND_UI(ID_SET_HATCH_LIMIT, OnUpdateSetHatchLimit)
	ON_COMMAND(ID_FIT_WINDOW, OnFitWindow)
	ON_UPDATE_COMMAND_UI(ID_SET_LAYER, OnUpdateSetLayer)
	ON_UPDATE_COMMAND_UI(ID_SELECT_STR, OnUpdateSelectStr)
	ON_COMMAND(ID_SET_ANGLE, OnSetAngle)
	ON_COMMAND(ID_MIRROR_X, OnMirrorX)
	ON_UPDATE_COMMAND_UI(ID_MIRROR_X, OnUpdateMirrorX)
	ON_COMMAND(ID_MIRROR_Y, OnMirrorY)
	ON_UPDATE_COMMAND_UI(ID_MIRROR_Y, OnUpdateMirrorY)
	ON_COMMAND(ID_FILL, OnFill)
	ON_UPDATE_COMMAND_UI(ID_FILL, OnUpdateFill)
	ON_COMMAND(ID_SHOW_CENTER_CROSS, OnShowCenterCross)
	ON_UPDATE_COMMAND_UI(ID_SHOW_CENTER_CROSS, OnUpdateShowCenterCross)
	ON_COMMAND(ID_SHOW_GRIDDLG, OnShowGridDlg)
	ON_UPDATE_COMMAND_UI(ID_SHOW_GRIDDLG, OnUpdateShowGridDlg)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_UPDATE_COMMAND_UI(ID_STRUCTURE_CB, OnUpdateStructureCb)
	ON_COMMAND(ID_WRITE_TO_EMF, OnWriteToEmf)
	ON_COMMAND(ID_WRITE_PART_TO_EMF, OnWritePartToEmf)
	ON_UPDATE_COMMAND_UI(ID_STR_LEVEL_CB, OnUpdateStrLevelCb)
	ON_COMMAND(ID_STRUCTURE_LEVEL, OnStructureLevel)
	ON_COMMAND(ID_MOUSE_CROSS_WINDOW, OnMouseCrossWindow)
	ON_UPDATE_COMMAND_UI(ID_MOUSE_CROSS_WINDOW, OnUpdateMouseCrossWindow)
	ON_COMMAND(ID_MOUSE_CROSS, OnMouseCross)
	ON_UPDATE_COMMAND_UI(ID_MOUSE_CROSS, OnUpdateMouseCross)
	ON_COMMAND(ID_MOUSE_STANDARD, OnMouseStandard)
	ON_UPDATE_COMMAND_UI(ID_MOUSE_STANDARD, OnUpdateMouseStandard)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_HELP_FINDER, OnHelpFinder)
	ON_COMMAND(ID_CONTEXT_HELP, OnContextHelp)
	ON_COMMAND(ID_SET_CURSOR_TO, OnSetCursorTo)
	ON_UPDATE_COMMAND_UI(ID_SET_CURSOR_TO, OnUpdateSetCursorTo)
	ON_WM_CREATE()
	ON_COMMAND(ID_ZOOM_SWITCH, OnZoomSwitch)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_SWITCH, OnUpdateZoomSwitch)
	ON_COMMAND(ID_CYCLE_GRID, OnCycleGrid)
	ON_WM_TIMER()
	ON_COMMAND(ID_MEASURE_SWITCH, OnMeasureSwitch)
	ON_UPDATE_COMMAND_UI(ID_MEASURE_SWITCH, OnUpdateMeasureSwitch)
	ON_COMMAND(ID_DELETE_SCALE, OnDeleteScale)
	ON_UPDATE_COMMAND_UI(ID_DELETE_SCALE, OnUpdateDeleteScale)
	ON_COMMAND(ID_CYCLE_DIRECTION, OnCycleDirection)
	ON_COMMAND(ID_MEMO_SWITCH, OnMemoSwitch)
	ON_UPDATE_COMMAND_UI(ID_MEMO_SWITCH, OnUpdateMemoSwitch)
	ON_COMMAND(ID_DELETE_MEMO, OnDeleteMemo)
	ON_UPDATE_COMMAND_UI(ID_DELETE_MEMO, OnUpdateDeleteMemo)
	ON_COMMAND(ID_BACKGROUND_BLACK, OnBackgroundBlack)
	ON_UPDATE_COMMAND_UI(ID_BACKGROUND_BLACK, OnUpdateBackgroundBlack)
	ON_COMMAND(ID_BACKGROUND_WHITE, OnBackgroundWhite)
	ON_UPDATE_COMMAND_UI(ID_BACKGROUND_WHITE, OnUpdateBackgroundWhite)
	ON_COMMAND(ID_MOUSE_PLUS_WINDOW, OnMousePlusWindow)
	ON_UPDATE_COMMAND_UI(ID_MOUSE_PLUS_WINDOW, OnUpdateMousePlusWindow)
	ON_COMMAND(ID_SHOW_AREF, OnShowAref)
	ON_UPDATE_COMMAND_UI(ID_SHOW_AREF, OnUpdateShowAref)
	ON_COMMAND(ID_AXIS_NONE, OnAxisNone)
	ON_UPDATE_COMMAND_UI(ID_AXIS_NONE, OnUpdateAxisNone)
	ON_COMMAND(ID_AXIS_STRUCTURE_ORIGIN, OnAxisStructureOrigin)
	ON_UPDATE_COMMAND_UI(ID_AXIS_STRUCTURE_ORIGIN, OnUpdateAxisStructureOrigin)
	ON_COMMAND(ID_AXIS_WINDOW_CENTER, OnAxisWindowCenter)
	ON_UPDATE_COMMAND_UI(ID_AXIS_WINDOW_CENTER, OnUpdateAxisWindowCenter)
	ON_COMMAND(ID_SKELTON, OnSkelton)
	ON_UPDATE_COMMAND_UI(ID_SKELTON, OnUpdateSkelton)
	ON_UPDATE_COMMAND_UI(ID_STRUCTURE_LEVEL, OnUpdateStructureLevel)
	ON_UPDATE_COMMAND_UI(ID_SET_ANGLE, OnUpdateSetAngle)
	ON_COMMAND(ID_BUTTON_BACK, OnButtonBack)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_BACK, OnUpdateButtonBack)
	ON_COMMAND(ID_STRUCTURE_INFO, OnStructureInfo)
	ON_COMMAND(ID_FORCE_SKELTON_ON, OnForceSkeltonOn)
	ON_COMMAND(ID_FORCE_SKELTON_OFF, OnForceSkeltonOff)
	ON_COMMAND(ID_SELECT_ALL_CELL, OnSelectAllCell)
	ON_WM_DESTROY()
	ON_COMMAND(ID_ZOOM_SWITCH_BY_KEY, OnZoomSwitchByKey)
	ON_COMMAND(ID_MEMO_SWITCH_BY_KEY, OnMemoSwitchByKey)
	ON_COMMAND(ID_MEASURE_SWITCH_BY_KEY, OnMeasureSwitchByKey)
	ON_COMMAND(ID_DELETE_ALL_RULER, OnDeleteAllRuler)
	ON_COMMAND(ID_BUTTON_STOP_DRAW, OnButtonStopDraw)
	ON_WM_PAINT()
	ON_COMMAND(ID_DRAW_BY_LAYER, OnDrawByLayer)
	ON_UPDATE_COMMAND_UI(ID_DRAW_BY_LAYER, OnUpdateDrawByLayer)
	ON_COMMAND(ID_OVERWRITE_MODE, OnOverwriteMode)
	ON_UPDATE_COMMAND_UI(ID_OVERWRITE_MODE, OnUpdateOverwriteMode)
	ON_COMMAND(ID_OUTLINE_PATH, OnOutlinePath)
	ON_UPDATE_COMMAND_UI(ID_OUTLINE_PATH, OnUpdateOutlinePath)
	ON_COMMAND(ID_EXPAND_CELL, OnExpandCell)
	ON_WM_SIZE()
	ON_COMMAND(ID_PAN_TO, OnPanToByHomeKey)
	ON_COMMAND(ID_HATCH_EXACT, OnHatchExact)
	ON_UPDATE_COMMAND_UI(ID_HATCH_EXACT, OnUpdateHatchExact)
	ON_COMMAND(ID_RULER_COLOR, OnRulerColor)
	ON_COMMAND(ID_COLOR_MEMO, OnColorMemo)
	ON_COMMAND(ID_SHOW_TIME_STAMP, OnShowTimeStamp)
	ON_COMMAND(ID_LAYER_SETUP_AT_BOOT, OnLayerSetupAtBoot)
	ON_COMMAND(ID_REDRAW, OnAutoRedraw)
	ON_UPDATE_COMMAND_UI(ID_REDRAW, OnUpdateRedraw)
	ON_COMMAND(ID_FORCE_REDRAW, OnForceRedraw)
	ON_COMMAND(ID_DEBUG_SW_AREF_AREA, OnDebugSwArefArea)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_SW_AREF_AREA, OnUpdateDebugSwArefArea)
	ON_COMMAND(ID_ZOOM_AREA, OnZoomAreaCoord)
	ON_COMMAND(ID_SHOW_SPOT_ELEMENT, OnShowSpotElement)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SPOT_ELEMENT, OnUpdateShowSpotElement)
	ON_COMMAND(ID_CHANGE_STRUCTURE, GoInToCell)
	ON_COMMAND(ID_SHOW_BOUNDARY, OnShowBoundary)
	ON_UPDATE_COMMAND_UI(ID_SHOW_BOUNDARY, OnUpdateShowBoundary)
	ON_COMMAND(ID_SHOW_PATH, OnShowPath)
	ON_UPDATE_COMMAND_UI(ID_SHOW_PATH, OnUpdateShowPath)
	ON_COMMAND(ID_VIEW_STORE, OnViewStore)
	ON_COMMAND(ID_VIEW_BACK, OnViewBack)
	ON_COMMAND(ID_VIEW_LIST, OnViewList)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LIST, OnUpdateViewList)
	ON_WM_DROPFILES()
	ON_COMMAND(ID_SYNC_VIEW, OnSyncView)
	ON_COMMAND(ID_SYNC_VIEW_ALWAYS, OnSyncViewAlways)
	ON_UPDATE_COMMAND_UI(ID_SYNC_VIEW_ALWAYS, OnUpdateSyncViewAlways)
	ON_COMMAND(ID_AUTO_REDRAW, OnAutoRedraw)
	ON_WM_KEYUP()
	ON_COMMAND(ID_DELETE_ALL_MEMO, OnDeleteMemo)
	ON_COMMAND(ID_STOP_DRAW, OnButtonStopDraw)
	ON_WM_RENDERALLFORMATS()
	ON_WM_RENDERFORMAT()
	ON_COMMAND(ID_SCROLL_LEFT, OnScrollLeft)
	ON_COMMAND(ID_SCROLL_DOWN, OnScrollDown)
	ON_COMMAND(ID_SCROLL_RIGHT, OnScrollRight)
	ON_COMMAND(ID_SCROLL_UP, OnScrollUp)
	ON_COMMAND(ID_CLIP_POLYGON, OnClipPolygon)
	ON_UPDATE_COMMAND_UI(ID_CLIP_POLYGON, OnUpdateClipPolygon)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
#include "LayerInfo.h"
CGdsView::CGdsView()
{
	m_byteTransparent = TRANSPARENT_ALPHA;

	m_bClipPolygon = FALSE;
	m_bBufferScreen = FALSE;
	m_bDlgLayer = FALSE;
	m_bViewInitialized = FALSE;
	m_bShowPath = TRUE;
	m_bShowBoundary = TRUE;
	m_pwcharFontName = NULL;
	m_bSkeltonSorted = FALSE;
	m_bShowAREFArea = FALSE;
	m_bSpotSmall = FALSE;
	m_bForceRedraw = FALSE;
	m_bAutoRedraw = TRUE;
	m_dblLegendAngle = 0.0;
	m_bPrintLegend = TRUE;
	m_bPrintScale = TRUE;
	m_bPrintFooter = TRUE;
	m_bOptShrink = TRUE;
	m_realPenWidth = 0.0;
	m_bAxisState = FALSE;
	m_bMouseInView = FALSE;
	m_bLockUpdate = FALSE;
	m_dblPrintScale = -1.0;
	m_dblPrintScaleArea = -1.0;
	m_intPrintMode = PRINT_MODE_UNKNOWN;
	m_pMetafile = NULL;
	m_hMetafile = NULL;
	m_bOverWrite = TRUE;
	m_pMainFrame = NULL;
 	m_pGrid = NULL;
	m_pDC = NULL;
	m_dblDrawScale = 1.0;
    m_dblShrinkRate = 0.0;
	m_dblViewRotation = 0.0;
	m_bInitView = TRUE;
    m_bDragging = FALSE;
	m_bButtonDown = FALSE;
    m_bCursorState = FALSE;
	m_bHatchExactly = FALSE;
	m_bShowCenterCross = FALSE;
	m_bDlgView = FALSE;
	m_bShowGridDlg = FALSE;
	m_bMemoDialog = FALSE;
	m_intDrawMode = DRAW_MODE_DISPLAY;
	m_bStructureDialog = FALSE;
	m_bSetCursorDialog = FALSE;
	m_bUpdateDrawTime = FALSE;
	m_bStructureInfoDlg = FALSE;
	m_bSelectAllCell = FALSE;
	m_intEditMode = EDIT_MODE_NONE;
	m_intMirror = MIRROR_NONE;
	m_coordViewOffset.SetCoord(0.0, 0.0);

	m_pDlgView = NULL;
	m_pDlgGrid = NULL;
	m_pDlgStructure = NULL;
	m_pDlgSetCursor = NULL;
	m_pDlgMemo = NULL;
	m_pDlgStructureInfo = NULL;
	m_pDlgLayer = NULL;

	(m_bmpAryFill + FILL_BMP_XDIAG)->LoadBitmap(IDB_XDIAG);
	(m_bmpAryFill + FILL_BMP_FDIAG)->LoadBitmap(IDB_FDIAG);
	(m_bmpAryFill + FILL_BMP_RDIAG)->LoadBitmap(IDB_RDIAG);
	(m_bmpAryFill + FILL_BMP_FDIAG_DASH)->LoadBitmap(IDB_FDIAG_DASH);
	(m_bmpAryFill + FILL_BMP_RDIAG_DASH)->LoadBitmap(IDB_RDIAG_DASH);
	(m_bmpAryFill + FILL_BMP_XHATCH)->LoadBitmap(IDB_XHATCH);
	(m_bmpAryFill + FILL_BMP_VHATCH)->LoadBitmap(IDB_VHATCH);
	(m_bmpAryFill + FILL_BMP_HHATCH)->LoadBitmap(IDB_HHATCH);
	(m_bmpAryFill + FILL_BMP_HZIGZAG)->LoadBitmap(IDB_HZIGZAG);
	(m_bmpAryFill + FILL_BMP_VZIGZAG)->LoadBitmap(IDB_VZIGZAG);
	(m_bmpAryFill + FILL_BMP_SOLID)->LoadBitmap(IDB_SOLID);
	(m_bmpAryFill + FILL_BMP_HDASH)->LoadBitmap(IDB_HDASH);
	(m_bmpAryFill + FILL_BMP_VDASH)->LoadBitmap(IDB_VDASH);
	(m_bmpAryFill + FILL_BMP_VWAVE)->LoadBitmap(IDB_VWAVE);
	(m_bmpAryFill + FILL_BMP_HWAVE)->LoadBitmap(IDB_HWAVE);
	(m_bmpAryFill + FILL_BMP_LIGHT)->LoadBitmap(IDB_LIGHT);
	(m_bmpAryFill + FILL_BMP_XHATCH_HD)->LoadBitmap(IDB_DIA);

	m_pStrActive = NULL;
	for(unsigned i = 0;i < GDS_MAX_LAYER;i++){
		*(m_aryIntFillType + i) = FILL_BMP_NONE;
		*(m_aryColorLayerRGB + i) = COLOR_INVALID;
	}

	CWinApp* app = AfxGetApp();

	m_colorMemo = app->GetProfileInt("Color", "Memo", COLOR_MEMO_DEFAULT);
	m_colorScale = app->GetProfileInt("Color", "Ruler", COLOR_SCALE_FIX);
	m_colorBK = app->GetProfileInt("Color", "Background", COLOR_BLACK);
	m_intCursorType = app->GetProfileInt("Misc", "Cursor style", CURSOR_STANDARD);
	m_intAxisAt = app->GetProfileInt("Misc", "Axis on center", AXIS_AT_STRUCTURE_ORIGIN);
	m_intHatchLimitD = app->GetProfileInt("Misc", "Minimum Fill/Hatch", FILL_LIMIT);
    m_intDrawLimitD = app->GetProfileInt("Misc", "Minimum Draw", DRAW_LIMIT);
	m_bOmitSmallStructure = app->GetProfileInt("Misc", "Omit small", 1) > 0 ? TRUE : FALSE;
	m_intZoomAreaMode = app->GetProfileInt("Misc", "Zoom area mode", ZOOM_AREA_MODE_SIDE);

	// for backward compatibility
	m_dblTextHeight = app->GetProfileInt("Misc", "Text Height UM", TEXT_HEIGHT);
	m_dblTextHeight *= 1e-6;

	int val = app->GetProfileInt("Misc", "Text Height NM", -1);
	if(val > 0)
		m_dblTextHeight = val * 1e-9 / 1000;

	m_strFontName = app->GetProfileString("Misc", "Font Name", DEFAULT_GDS2_FONT);
	val = lstrlenA(m_strFontName) + 1;
	m_pwcharFontName = new WCHAR[val * 2];
	ATLA2WHELPER(m_pwcharFontName, m_strFontName, val);
	
	m_intFontStyle = app->GetProfileInt("Misc", "Font Style", FontStyleRegular);

	m_intExpandLevel = app->GetProfileInt("Misc", "Substructure expand", RECURSE_LEVEL);
	m_bShowAREF = app->GetProfileInt("Misc", "Show AREF", 1) > 0 ? TRUE : FALSE;
	m_realPenWidthPrint = (REAL)(int)app->GetProfileInt("Misc", "Print Pen Width", -1);
	if(m_realPenWidthPrint < 0.0)
		m_realPenWidthPrint = (REAL)DEFAULT_PRINT_PEN_WIDTH;
	else
		m_realPenWidthPrint *= 1.0e-6f;
	m_realPenWidthPrint /= (REAL)HIMETRIC;

	m_bPrintPenColor = app->GetProfileInt("Misc", "Print Pen Color", FALSE);
	m_dblLegendSize = app->GetProfileInt("Misc", "Legend size", 0);
	if(m_dblLegendSize == 0.0)
		 m_dblLegendSize = LEGEND_HEIGHT;
	else
		m_dblLegendSize /= 1000.0;

	m_bFillEnable = app->GetProfileInt("Misc", "Fill Polygon", FALSE);
	m_bShowText = app->GetProfileInt("Misc", "Show Text", FALSE);

	m_cStandardCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_cZoomCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_ZOOM);
	m_cZoomUpCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_ZOOM_UP);
	m_cZoomDownCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_ZOOM_DOWN);
	m_cCross = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
	m_cOldCursor = NULL;

	m_bDrawByLayer = app->GetProfileInt("Misc", "Draw by Layer", FALSE);
	m_bOutlinePath = app->GetProfileInt("Misc", "Draw Path outline", TRUE);
	m_dblHatchDensity = app->GetProfileInt("Misc", "Hatch density", 0);

	m_realHatchPenWidthPrint = (REAL)(int)app->GetProfileInt("Misc", "Hatch Pen Width", -1);
	if(m_realHatchPenWidthPrint < 0.0)
		m_realHatchPenWidthPrint = (REAL)DEFAULT_PRINT_PEN_WIDTH;
	else
		m_realHatchPenWidthPrint *= 1.0e-6f;
	m_realHatchPenWidthPrint /= (REAL)HIMETRIC;

	m_realHatchSpacingPrint = (REAL)(int)app->GetProfileInt("Misc", "Hatch Spacing", -1);
	if(m_realHatchSpacingPrint < 0.0)
		m_realHatchSpacingPrint = (REAL)DEFAULT_PRINT_HATCH_SPACE;
	else
		m_realHatchSpacingPrint *= 1.0e-6f;
	m_realHatchSpacingPrint /= (REAL)HIMETRIC;


	m_intGridMarkerSize = app->GetProfileInt("Grid", "Marker size", GRID_MARKER_SIZE);
	app->WriteProfileInt("Grid", "Marker size", m_intGridMarkerSize);

	for(i = 0;i < sizeof(m_bShowList)/sizeof(int);i++){
		*(m_bLayerInTec + i) = FALSE;
		*(m_bShowList + i) = FALSE;
		*(m_aryIntLayerOrder + i) = -1;
		*(m_bLayerExist + i) = FALSE;
	}

	m_intNumActiveLayer = 0;
	m_bSkelton = FALSE;
	m_pFrameActive = NULL;
	m_pFrameTemp = NULL;
	m_strToolTip = "";
	m_pThreadRender = NULL;
	m_bCancelDraw = FALSE;
	m_bAbortDraw = FALSE;
	m_pScreenDC = NULL;

	m_pPenDraw = ::new Pen(m_colorPen, m_realPenWidth);
}

void CGdsView::OnDestroy() 
{
	m_pMainFrame->RemoveView(this);
	CScrollView::OnDestroy();
}

CGdsView::~CGdsView()
{
	if(m_pwcharFontName)
		delete [] m_pwcharFontName;

	LockWindowUpdate();
	StopDraw();

	::delete m_pPenDraw;

	ClearViewHistory();
	ClearHistory();
	ClearHitTestArray();
	if(m_bDlgLayer)
		m_pDlgLayer->ExitDialog();

	if(m_bDlgView)
		m_pDlgView->ExitDialog();
	if(m_bShowGridDlg)
		m_pDlgGrid->ExitMeasure();
	if(m_bStructureDialog)
		m_pDlgStructure->ExitDialog();
	if(m_bSetCursorDialog)
		m_pDlgSetCursor->ExitDialog();
	if(m_bStructureInfoDlg)
		m_pDlgStructureInfo->ExitDialog();
	if(m_bMemoDialog)
		m_pDlgMemo->ExitDialog();
	if(m_pGrid)
		delete m_pGrid;
	if(m_pScreenDC)
		delete m_pScreenDC;
	if(m_hMetafile){
		::DeleteEnhMetaFile(m_hMetafile);
		m_bmpCopy.DeleteObject();
	}
	m_bmpScreen.DeleteObject();
	KillRenderThread();

}

BOOL CGdsView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////

void CGdsView::RotatePoint(CCoord* point, double& angle)
{
	if(angle == 0.0 || (point->x == 0.0 && point->y == 0.0))
		return;

	static double t;
	if(angle == 90.0 || angle == -270.0){
		t = point->x;
		point->x = -point->y;
		point->y = t;
	}
	else if(angle == 180.0 || angle == -180.0){
		point->x = -point->x;
		point->y = -point->y;
	}
	else if(angle == 270.0 || angle == -90.0){
		t = point->x;
		point->x = point->y;
		point->y = -t;
	}
	else{
		t = sqrt(point->x * point->x + point->y * point->y);
		double newang = atan2(point->y, point->x) + angle/180.0*PAI;
		point->SetCoord(t*cos(newang), t*sin(newang));
	}
	return;
}

void CGdsView::RotatePoints(PointF* point, double& angle, unsigned num)
{
	if(angle == 0.0)
		return;
	static int dir;
	if(angle == 90.0 || angle == -270.0)
		dir = 0;
	else if(angle == 180.0 || angle == -180.0)
		dir = 1;
	else if(angle == 270.0 || angle == -90.0)
		dir = 2;
	else
		dir = 3;

	static REAL t;
	for(unsigned i = 0;i < num;i++){
		if((point + i)->X == 0.0 && (point + i)->Y == 0.0)
			continue;
		switch(dir){
		case 0:
			t = (point + i)->X;
			(point + i)->X = -(point + i)->Y;
			(point + i)->Y = t;
			break;
		case 1:
			(point + i)->X = -(point + i)->X;
			(point + i)->Y = -(point + i)->Y;
			break;
		case 2:
			t = (point + i)->X;
			(point + i)->X = (point + i)->Y;
			(point + i)->Y = -t;
			break;
		case 3:
		default:
			{
			t = (REAL)sqrt((point + i)->X * (point + i)->X + (point + i)->Y * (point + i)->Y);
			REAL newang = (REAL)(atan2((point + i)->Y, (point + i)->X) + angle/180.0*PAI);
			(point + i)->X = (REAL)(t*cos(newang));
			(point + i)->Y = (REAL)(t*sin(newang)); 
			}
			break; 
		}
	}
	return;
}


void CGdsView::RotatePoints(CCoord* point, double& angle, unsigned num)
{
	if(angle == 0.0)
		return;
	static int dir;
	if(angle == 90.0 || angle == -270.0)
		dir = 0;
	else if(angle == 180.0 || angle == -180.0)
		dir = 1;
	else if(angle == 270.0 || angle == -90.0)
		dir = 2;
	else
		dir = 3;

	static double t;
	for(unsigned i = 0;i < num;i++){
		if((point + i)->x == 0.0 && (point + i)->y == 0.0)
			continue;
		switch(dir){
		case 0:
			t = (point + i)->x;
			(point + i)->x = -(point + i)->y;
			(point + i)->y = t;
			break;
		case 1:
			(point + i)->x = -(point + i)->x;
			(point + i)->y = -(point + i)->y;
			break;
		case 2:
			t = (point + i)->x;
			(point + i)->x = (point + i)->y;
			(point + i)->y = -t;
			break;
		case 3:
		default:
			{
			t = sqrt((point + i)->x * (point + i)->x + (point + i)->y * (point + i)->y);
			double newang = atan2((point + i)->y, (point + i)->x) + angle/180.0*PAI;
			(point + i)->x = t*cos(newang);
			(point + i)->y = t*sin(newang); 
			}
			break; 
		}
	}
	return;
}

void CGdsView::RotatePoints(CPoint* point, double& angle, unsigned num)
{
	if(angle == 0.0)
		return; 
	static int dir;
	if(angle == 90.0 || angle == -270.0)
		dir = 0;
	else if(angle == 180.0 || angle == -180.0)
		dir = 1;
	else if(angle == 270.0 || angle == -90.0)
		dir = 2;
	else
		dir = 3;

	static long t;
	for(unsigned i = 0;i < num;i++){
		if((point + i)->x == 0.0 && (point + i)->y == 0.0)
			continue;
		switch(dir){
		case 0:
			t = (point + i)->x;
			(point + i)->x = -(point + i)->y;
			(point + i)->y = t;
			break;
		case 1:
			(point + i)->x = -(point + i)->x;
			(point + i)->y = -(point + i)->y;
			break;
		case 2:
			t = (point + i)->x;
			(point + i)->x = (point + i)->y;
			(point + i)->y = -t;
			break;
		case 3:
		default:
			{
			double t = sqrt((point + i)->x * (point + i)->x + (point + i)->y * (point + i)->y);
			double newang = atan2((point + i)->y, (point + i)->x) + angle/180.0*PAI;
			(point + i)->x = (long)(t*cos(newang));
			(point + i)->y = (long)(t*sin(newang));
			}
			break;
		}
	}
	return;
}


void CGdsView::RotateRect(CRect* rect, double& angle)
{
	if(angle == 0.0)
		return;
	static int dir;
	if(angle == 90.0 || angle == -270.0)
		dir = 0;
	else if(angle == 180.0 || angle == -180.0)
		dir = 1;
	else if(angle == 270.0 || angle == -90.0)
		dir = 2;
	else
		dir = 3;

	static CPoint point[4];
	point[0].x = point[1].x = rect->left;
	point[0].y = point[3].y = rect->top;
	point[2].x = point[3].x = rect->right;
	point[1].y = point[2].y = rect->bottom;
	static long t;
	for(unsigned i = 0;i < 4;i++){
		switch(dir){
		case 0:
			t = (point + i)->x;
			(point + i)->x = -(point + i)->y;
			(point + i)->y = t;
			break;
		case 1:
			(point + i)->x = -(point + i)->x;
			(point + i)->y = -(point + i)->y;
			break;
		case 2:
			t = (point + i)->x;
			(point + i)->x = (point + i)->y;
			(point + i)->y = -t;
			break;
		case 3:
		default:
			{
			double t = sqrt((point + i)->x * (point + i)->x + (point + i)->y * (point + i)->y);
			double newang = atan2((point + i)->y, (point + i)->x) + angle/180.0*PAI;
			(point + i)->x = (long)(t*cos(newang));
			(point + i)->y = (long)(t*sin(newang));
			}
			break;
		}
	}
	for(i = 0;i < 4;i++){
		if(i == 0){
			rect->left = rect->right = (point + i)->x;
			rect->bottom = rect->top = (point + i)->y;
		}
		else{
			if((point + i)->x < rect->left)
				rect->left = (point + i)->x;
			else if((point + i)->x > rect->right)
				rect->right = (point + i)->x;
			if((point + i)->y < rect->bottom)
				rect->bottom = (point + i)->y;
			else if((point + i)->y > rect->top)
				rect->top = (point + i)->y;
		}
	}
	return;
}




void CGdsView::AdjustPathEnds(PointF* points, int num, double bgnext, double endext)
{
	static double x, y;
	x = (points + 0)->X - (points + 1)->X;
	y = (points + 0)->Y - (points + 1)->Y;
	if(x != 0.0 && y != 0.0){
		// Begin point
		double len = sqrt(x * x + y * y);
		double angle = atan2(y, x);
		(points + 0)->X = (points + 1)->X + (REAL)(len + bgnext*cos(angle));
		(points + 0)->Y = (points + 1)->Y + (REAL)(len + bgnext*sin(angle));

		// End point
		x = (points + num - 1)->X - (points + num - 2)->X;
		y = (points + num - 1)->Y - (points + num - 2)->Y;
		len = sqrt(x * x + y * y);
		angle = atan2(y, x);
		(points + num - 1)->X = (points + num - 2)->X + (REAL)(len + endext*cos(angle));
		(points + num - 1)->Y = (points + num - 2)->Y + (REAL)(len + endext*sin(angle));
	}
	else if(x == 0.0){
		// Begin point
		double len = y;
		(points + 0)->X = (points + 1)->X;
		(points + 0)->Y = (points + 1)->Y + (REAL)(len + bgnext);
		// End point
		len = (points + num - 1)->Y - (points + num - 2)->Y;
		(points + num - 1)->X = (points + num - 2)->X;
		(points + num - 1)->Y = (points + num - 2)->Y + (REAL)(len + endext);
	}
	else if(y == 0.0){
		// Begin point
		double len = x;
		(points + 0)->X = (points + 1)->X + (REAL)(len + bgnext);
		(points + 0)->Y = (points + 1)->Y;
		// End point
		len = (points + num - 1)->X - (points + num - 2)->X;
		(points + num - 1)->X = (points + num - 2)->X + (REAL)(len + endext);
		(points + num - 1)->Y = (points + num - 2)->Y;
	}
	return;
}


void CGdsView::SetOrigin(CDC* pDC)
{
	SIZE sz = GetTotalSize();
	CPoint point(-sz.cx / 2, sz.cy / 2); // Set logical origin to center
	pDC->SetWindowOrg(point);
}

void CGdsView::Render(CDC* pDC)
{
	if(!m_bViewInitialized || !::IsWindow(m_hWnd))
		return;
	DeleteCursor();
	LockWindowUpdate();
    CClientDC dc(this);
	OnPrepareDC(&dc);
	SetOrigin(&dc);

    if(!pDC)
        pDC = &dc;

    SetOrigin(pDC);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(m_colorBK);
    int rop = pDC->GetROP2();
	pDC->SetROP2(GetROP(pDC));
	pDC->SelectStockObject(NULL_BRUSH);

	
	Metafile m(pDC->m_hDC); // dummy metafile

	Graphics gmf(m_pMetafile ? m_pMetafile : &m); 

	Graphics gdc(pDC->m_hDC);


	if(m_intDrawMode == DRAW_MODE_CLIPBOARD){
		m_pGraphics = &gmf;
		m_pGraphics->SetPageUnit(UnitPixel);
		m_pGraphics->ScaleTransform(1.0f/m_realScaleBmpX, -1.0f/m_realScaleBmpY);
	}
	else{
		if(m_intDrawMode != DRAW_MODE_PRINT)
			pDC->SelectClipRgn(&m_rgnUpdate, RGN_COPY);
		m_pGraphics = &gdc;
		m_pGraphics->SetPageUnit(UnitPixel);
		m_pGraphics->ScaleTransform(1.0f, 1.0f);
	}
	m_pGraphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);
	m_pGraphics->SetPixelOffsetMode(PixelOffsetModeHighQuality);

	m_pGraphics->SetSmoothingMode(SmoothingModeNone);
	m_pGraphics->SetTextRenderingHint(TextRenderingHintAntiAlias);
	Rect clip;
	clip.X = m_rectClip.left;
	clip.Y = m_rectClip.top;
	clip.Width = m_rectClip.Width();
	clip.Height = m_rectClip.Height();
	m_rectfClip.X = (REAL)m_rectClip.left;
	m_rectfClip.Y = (REAL)m_rectClip.top;
	m_rectfClip.Width = (REAL)m_rectClip.Width();
	m_rectfClip.Height = (REAL)m_rectClip.Height();
	if(m_intDrawMode == DRAW_MODE_PRINT){
		if(m_bPrintLegend){
			int width = DrawPrintLegend() + (int)m_realPenWidth;
			if(m_dblLegendAngle == 0.0)
				clip.Height -= width;
			else if(m_dblLegendAngle == 90.0){
				clip.X += width;
				clip.Width -= width;
			}
			else if(m_dblLegendAngle == -90.0)
				clip.Width -= width;
		}
		if(m_bPrintFooter){
			DrawPrintFooter();
			clip.Y += (int)(FOOTER_MARGIN + m_realPenWidth);
			clip.Height -= (int)(FOOTER_MARGIN + m_realPenWidth);
		}
	}
	m_pGraphics->SetClip(clip, CombineModeReplace);

	FillBackground(FALSE);

	m_clocktStart = clock();

	m_scale.SetDragging(FALSE);
	m_memo.SetDragging(FALSE);
	m_bCancelDraw = TRUE;


	if(m_bAbortDraw){
		CRect rect;
		GetClientRect(&rect);
		CRgn rgn;
		rgn.CreateRectRgnIndirect(rect);
		pDC->SelectClipRgn(&rgn, RGN_COPY);
	}
	m_bAbortDraw = FALSE;


	pDC->SelectStockObject(HOLLOW_BRUSH);


	DrawGrid(m_pDC);

	double angle;
    switch(m_intMirror){
	case MIRROR_NONE:
		angle = m_dblViewRotation;
		break;
	case MIRROR_TO_X:
		angle = -m_dblViewRotation;
		break;
	case MIRROR_TO_Y:
		angle = -m_dblViewRotation + 180.0;
		break;
	}
	m_intLayerDraw = -1;
	double mag = 1.0;
	if(m_bDrawByLayer && !m_bInitView){
		for(unsigned i = 0;i < GDS_MAX_LAYER;i++){
			m_intLayerDraw = *(m_aryIntLayerOrder + i);
			if(m_intLayerDraw < 0)
				break;
			if(GetLayerStatus(m_intLayerDraw) > 0 && m_pStrActive->IsVisibleOnLayer(m_intLayerDraw))
				DrawStructure(0, m_pStrActive, m_coordViewOffset, FALSE, angle, mag);
		}
	}
	else
		DrawStructure(0, m_pStrActive, m_coordViewOffset, FALSE, angle, mag);

	UnlockWindowUpdate();

	if(m_intDrawMode == DRAW_MODE_PRINT && m_bPrintScale)
		DrawPrintScale();

	if(!m_bAbortDraw){
		DrawMemos(pDC);
		DrawScales(pDC);
		DrawAxis();
		DrawCenterMark();
	}

	if(m_bSkelton)
		DrawSkelton(pDC);

	m_bCancelDraw = FALSE;
	UnlockWindowUpdate();

    pDC->SetROP2(rop);

	if(m_hWnd)
		PostMessage(WM_USER_END_RENDER);
}

void CGdsView::OnDraw(CDC* pDC)
{
	//if(m_bInitView)
	//	SetRedraw(FALSE);

	m_uintDrawingAREF = 0;

	if(m_dblDrawScale <= m_dblDrawScaleFit && m_coordViewOffset.x == 0.0 && m_coordViewOffset.y == 0.0)
		m_bEverythingInView = TRUE;
	else
		m_bEverythingInView = FALSE;

	if(m_intDrawMode == DRAW_MODE_DISPLAY && m_bBufferScreen){
		if(!m_bForceRedraw){
			if(!m_bAutoRedraw){// || m_bRedrawPart){
				m_bBufferScreen = TRUE;
				ShowBufferBmp();
				DrawGridMarker();
				return;
			}
		} 
	}
	m_bBufferScreen = FALSE;
	OnButtonStopDraw();
	m_pMainFrame->ActivateToolTip(FALSE);
	m_pDC = pDC;
	SetCommandGuide(CMD_GUIDE_STOP_DRAW);

	if(m_intDrawMode == DRAW_MODE_DISPLAY){ 
		if(m_bSkelton || pDC->GetClipBox(&m_rectClip) == NULLREGION){
			GetClientRect(&m_rectClip);
			m_pDC->DPtoLP(&m_rectClip);
		}
	}
	m_rectClip.NormalizeRect();

	CRect rect(0, 0, m_intDrawLimitD, m_intDrawLimitD);
	pDC->DPtoLP(&rect);
	m_intDrawLimitL = abs(rect.Width());

	rect.SetRect(0, 0, m_intHatchLimitD, m_intHatchLimitD);
	pDC->DPtoLP(&rect); 
	m_intHatchLimitL = abs(rect.Width());

	rect.SetRect(0, 0, 3, 3);
	pDC->DPtoLP(&rect);
	m_intDPInvisibleLP = abs(rect.Width());

	rect.SetRect(0, 0, 1, 1);
	pDC->DPtoLP(&rect);
	m_intDPoneLP = abs(rect.Width())/2;
	m_intPathLimit = m_intDPoneLP * 2;
	m_realPenWidthLim = m_realPenWidthLim * 4.0f;

	rect.SetRect(0, 0, 1, 1);
	pDC->DPtoLP(&rect);
    rect.NormalizeRect();
	m_realScaleBmpX = (REAL)rect.Width();
	m_realScaleBmpY = (REAL)rect.Height();
	if(m_intDrawMode == DRAW_MODE_PRINT){
		REAL printQualityX = 
			(REAL)(m_pDC->GetDeviceCaps(LOGPIXELSX));
		m_realScaleBmpX *= printQualityX/m_pScreenDC->GetDeviceCaps(LOGPIXELSX);
		REAL printQualityY = 
			(REAL)(m_pDC->GetDeviceCaps(LOGPIXELSY));
		m_realScaleBmpY *= printQualityY/m_pScreenDC->GetDeviceCaps(LOGPIXELSY);
	}
	m_rectClip.InflateRect(rect.Width(), rect.Height());

	double angle;
    switch(m_intMirror){
	case MIRROR_NONE:
		angle = m_dblViewRotation;
		break;
	case MIRROR_TO_X:
		angle = -m_dblViewRotation;
		break;
	case MIRROR_TO_Y:
		angle = -m_dblViewRotation + 180.0;
		break;
	}
	if(m_pStrActive){
        DeleteCursor();
		CalcStructureAreaTop(angle);
		if(m_bInitView){
			CRgn rgn;
			rgn.CreateRectRgn(0, 0, 0, 0);
			pDC->SelectClipRgn(&rgn);
			ClearHitTestArray();
			ZoomFitWindow(FALSE);
		}
		if(m_intDrawMode != DRAW_MODE_DISPLAY || m_bInitView){
			CWaitCursor cursor;
			Render(m_pDC);
		}
        else{
            m_bCancelDraw = TRUE;
			if(m_pThreadRender)
				m_pThreadRender->AskDrawStructure();
			else
				Render();
        }
    }
	m_pDC = m_pScreenDC;
}

BOOL CGdsView::CheckStructureFrame(int level, CGDSStructure* structure, 
							 CCoord& pos, BOOL reflect, double& angle, double& mag, CGDSElement* element)
{
	if(!structure)
		return FALSE;

	if(!m_bOmitSmallStructure){
		if(!m_bInitView && m_bEverythingInView && level <= m_intExpandLevel)
			return TRUE;
		if(m_uintDrawingAREF > 0)
			return TRUE; 
	}

	static CCoord area[4];
	CalcStructureArea(area, structure, pos, reflect, angle, mag);
	if(m_bInitView && element && level == 1){
		(area + 0)->OffsetNeg(m_coordViewOffset);
		(area + 1)->OffsetNeg(m_coordViewOffset);
		AddSkeltonStructure(structure, angle, area, TRUE, element);
		return TRUE;
	}
	(area + 0)->Mul(m_dblDrawScale);
	(area + 1)->Mul(m_dblDrawScale);

	for(int i = 0;i < 2;i++){
		if((area + i)->x >= LONG_MAX)
			(area + i)->x = LONG_MAX;
		else if((area + i)->x <= LONG_MIN)
			(area + i)->x = LONG_MIN;
		if((area + i)->y >= LONG_MAX)
			(area + i)->y = LONG_MAX;
		else if((area + i)->y <= LONG_MIN)
			(area + i)->y = LONG_MIN;
	}
	CRect rect((long)(area + 0)->x, (long)(area + 0)->y, (long)(area + 1)->x, (long)(area +1)->y);
	if(IsOutOfView(rect))
		return FALSE;
	if(level > m_intExpandLevel){
		CalcStructureFrame(area, structure, pos, reflect, angle, mag);
		DrawStructureFrame(area, structure);
		return FALSE;
	}
	double h = min(((double)rect.top - rect.bottom), LONG_MAX);
	double w = min(((double)rect.right - rect.left), LONG_MAX);
	if((h < m_intDPoneLP && w < m_intDPoneLP) ||
		(m_bOmitSmallStructure && 
			w < m_intDrawLimitL && h < m_intDrawLimitL)){
		for(unsigned i = 0;i < GDS_MAX_LAYER;i++){
			if(GetLayerStatus(i) > 0 && structure->IsVisibleOnLayer(i)){
				m_pDC->FillSolidRect(&rect, COLOR_DUMMY_STRUCTURE);
				break;
			}
		}
		return FALSE;
	}

	return TRUE;
}

void CGdsView::fmod360(double& angle)
{
	if(angle >= 360.0)
		angle -= 360.0;
	else if(angle <= -360.0)
		angle += 360.0;
}

void CGdsView::DrawStructure(int level, CGDSStructure* structure, 
							 CCoord& pos, BOOL reflect, double& angle, double& mag)
{
	if(!structure)
		return;
	if(m_bAbortDraw || (m_bSkelton && level > 0))
		return;
	if(m_intDrawMode != DRAW_MODE_PRINT && !CheckStructureFrame(level, structure, pos, reflect, angle, mag))
		return;

	BOOL anydraw = FALSE;

	CObList* pElementList = structure->GetElementList();
	POSITION pidx = pElementList->GetHeadPosition();
	while(pidx && !m_bAbortDraw){
		CGDSElement* element = (CGDSElement*)pElementList->GetNext(pidx);
		if(!element)
			continue;
		switch(element->GetElementType()){
		case GDS_SREF:
			{
			CGDSStructure* refStructure = element->GetRefStructure();
			CCoord coord;
			element->GetPos(&coord);
			coord.Mul(mag);
			if(reflect)
				coord.y = -coord.y;
	    	RotatePoint(&coord, angle);
			coord += pos;

			double emag;
			((CGDSSREF*)element)->GetMag(&emag);
			emag *= mag;
			BOOL ereflect = reflect ^ (((CGDSStrans*)element)->IsReflect());

			double eangle;
			if(ereflect != ((CGDSStrans*)element)->IsReflect())
				eangle = angle - ((CGDSSREF*)element)->GetAngle();
			else
				eangle = angle + ((CGDSSREF*)element)->GetAngle();

			fmod360(eangle);
			if(CheckStructureFrame(level + 1, refStructure, coord, ereflect, eangle, emag, element))
				DrawStructure(level + 1, refStructure, coord, ereflect, eangle, emag);
			}
			anydraw = TRUE;
			break;
		case GDS_AREF:
			if(!m_bInitView || level < 1)
				DrawAref(level + 1, (CGDSAREF*)element, pos, reflect, angle, mag);
			anydraw = TRUE;
			break;
		case NULL:
			continue;
			break;
		default:
			if(!m_bInitView)
				anydraw |= DrawGDSElement(element, pos, reflect, angle, mag);
			break;
		}
	} 
	if(!m_bInitView && !anydraw) // if nothing was drawn
		DrawDummyStructure(structure, pos, reflect, angle, mag);
	return;
}

void CGdsView::DrawAref(int level, CGDSAREF* element, CCoord& pos, BOOL reflect, double& angle, double& mag)
{
	if(!element)
		return;

	CCoord coordOrg, coordCol, coordRow;
	element->GetNthCoord(0, &coordOrg);
	element->GetNthCoord(1, &coordCol);
	element->GetNthCoord(2, &coordRow);
	coordOrg.Mul(mag);
	coordCol.Mul(mag);
	coordRow.Mul(mag);
	if(reflect){
		coordCol.y = -coordCol.y;
		coordOrg.y = -coordOrg.y;
		coordRow.y = -coordRow.y;
	}

	unsigned numCol = element->GetNumCol();
	unsigned numRow = element->GetNumRow(); 

	double xstep, ystep;
	if(coordCol.x == coordOrg.x && coordRow.y == coordOrg.y){
		numCol = element->GetNumRow();
		numRow = element->GetNumCol();
		xstep = (coordRow.x - coordOrg.x)/(double)numCol;
		ystep = (coordCol.y - coordOrg.y)/(double)numRow;
	}
	else{
		xstep = (coordCol.x - coordOrg.x)/(double)numCol;
		ystep = (coordRow.y - coordOrg.y)/(double)numRow;
	}
	double emag;
	element->GetMag(&emag);
	emag *= mag;
	BOOL ereflect = reflect ^ element->IsReflect();
	double eangle;
	if(ereflect != element->IsReflect())
		eangle = angle - element->GetAngle();
	else
		eangle = angle + element->GetAngle();
	fmod360(eangle);



	if(m_bInitView){
		CCoord area[2];
		double minx, maxx, miny, maxy;
		for(unsigned i = 0;i < numRow;i++){
			for(unsigned j = 0;j < numCol;j++){
				CCoord coord(coordOrg.x + j * xstep, coordOrg.y + i * ystep);
	    		RotatePoint(&coord, angle);
				coord += pos;
				CalcStructureArea(area, element->GetRefStructure(), coord, ereflect, eangle, emag);
				(area + 0)->OffsetNeg(m_coordViewOffset);
				(area + 1)->OffsetNeg(m_coordViewOffset);
				if(i == 0 && j == 0){
					minx = maxx = (area + 0)->x;
					miny = maxy = (area + 0)->y;
				}
				for(int k = 0;k < 2;k++){
					if((area + k)->x < minx)
						minx = (area + k)->x;
					else if((area + k)->x > maxx)
						maxx = (area + k)->x;
					if((area + k)->y < miny)
						miny = (area + k)->y;
					else if((area + k)->y > maxy)
						maxy = (area + k)->y;
				}
			}
		}
		(area + 0)->SetCoord(minx, miny);
		(area + 1)->SetCoord(maxx, maxy);
		AddSkeltonStructure(element->GetRefStructure(), angle, area, FALSE, element);
	}
	else{
		BOOL checkEach = m_bEverythingInView ? FALSE : TRUE;
		CCoord area[2];
		if(!m_bEverythingInView && numCol > 2 && numRow > 2){// Check whole AREF is out of view
			CalcStructureArea(area, element->GetRefStructure(), coordOrg, ereflect, eangle, emag);
			double width = (area + 1)->x - (area + 0)->x;
			double height = (area + 0)->y - (area + 1)->y;
			if(xstep < 0.0)
				width = -width;
			if(ystep < 0.0)
				height = -height;
			
			CRect rect((long)((coordOrg.x - width) * m_dblDrawScale),
						(long)((coordOrg.y - height) * m_dblDrawScale),
						(long)((coordOrg.x + numCol * xstep + width) * m_dblDrawScale),
						(long)((coordOrg.y + numRow * ystep + height) * m_dblDrawScale));
			RotateRect(&rect, angle);
			rect.OffsetRect((long)(pos.x * m_dblDrawScale), (long)(pos.y * m_dblDrawScale));
			if(m_intMirror > 0){
				rect.top = -rect.top;
				rect.bottom = -rect.bottom;
			}

			///// for debug
			if(m_bShowAREFArea){
				CPen pen(PS_DASH, 0, RGB(255,255,0));
				CPen* pPen = (CPen*)m_pDC->SelectObject(&pen);
				m_pDC->MoveTo(rect.TopLeft());
				m_pDC->LineTo(rect.left, rect.bottom);
				m_pDC->LineTo(rect.right, rect.bottom);
				m_pDC->LineTo(rect.right, rect.top);
				m_pDC->LineTo(rect.TopLeft());
				m_pDC->SelectObject(pPen);
			}
			if(IsOutOfView(rect))
				return;
			rect.NormalizeRect();
			if((m_rectClip & rect) == rect)
				checkEach = FALSE;
		}
		for(unsigned i = 0;i < numRow;i++){ 
			for(unsigned j = 0;j < numCol;j++){
				CCoord coord(coordOrg.x + j * xstep, coordOrg.y + i * ystep);
				if(!m_bShowAREF && j == 0){ // draw outline only
					if(numCol > 2 && i > 0 && i < numRow - 1)
						j = numCol - 2;
				}
	    		RotatePoint(&coord, angle);
				coord += pos;

				if(checkEach){
					CalcStructureArea(area, element->GetRefStructure(), coord, ereflect, eangle, emag);
					(area + 0)->Mul(m_dblDrawScale);
					(area + 1)->Mul(m_dblDrawScale);
					CRect rect((long)(area + 0)->x, (long)(area + 0)->y, (long)(area + 1)->x, (long)(area + 1)->y);
					if(m_bShowAREFArea){
						CPen pen(PS_DASH, 0, RGB(255,255,0));
						CPen* pPen = (CPen*)m_pDC->SelectObject(&pen);
						m_pDC->MoveTo(rect.TopLeft());
						m_pDC->LineTo(rect.left, rect.bottom);
						m_pDC->LineTo(rect.right, rect.bottom);
						m_pDC->LineTo(rect.right, rect.top);
						m_pDC->LineTo(rect.TopLeft());
						m_pDC->SelectObject(pPen);
					}
					if(IsOutOfView(rect))
						continue;
					DrawStructure(level, element->GetRefStructure(), coord, ereflect, eangle, emag);
				}
				else{
					m_uintDrawingAREF++;
					DrawStructure(level, element->GetRefStructure(), coord, ereflect, eangle, emag);
					m_uintDrawingAREF--;
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
#include "PrintScaleDlg.h"

BOOL CGdsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->SetMaxPage(1);
	BOOL retval = DoPreparePrinting(pInfo);

	if(!retval) 
		return FALSE;

	SetRedraw(FALSE);

	CPrintDialog printdlg(FALSE); 
	CWinApp* app = AfxGetApp();
	if(!app->GetPrinterDeviceDefaults(&printdlg.m_pd)){
		if(app->DoPrintDialog(&printdlg) != IDOK)
		   return FALSE;
    }
	if(printdlg.CreatePrinterDC() == NULL){
		SetRedraw(TRUE);
		return FALSE;
	}
	CDC PrintDC;
	PrintDC.Attach(printdlg.m_pd.hDC);

	m_drawParam.PushParam(this);

	SIZE sz = GetTotalSize();
	m_rectView.BottomRight() = CPoint(sz.cx, sz.cy);
	m_rectView.TopLeft () = GetScrollPosition();

	CRect rect;
	GetClientRect(&rect);
	m_pDC->DPtoLP(&rect);
	rect.NormalizeRect();
	m_rectView.TopLeft().Offset(rect.Width()/2, -rect.Height()/2);
	m_rectView.left = (long)((m_rectView.right/2 - m_rectView.left)/m_dblDrawScale);
	m_rectView.top = (long)((-m_rectView.top - m_rectView.bottom/2)/m_dblDrawScale);
	if(m_intMirror > 0)
		m_rectView.top = -m_rectView.top;


	sz.cx = (long)(PrintDC.GetDeviceCaps(HORZSIZE) * 1e-3 / HIMETRIC);
	sz.cy = (long)(PrintDC.GetDeviceCaps(VERTSIZE) * 1e-3 / HIMETRIC);
	SetScrollSizes(MM_HIMETRIC, sz);

	SetOrigin(m_pDC);

	OnEraseBkgnd(m_pDC);
	CPrintScaleDlg dlg(this);
	dlg.SetPrintMode(m_intPrintMode);
	dlg.SetFitScale(m_dblPrintScale, m_dblPrintScaleArea);
	dlg.SetHatch(m_realHatchPenWidthPrint * (REAL)HIMETRIC, m_realHatchSpacingPrint * (REAL)HIMETRIC);
	dlg.SetShrinkRate(m_bOptShrink, m_dblShrinkRate);
	dlg.SetLineWidth(m_realPenWidthPrint * (REAL)HIMETRIC);
	m_bPrintPenColor = AfxGetApp()->GetProfileInt("Misc", "Print Pen Color", FALSE);
	dlg.SetMisc(m_bPrintLegend, m_bPrintScale, m_bPrintFooter, m_bPrintPenColor);
	m_bPrintFooter = TRUE; 
	dlg.SetLegendSize(m_dblLegendSize);
	dlg.SetLegendAngle(m_dblLegendAngle);

	CRect dlgrect;
	if(dlg.DoModal(&dlgrect) == IDCANCEL){
		SetRedraw(TRUE);
		ScreenToClient(&dlgrect);
		m_drawParam.PopParam(this);
		return FALSE;
	}

	if(m_dblShrinkRate > 0.0)
		m_bOptShrink = dlg.GetShrink();
	m_intPrintMode = dlg.GetPrintMode();
	m_dblPrintScale = dlg.GetPrintScale(TRUE);
	m_dblPrintScaleArea = dlg.GetPrintScale(FALSE);
	m_bPrintLegend = dlg.GetLegendSwitch();
	m_bPrintScale = dlg.GetScaleSwitch();
	m_bPrintFooter = dlg.GetFooterSwitch();
	m_bPrintPenColor = dlg.IsColorPen();
	m_dblLegendAngle = dlg.GetLegendAngle();
 	AfxGetApp()->WriteProfileInt("Misc", "Print Pen Color", m_bPrintPenColor);
	if(!m_bFillEnable)
		m_bPrintPenColor = TRUE;
	m_dblLegendSize = dlg.GetLegendSize();
	AfxGetApp()->WriteProfileInt("Misc", "Legend size", (int)(m_dblLegendSize * 1000.0));


	m_realPenWidth = m_realPenWidthPrint = dlg.GetLineWidth();

	double tmp = (int)(m_realPenWidth * 1e+6 + 0.5);
 	AfxGetApp()->WriteProfileInt("Misc", "Print Pen Width", (int)tmp);
	m_realPenWidth /= (REAL)HIMETRIC;
	m_realPenWidthPrint /= (REAL)HIMETRIC;


	m_realHatchPenWidthPrint = dlg.GetHatchPenWidth();
	tmp = (int)(m_realHatchPenWidthPrint * 1e+6 + 0.5);
 	AfxGetApp()->WriteProfileInt("Misc", "Hatch Pen Width", (int)tmp);
	m_realHatchPenWidthPrint /= (REAL)HIMETRIC;

	m_realHatchSpacingPrint = dlg.GetHatchSpacing();
	tmp = (int)(m_realHatchSpacingPrint * 1e+6 + 0.5);
 	AfxGetApp()->WriteProfileInt("Misc", "Hatch Spacing", (int)tmp);
	m_realHatchSpacingPrint /= (REAL)HIMETRIC;

	return retval;
}

void CGdsView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	m_pDC = pDC;
	SetRedraw(FALSE);
}

void CGdsView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	m_drawParam.PopParam(this);
	SetRedraw(TRUE);
	m_bForceRedraw = TRUE;
	InvalidateRect(TRUE, NULL);
}

void CGdsView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	m_pMainFrame->CreateAbortDrawTimer();
	SIZE sz;
	sz.cx = abs(pInfo->m_rectDraw.right - pInfo->m_rectDraw.left);
	sz.cy = abs(pInfo->m_rectDraw.bottom - pInfo->m_rectDraw.top);
	SetScrollSizes(MM_HIMETRIC, sz);
	SetOrigin(pDC);

	m_intDrawMode = DRAW_MODE_PRINT;
	CCoord offset_store = m_coordViewOffset;

	switch(m_intPrintMode){
	case PRINT_MODE_WYSIWYG:
		MoveViewOffset(m_rectView.left, m_rectView.top);
		break;
	case PRINT_MODE_SCALED_FULL:
		OnFitView();
		m_dblDrawScale = m_dblPrintScale * (GetGDShcy()->GetDBUnit()/ HIMETRIC);
		if(m_bOptShrink)
			m_dblDrawScale *= (1.0 - m_dblShrinkRate);
		break;
	case PRINT_MODE_SCALED_AREA:
		MoveViewOffset(m_rectView.left, m_rectView.top);
		m_dblDrawScale = m_dblPrintScaleArea * (GetGDShcy()->GetDBUnit()/ HIMETRIC);
		if(m_bOptShrink)
			m_dblDrawScale *= (1.0 - m_dblShrinkRate);
		break;
	default:
		break;
	}

	CRect rect;
	BOOL redraw = GetUpdateRect(&rect);
	SetRedraw(FALSE);

	m_rectClip.SetRect(-sz.cx/2, sz.cy/2, sz.cx/2, -sz.cy/2);

	CScrollView::OnPrint(pDC, pInfo);
	SetRedraw(TRUE);
	if(redraw)
		InvalidateRect(TRUE, &rect);

	m_coordViewOffset = offset_store;
	m_intDrawMode = DRAW_MODE_DISPLAY;
	m_pMainFrame->KillAbortDrawTimer();

}


/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void CGdsView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CGdsView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CGdsDoc* CGdsView::GetDocument()
{
//	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGdsDoc)));
	return (CGdsDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

void CGdsView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();

	m_pGrid = new CGrid(GetGDShcy()->GetDBUnit());

	((CGdsApp*)AfxGetApp())->SetCGdsViewHWND(m_hWnd);

	m_intNumActiveLayer = 0;
	for(int i = 0;i < sizeof(m_bShowList)/sizeof(int);i++){
		*(m_aryIntLayerOrder + i) = -1;
		*(m_bShowList + i) = FALSE;
		*(m_bLayerInTec + i) = FALSE;
		if(GetGDShcy()->DoesLayerExist(i)){
			*(m_bLayerExist + i) = TRUE;
			*(m_bShowList + i) = TRUE;
			*(m_aryIntLayerOrder + m_intNumActiveLayer) = i;
			m_intNumActiveLayer++;
		}
	}
    SetDefaultLayerColor();
	RecalcNumActiveLayer();
	m_pStrActive = GetGDShcy()->FindTopStructure();

	double scale = GetGDShcy()->GetDBUnit(); // Database unit in meter
	scale /= HIMETRIC;						 // Scale meter to HIMETRIC
	m_dblDrawScale = scale;
	SetCommandGuide(CMD_GUIDE_NONE);
	m_bInitView = TRUE;
	m_pMainFrame->AddView(this);

	CRect rect;
	m_pMainFrame->GetDropDownRect(&rect);
	CreateComboBox(rect, m_pMainFrame->GetToolBar());
	m_pMainFrame->GetStrLvlRect(&rect);
	CreateStrLvlBox(rect, m_pMainFrame->GetToolBar());

	m_pScreenDC = new CClientDC(this);
	OnPrepareDC(m_pScreenDC);
	SetOrigin(m_pScreenDC);
	m_pScreenDC->SelectClipRgn(NULL);
	m_pScreenDC->SetBkMode(TRANSPARENT);

	m_pDC = m_pScreenDC;
	CreateRenderThread();

	ReadWriteIniFile(TRUE);
	m_bViewInitialized = TRUE;

	DragAcceptFiles();
}

COLORREF CGdsView::ReverseGray(COLORREF rgb)
{
	static const unsigned char diff = 10;
	if(m_colorBK == COLOR_WHITE)
		return rgb;
	if(abs(GetRValue(rgb) - GetGValue(rgb)) > diff ||
		abs(GetRValue(rgb) - GetBValue(rgb)) > diff ||
		abs(GetGValue(rgb) - GetBValue(rgb)) > diff)
		return rgb;
	return RGB(255 - GetRValue(rgb), 255 - GetGValue(rgb), 255 - GetBValue(rgb));
}

BOOL CGdsView::DrawGDSElement(CGDSElement *element, CCoord& pos, 
							  BOOL reflect, double& angle, double& mag)
{
	m_intLayerNow = element->GetLayer();
	if(m_bDrawByLayer && m_intLayerDraw != m_intLayerNow) 
		return TRUE;
	if(!*(m_bShowList + m_intLayerNow)) // if layer invisible
		return FALSE;

	m_colorLayerRGB = m_colorPenRGB = *(m_aryColorLayerRGB + m_intLayerNow);
	m_colorLayer = m_colorPen = *(m_aryColorLayer + m_intLayerNow);

	if(m_intDrawMode == DRAW_MODE_PRINT){
		// Reverse BLACK<->WHITE @ print
		m_colorLayerRGB = ReverseGray(m_colorLayerRGB);
		m_colorLayer.SetFromCOLORREF(m_colorLayerRGB);

		m_colorPenRGB = ReverseGray(m_colorPenRGB);
		m_colorPen.SetFromCOLORREF(m_colorPenRGB);
		if(!m_bPrintPenColor && *(m_aryIntFillType + m_intLayerNow) != FILL_BMP_NONE){
			m_colorPenRGB = COLOR_BLACK;
			m_colorPen = Color::Black;
		}

		//m_colorPenRGB = ReverseGray(m_colorPenRGB);
		//m_colorPen.SetFromCOLORREF(m_colorPenRGB);
	}
	if(!m_bOverWrite){
		m_colorPen.SetValue(Color::MakeARGB(m_byteTransparent, GetRValue(m_colorLayerRGB), 
					GetGValue(m_colorLayerRGB), GetBValue(m_colorLayerRGB)));
		m_colorLayer.SetValue(Color::MakeARGB(m_byteTransparent, GetRValue(m_colorPenRGB), 
					GetGValue(m_colorPenRGB), GetBValue(m_colorPenRGB)));
	}

	switch(element->GetElementType()){
	case GDS_BOUNDARY:
	case GDS_BOX:
		if(m_bShowBoundary)
			return DrawGDSBoundary((CGDSBoundary*)element, pos, reflect, angle, mag);
		else
			return TRUE;
		break;
	case GDS_PATH:
		if(m_bShowPath)
			return DrawGDSPath((CGDSPath*)element, pos, reflect, angle, mag);
		else
			return TRUE;
		break;
	case GDS_TEXT:
		DrawGDSText((CGDSText*)element, pos, reflect, angle, mag);
		break;
	/*case GDS_NODE:
		// Not supported yet
		DrawGDSNode((CGDSNode*)element, pos, reflect, angle, mag);
		break;
	case GDS_CONTACT:
		// Not supported yet
		DrawGDSContact((CGDSNode*)element, pos, reflect, angle, mag);
		break;*/
	default:
		break;
	}
	return FALSE;
}


BOOL CGdsView::DrawGDSPath(CGDSPath *element, 
						   CCoord pos, BOOL reflect, double angle, double mag)
{
	unsigned numP = ((CGDSXY*)element)->GetNumCoords();
	if(numP == 0)
		return FALSE;

	REAL pwidth = (REAL)(element->GetWidth() * m_dblDrawScale);
	if(pwidth < 0.0)
		pwidth = -(REAL)(element->GetWidth() * HIMETRIC);
	else
		pwidth *= (REAL)mag;
	pos.Mul(m_dblDrawScale);
	mag *= m_dblDrawScale;
	if(m_intDrawMode == DRAW_MODE_DISPLAY){
		switch(DrawDummyElement(element, pos, reflect, angle, mag)){
		case ELEMENT_OMITTED:
			return FALSE;
			break;
		case ELEMENT_DUMMY:
			return TRUE;
			break;
		case ELEMENT_TO_BE_DRAWN:
		default:
			break;
		}
	}


	CiCoord *pCoord = ((CGDSXY*)element)->GetCoordArray();

	for(unsigned i = 0;i < numP;i++){
		(points + i)->X = (REAL)(mag * (pCoord + i)->x);
		if(reflect)
			(points + i)->Y = -(REAL)(mag * (pCoord + i)->y);
		else
			(points + i)->Y = (REAL)(mag * (pCoord + i)->y);
	}
	RotatePoints(points, angle, numP);


	CRect rect;
	long x, y;
	BOOL needClip = FALSE;
	for(i = 0;i < numP;i++){
		(points + i)->X += (REAL)pos.x;
		(points + i)->Y += (REAL)pos.y;
		if(m_intMirror > 0)
			(points + i)->Y = -(points + i)->Y;
		needClip |= NeedClip(points + i);
		x = (long)(points + i)->X;
		y = (long)(points + i)->Y;
		if(i == 0){
			rect.left = rect.right = x;
			rect.top = rect.bottom = y;
		}
		else{
			if(x < rect.left)
				rect.left = x;
			else if(x > rect.right)
				rect.right = x;
			if(y > rect.top)
				rect.top = y;
			else if(y < rect.bottom)
				rect.bottom = y;
		}
	}
	if(m_intDrawMode == DRAW_MODE_DISPLAY && numP > 2 && !needClip){
		rect.InflateRect((int)pwidth/2, -(int)pwidth/2);
		if(rect.Width() < m_intDrawLimitL || -rect.Height() < m_intDrawLimitL){
			DrawRect(&rect);
			return TRUE;
		}
	}

	Pen pen(m_colorPen, pwidth);
	LineCap cap;
	pen.SetMiterLimit(pwidth * 10.0f);
	switch(element->GetPathType()){
	case 0:
		cap = LineCapFlat;
		pen.SetLineCap(cap, cap, DashCapFlat);
		pen.SetLineJoin(LineJoinMiterClipped);
		break;
	case 4:
		cap = LineCapFlat;
		pen.SetLineCap(cap, cap, DashCapFlat);
		pen.SetLineJoin(LineJoinMiterClipped);
		if(numP > 2)
			AdjustPathEnds(points, numP, 
							element->GetBgnExtn() * mag, element->GetEndExtn() * mag);
		break;
	case 1:
		cap = LineCapRound;
		pen.SetLineCap(cap, cap, DashCapFlat);
		pen.SetLineJoin(LineJoinRound);
		break;
	case 2:
		cap = LineCapSquare;
		pen.SetLineCap(cap, cap, DashCapFlat);
		pen.SetLineJoin(LineJoinMiterClipped);
		break;
	default:
		cap = LineCapFlat;
		pen.SetLineCap(cap, cap, DashCapFlat);
		pen.SetLineJoin(LineJoinMiterClipped);
		break;
	}

	if(m_intDrawMode == DRAW_MODE_DISPLAY){
		if(!m_bOutlinePath || pwidth < m_intPathLimit){
			m_pGraphics->DrawLines(&pen, points, numP);
			return TRUE;
		}
	}

	GraphicsPath path(FillModeAlternate);
	path.AddLines(points, numP);
	path.Widen(&pen);
	//path.Flatten();

	float xf = (float)(rect.CenterPoint().x - m_coordViewOffset.x * m_dblDrawScale);
	float yf = (float)(rect.CenterPoint().y - m_coordViewOffset.y * m_dblDrawScale);
	Matrix mat;
	Status stat;
	float s1, s2;
	unsigned maxtry = 20;
	for(i = 0;i < maxtry;i++){
		//path.Reverse();
		s1 = 1.2f * (i + 1);
		s2 = 1.0f/s1;
		mat.Reset();
		mat.Translate(-xf, -yf, MatrixOrderAppend);
		mat.Scale(s1, s1, MatrixOrderAppend);
		path.Transform(&mat);
		stat = path.Outline();
		mat.Reset();
		mat.Scale(s2, s2, MatrixOrderAppend);
		mat.Translate(xf, yf, MatrixOrderAppend);
		path.Transform(&mat);
		if(stat == Ok)
			break;
	}
	//if(stat != Ok)
	//	stat = stat;


	if(m_intDrawMode == DRAW_MODE_PRINT){
		if(m_realPenWidthLim > rect.Width() || m_realPenWidthLim > -rect.Height())
			m_pPenDraw->SetWidth(0.0);
		else
			m_pPenDraw->SetWidth(m_realPenWidth);
	}
	else
		m_pPenDraw->SetWidth(m_realPenWidth);
	m_pPenDraw->SetColor(m_colorPen);


	if(needClip || m_intDrawMode == DRAW_MODE_CLIPBOARD || m_bClipPolygon)
		ClipPath(&path);
	m_pGraphics->DrawPath(m_pPenDraw, &path);
	if(m_bFillEnable && pwidth >= m_intHatchLimitL)
		FillPolygon(&path);

	return TRUE;
}


void CGdsView::FillPolygon(GraphicsPath* path, BOOL legend)
{
	if(*(m_aryIntFillType + m_intLayerNow) == FILL_BMP_NONE)
		return;

	if((m_intDrawMode == DRAW_MODE_PRINT || m_bHatchExactly) && *(m_aryIntFillType + m_intLayerNow) != FILL_BMP_SOLID)
		FillPolygonPrint(path, legend);
	else
		FillPolygonScreen(path);
}

void CGdsView::DrawRect(CRect* pRect)
{

	int w = pRect->Width();
	int h = -pRect->Height();
	if(w < m_intDPoneLP && h < m_intDPoneLP)
		return;

	static int lim_fill = m_intDPoneLP * 2;
	static int lim_len = m_intDPInvisibleLP * 2;

	if((w > m_intDPInvisibleLP && h > m_intDPInvisibleLP) ||
		((w > lim_len || h > lim_len) && (w < lim_fill || h < lim_fill))){
		/*static */CPen pen;
		m_pDC->MoveTo(pRect->left, pRect->top);
		pen.CreatePen(PS_SOLID, 0, m_colorLayerRGB);
		m_pDC->SelectObject(&pen);
		m_pDC->LineTo(pRect->left, pRect->bottom);
		m_pDC->LineTo(pRect->right, pRect->bottom);
		m_pDC->LineTo(pRect->right, pRect->top);
		m_pDC->LineTo(pRect->left, pRect->top);
		pen.DeleteObject();
	}
	else
		m_pDC->FillSolidRect(pRect, m_colorLayerRGB);

}

void CGdsView::FillPolygonScreen(GraphicsPath* path)
{
	REAL ratio = (REAL)(1.0 - m_dblHatchDensity / HATCH_DENSITY_DIV);
	REAL scalex = m_realScaleBmpX * ratio;
	REAL scaley = m_realScaleBmpY * ratio;

	BITMAP bmpobj;
	CBitmap* bmplayer = (m_bmpAryFill + *(m_aryIntFillType + m_intLayerNow));
	bmplayer->GetObject(sizeof(bmpobj), &bmpobj);
	int count = bmpobj.bmWidthBytes * bmpobj.bmHeight;
	BYTE* bmpary = new BYTE[count];
	bmplayer->GetBitmapBits(count, bmpary);
	BitmapData bitmapData;

	Bitmap bitmap(bmpobj.bmWidth, bmpobj.bmHeight, m_pGraphics);
	Rect rect(0, 0, bmpobj.bmWidth, bmpobj.bmHeight);
	bitmap.LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);
	UINT* pixels = (UINT*)bitmapData.Scan0;

	for(int row = 0; row < bmpobj.bmHeight; row++){
		for(int col = 0; col < bmpobj.bmWidth; col++){
		int row_pixel = bmpobj.bmHeight - row - 1;
		if(*(bmpary +  bmpobj.bmWidthBytes * row + col / 8) & (0x80 >> col % 8))
			pixels[row_pixel * bitmapData.Stride / 4 + col] = 0;
		else
			pixels[row_pixel * bitmapData.Stride / 4 + col]  
			 = (m_colorLayer.GetA() << 24) | (m_colorLayer.GetR() << 16) | 
			   (m_colorLayer.GetG() << 8) | m_colorLayer.GetB();
	  }
	}
	bitmap.UnlockBits(&bitmapData);
	delete [] bmpary;

	TextureBrush brush((&bitmap), WrapModeTile);
	brush.ScaleTransform(scalex, scaley);

	if(m_intDrawMode == DRAW_MODE_CLIPBOARD){
		Region r(path);
		Rect rct(m_rectClip.left, m_rectClip.top, m_rectClip.Width(), m_rectClip.Height());
		r.Intersect(rct);
		m_pGraphics->FillRegion(&brush, &r);
	}
	else
		m_pGraphics->FillPath(&brush, path);
	return;
}

void CGdsView::FillPolygonPrint(GraphicsPath* path, BOOL legend)
{
	CHatchStyle style;
 
	style.m_dblSpace = m_realHatchSpacingPrint;

	switch(*(m_aryIntFillType + m_intLayerNow)){
	case FILL_BMP_FDIAG:
		style.m_dblAngle = 45.0;
		style.m_bCross = FALSE;
		style.m_dblDashRatio = 1.0;
		style.m_dblOffsetX = 0.0;
		style.m_dblOffsetY = 0.0;
		style.m_intLineShape = CHatchStyle::HatchLineStraight;
		style.m_bPhaseShift = FALSE;
		break;
	case FILL_BMP_RDIAG:
		style.m_dblAngle = -45.0;
		style.m_bCross = FALSE;
		style.m_dblDashRatio = 1.0;
		style.m_dblOffsetX = 0.0;
		style.m_dblOffsetY = 0.0;
		style.m_intLineShape = CHatchStyle::HatchLineStraight;
		style.m_bPhaseShift = FALSE;
		break;
	case FILL_BMP_FDIAG_DASH:
		style.m_dblAngle = 45.0;
		style.m_bCross = FALSE;
		style.m_dblDashRatio = 1.0/2.0;
		style.m_dblOffsetX = 0.0;
		style.m_dblOffsetY = 0.0;
		style.m_intLineShape = CHatchStyle::HatchLineStraight;
		style.m_bPhaseShift = FALSE;
		break;
	case FILL_BMP_RDIAG_DASH:
		style.m_dblAngle = -45.0;
		style.m_bCross = FALSE;
		style.m_dblDashRatio = 1.0/2.0;
		style.m_dblOffsetX = 0.0;
		style.m_dblOffsetY = 0.0;
		style.m_intLineShape = CHatchStyle::HatchLineStraight;
		style.m_bPhaseShift = FALSE;
		break;
	case FILL_BMP_HWAVE:
		style.m_dblAngle = 0.0;
		style.m_bCross = FALSE;
		style.m_dblDashRatio = 1.0;
		style.m_dblOffsetX = 0.0;
		style.m_dblOffsetY = 0.0;
		style.m_intLineShape = CHatchStyle::HatchLineWave;
		style.m_bPhaseShift = FALSE;
		break;
	case FILL_BMP_VWAVE:
		style.m_dblAngle = 90.0;
		style.m_bCross = FALSE;
		style.m_dblDashRatio = 1.0;
		style.m_dblOffsetX = 0.0;
		style.m_dblOffsetY = 0.0;
		style.m_intLineShape = CHatchStyle::HatchLineWave;
		style.m_bPhaseShift = FALSE;
		break;
	case FILL_BMP_VHATCH:
		style.m_dblAngle = 90.0;
		style.m_bCross = FALSE;
		style.m_dblDashRatio = 1.0;
		style.m_dblOffsetX = 0.0;
		style.m_dblOffsetY = 0.0;
		style.m_intLineShape = CHatchStyle::HatchLineStraight;
		style.m_bPhaseShift = FALSE;
		break;
	case FILL_BMP_HHATCH:
		style.m_dblAngle = 0.0;
		style.m_bCross = FALSE;
		style.m_dblDashRatio = 1.0;
		style.m_dblOffsetX = 0.0;
		style.m_dblOffsetY = 0.0;
		style.m_intLineShape = CHatchStyle::HatchLineStraight;
		style.m_bPhaseShift = FALSE;
		break;
	case FILL_BMP_XDIAG:
		style.m_dblAngle = 45.0;
		style.m_bCross = TRUE;
		style.m_dblDashRatio = 1.0;
		style.m_dblOffsetX = style.m_dblSpace / 2.0;
		style.m_dblOffsetY = style.m_dblSpace / 2.0;
		style.m_intLineShape = CHatchStyle::HatchLineStraight;
		style.m_bPhaseShift = FALSE;
		break;
	case FILL_BMP_XHATCH:
		style.m_dblAngle = 0.0;
		style.m_bCross = TRUE;
		style.m_dblDashRatio = 1.0;
		style.m_dblOffsetX = style.m_dblSpace / 2.0;
		style.m_dblOffsetY = style.m_dblSpace / 2.0;
		style.m_intLineShape = CHatchStyle::HatchLineStraight;
		style.m_bPhaseShift = FALSE;
		break;
	case FILL_BMP_HDASH:
		style.m_dblAngle = 0.0;
		style.m_bCross = FALSE;
		style.m_dblDashRatio = 1.0/2.0;
		style.m_dblOffsetX = 0.0;
		style.m_dblOffsetY = style.m_dblSpace / 2.0;
		style.m_intLineShape = CHatchStyle::HatchLineStraight;
		style.m_bPhaseShift = TRUE;
		break;
	case FILL_BMP_VDASH:
		style.m_dblAngle = 90.0;
		style.m_bCross = FALSE;
		style.m_dblDashRatio = 1.0/2.0;
		style.m_dblOffsetX = style.m_dblSpace / 2.0;
		style.m_dblOffsetY = 0.0;
		style.m_intLineShape = CHatchStyle::HatchLineStraight;
		style.m_bPhaseShift = TRUE;
		break;
	case FILL_BMP_VZIGZAG:
		style.m_dblAngle = 90.0;
		style.m_bCross = FALSE;
		style.m_dblDashRatio = 1.0;
		style.m_dblOffsetX = -style.m_dblSpace / 2.0;
		style.m_dblOffsetY = 0.0;
		style.m_intLineShape = CHatchStyle::HatchLineZigzag;
		style.m_bPhaseShift = FALSE;
		break;
	case FILL_BMP_HZIGZAG:
		style.m_dblAngle = 0.0;
		style.m_bCross = FALSE;
		style.m_dblDashRatio = 1.0;
		style.m_dblOffsetX = style.m_dblSpace / 3.0;
		style.m_dblOffsetY = style.m_dblSpace / 3.0;
		style.m_intLineShape = CHatchStyle::HatchLineZigzag;
		style.m_bPhaseShift = FALSE;
		break;
	case FILL_BMP_LIGHT:
		style.m_dblAngle = 45.0;
		style.m_bCross = FALSE;
		style.m_dblDashRatio = 1.0/2.0;
		style.m_dblOffsetX = 0.0;
		style.m_dblOffsetY = style.m_dblSpace / 2.0;
		style.m_intLineShape = CHatchStyle::HatchLineStraight;
		style.m_bPhaseShift = TRUE;
		style.m_dblSpace /= 4.0f;
		break;
	case FILL_BMP_XHATCH_HD:
		style.m_dblAngle = 45.0;
		style.m_bCross = TRUE;
		style.m_dblDashRatio = 1.0;
		style.m_dblOffsetX = 0.0;
		style.m_dblOffsetY = style.m_dblOffsetX;
		style.m_intLineShape = CHatchStyle::HatchLineStraight;
		style.m_bPhaseShift = FALSE;
		style.m_dblSpace /= 2.0f;
		break;
	default:
		break;
	}

	RectF rect;
	path->GetBounds(&rect);
	rect.Intersect(m_rectfClip);

	REAL ratio = 1.0f;
	if(rect.IsEmptyArea()){
		if(!legend)
			return;
		//else  // this may cause empty legend
		//	ratio *= 0.5f;
	}


	REAL step = (REAL)(style.m_dblSpace);
	REAL offsetx = (REAL)(style.m_dblOffsetX);
	REAL offsety = (REAL)(style.m_dblOffsetY);

	step *= ratio;
	offsetx *= ratio;
	offsety *= ratio;

	PointF center;
	center.X = rect.X + rect.Width / 2.0f;
	center.Y = rect.Y + rect.Height / 2.0f;
	REAL r = (REAL)sqrt(rect.Width * rect.Width + rect.Height * rect.Height) / 2.0f;
	REAL y, ydraw, xdraw, x1, x2, y1, y2, a;


	Pen pen(m_colorLayer, m_realHatchPenWidthPrint);
	//pen.SetWidth((REAL)(style.m_dblPenWidth / HIMETRIC));
	static REAL pat[2];
	static PointF curve[5];
	static REAL rt2 = (REAL)sqrt(2.0);

	for(int i = 0;i < (style.m_bCross ? 2 : 1) && !m_bAbortDraw;i++){
		if(i > 0)
			style.m_dblAngle += 90.0;


		GraphicsState state = m_pGraphics->Save();

		m_pGraphics->SetClip(path, CombineModeIntersect);
		m_pGraphics->TranslateTransform(center.X, center.Y, MatrixOrderPrepend);
		m_pGraphics->RotateTransform((REAL)style.m_dblAngle, MatrixOrderPrepend);

		int line_count = 0;
		for(y = -r - step;y <= r + step && !m_bAbortDraw;y += step, line_count++){
			ydraw = y + offsety;
			int count = 0;
			if(style.m_dblDashRatio < 1.0){
				pat[0] = (REAL)(step * style.m_dblDashRatio) / m_realHatchPenWidthPrint;
				pat[1] = (REAL)(step * (1.0 - style.m_dblDashRatio)) / m_realHatchPenWidthPrint;
				if(!style.m_bPhaseShift || line_count % 2)
					pen.SetDashOffset(0);
				else
					pen.SetDashOffset((pat[0] + pat[1])/2.0f);
				pen.SetDashStyle(DashStyleDash);
				pen.SetDashPattern(pat, 2);
			}
			switch(style.m_intLineShape){
			case CHatchStyle::HatchLineWave:
				a = step / 3.0f;
				for(x1 = -r - step;x1 <= r + step;x1 += step, count++){
					xdraw = x1 + offsetx;
					curve[0].Y = curve[4].Y = ydraw;
					curve[0].X = xdraw;
					curve[1].X = xdraw + step / 4.0f;
					curve[2].X = xdraw + step / 2.0f;
					curve[3].X = xdraw + step * 3.0f/4.0f;
					curve[4].X = xdraw + step;
					if(count % 2 == 0){
						curve[1].Y = ydraw + a / rt2;
						curve[2].Y = ydraw + a;
						curve[3].Y = ydraw + a / rt2;
					}
					else{
						curve[1].Y = ydraw - a / rt2;
						curve[2].Y = ydraw - a;
						curve[3].Y = ydraw - a / rt2;
					}
					m_pGraphics->DrawCurve(&pen, curve, 5);
				}
				break;
			case CHatchStyle::HatchLineZigzag:
				a = step / 2.0f;
				for(x1 = -r;x1 <= r;x1 += step, count++){
					xdraw = x1 + offsetx;
					x2 = xdraw + step;
					if(count % 2 == 0){
						y1 = ydraw - a;
						y2 = ydraw + a;
					}
					else{
						y2 = ydraw - a;
						y1 = ydraw + a;
					}
					m_pGraphics->DrawLine(&pen, xdraw, y1, x2, y2);
				}
				break;
			case CHatchStyle::HatchLineStraight:
			default:
				m_pGraphics->DrawLine(&pen, -r, ydraw, r, ydraw);
				break;
			}
		}
		m_pGraphics->Restore(state);
	}
	return;
}

void CGdsView::DrawDummySkelton(CDC* pDC, CRect* rect, COLORREF color)
{
	rect->NormalizeRect();
	pDC->FillSolidRect(rect, color);
	return;
}

void CGdsView::DrawDummyStructure(CGDSStructure* structure, 
							 CCoord& pos, BOOL reflect, double& angle, double& mag)
{
	for(unsigned i = 0;i < GDS_MAX_LAYER;i++){
		if(GetLayerStatus(i) > 0 && structure->IsVisibleOnLayer(i)){
			i = 0;
			break;
		}
	}
	if(i != 0)
		return;

	CCoord area[2];
	CalcStructureArea(area, structure, pos, reflect, angle, mag);
	(area + 0)->Mul(m_dblDrawScale);
	(area + 1)->Mul(m_dblDrawScale);
	CRect rect((long)(area + 0)->x, (long)(area + 0)->y, (long)(area + 1)->x, (long)(area +1)->y);
	if(rect.Width() > m_intDrawLimitL || -rect.Height() > m_intDrawLimitL){
		return;
		// A big structure may consist of very small elements
		// all omitted.
		//
		//CPen pen(PS_DOT, 0, COLOR_DUMMY_STRUCTURE);
		//CPen* pPen = (CPen*)m_pDC->SelectObject(&pen);
		//m_pDC->MoveTo(rect.TopLeft());
		//m_pDC->LineTo(rect.left, rect.bottom);
		//m_pDC->LineTo(rect.right, rect.bottom);
		//m_pDC->LineTo(rect.right, rect.top);
		//m_pDC->LineTo(rect.TopLeft());
		//m_pDC->SelectObject(pPen);
	}
	else
		m_pDC->FillSolidRect(&rect, COLOR_DUMMY_STRUCTURE);
}


void CGdsView::DrawStructureFrame(CCoord* pframe, CGDSStructure* structure)
{
	for(unsigned i = 0;i < GDS_MAX_LAYER;i++){
		if(GetLayerStatus(i) > 0 && structure->IsVisibleOnLayer(i)){
			i = 0;
			break;
		}
	}
	if(i != 0)
		return;

	static CRect rect;
	static long x, y;

	for(i = 0;i < 4;i++){
		x = (long)(pframe + i)->x;
		y = (long)(pframe + i)->y;
		if(i == 0){
			rect.left = rect.right = x;
			rect.top = rect.bottom = y;
		}
		else{
			if(x > rect.right)
				rect.right = x;
			else if(x < rect.left)
				rect.left = x;
			if(y > rect.top)
				rect.top = y;
			else if(y < rect.bottom)
				rect.bottom = y;
		}
	}
	if(rect.Width() > m_intDrawLimitL && -rect.Height() > m_intDrawLimitL){
		Color color;
		static REAL pat[2];
		static PointF points[4];

		if(m_colorBK == COLOR_BLACK)
			color = Color::White;
		else
			color = Color::Black;
		Pen pen(color, 0.0);
		pat[0] = pat[1] = m_realScaleBmpX * 4.0f;
		pen.SetDashStyle(DashStyleDash);
		pen.SetDashPattern(pat, 2);
		for(int i = 0;i < 4;i++){
			(points + i)->X = (REAL)(pframe + i)->x;
			(points + i)->Y = (REAL)(pframe + i)->y;
		}
		m_pGraphics->DrawPolygon(&pen, points, 4);
		m_pGraphics->DrawLine(&pen, *(points + 0), *(points + 2));
		m_pGraphics->DrawLine(&pen, *(points + 1), *(points + 3));

	}
	else{
		COLORREF color;
		if(m_colorBK == COLOR_BLACK)
			color = COLOR_WHITE;
		else
			color = COLOR_BLACK;
		if(rect.Width() < m_intDPInvisibleLP || -rect.Height() < m_intDPInvisibleLP)
			m_pDC->FillSolidRect(&rect, color);
		else{
			static CPen pen;
			pen.CreatePen(PS_SOLID, 0, color);
			m_pDC->SelectObject(&pen);
			m_pDC->SelectStockObject(NULL_BRUSH);
			m_pDC->Rectangle(rect.left, rect.bottom, rect.right, rect.top);
			pen.DeleteObject();
		}
	}
	return;
}

void CGdsView::CalcStructureAreaTop(double angle)
{
	if(!m_pStrActive){
		m_coordMin.SetCoord(0.0, 0.0);
		m_coordMax.SetCoord(0.0, 0.0);
		return;
	}
	CCoord area[4], cmin, cmax;
	m_pStrActive->GetDimMin(&cmin);
	m_pStrActive->GetDimMax(&cmax);
	(area + 0)->x = (area + 1)->x = cmin.x;
	(area + 2)->x = (area + 3)->x = cmax.x;
	(area + 0)->y = (area + 3)->y = cmax.y;
	(area + 1)->y = (area + 2)->y = cmin.y;

	RotatePoints(area, angle, 4);
	if(m_intMirror > 0){
		for(int i = 0;i < 4;i++)
			(area + i)->y = -(area + i)->y;
	}
	for(int i = 0;i < 4;i++){
		if(i == 0){
			m_coordMin.Copy(area + i);
			m_coordMax.Copy(area + i);
		}
		else{
			if((area + i)->x < m_coordMin.x)
				m_coordMin.x = (area + i)->x;
			else if((area + i)->x > m_coordMax.x)
				m_coordMax.x = (area + i)->x;
			if((area + i)->y < m_coordMin.y)
				m_coordMin.y = (area + i)->y;
			else if((area + i)->y > m_coordMax.y)
				m_coordMax.y = (area + i)->y;
		}
	}
}

void CGdsView::CalcStructureFrame(CCoord* rect, CGDSStructure* structure, CCoord& pos, 
						   BOOL reflect, double& angle, double& mag)
{
	double scale = mag * m_dblDrawScale;
	CCoord coord;
	structure->GetDimMin(&coord);
	coord.Mul(scale);
	(rect + 0)->x = (rect + 1)->x = coord.x;
	(rect + 1)->y = (rect + 2)->y = coord.y;

	structure->GetDimMax(&coord);
	coord.Mul(scale);
	(rect + 2)->x = (rect + 3)->x = coord.x;
	(rect + 0)->y = (rect + 3)->y = coord.y;

	if(reflect){
		for(int i = 0;i < 4;i++)
			(rect + i)->y = -(rect + i)->y;
	}
	RotatePoints(rect, angle, 4);

	coord = pos;
	coord.Mul(m_dblDrawScale);
	for(int i = 0;i < 4;i++)
		(rect + i)->Offset(coord);

	if(m_intMirror > 0){
		for(int i = 0;i < 4;i++)
			(rect + i)->y = -(rect + i)->y;
	}
}



void CGdsView::CalcStructureArea(CCoord* rect, CGDSStructure* structure, CCoord& pos, 
						   BOOL reflect, double& angle, double& mag)
{
	if(!structure)
		return;

	static CCoord rect4[4];
	static double minx, maxx, miny, maxy;

	structure->GetDimMin(rect4 + 1);
	structure->GetDimMax(rect4 + 3);
	(rect4 + 1)->Mul(mag);
	(rect4 + 3)->Mul(mag);
	if(reflect){
		(rect4 + 1)->y = -(rect4 + 1)->y;
		(rect4 + 3)->y = -(rect4 + 3)->y;
	}
	(rect4 + 0)->x = (rect4 + 1)->x;
	(rect4 + 0)->y = (rect4 + 3)->y;
	(rect4 + 2)->x = (rect4 + 3)->x;
	(rect4 + 2)->y = (rect4 + 1)->y;

	RotatePoints(rect4, angle, 4);

	for(int i = 0;i < 4;i++){
		if(i == 0){
			minx = maxx = (rect4 + i)->x;
			miny = maxy = (rect4 + i)->y;
		}
		else{
			if((rect4 + i)->x < minx)
				minx = (rect4 + i)->x;
			else if((rect4 + i)->x > maxx)
				maxx = (rect4 + i)->x;
			if((rect4 + i)->y < miny)
				miny = (rect4 + i)->y;
			else if((rect4 + i)->y > maxy)
				maxy = (rect4 + i)->y;
		}
	}
	(rect + 0)->SetCoord(&minx, &maxy);
	(rect + 1)->SetCoord(&maxx, &miny);
	if(m_intMirror == 0){
		(rect + 0)->Offset(pos.x, pos.y);
		(rect + 1)->Offset(pos.x, pos.y);
	}
	else{
		double d = -rect->y;
		rect->y = -(rect + 1)->y;
		(rect + 1)->y = d;

		(rect + 0)->Offset(pos.x, -pos.y);
		(rect + 1)->Offset(pos.x, -pos.y);
	}
}

static int sort_skelton(const void *arg1, const void *arg2 )
{
	CStrFrame* cmp1 = *(CStrFrame**)arg1;
	CStrFrame* cmp2 = *(CStrFrame**)arg2;

    return cmp1->compare_size(cmp2);
}

void CGdsView::SortSkelton()
{
	CWaitCursor cursor;

	CObject** p = m_arrayHitTest.GetData();
	size_t len = m_arrayHitTest.GetSize();
	size_t spacing = sizeof(CObject**);

	qsort(p, len, spacing, sort_skelton);
}

void CGdsView::AddSkeltonStructure(CGDSStructure* structure, double angle, CCoord* area, BOOL isSref, CGDSElement* element)
{
	CCoord* min = area + 0;
	CCoord* max = area + 1;

	CStrFrame* pFrame = new CStrFrame;
	pFrame->SetAngle(angle);
	pFrame->SetArea(*min, *max);
	pFrame->SetStructure(structure);
	pFrame->SetAref(!isSref);
	pFrame->SetElement(element);
	m_arrayHitTest.Add(pFrame);
}

void CGdsView::DrawSkelton(CDC* pDC, BOOL redraw_structure)
{
	for(int i = 0;i < m_arrayHitTest.GetSize();i++){
		if(m_bAbortDraw || !::IsWindow(this->m_hWnd))
			break;
		CStrFrame* pFrame = (CStrFrame*)m_arrayHitTest.GetAt(i);
		CRect rect;
		CalcStructureArea(pFrame, rect);
		if(IsOutOfView(pDC, rect))
			continue;
		DrawSkeltonStructure(pDC, pFrame, 0);
		if(pFrame->GetExpand() && redraw_structure)
			DrawSingleStructure(pFrame);
	}
	DrawSkeltonStructure(pDC, m_pFrameActive, 1);
	DrawSkeltonStructure(pDC, m_pFrameTemp, -1);
}

void CGdsView::CalcStructureArea(CStrFrame* pFrame, CRect& rect)
{
	static CCoord pArea[2];
	pFrame->GetArea(pArea);
	(pArea + 0)->Mul(m_dblDrawScale);
	(pArea + 1)->Mul(m_dblDrawScale);

	long x = (long)(m_coordViewOffset.x * m_dblDrawScale);
	long y = (long)(m_coordViewOffset.y * m_dblDrawScale);
	rect.left   = x + (long)(pArea + 0)->x;
	rect.top    = y + (long)(pArea + 0)->y;
	rect.right  = x + (long)(pArea + 1)->x;
	rect.bottom = y + (long)(pArea + 1)->y; 

	rect.NormalizeRect();
}


CStrFrame* CGdsView::HitTestStructure(CDC* pDC, CPoint point, BOOL select)
{
	CRect rect;
	rect.TopLeft() = rect.BottomRight() = point;
	return HitTestStructure(pDC, rect, select, FALSE);
}

void CGdsView::DeleteDottedFrame(CDC *pDC)
{
	if(m_pFrameTemp)
		DrawSkeltonStructure(pDC, m_pFrameTemp, 0);
	if(m_bStructureInfoDlg)
		m_pDlgStructureInfo->ClearText();
	m_pFrameTemp = NULL;
}

CStrFrame* CGdsView::HitTestStructure(CDC *pDC, CRect rect, BOOL select, BOOL area)
{
	pDC->DPtoLP(&rect);

	CStrFrame* ret = NULL;

	int size = m_arrayHitTest.GetSize();
	for(int i = 0;i < size;i++){
		CStrFrame* pframe = (CStrFrame*)m_arrayHitTest.GetAt(area ? size - i - 1 : i);
		CRect frect;
		CalcStructureArea(pframe, frect);
		BOOL match = FALSE;
		if(!area)
			match = frect.PtInRect(rect.TopLeft());
		else{
			rect.NormalizeRect();
			match = (frect & rect) == frect ? TRUE : FALSE;
		}
		if(match){
			if(pframe->IsSref())
				m_strToolTip = pframe->GetStructure()->GetStructureName();
			else
				m_strToolTip = CString("Array of ") + pframe->GetStructure()->GetStructureName();
			ret = pframe;
			break;
		}
	}
	if(m_bSelectAllCell)
		return NULL;

	if(select){
		if(ret){
			if(m_pFrameActive != ret){
				if(m_pFrameActive){
					if(m_pFrameActive == m_pFrameTemp)
						DrawSkeltonStructure(pDC, m_pFrameActive, -1);
					else
						DrawSkeltonStructure(pDC, m_pFrameActive, 0);
				}
				DrawSkeltonStructure(pDC, ret, 1);
				if(!area){
					DrawSkeltonStructure(pDC, ret, -1);
					m_pFrameTemp = m_pFrameActive = ret;
				}
				else
					m_pFrameTemp = NULL;
				if(m_bStructureInfoDlg){
					m_pFrameActive = ret;
					m_pDlgStructureInfo->SetInformation();
				}
			}
		}
		else{
			if(m_pFrameActive){
				if(m_pFrameActive == m_pFrameTemp)
					DrawSkeltonStructure(pDC, m_pFrameActive, -1);
				else
					DrawSkeltonStructure(pDC, m_pFrameActive, 0);
			}
			if(m_bStructureInfoDlg)
				m_pDlgStructureInfo->ClearText();
			m_pFrameActive = NULL;
			m_strToolTip = "";
		}
		m_pFrameTemp = m_pFrameActive = ret;
	}
	else{
		if(ret){
			if(m_pFrameTemp != ret){
				if(m_pFrameTemp){
					if(m_pFrameTemp == m_pFrameActive)
						DrawSkeltonStructure(pDC, m_pFrameTemp, 1);
					else
						DrawSkeltonStructure(pDC, m_pFrameTemp, 0);
				}
				DrawSkeltonStructure(pDC, ret, -1);
				m_pFrameTemp = ret;
			}
		}
		else{
			if(m_pFrameTemp){
				if(m_pFrameTemp == m_pFrameActive)
					DrawSkeltonStructure(pDC, m_pFrameTemp, 1);
				else
					DrawSkeltonStructure(pDC, m_pFrameTemp, 0);
			}
			m_pFrameTemp = NULL;
		}
		m_pFrameTemp = ret;
	}
	return ret;
}

void CGdsView::DrawSkeltonStructure(CDC* pDC, CStrFrame* pFrame, int level)
{
	if(!m_bSkelton || !pFrame)
		return;

	CRect rect;
	CalcStructureArea(pFrame, rect);
	if(IsOutOfView(pDC, rect))
		return;

	DeleteCursor();

	double angle = pFrame->GetAngle();
	int penStyle = PS_SOLID;

	COLORREF color = COLOR_BOUNDARY_FRAME;
	switch(level){
	case 0:
		color = COLOR_BOUNDARY_FRAME;
		break;
	case 1:
		color = COLOR_HLT_STRUCTURE;
		break;
	case -1:
		color = COLOR_RED;
		penStyle = PS_DASH;
		break;
	default:
		break;
	}

	if(m_bSelectAllCell)
		color = COLOR_HLT_STRUCTURE;

	if(rect.Width() >= m_intDrawLimitL && rect.Height() >= m_intDrawLimitL){
		Graphics *pGraphics, graphics(pDC->m_hDC);

		if(m_intDrawMode != DRAW_MODE_DISPLAY)
			pGraphics = m_pGraphics;
		else{
			pGraphics = &graphics;
			pGraphics->SetTextRenderingHint(TextRenderingHintSingleBitPerPixel);
		}

		rect.NormalizeRect();
		Color col;
		col.SetFromCOLORREF(color);
		Pen pen(col, m_realPenWidth);
		REAL pat[2];
		if(penStyle == PS_DASH){
			pat[0] = pat[1] = m_realScaleBmpX * 8.0f;
			pen.SetDashStyle(DashStyleDash);
			pen.SetDashPattern(pat, 2);
		}
		pGraphics->DrawRectangle(&pen, rect.left, rect.top, rect.Width(), rect.Height());

		long font_h;
		if(m_intDrawMode != DRAW_MODE_PRINT){
			CRect client;
			GetClientRect(&client);
			pDC->DPtoLP(&client);
			font_h = (int)(abs(client.Height()) * SCALE_TEXT_RATIO);
		}
		else{
			SIZE sz = GetTotalSize();
			font_h = (int)(sz.cy * SCALE_TEXT_RATIO);
		}

		long w = abs(rect.Width());
		long h = abs(rect.Height());
		if(w < m_intDPInvisibleLP || h < m_intDPInvisibleLP)
			return;
		if(w >= h)
			angle = 0.0;
		else
			angle = 90.0;
		CString str = pFrame->GetStructure()->GetStructureName();
		if(!pFrame->IsSref())
			str += " *";

		USES_CONVERSION;
		LPTSTR lpsz = new TCHAR[str.GetLength() + 1];
		_tcscpy(lpsz, str);
		WCHAR* pwstr = A2W(lpsz);
		delete [] lpsz;


		long height = min(font_h, max(w, h) / str.GetLength());
		Color bcolor;
		bcolor.SetFromCOLORREF(color);
		SolidBrush brush(bcolor);

		FontFamily fontFamily(m_pwcharFontName);//L"Courier New");
		Font font(&fontFamily, (REAL)height, FontStyleBold, UnitPixel);
		StringFormat format;

		fmod360(angle);
		long x, y;
		rect.NormalizeRect();
		if(angle == 0.0){
			x = rect.CenterPoint().x;
			y = rect.top + height;
		}
		else if(angle == 90.0){
			x = rect.left;
			y = rect.CenterPoint().y;
			format.SetFormatFlags(StringFormatFlagsDirectionVertical);
		}
		Matrix matrix;
		pGraphics->GetTransform(&matrix);
		format.SetAlignment(StringAlignmentCenter);
		pGraphics->ScaleTransform(1.0f, -1.0f);
		PointF pos((REAL)x, (REAL)-y);
		pGraphics->DrawString(pwstr, str.GetLength(), &font, pos, &format, &brush); 
		pGraphics->SetTransform(&matrix);
	}
	else
		DrawDummySkelton(pDC, &rect, color);
	DrawCursor();
}

int CGdsView::DrawDummyElement(CGDSElement *element, 
							   CCoord& pos, BOOL reflect, double& angle, double& mag)
{
	double szmax = mag * ((CGDSXY*)element)->GetSize();

	if(szmax > m_intDrawLimitL)
		return ELEMENT_TO_BE_DRAWN;

	if(szmax <= m_intDPoneLP){
		if(m_bSpotSmall){
			CCoord ecoord;
			((CGDSXY*)element)->GetPos(&ecoord); 
			ecoord.Mul(mag);
			if(reflect)
				ecoord.y = -ecoord.y;
			RotatePoint(&ecoord, angle);
			ecoord += pos;
			if(m_intMirror > 0) 
				ecoord.y = -ecoord.y;
			m_pDC->SetPixel((long)ecoord.x, (long)ecoord.y, m_colorLayerRGB);
			return ELEMENT_DUMMY;
		}
		return ELEMENT_OMITTED;
	}


	CCoord ecoord;
	((CGDSXY*)element)->GetPos(&ecoord);
	ecoord.Mul(mag);
	if(reflect)
		ecoord.y = -ecoord.y;
	RotatePoint(&ecoord, angle);
	ecoord += pos;
	if(m_intMirror > 0) 
		ecoord.y = -ecoord.y;

	int szoffset = (int)szmax / 2;
	CRect rect((long)ecoord.x - szoffset, (long)ecoord.y + szoffset, (long)ecoord.x + szoffset, (long)ecoord.y - szoffset);
	// This instruction is most time consuming
	m_pDC->FillSolidRect(&rect, m_colorLayerRGB);
	return ELEMENT_DUMMY;
}


BOOL CGdsView::DrawGDSBoundary(CGDSBoundary *element, 
			   CCoord pos, BOOL reflect, double angle, double mag)
{
	unsigned numP =	((CGDSXY*)element)->GetNumCoords();
	if(numP == 0)
		return FALSE;
 
	pos.Mul(m_dblDrawScale);
	mag *= m_dblDrawScale;

	if(m_intDrawMode == DRAW_MODE_DISPLAY){
		switch(DrawDummyElement(element, pos, reflect, angle, mag)){
		case ELEMENT_OMITTED:
			return FALSE;
			break;
		case ELEMENT_DUMMY:
			return TRUE;
			break;
		case ELEMENT_TO_BE_DRAWN:
		default:
			break;
		}
	}

	CiCoord *pCoord = ((CGDSXY*)element)->GetCoordArray();

	for(unsigned i = 0;i < numP;i++){
		(points + i)->X = (REAL)(mag * (pCoord + i)->x);
		if(reflect)
			(points + i)->Y = -(REAL)(mag * (pCoord + i)->y);
		else
			(points + i)->Y = (REAL)(mag * (pCoord + i)->y);
	}
	RotatePoints(points, angle, numP);

	CRect rect;
	long x, y;
	BOOL needClip = FALSE;
	for(i = 0;i < numP;i++){
		(points + i)->X += (REAL)pos.x;
		(points + i)->Y += (REAL)pos.y;
		if(m_intMirror > 0)
			(points + i)->Y = -(points + i)->Y;
		needClip |= NeedClip(points + i);
		x = (long)(points + i)->X;
		y = (long)(points + i)->Y;
		if(i == 0){
			rect.left = rect.right = x;
			rect.top = rect.bottom = y;
		}
		else{
			if(x < rect.left) 
				rect.left = x;
			else if(x > rect.right)
				rect.right = x;
			if(y < rect.bottom)
				rect.bottom = y;
			else if(y > rect.top)
				rect.top = y;
		}
	}

	if(m_intDrawMode == DRAW_MODE_DISPLAY && !needClip){
		if(rect.Width() < m_intDrawLimitL || -rect.Height() < m_intDrawLimitL){
			DrawRect(&rect);
			return TRUE;
		}
	}
	else if(m_intDrawMode == DRAW_MODE_CLIPBOARD && !m_bEverythingInView && !needClip){
		if(IsOutOfView(rect))
			return TRUE;
	}

	GraphicsPath path;
	path.StartFigure();
	path.AddPolygon(points, numP);
	path.CloseFigure();


	if(m_intDrawMode == DRAW_MODE_PRINT){
		if(m_realPenWidthLim > rect.Width() || m_realPenWidthLim > -rect.Height())
			m_pPenDraw->SetWidth(0.0);
		else
			m_pPenDraw->SetWidth(m_realPenWidth);
	}
	else
		m_pPenDraw->SetWidth(m_realPenWidth);
	m_pPenDraw->SetColor(m_colorPen);


	if(needClip || m_intDrawMode == DRAW_MODE_CLIPBOARD || m_bClipPolygon)
		ClipPath(&path);

	m_pGraphics->DrawPath(m_pPenDraw, &path);
	if(m_bFillEnable)
		FillPolygon(&path);

	return TRUE;
}


void CGdsView::DrawGDSText(CGDSText *element, 
						   CCoord pos, BOOL reflect, double angle, double mag)
{
	if(!m_bShowText)
		return;

	pos.Mul(m_dblDrawScale);

	CCoord coord;
	element->GetPos(&coord);
	coord.Mul(m_dblDrawScale);
	coord.Mul(mag);

	if(reflect)
		coord.y = -coord.y;
	RotatePoint(&coord, angle);
	coord += pos;
	if(m_intMirror > 0)
		coord.y = -coord.y;

	double height = m_dblTextHeight * 1e3 / GetGDShcy()->GetDBUnit(); 
	height *= m_dblDrawScale * mag;
	char* str = element->GetString();
	double width = strlen(str) * height;

	long shiftx = (long)(width/2);
	long shifty = (long)(height/2);
	if(m_intMirror > 0)
		shifty = -shifty;

	CRect rect((long)coord.x - shiftx, (long)coord.y + shifty, 
				(long)coord.x + shiftx, (long)coord.y - shifty);

	switch(element->GetVertJust()){
	case 0: // Top
		rect.top -= shifty;
		rect.bottom -= shifty;
		break;
	case 1: // Middle
		break;
	case 2: // Bottom
		rect.top += shifty;
		rect.bottom += shifty;
		break;
	default:
		break;
	}

	switch(element->GetHorzJust()){
	case 0: // Left
		rect.left += shiftx;
		rect.right += shiftx;
		break;
	case 1: // Center
		break;
	case 2: // Right
		rect.left -= shiftx;
		rect.right -= shiftx;
		break;
	default:
		break;
	}

	if(m_intMirror > 0)
		angle = -angle;

	if(IsOutOfView(rect))
		return;

	static int space[512];
	for(unsigned i = 0;i < strlen(str) - 1;i++)
		*(space + i) = (int)height;

	if(m_intMirror > 0)
		angle += 180.0;

	USES_CONVERSION;
	LPTSTR lpsz = new TCHAR[strlen(str) + 1];
	_tcscpy(lpsz, str);
	WCHAR* pwstr = A2W(lpsz);
	delete [] lpsz;


	SolidBrush brush(m_colorPen);

	FontFamily fontFamily(m_pwcharFontName);
	Font font(&fontFamily, (REAL)height, m_intFontStyle, UnitPixel);

	UINT descent = fontFamily.GetCellDescent(m_intFontStyle);
	REAL descentPixel = 
	   font.GetSize() * descent / fontFamily.GetEmHeight(m_intFontStyle);

	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);

	rect.NormalizeRect();
	RectF rectf((REAL)rect.left, (REAL)rect.top, 
				(REAL)rect.Width(), (REAL)rect.Height() + descentPixel);
	PointF center((REAL)rect.CenterPoint().x, (REAL)rect.CenterPoint().y);
	GraphicsState state = m_pGraphics->Save();
	m_pGraphics->TranslateTransform(center.X, center.Y, MatrixOrderPrepend);
	m_pGraphics->RotateTransform((REAL)angle, MatrixOrderPrepend);
	m_pGraphics->ScaleTransform(1.0f, -1.0f, MatrixOrderPrepend);
	m_pGraphics->TranslateTransform(-center.X, -center.Y, MatrixOrderPrepend);
	m_pGraphics->DrawString(pwstr, -1, &font, rectf, &format, &brush); 

	//Pen pen(m_colorLayer);
	//m_pGraphics->DrawRectangle(&pen, rectf);

	m_pGraphics->Restore(state);
	return;
}

/*
void CGdsView::DrawGDSNode(CGDSNode *element, 
						   CCoord pos, BOOL reflect, double angle, double mag)
{
	static BOOL warn = TRUE;
	if(warn){
		MessageBox("Warninig : Non-supported element NODE included.");
		warn = FALSE;
	}
	return;
}

void CGdsView::DrawGDSContact(CGDSNode *element, 
						   CCoord pos, BOOL reflect, double angle, double mag)
{
	static BOOL warn = TRUE;
	if(warn){
		MessageBox("Warninig : Non-supported element CONTACT included.");
		warn = FALSE;
	}
	return;
}*/

void CGdsView::OnKeyEscape()
{
	if(IsDrawing()){
		StopDraw();
		return;
	}

	switch(m_intEditMode){
	case EDIT_MODE_MEASURE:
		if(m_scale.IsMeasuring()){
			AbortMeasuring();
			return;
		}
		break;
	case EDIT_MODE_NONE:
	default:
		if(m_pFrameActive){
			CClientDC dc(this);
			OnPrepareDC(&dc);
			SetOrigin(&dc);
			DrawSkeltonStructure(&dc, m_pFrameActive, 0);
			DrawSkeltonStructure(&dc, m_pFrameTemp, 0);
		}
		m_pFrameActive = NULL;
		m_pFrameTemp = NULL;
		m_strToolTip = "";
		break;
	}
	ChangeEditMode(EDIT_MODE_NONE);
}

void CGdsView::OnZoomSwitchByKey() 
{
	if(m_intEditMode != EDIT_MODE_ZOOM)
		OnZoomSwitch();
}

void CGdsView::OnMemoSwitchByKey() 
{
	if(m_intEditMode != EDIT_MODE_MEMO)
		OnMemoSwitch();
}

void CGdsView::OnMeasureSwitchByKey() 
{
	if(m_intEditMode != EDIT_MODE_MEASURE)
		OnMeasureSwitch();
}

void CGdsView::OnDeleteAllRuler() 
{
	AbortMeasuring(TRUE);
}

void CGdsView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	m_bUpdateDrawTime = TRUE;
	switch(nChar){
	case VK_ESCAPE:
		OnKeyEscape();
		break;
	case VK_OEM_PLUS: //'+' any country
		GetAsyncKeyState(VK_CONTROL);
		GetAsyncKeyState(VK_SHIFT);
		if(GetAsyncKeyState(VK_SHIFT))
			OnZoomUp();
		if(GetAsyncKeyState(VK_CONTROL))
			OnIncreaseFontSize();

		break;
	case VK_OEM_MINUS: //'-' any country
		GetAsyncKeyState(VK_CONTROL);
		if(GetAsyncKeyState(VK_CONTROL))
			OnDecreaseFontSize();
		else if(!GetAsyncKeyState(VK_SHIFT))
			OnZoomDown();
		break;
	default:
		break;
	}
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CGdsView::UpdateLayer()
{
	if(!m_bDlgLayer)
		return;
	StopDraw();
	GetDocument()->SetTecFilePath(m_pDlgLayer->GetTecFilePath());
	m_dblHatchDensity = m_pDlgLayer->GetHatchDensity();
	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Hatch density", (int)m_dblHatchDensity);

	RecalcNumActiveLayer();
	InvalidateRect(TRUE, NULL);
}


void CGdsView::OnSetLayer() 
{
	if(m_bDlgLayer){
		m_pDlgLayer->ExitDialog();
		m_pDlgLayer = NULL;
		m_bDlgLayer = FALSE;
	}
	else{
		m_pDlgLayer = new CDlgLayer;
		m_pDlgLayer->SetEnableSwitch(&m_bDlgLayer);
		m_pDlgLayer->SetLayerAndColor(m_aryIntLayerOrder, m_aryLayerName, m_bShowList, m_aryColorLayerRGB, m_aryIntFillType, m_bLayerExist, m_bLayerInTec);
		m_pDlgLayer->SetHatchDensity(m_dblHatchDensity);
		m_pDlgLayer->SetTecFilePath(GetDocument()->GetTecFilePath());
		m_pDlgLayer->SetCaller(this);
		m_pDlgLayer->Create(IDD_LAYER_DLG, this);
		m_bDlgLayer = TRUE;
	}
}

void CGdsView::OnSelectStr() 
{
	if(!m_bStructureDialog){
		m_pDlgStructure = new CStructureDlg;
		m_pDlgStructure->SetCaller(this);
		m_pDlgStructure->SetEnableSwitch(&m_bStructureDialog);
		m_pDlgStructure->SetStructure(GetGDShcy());
		if(m_pStrActive)
			m_pDlgStructure->SetActive(m_pStrActive->GetStructureName());

		OnButtonStopDraw();
		m_pDlgStructure->Create(IDD_STRUCTURE_DLG, this);
		SetFocus();
		CString name = GetDocument()->GetPathName();
		int idx = name.ReverseFind('\\');
		if(idx > 0)
			name = name.Mid(++idx);
		m_pDlgStructure->SetWindowName(name);
		m_pDlgStructure->SetExpandLevel(m_intExpandLevel);

		m_bStructureDialog = TRUE;
	}
	else{
		m_pDlgStructure->ExitDialog();
		m_pDlgStructure = NULL;
		m_bStructureDialog = FALSE;
	}
}

#include "HatchDlg.h"
void CGdsView::OnSetHatchLimit() 
{
	CHatchDlg hatchdlg;
	hatchdlg.SetHatchLimit(m_intHatchLimitD);
	hatchdlg.SetDrawLimit(m_intDrawLimitD);
	hatchdlg.SetOmitStructure(m_bOmitSmallStructure);
	if(hatchdlg.DoModal() == IDCANCEL)
		return;
	m_intHatchLimitD = hatchdlg.GetHatchLimit();
	m_intDrawLimitD = hatchdlg.GetDrawLimit();
	m_bOmitSmallStructure = hatchdlg.GetOmitStructure();
	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Minimum Fill/Hatch", m_intHatchLimitD);
	app->WriteProfileInt("Misc", "Minimum Draw", m_intDrawLimitD);
	app->WriteProfileInt("Misc", "Omit small", m_bOmitSmallStructure ? 1 : 0);
	InvalidateRect(TRUE, NULL);
}

void CGdsView::OnUpdateSetHatchLimit(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}


BOOL CGdsView::PanTo(BOOL client, CPoint pos)
{
	DrawAxis(FALSE);
	DeleteCursor();
	LockWindowUpdate();
    m_toolTipCtrl.Pop();

	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);

	CRect rect;
	GetClientRect(&rect);

    dc.DPtoLP(&rect);
    CPoint point;
	CPoint org = dc.GetWindowOrg();

	if(client){
		point.x = m_rectView.left;
		point.y = m_rectView.top;
	}
	else
		point = pos;
	dc.DPtoLP(&point);
	point.x = point.x - (rect.Width())/2;
	point.y = point.y + (-rect.Height())/2;
    point -= org;

	//
	CSize sz = GetTotalSize();
	BOOL redraw = FALSE;
	double offsetx, offsety, adjust;
	offsetx = 0.0;
	offsety = 0.0;
	if(point.y >= 0){
        modf(point.y / m_dblDrawScale, &adjust);
		if(m_intMirror > 0)
			offsety = adjust;
		else
			offsety = -adjust;
		point.y = 0;
		redraw = TRUE;
	}
	else if(point.y - (-rect.Height()) <= -sz.cy){
        modf((point.y - (-rect.Height()) + sz.cy) / m_dblDrawScale, &adjust);
		if(m_intMirror > 0)
			offsety = adjust;
		else
			offsety = -adjust;
		point.y -= (long)(adjust * m_dblDrawScale);
		redraw = TRUE;
	}
	if(point.x <= 0){
        modf(point.x / m_dblDrawScale, &adjust);
		offsetx = -adjust;
		point.x = 0;
		redraw = TRUE;
	}
	else if(point.x + (rect.Width()) >= sz.cx){
        modf((point.x + (rect.Width()) - sz.cx) / m_dblDrawScale, &adjust);
		offsetx = -adjust;
		point.x -= (long)(adjust * m_dblDrawScale);
		redraw = TRUE;
	}
	if(redraw){
		InvalidateRect(TRUE, NULL);
		MoveViewOffset(offsetx, offsety);
		ScrollToPosition(point);
	}
	else{
		ScrollToPosition(point);
		DrawAxis();
		DrawCenterMark();
		DrawCursor();
	}
	return redraw;
}

void CGdsView::ZoomAreaCoord(CCoord center, CCoord size) 
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);	
	
	CRect rect;
	GetClientRect(&rect);
    dc.DPtoLP(&rect);


    CPoint point;
	point.x =  -(rect.Width())/2;
	point.y =  (-rect.Height())/2;
    point -= dc.GetWindowOrg();

	double left = center.x - size.x/2.0;
	double right = left + size.x;
	double top = center.y + size.y/2.0;
	double bottom = top - size.y;

    double scale = GetGDShcy()->GetDBUnit() * (1.0 - m_dblShrinkRate);

	left /= scale;
	top /= scale;
	right /= scale;
	bottom /= scale;
	double scalex = rect.Width()/(right - left);
	double scaley = rect.Height()/(-top + bottom);

	m_dblDrawScale = min(scalex, scaley);

	double x = (left + right) / 2.0;
	double y = (top + bottom) / 2.0;
	if(m_intMirror > 0)
		y = -y;
	m_coordViewOffset.x = -x;
	m_coordViewOffset.y = -y;

	x *= m_dblDrawScale;
	y *= m_dblDrawScale;



	//
	CSize sz = GetTotalSize();
	double offsetx, offsety, adjust;
	offsetx = 0.0;
	offsety = 0.0;
	if(y >= 0){
        modf(y / m_dblDrawScale, &adjust);
		if(m_intMirror > 0)
			offsety = adjust;
		else
			offsety = -adjust;
		y = 0;
	}
	else if(y - (-rect.Height()) <= -sz.cy){
        modf((y - (-rect.Height()) + sz.cy) / m_dblDrawScale, &adjust);
		if(m_intMirror > 0)
			offsety = adjust;
		else
			offsety = -adjust;
		y -= (long)(adjust * m_dblDrawScale);
	}
	if(x <= 0){
        modf(x / m_dblDrawScale, &adjust);
		offsetx = -adjust;
		x = 0;
	}
	else if(x + (rect.Width()) >= sz.cx){
        modf((x + (rect.Width()) - sz.cx) / m_dblDrawScale, &adjust);
		offsetx = -adjust;
		x -= (long)(adjust * m_dblDrawScale);
	}

	InvalidateRect(TRUE, NULL);
	ScrollToPosition(point);
}

void CGdsView::GetViewArea(double* left, double* right, double* top, double* bottom)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);	
	CRect rect;
	GetClientRect(&rect);
    dc.DPtoLP(&rect);
	
	LPtoMicron(rect.TopLeft(), left, top);
	LPtoMicron(rect.BottomRight(), right, bottom);
}

void CGdsView::OnZoomAreaCoord() 
{
	StopDraw();

	CZoomAreaDlg dlg;
	double left, right, top, bottom;
	GetViewArea(&left, &right, &top, &bottom);

	dlg.SetCurrentArea(left, top, right, bottom);
	dlg.SetZoomAreaMode(m_intZoomAreaMode);
	if(dlg.DoModal() == IDCANCEL)
		return;
	m_intZoomAreaMode = dlg.GetZoomAreaMode();
	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Zoom area mode", m_intZoomAreaMode);

	DrawAxis(FALSE);
	DeleteCursor();
	LockWindowUpdate();
    m_toolTipCtrl.Pop();

	GetViewArea();

	dlg.GetCoord(&left, &top, &right, &bottom);
	CCoord center((left + right)/2.0, (top + bottom)/2.0);
	CCoord size(right - left, top - bottom);
	ZoomAreaCoord(center, size);
}

void CGdsView::OnZoomArea()
{
	GetViewArea();

	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);
    double xscale, scalex, scaley;

	CRect rect;
	GetClientRect(&rect);
    dc.DPtoLP(&rect);
    CPoint point;

    dc.DPtoLP(&m_rectView);

	point.x = (long)((m_rectView.left + m_rectView.right)/2.0 + 0.5);
	point.y = (long)((m_rectView.top + m_rectView.bottom)/2.0 + 0.5);

	if(m_rectView.left > m_rectView.right && m_rectView.top < m_rectView.bottom){
		scalex = fabs((m_rectView.right - m_rectView.left)/((double)rect.Width()));
		scaley = fabs((m_rectView.top - m_rectView.bottom)/((double)-rect.Height()));
		xscale = max(scalex, scaley);
	}
	else{
		scalex = fabs(((double)rect.Width())/(m_rectView.right - m_rectView.left));
		scaley = fabs(((double)-rect.Height())/(m_rectView.top - m_rectView.bottom));
		xscale = min(scalex, scaley);
	}
    m_dblDrawScale *= xscale;
	m_rectView.left = (long)(xscale * point.x);
	m_rectView.top = (long)(xscale * point.y);
    dc.LPtoDP(&m_rectView);
    if(!PanTo())
		InvalidateRect(TRUE, NULL);
}

void CGdsView::OnZoom(int zoomUp)
{
	GetViewArea();

	m_bUpdateDrawTime = TRUE;
	
   	SIZE sz;
	sz = GetTotalSize();

    double xscale;
    switch(zoomUp){
	case 1:
	    xscale = 1.5;
		m_dblDrawScale *= xscale;
		break;
    case 0:
		xscale = 1.0/1.5;
		m_dblDrawScale *= xscale;
		break;
	default:
		xscale = 1.0;
		break;
	}
	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);

	CRect rect;
	GetClientRect(&rect);
    dc.DPtoLP(&rect);
    
    // Get center coordinate
    m_rectView.left = (rect.left + rect.right)/2;  
    m_rectView.top = (rect.top + rect.bottom)/2;

    m_rectView.left = (long)(xscale * m_rectView.left);
    m_rectView.top = (long)(xscale * m_rectView.top);

    dc.LPtoDP(&m_rectView);
    if(!PanTo())
		InvalidateRect(TRUE, NULL);
    return;
}

void CGdsView::OnZoomUp() 
{
    OnZoom(1);
}

void CGdsView::OnZoomDown() 
{
    OnZoom(0);
}


void CGdsView::DeleteCursor()
{
	if(!m_bCursorState)
		return;

    DrawCursor(FALSE);
	m_bCursorState = FALSE;
}

void CGdsView::DrawCursor(BOOL draw)
{
	if(m_bInitView || !::IsWindow(this->m_hWnd))
		return;
	if(draw && m_bCancelDraw)
		return;
	if(m_intCursorType != CURSOR_CROSS_WINDOW && 
		m_intCursorType != CURSOR_PLUS_WINDOW)
		return;

	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	CClientDC dc(this);
	OnPrepareDC(&dc);
	SetOrigin(&dc);

    CRect rect;
    GetClientRect(&rect);
    CPen pen, *pOldPen;
	COLORREF color = ReverseGray(COLOR_CURSOR);
	pen.CreatePen(PS_SOLID, 0, color);
	pOldPen = (CPen*)dc.SelectObject(&pen);

	if(draw && !rect.PtInRect(point))
		return;
    CPoint pointfm, pointto;

    int rop = dc.GetROP2();
	dc.SetROP2(R2_XORPEN);

    if(m_bCursorState){
        // Delete old line
	    pointfm.x = pointto.x = m_pCursor.x;
        pointfm.y = rect.top;
        pointto.y = rect.bottom;
        dc.DPtoLP(&pointfm);
        dc.DPtoLP(&pointto);
        dc.MoveTo(pointfm);
        dc.LineTo(pointto);

        pointfm.x = rect.left;
	    pointfm.y = pointto.y = m_pCursor.y;
        pointto.x = rect.right;

        dc.DPtoLP(&pointfm);
        dc.DPtoLP(&pointto);
        dc.MoveTo(pointfm);
        dc.LineTo(pointto);
		m_bCursorState = FALSE;
    }
	if(draw && m_bMouseInView){
		// Write new line
		pointfm.x = pointto.x = point.x;
		pointfm.y = rect.top;
		pointto.y = rect.bottom;
		dc.DPtoLP(&pointfm);
		dc.DPtoLP(&pointto);
		dc.MoveTo(pointfm);
		dc.LineTo(pointto);

		pointfm.x = rect.left;
		pointfm.y = pointto.y = point.y;
		pointto.x = rect.right;

		dc.DPtoLP(&pointfm);
		dc.DPtoLP(&pointto);
		dc.MoveTo(pointfm);
		dc.LineTo(pointto);
        m_bCursorState = TRUE;
	}
    dc.SetROP2(rop);
    dc.SelectObject(pOldPen);
	pen.DeleteObject();
    m_pCursor = point;
}

BOOL CGdsView::IsGridVisible()
{
	CRect recttest;
	recttest.left = 0;
	recttest.right = (long)(m_pGrid->GetMinorX() * m_dblDrawScale);
	recttest.top = (long)(m_pGrid->GetMinorY() * m_dblDrawScale);
	recttest.bottom = 0;

	CClientDC dc(this);
	OnPrepareDC(&dc);
	SetOrigin(&dc);
	dc.LPtoDP(&recttest);
	if(recttest.Width() < GRID_LIMIT || recttest.Height() < GRID_LIMIT)
		return FALSE;
	return TRUE;
}

void CGdsView::DrawGrid(CDC* pDC)
{
	if(m_pGrid->GetGridType() == GRID_TYPE_NONE || 
		(m_intDrawMode == DRAW_MODE_PRINT && m_intPrintMode != PRINT_MODE_WYSIWYG))
		return;

	if(!IsGridVisible())
		return;

	CRect rect;
	GetClientRect(&rect);
	pDC->DPtoLP(&rect);

	if(m_intDrawMode == DRAW_MODE_PRINT){
		rect = m_rectClip;
		long tmp = rect.top;
		rect.top = rect.bottom;
		rect.bottom = tmp;
	}
	REAL top = (REAL)rect.top;
	REAL left = (REAL)rect.left;
	REAL bottom = (REAL)rect.bottom;
	REAL right = (REAL)rect.right;

	LPtoDBUnit(&left, &top);
	LPtoDBUnit(&right, &bottom);
	if(left > right){
		REAL t = left;
		left = right;
		right = t;
	}
	if(bottom > top){
		REAL t = top;
		top = bottom;
		bottom = t;
	}

	REAL ystep = (REAL)m_pGrid->GetMinorY();
	REAL xstep = (REAL)m_pGrid->GetMinorX();
	rect.SetRect(0, 0, GRID_SIZE, GRID_SIZE);


	REAL width, height;
	if(m_intDrawMode == DRAW_MODE_PRINT){
		width = (REAL)(GRID_SIZE * m_realScaleBmpX);
		height = (REAL)(GRID_SIZE * m_realScaleBmpY);
	}
	else{
		pDC->DPtoLP(&rect);
		height = (REAL)abs(rect.Height());
		width = (REAL)abs(rect.Width());
	}

	Graphics* pg;
	Graphics g(pDC->m_hDC);

	if(m_intDrawMode != DRAW_MODE_DISPLAY)
		pg = m_pGraphics;
	else
		pg = &g;

    Color color_h;
    Color color_l;
    Color color_d;
    if(m_colorBK == COLOR_WHITE || m_intDrawMode == DRAW_MODE_PRINT){
        BYTE r, g, b;
        r = 255 - GetRValue(COLOR_GRID_HIGHLIGHT);
        g = 255 - GetGValue(COLOR_GRID_HIGHLIGHT);
        b = 255 - GetBValue(COLOR_GRID_HIGHLIGHT);
        ARGB argb =  Color::MakeARGB(255, r, g, b);
        color_h.SetValue(argb);
        r = 255 - GetRValue(COLOR_GRID_LINE);
        g = 255 - GetGValue(COLOR_GRID_LINE);
        b = 255 - GetBValue(COLOR_GRID_LINE);
        argb =  Color::MakeARGB(255, r, g, b);
        color_l.SetValue(argb);
        r = 255 - GetRValue(COLOR_GRID_DOT);
        g = 255 - GetGValue(COLOR_GRID_DOT);
        b = 255 - GetBValue(COLOR_GRID_DOT);
        argb =  Color::MakeARGB(255, r, g, b);
        color_d.SetValue(argb);
    }
    else{
        color_h.SetFromCOLORREF(COLOR_GRID_HIGHLIGHT);
        color_l.SetFromCOLORREF(COLOR_GRID_LINE);
        color_d.SetFromCOLORREF(COLOR_GRID_DOT);
    }
    Pen pen_h(color_h, m_realPenWidth);
    Pen pen(color_l, m_realPenWidth);
    SolidBrush brush(color_d);

	DeleteCursor();
	PointF start, stop;

	REAL x, y;
	left += (REAL)(m_coordViewOffset.x - xstep * 2);
	right += (REAL)(m_coordViewOffset.x + xstep * 2);
	top += (REAL)(m_coordViewOffset.y + ystep * 2);
	bottom += (REAL)(m_coordViewOffset.y - ystep * 2);

    if(m_pGrid->GetGridType() == GRID_TYPE_LINE){
		start.X = (REAL)(left * m_dblDrawScale);
		stop.X = (REAL)(right * m_dblDrawScale);
		for(y = bottom;y <= top;y += ystep){
			start.Y = (REAL)(y * m_dblDrawScale);
			stop.Y = start.Y;
            pg->DrawLine(&pen, start, stop);
		}
		start.Y = (REAL)(top * m_dblDrawScale);
		stop.Y = (REAL)(bottom * m_dblDrawScale);
		for(x = left;x <= right;x += xstep){
			start.X = (REAL)(x * m_dblDrawScale);
			stop.X = start.X;
            pg->DrawLine(&pen, start, stop);
		}
    }
	int yidx = 0;
	int xidx = 0;

	for(y = bottom;y <= top;y += ystep, yidx++){
		if(m_bAbortDraw)
			break;
		BOOL hlty = FALSE;
		double iptr;
		double d = modf((y - m_coordViewOffset.y)/ystep, &iptr);
		if(d >= 0.5)
			iptr += 1.0;
		else if(d <= -0.5)
			iptr -= 1.0;
		if((int)iptr % m_pGrid->GetMajorY() == 0)
			hlty = TRUE;

		for(x = left, xidx = 0;x <= right;x += xstep, xidx++){
            start.X = stop.X = (REAL)(x * m_dblDrawScale);
            start.Y = stop.Y = (REAL)(y * m_dblDrawScale);
			BOOL hltx = FALSE;
			double iptr;
			double d = modf((x - m_coordViewOffset.x)/xstep, &iptr);
			if(d >= 0.5)
				iptr += 1.0;
			else if(d <= -0.5)
				iptr -= 1.0;
			if((int)iptr % m_pGrid->GetMajorX() == 0)
				hltx = TRUE;

			if(hltx && hlty){
				REAL w = (REAL)(width * 2);
				start.X -= w;
				stop.X += w;
                pg->DrawLine(&pen_h, start, stop);
				start.X += w;
				stop.X -= w;

				start.Y -= w;
				stop.Y += w;
                pg->DrawLine(&pen_h, start, stop);
			}
			else if(m_pGrid->GetGridType() == GRID_TYPE_DOT){
				start.X -= width/2;
				start.Y -= height/2;
				pg->FillRectangle(&brush, start.X, start.Y, width, height);
			}
		}
	}
}

void CGdsView::DrawAxis(BOOL draw)
{
	if(m_intAxisAt == AXIS_AT_NONE)
		return;
	if(m_intDrawMode != DRAW_MODE_DISPLAY){
		if(draw && m_bAxisState)
			return;
		if(!draw && !m_bAxisState)
			return;
	}

	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);


    CRect wrect;
    GetClientRect(&wrect);
	dc.DPtoLP(&wrect);
	if(m_intDrawMode != DRAW_MODE_DISPLAY)
		wrect = m_rectClip;


	CPoint pcenter;
	pcenter.x = (long)(m_coordViewOffset.x * m_dblDrawScale);
	pcenter.y = (long)(m_coordViewOffset.y * m_dblDrawScale);

	if(m_intMirror > 0)
		pcenter.y = -pcenter.y;

	if(m_intAxisAt == AXIS_AT_WINDOW_CENTER){
		pcenter.x = wrect.CenterPoint().x;
		pcenter.y = wrect.CenterPoint().y;
	}

    CPen pen, *pOldPen;
	COLORREF color = COLOR_AXIS;
	if(m_colorBK != COLOR_BLACK)
		color = COLOR_WHITE - COLOR_AXIS;
	pen.CreatePen(PS_SOLID, 0, color);
	pOldPen = (CPen*)dc.SelectObject(&pen);

    CPoint pfm1, pfm2, pto1, pto2;

    int rop = dc.GetROP2();
	dc.SetROP2(R2_XORPEN);//GetROP(&dc));

	pfm1.x = pto1.x = pcenter.x;
    pfm1.y = wrect.top;
    pto1.y = wrect.bottom;

    pfm2.x = wrect.left;
	pfm2.y = pto2.y = pcenter.y;
    pto2.x = wrect.right;

	if(m_intDrawMode != DRAW_MODE_DISPLAY){
		if(m_intDrawMode == DRAW_MODE_PRINT)
			color = COLOR_BLACK;
		else if(m_colorBK == COLOR_BLACK)
			color = COLOR_WHITE;
		else
			color = COLOR_BLACK;

		Point p1(pfm1.x, pfm1.y);
		Point p2(pto1.x, pto1.y);
		Color pcolor;
		pcolor.SetFromCOLORREF(color);
		Pen gpen(pcolor, m_realPenWidth);
		m_pGraphics->DrawLine(&gpen, p1, p2);
		p1.X = pfm2.x;
		p1.Y = pfm2.y;
		p2.X = pto2.x;
		p2.Y = pto2.y;
		m_pGraphics->DrawLine(&gpen, p1, p2);
	}
	else{
		if(m_bAxisState){//delete previous
			dc.MoveTo(pfm1);
			dc.LineTo(pto1);
			dc.MoveTo(pfm2);
			dc.LineTo(pto2);
			m_bAxisState = FALSE;
		}
		else{
			dc.MoveTo(pfm1);
			dc.LineTo(pto1);
			dc.MoveTo(pfm2);
			dc.LineTo(pto2);
			m_bAxisState = TRUE;
		}
	}
    dc.SetROP2(rop);
    dc.SelectObject(pOldPen);
	pen.DeleteObject();
}

BOOL CGdsView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
    if(nHitTest == HTCLIENT){
		switch(m_intCursorType){
		case CURSOR_CROSS_WINDOW:
	        SetCursor(NULL);
			break;
		case CURSOR_CROSS:
			SetCursor(m_cCross);
			break;
		case CURSOR_STANDARD:
		case CURSOR_PLUS_WINDOW:
			SetCursor(m_cStandardCursor);
			break;
		}
        return TRUE;
    }
	else{
		DisableStatusBarDisplay();
	}
	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}


void CGdsView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	if(bActivate && pActivateView == this){
		UnlockWindowUpdate();
		DrawComboBox(TRUE);
	}
	else if(!bActivate && pDeactiveView == this)
		DrawComboBox(FALSE);
	if(GetForegroundWindow() == m_pMainFrame)
		DisableStatusBarDisplay();
	CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if(m_bForceRedraw)
		OnButtonStopDraw();
}

void CGdsView::DisableStatusBarDisplay(BOOL drwtime)
{
    m_pMainFrame->SetStatusBarXY(0, 0, FALSE);
	if(drwtime)
		m_pMainFrame->SetStatusBarDrawTime(-1);
}


BOOL CGdsView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll) 
{
	if(m_pThreadRender && !m_pThreadRender->WaitUntilDrawn())
		return FALSE;
	DrawAxis(FALSE);
	DeleteCursor();
	BOOL ret = CScrollView::OnScrollBy(sizeScroll, bDoScroll);
    if(!ret){
		DrawAxis();
		DrawCursor();
	}
	CPoint pos = GetScrollPosition();
	ScrollToPosition(pos);
    return ret;
}

void CGdsView::OnScrollLeft() 
{
	SCROLLINFO scroll;
	GetScrollInfo(SB_VERT, &scroll);
	OnScroll(SB_THUMBTRACK << 8 | SB_LINEUP, scroll.nPos, TRUE); 
}

void CGdsView::OnScrollRight() 
{
	SCROLLINFO scroll;
	GetScrollInfo(SB_VERT, &scroll);
	OnScroll(SB_THUMBTRACK << 8 | SB_LINEDOWN, scroll.nPos, TRUE); 
}

void CGdsView::OnScrollDown() 
{
	SCROLLINFO scroll;
	GetScrollInfo(SB_HORZ, &scroll);
	OnScroll(SB_THUMBTRACK | SB_LINEDOWN << 8, scroll.nPos, TRUE); 
}


void CGdsView::OnScrollUp() 
{
	SCROLLINFO scroll;
	GetScrollInfo(SB_HORZ, &scroll);
	OnScroll(SB_THUMBTRACK | SB_LINEUP << 8, scroll.nPos, TRUE); 
}


BOOL CGdsView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	BOOL ret = CScrollView::OnMouseWheel(nFlags, zDelta, pt);
	if(!ret){
		UINT code = SB_THUMBTRACK;
		if(zDelta < 0)
			code |= SB_LINEDOWN << 8;
		else
			code |= SB_LINEUP << 8;
		OnScroll(code, 0, TRUE);
	}
	return ret;
}

BOOL CGdsView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll) 
{
	m_bUpdateDrawTime = TRUE;

	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);

	SIZE total, page, step;
	int mapmode;
	GetDeviceScrollSizes(mapmode,  total, page, step);

	CRect rect(0, 0, step.cx, step.cy);
	dc.DPtoLP(&rect);
	step.cx = (long)(rect.Width() / m_dblDrawScale);
	step.cy = (long)(rect.Height() / m_dblDrawScale);

	BOOL redraw = FALSE;
	SCROLLINFO info;
	UINT horz = nScrollCode & 0xff;
	if(horz != SB_THUMBTRACK){
		GetScrollInfo(SB_HORZ, &info);
		switch(horz){
		case SB_LINERIGHT:
			if(info.nPage + info.nPos >= (UINT)info.nMax){
				m_coordViewOffset.x -= step.cx;
				redraw = TRUE;
			}
			break;
		case SB_LINELEFT:
			if(info.nPos <= info.nMin){
				m_coordViewOffset.x += step.cx;
				redraw = TRUE;
			}
			break;
		default:
			break;
		}
	}
	UINT vert = (nScrollCode & 0xff00) >> 8;
	if(vert != SB_THUMBTRACK){
		GetScrollInfo(SB_VERT, &info);
		switch(vert){
		case SB_LINEDOWN:
			if(info.nPage + info.nPos >= (UINT)info.nMax){
				m_coordViewOffset.y -= step.cy;
				redraw = TRUE;
			}
			break;
		case SB_LINEUP:
			if(info.nPos <= info.nMin){
				m_coordViewOffset.y += step.cy;
				redraw = TRUE;
			}
			break;
		default:
			break;
		}
	}
	CPoint coord;
	GetCursorPos(&coord);
	ScreenToClient(&coord);
    UpdateXYCoord(coord);

	if(redraw){
		InvalidateRect(TRUE, NULL);
		return FALSE;
	}
	else
		return CScrollView::OnScroll(nScrollCode, nPos, bDoScroll);
}

void CGdsView::LPtoDBUnit(REAL *x, REAL *y)
{
	double yoffset = m_coordViewOffset.y;
	if(m_intMirror > 0)
		yoffset = -yoffset;

	double dx = (double)(*x / m_dblDrawScale - m_coordViewOffset.x);
	double dy = (double)(*y / m_dblDrawScale - yoffset);

	RoundToUserGrid(&dx, &dy, FALSE);
	*x = (REAL)dx;
	*y = (REAL)dy;
}

void CGdsView::LPtoMicron(CPoint point, double *x, double *y)
{
	double yoffset = m_coordViewOffset.y;
	if(m_intMirror > 0)
		yoffset = -yoffset;

    *x = (point.x / m_dblDrawScale - m_coordViewOffset.x); 
    *y = (point.y / m_dblDrawScale - yoffset);

	RoundToUserGrid(x, y);
    *x *= GetGDShcy()->GetDBUnit() * (1.0 - m_dblShrinkRate);
    *y *= GetGDShcy()->GetDBUnit() * (1.0 - m_dblShrinkRate);


	*x *= 1e+6; // convert to um
	*y *= 1e+6;
}

void CGdsView::UpdateXYCoord(CPoint cursor, CDC* pDC)
{
	if(m_bInitView)
		return;

	if(!pDC){
		CClientDC dc(this);
		OnPrepareDC(&dc);
		SetOrigin(&dc);
		dc.DPtoLP(&cursor);
	}
	else
		pDC->DPtoLP(&cursor);


	double x, y;
	LPtoMicron(cursor, &x, &y);
    m_pMainFrame->SetStatusBarXY(x, y);

	if(m_bSetCursorDialog)
		m_pDlgSetCursor->SetCurrentCoord(x, y);
}


BOOL CGdsView::OnEraseBkgnd(CDC* pDC) 
{
	DeleteCursor();
	DrawAxis(FALSE);

    CBrush br(GetSysColor(COLOR_WINDOW)); 
    FillOutsideRect( pDC, &br );

	BOOL ret = CScrollView::OnEraseBkgnd(pDC);
	return ret;
}

void CGdsView::FillBackground(BOOL screen)
{
    CRect rect;
	if(screen){
		m_pDC->SelectClipRgn(&m_rgnUpdate, RGN_COPY);
		m_rgnUpdate.GetRgnBox(&rect);
		m_pDC->DPtoLP(&rect);
		m_pDC->FillSolidRect(&rect, m_colorBK);
		m_pDC->SelectClipRgn(NULL);
		m_bCursorState = FALSE;
		return;
	}

	if(m_intDrawMode == DRAW_MODE_PRINT)
		return;

	if(m_intDrawMode == DRAW_MODE_CLIPBOARD){
		Color back;
		back.SetFromCOLORREF(m_colorBK);
		SolidBrush brush(back);
		m_pGraphics->FillRectangle(&brush, m_rectClip.left, 
			m_rectClip.top,	m_rectClip.Width(), m_rectClip.Height());
	}
}

#include "ShrinkDlg.h"
void CGdsView::OnSetShrinkRate() 
{
	CShrinkDlg shrinkdlg;
    shrinkdlg.SetShrinkRate(m_dblShrinkRate);
	if(shrinkdlg.DoModal() == IDCANCEL)
		return;
	double newrate = shrinkdlg.GetShrinkRate();
	if(newrate != m_dblShrinkRate)
		InvalidateRect(TRUE, NULL);
	m_dblShrinkRate = newrate;

	if(m_bStructureInfoDlg)
		m_pDlgStructureInfo->SetInformation();
}

void CGdsView::ReadTecFile()
{ 
	CGdsApp* app = (CGdsApp*)AfxGetApp();


	CString path_default = app->GetProfileString("Settings", "Default technology map file", "");
	int idx = path_default.ReverseFind('\\');
	if(idx >= 0)
		path_default = path_default.Left(idx);

	CGdsDoc* pDoc = GetDocument();

	CString path = app->GetTecFile();

	if(path.GetLength() == 0){
		CFileFind find;
		path = pDoc->GetPathName();
		idx = path.ReverseFind('\\');
		if(idx >= 0)
			path = path.Left(idx);

		BOOL try_local = TRUE;

		if(path == path_default)
			try_local = FALSE;
		else
			path += "\\*.tec";
		if(try_local && find.FindFile(path)){
			find.FindNextFile(); // 1st file only
			path = find.GetFilePath();
		}
		else
			path = app->GetProfileString("Settings", "Default technology map file", "");
	}

	if(path.GetLength() == 0)
		return;

	CLayerSet layers[GDS_MAX_LAYER];
	for(int i = 0;i < GDS_MAX_LAYER;i++){
		(layers + i)->m_intNumber = i;
		(layers + i)->m_color = *(m_aryColorLayerRGB + i);
		(layers + i)->m_intFill = *(m_aryIntFillType + i);
		(layers + i)->m_bShow = *(m_bShowList + i);
		(layers + i)->m_bValid = FALSE;
		(layers + i)->m_strName = *(m_aryLayerName + i);
		(layers + i)->m_intOrder = -1;
	}
	idx = 0;
	for(i = 0;i < GDS_MAX_LAYER;i++){
		int layer = *(m_aryIntLayerOrder + i);
		if(layer >= 0)
			(layers + layer)->m_intOrder = idx++;
	}

	CLayerInfo info;
	if(!info.ReadLayerSetup(layers, path))
		return;
	GetDocument()->SetTecFilePath(info.GetTecFilePath());
	ReorderLayer(layers);
}

void CGdsView::ReorderLayer(CLayerSet* layers)
{
	for(int i = 0;i < GDS_MAX_LAYER;i++){
		*(m_aryColorLayerRGB + i) = (layers + i)->m_color;
		*(m_aryIntFillType + i) = (layers + i)->m_intFill;
		*(m_bShowList + i) = (layers + i)->m_bShow;
		*(m_aryLayerName + i) = (layers + i)->m_strName;
		*(m_bLayerInTec + i) = (layers + i)->m_bValid;
		*(m_aryIntLayerOrder + i) = -1;
	}
	int max = 0;
	for(i = 0;i < GDS_MAX_LAYER;i++){
		int idx = (layers + i)->m_intOrder;
		if(idx >= 0){
			*(m_aryIntLayerOrder + idx) = (layers + i)->m_intNumber;
			max++;
		}
	}
	for(i = 0;i < GDS_MAX_LAYER;i++){
		if(! *(m_bLayerExist + i))
			continue;
		BOOL ok = FALSE;
		for(int j = 0;j < max;j++){
			if(*(m_aryIntLayerOrder + j) == i){
				ok = TRUE;
				break;
			}
		}
		if(!ok)
			*(m_aryIntLayerOrder + max++) = i;
	}
}


void CGdsView::SetDefaultLayerColor()
{
	CWinApp* app = AfxGetApp();

	for(int i = 0;i < GDS_MAX_LAYER;i++){
		*(m_aryColorLayerRGB + i) = COLOR_INVALID;
		(m_aryColorLayer + i)->SetFromCOLORREF(*(m_aryColorLayerRGB + i));
		*(m_aryIntFillType + i) = FILL_BMP_NONE;
	}

	ReadTecFile();

	/*
	const int full = 220;
	int red, green, blue;
	COLORREF color[GDS_MAX_LAYER];

	double factor = (double)full/(m_intNumActiveLayer - (m_intNumActiveLayer > 1 ? 1 : 0));

	for(i = 0;i < m_intNumActiveLayer;i++){
		int idx = (int)(i * factor);
		if(idx < full/2)
			blue = full - idx*2;
		else
			blue = (idx - full/2)*2;


		if(idx < full/3)
			green = full/6 + idx*2;
		else{
			if(idx < full*5/6)
				green = full - (idx - full/3)*2;
			else
				green = (idx - full*5/6)*2;
		}

		if(idx < full/6)
			red = full - full/3 - idx*2;
		else{
			if(idx < full*2/3)
				red = (idx - full/6)*2;
			else
				red = full - (idx - full*2/3)*2;
		}
		red *= 2;
		red = red % 256;

		if(red < full && green < full && blue < full){
			double max = max(red, max(green, blue));
			double ratio = full/max;
			red = (int)(red * ratio);
			green = (int)(green * ratio);
			blue = (int)(blue * ratio);
		}
		if(red > 255) red = 255;
		if(green > 255) green = 255;
		if(blue > 255) blue = 255;

		*(color + i) = RGB(red, green, blue);
	}

	int idx = 0;
	for(i = 0;i < GDS_MAX_LAYER;i++){
		if(*(m_bLayerExist + i) && *(m_aryColorLayerRGB + i) == COLOR_INVALID)
			*(m_aryColorLayerRGB + i) = *(color + idx++);
		(m_aryColorLayer + i)->SetFromCOLORREF(*(m_aryColorLayerRGB + i));
	}*/

	COLORREF win_color[] = {  // Windows fundamental 48 colors
		RGB(255,128,128), RGB(255,255,128), RGB(128,255,128), RGB(  0,255,128),
		RGB(128,255,255), RGB(  0,128,255), RGB(255,128,192), RGB(255,128,255),
		RGB(255,  0,  0), RGB(255,255,  0), RGB(128,255,  0), RGB(  0,255, 64),
		RGB(  0,255,255), RGB(  0,128,192), RGB(128,128,192), RGB(255,  0,255),
		RGB(128, 64, 64), RGB(255,128, 64), RGB(  0,255,  0), RGB(  0,128,128),
		RGB(  0, 64,128), RGB(128,128,255),	RGB(128,  0, 64), RGB(255,  0,128),
		RGB(128,  0,  0), RGB(255,128,  0), RGB(  0,128,  0), RGB(  0,128, 64),
		RGB(  0,  0,255), RGB(  0,  0,160), RGB(128,  0,128), RGB(128,  0,255),
		RGB(128, 64,  0), RGB(  0, 64, 64),	RGB(  0,  0,128), RGB( 64,  0, 64),
		RGB( 64,  0,128), RGB(128,128,  0), RGB(128,128, 64), RGB(128,128,128),
		RGB( 64,128,128), RGB(192,192,192), RGB( 64,  0, 64),
	// do not use black, white and too dark
	//	RGB(  0,  0,  0), RGB(255,255,255), 
	//  RGB( 64,  0,  0), RGB(  0, 64,  0), RGB(  0,  0, 64), 
	};
	int idx = 0;
	for(i = 0;i < GDS_MAX_LAYER;i++){
		if(*(m_bLayerExist + i) && *(m_aryColorLayerRGB + i) == COLOR_INVALID)
			*(m_aryColorLayerRGB + i) = *(win_color + idx++);
		(m_aryColorLayer + i)->SetFromCOLORREF(*(m_aryColorLayerRGB + i));
		if(idx >= sizeof(win_color)/sizeof(COLORREF))
			idx = 0;
	}

}

#include "FontSizeDlg.h"
void CGdsView::OnSetFontSize() 
{
	CFontSizeDlg dlg;
	dlg.SetDC(m_pDC);
	dlg.SetFontSize(m_dblTextHeight*1e+9);
	dlg.SetCurrentFont(m_strFontName);
	dlg.SetFontStyle(m_intFontStyle);
	if(dlg.DoModal() == IDCANCEL)
		return;

	CWinApp* app = AfxGetApp();

	m_strFontName = dlg.GetFontName();
	if(m_strFontName.GetLength() == 0)
		m_strFontName = DEFAULT_GDS2_FONT;

	if(m_pwcharFontName)
		delete [] m_pwcharFontName;
	int len = lstrlenA(m_strFontName) + 1;
	m_pwcharFontName = new WCHAR[len * 2];
	ATLA2WHELPER(m_pwcharFontName, m_strFontName, len);

	app->WriteProfileString("Misc", "Font Name", m_strFontName);
	m_intFontStyle = dlg.GetFontStyle();
	app->WriteProfileInt("Misc", "Font Style", m_intFontStyle);

	m_dblTextHeight = dlg.GetFontSize();
	//app->WriteProfileInt("Misc", "Text Height UM", (int)m_dblTextHeight);
	//m_dblTextHeight *= 1e-6;
	app->WriteProfileInt("Misc", "Text Height NM", (int)(1000*m_dblTextHeight));
	m_dblTextHeight *= 1e-9;
	InvalidateRect(TRUE, NULL);
}

void CGdsView::OnDecreaseFontSize() 
{
	if(!m_bShowText)
		return;

	m_dblTextHeight *= 1.0e+9;
	int n10 = (int)log10(m_dblTextHeight);
	int n = (int)(m_dblTextHeight/pow(10.0, n10));
	if(n == 0){
		n10--;
		n = (int)(m_dblTextHeight/pow(10.0, n10));
	}
	n--;
	if(n <= 0){
		n = 9;
		n10--;
	}
	m_dblTextHeight = pow(10.0, n10) * n; 

	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Text Height NM", (int)(1000*m_dblTextHeight));
	m_dblTextHeight *= 1e-9;
	InvalidateRect(TRUE, NULL);
}

void CGdsView::OnIncreaseFontSize() 
{
	if(!m_bShowText)
		return;

	m_dblTextHeight *= 1.0e+9;
	int n10 = (int)log10(m_dblTextHeight);
	int n = (int)(m_dblTextHeight/pow(10.0, n10));
	if(n == 0){
		n10--;
		n = (int)(m_dblTextHeight/pow(10.0, n10));
	}
	n++;
	m_dblTextHeight = pow(10.0, n10) * n;

	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Text Height NM", (int)(1000*m_dblTextHeight));
	m_dblTextHeight *= 1e-9;
	InvalidateRect(TRUE, NULL);
}

void CGdsView::OnViewText() 
{
	OnButtonStopDraw();
	m_bShowText ^= TRUE;
	InvalidateRect(TRUE, NULL);

	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Show Text", m_bShowText ? 1 : 0);
}

void CGdsView::OnUpdateViewText(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE); 
	pCmdUI->SetCheck(m_bShowText);
}

void CGdsView::OnUpdateSetFontSize(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

void CGdsView::OnUpdateDecreaseFontSize(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

void CGdsView::OnUpdateIncreaseFontSize(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}


double CGdsView::OnFitView(BOOL examine, BOOL view_to_paper, BOOL optshrink)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);

    double xscale, scalex, scaley;

	CSize sz = GetTotalSize();
	CRect rect;
	rect.left = -sz.cx/2;
	rect.right = sz.cx/2;
	rect.top = sz.cy/2;
	rect.bottom = -sz.cy/2;

	double ratio = SCALE_FIT_RATIO;
	double width = m_coordMax.x - m_coordMin.x;
	double height = m_coordMax.y - m_coordMin.y;
	if(view_to_paper){
		CRect wrect;
		GetClientRect(&wrect);
		dc.DPtoLP(&wrect);
		wrect.NormalizeRect();
		width = wrect.Width() / m_dblDrawScale;
		height = wrect.Height() / m_dblDrawScale;
		//ratio = 1.0;
	}
	if(width == 0.0)
		scalex = DBL_MAX;
	else
		scalex = fabs(ratio*(rect.Width())/width/m_dblDrawScale);
	if(height == 0.0)
		scaley = DBL_MAX;
	else
		scaley = fabs(ratio*(-rect.Height())/height/m_dblDrawScale);
	xscale = min(scalex, scaley);

	if(examine){
		xscale *= SCALE_FIT_RATIO;
		double retval = m_dblDrawScale * xscale / (GetGDShcy()->GetDBUnit()/ HIMETRIC);
		if(optshrink)
			retval /= (1.0 - m_dblShrinkRate);
		return retval;
	}

    m_dblDrawScale *= xscale;
	m_coordViewOffset.x = -(m_coordMax.x + m_coordMin.x)/2.0;
	if(m_intMirror > 0)
		m_coordViewOffset.y = (m_coordMax.y + m_coordMin.y)/2.0;
	else
		m_coordViewOffset.y = -(m_coordMax.y + m_coordMin.y)/2.0;
	RoundToUserGrid(&m_coordViewOffset.x, &m_coordViewOffset.y);
	
	CPoint pos(sz.cx/2, -sz.cy/2);
	MoveViewOffset(0.0, 0.0);
	ScrollToPosition(pos);
	return m_dblDrawScale;
}

void CGdsView::ZoomFitWindow(BOOL redraw)
{
	CDC* pDC = GetWindowDC();
	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);

    long double xscale, scalex, scaley;

	CRect rect;
	if(! ::GetClientRect(m_hWnd, &rect))
		MessageBox("Warninig : failed to get client rect.");
    dc.DPtoLP(&rect);
    CPoint point;
 
    m_dblDrawScale = 1.0;
	long double fit_factor = SCALE_FIT_RATIO;
	if(fabs(m_coordMax.x - m_coordMin.x) < 1.0)
		scalex = DBL_MAX;
	else
		scalex = fabs(fit_factor*rect.Width()/(m_coordMax.x - m_coordMin.x));

	if(fabs(m_coordMax.y - m_coordMin.y) < 1.0)
		scaley = DBL_MAX;
	else
		scaley = fabs(fit_factor*rect.Height()/(m_coordMax.y - m_coordMin.y));

	xscale = min(scalex, scaley);

    m_dblDrawScale *= xscale;
	m_dblDrawScaleFit = m_dblDrawScale;

	point.x =  -(rect.Width())/2;
	point.y =  -rect.Height()/2;
    point -= dc.GetWindowOrg();
	m_coordViewOffset.x = -(m_coordMax.x + m_coordMin.x)/2.0;
	m_coordViewOffset.y = -(m_coordMax.y + m_coordMin.y)/2.0;
	if(m_intMirror > 0)
		m_coordViewOffset.y = -m_coordViewOffset.y;
    MoveViewOffset(0.0, 0.0);

	ScrollToPosition(point);
	if(redraw)
		InvalidateRect(TRUE, NULL);
}

void CGdsView::OnFitWindow() 
{
	OnButtonStopDraw();
	GetViewArea();
	ZoomFitWindow();
	m_bUpdateDrawTime = TRUE;
}

void CGdsView::OnUpdateSetLayer(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_intNumActiveLayer > 0 ? TRUE : FALSE);
	pCmdUI->SetCheck(m_bDlgLayer);
}

void CGdsView::OnUpdateSelectStr(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetGDShcy()->GetNumStructure() > 0 ? TRUE : FALSE);
	pCmdUI->SetCheck(m_bStructureDialog);
}

#include "AngleDlg.h"
void CGdsView::OnSetAngle() 
{
	OnButtonStopDraw();
	CAngleDlg dlg;
	dlg.SetAngle(m_dblViewRotation);
	if(dlg.DoModal() == IDCANCEL)
		return;
	m_dblViewRotation = dlg.GetAngle();
	if(m_dblViewRotation < 0.0)
		m_dblViewRotation = 360.0 + m_dblViewRotation;
	if(m_dblViewRotation != 0.0)
		m_pMainFrame->SetSyncView(FALSE);
	ChangeStructure(NULL, TRUE);
}

void CGdsView::OnMirrorX() 
{
	StopDraw();
	if(m_intMirror > MIRROR_NONE)
		m_intMirror = MIRROR_NONE;
	else{
		m_intMirror = MIRROR_TO_X;
	}
	ChangeStructure(NULL, TRUE);
}

void CGdsView::OnUpdateMirrorX(CCmdUI* pCmdUI) 
{
	if(m_intMirror == MIRROR_TO_Y || m_pMainFrame->GetSyncView())
		pCmdUI->Enable(FALSE);
	else{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(m_intMirror > MIRROR_NONE ? TRUE : FALSE);
	}
}

void CGdsView::OnMirrorY() 
{
	StopDraw();
	if(m_intMirror > MIRROR_NONE)
		m_intMirror = MIRROR_NONE;
	else
		m_intMirror = MIRROR_TO_Y;
	ChangeStructure(NULL, TRUE);
}

void CGdsView::OnUpdateMirrorY(CCmdUI* pCmdUI) 
{
	if(m_intMirror == MIRROR_TO_X || m_pMainFrame->GetSyncView())
		pCmdUI->Enable(FALSE);
	else{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(m_intMirror > MIRROR_NONE ? TRUE : FALSE);
	}
}


void CGdsView::OnFill() 
{
	OnButtonStopDraw();
	m_bFillEnable ^= TRUE;
	InvalidateRect(TRUE, NULL);

	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Fill Polygon", m_bFillEnable ? 1 : 0);
}

void CGdsView::OnUpdateFill(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_bFillEnable);
}

void CGdsView::DrawCenterMark()
{
	if(!m_bShowCenterCross)
		return;
	CPoint pfm1, pfm2, pto1, pto2;

	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);

	double len;
	CRect rect;
	GetClientRect(&rect);
	dc.DPtoLP(&rect);
	len = ((rect.Width())*0.03);
	COLORREF color;
	if(m_colorBK == COLOR_WHITE || m_intDrawMode == DRAW_MODE_PRINT)
		color = COLOR_BLACK;
	else
		color = COLOR_WHITE;
	pfm1.x = (long)(-len/2 + m_coordViewOffset.x * m_dblDrawScale);
	pfm1.y = (long)(-len/2 + m_coordViewOffset.y * m_dblDrawScale);
	pto1.x = (long)(pfm1.x + len);
	pto1.y = (long)(pfm1.y + len);
	if(m_intMirror > 0)
		pfm1.y = -pfm1.y;
	if(m_intMirror > 0)
		pto1.y = -pto1.y;
	pfm2.x = (long)(-len/2 + m_coordViewOffset.x * m_dblDrawScale);
	pfm2.y = (long)(len/2 + m_coordViewOffset.y * m_dblDrawScale);
	pto2.x = (long)(pfm2.x + len);
	pto2.y = (long)(pfm2.y - len);
	if(m_intMirror > 0)
		pfm2.y = -pfm2.y;
	if(m_intMirror > 0)
		pto2.y = -pto2.y;

	rect.left = pfm1.x;
	rect.bottom = pfm1.y;
	rect.right = pto1.x;
	rect.top = pto1.y;
	rect.NormalizeRect();
	if(IsOutOfView(rect))
		return;

	Point p1(pfm1.x, pfm1.y);
	Point p2(pto1.x, pto1.y);
	Color pcolor;
	pcolor.SetFromCOLORREF(color);
	Pen gpen(pcolor, m_realPenWidth);

	if(m_intDrawMode == DRAW_MODE_DISPLAY){
		Graphics g(dc.m_hDC);
		g.DrawLine(&gpen, p1, p2);
		p1.X = pfm2.x;
		p1.Y = pfm2.y;
		p2.X = pto2.x;
		p2.Y = pto2.y;
		g.DrawLine(&gpen, p1, p2);
	}
	else{
		m_pGraphics->DrawLine(&gpen, p1, p2);
		p1.X = pfm2.x;
		p1.Y = pfm2.y;
		p2.X = pto2.x;
		p2.Y = pto2.y;
		m_pGraphics->DrawLine(&gpen, p1, p2);
	}
}

void CGdsView::OnShowCenterCross() 
{
	m_bShowCenterCross ^= TRUE;
	if(m_bShowCenterCross){
		DrawCenterMark();
	}
	else
		InvalidateRect(TRUE, NULL);
}

void CGdsView::OnUpdateShowCenterCross(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowCenterCross);
}



void CGdsView::OnZoomSwitch() 
{
	if(m_intEditMode != EDIT_MODE_ZOOM)
		ChangeEditMode(EDIT_MODE_ZOOM);
	else
		ChangeEditMode(EDIT_MODE_NONE);
}

void CGdsView::OnUpdateZoomSwitch(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_intEditMode == EDIT_MODE_ZOOM ? TRUE : FALSE);
}

void CGdsView::OnMeasureSwitch() 
{
	if(m_intEditMode != EDIT_MODE_MEASURE){
		ChangeEditMode(EDIT_MODE_MEASURE);
		SetCommandGuide(CMD_GUIDE_MEASURE_P1);
	}
	else{
		AbortMeasuring();
		ChangeEditMode(EDIT_MODE_NONE);
	}
}

void CGdsView::OnUpdateMeasureSwitch(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_intEditMode == EDIT_MODE_MEASURE ? TRUE : FALSE);
}

void CGdsView::OnShowGridDlg() 
{
	m_bShowGridDlg ^= TRUE;
	if(m_bShowGridDlg){
		m_pDlgGrid = new CGridDlg(this);
		m_pDlgGrid->SetEnabler(&m_bShowGridDlg);
		m_pDlgGrid->Create(IDD_GRID_DLG, this);
		SetFocus();
	}
	else{
		m_pDlgGrid->ExitMeasure();
		m_pDlgGrid = NULL;
	}
}

void CGdsView::OnUpdateShowGridDlg(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowGridDlg);
}

void CGdsView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_pThreadRender && !m_pThreadRender->WaitUntilDrawn())
		return;

	m_bUpdateDrawTime = TRUE;
	m_bButtonDown = TRUE;
	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);
	SetCapture();

	CPoint lpoint = point;
    dc.DPtoLP(&lpoint);

	double x, y;

	switch(m_intEditMode){
	case EDIT_MODE_ZOOM:
		m_rectView.left = point.x;
		m_rectView.top  = point.y;
		m_cOldCursor = SetCursor(m_cZoomCursor);
		return;
		break;
	case EDIT_MODE_MEASURE:
		x = (lpoint.x / m_dblDrawScale - m_coordViewOffset.x);
		y = (lpoint.y / m_dblDrawScale - m_coordViewOffset.y); 
		RoundToUserGrid(&x, &y);
		if(m_scale.IsMeasuring()){
			DrawScale(m_scale.GetPoint1(), m_scale.GetPoint2());
			MakeBufferBmp();
			ShowBufferBmp();
			m_scale.AddScale();
			SetCommandGuide(CMD_GUIDE_MEASURE_P1);
		}
		else{
			m_scale.SetPoint1(CCoord(x, y));
			SetCommandGuide(CMD_GUIDE_MEASURE_P2);
		}
		break;
	case EDIT_MODE_MEMO:
		if(m_bMemoDialog){
			if(m_pDlgMemo->UpdateMemoString().GetLength() > 0){
				m_pDlgMemo->OnApply();
				m_memo.AddMemo();
				DeleteCursor();
				DrawMemo(m_memo.GetLastMemo());
				MakeBufferBmp();
				ShowBufferBmp();
			}
		}
		break;
	case EDIT_MODE_NONE:
	default:
		m_rectView.left = point.x;
		m_rectView.top  = point.y;
		if(m_bSkelton && !IsDrawing()){
			if(m_bSelectAllCell){
				m_bSelectAllCell = FALSE;
				DrawSkelton(&dc, FALSE);
			}
			HitTestStructure(&dc, point, TRUE);
			if(nFlags & MK_SHIFT)
				OnStructureInfo();
			else if(nFlags & MK_CONTROL)
				ExpandCell();
		}
		break;
	}
	CScrollView::OnLButtonDown(nFlags, point);
}

void CGdsView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);
	ReleaseCapture();

	m_bButtonDown = FALSE;

	if(m_bDragging){
		CSize sz;
 		sz.cx = sz.cy = RESIZE_RECT_WIDTH;
		//dc.DrawDragRect(&m_rectDrag, sz, NULL, sz);
	}

	switch(m_intEditMode){
	case EDIT_MODE_NONE:
		if(m_bSkelton && m_bDragging && !IsDrawing()){
			CRect test;
			test = m_rectDrag;
			test.NormalizeRect();
			if(test.Width() > 2 && test.Height() > 2){
				DeleteDottedFrame(&dc);
				if(m_bSelectAllCell){
					m_bSelectAllCell = FALSE;
					DrawSkelton(&dc, FALSE);
				}
				HitTestStructure(&dc, test, TRUE, TRUE);
				if(nFlags & MK_SHIFT)
					OnStructureInfo();
			}
		}
		break;
	case EDIT_MODE_ZOOM:
		m_rectView.right = point.x;
		m_rectView.bottom = point.y;
		if(m_rectView.right != m_rectView.left &&
			m_rectView.top != m_rectView.bottom){
			OnZoomArea();
		}
		SetCursor(m_cOldCursor);
		break;
	case EDIT_MODE_MEASURE:
		break;
	default:
		break;
	}
    m_bDragging = FALSE;
	CScrollView::OnLButtonUp(nFlags, point);	
}

void CGdsView::OnPanToByHomeKey() 
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	OnMButtonDown(0, point);
}

void CGdsView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if(m_intEditMode == EDIT_MODE_NONE && m_pFrameActive){
		if(!IsDrawing())
			GoInToCell();
	}
	else
		OnMButtonDown(nFlags, point);

	CScrollView::OnLButtonDblClk(nFlags, point);
}

void CGdsView::GoInToCell()
{
	if(m_pFrameActive){
		StopDraw();
		AddHistory();
		ChangeStructure(m_pFrameActive->GetStructure(), TRUE);
	}
}

void CGdsView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CPoint diff = m_pToolTip - point;
	if(abs(diff.x) > 1 || abs(diff.y) > 1){
		m_toolTipCtrl.Pop();
		m_pToolTip = point;
	}
	if(!IsDrawing())
	    UpdateXYCoord(point);

	DrawGridMarker(point);
	DrawCursor();

	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);

	switch(m_intEditMode){
	case EDIT_MODE_ZOOM:
		SetCursor(m_cZoomCursor);
		break;
	case EDIT_MODE_MEASURE:
		if(m_bShowGridDlg)
			m_pDlgGrid->OnOK();
		DrawRubberScale(point);
		break;
	case EDIT_MODE_MEMO:
		DrawRubberMemo(point);
		break;
	case EDIT_MODE_NONE:
	default:
		if(m_bSkelton && !IsDrawing())
			HitTestStructure(&dc, point, FALSE);
		break;

	}
	BOOL dragrect = FALSE;
	if(m_bButtonDown && !(nFlags & MK_SHIFT)){
		if(nFlags & MK_RBUTTON)
			dragrect = TRUE;
		else if((nFlags & MK_LBUTTON)){
			if(m_intEditMode == EDIT_MODE_NONE || m_intEditMode == EDIT_MODE_ZOOM)
				dragrect = TRUE;
		}
	}

	if(dragrect){
		CRect rect;
		rect.left = m_rectView.left;
		rect.top = m_rectView.top;
		rect.right = point.x;
		rect.bottom = point.y;

		if((nFlags & MK_RBUTTON) || m_intEditMode == EDIT_MODE_ZOOM){
			BOOL zoomUp = TRUE;
			if(rect.left > rect.right && rect.top > rect.bottom)
				zoomUp = FALSE;
			if(zoomUp)
				SetCursor(m_cZoomUpCursor);
			else
				SetCursor(m_cZoomDownCursor);
		}
		rect.NormalizeRect();
		CSize sz;

 		sz.cx = sz.cy = RESIZE_RECT_WIDTH;
	   if(!m_bDragging){
			dc.DrawDragRect(&rect, sz, NULL, sz);
			m_bDragging = TRUE;
		}
	   else{
		   if(m_bBufferScreen && !m_bSkelton)
				dc.DrawDragRect(&rect, sz, NULL/*&m_rectDrag*/, sz);
		   else
				dc.DrawDragRect(&rect, sz, &m_rectDrag, sz);
	   }
		m_rectDrag = rect;
	}

	CScrollView::OnMouseMove(nFlags, point);
}

void CGdsView::OnStructureInfo() 
{
	if(!m_pFrameActive)
		return;
	if(!m_bStructureInfoDlg){
		m_pDlgStructureInfo = new CStructureInfoDlg(this);
		m_pDlgStructureInfo->SetEnabler(&m_bStructureInfoDlg);
		m_pDlgStructureInfo->Create(IDD_STRUCTURE_INFO, this);
		m_bStructureInfoDlg = TRUE;
	}
}

void CGdsView::OnStructureContext(CPoint point)
{
	ClientToScreen(&point);

	CMenu *menu, *menu_tmp = NULL;

	menu_tmp = new CMenu;
	menu_tmp->LoadMenu(IDR_MENU_STRUCTURE);
	if(menu_tmp){
		menu = menu_tmp->GetSubMenu(0);
		menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
								point.x, point.y, this, NULL);
		menu->DestroyMenu();
		delete menu_tmp;
	}
}

void CGdsView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_bButtonDown = TRUE;
	m_rectView.left = point.x;
	m_rectView.top  = point.y;
	m_bUpdateDrawTime = TRUE;
	SetCapture();
	m_cOldCursor = SetCursor(m_cZoomCursor);

	if(m_scale.IsDragging())
		m_scale.SetDragging(FALSE);
	CScrollView::OnRButtonDown(nFlags, point);
}

void CGdsView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_bButtonDown = FALSE;

	m_rectView.right = point.x;
	m_rectView.bottom = point.y;
	ReleaseCapture();
	if(m_bDragging){
		CClientDC dc(this);
		OnPrepareDC(&dc);
		SetOrigin(&dc);

		CRect rect;
		rect.left = m_rectView.left;
		rect.top = m_rectView.top;
		rect.right = point.x;
		rect.bottom = point.y;
		rect.NormalizeRect();
		CSize sz;
 		sz.cx = sz.cy = RESIZE_RECT_WIDTH;

        dc.DrawDragRect(&rect, sz, NULL, sz);
		if(abs(m_rectView.right - m_rectView.left) > ZOOM_AREA_MIN &&
			abs(m_rectView.top - m_rectView.bottom) > ZOOM_AREA_MIN)
			OnZoomArea();
	}
	SetCursor(m_cOldCursor);

	m_bDragging = FALSE;
	CScrollView::OnRButtonUp(nFlags, point);
}

void CGdsView::OnForceRedraw() 
{
	OnButtonStopDraw();
	m_bUpdateDrawTime = TRUE;
	InvalidateRect(TRUE, NULL);
	m_bForceRedraw = TRUE;
}

void CGdsView::OnUpdateRedraw(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bAutoRedraw);
}

void CGdsView::OnAutoRedraw() 
{
	m_bAutoRedraw ^= TRUE;
	if(m_bAutoRedraw)
		InvalidateRect(TRUE, NULL);
	else
		OnButtonStopDraw();
}

void CGdsView::OnMButtonDown(UINT nFlags, CPoint point)
{
	if(m_pThreadRender && !m_pThreadRender->WaitUntilDrawn())
		return;
	m_bUpdateDrawTime = TRUE;
    m_rectView.left = point.x;
    m_rectView.top = point.y;
	PanTo();
}

void CGdsView::CreateComboBox(CRect rect, CToolBar* tb)
{
	if(!m_cmbboxStructure){
		m_cmbboxStructure.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL |
			CBS_DROPDOWNLIST | CBS_SORT, rect, tb, ID_STRUCTURE_CB);
		CString strname;
		CObArray* structures = GetGDShcy()->GetStructureList();
		for(int idx = 0;idx < structures->GetSize();idx++){
			CGDSStructure* str = (CGDSStructure*)structures->GetAt(idx);
			strname = str->GetStructureName();
			m_cmbboxStructure.AddString(strname);
		}
		m_cmbboxStructure.SetView(this);
	}
}

void CGdsView::CreateStrLvlBox(CRect rect, CToolBar *tb)
{
	if(!m_cmbboxStructureLvl){
		m_cmbboxStructureLvl.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL |
			CBS_DROPDOWNLIST, rect, tb, ID_STR_LEVEL_CB);
		CString string;
		for(int i = 0;i < 1024;i++){
			string.Format("%d", i);
			m_cmbboxStructureLvl.AddString(string);
		}
		m_cmbboxStructureLvl.SetView(this);
	}
}

void CGdsView::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CScrollView::OnShowWindow(bShow, nStatus);
}


void CGdsView::OnUpdateStructureCb(CCmdUI* pCmdUI) 
{
	if(!pCmdUI && m_pStrActive)
		m_cmbboxStructure.SetCurSel(m_pStrActive->GetStructureName());
	if(m_cmbboxStructure.GetDroppedState())
		return;

	CString name;
	int idx = m_cmbboxStructure.GetCurSel();
	if(idx == CB_ERR)
		return;
	m_cmbboxStructure.GetLBText(idx, name);

	if(!m_pStrActive || m_pStrActive->GetStructureName() != name){
		AddHistory();
		ChangeStructure(GetGDShcy()->FindStructure(name.GetBuffer(0)), TRUE);
	}
	return;
}


#include "StrLvlDlg.h"
void CGdsView::OnStructureLevel() 
{
	CStrLvlDlg dlg;
	dlg.SetLevel(m_intExpandLevel);
	if(dlg.DoModal() == IDCANCEL)
		return;
	int newlevel = dlg.GetLevel();

	if(newlevel == m_intExpandLevel)
		return;

	m_intExpandLevel = newlevel;
	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Substructure expand", m_intExpandLevel);
	StopDraw();
	OnUpdateStrLevelCb(NULL);
	InvalidateRect(TRUE, NULL);
}

int CGdsView::GetEncoderClsid(CString format, CLSID *pClsid)
{
	USES_CONVERSION;
	LPTSTR lpsz = new TCHAR[format.GetLength() + 1];
	_tcscpy(lpsz, format);
	WCHAR* wformat = A2W(lpsz);
	delete [] lpsz;

	UINT  num = 0;  // number of image encoders
	UINT  size = 0; // size of the image encoder array in bytes
	GetImageEncodersSize(&num, &size);
	if(size == 0)
	  return -1;
	ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(!pImageCodecInfo)
		return -1;
	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT i = 0; i < num; i++){
		if(wcscmp((pImageCodecInfo + i)->MimeType, wformat) == 0 ){
			*pClsid = (pImageCodecInfo + i)->Clsid;
			free(pImageCodecInfo);
			return i;
	  }    
	}
	free(pImageCodecInfo);
	return -1;
}

BOOL CGdsView::SaveImageAsFile(BOOL whole)
{
	enum fileType {
		TYPE_JPG = 0,
		TYPE_TIF,
		TYPE_BMP,
		TYPE_GIF,
		TYPE_PNG,
		TYPE_EMF,
		TYPE_NONE = -1,
	};
	char* formats[] = {
		"image/jpeg", 
		"image/tiff", 
		"image/bmp", 
		"image/gif", 
		"image/png",
	}; 

	CFileDialog dlg(FALSE, "emf", NULL, OFN_OVERWRITEPROMPT ,
		"JPG JPEG(*.JPG)|*.jpg|"
		"TIF Tag Image Format(*.TIF)|*.tif|"
		"BMP Windows Bitmap(*.BMP)|*.bmp|"
		"GIF CompuServe GIF(*.GIF)|*.gif|"
		"PNG Portable Network Graphics(*.PNG)|*.png|"
		"EMF Windows Enhanced Meta File (*.EMF)|*.emf|"
		"|", NULL);

	if(dlg.DoModal() == IDCANCEL)
		return FALSE;


	CString ext = dlg.GetFileExt(); 
	if(ext == "emf"){
		HENHMETAFILE metafile = WriteToEmf(whole);
		::DeleteEnhMetaFile(::CopyEnhMetaFile(metafile, dlg.GetPathName()));
		::DeleteEnhMetaFile(metafile);
		return TRUE;
	}

	int type = TYPE_NONE;
	if(ext == "bmp")
		type = TYPE_BMP;
	else if(ext == "jpg")
		type = TYPE_JPG;
	else if(ext == "gif")
		type = TYPE_GIF;
	else if(ext == "tif")
		type = TYPE_TIF;
	else if(ext == "png")
		type = TYPE_PNG;

	if(type < 0)
		return FALSE;

	HENHMETAFILE metafile = WriteToEmf(whole);
	Metafile gmetafile(metafile);

	char* format = *(formats + type);
	CLSID  Clsid;
	if(GetEncoderClsid(format, &Clsid) < 0){
		::DeleteEnhMetaFile(metafile);
		return FALSE;
	}

	// Save jpeg image with highest quality
	ULONG quality = 100;
	EncoderParameters param;
	param.Count = 1;
	param.Parameter[0].Guid = EncoderQuality;
	param.Parameter[0].Type = EncoderParameterValueTypeLong;
	param.Parameter[0].NumberOfValues = 1;
	param.Parameter[0].Value = &quality;

	USES_CONVERSION;
	LPTSTR lpsz = new TCHAR[dlg.GetPathName().GetLength() + 1];
	_tcscpy(lpsz, dlg.GetPathName());
	WCHAR* w = A2W(lpsz);
	if(type == TYPE_JPG)
		gmetafile.Save(w, &Clsid, &param);
	else
		gmetafile.Save(w, &Clsid);
	delete [] lpsz;
	::DeleteEnhMetaFile(metafile);
	return TRUE;
}

void CGdsView::OnWriteToEmf() 
{
	SaveImageAsFile(TRUE);
}

void CGdsView::OnWritePartToEmf() 
{
	SaveImageAsFile(FALSE);
}

HENHMETAFILE CGdsView::WriteToEmf(BOOL whole)
{
	m_pMainFrame->CreateAbortDrawTimer();

	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);
	dc.SetMapMode(MM_HIMETRIC);

	m_drawParam.PushParam(this);
	m_intDrawMode = DRAW_MODE_CLIPBOARD;

	DeleteCursor();

	CRect rect;
	CSize sz;

	if(whole){
		sz.cx = (long)((m_coordMax.x - m_coordMin.x) * m_dblDrawScale);
		sz.cy = (long)((m_coordMax.y - m_coordMin.y) * m_dblDrawScale);
		SetScrollSizes(MM_HIMETRIC, sz);
		OnFitView();
		sz = GetTotalSize(); //for fail safe
		m_rectClip.SetRect(-sz.cx/2, sz.cy/2, sz.cx/2, -sz.cy/2);
	}
	else{
		sz = GetTotalSize();
		GetClientRect(&rect);
		dc.DPtoLP(&rect);
		m_rectClip = rect;
	}
	m_rectClip.NormalizeRect();
	Rect mrect;
	mrect.X = m_rectClip.left;
	mrect.Y = -m_rectClip.bottom;
	mrect.Width = m_rectClip.Width();
	mrect.Height = m_rectClip.Height();
	Metafile m(dc.m_hDC, mrect, MetafileFrameUnitGdi, EmfTypeEmfPlusDual);
	m_pMetafile = &m;

	BOOL redraw = GetUpdateRect(&rect);
	SetRedraw(FALSE);
	OnDraw(&dc);
	SetRedraw(TRUE);
	if(redraw)
		InvalidateRect(TRUE, rect);

	m_intDrawMode = DRAW_MODE_DISPLAY;
	m_drawParam.PopParam(this);
	m_pMainFrame->KillAbortDrawTimer();

	HENHMETAFILE metafile = m_pMetafile->GetHENHMETAFILE();
	m_pMetafile = NULL;
	m_bCursorState = FALSE;

	DrawCursor();
	DrawGridMarker();

	return metafile;
}






void CGdsView::OnMouseCrossWindow() 
{
	if(m_intCursorType != CURSOR_CROSS_WINDOW)
		m_cOldCursor = GetCursor();
	m_intCursorType = CURSOR_CROSS_WINDOW;
	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Cursor style", m_intCursorType);
}

void CGdsView::OnMouseCross() 
{
	if(m_intCursorType != CURSOR_CROSS)
		m_cOldCursor = GetCursor();
	if(m_intCursorType == CURSOR_CROSS_WINDOW ||
		m_intCursorType == CURSOR_PLUS_WINDOW)
		DeleteCursor();
	m_intCursorType = CURSOR_CROSS;
	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Cursor style", m_intCursorType);
}

void CGdsView::OnMouseStandard() 
{
	if(m_intCursorType != CURSOR_STANDARD)
		m_cOldCursor = GetCursor();
	if(m_intCursorType == CURSOR_CROSS_WINDOW ||
		m_intCursorType == CURSOR_PLUS_WINDOW)
		DeleteCursor();
	m_intCursorType = CURSOR_STANDARD;
	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Cursor style", m_intCursorType);
}

void CGdsView::OnMousePlusWindow() 
{
	if(m_intCursorType != CURSOR_PLUS_WINDOW)
		m_cOldCursor = GetCursor();
	if(m_intCursorType == CURSOR_CROSS_WINDOW)
		DeleteCursor();
	m_intCursorType = CURSOR_PLUS_WINDOW;
	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Cursor style", m_intCursorType);
}

void CGdsView::OnUpdateMousePlusWindow(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_intCursorType == CURSOR_PLUS_WINDOW ? TRUE : FALSE);
}

void CGdsView::OnUpdateMouseCross(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_intCursorType == CURSOR_CROSS ? TRUE : FALSE);
}

void CGdsView::OnUpdateMouseCrossWindow(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_intCursorType == CURSOR_CROSS_WINDOW ? TRUE : FALSE);
}

void CGdsView::OnUpdateMouseStandard(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_intCursorType == CURSOR_STANDARD ? TRUE : FALSE);
}


void CGdsView::UpdateSubStructureLevel(int level)
{
	if(level == m_intExpandLevel)
		return;

	CString string;
	string.Format("%d", level);
	int idx = m_cmbboxStructureLvl.FindStringExact(0, string);
	if(idx != CB_ERR)
		m_cmbboxStructureLvl.SetCurSel(string);

	m_intExpandLevel = level;
	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Substructure expand", m_intExpandLevel);

	StopDraw();
	ChangeStructure(NULL, TRUE);
}

void CGdsView::OnUpdateStrLevelCb(CCmdUI* pCmdUI) 
{
	if(pCmdUI == NULL){
		CString str;
		str.Format("%d", m_intExpandLevel);
		m_cmbboxStructureLvl.SetCurSel(str);
		return;
	}
	if(m_cmbboxStructureLvl.GetDroppedState()){
		StopDraw();
		return;
	}

	CString label;
	int idx = m_cmbboxStructureLvl.GetCurSel();
	if(idx == CB_ERR)
		return;
	m_cmbboxStructureLvl.GetLBText(idx, label);
 
	int newlevel = atoi(label);
	if(newlevel == m_intExpandLevel)
		return;
	m_intExpandLevel = newlevel;
	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Substructure expand", m_intExpandLevel);
	if(m_bStructureDialog)
		m_pDlgStructure->SetExpandLevel(m_intExpandLevel);

	StopDraw();
	ChangeStructure(NULL, TRUE);
	return;
}



void CGdsView::OnHelp() 
{
    m_pMainFrame->OnHelpIndex();
}

void CGdsView::OnHelpFinder() 
{
    m_pMainFrame->OnHelpIndex();
}

void CGdsView::OnContextHelp() 
{
    m_pMainFrame->OnHelpIndex();
}

void CGdsView::OnSetCursorTo() 
{
	m_bSetCursorDialog ^= TRUE;
	if(m_bSetCursorDialog){
		m_pDlgSetCursor = new CSetCursor;
		m_pDlgSetCursor->SetEnabler(&m_bSetCursorDialog);
		m_pDlgSetCursor->SetCaller(this);
		m_pDlgSetCursor->Create(IDD_MOVE_TO, this);

		CString name = GetDocument()->GetPathName();
		int idx = name.ReverseFind('\\');
		if(idx > 0)
			name = name.Mid(++idx);
		m_pDlgSetCursor->SetWindowName(name);
	}
	else{
		m_pDlgSetCursor->ExitDialog();
		m_bSetCursorDialog = FALSE;
	}
}

void CGdsView::OnUpdateSetCursorTo(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bSetCursorDialog);
}

CPoint CGdsView::PanToPosMeter(CCoord point)
{
	double dbu = GetGDShcy()->GetDBUnit();
	point.x /= dbu * (1.0 - m_dblShrinkRate);
	point.y /= dbu * (1.0 - m_dblShrinkRate);
	return PanToPos(point, TRUE);
}

CPoint CGdsView::PanToPos(CCoord point, BOOL panto)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);

	point.x += m_coordViewOffset.x;
	if(m_intMirror > 0)
		point.y -= m_coordViewOffset.y;
	else
		point.y += m_coordViewOffset.y;

    //RoundToUserGrid(&point.x, &point.y);
    point.Mul(m_dblDrawScale);

	double offsetx = m_coordViewOffset.x / m_pGrid->GetMinorX();
	double offsety = m_coordViewOffset.y / m_pGrid->GetMinorY();
	double modx = modf(offsetx, &offsetx);
	double mody = modf(offsety, &offsety);
	offsetx = m_coordViewOffset.x - offsetx * m_pGrid->GetMinorX();
	offsety = m_coordViewOffset.y - offsety * m_pGrid->GetMinorY();

	CPoint p((long)point.x, (long)point.y);
	if(panto){
		dc.LPtoDP(&p);
		PanTo(FALSE, p);
	}
	return p;
}

void CGdsView::OnUpdateEditCopyCut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsSelected());
}

void CGdsView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(::IsClipboardFormatAvailable(CF_ENHMETAFILE));
}

void CGdsView::OnRenderAllFormats( )
{
	if(!OpenClipboard())
		return;

    EmptyClipboard();

	OnRenderFormat(CF_ENHMETAFILE);
	OnRenderFormat(CF_BITMAP);


	CloseClipboard();
}

void CGdsView::OnRenderFormat( UINT format) 
{	
	if(!m_hMetafile)
		return;

	if(format == CF_ENHMETAFILE)
        SetClipboardData(CF_ENHMETAFILE, m_hMetafile);
	else if(format == CF_BITMAP){
	/*  //Following procedure does not work. I don't know why.
		IStream* is;
		CreateStreamOnHGlobal(NULL, TRUE, &is);
		Metafile metafile(m_hMetafile);

		CLSID  Clsid;
		if(GetEncoderClsid("image/bmp", &Clsid) < 0)
			return;
		metafile.Save(is, &Clsid);
		Bitmap bmp(is);
		HBITMAP hbmp;
		bmp.GetHBITMAP(Color(0, 0, 0, 0), &hbmp);
		::SetClipboardData(CF_BITMAP, hbmp);*/
		SetClipboardData(CF_BITMAP, m_bmpCopy);
	}
}

void CGdsView::OnEditCopy()
{
	CGdsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if(OpenClipboard()){
		CRect rect;
		GetCursorPos(&rect.TopLeft());
		rect.BottomRight() = rect.TopLeft();
		ClipCursor(rect);

        EmptyClipboard();

		if(m_hMetafile){
			::DeleteEnhMetaFile(m_hMetafile);
			m_bmpCopy.DeleteObject();
		}

		m_hMetafile = WriteToEmf(FALSE);
        SetClipboardData(CF_ENHMETAFILE, NULL);

		WriteToBmp(&m_bmpCopy);
        SetClipboardData(CF_BITMAP, NULL);
		CloseClipboard();

		ClipCursor(NULL);
	}
	else
		AfxMessageBox(CG_IDS_CANNOT_OPEN_CLIPBOARD);
}

void CGdsView::OnEditCut()
{
	CSharedFile	memFile;
	CArchive ar(&memFile, CArchive::store|CArchive::bNoFlushOnDelete);

	DoCutCopyPaste(ar, TRUE);

	ar.Flush();

	HGLOBAL hData = memFile.Detach();
	if (OpenClipboard()){
		::SetClipboardData(CF_ENHMETAFILE, hData);
		CloseClipboard();
	}
	else
		AfxMessageBox(CG_IDS_CANNOT_OPEN_CLIPBOARD);
}

void CGdsView::OnEditPaste()
{
	if(OpenClipboard()){
		HANDLE hData = ::GetClipboardData(CF_ENHMETAFILE);
		if(hData != NULL){
			CSharedFile memFile;
			memFile.SetHandle(hData,FALSE);
			CArchive ar(&memFile, CArchive::load);

			DoCutCopyPaste(ar, FALSE);
			ar.Close();
			memFile.Detach();
		}
		else
			AfxMessageBox(CG_IDS_CANNOT_GET_CLIPBOARD_DATA);
		CloseClipboard();
	}
	else
		AfxMessageBox(CG_IDS_CANNOT_OPEN_CLIPBOARD);
}

BOOL CGdsView::IsSelected()
{

	return TRUE;
}

void CGdsView::DoCutCopyPaste(CArchive &ar, BOOL bCut)
{
	GetDocument()->Serialize(ar);

	if(bCut){
		ASSERT(ar.IsStoring());
		GetDocument()->DeleteContents();
	}
	if (!(ar.IsStoring() && !bCut)){
		GetDocument()->UpdateAllViews(NULL);
		GetDocument()->SetModifiedFlag();
	}
}


int CGdsView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_toolTipCtrl.Create(this);
	m_toolTipCtrl.AddTool(this, LPSTR_TEXTCALLBACK); 
	m_pMainFrame = (CMainFrame*)GetTopLevelFrame();
    DisableStatusBarDisplay(0);
	m_pMainFrame->ActivateToolTip(FALSE);
	BringWindowToTop();

	m_oleDropTarget.Register(this);

	return 0;
}


void CGdsView::RoundToUserGrid(double *x, double *y, BOOL snap)
{
	double mulx = snap ? m_pGrid->GetSnapGridX() : m_pGrid->GetMinorX();
	double muly = snap ? m_pGrid->GetSnapGridY() : m_pGrid->GetMinorY();


	double dec;
	if(mulx - 1.0 > mulx/pow(10.0, 15.0)){
		dec = modf(*x, x); // round to database unit (to integer)
		if(dec > 0.5)
			*x += 1.0;
		else if(dec < -0.5)
			*x -= 1.0;
	}
	if(muly - 1.0 > muly/pow(10.0, 15.0)){
		dec = modf(*y, y);
		if(dec > 0.5)
			*y += 1.0;
		else if(dec < -0.5)
			*y -= 1.0;
	}

	*x /= mulx; // convert to user unit
	*y /= muly;
	dec = modf(*x, x); // round to user unit
	if(dec > 0.5)
		*x += 1.0;
	else if(dec < -0.5)
		*x -= 1.0;

	dec = modf(*y, y);
	if(dec > 0.5)
		*y += 1.0;
	else if(dec < -0.5)
		*y -= 1.0;
	*x *= mulx;
	*y *= muly;

	if(!snap){
		double offsetx = m_coordViewOffset.x / mulx;
		double offsety = m_coordViewOffset.y / muly;
		double modx = modf(offsetx, &offsetx);
		double mody = modf(offsety, &offsety);

		if(offsetx > 0.0)
			offsetx = (double)((long)(offsetx + 0.5));
		else
			offsetx = (double)((long)(offsetx - 0.5));

		if(offsety > 0.0)
			offsety = (double)((long)(offsety + 0.5));
		else
			offsety = (double)((long)(offsety - 0.5));
		offsetx = m_coordViewOffset.x - offsetx * mulx;
		offsety = m_coordViewOffset.y - offsety * muly;
		*x += offsetx;
		*y += offsety;

		*x /= mulx; // convert to user unit
		*y /= muly;
		dec = modf(*x, x); // round to user unit
		if(dec > 0.5)
			*x += 1.0;
		else if(dec < -0.5)
			*x -= 1.0;

		dec = modf(*y, y);
		if(dec > 0.5)
			*y += 1.0;
		else if(dec < -0.5)
			*y -= 1.0;
		*x *= mulx;
		*y *= muly;
	}
}

void CGdsView::OnCycleGrid() 
{
	switch(m_pGrid->GetGridType()){
	case GRID_TYPE_NONE:
		OnGridDot();
		break;
	case GRID_TYPE_DOT:
		OnGridLine();
		break;
	case GRID_TYPE_LINE:
		OnGridNone();
		break;
	default:
		break;
	}
}

void CGdsView::OnGridNone() 
{
	if(m_pGrid->GetGridType() == GRID_TYPE_NONE)
		return;

	InvalidateRect(TRUE, NULL);
	m_pGrid->SetGridType(GRID_TYPE_NONE);
	if(m_bShowGridDlg)
		m_pDlgGrid->SetGridType(m_pGrid->GetGridType());
	SetCommandGuide(CMD_GUIDE_UPDATE);
}

void CGdsView::OnGridDot() 
{
	if(m_pGrid->GetGridType() == GRID_TYPE_DOT)
		return;

	InvalidateRect(TRUE, NULL);
	m_pGrid->SetGridType(GRID_TYPE_DOT);
	if(m_bShowGridDlg)
		m_pDlgGrid->SetGridType(m_pGrid->GetGridType());
	SetCommandGuide(CMD_GUIDE_UPDATE);
}

void CGdsView::OnGridLine() 
{
	if(m_pGrid->GetGridType() == GRID_TYPE_LINE)
		return;

	InvalidateRect(TRUE, NULL);
	m_pGrid->SetGridType(GRID_TYPE_LINE);
	if(m_bShowGridDlg)
		m_pDlgGrid->SetGridType(m_pGrid->GetGridType());
	SetCommandGuide(CMD_GUIDE_UPDATE);
}

void CGdsView::CheckMeasureDirection(double* x, double* y)
{
	if(!m_scale.IsMeasuring())
		return;
	
	CCoord p1 = m_scale.GetPoint1();
	double xdiff = *x - p1.x;
	double ydiff = *y - p1.y;
	double ratio = fabs(ydiff/xdiff);
	BOOL x_first = fabs(xdiff) > fabs(ydiff) ? TRUE : FALSE;
	switch(m_scale.GetScaleDirection()){
	case MEAS_DIR_DIAGONAL:
		if(ratio > TAN225 && ratio < TAN675){
			if(x_first)
				*y = p1.y + fabs(xdiff) * (ydiff > 0 ? 1.0 : -1.0);
			else
				*x = p1.x + fabs(ydiff) * (xdiff > 0 ? 1.0 : -1.0);
			break;
		}
	case MEAS_DIR_XY:
		if(fabs(xdiff) > fabs(ydiff))
			*y = p1.y;
		else
			*x = p1.x;
		break;
	case MEAS_DIR_ANY:
	default:
		break;
	}
}

void CGdsView::DrawGridMarker()
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	DrawGridMarker(point);
}

void CGdsView::DrawGridMarker(CPoint point)
{
	if(!m_bBufferScreen)
		return;
	if(m_bInitView || !::IsWindow(this->m_hWnd))
		return;
	if(m_bCancelDraw || m_bSkelton)
		return;
    if(!m_bMouseInView)
        return;

	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);

	dc.DPtoLP(&point);
	double x = (point.x / m_dblDrawScale - m_coordViewOffset.x);
	double y = point.y / m_dblDrawScale;
	if(m_intMirror)
		y += m_coordViewOffset.y; 
	else
		y -= m_coordViewOffset.y;
	RoundToUserGrid(&x, &y);

	if(!m_scale.IsMeasuring()){
		m_scale.SetPoint1(CCoord(x, y));
		m_scale.Reset();
	}
	else
		CheckMeasureDirection(&x, &y);

	CRect rect(0, 0, m_intGridMarkerSize, m_intGridMarkerSize);
	m_pDC->DPtoLP(&rect);
	SIZE sz;
	sz.cx = abs(rect.Width()/2);
	sz.cy = abs(rect.Height()/2);

	Color color = (m_colorBK == COLOR_BLACK ? Color::White : Color::Black);
	Pen gpen(color);
	Graphics g(dc.m_hDC);
	g.SetRenderingOrigin(0, 0);

	ShowBufferBmp();

	CPoint p = PanToPos(CCoord(x, y), FALSE);
	if(IsGridVisible())
		g.DrawRectangle(&gpen, p.x - sz.cx, p.y - sz.cy, sz.cx * 2, sz.cy * 2);

}

void CGdsView::DrawRubberMemo(CPoint point)
{
	if(!m_bMouseInView || !m_bBufferScreen)
		return;

	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);

	dc.DPtoLP(&point);
	double x = (point.x / m_dblDrawScale - m_coordViewOffset.x);
	double y = (point.y / m_dblDrawScale - m_coordViewOffset.y); 
	RoundToUserGrid(&x, &y);

	if(m_bMemoDialog)
		m_pDlgMemo->OnApply();

	m_memo.SetPosition(CCoord(x, y));

	DrawMemo(NULL);

	m_memo.SetDragging(TRUE);
}

void CGdsView::DrawRubberScale(CPoint point)
{
	if(!m_bBufferScreen)
		return;
	if(m_bLockUpdate)
		return;
	if(!m_scale.IsMeasuring())
		return;

	CClientDC dc(this);
	OnPrepareDC(&dc);
    SetOrigin(&dc);

	CPoint p1, p2_new;
	CCoord c1, c2_new;
	c1 = m_scale.GetPoint1();
	p1 = PanToPos(c1, FALSE);

	dc.DPtoLP(&point);
	double x = (point.x / m_dblDrawScale - m_coordViewOffset.x);
	double y = (point.y / m_dblDrawScale - m_coordViewOffset.y); 
	RoundToUserGrid(&x, &y);
	CheckMeasureDirection(&x, &y);
	m_scale.SetPoint2(CCoord(x, y), FALSE);
	c2_new = m_scale.GetPoint2();
	p2_new = PanToPos(c2_new, FALSE);

	DrawScale(c1, c2_new);
	m_scale.SetDragging(TRUE);
}

void CGdsView::DrawScales(CDC* pDC)
{
	CPoints* ppoint = m_scale.OpenFirstPoints();
	while(ppoint){
		DrawScale(ppoint->p1, ppoint->p2, pDC);
		ppoint = m_scale.OpenNextPoints();
	}
}

void CGdsView::DrawScale(CCoord p1, CCoord p2, CDC* pDC)
{
	if(m_bLockUpdate)
		return;

	CClientDC dc(this);
	OnPrepareDC(&dc);
	SetOrigin(&dc);
	if(!pDC)
		pDC = &dc;

	CRect rect;
	GetClientRect(&rect);
	dc.DPtoLP(&rect);
	int font_h = (int)(abs(rect.Height()) * SCALE_TEXT_RATIO);
	int font_w = (int)(font_h * 0.5);

	COLORREF color = m_colorScale;
	if(m_intDrawMode == DRAW_MODE_PRINT){// Reverse BLACK<->WHITE @ print
		color = ReverseGray(color);

		CRect rect1;
		rect1.SetRect(0, 0, 1, 1);
		pDC->DPtoLP(&rect1);
		font_h *= rect1.Width();
		font_w *= rect1.Width();
	}

	double x = p2.x - p1.x;
	double y = p2.y - p1.y;
	double xdiff = fabs(x);
	double ydiff = fabs(y);
	double ang = atan2(y, x);
	BOOL dir_x = (fabs(x) > fabs(y) ? TRUE : FALSE);
	double len = sqrt(pow(xdiff, 2.0) + pow(ydiff, 2.0));
	len *= GetGDShcy()->GetDBUnit() * (1.0 - m_dblShrinkRate);
	len *= 1.0e+6; // convert to um

	double ang_mark = ang + PAI/2.0;
	double space = font_h * 1.0 * SCALE_GRID_RATIO;
	double fspace = space * 1.2;
	double markx = cos(ang_mark) * space;
	double marky = sin(ang_mark) * space;

	CString str;
	CCoord postext;
	postext.x = p1.x + cos(ang_mark) * fspace/m_dblDrawScale;
	postext.y = p1.y + sin(ang_mark) * fspace/m_dblDrawScale;
	CPoint pos = PanToPos(postext, FALSE);
	str = "0";

	Color bcolor;
	bcolor.SetFromCOLORREF(color);
	SolidBrush gbrush(bcolor);
	Pen gpen(bcolor, m_realPenWidth);
	REAL height = (REAL)font_h;
	FontFamily fontFamily(L"Courier New");
	Font gfont(&fontFamily, height, FontStyleBold, UnitPixel);
	PointF pointf;
	StringFormat format;
	GraphicsState state;
	USES_CONVERSION;

	Graphics g(pDC->m_hDC);
	g.SetRenderingOrigin(0, 0);

	pointf.X = (REAL)pos.x;
	pointf.Y = (REAL)pos.y;

	format.SetAlignment(StringAlignmentCenter);
	state = g.Save();
	g.TranslateTransform(pointf.X, pointf.Y, MatrixOrderPrepend);
	g.RotateTransform((REAL)(ang/PAI*180.0), MatrixOrderPrepend);
	g.ScaleTransform(1.0f, -1.0f, MatrixOrderPrepend);
	g.TranslateTransform(-pointf.X, -pointf.Y, MatrixOrderPrepend);
	pointf.Y -=  gfont.GetSize() * 
		(REAL)fontFamily.GetCellAscent(FontStyleBold) / fontFamily.GetEmHeight(FontStyleBold);
	g.DrawString(L"0", -1, &gfont, pointf, &format, &gbrush);
	g.Restore(state);

	postext.x = p2.x + cos(ang_mark) * fspace/m_dblDrawScale;
	postext.y = p2.y + sin(ang_mark) * fspace/m_dblDrawScale;

	if(len < 0.05)
		str.Format("%.3f", len * 1e+3);
	else
		str.Format("%.3f", len );
	str.TrimRight('0');
	str.TrimRight('.');
	if(len < 0.05)
		str += "nm";
	else
		str += "um";
	pos = PanToPos(postext, FALSE);

	CPoint point1, point2;
	point1 = PanToPos(p1, FALSE);
	point2 = PanToPos(p2, FALSE);

	LPSTR lpsz = new TCHAR[str.GetLength() + 1];
	_tcscpy(lpsz, str);
	WCHAR* wstr = A2W(lpsz);
	delete [] lpsz;
	pointf.X = (REAL)pos.x;
	pointf.Y = (REAL)pos.y;
	format.SetAlignment(StringAlignmentNear);
	state = g.Save();
	g.TranslateTransform(pointf.X, pointf.Y, MatrixOrderPrepend);
	g.RotateTransform((REAL)(ang/PAI*180.0), MatrixOrderPrepend);
	g.ScaleTransform(1.0f, -1.0f, MatrixOrderPrepend);
	g.TranslateTransform(-pointf.X, -pointf.Y, MatrixOrderPrepend);
	pointf.Y -=  gfont.GetSize() * 
		(REAL)fontFamily.GetCellAscent(FontStyleBold) / fontFamily.GetEmHeight(FontStyleBold);
	g.DrawString(wstr, -1, &gfont, pointf, &format, &gbrush);
	g.Restore(state);

	pointf.X = (REAL)point1.x;
	pointf.Y = (REAL)point1.y;
	PointF pointf2((REAL)point2.x, (REAL)point2.y);
	g.DrawLine(&gpen, pointf, pointf2);

	len = sqrt(pow(markx, 2.0) + pow(marky, 2.0));
	rect.left = rect.bottom = 0;
	rect.right = (long)len;
	rect.top = (long)len;
	pDC->LPtoDP(&rect);

	double step = (1.0e-9 / GetGDShcy()->GetDBUnit());

	double tmp = step * m_dblDrawScale;
	if(tmp < font_h){
		tmp = font_h/tmp;
		tmp = pow(10.0, (int)log10(tmp));
		step *= tmp;
	}

	double xstep = 5 * step * cos(ang);
	double ystep = 5 * step * sin(ang);

	int num;
	if(xstep != 0.0)
		num = (int)(xdiff/fabs(xstep));
	if(num == 0 && ystep != 0.0)
		num = (int)(ydiff/fabs(ystep));

	if(num >= 20){
		tmp = pow(10.0, (int)log10(num));
		num = (int)(num / tmp);
		xstep *= tmp;
		ystep *= tmp;
	}
	if(abs(rect.Width()) >= GRID_LIMIT){// && num > 0){
		int count;
		double xpos, ypos;
		BOOL draw_end = TRUE;
		if(num > 100){
			count = num - 100;
			xpos = p1.x + count * xstep;
			ypos = p1.y + count * ystep;
		}
		else{
			xpos = p1.x;
			ypos = p1.y;
			count = 0;
		}
		CPoint fm, to;
		for(;;xpos += xstep, ypos += ystep, count++){
			if(fabs(xpos - p1.x) > (xdiff + 1) || fabs(ypos - p1.y) > (ydiff + 1))
				break;
			if(fabs(xpos - p2.x) < 1 && fabs(ypos - p2.y) < 1)
				draw_end = FALSE;
			fm = PanToPos(CCoord(xpos, ypos), FALSE);
			switch(count % 2){
			case 0:
				to.x = (long)(fm.x + markx);
				to.y = (long)(fm.y + marky);
				break;
			default:
				to.x = (long)(fm.x + markx * 0.4);
				to.y = (long)(fm.y + marky * 0.4);
				break;
			}
			rect.TopLeft() = fm;
			rect.BottomRight() = to;
			rect.InflateRect(m_intDPInvisibleLP, m_intDPInvisibleLP);
			if(IsOutOfView(&dc, rect))
				continue;
			pointf.X = (REAL)fm.x;
			pointf.Y = (REAL)fm.y;
			PointF pointf2((REAL)to.x, (REAL)to.y);
			g.DrawLine(&gpen, pointf, pointf2);
		}
		if(draw_end){
			fm = PanToPos(p2, FALSE);
			to.x = (long)(fm.x + markx);
			to.y = (long)(fm.y + marky);
			pointf.X = (REAL)fm.x;
			pointf.Y = (REAL)fm.y;
			PointF pointf2((REAL)to.x, (REAL)to.y);
			g.DrawLine(&gpen, pointf, pointf2);
		}
	}
}

void CGdsView::DrawMemos(CDC* pDC)
{
	CMemoElement* ptr = m_memo.OpenFirstMemo();
	while(ptr){
		DrawMemo(ptr, pDC);
		ptr = m_memo.OpenNextMemo();
	}
}

void CGdsView::DrawMemo(CMemoElement* ptr, CDC* pDC)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	SetOrigin(&dc);
	if(!pDC)
		pDC = &dc;

	CMemoElement melem;
	if(!ptr){
		melem.logfont = *m_memo.GetFont();
		melem.pos = m_memo.GetPosition();
		melem.memo = m_memo.GetMemoStr();
		ptr = &melem;
	}

	double font_ratio = 30.0;
	CRect rect;
	if(m_intDrawMode != DRAW_MODE_DISPLAY){
		rect.SetRect(0, 0, 1, 1);
		pDC->DPtoLP(&rect);
		font_ratio *= rect.Width();
	}

	COLORREF color = m_colorMemo;
	if(m_intDrawMode == DRAW_MODE_PRINT) // Reverse BLACK<->WHITE @ print
		color = ReverseGray(m_colorMemo);

	CCoord postext;
	postext.x = ptr->pos.x;
	postext.y = ptr->pos.y;
	CPoint point = PanToPos(postext, FALSE);
	rect.TopLeft() = CPoint(0, 0);
	rect.BottomRight() = CPoint(1, 1);
	pDC->DPtoLP(&rect);
	rect.NormalizeRect();
	LOGFONT lf = ptr->logfont;

	if(ptr->memo.GetLength() == 0)
		return;

	USES_CONVERSION;
	LPTSTR lpsz = new TCHAR[ptr->memo.GetLength() + 1];
	_tcscpy(lpsz, ptr->memo);
	WCHAR* pwstr = A2W(lpsz);
	delete [] lpsz;

	lpsz = new TCHAR[strlen(lf.lfFaceName) + 1];
	_tcscpy(lpsz, lf.lfFaceName);
	WCHAR* pface = A2W(lpsz);
	delete [] lpsz;

	REAL height = (REAL)fabs(font_ratio * lf.lfHeight);
	PointF pointf((REAL)point.x, (REAL)point.y);

	Color bcolor;
	bcolor.SetFromCOLORREF(color);
	SolidBrush brush(bcolor);
	FontFamily fontFamily(pface);
	Font font(&fontFamily, height, FontStyleBold, UnitPixel);
	StringFormat format;
	format.SetAlignment(StringAlignmentNear);
	Graphics g(pDC->m_hDC);
	GraphicsState state = g.Save();
	g.TranslateTransform(pointf.X, pointf.Y, MatrixOrderPrepend);
	g.ScaleTransform(1.0f, -1.0f, MatrixOrderPrepend);
	g.TranslateTransform(-pointf.X, -pointf.Y, MatrixOrderPrepend);
	pointf.Y -= font.GetSize() * 
		(REAL)fontFamily.GetCellAscent(FontStyleBold) / fontFamily.GetEmHeight(FontStyleBold);
	g.DrawString(pwstr, -1, &font, pointf, &format, &brush); 
	g.Restore(state);
}

void CGdsView::DrawComboBox(BOOL on)
{
	if(on){
		m_pMainFrame->HideDummyComboBoxes();
		m_cmbboxStructure.ShowWindow(SW_SHOW);
		m_cmbboxStructure.BringWindowToTop();
		m_cmbboxStructureLvl.ShowWindow(SW_SHOW);
		m_cmbboxStructureLvl.BringWindowToTop();
		OnUpdateStructureCb(NULL);
		OnUpdateStrLevelCb(NULL);
	}
	else{
		m_cmbboxStructure.ShowWindow(SW_HIDE);
		m_cmbboxStructureLvl.ShowWindow(SW_HIDE);
	}
}

void CGdsView::AbortMeasuring(BOOL del)
{
	if(del){
		DrawScales();
		m_scale.DeleteAll();
		InvalidateRect(FALSE, NULL);
	}
	if(m_scale.IsDragging()){
		m_scale.Reset();
		m_scale.SetDragging(FALSE);
		SetCommandGuide(CMD_GUIDE_MEASURE_P1);
		DrawGridMarker();
	}
}

static VOID CALLBACK TrackMouseLeave(HWND hWnd,UINT uMsg,UINT idEvent,DWORD dwTime)
{
	RECT rect;
	POINT pt;

	GetClientRect(hWnd,&rect);
	MapWindowPoints(hWnd,NULL,(LPPOINT)&rect,2);
	GetCursorPos(&pt);
	if((!PtInRect(&rect,pt) || (WindowFromPoint(pt) != hWnd))){
		 PostMessage(hWnd,WM_MOUSELEAVE,0,0);
	}

}


static BOOL TrackMouseEvent(LPTRACKMOUSEEVENT ptme)
{
    if(!ptme || ptme->cbSize < sizeof(TRACKMOUSEEVENT))
		return FALSE;

	if(!IsWindow(ptme->hwndTrack))
		return FALSE;

	if(ptme->dwFlags & TME_LEAVE) {
		return SetTimer(ptme->hwndTrack, ptme->dwFlags,
					  100,(TIMERPROC)TrackMouseLeave);
	}
	return FALSE;
}

BOOL CGdsView::PreTranslateMessage(MSG* pMsg) 
{
	TRACKMOUSEEVENT tme;

	if(GetFocus() == this)
		m_toolTipCtrl.RelayEvent(pMsg);
    switch (pMsg->message){
	case WM_USER_END_RENDER:
		if(!m_bAbortDraw)
			OnEndRender();
		break;
	case WM_MOUSELEAVE:
		if(m_bMouseInView){
			DeleteCursor();
			ShowBufferBmp();
			m_pMainFrame->SetStatusBarXY(0, 0, FALSE);
			m_bMouseInView = FALSE;
		}
		break;
	case WM_MOUSEMOVE:
		if(!m_bMouseInView && GetFocus() == this){
			m_bMouseInView = TRUE;
			ShowBufferBmp();
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = m_hWnd;
			if(!TrackMouseEvent(&tme)) {
				//::MessageBox(m_hWnd, TEXT("TrackMouseEvent Failed"), TEXT("Mouse Leave"),MB_OK);
			}
		}
		break;
	default:
        break;
	}
	return CScrollView::PreTranslateMessage(pMsg);
}

void CGdsView::OnTimer(UINT nIDEvent) 
{
	CScrollView::OnTimer(nIDEvent);
}

void CGdsView::DecrementScale()
{
	CPoints point;
	if(m_scale.DeleteLast(&point))
		InvalidateRect(FALSE, NULL);
}

void CGdsView::MoveViewOffset(double x, double y)
{
	m_coordViewOffset.x += x;
	m_coordViewOffset.y += y;

	double modx = modf(m_coordViewOffset.x, &m_coordViewOffset.x);
	double mody = modf(m_coordViewOffset.y, &m_coordViewOffset.y);

	if(modx >= 0.5)
		m_coordViewOffset.x += 1.0;
	else if(modx <= -0.5)
		m_coordViewOffset.x -= 1.0;
	m_coordViewOffset.x = (double)((long)m_coordViewOffset.x);
	if(mody >= 0.5)
		m_coordViewOffset.y += 1.0;
	else if(mody <= -0.5)
		m_coordViewOffset.y -= 1.0;
	m_coordViewOffset.y = (double)((long)m_coordViewOffset.y);

//	RoundToUserGrid(&m_coordViewOffset.x, &m_coordViewOffset.y);*/
}

void CGdsView::OnDeleteScale() 
{
	GetAsyncKeyState(VK_SHIFT);
	if(GetAsyncKeyState(VK_SHIFT))
		AbortMeasuring(TRUE);
	else
		DecrementScale();
}

void CGdsView::OnUpdateDeleteScale(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_scale.GetNumScales() > 0 ? TRUE : FALSE);
}

void CGdsView::OnEndRender()
{
	m_uintDrawingAREF = 0;

	if(m_bInitView){
		m_bInitView = FALSE;
		SetRedraw(TRUE);
		InvalidateRect(TRUE, NULL);
	}
	else
		m_pMainFrame->ActivateToolTip(TRUE);

	if(m_bSkelton && !m_bSkeltonSorted){
		SortSkelton();
		m_bSkeltonSorted = TRUE;
	}
	if(m_bUpdateDrawTime)
		m_pMainFrame->SetStatusBarDrawTime((double)(clock() - m_clocktStart));
	ChangeEditMode(m_intEditMode);

	if(!m_bAbortDraw){
		m_bmpScreen.DeleteObject();
		MakeBufferBmp();
		ShowBufferBmp();
		DrawGridMarker();
	}
	else
		m_bBufferScreen = FALSE;
	m_bForceRedraw = FALSE;
	m_bEverythingInView = FALSE;

	if(m_pMainFrame->GetSyncView())
		OnSyncView();
}

void CGdsView::OnButtonStopDraw() 
{
	if(!m_bCancelDraw || m_bInitView)
		return;
	m_bAbortDraw = TRUE;
	m_bLockUpdate = FALSE;
	if(m_pThreadRender && m_intDrawMode == DRAW_MODE_DISPLAY){
		if(WaitForSingleObject(m_pThreadRender->m_hEventStop, 2000) == WAIT_TIMEOUT)//INFINITE);
            m_bCancelDraw = FALSE;
		ResetEvent(m_pThreadRender->m_hEventStop);
	}
	m_pMainFrame->ActivateToolTip(TRUE);
}

void CGdsView::OnCycleDirection() 
{
	switch(m_scale.GetScaleDirection()){
	case MEAS_DIR_XY:
		m_scale.SetScaleDirection(MEAS_DIR_DIAGONAL);
		break;
	case MEAS_DIR_DIAGONAL:
		m_scale.SetScaleDirection(MEAS_DIR_ANY);
		break;
	case MEAS_DIR_ANY:
		m_scale.SetScaleDirection(MEAS_DIR_XY);
		break;
	default:
		break;
	}
	if(m_bShowGridDlg)
		m_pDlgGrid->SetDirection(m_scale.GetScaleDirection());
	SetCommandGuide(CMD_GUIDE_UPDATE);
}

void CGdsView::SetCommandGuide(int id)
{
	CString message;
	int idx;
	switch(id){
	case CMD_GUIDE_UPDATE:
		message = m_pMainFrame->GetStatusBarMessage();
		if((idx = message.Find("G:")) >= 0){
			message = message.Left(idx);
			break;
		}
	case CMD_GUIDE_NONE:
		message = "Mid click to pan, Right drag to Zoom";
		break;
	case CMD_GUIDE_MEASURE_P1:
		message = "Left click first point";
		break;
	case CMD_GUIDE_MEASURE_P2:
		message = "Left click second point";
		break;
	case CMD_GUIDE_ZOOM:
		message = "Left drag new view area";
		break;
	case CMD_GUIDE_MEMO:
		message = "Memo";
		break;
	case CMD_GUIDE_STOP_DRAW:
		message = "Press Ctrl+C to abort drawing";
		break;
	default:
		message = "";
		break;
	}
	message.TrimRight(' ');
	int len = 40 - message.GetLength();
	while(--len)
		message += " ";
	switch(m_pGrid->GetGridType()){
	case GRID_TYPE_NONE:
		message += "G:None";
		break;
	case GRID_TYPE_DOT:
		message += "G:Dot ";
		break;
	case GRID_TYPE_LINE:
		message += "G:Line";
		break;
	default:
		break;
	}
	switch(m_scale.GetScaleDirection()){
	case MEAS_DIR_DIAGONAL:
		message += " D:45dgr";
		break;
	case MEAS_DIR_XY:
		message += " D:90dgr";
		break;
	case MEAS_DIR_ANY:
		message += " D:Any  ";
		break;
	default:
		break;
	}

	m_pMainFrame->SetStatusBarMessage(message);
}

void CGdsView::OnMemoSwitch() 
{
	if(m_intEditMode != EDIT_MODE_MEMO){
		ChangeEditMode(EDIT_MODE_MEMO);
	}
	else{
		ChangeEditMode(EDIT_MODE_NONE);
	}
	m_memo.Reset();
}

void CGdsView::OnUpdateMemoSwitch(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_intEditMode == EDIT_MODE_MEMO ? TRUE : FALSE);
}

void CGdsView::OnDeleteMemo() 
{
	GetAsyncKeyState(VK_SHIFT);
	if(GetAsyncKeyState(VK_SHIFT)){
		DrawMemos();
		m_memo.DeleteAll();
	}
	else{
		CMemoElement del;
		m_memo.DeleteLast(&del);
	}
	InvalidateRect(FALSE, NULL);
}

void CGdsView::OnUpdateDeleteMemo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_memo.GetNumMemos() > 0 ? TRUE : FALSE);
}

void CGdsView::ChangeEditMode(unsigned newmode)
{
	if(newmode != m_intEditMode){
		ExitModeDialogs();

		switch(m_intEditMode){
		case EDIT_MODE_NONE:
			break;
		case EDIT_MODE_ZOOM:
			break;
		case EDIT_MODE_MEASURE:
			m_scale.SetDragging(FALSE);
			m_scale.Reset();
			break;
		case EDIT_MODE_MEMO:
			DrawRubberMemo(CPoint(0, 0));
			m_memo.SetDragging(FALSE);
			m_memo.Reset();
			break;
		default:
			break;
		}
	}
	switch(newmode){
	case EDIT_MODE_NONE:
		SetCommandGuide(CMD_GUIDE_NONE);
		break;
	case EDIT_MODE_ZOOM:
		SetCommandGuide(CMD_GUIDE_ZOOM);
		break;
	case EDIT_MODE_MEASURE:
		SetCommandGuide(CMD_GUIDE_MEASURE_P1);
		break;
	case EDIT_MODE_MEMO:
		SetCommandGuide(CMD_GUIDE_MEMO);
		break;
	default:
		break;
	}
	if(m_intEditMode != newmode){
		m_intEditMode = newmode;
		CreateModelessDialogs();
	}
}

void CGdsView::ExitModeDialogs(BOOL desctractor)
{
	if(m_bMemoDialog){
		m_pDlgMemo->ExitDialog();
		DrawGridMarker();
	}
	m_memo.Reset();
}

void CGdsView::CreateModelessDialogs()
{
	switch(m_intEditMode){
	case EDIT_MODE_MEMO:
		m_pDlgMemo = new CMemoDlg(this);
		m_pDlgMemo->Create(IDD_MEMO_DLG, this);
		m_pDlgMemo->SetEnabler(&m_bMemoDialog);
		m_bMemoDialog = TRUE;
		break;
	case EDIT_MODE_NONE:
	case EDIT_MODE_ZOOM:
	case EDIT_MODE_MEASURE:
		break;
	default:
		break;
	}
}


void CGdsView::OnBackgroundBlack() 
{
	if(m_colorBK != COLOR_BLACK){
		m_colorBK = COLOR_BLACK;
 		CWinApp* app = AfxGetApp();
		app->WriteProfileInt("Color", "Background", m_colorBK);
		InvalidateRect(TRUE, NULL);
	}
}

void CGdsView::OnUpdateBackgroundBlack(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_colorBK == COLOR_BLACK ? TRUE : FALSE);
}

void CGdsView::OnBackgroundWhite() 
{
	if(m_colorBK != COLOR_WHITE){
		m_colorBK = COLOR_WHITE;
 		CWinApp* app = AfxGetApp();
		app->WriteProfileInt("Color", "Background", m_colorBK);
		InvalidateRect(TRUE, NULL);
	}
}

void CGdsView::OnUpdateBackgroundWhite(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_colorBK == COLOR_WHITE ? TRUE : FALSE);
}

BOOL CGdsView::IsOutOfView(CDC* pDC, CRect rect)
{
    if(m_bInitView)
		return FALSE;

	rect.NormalizeRect();
	return (m_rectClip & rect).IsRectEmpty();
}

BOOL CGdsView::IsOutOfView(CRect rect)
{
	if(m_bInitView || m_bEverythingInView)
		return FALSE; 

	if(m_intDrawMode == DRAW_MODE_DISPLAY){
		m_pDC->LPtoDP(&rect);
		rect.NormalizeRect();
		return !m_rgnUpdate.RectInRegion(&rect);
	}
	else{
		rect.NormalizeRect();
		return (m_rectClip & rect).IsRectEmpty();
	}
}

void CGdsView::MakeBufferBmp()
{
	m_bmpScreen.DeleteObject();
	m_bBufferScreen = TRUE;
	WriteToBmp(&m_bmpScreen);
}

void CGdsView::WriteToBmp(CBitmap* bmp)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	SetOrigin(&dc);

	CRect rect;
	GetClientRect(&rect);
	CRect lrect = rect;
	dc.DPtoLP(&lrect);	
	
	DeleteCursor();
	CDC cpDC; 
	cpDC.CreateCompatibleDC(&dc);
	bmp->CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	cpDC.SelectObject(bmp);
	cpDC.SetBkColor(m_colorBK);
	cpDC.SetBkMode(TRANSPARENT);
	cpDC.FillSolidRect(&rect, m_colorBK);
	cpDC.StretchBlt(0, 0, rect.Width(), rect.Height(), &dc, 
		lrect.left, lrect.top, lrect.Width(), lrect.Height(), SRCCOPY);
	DrawCursor();
	DrawGridMarker();
}

void CGdsView::ShowBufferBmp()
{
	if(!m_bBufferScreen)
		return;

	CClientDC dc(this);
	OnPrepareDC(&dc);
	SetOrigin(&dc);

	CRect rect;
	GetClientRect(&rect);
	CRect lrect = rect;
	dc.DPtoLP(&lrect);	
	
	DeleteCursor();
	CDC cpDC; 
	cpDC.CreateCompatibleDC(&dc);
	cpDC.SelectObject(&m_bmpScreen);
	cpDC.SetBkColor(m_colorBK);
	cpDC.SetBkMode(TRANSPARENT);

	dc.StretchBlt(lrect.left, lrect.top, lrect.Width(), lrect.Height(), &cpDC, 
		0, 0, rect.Width(), rect.Height(), SRCCOPY);
	DrawCursor();
}

int CGdsView::GetROP(CDC* pDC)
{
	if(m_bOverWrite)
		return R2_COPYPEN;

	if(m_intDrawMode == DRAW_MODE_PRINT)
		return R2_MERGEPENNOT;

    if(m_colorBK == COLOR_BLACK)
        return R2_MERGEPEN;
    else
        return R2_MERGEPENNOT;

}


void CGdsView::OnShowAref() 
{
	if(m_bSkelton)
		return;
	m_bShowAREF ^= TRUE;

	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Show AREF", m_bShowAREF ? 1 : 0);

	if(!m_bShowAREF)
		InvalidateRect(TRUE, NULL);
}

void CGdsView::OnUpdateShowAref(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowAREF);
}

void CGdsView::OnAxisNone() 
{
	if(m_intAxisAt != AXIS_AT_NONE)
		DrawAxis(FALSE);
	m_intAxisAt = AXIS_AT_NONE;
	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Axis on center", 0);
}


void CGdsView::OnUpdateAxisNone(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_intAxisAt == AXIS_AT_NONE ? TRUE : FALSE);
}

void CGdsView::OnAxisStructureOrigin() 
{
	BOOL redraw = FALSE;
	if(m_intAxisAt != AXIS_AT_STRUCTURE_ORIGIN){
		DrawAxis(FALSE);
		redraw = TRUE;
	}
	m_intAxisAt = AXIS_AT_STRUCTURE_ORIGIN;
	if(redraw)
		DrawAxis();
	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Axis on center", AXIS_AT_STRUCTURE_ORIGIN);
}

void CGdsView::OnUpdateAxisStructureOrigin(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_intAxisAt == AXIS_AT_STRUCTURE_ORIGIN ? TRUE : FALSE);
}

void CGdsView::OnAxisWindowCenter() 
{
	BOOL redraw = FALSE;
	if(m_intAxisAt != AXIS_AT_WINDOW_CENTER){
		DrawAxis(FALSE);
		redraw = TRUE;
	}
	m_intAxisAt = AXIS_AT_WINDOW_CENTER;
	if(redraw)
		DrawAxis();
	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Misc", "Axis on center", AXIS_AT_WINDOW_CENTER);
}

void CGdsView::OnUpdateAxisWindowCenter(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_intAxisAt == AXIS_AT_WINDOW_CENTER ? TRUE : FALSE);
}

int  CGdsView::GetLayerStatus(int layer)
{
	return *(m_bShowList + layer);
}

void CGdsView::SetLayerStatus(int layer, int value)
{
	if(value == TRUE || value == FALSE)
		*(m_bShowList + layer) = value;
}

void CGdsView::RecalcNumActiveLayer()
{
	m_intNumActiveLayer = 0;
	int numlayer = 0;
	for(int i = 0;i < sizeof(m_bShowList)/sizeof(int);i++){
		if(GetGDShcy()->DoesLayerExist(i))
			m_intNumActiveLayer++;
		if(*(m_bShowList + i))
			numlayer++;
		COLORREF color = *(m_aryColorLayerRGB + i);
		(m_aryColorLayer + i)->SetFromCOLORREF(color);
	}
}

void CGdsView::OnForceSkeltonOn() 
{
	if(m_pThreadRender && !m_pThreadRender->WaitUntilDrawn())
		return;
	if(m_bSkelton)
		return;
	StopDraw();

	m_bSkelton = TRUE;
	//m_cmbboxStructureLvl.EnableWindow(FALSE);
	ChangeEditMode(EDIT_MODE_NONE);
	ChangeStructure(NULL, FALSE);//TRUE);
	for(int i = 0;i < m_arrayHitTest.GetSize();i++){
		CStrFrame* pframe = (CStrFrame*)m_arrayHitTest.GetAt(i);
		pframe->SetExpand(FALSE);
	}
}

void CGdsView::OnForceSkeltonOff() 
{
	if(!m_bSkelton)
		return;

	StopDraw();

	m_bSkelton = FALSE;
	m_cmbboxStructureLvl.EnableWindow(TRUE);
	if(m_bStructureInfoDlg)
		m_pDlgStructureInfo->ExitDialog();
	ChangeEditMode(EDIT_MODE_NONE);
	ChangeStructure(NULL, FALSE);
}

void CGdsView::OnSkelton() 
{
	OnButtonStopDraw();

	InvalidateRect(TRUE, NULL);
	if(m_bSkelton)
		OnForceSkeltonOff();
	else
		OnForceSkeltonOn();
}

void CGdsView::OnUpdateSkelton(CCmdUI* pCmdUI) 
{
	BOOL enable = TRUE;

	if(fmod(m_dblViewRotation, 90.0) != 0)
		enable = FALSE;
	pCmdUI->Enable(enable);
	pCmdUI->SetCheck(m_bSkelton);
}

void CGdsView::OnUpdateStructureLevel(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bSkelton);
}

void CGdsView::OnOmitSmallStructure() 
{
	m_bOmitSmallStructure ^= TRUE;
	AfxGetApp()->WriteProfileInt("Misc", "Omit small", m_bOmitSmallStructure ? 1 : 0);
}

void CGdsView::OnUpdateSetAngle(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bSkelton && !m_pMainFrame->GetSyncView());
}

CGDSStructure* CGdsView::PopHistory()
{
	LockWindowUpdate();
	if(m_arrayStructureHist.GetSize() <= 0){
		UnlockWindowUpdate();
		return NULL;
	}
	int pos = m_arrayStructureHist.GetSize() - 1;
	CPointer* p = (CPointer*)m_arrayStructureHist.GetAt(pos);
	CGDSStructure* str = (CGDSStructure*)p->GetPointer();
	delete p;
	m_arrayStructureHist.RemoveAt(pos);
	UnlockWindowUpdate();
	return str;
}

void CGdsView::AddHistory()
{ 
	LockWindowUpdate();
	CPointer* p = new CPointer;
	p->SetPointer((DWORD)m_pStrActive);
	m_arrayStructureHist.Add(p);
	UnlockWindowUpdate();
}

void CGdsView::ClearHistory()
{ 
	int size = m_arrayStructureHist.GetSize();
	for(int i = 0;i < size;i++){
		CPointer* p = (CPointer*)m_arrayStructureHist.GetAt(i);
		delete p;
	}
	m_arrayStructureHist.RemoveAll();
}
 
void CGdsView::ClearHitTestArray()
{
	int size = m_arrayHitTest.GetSize();
	for(int i = 0;i < size;i++){
		CStrFrame* pframe = (CStrFrame*)m_arrayHitTest.GetAt(i);
		delete pframe;
	}
	m_arrayHitTest.RemoveAll();
	m_pFrameTemp = NULL;
	m_pFrameActive = NULL;
	m_bSelectAllCell = FALSE;
	m_strToolTip = "";
}


BOOL CGdsView::OnNotifyNeedText(UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;

	if(!m_bSkelton)
		return TRUE;

	if(!m_pFrameTemp)
		return TRUE;


    if(pTTT->uFlags & TTF_IDISHWND){
		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);

		CClientDC dc(this);
		OnPrepareDC(&dc);
		SetOrigin(&dc);

		m_pToolTip = point;
		dc.DPtoLP(&point);

		pTTT->lpszText = m_strToolTip.GetBuffer(0);
		return TRUE;
    }
    return(FALSE);
}

void CGdsView::OnButtonBack() 
{
	if(m_bInitView)
		return;
	StopDraw();
	if(m_pThreadRender && !m_pThreadRender->WaitUntilDrawn())
		return;
	ChangeStructure(PopHistory(), TRUE);
}

void CGdsView::OnUpdateButtonBack(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!IsDrawing() && m_arrayStructureHist.GetSize() > 0);	
}

void CGdsView::ChangeStructure(CGDSStructure *str, BOOL init, BOOL fromDlg)
{
	StopDraw();
	m_bAbortDraw = TRUE;
	if(str && m_pStrActive != str){
		GetViewArea();
		m_pStrActive = str;
		m_cmbboxStructure.SetCurSel(m_pStrActive->GetStructureName());
		if(!fromDlg && m_bStructureDialog){
			m_pDlgStructure->SetActive(m_pStrActive->GetStructureName());
			m_pDlgStructure->SelectActive();
		}
		if(m_bStructureInfoDlg)
			m_pDlgStructureInfo->ClearText();
		ClearHitTestArray();
		init = TRUE;
		m_bSkeltonSorted = FALSE;
	}
	if(init){
		m_bInitView = TRUE;
		InvalidateRect(FALSE, NULL);
	}
	m_bForceRedraw = TRUE;
}

void CGdsView::WriteAllCellInfo(CFile &file)
{
	int num = m_arrayHitTest.GetSize();
	CString buffer;
	buffer.Format("Number of SREF and AREF : %d", num);
	buffer += NEWLINE;
	buffer += NEWLINE;
	file.Write(buffer, buffer.GetLength());

	for(int i = 0;i < num;i++){
		CStrFrame* pFrame = (CStrFrame*)m_arrayHitTest.GetAt(i);
		WriteStructureInfo(file, pFrame);
		buffer = NEWLINE;
		file.Write(buffer, buffer.GetLength());
	}
}

void CGdsView::WriteStructureInfo(CFile &file, CStrFrame* frame)
{
	if(!frame){
		if(m_bSelectAllCell){
			WriteAllCellInfo(file);
			return;
		}

		if(!m_pFrameActive)
			return;
		frame = m_pFrameActive;
	}
	CGDSStructure* pStr = frame->GetStructure();

	CString buffer;

	buffer = "Type      : ";
	if(frame->IsSref())
		buffer += "SREF";
	else
		buffer += "AREF";
	buffer += NEWLINE;
	file.Write(buffer, buffer.GetLength());

	buffer.Format("Name      : %s", pStr->GetStructureName());
	buffer += NEWLINE;
	file.Write(buffer, buffer.GetLength());

	CGDSElement* pElement = frame->GetElement();
	CCoord coord ;
	pElement->GetNthCoord(0, &coord);

    coord.x *= GetGDShcy()->GetDBUnit() * (1.0 - m_dblShrinkRate);
    coord.y *= GetGDShcy()->GetDBUnit() * (1.0 - m_dblShrinkRate);
	coord.x *= 1e+6; // convert to um
	coord.y *= 1e+6;

	CString strx, stry;

	strx.Format("%.3f", coord.x);
	strx.TrimRight('0');
	strx.TrimRight('.');
	stry.Format("%.3f", coord.y);
	stry.TrimRight('0');
	stry.TrimRight('.');

	buffer.Format("Coord     : X=%s um, Y=%s um", strx, stry);
	buffer += NEWLINE;
	file.Write(buffer, buffer.GetLength());


	if(!frame->IsSref()){
		pElement->GetNthCoord(1, &coord);

		coord.x *= GetGDShcy()->GetDBUnit() * (1.0 - m_dblShrinkRate);
		coord.y *= GetGDShcy()->GetDBUnit() * (1.0 - m_dblShrinkRate);
		coord.x *= 1e+6; // convert to um
		coord.y *= 1e+6;

		strx.Format("%.3f", coord.x);
		strx.TrimRight('0');
		strx.TrimRight('.');
		stry.Format("%.3f", coord.y);
		stry.TrimRight('0');
		stry.TrimRight('.');

		buffer.Format("Corner1   : X=%s um, Y=%s um", strx, stry);
		buffer += NEWLINE;
		file.Write(buffer, buffer.GetLength());

		pElement->GetNthCoord(2, &coord);

		coord.x *= GetGDShcy()->GetDBUnit() * (1.0 - m_dblShrinkRate);
		coord.y *= GetGDShcy()->GetDBUnit() * (1.0 - m_dblShrinkRate);
		coord.x *= 1e+6; // convert to um
		coord.y *= 1e+6;

		strx.Format("%.3f", coord.x);
		strx.TrimRight('0');
		strx.TrimRight('.');
		stry.Format("%.3f", coord.y);
		stry.TrimRight('0');
		stry.TrimRight('.');

		buffer.Format("Corner2   : X=%s um, Y=%s um", strx, stry);
		buffer += NEWLINE;
		file.Write(buffer, buffer.GetLength());

		int col = ((CGDSAREF*)pElement)->GetNumCol();
		int row = ((CGDSAREF*)pElement)->GetNumRow();
		buffer.Format("Col x Row : %d x %d = %d", col, row, col * row);
		buffer += NEWLINE;
		file.Write(buffer, buffer.GetLength());
	}

	buffer = "Reflected?: ";
	if(((CGDSSREF*)pElement)->IsReflect())
		buffer += "Yes";
	else
		buffer += "No";
	buffer += NEWLINE;
	file.Write(buffer, buffer.GetLength());

	strx.Format("%.3f", ((CGDSSREF*)pElement)->GetAngle());
	strx.TrimRight('0');
	strx.TrimRight('.');
	buffer.Format("Angle     : %s dgr", strx);
	buffer += NEWLINE;
	file.Write(buffer, buffer.GetLength());

	strx.Format("%.3f", ((CGDSSREF*)pElement)->GetMag());
	strx.TrimRight('0');
	strx.TrimRight('.');
	buffer.Format("Mag.      : %s", strx);
	buffer += NEWLINE;
	file.Write(buffer, buffer.GetLength());


	CCoord pArea[2];
	frame->GetArea(pArea);
	double width = fabs(pArea[0].x - pArea[1].x);
	double height = fabs(pArea[0].y - pArea[1].y);
    width *= GetGDShcy()->GetDBUnit() * (1.0 - m_dblShrinkRate);
	height *= GetGDShcy()->GetDBUnit() * (1.0 - m_dblShrinkRate);
	width *= 1e+6; // convert to um
	height *= 1e+6;

	strx.Format("%.3f", width);
	strx.TrimRight('0');
	strx.TrimRight('.');
	stry.Format("%.3f", height);
	stry.TrimRight('0');
	stry.TrimRight('.');

	buffer.Format("Size      : X=%s um, Y=%s um", strx, stry);
	buffer += NEWLINE;
	file.Write(buffer, buffer.GetLength());
}




void CGdsView::OnSelectAllCell() 
{
	if(!m_bSkelton)
		return;

	if(!m_bSelectAllCell){
		m_bSelectAllCell = TRUE;	
		m_pFrameActive = m_pFrameTemp = NULL;
		CClientDC dc(this);
		OnPrepareDC(&dc);
		SetOrigin(&dc);
		DrawSkelton(&dc, FALSE);
	}
	if(m_bStructureInfoDlg)
		m_pDlgStructureInfo->SetInformation();
}



void CGdsView::OnExpandCell() 
{
	ExpandCell();	
}

BOOL CGdsView::ExpandCell()
{
	if(!m_pFrameActive)
		return FALSE;

	m_toolTipCtrl.Pop();

	if(m_pFrameActive->GetExpand())
		m_pFrameActive->SetExpand(FALSE);
	else
		m_pFrameActive->SetExpand(TRUE);
	CRect rect;
	CalcStructureArea(m_pFrameActive, rect);
	CClientDC dc(this);
	OnPrepareDC(&dc);
	SetOrigin(&dc);
	dc.LPtoDP(&rect);
	rect.NormalizeRect();

	DeleteCursor();
	InvalidateRect(TRUE, rect, FALSE);
	return TRUE;
}

void CGdsView::DrawSingleStructure(CStrFrame *pFrame)
{
	m_pMainFrame->ActivateToolTip(FALSE);
	CClientDC dc(this);
	OnPrepareDC(&dc);
	SetOrigin(&dc);

	DeleteCursor();
	int rop = dc.SetROP2(GetROP(&dc));
	double angle;
	switch(m_intMirror){
	case MIRROR_NONE:
		angle = m_dblViewRotation;
		break;
	case MIRROR_TO_X:
		angle = -m_dblViewRotation;
		break;
	case MIRROR_TO_Y:
		angle = -m_dblViewRotation + 180.0;
		break;
	}
	CCoord pos = m_coordViewOffset;
	BOOL reflect = FALSE;
	double mag = 1.0;

	CDC* tmpDC = m_pDC;
	m_pDC = &dc;
	m_bSkelton = FALSE;


	CGDSStructure* pStructure = pFrame->GetStructure();
	if(pFrame->IsSref()){
		CGDSSREF* sref = (CGDSSREF*)pFrame->GetElement();
		CCoord offset;
		sref->GetPos(&offset);
		RotatePoint(&offset, angle);
		pos += offset;
		angle += sref->GetAngle();
		mag *= sref->GetMag();
		reflect = sref->IsReflect();
		m_intLayerDraw = -1;

		if(m_bDrawByLayer){
			for(unsigned i = 0;i < GDS_MAX_LAYER;i++){
				m_intLayerDraw = *(m_aryIntLayerOrder + i);
				if(m_intLayerDraw < 0)
					break;
				if(GetLayerStatus(m_intLayerDraw) > 0 && pStructure->IsVisibleOnLayer(m_intLayerDraw))
					DrawStructure(0, pStructure, pos, reflect, angle, mag);
			}
		}
		else
			DrawStructure(0, pStructure, pos, reflect, angle, mag);
	}
	else{
		m_intLayerDraw = -1;
		if(m_bDrawByLayer){
			for(unsigned i = 0;i < GDS_MAX_LAYER;i++){
				m_intLayerDraw = *(m_aryIntLayerOrder + i);
				if(m_intLayerDraw < 0)
					break;
				if(GetLayerStatus(m_intLayerDraw) > 0)
					DrawAref(0, (CGDSAREF*)pFrame->GetElement(), pos, reflect, angle, mag);
			}
		}
		else
			DrawAref(0, (CGDSAREF*)pFrame->GetElement(), pos, reflect, angle, mag);
	}
	
	dc.SetROP2(rop);
	m_pDC = tmpDC;
	m_bSkelton = TRUE;
	DrawCursor();
	m_pMainFrame->ActivateToolTip(TRUE);
}

void CGdsView::CreateRenderThread()
{
#ifdef PROFILE
	return;
#endif
	if(!m_pThreadRender){
		m_pThreadRender = new CMTRender(m_hWnd, m_pScreenDC->m_hDC);
		m_pThreadRender->m_pThreadParams = NULL;
		m_pThreadRender->CreateThread(CREATE_SUSPENDED);
		m_pThreadRender->SetThreadPriority(THREAD_PRIORITY_IDLE);
		m_pThreadRender->ResumeThread();
	}
}

void CGdsView::KillRenderThread()
{
	OnButtonStopDraw();
	if(m_pThreadRender){
		m_pThreadRender->KillThread();
		if(m_pThreadRender)
			WaitForSingleObject(m_pThreadRender->m_hEventDead, INFINITE);
	}
	m_pThreadRender = NULL;
}

void CGdsView::InvalidateRect(BOOL stop_draw, LPCRECT lpRect, BOOL bErase)
{
	if(stop_draw || m_bForceRedraw)
		StopDraw();
	m_bAbortDraw = TRUE;
	m_toolTipCtrl.Pop();
	UnlockWindowUpdate();

	CScrollView::InvalidateRect(lpRect, bErase);
}


void CGdsView::OnPaint() 
{
	if(IsDrawing()){ 
		if(m_pThreadRender)
			m_pThreadRender->WaitUntilDrawn();
	}

	BOOL combine_region = FALSE;
	if(m_bCancelDraw)
		combine_region = TRUE;

	CRect crect;
	GetClientRect(&crect);
	CRgn rgn;
	CRect rect(0, 0, 0, 0);
	rgn.CreateRectRgnIndirect(&rect);

	if(!m_bInitView)
		rgn.CopyRgn(&m_rgnUpdate);

	m_rgnUpdate.DeleteObject();
	m_rgnUpdate.CreateRectRgnIndirect(crect);

	m_bRedrawPart = TRUE;
	if(GetUpdateRgn(&m_rgnUpdate) == SIMPLEREGION){
		CRect rect;
		m_rgnUpdate.GetRgnBox(&rect);
		if(rect == crect)
			m_bRedrawPart = FALSE;
	}

	CPaintDC dc(this);
	OnPrepareDC(&dc);
	SetOrigin(&dc);
	m_pDC = &dc;
	FillBackground();


	if(combine_region)
		m_rgnUpdate.CombineRgn(&m_rgnUpdate, &rgn, RGN_OR);

	SetGraphicsMode(dc.m_hDC, GM_COMPATIBLE);
	OnDraw(&dc);
}



void CGdsView::OnDrawByLayer() 
{
	InvalidateRect(TRUE, NULL);
	m_bDrawByLayer ^= TRUE;
	AfxGetApp()->WriteProfileInt("Misc", "Draw by Layer", m_bDrawByLayer);
}

void CGdsView::OnUpdateDrawByLayer(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bDrawByLayer);
}

void CGdsView::OnOverwriteMode() 
{
	m_bOverWrite ^= TRUE;
	InvalidateRect(TRUE, NULL);
}

void CGdsView::OnUpdateOverwriteMode(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(!m_bOverWrite);
}

void CGdsView::OnOutlinePath() 
{
	m_bOutlinePath ^= TRUE;
	InvalidateRect(TRUE, NULL);
	AfxGetApp()->WriteProfileInt("Misc", "Draw Path outline", m_bOutlinePath);
}

void CGdsView::OnUpdateOutlinePath(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bOutlinePath);
}

void CGdsView::LockWindowUpdate()
{
	m_bLockUpdate = TRUE;
}

void CGdsView::UnlockWindowUpdate()
{
	m_bLockUpdate = FALSE;
}

void CGdsView::OnSize(UINT nType, int cx, int cy) 
{
	StopDraw();
	CScrollView::OnSize(nType, cx, cy);
}


void CGdsView::ScrollToPosition(CPoint pos)
{
	CScrollView::ScrollToPosition(pos);
}

int CGdsView::DrawPrintLegend()
{
	GraphicsState state = m_pGraphics->Save();
	m_pGraphics->RotateTransform((REAL)m_dblLegendAngle);

	REAL pen_width = m_realPenWidth;
	REAL height = (REAL)(m_dblLegendSize/HIMETRIC);
	REAL width = height * 2.0f;
	REAL xstep = width * 4.0f;
	REAL ystep = height * 1.2f;

	if(ystep - height < pen_width)
		ystep += 1.5f * (pen_width - (ystep - height));

	REAL xstart = (REAL)(m_rectClip.left + FOOTER_MARGIN * 2);
	REAL xstop  = (REAL)(m_rectClip.right - FOOTER_MARGIN * 2);
	REAL ystart = (REAL)(m_rectClip.bottom - FOOTER_MARGIN);
	
	if(m_dblLegendAngle != 0.0){
		xstart = (REAL)(m_rectClip.top + FOOTER_MARGIN * 2);
		xstop  = (REAL)(m_rectClip.bottom - FOOTER_MARGIN * 2);
		ystart = (REAL)(m_rectClip.right - FOOTER_MARGIN);
	}
	REAL x, y;

	x = xstart;
	y = ystart - height;
	for(unsigned i = 0;i < GDS_MAX_LAYER;i++){
		m_intLayerNow = *(m_aryIntLayerOrder + i);
		if(m_intLayerNow < 0)
			break;
		if(GetLayerStatus(m_intLayerNow) <= 0 || !m_pStrActive->IsVisibleOnLayer(m_intLayerNow))
			continue;

		COLORREF rgb = ReverseGray(*(m_aryColorLayerRGB + m_intLayerNow));
		Color color;
		color.SetFromCOLORREF(rgb);

		SolidBrush brush(color);
		FontFamily fontFamily(L"Courier New");
		Font font(&fontFamily, height, FontStyleBold, UnitPixel);
		StringFormat format;

		CString str;
		str.Format("%d:%s", m_intLayerNow, *(m_aryLayerName + m_intLayerNow));

		USES_CONVERSION;
		LPTSTR lpsz = new TCHAR[str.GetLength() + 1];
		_tcscpy(lpsz, str);
		WCHAR* pwstr = A2W(lpsz);
		delete [] lpsz;

		PointF pointf(x + width + pen_width, y + height);
		format.SetAlignment(StringAlignmentNear);
		GraphicsState state = m_pGraphics->Save();
		m_pGraphics->TranslateTransform(pointf.X, pointf.Y, MatrixOrderPrepend);
		m_pGraphics->ScaleTransform(1.0f, -1.0f, MatrixOrderPrepend);
		m_pGraphics->TranslateTransform(-pointf.X, -pointf.Y, MatrixOrderPrepend);
		//pointf.Y -= font.GetSize()/2.0f;

		RectF area;
		REAL str_width = area.Width;
		m_pGraphics->MeasureString(pwstr, -1, &font, pointf, &format, &area);
		//m_pGraphics->DrawRectangle(&pen, area);

		if(pointf.X + area.Width + height >= xstop){
			x = xstart;
			y -= ystep;
			pointf.X = x + width + pen_width;
			pointf.Y += ystep;
		}

		m_pGraphics->DrawString(pwstr, -1, &font, pointf, &format, &brush);
		m_pGraphics->Restore(state);

		COLORREF pen_color = /*(m_bPrintMode && !m_bPrintPenColor) ? COLOR_BLACK : */rgb;
		Color color_pen;
		color_pen.SetFromCOLORREF(pen_color);

		if(*(m_aryIntFillType + m_intLayerNow) != FILL_BMP_NONE && m_bFillEnable){
			PointF points[4];
			m_colorLayer = color_pen;
			points[0].X = points[1].X = x;
			points[0].Y = points[3].Y = y;
			points[1].Y = points[2].Y = y + height;
			points[2].X = points[3].X = x + width;
			GraphicsPath path;
			path.StartFigure();
			path.AddPolygon(points, 4);
			path.CloseFigure();
			if(m_bFillEnable)
				FillPolygon(&path, TRUE);
		}

		Pen pen(color_pen, pen_width);
		m_pGraphics->DrawRectangle(&pen, x, y, width, height);

		x = pointf.X + area.Width + height;
	}
	m_pGraphics->Restore(state);

	return (int)(ystart - y + height * 2);
}

void CGdsView::DrawPrintScale()
{
	double dbunit = GetGDShcy()->GetDBUnit();
	REAL len1 = (REAL)(SCALE_DIV_ONE);
	REAL len2 = (REAL)(SCALE_DIV_FIV);
	REAL space = len2 * 1.5f;
	REAL fonth = len2;
	int  strcnt = 10;
	REAL strlen = fonth * (REAL)strcnt;
	REAL len;
	REAL x, y;
	REAL pen_width = m_realPenWidth;


	SolidBrush brush(Color(255, 0, 0, 0));
	FontFamily fontFamily(L"Courier New");
	Font font(&fontFamily, fonth, FontStyleBold, UnitPixel);
	StringFormat format;

	Pen pen(Color(255, 0, 0, 0), pen_width);
	REAL xstart = (REAL)((m_coordMin.x + m_coordViewOffset.x) * m_dblDrawScale);
	REAL xstop = (REAL)((m_coordMax.x + m_coordViewOffset.x) * m_dblDrawScale);
	REAL ystart = (REAL)((m_coordMin.y + m_coordViewOffset.y) * m_dblDrawScale);
	REAL ystop = ystart;

	m_pGraphics->DrawLine(&pen, xstart, ystart, xstop, ystop);

	USES_CONVERSION;
	CString str;


	ystop = (REAL)((m_coordMax.y + m_coordViewOffset.y) * m_dblDrawScale);


	REAL step = (REAL)(1.0 * 1e-6 / dbunit); // 1um
	double n = strlen / (step * m_dblDrawScale);
	n /= 5;
	if(n > 0){
		if(n < 10.0)
			n = (int)(n / 5.0) * 5.0;
		else if(n < 20.0)
			n = (int)(n / 10.0) * 10.0;
		else
			n = (int)(n / 20.0) * 20.0;
		if(n > 0)
			step *= (REAL)n;
		else
			step *= 10.0;
	}

	int num = 0;
	for(x = 0.0;x <= m_coordMax.x;x += step){
		if(num % 5 == 0)
			len = len2;
		else
			len = len1;
		REAL xpos = x; 
		if(m_bOptShrink){
			xpos /= (REAL)(1.0 - m_dblShrinkRate);
			if(xpos > m_coordMax.x)
				break;
		}
		xpos += (REAL)m_coordViewOffset.x;
		xpos *= (REAL)m_dblDrawScale;

		m_pGraphics->DrawLine(&pen, xpos, ystart, xpos, ystart - len);
		m_pGraphics->DrawLine(&pen, xpos, ystop, xpos, ystop + len);
		if(num % 5 == 0){
			str.Format("%.3f", x * dbunit * 1e+6);
			if(str.Find(".") >= 0){
				str.TrimRight("0");
				str.TrimRight(".");
			}
			str += CString("um");
			LPTSTR lpsz = new TCHAR[str.GetLength() + 1];
			_tcscpy(lpsz, str);
			WCHAR* pwstr = A2W(lpsz);
			delete [] lpsz;

			PointF pointf(xpos, ystart - space);
			format.SetAlignment(StringAlignmentCenter);
			GraphicsState state = m_pGraphics->Save();
			m_pGraphics->TranslateTransform(pointf.X, pointf.Y, MatrixOrderPrepend);
			m_pGraphics->ScaleTransform(1.0f, -1.0f, MatrixOrderPrepend);
			m_pGraphics->TranslateTransform(-pointf.X, -pointf.Y, MatrixOrderPrepend);
			m_pGraphics->DrawString(pwstr, -1, &font, pointf, &format, &brush);
			m_pGraphics->Restore(state);

			pointf.Y = ystop + space;
			state = m_pGraphics->Save();
			m_pGraphics->TranslateTransform(pointf.X, pointf.Y, MatrixOrderPrepend);
			m_pGraphics->ScaleTransform(1.0f, -1.0f, MatrixOrderPrepend);
			m_pGraphics->TranslateTransform(-pointf.X, -pointf.Y, MatrixOrderPrepend);
			pointf.Y -= font.GetSize();
			m_pGraphics->DrawString(pwstr, -1, &font, pointf, &format, &brush);
			m_pGraphics->Restore(state);
		}
		num++;
	}
	num = -1;
	for(x = -step;x >= m_coordMin.x;x -= step){
		if(num % 5 == 0)
			len = len2;
		else
			len = len1;
		REAL xpos = x; 
		if(m_bOptShrink){
			xpos /= (REAL)(1.0 - m_dblShrinkRate);
			if(xpos < m_coordMin.x)
				break;
		}
		xpos += (REAL)m_coordViewOffset.x;
		xpos *= (REAL)m_dblDrawScale;

		m_pGraphics->DrawLine(&pen, xpos, ystart, xpos, ystart - len);
		m_pGraphics->DrawLine(&pen, xpos, ystop, xpos, ystop + len);
		if(num % 5 == 0){
			str.Format("%.3f", x * dbunit * 1e+6);
			if(str.Find(".") >= 0){
				str.TrimRight("0");
				str.TrimRight(".");
			}
			str += CString("um");
			LPTSTR lpsz = new TCHAR[str.GetLength() + 1];
			_tcscpy(lpsz, str);
			WCHAR* pwstr = A2W(lpsz);
			delete [] lpsz;

			PointF pointf(xpos, ystart - space);
			format.SetAlignment(StringAlignmentCenter);
			GraphicsState state = m_pGraphics->Save();
			m_pGraphics->TranslateTransform(pointf.X, pointf.Y, MatrixOrderPrepend);
			m_pGraphics->ScaleTransform(1.0f, -1.0f, MatrixOrderPrepend);
			m_pGraphics->TranslateTransform(-pointf.X, -pointf.Y, MatrixOrderPrepend);
			m_pGraphics->DrawString(pwstr, -1, &font, pointf, &format, &brush);
			m_pGraphics->Restore(state);

			pointf.Y = ystop + space + font.GetSize();
			state = m_pGraphics->Save();
			m_pGraphics->TranslateTransform(pointf.X, pointf.Y, MatrixOrderPrepend);
			m_pGraphics->ScaleTransform(1.0f, -1.0f, MatrixOrderPrepend);
			m_pGraphics->TranslateTransform(-pointf.X, -pointf.Y, MatrixOrderPrepend);
			m_pGraphics->DrawString(pwstr, -1, &font, pointf, &format, &brush);
			m_pGraphics->Restore(state);
		}
		num--;
	}


	ystart = ystop = (REAL)((m_coordMax.y + m_coordViewOffset.y) * m_dblDrawScale);
	m_pGraphics->DrawLine(&pen, xstart, ystart, xstop, ystop);

	step = (REAL)(1.0 * 1e-6 / dbunit); // 1um
	n = strlen / (step * m_dblDrawScale);
	n /= 5;
	if(n > 0){
		if(n < 10.0)
			n = (int)(n / 5.0) * 5.0;
		else if(n < 20.0)
			n = (int)(n / 10.0) * 10.0;
		else
			n = (int)(n / 20.0) * 20.0;
		if(n > 0)
			step *= (REAL)n;
		else
			step *= 10.0;
	}
	xstart = xstop = (REAL)((m_coordMin.x + m_coordViewOffset.x) * m_dblDrawScale);
	ystart = (REAL)((m_coordMin.y + m_coordViewOffset.y) * m_dblDrawScale);
	ystop = (REAL)((m_coordMax.y + m_coordViewOffset.y) * m_dblDrawScale);
	m_pGraphics->DrawLine(&pen, xstart, ystart, xstop, ystop);

	xstop = (REAL)((m_coordMax.x + m_coordViewOffset.x) * m_dblDrawScale);
	num = 0;
	for(y = 0.0;y <= m_coordMax.y;y += step){
		if(num % 5 == 0)
			len = len2;
		else
			len = len1;
		REAL ypos = y; 
		if(m_bOptShrink){
			ypos /= (REAL)(1.0 - m_dblShrinkRate);
			if(ypos > m_coordMax.y)
				break;
		}
		ypos += (REAL)m_coordViewOffset.y;
		ypos *= (REAL)m_dblDrawScale;

		m_pGraphics->DrawLine(&pen, xstart, ypos, xstart - len, ypos);
		m_pGraphics->DrawLine(&pen, xstop, ypos, xstop + len, ypos);
		if(num % 5 == 0){
			str.Format("%.3f", y * dbunit * 1e+6);
			if(str.Find(".") >= 0){
				str.TrimRight("0");
				str.TrimRight(".");
			}
			str += CString("um");
			LPTSTR lpsz = new TCHAR[str.GetLength() + 1];
			_tcscpy(lpsz, str);
			WCHAR* pwstr = A2W(lpsz);
			delete [] lpsz;

			PointF pointf(xstart - space - font.GetSize(), ypos);
			format.SetAlignment(StringAlignmentCenter);
			GraphicsState state = m_pGraphics->Save();
			m_pGraphics->TranslateTransform(pointf.X, pointf.Y, MatrixOrderPrepend);
			m_pGraphics->RotateTransform(90.0f, MatrixOrderPrepend);
			m_pGraphics->ScaleTransform(1.0f, -1.0f, MatrixOrderPrepend);
			m_pGraphics->TranslateTransform(-pointf.X, -pointf.Y, MatrixOrderPrepend);
			m_pGraphics->DrawString(pwstr, -1, &font, pointf, &format, &brush);
			m_pGraphics->Restore(state);

			pointf.X = xstop + space;
			state = m_pGraphics->Save();
			m_pGraphics->TranslateTransform(pointf.X, pointf.Y, MatrixOrderPrepend);
			m_pGraphics->RotateTransform(-90.0f, MatrixOrderPrepend);
			m_pGraphics->ScaleTransform(1.0f, -1.0f, MatrixOrderPrepend);
			m_pGraphics->TranslateTransform(-pointf.X, -pointf.Y, MatrixOrderPrepend);
			pointf.Y -= font.GetSize();
			m_pGraphics->DrawString(pwstr, -1, &font, pointf, &format, &brush);
			m_pGraphics->Restore(state);
		}
		num++;
	}
	num = -1;
	for(y = -step;y >= m_coordMin.y;y -= step){
		if(num % 5 == 0)
			len = len2;
		else
			len = len1;
		REAL ypos = y; 
		if(m_bOptShrink){
			ypos /= (REAL)(1.0 - m_dblShrinkRate);
			if(ypos < m_coordMin.y)
				break;
		}
		ypos += (REAL)m_coordViewOffset.y;
		ypos *= (REAL)m_dblDrawScale;

		m_pGraphics->DrawLine(&pen, xstart, ypos, xstart - len, ypos);
		m_pGraphics->DrawLine(&pen, xstop, ypos, xstop + len, ypos);
		if(num % 5 == 0){
			str.Format("%.3f", y * dbunit * 1e+6);
			if(str.Find(".") >= 0){
				str.TrimRight("0");
				str.TrimRight(".");
			}
			str += CString("um");
			LPTSTR lpsz = new TCHAR[str.GetLength() + 1];
			_tcscpy(lpsz, str);
			WCHAR* pwstr = A2W(lpsz);
			delete [] lpsz;

			PointF pointf(xstart - space - font.GetSize(), ypos);
			format.SetAlignment(StringAlignmentCenter);
			GraphicsState state = m_pGraphics->Save();
			m_pGraphics->TranslateTransform(pointf.X, pointf.Y, MatrixOrderPrepend);
			m_pGraphics->RotateTransform(90.0f, MatrixOrderPrepend);
			m_pGraphics->ScaleTransform(1.0f, -1.0f, MatrixOrderPrepend);
			m_pGraphics->TranslateTransform(-pointf.X, -pointf.Y, MatrixOrderPrepend);
			m_pGraphics->DrawString(pwstr, -1, &font, pointf, &format, &brush);
			m_pGraphics->Restore(state);

			pointf.X = xstop + space;
			state = m_pGraphics->Save();
			m_pGraphics->TranslateTransform(pointf.X, pointf.Y, MatrixOrderPrepend);
			m_pGraphics->RotateTransform(-90.0f, MatrixOrderPrepend);
			m_pGraphics->ScaleTransform(1.0f, -1.0f, MatrixOrderPrepend);
			m_pGraphics->TranslateTransform(-pointf.X, -pointf.Y, MatrixOrderPrepend);
			pointf.Y -= font.GetSize();
			m_pGraphics->DrawString(pwstr, -1, &font, pointf, &format, &brush);
			m_pGraphics->Restore(state);
		}
		num--;
	}

	xstart = xstop = (REAL)((m_coordMax.x + m_coordViewOffset.x) * m_dblDrawScale);
	m_pGraphics->DrawLine(&pen, xstart, ystart, xstop, ystop);
}

void CGdsView::DrawPrintFooter()
{
	REAL pen_width = m_realPenWidth;
	REAL height = (REAL)(FOOTER_HEIGHT);

	SolidBrush brush(Color(255, 0, 0, 0));
	FontFamily fontFamily(L"Courier New");
	Font font(&fontFamily, height, FontStyleBold, UnitPixel);
	StringFormat format;

	Pen pen(Color(255, 0, 0, 0), pen_width);

	
	m_pGraphics->DrawLine(&pen, 
		(REAL)(m_rectClip.left + FOOTER_MARGIN), (REAL)(m_rectClip.top + FOOTER_MARGIN),
		(REAL)(m_rectClip.right - FOOTER_MARGIN), (REAL)(m_rectClip.top + FOOTER_MARGIN));

	USES_CONVERSION;
	CString str, title;
	title = GetDocument()->GetPathName();
	int idx = title.ReverseFind('\\');
	if(idx > 0)
		title = title.Right(title.GetLength() - idx - 1);
	str.Format("File: %s  Cell: %s", 
		title, m_pStrActive->GetStructureName());

	CString stra;
	switch(m_intPrintMode){
	case PRINT_MODE_WYSIWYG:
		stra = "";
		break;
	case PRINT_MODE_SCALED_FULL:
		stra.Format("  Scale: %d", (int)(m_dblPrintScale));
		break;
	case PRINT_MODE_SCALED_AREA:
		stra.Format("  Scale: %d", (int)(m_dblPrintScaleArea));
	}
	str += stra;

	if(m_bOptShrink && m_dblShrinkRate > 0.0){
		stra.Format("  Shrink: %d%%", (int)(m_dblShrinkRate * 100.0));
		str += stra;
	}

	LPTSTR lpsz = new TCHAR[str.GetLength() + 1];
	_tcscpy(lpsz, str);
	WCHAR* pwstr = A2W(lpsz);
	delete [] lpsz;

	PointF pointf((REAL)(m_rectClip.left + FOOTER_MARGIN),
					(REAL)(m_rectClip.top + FOOTER_MARGIN));
	format.SetAlignment(StringAlignmentNear);
	GraphicsState state = m_pGraphics->Save();
	m_pGraphics->TranslateTransform(pointf.X, pointf.Y, MatrixOrderPrepend);
	m_pGraphics->ScaleTransform(1.0f, -1.0f, MatrixOrderPrepend);
	m_pGraphics->TranslateTransform(-pointf.X, -pointf.Y, MatrixOrderPrepend);
	m_pGraphics->DrawString(pwstr, -1, &font, pointf, &format, &brush);
	m_pGraphics->Restore(state);
}


void CGdsView::OnHatchExact() 
{
	m_bHatchExactly ^= TRUE;
	InvalidateRect(TRUE, NULL);
}

void CGdsView::OnUpdateHatchExact(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bHatchExactly);
	pCmdUI->Enable(m_bFillEnable);	
}

void CGdsView::OnRulerColor() 
{
	CColorDialog dlg(m_colorScale);
	if(dlg.DoModal() == IDCANCEL)
		return;
	m_colorScale = dlg.GetColor();
	AfxGetApp()->WriteProfileInt("Color", "Ruler", m_colorScale);
	InvalidateRect(TRUE, NULL);
}

void CGdsView::OnColorMemo() 
{
	CColorDialog dlg(m_colorMemo);
	if(dlg.DoModal() == IDCANCEL)
		return;
	m_colorMemo = dlg.GetColor();
	AfxGetApp()->WriteProfileInt("Color", "Memo", m_colorMemo);
	InvalidateRect(TRUE, NULL);
}


#include "TimeStampDlg.h"
void CGdsView::OnShowTimeStamp() 
{
	CTimeStampDlg dlg;

	CGDS* pGds = GetGDShcy();
	dlg.SetCreationDate(pGds->GetCreationDate());
	dlg.SetAccessDate(pGds->GetCreationDate());
	dlg.SetDBName(pGds->GetLibName());

	dlg.DoModal();
}


void CGdsView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	SIZE sz;
	sz.cx = (long)(SCROLL_WIDTH / HIMETRIC); 
	sz.cy = (long)(SCROLL_HEIGHT / HIMETRIC); 
	SetScrollSizes(MM_HIMETRIC, sz);
}

#include "DefLayerSet.h"
void CGdsView::OnLayerSetupAtBoot() 
{
	CDefLayerSet dlg;
	CWinApp* app = AfxGetApp();

	dlg.SetFileName(app->GetProfileString("Settings", "Default technology map file", ""));
	if(dlg.DoModal() == IDCANCEL)
		return;

	CString path = dlg.GetFileName();
	app->WriteProfileString("Settings", "Default technology map file", path);
}

BOOL CGdsView::ReadWriteIniFile(BOOL read)
{
	CGdsApp* app = (CGdsApp*)AfxGetApp();
	if(read){
		CString path = app->GetExePath();
		int idx = path.ReverseFind('\\');
		if(idx >= 0)
			path = path.Left(idx + 1);
		path = path + app->m_pszExeName + ".ini";
		CFile file;
		if(!file.Open(path, CFile::modeRead))
			return FALSE;
		int size = file.GetLength();
		file.Close();
		char* buf = new char[min(1024, size)];
		size = GetPrivateProfileSection("Layer", buf, size, path);
		delete [] buf;
	}
	else{
	}
	return TRUE;
}

void CGdsView::OnShowSpotElement() 
{
	m_bSpotSmall ^= TRUE;
	InvalidateRect(TRUE, NULL);
}

void CGdsView::OnUpdateShowSpotElement(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bSpotSmall);	
}



void CGdsView::OnShowBoundary() 
{
	m_bShowBoundary ^= TRUE;
	InvalidateRect(TRUE, NULL);
}

void CGdsView::OnUpdateShowBoundary(CCmdUI* pCmdUI) 
{
//#ifndef _DEBUG
//	pCmdUI->Enable(FALSE);
//#endif
	pCmdUI->SetCheck(m_bShowBoundary);
}

void CGdsView::OnShowPath() 
{
	m_bShowPath ^= TRUE;
	InvalidateRect(TRUE, NULL);
}

void CGdsView::OnUpdateShowPath(CCmdUI* pCmdUI) 
{
//#ifndef _DEBUG
//	pCmdUI->Enable(FALSE);
//#endif
	pCmdUI->SetCheck(m_bShowPath);
}

void CGdsView::OnDebugSwArefArea() 
{
	m_bShowAREFArea ^= TRUE;
	InvalidateRect(TRUE, NULL);
}

void CGdsView::OnUpdateDebugSwArefArea(CCmdUI* pCmdUI) 
{
//#ifndef _DEBUG
//	pCmdUI->Enable(FALSE);
//#endif
	pCmdUI->SetCheck(m_bShowAREFArea);
}

void CGdsView::ClearViewHistory()
{
	for(int i = 0;i < m_arrayViewSetting.GetSize();i++){
		CViewSetting* pset = (CViewSetting*)m_arrayViewSetting.GetAt(i);
		delete pset;
	}
	m_arrayViewSetting.RemoveAll();
}

void CGdsView::GetViewArea(CViewSetting* pSet)
{
	double left, right, top, bottom;
	GetViewArea(&left, &right, &top, &bottom);
	if(!pSet)
		pSet = &m_viewSetting;
	pSet->m_coordCenter.x = (left + right)/2.0;
	pSet->m_coordCenter.y = (top + bottom)/2.0;
	pSet->m_coordSize.x = fabs(right - left);
	pSet->m_coordSize.y = fabs(top - bottom);
	pSet->m_coordCenter.Mul(1e-6);// um
	pSet->m_coordSize.Mul(1e-6);// um
	pSet->m_pStructure = m_pStrActive;
}


void CGdsView::OnViewBack() 
{
	CGDSStructure* pPrev = m_pStrActive;

	BOOL push_pop = m_pStrActive != m_viewSetting.m_pStructure;
	if(push_pop)
		PopHistory();
	RestoreView(&m_viewSetting);
	if(push_pop){
		CGDSStructure* pNow = m_pStrActive;
		m_pStrActive = pPrev;
		AddHistory();
		m_pStrActive = pNow;
	}
}

void CGdsView::RestoreView(CViewSetting* pRestore)
{
	StopDraw();

	CViewSetting now, restore;
	GetViewArea(&now);
	restore = *pRestore; // overwritten when change structure

	CCoord center, size;

	if(m_pStrActive != pRestore->m_pStructure){
		ChangeStructure(pRestore->m_pStructure, TRUE);
		CClientDC dc(this);
		OnPrepareDC(&dc);
		SetOrigin(&dc);
		OnDraw(&dc);
		center = restore.m_coordCenter;
		size = restore.m_coordSize;
	}
	else{
		center = pRestore->m_coordCenter;
		size = pRestore->m_coordSize;
	}
	ZoomAreaCoord(center, size);

	m_bAbortDraw = FALSE;
	m_viewSetting = now;
}


void CGdsView::OnViewList() 
{
	GetViewArea();
	if(!m_bDlgView){
		m_pDlgView = new CSelViewSetting();
		m_pDlgView->SetEnabler(&m_bDlgView);
		m_bDlgView = TRUE;
		m_pDlgView->SetCaller(this);
		m_pDlgView->Create(IDD_VIEW_SELECT, this);
	}
}

void CGdsView::RestoreView(int idx)
{
	if(idx < 0 || idx > m_arrayViewSetting.GetSize() - 1)
		return;
	CViewSetting* pset = (CViewSetting*)m_arrayViewSetting.GetAt(idx);
	RestoreView(pset);
}

void CGdsView::DeleteView(int idx)
{
	if(idx < 0 || idx > m_arrayViewSetting.GetSize() - 1)
		return;
	CViewSetting* pset = (CViewSetting*)m_arrayViewSetting.GetAt(idx);
	delete pset;
	m_arrayViewSetting.RemoveAt(idx);
}

void CGdsView::DeleteAllView()
{
	for(int i = 0;i < m_arrayViewSetting.GetSize();i++){
		CViewSetting* pset = (CViewSetting*)m_arrayViewSetting.GetAt(i);
		delete pset;
	}
	m_arrayViewSetting.RemoveAll();
}

#include "ViewSetCommentDlg.h"
void CGdsView::OnViewStore() 
{
	CViewSetCommentDlg dlg;
	if(dlg.DoModal() == IDCANCEL)
		return;

	CViewSetting* pSet = new CViewSetting;
	GetViewArea(pSet);
	pSet->m_pStructure = m_pStrActive;
	pSet->m_strComment = dlg.GetComment();
	m_arrayViewSetting.Add(pSet);

	if(m_bDlgView)
		m_pDlgView->AddParam();
}

void CGdsView::OnUpdateViewList(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_arrayViewSetting.GetSize() > 0 ? TRUE : FALSE);
}

void CGdsView::OnDropFiles(HDROP hDropInfo) 
{
	CGdsDoc* pDoc = GetDocument();

	((CMainFrame*)GetTopLevelFrame())->OnDropFiles(hDropInfo, pDoc->GetTecFilePath());

	//CScrollView::OnDropFiles(hDropInfo);
}

DROPEFFECT CGdsView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	LockWindowUpdate();	
	return CScrollView::OnDragEnter(pDataObject, dwKeyState, point);
}

void CGdsView::OnDragLeave() 
{
	UnlockWindowUpdate();	
	CScrollView::OnDragLeave();
}

DROPEFFECT CGdsView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	LockWindowUpdate();	
	return CScrollView::OnDragOver(pDataObject, dwKeyState, point);
}

void CGdsView::SyncToAnotherView(CGdsView *pView)
{
	if(m_intDrawMode != DRAW_MODE_DISPLAY)
		return;
	if(m_intMirror > MIRROR_NONE || m_dblViewRotation != 0.0)
		return;
	m_dblDrawScale = pView->m_dblDrawScale;
	m_coordViewOffset = pView->m_coordViewOffset;

	CPoint pos = pView->GetScrollPosition();
	ScrollToPosition(pos);
	InvalidateRect(TRUE, NULL);
}

void CGdsView::OnSyncView() 
{
	if(m_intMirror > MIRROR_NONE || m_dblViewRotation != 0.0)
		return;
	if(m_pMainFrame->IsViewActive(this))
		m_pMainFrame->SyncViews(this);
}

void CGdsView::OnSyncViewAlways() 
{
	if(m_intDrawMode != DRAW_MODE_DISPLAY)
		return;
	m_pMainFrame->SetSyncView(m_pMainFrame->GetSyncView() ^ TRUE);
	if(m_pMainFrame->GetSyncView())
		OnSyncView();
}

void CGdsView::OnUpdateSyncViewAlways(CCmdUI* pCmdUI) 
{
	//BOOL twoOrMore = m_pMainFrame->IsThereMoreView(this);

	//pCmdUI->Enable(twoOrMore);
	//if(!twoOrMore)
	//	m_pMainFrame->SetSyncView(FALSE);

	if(m_intMirror > MIRROR_NONE || m_dblViewRotation != 0.0){
		m_pMainFrame->SetSyncView(FALSE);
		pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(TRUE);

	pCmdUI->SetCheck(m_pMainFrame->GetSyncView());
}


static inline PointF ClipWindow(PointF& p1, PointF& p2, float limit, BOOL horz)
{
	float d = 0.0f;

	if(horz){
		if(p2.X != p1.X){
			d = (p2.Y - p1.Y)/(p2.X - p1.X);
			return PointF(limit, p1.Y + d * (limit - p1.X));
		}
		else
			return PointF(limit, p2.Y);
	}
	else{
		if(p2.Y != p1.Y){
			d = (p2.X - p1.X)/(p2.Y - p1.Y);
			return PointF(p1.X + d * (limit - p1.Y), limit);
		}
		else
			return PointF(p2.X, limit);
	}
}

static inline BOOL IsInside(PointF& p, float limit, BOOL horizon, int posneg)
{
	float cmp = horizon ? p.X : p.Y;

	if(posneg > 0 && cmp > limit)
		return FALSE;
	else if(posneg < 0 && cmp < limit)
		return FALSE;
	return TRUE;
}

void CGdsView::ClipPath(GraphicsPath *pPath, BOOL horizon, int posneg)
{
	BOOL recurse = FALSE;
	if(posneg == 0){
		recurse = TRUE;
		horizon = TRUE; // Left : default
		posneg = -1;
	}

	REAL lim = (REAL)((SCROLL_WIDTH / HIMETRIC)/2 * 1.2);

	BOOL clipWin = TRUE;
	if(m_intDrawMode != DRAW_MODE_CLIPBOARD && !m_bClipPolygon)
		clipWin = FALSE;

	float limit;
	if(posneg < 0){
		if(horizon)
			limit = (float) clipWin ? m_rectClip.left : -lim;
		else
			limit = (float) clipWin ? m_rectClip.top : -lim;
	}
	else{
		if(horizon)
			limit = (float) clipWin ? m_rectClip.right : lim;
		else
			limit = (float) clipWin ? m_rectClip.bottom : lim;
	}


	GraphicsPath subpath, newsubpath, newpath;
	GraphicsPathIterator iterator(pPath);
	PathData pdat;

	newpath.Reset();
	newpath.StartFigure();

	PointF pNow, pNext, pLast, pClip;
	BOOL closed;

	while(iterator.NextSubpath(&subpath, &closed) > 0){
		subpath.GetPathData(&pdat);

		for(int i = 0;i < pdat.Count;i++){ 
			// find first invisible point and re-order, begins with it
			//
			if(!IsInside(*(pdat.Points + i), limit, horizon, posneg)){
				if(i == 0)
					break;
				int k = 0;
				for(int j = i;j < pdat.Count;j++)
					*(points + k++) = *(pdat.Points + j);
				for(j = 0;j < i;j++)
					*(points + k++) = *(pdat.Points + j);

				for(j = 0;j < pdat.Count;j++)
					*(pdat.Points + j) = *(points + j);
				break;
			}
		}
		newsubpath.Reset();
		newsubpath.StartFigure();
		for(int idx = 0;idx < pdat.Count;idx++){
			pNow = *(pdat.Points + idx);
			if(idx >= pdat.Count - 1)
				pNext = *(pdat.Points + 0);
			else
				pNext = *(pdat.Points + idx + 1);

			if(IsInside(pNow, limit, horizon, posneg)){ // pNow is visible
				if(IsInside(pNext, limit, horizon, posneg)) // both visible
					newsubpath.AddLine(pNow, pNext);
				else{ // go out of window
					pClip = ClipWindow(pNow, pNext, limit, horizon);
					newsubpath.AddLine(pNow, pClip);
					newsubpath.AddLine(pClip, pLast);

					newsubpath.SetMarker();
					newsubpath.CloseFigure();
					newpath.AddPath(&newsubpath, FALSE);
					newsubpath.Reset();
					newsubpath.StartFigure();
				}
			}
			else{ // pNow is invisible
				if(!IsInside(pNext, limit, horizon, posneg)) // both invisible
					;// do nothing
				else{ // come into window
					pLast = ClipWindow(pNow, pNext, limit, horizon);
					newsubpath.AddLine(pLast, pNext);
				}
			}
		}
		newsubpath.CloseFigure();
		if(newsubpath.GetPointCount() > 0)
			newpath.AddPath(&newsubpath, FALSE);
	}
	newpath.CloseFigure();
	pPath->Reset();
	pPath->AddPath(&newpath, FALSE);

	if(recurse){
		ClipPath(pPath, TRUE, +1); // right
		ClipPath(pPath, FALSE, +1); // top
		ClipPath(pPath, FALSE, -1); // bottom
	}
}

void CGdsView::OnClipPolygon() 
{
	m_bClipPolygon ^= TRUE;
	InvalidateRect(TRUE, NULL);
}

void CGdsView::OnUpdateClipPolygon(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bClipPolygon);	
}


inline BOOL CGdsView::NeedClip(PointF* p)
{
	static CSize sz = GetTotalSize();
	static REAL max = (REAL)sz.cx / 2;
	static REAL min = (REAL)-sz.cx / 2;

	if(p->X > max)
		return TRUE;
	else if(p->X < min)
		return TRUE;
	if(p->Y > max)
		return TRUE;
	else if(p->Y < min)
		return TRUE;
	return FALSE;
}

