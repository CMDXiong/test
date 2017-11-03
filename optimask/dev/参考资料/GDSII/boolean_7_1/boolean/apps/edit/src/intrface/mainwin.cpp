// de nieuwe mainWin voor wxW*indows

#ifdef __GNUG__
#pragma implementation
#endif

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "open.xpm"
#include "save.xpm"
#endif

#include <locale.h>
#include <math.h>

#include "wxwin.h"

#include "wx/cursor.h"
#include "cursordlg.h"

#include "chandbas.h"
#include "gdserr.h"
#include "windrv.h"
#include "winstatb.h"
#include "gdsapp.h"
#include "about.h"
#include "b_lnk_itr.h"
#include "b_statusb.h"
#include "prcspars.h"
#include "about.h"
#include "execdlg.h"
#include "debugdlg.h"
#include "insdlg.h"
#include "identdlg.h"
#include "distdlg.h"
#include "wait.h"
#include "strucdlg.h"
#include "groupdlg.h"
#include "transdlg.h"
#include "settingsdlg.h"
#include "griddlg.h"
#include "unitsdlg.h"
#include "orderdlg.h"
#include "facdlg.h"
#include "patdlg.h"
#include "newfiledlg.h"
#include "textinsdlg.h"
#include "layprops.h"
#include "printer.h"
#include "propedit.h"
#include "toolcont.h"

#include "wx/imagpng.h"

#include "wx/bitmap.h"

#include "eventids.h"

#ifdef __UNIX__
	long style_top = (wxRESIZE_BORDER |wxTHICK_FRAME | wxCAPTION | wxUSER_COLOURS);
	long style = (wxRESIZE_BORDER |wxTHICK_FRAME | wxCAPTION | wxUSER_COLOURS);
	long style_noresize = (wxTHICK_FRAME | wxCAPTION | wxUSER_COLOURS);
#else
	long style_top = (wxRESIZE_BORDER |wxTHICK_FRAME | wxSYSTEM_MENU | wxCAPTION);
	long style = (wxRESIZE_BORDER |wxTHICK_FRAME | wxSYSTEM_MENU | wxCAPTION);
	long style_noresize = (wxTHICK_FRAME |  wxSYSTEM_MENU | wxCAPTION);
#endif

BEGIN_EVENT_TABLE(GDSMainWindow, wxFrame)

         EVT_MENU( BFILE_NEW 						,GDSMainWindow::CmMenu)

         EVT_MENU( BFILE_OPEN 						,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_OPEN_GDSII 				,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_OPEN_KEY 				,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_OPEN_FLASH 				,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_OPEN_EMASK 				,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_SAVE 						,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_SAVE_AS_GDSII 			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_SAVE_AS_KEY 			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_SAVE_AS_SVG 			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_SAVE_AS_FLASH 			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_SAVE_AS_EMASK 			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_SAVE_AS_CNC 			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_SAVE_AS_GBR 			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_SAVE_AS_EXL 			,GDSMainWindow::CmMenu)
			EVT_MENU( BFILE_SAVE_AS_PNG				,GDSMainWindow::CmMenu)
			EVT_MENU( BFILE_SAVE_AS_BMP				,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_IMPORT_GDSII 			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_IMPORT_KEY 				,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_IMPORT_FLASH 			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_IMPORT_EMASK 			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_CLOSE 					,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_LOAD_SETUP 				,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_SAVE_SETUP 				,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_PRINT 					,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_PRINT_SETUP 			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_EXIT 						,GDSMainWindow::CmMenu)

         EVT_MENU( BFILE_HISTORY					,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_HISTORY	+ 1			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_HISTORY	+ 2			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_HISTORY	+ 3			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_HISTORY	+ 4			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_HISTORY	+ 5			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_HISTORY	+ 6			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_HISTORY	+ 7			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_HISTORY	+ 8			,GDSMainWindow::CmMenu)
         EVT_MENU( BFILE_HISTORY	+ 9			,GDSMainWindow::CmMenu)
         EVT_MENU( AUTOMATIC_LOAD 				,GDSMainWindow::CmMenu)
         EVT_MENU( AUTOMATIC_SAVE 				,GDSMainWindow::CmMenu)
         EVT_MENU( AUTOMATIC_STEPMODE			,GDSMainWindow::CmMenu)
         EVT_MENU( EXEC_DIALOG_SHOW				,GDSMainWindow::CmMenu)
         EVT_MENU( DIST_DIALOG_SHOW				,GDSMainWindow::CmMenu)
         EVT_MENU( IDENT_DIALOG_SHOW			,GDSMainWindow::CmMenu)
         EVT_MENU( GROUPS_DEFINE 				,GDSMainWindow::CmMenu)
		   EVT_MENU( GROUPASELECTED				,GDSMainWindow::CmMenu)
		   EVT_MENU( GROUPBSELECTED				,GDSMainWindow::CmMenu)
      	EVT_MENU( GROUPDESTINATION				,GDSMainWindow::CmMenu)
         EVT_MENU( SELECTED_ONLY					,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATION_STRUCTURE_DEFAULT,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATION_STRUCTURE_NAMED	,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATION_FLATTEN_ONE   	,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATION_FLATTEN 			,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATION_PROPERTY 			,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATION_SELECT 			,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATION_UNSELECT 			,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATION_DELETE 			,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATION_TRANSFORM			,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATIONA_DELETE 			,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATIONA_ADDPROP 			,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATIONA_TRANSFORM		,GDSMainWindow::CmMenu)
			EVT_MENU( OPERATIONA_CALCAREA       ,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATIONA2D_CREATE_RING 	,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATIONA2D_CORRECTION 	,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATIONA2D_COPY 			,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATIONA2D_MOVE 			,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATIONA2D_MERGE 			,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATIONA2D_TO_ARCS 		,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATIONA2D_TO_LINES 		,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATIONA2D_SMOOTHEN 		,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATIONAB2D_INTERSECT 	,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATIONAB2D_MERGE 		,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATIONAB2D_AMINB 		,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATIONAB2D_BMINA 		,GDSMainWindow::CmMenu)
         EVT_MENU( OPERATIONAB2D_EXOR 			,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_ZOOMIN			 		,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_ZOOMOUT			 		,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_ZOOMBACK			 		,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_SELECT 			 		,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_DESELECT 		 		,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_INSERTLAYER		 		,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_PATHTYPE			 		,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_POLYGON 			 		,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_POLYLINE			 		,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_RECTANGLE		 		,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_CIRCLE			 		,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_CIRCLE2			 		,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_EDIT				 		,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_DELETE			 		,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_TEXT                 ,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_MOVE						,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_COPY						,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_CURSOR					,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_EDITPROPERTY			,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_INSERT_REF				,GDSMainWindow::CmMenu)
         EVT_MENU( TOOL_INSERT_STRUC			,GDSMainWindow::CmMenu)
         EVT_MENU( VIEW_LAYER_PROPERTIES 		,GDSMainWindow::CmMenu)
         EVT_MENU( VIEW_LAYER_ORDER 			,GDSMainWindow::CmMenu)
         EVT_MENU( VIEW_STRUCTURE 		   	,GDSMainWindow::CmMenu)
         EVT_MENU( VIEW_COORDINATES 			,GDSMainWindow::CmMenu)
         EVT_MENU( VIEW_DRAW_ONLY_OUTLINES 	,GDSMainWindow::CmMenu)
         EVT_MENU( PREFERENCES_FACTORS 		,GDSMainWindow::CmMenu)
         EVT_MENU( VISIBLE_ONLY					,GDSMainWindow::CmMenu)
         EVT_MENU( SAVE_SETUP_ON_EXIT			,GDSMainWindow::CmMenu)
         EVT_MENU( SHOW_GRID						,GDSMainWindow::CmMenu)
         EVT_MENU( GRID_SET						,GDSMainWindow::CmMenu)
         EVT_MENU( UNITS_SET						,GDSMainWindow::CmMenu)
         EVT_MENU( HELP                      ,GDSMainWindow::CmMenu)
         EVT_MENU( CONTEXT_HELP              ,GDSMainWindow::CmMenu)
         EVT_MENU( HELP_ABOUT 					,GDSMainWindow::CmMenu)
         EVT_MENU( REGISTER                  ,GDSMainWindow::CmMenu)
         EVT_MENU( CM_UNDO						  ,GDSMainWindow::CmUndo)
         EVT_MENU( CM_CANCEL	  				  ,GDSMainWindow::CmCancel)
         EVT_MENU( CM_GDSZOOMBACK			  ,GDSMainWindow::CmZoomBack)
         EVT_MENU( CM_GDSZOOMOUT				  ,GDSMainWindow::CmZoomOut)
         EVT_MENU( CM_GDSZOOMOUT2			  ,GDSMainWindow::CmZoomOut2)
         EVT_MENU( CM_VIEWSTRUCTURE			  ,GDSMainWindow::CmViewStructure)
         EVT_MENU( CM_GDSLAYERS				  ,GDSMainWindow::ShowLayers)
         EVT_MENU( CM_RELATIVE_START		  ,GDSMainWindow::SetRelStart)
         EVT_MENU( CM_CALC_DISTANCE			  ,GDSMainWindow::CalcDistance)
		   EVT_TOOL_RANGE(0, 50, GDSMainWindow::OnToolLeftClick)

    		EVT_TOOL_ENTER(ID_TOOLBAR, 			 GDSMainWindow::OnToolEnter)
         EVT_IDLE( GDSMainWindow::OnIdle)
			EVT_CLOSE(                           GDSMainWindow::OnCloseWindow)

END_EVENT_TABLE()

void GDSMainWindow::CmMenu(wxCommandEvent& event)
{
   wxCommandEvent a = wxCommandEvent(); 

   set_Cursor("wait");
	try
	{
//		switch (event.m_commandInt)
		switch (event.GetId())
      {
         case BFILE_NEW  						: CmShowNewfileDialog(); break;
         case BFILE_OPEN 						: CmFileOpen(); break;
         case BFILE_OPEN_GDSII 				: CmFileOpenGDS(); break;
         case BFILE_OPEN_KEY 					: CmFileOpenKEY(); break;
         case BFILE_OPEN_FLASH 				: CmFileOpenFLASH(); break;
         case BFILE_OPEN_EMASK 				: CmFileOpenEMASK(); break;
         case BFILE_SAVE 						: CmFileSave(); break;
         case BFILE_SAVE_AS 					: break;
         case BFILE_SAVE_AS_GDSII 			: CmFileSaveGDS(); break;
         case BFILE_SAVE_AS_KEY 				: CmFileSaveKEY(); break;
         case BFILE_SAVE_AS_SVG 				: CmFileSaveSVG(); break;
         case BFILE_SAVE_AS_FLASH 			: CmFileSaveFLASH(); break;
         case BFILE_SAVE_AS_EMASK 			: CmFileSaveEMASK(); break;
         case BFILE_SAVE_AS_CNC 				: CmFileSaveCNC();break;
         case BFILE_SAVE_AS_GBR 				: CmFileSaveGBR();break;
         case BFILE_SAVE_AS_EXL 				: CmFileSaveExelon();break;
 		   case BFILE_SAVE_AS_PNG				: CmFilePNG(); break;
 		   case BFILE_SAVE_AS_BMP				: CmFileBMP(); break;
         case BFILE_IMPORT_GDSII 				: CmFileImportGDS(); break;
         case BFILE_IMPORT_KEY 				: CmFileImportKEY(); break;
         case BFILE_IMPORT_FLASH 				: CmFileImportFLASH(); break;
         case BFILE_IMPORT_EMASK 				: CmFileImportEMASK(); break;
         case BFILE_CLOSE 						: CmFileClose(); break;
         case BFILE_LOAD_SETUP 				: CmLoadSetup(); break;
         case BFILE_SAVE_SETUP 				: CmSaveSetup(); break;
         case BFILE_PRINT 						: CmFilePrint(); break;
         case BFILE_PRINT_SETUP 				: CmFilePrintSetup(); break;
         case BFILE_EXIT 						: CmFileQuit(); break;
         case BFILE_HISTORY						:
         case BFILE_HISTORY	+ 1				:
         case BFILE_HISTORY	+ 2				:
         case BFILE_HISTORY	+ 3				:
         case BFILE_HISTORY	+ 4				:
         case BFILE_HISTORY	+ 5				:
         case BFILE_HISTORY	+ 6				:
         case BFILE_HISTORY	+ 7				:
         case BFILE_HISTORY	+ 8				:
         case BFILE_HISTORY	+ 9				: CmOpenOldFile(event); break;
         case AUTOMATIC_LOAD 					: CmLoadProces(); break;
         case AUTOMATIC_SAVE 					: CmRecord(); break;
         case AUTOMATIC_STEPMODE				: if (GLOB->CMDH->GetStepMode())
	         											  CmStepMode(false);
                                            else
	         											  CmStepMode(true);
         											  break;
         case EXEC_DIALOG_SHOW				: GLOB->CMDH->DoCommand("showexecdlg"); break;
         case GROUPS_DEFINE 					: GLOB->CMDH->DoCommand("showgroupsdlg"); break;
		   case GROUPASELECTED					: GLOB->CMDH->DoCommand("group_a selected");break;
		   case GROUPBSELECTED					: GLOB->CMDH->DoCommand("group_b selected");break;
      	case GROUPDESTINATION				: CmDestination();break;
         case OPERATION 						: break;
         case SELECTED_ONLY					: CmSelectedOnly(); break;
         case OPERATION_STRUCTURE_DEFAULT	: CmMakeStructure(false); break;
         case OPERATION_STRUCTURE_NAMED	: CmMakeStructure(true); break;
         case OPERATION_FLATTEN_ONE			: Operation("flatten one"); break;
         case OPERATION_FLATTEN 				: Operation("flatten"); break;
         case OPERATION_PROPERTY				: CmProperty(false); break;
         case OPERATION_SELECT 				: Operation("select all true"); break;
         case OPERATION_UNSELECT 			: Operation("select all false"); break;
         case OPERATION_DELETE 				: Operation("delete"); break;
         case OPERATION_TRANSFORM			: CmTransform(); break;
			case OPERATIONA_CALCAREA         : CmCalcArea();break;
         case OPERATIONA 						: break;
         case OPERATIONA_DELETE 				: Operation("delete_a"); break;
         case OPERATIONA_ADDPROP				: CmProperty(true); break;
         case OPERATIONA_TRANSFORM			: CmTransformA(); break;
         case OPERATIONA2D 					: break;
         case OPERATIONA2D_CREATE_RING 	: Operation("makering"); break;
         case OPERATIONA2D_CORRECTION 		: Operation("correction"); break;
         case OPERATIONA2D_COPY 				: Operation("copy_a"); break;
         case OPERATIONA2D_MOVE 				: Operation("move_a"); break;
         case OPERATIONA2D_MERGE 			: Operation("merge"); break;
         case OPERATIONA2D_TO_ARCS 			: Operation("toarcs"); break;
         case OPERATIONA2D_TO_LINES 		: Operation("tolines"); break;
         case OPERATIONA2D_SMOOTHEN 		: Operation("smoothen"); break;
         case OPERATIONAB2D 					: break;
         case OPERATIONAB2D_INTERSECT 		: Operation("intersect"); break;
         case OPERATIONAB2D_MERGE 			: Operation("merge"); break;
         case OPERATIONAB2D_AMINB 			: Operation("a_subtract_b"); break;
         case OPERATIONAB2D_BMINA 			: Operation("b_subtract_a"); break;
         case OPERATIONAB2D_EXOR 			: Operation("exor"); break;
         case TOOL_ZOOMIN			 		   : GLOB->CMDH->DoCommand("tool zoom"); break;
         case TOOL_ZOOMOUT			 		   : GLOB->CMDH->DoCommand("zoomout"); break;
         case TOOL_ZOOMBACK		 		   : GLOB->CMDH->DoCommand("zoomback"); break;
         case TOOL_SELECT 			 		   : GLOB->CMDH->DoCommand("tool select"); break;
         case TOOL_DESELECT 		 			: GLOB->CMDH->DoCommand("tool deselect"); break;
         case TOOL_POLYGON 			 		: GLOB->CMDH->DoCommand("tool polygon"); break;
         case TOOL_POLYLINE 			 		: GLOB->CMDH->DoCommand("tool polyline"); break;
         case TOOL_RECTANGLE		 	   	: GLOB->CMDH->DoCommand("tool rectangle"); break;
         case TOOL_CIRCLE 			 		   : GLOB->CMDH->DoCommand("tool circle"); break;
         case TOOL_CIRCLE2			 		   : GLOB->CMDH->DoCommand("tool circle2"); break;
         case TOOL_EDIT				 		   : GLOB->CMDH->DoCommand("tool edit"); break;
         case TOOL_DELETE			 		   : GLOB->CMDH->DoCommand("tool delete"); break;
         case TOOL_INSERTLAYER		 		: GLOB->CMDH->DoCommand("inslayerdlg"); break;
         case TOOL_TEXT				 			: GLOB->CMDH->DoCommand("tool text"); break;
         case TOOL_MOVE                   : GLOB->CMDH->DoCommand("tool move"); break;
         case TOOL_CURSOR                 : GLOB->CMDH->DoCommand("showcursordlg"); break;
         case TOOL_EDITPROPERTY				: GLOB->CMDH->DoCommand("tool editproperty"); break;
         case TOOL_INSERT_REF					: GLOB->CMDH->DoCommand("tool insertreference"); break;
         case TOOL_INSERT_STRUC				: GLOB->CMDH->DoCommand("tool insertstructure"); break;
         case DIST_DIALOG_SHOW				: GLOB->CMDH->DoCommand("showdistdlg"); break;
         case IDENT_DIALOG_SHOW				: GLOB->CMDH->DoCommand("showidentdlg"); break;
         case TOOL_COPY                   : GLOB->CMDH->DoCommand("tool copy"); break;
         case TOOL_PATHTYPE			 		: GLOB->CMDH->DoCommand("pathtypedlg"); break;
         case VIEW_LAYER_PROPERTIES 		: GLOB->CMDH->DoCommand("showlayersdlg"); break;
         case VIEW_LAYER_ORDER 				: GLOB->CMDH->DoCommand("showorderdlg") ; break;
         case VIEW_STRUCTURE 					: CmViewStructure(a); break;
         case VIEW_DRAW_ONLY_OUTLINES 		: CmViewWireFrame(); break;
         case VIEW_COORDINATES 				: CmViewCoordinates(); break;
         case PREFERENCES_FACTORS 			: GLOB->CMDH->DoCommand("showfactorsdlg"); break;
         case VISIBLE_ONLY						: CmVisibleOnly(); break;
         case SAVE_SETUP_ON_EXIT				: CmSaveSetupOnExit(); break;
         case SHOW_GRID							: CmShowGrid(); break;
         case GRID_SET							: CmShowGridSettings(); break;
         case UNITS_SET							: CmShowUnitsSettings(); break;
         case HELP                        : CmHelp(); break;
         case CONTEXT_HELP                : CmContextHelp(); break;
         case HELP_ABOUT 						: CmHelpAbout(); break;
         case REGISTER                    : CmRegisterApplication(); break;
      }
	}
   catch (GDS_Error& _error) {
      _messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
   }
   catch(...) {
      _messagehandler->info("SOMETHING WENT WRONG leave program","NON HANDLED ERROR");
      return;
   }
	GetMenuBar()->Enable(VIEW_LAYER_PROPERTIES,true);
	GetMenuBar()->Enable(VIEW_LAYER_ORDER,true);
	GetMenuBar()->Enable(DEFINEGROUPS,true);
	set_Cursor("ready");
}

GDSMainWindow::GDSMainWindow(wxFrame *frame, char *title, int x, int y, int w, int h ):
   wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h), wxDEFAULT_FRAME_STYLE)
   //this one gives problems with toolbars
   //,wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxTHICK_FRAME | wxSYSTEM_MENU | wxCAPTION)
{

	::wxInitAllImageHandlers();

	// just to make sure, initialize all pointers to NULL
	_cursorWait = wxNullCursor;
	_cursorArrow = wxNullCursor;
	_cursorWait = wxNullCursor;
	_dragDC = NULL;
	_zoom_pen = NULL;
	_mousemenu = NULL;
	_printing = 0;
	_nbLoadedFiles = 0;
	_aboutDialog = NULL;
   _execDialog = NULL;
   _debugerDialog = NULL;
   _stopDialog = NULL;
   _insertDialog = NULL;
   _distDialog = NULL;
   _identDialog = NULL;
	_layerorderDialog = NULL;
	_layerpropertiesDialog = NULL;
   _cursorDialog = NULL;
	_factorsDialog = NULL;
   _settingsDialog = NULL;
   _textinsDialog = NULL;
#if wxUSE_PRINTING_ARCHITECTURE
	_printer = NULL;
	_printout = NULL;
#endif //wxUSE_PRINTING_ARCHITECTURE
	_execDlgVisible = false;
	_insertDlgVisible = false;
   _distDlgVisible = false;
	_identDlgVisible = false;
   _exit=false;

	_cursorWait 	= wxCursor(wxCURSOR_WAIT);
	_cursorArrow 	= wxCursor(wxCURSOR_ARROW);
	_cursorHelp 	= wxCursor(wxCURSOR_QUESTION_ARROW);

   m_dummyFrame= new wxFrame(this, -1,"foo");
  	InitToolbar();

	Canvas*			canvas;					// the canvas window
	canvas = new Canvas(this,0,0,w,h, /* wxRETAINED |*/ wxBORDER ,"GDSII_Canvas");
   canvas->SetFocus();

   GLOB->CANVAS=canvas;

	_winstatusbar 	= new WinStatusBar(this);
	_statusbar    	= (StatusBar *) _winstatusbar;

	DrawDriver* _draw_driver = new DrawDriver();
   GLOB->SCRDRV=_draw_driver;
	GLOB->SCRDRV->Init();

   //this is the right place since it pushes event handlers in the above
   GLOB->TOOLCTRL= new ToolControl();

	// make a popupmenu for the right mousebutton
	_mousemenu = new wxMenu("POPME",(long)0);
	_mousemenu->Append(CM_CANCEL, "endcommand");
	_mousemenu->Append(CM_UNDO, "undo");
	_mousemenu->Append(CM_GDSZOOMBACK, "Zoom Undo");
	_mousemenu->Append(CM_GDSZOOMOUT, "Zoom out");
	_mousemenu->Append(CM_GDSZOOMOUT2, "Zoom out 2X");
	_mousemenu->AppendSeparator();
	_mousemenu->Append(CM_VIEWSTRUCTURE, "View &structure...");
	_mousemenu->Append(CM_GDSLAYERS, "View &layers...");
	_mousemenu->AppendSeparator();
	_mousemenu->Append(CM_RELATIVE_START, "Set &relative start");
	_mousemenu->Append(CM_CALC_DISTANCE, "&Calculate distance");
}

GDSMainWindow::~GDSMainWindow ()
{
#if wxUSE_PRINTING_ARCHITECTURE
	delete _printout;
	delete _printer;
#endif //wxUSE_PRINTING_ARCHITECTURE
	delete _mousemenu;
	delete GLOB->SCRDRV;
	delete _winstatusbar;
}

void GDSMainWindow::SetUpWindow()
{
	// initialize subwindows (Dialogs)
/*
#ifdef __UNIX__
	style_top = (wxSTAY_ON_TOP |wxRESIZE_BORDER |wxTHICK_FRAME | wxCAPTION | wxUSER_COLOURS);
	style = (wxRESIZE_BORDER |wxTHICK_FRAME | wxCAPTION | wxUSER_COLOURS);
	style_noresize = (wxTHICK_FRAME | wxCAPTION | wxUSER_COLOURS);
#else
	style_top = (wxSTAY_ON_TOP |wxRESIZE_BORDER |wxTHICK_FRAME | wxSYSTEM_MENU | wxCAPTION);
	style = (wxRESIZE_BORDER |wxTHICK_FRAME | wxSYSTEM_MENU | wxCAPTION);
	style_noresize = (wxTHICK_FRAME |  wxSYSTEM_MENU | wxCAPTION);
#endif
*/

	_aboutDialog = new About(this,"About",style_noresize);
	_waitDialog = new Wait(this,"please wait...",0,0,0,0,style);
	_structDialog = new StructuresDialog(this,style_top);

	_gridDialog = new Gridsettings(this,style_noresize);
	_layerorderDialog = new LayerOrder(this,"Layer Order Dialog",style);
	_unitsDialog = new Unitssettings(this,style_noresize);
	_execDialog = new ExecDlg(this,"Execution Dialog",style_top);
	_stopDialog = new StopDialog(this,"stop processing",style_top);
	_debugerDialog = new DebugDialog(this,"Debug Dialog",style_top);
	_insertDialog = new InsLayer(this,"Insert Layer",wxDEFAULT_FRAME_STYLE );
//   				wxMAXIMIZE_BOX|wxMINIMIZE_BOX |wxRESIZE_BORDER|wxTHICK_FRAME  | wxSTAY_ON_TOP |wxSYSTEM_MENU |wxTINY_CAPTION_HORIZ);
	_distDialog = new DistanceDlg(this,"Distance Dialog",style_top);
	_identDialog = new IdentDlg(this,"Identification Dialog",style_top);
   _groupDialog = new GroupDialog(this,style_noresize);
	_factorsDialog = new Factors(this,style_noresize);
	_transDialog = new Transformations(this,style_noresize);
   _cursorDialog = new CursorDialog(this,style_noresize);
   _settingsDialog = new SettingsDialog(this,style_noresize);
   _newfileDialog = new NewFileDialog(this,style_noresize);
	_layerpropertiesDialog = new LayerPropertiesDialog(this,"Layer Properties",style_noresize);

#if wxUSE_PRINTING_ARCHITECTURE
	_printer = new wxPrinter();
	_printout = new PrintDriver(this,PROGRAM_NAME);
#endif //wxUSE_PRINTING_ARCHITECTURE
}

void GDSMainWindow::Exit()
{
//  Close(true); //this closes down the whole graphical interface
  _exit=true;
}

void GDSMainWindow::OnIdle(wxIdleEvent& event)
{
   if (_exit)
      Close(true);
   else
   {
      if (GLOB->INITIALIZED)
      if (GLOB->SETUP->Get_UpdateAvailableLayers() || !GLOB->CANVAS->Get_Valid_drawing())
      {
         ::wxSetCursor(_cursorWait);
//no need for it yet         
//         GLOB->CANVAS->Update();
//         GLOB->CANVAS->Refresh(true,NULL);
			GLOB->CMDH->Draw();
         SetCursor(_cursorArrow);
         ::wxSetCursor(_cursorArrow);
      }
   }

}

void GDSMainWindow::OnCloseWindow( wxCloseEvent&)
{
	if (GLOB->CMDH->Get_GDSII())
   	CmFileClose(); // close the active file (needed when program is not closed using the menu)
	// save the setup first

   if (GLOB->CMDH->GetSaveSetUpOnExit() && !GLOB->CMDH->GetViewer())
		GLOB->CMDH->DoCommand("savesetup %s","@{setupfile}");
	GLOB->CMDH->SaveHistory();

	// now delete everything
   if (GLOB->TOOLCTRL)
	   delete GLOB->TOOLCTRL;
   Destroy();
	_aboutDialog->Show(false);
	_distDialog->Show(false);
	_execDialog->Show(false);
	_debugerDialog->Show(false);
	_stopDialog->Show(false);
	_insertDialog->Show(false);
	_identDialog->Show(false);
	_waitDialog->Show(false);
	_structDialog->Show(false);
	_groupDialog->Show(false);
	_layerorderDialog->Show(false);
	_factorsDialog->Show(false);
	_transDialog->Show(false);
	_cursorDialog->Show(false);
	_settingsDialog->Show(false);
	_gridDialog->Show(false);
	_unitsDialog->Show(false);
	_layerpropertiesDialog->Show(false);
	_newfileDialog->Show(false);
	//_textinsDialog->Show(false);
}


/*void GDSMainWindow::OnMenuSelect(int id)
{
  wxBeginBusyCursor(_cursorHelp);
		::wxSetCursor(_cursorHelp);
      SetCursor(_cursorHelp);
		_canvas->SetCursor(_cursorHelp);
      GetMenuBar()->SetCursor(_cursorHelp);    // doesn't seem to have any effect...
}*/

//NOT USED
void GDSMainWindow::OnSize(wxSizeEvent& event)
{
	wxFrame::OnSize(event);
}

void GDSMainWindow::set_Cursor(char* type)
{
	if (strcmp(type,"wait")==0)
   {
     	SetCursor(_cursorWait);
		GLOB->CANVAS->SetCursor(_cursorWait);
		GetToolBar()->SetCursor(_cursorWait);
		if (_execDlgVisible)
     	   	_execDialog->SetCursor(_cursorWait);
   }
	else if (strcmp(type,"ready")==0)
   {
   	SetCursor(_cursorArrow);
		GLOB->CANVAS->SetCursor(_cursorArrow);
 		GetToolBar()->SetCursor(_cursorArrow);
		if (_execDlgVisible)
      {
     	   	_execDialog->SetCursor(_cursorArrow);
      }
	}
	else if (strcmp(type,"help")==0)
   {
/*   wxBeginBusyCursor(_cursorHelp);
		::wxSetCursor(_cursorHelp);
      SetCursor(_cursorHelp);
		GLOB->CANVAS->SetCursor(_cursorHelp);
      GetMenuBar()->SetCursor(_cursorHelp);    // doesn't seem to have any effect...
		if (_execDlgVisible)
      {
     	   	_execDialog->SetCursor(_cursorHelp);
      }*/
	}
//	while (wxGetApp().Pending())
//		wxGetApp().Dispatch();
}

void GDSMainWindow::CmDestination()
{
   wxString result;
   long destination=0;
	Ask("Give destination layer:",result);
   result.ToLong(&destination);
	GLOB->CMDH->DoCommand("destination %d",destination);
}

void GDSMainWindow::ShowPathtypeDlg()
{
	_settingsDialog->SetupWindow();           
}


void GDSMainWindow::ShowTextDlg(wxCommandEvent&)
{
	TextinsDialog textdlg(GLOB->MAINWINDOW);
   textdlg.ShowModal();                     
}

void GDSMainWindow::CmTransform()
{
	set_Cursor("wait");
	_transDialog->SetupWindow(TRUE);
	set_Cursor("ready");
}

void GDSMainWindow::CmTransformA()
{
	set_Cursor("wait");
	_transDialog->SetupWindow(FALSE);
   _transDialog->ShowModal();
	set_Cursor("ready");
}

void GDSMainWindow::CmCalcArea()
{
	set_Cursor("wait");
	GLOB->CMDH->DoCommand("calcarea_a layerarea");
	GLOB->CMDH->DoCommand("showalias layerarea");
	set_Cursor("ready");
}

void GDSMainWindow::CmProperty(bool atlayer)
{
	long style = (wxTHICK_FRAME | wxSYSTEM_MENU | wxCAPTION);
   PropAddDialog a_prop(this,"add a property",style ,"propadd");
   if (a_prop.ShowModal())
   {

		set_Cursor("wait");
      char name[80];
      strcpy(name,a_prop.GetName());
      char type[80];
      strcpy(type,a_prop.GetType());
      char value[80];
      strcpy(value,a_prop.GetValue());
      if (atlayer)
			GLOB->CMDH->DoCommand("addproperty_a  %s %s %s",name,type,value);
		else
      	GLOB->CMDH->DoCommand("addproperty  %s %s %s",name,type,value);
		set_Cursor("ready");
   }
}

void GDSMainWindow::Redraw()
{
	set_Cursor("wait");
	// not realy optimized
	if (GLOB->CMDH->Get_GDSII())
   	GLOB->CMDH->Draw();
	set_Cursor("ready");
}

void GDSMainWindow::CmFileOpen()
{
	wxString name;
	char filetype[8];

   GLOB->CMDH->DoCommand("askfile  %s"," {Open file} OPEN {*.*} @{file_path} {*} curfile");
	set_Cursor("wait");
	GLOB->CMDH->GetAlias("curfile",name);

  if ( !name.IsEmpty() )	//New position (Olly) - Stops file open error when cancelled
  {							//New position (Olly) - (i.e. doesn't switch on empty string)
	switch (name.Last()) {
		case 's':
		case 'l': strcpy(filetype,"gds-ii"); break;
		case 'y': strcpy(filetype,"key"); break;
		case 'm': strcpy(filetype,"flash"); break;
		case 'e': strcpy(filetype,"emask"); break;
		default: _messagehandler->info("unknown file type, unable to load.","load error");
					set_Cursor("ready");
					return;
	}

//	if ( !name.IsEmpty() )	//Line moved to above switch
//   {						//Line moved to above switch

		try {
			GLOB->CMDH->DoCommand("loadfile \"%s\" %s","@{curfile}",filetype);
			GLOB->CMDH->DoCommand("draw");
      	GLOB->CMDH->AddFileToHistory(name.c_str(),filetype);
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());

			// if there is memory allocated for the file -> delete it
			if (GLOB->CMDH->Get_GDSII()) GLOB->CMDH->DoCommand("closefile");
			set_Cursor("ready");
			return;
		}
		catch(...) {
			_messagehandler->info("Unknown error while loading file.","CmFileOpen(), Load error");
			set_Cursor("ready");
			return;
		}
	}
	set_Cursor("ready");
}



// response function to menuitem load GDS-II file
// also used when it's button is pressed on the controlbar
void GDSMainWindow::CmFileOpenGDS()
{
	wxString name;
   GLOB->CMDH->DoCommand("askfile  %s"," {Open GDS-II file} OPEN {GDSII file (*.cal)|*.cal| GDS file (*.db)|*.db |GDS file (*.gds)|*.gds|ALL (*.*)|*.*} @{file_path} @{file_ext} curfile");
	set_Cursor("wait");

	GLOB->CMDH->GetAlias("curfile",name);

	if ( !name.IsEmpty() )
   {
		try {
			GLOB->CMDH->DoCommand("loadfile \"%s\" gds-ii","@{curfile}");
      	GLOB->CMDH->AddFileToHistory(name.c_str(),"gds-ii");
			GLOB->CMDH->DoCommand("draw");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());

			// if there is memory allocated for the file -> delete it
			if (GLOB->CMDH->Get_GDSII()) GLOB->CMDH->DoCommand("closefile");
			set_Cursor("ready");
			return;
		}
		catch(...) {
			_messagehandler->info("Unknown error while loading file.","CmFileOpenGDS(), Load error");
			set_Cursor("ready");
			return;
		}
	}
   else
	   ;
//		GDS_loaded(FALSE);    just keep the file that is in memory...

	set_Cursor("ready");
}

// response function to menuitem load FLASH file
// also used when it's button is pressed on the controlbar
void GDSMainWindow::CmFileOpenFLASH()
{
	wxString name;

   GLOB->CMDH->DoCommand("askfile  %s","{Open Flash file} OPEN {FLASH file (*.??m)|*.??m|ALL (*.*)|*.*}  @{file_path} @{file_ext} curfile");
	set_Cursor("wait");
	GLOB->CMDH->GetAlias("curfile",name);

	 if ( !name.IsEmpty() )
    {
		try {
			wxString result="0";
			Ask("Into which layer would you like to insert the drawing?",result);
		   GLOB->CMDH->DoCommand("setlayer %d",atoi(result.c_str())); //in case of wrong input a zero will be returned!!

			GLOB->CMDH->DoCommand("loadfile \"%s\" flash","@{curfile}");
      	GLOB->CMDH->AddFileToHistory(name.c_str(),"flash");
			GLOB->CMDH->DoCommand("draw");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());

			// if there is memory allocated for the file -> delete it
			if (GLOB->CMDH->Get_GDSII()) GLOB->CMDH->DoCommand("closefile");
	      set_Cursor("ready");
			return;
		}
		catch(...) {
			_messagehandler->info("Unknown error while loading file.","CmFileOpenFLASH(), Load error");
			set_Cursor("ready");
			return;
		}
		set_Cursor("ready");
	}
}

// response function to menuitem load EMASK file
// also used when it's button is pressed on the controlbar
void GDSMainWindow::CmFileOpenEMASK()
{
	wxString name;

   GLOB->CMDH->DoCommand("askfile  %s","{Open EMASK file} OPEN {EMASK file (*.emk)|*.emk|EMASK file (*.em)|*.em|ALL (*.*)|*.*}  @{file_path} @{file_ext} curfile");
	set_Cursor("wait");
	GLOB->CMDH->GetAlias("curfile",name);

	 if ( !name.IsEmpty() )
    {
		try {
			wxString result="0";
			Ask("Into which layer would you like to insert the drawing?",result);
		   GLOB->CMDH->DoCommand("setlayer %d",atoi(result.c_str())); //in case of wrong input a zero will be returned!!

			GLOB->CMDH->DoCommand("loadfile \"%s\" emask","@{curfile}");
      	GLOB->CMDH->AddFileToHistory(name.c_str(),"emask");
			GLOB->CMDH->DoCommand("draw");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());

			// if there is memory allocated for the file -> delete it
			if (GLOB->CMDH->Get_GDSII()) GLOB->CMDH->DoCommand("closefile");
	      set_Cursor("ready");
			return;
		}
		catch(...) {
			_messagehandler->info("Unknown error while loading file.","CmFileOpenEMASK(), Load error");
			set_Cursor("ready");
			return;
		}
		set_Cursor("ready");
	}
}

// response function to menuitem load KEY file
// also used when it's button is pressed on the controlbar
void GDSMainWindow::CmFileOpenKEY()
{
	wxString name;

   GLOB->CMDH->DoCommand("askfile  %s","{Open Key file} OPEN {KEY file (*.key)|*.key|ALL (*.*)|*.*}  @{file_path} @{file_ext} curfile");
   set_Cursor("wait");
	GLOB->CMDH->GetAlias("curfile",name);

	 if ( !name.IsEmpty() )
    {
		try {
			GLOB->CMDH->DoCommand("loadfile \"%s\" key","@{curfile}");
      	GLOB->CMDH->AddFileToHistory(name.c_str(),"key");
			GLOB->CMDH->DoCommand("draw");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());

			// if there is memory allocated for the file -> delete it
			if (GLOB->CMDH->Get_GDSII()) GLOB->CMDH->DoCommand("closefile");
		   set_Cursor("ready");
			return;
		}
		catch(...) {
			_messagehandler->info("Unknown error while loading file.","CmFileOpenKEY(), Load error");
		   set_Cursor("ready");
			return;
		}
	}
   set_Cursor("ready");
}


// response function to filename in the filemenui
void GDSMainWindow::CmOpenOldFile(wxCommandEvent& event)
{
   set_Cursor("wait");
	wxString namewx;
	// get the filename from the menu
   wxMenuBar* menu=GetMenuBar();
	namewx=menu->GetLabel(event.GetId());
   char name[100];
	strcpy(name,namewx);
   char* c=name;
	while (*c != '\0')
	{
   	if (*c=='\\') *c='/';
   	c++;
	}

	if (strlen(name)>0)
   {
		try
      {
         char type[20];
         {
            DL_Iter<Alias*> I(GLOB->CMDH->_file_history);
            I.toroot();
            while (I.iterate() && strcmp(I.item()->GetAlias(),name) );
            if (!I.hitroot())
            {
               strcpy(type,I.item()->GetValue());
               if (!strcmp(type,"flash") || !strcmp(type,"emask"))
               {
                  wxString result="0";
                  Ask("Into which layer would you like to insert the drawing?",result);
					   GLOB->CMDH->DoCommand("setlayer %d",atoi(result.c_str())); //in case of wrong input a zero will be returned!!
               }
            }
         }
         if (!strcmp(type,"prs"))
         {
		      if (GLOB->CMDH->GetStepMode())
            {
        		   _debugerDialog->Init(name);
            }
            else
            {
               _stopDialog->Show(true);
				   GLOB->CMDH->SetContinue(true);
				   ::wxYield(); //need to give stopdialog a change to built itself
              	GLOB->CMDH->DoCommand("loadprocess  %s", name);
               _stopDialog->Show(false);
				   GLOB->CMDH->SetContinue(true);
            }
         	GLOB->CMDH->AddFileToHistory(name,"prs");
         }
         else
         {
            GLOB->CMDH->DoCommand("loadfile \"%s\" %s", name,type);
         	GLOB->CMDH->AddFileToHistory(name,type);
         }

         GLOB->CMDH->DoCommand("draw");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());

			// if there is memory allocated for the file -> delete it
			if (GLOB->CMDH->Get_GDSII()) GLOB->CMDH->DoCommand("closefile");
		   set_Cursor("ready");
			return;
		}
		catch(...) {
			_messagehandler->info("Unknown error while loading.", "Load Error");
		   set_Cursor("ready");
			return;
		}
	}
   set_Cursor("ready");
}

void GDSMainWindow::CmFileImportGDS()
{
	wxString name;

   GLOB->CMDH->DoCommand("askfile  %s","{Open GDS-II file} OPEN {GDSII file (*.cal)|*.cal| GDS file (*.db)|*.db|GDS file (*.gds)|*.gds|ALL (*.*)|*.*} @{file_path} @{file_ext} mergefile");
   set_Cursor("wait");
	GLOB->CMDH->GetAlias("mergefile",name);

	if ( !name.IsEmpty() )
   {
		try {
			GLOB->CMDH->DoCommand("mergefile  \"%s\" gds-ii","@{mergefile}");
			GLOB->CMDH->DoCommand("draw");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
		   set_Cursor("ready");
			return;
		}
		catch(...) {
			_messagehandler->info("Unknown error while loading file.","CmFileImportGDS(), Load error");
		   set_Cursor("ready");
			return;
		}
	}
   set_Cursor("ready");
}

void GDSMainWindow::CmFileImportKEY()
{
	wxString name;

   GLOB->CMDH->DoCommand("askfile  %s", "{Open Key file} OPEN {KEY file (*.key)|*.key|ALL (*.*)|*.*}  @{file_path} @{file_ext} mergefile");
   set_Cursor("wait");
	GLOB->CMDH->GetAlias("mergefile",name);

	if ( !name.IsEmpty() )
   {
		try {
			GLOB->CMDH->DoCommand("mergefile  \"%s\" key","@{mergefile}");
			GLOB->CMDH->DoCommand("draw");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
   		set_Cursor("ready");
			return;
		}
		catch(...) {
			_messagehandler->info("Unknown error while loading file.","CmFileImportKEY(), Load error");
		   set_Cursor("ready");
			return;
		}
	}
   set_Cursor("ready");
}

void GDSMainWindow::CmFileImportFLASH()
{
	wxString name;

   GLOB->CMDH->DoCommand("askfile  %s", "{Open Flash file} OPEN {FLASH file (*.??m)|*.??m|ALL (*.*)|*.*} @{file_path} @{file_ext} curfile");
   set_Cursor("wait");
	GLOB->CMDH->GetAlias("curfile",name);

	if ( !name.IsEmpty() )
   {
		try
      {
			wxString result="0";
			Ask("Into which layer would you like to insert the drawing?",result);
		   GLOB->CMDH->DoCommand("setlayer %d",atoi(result.c_str())); //in case of wrong input a zero will be returned!!

			GLOB->CMDH->DoCommand("mergefile \"%s\" flash","@{curfile}");
			GLOB->CMDH->DoCommand("draw");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());

			// if there is memory allocated for the file -> delete it
			if (GLOB->CMDH->Get_GDSII()) GLOB->CMDH->DoCommand("closefile");
   		set_Cursor("ready");
			return;
		}
		catch(...) {
			_messagehandler->info("Unknown error while loading file.","CmFileImportFLASH(), Load error");
		   set_Cursor("ready");
			return;
		}
	}
   set_Cursor("ready");
}

void GDSMainWindow::CmFileImportEMASK()
{
	wxString name;

   GLOB->CMDH->DoCommand("askfile  %s", "{Open Flash file} OPEN {EMASK file (*.emk)|*.emk|EMASK file (*.em)|*.em|ALL (*.*)|*.*} @{file_path} @{file_ext} curfile");
   set_Cursor("wait");
	GLOB->CMDH->GetAlias("curfile",name);

	if ( !name.IsEmpty() )
   {
		try
      {
			wxString result="0";
			Ask("Into which layer would you like to insert the drawing?",result);
		   GLOB->CMDH->DoCommand("setlayer %d",atoi(result.c_str())); //in case of wrong input a zero will be returned!!

			GLOB->CMDH->DoCommand("mergefile \"%s\" emask","@{curfile}");
        	GLOB->CMDH->AddFileToHistory(name,"emask");
			GLOB->CMDH->DoCommand("draw");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());

			// if there is memory allocated for the file -> delete it
			if (GLOB->CMDH->Get_GDSII()) GLOB->CMDH->DoCommand("closefile");
   		set_Cursor("ready");
			return;
		}
		catch(...) {
			_messagehandler->info("Unknown error while loading file.","CmFileOpenEMASK(), Load error");
		   set_Cursor("ready");
			return;
		}
	}
   set_Cursor("ready");
}

// response function to menuitem save GDS-II
void GDSMainWindow::CmFileSaveGDS()
{
	wxString name;
   GLOB->CMDH->DoCommand("askfile  %s",
   	" {Save GDS-II file} SAVE {GDSII file (*.cal)|*.cal| GDS file (*.db)|*.db|ALL (*.*)|*.*} @{file_path} @{file_ext} curfile");
   set_Cursor("wait");
	GLOB->CMDH->GetAlias("curfile",name);

	if ( !name.IsEmpty() )
   {
		try {
			GLOB->CMDH->DoCommand("savefile  \"%s\" gds-ii","@{curfile}");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
		   set_Cursor("ready");
		}
	}
   set_Cursor("ready");
}

// response function to menuitem save FLASH
void GDSMainWindow::CmFileSaveFLASH()
{
	wxString name;

   GLOB->CMDH->DoCommand("askfile  %s", " {Save Flash file} SAVE {FLASH file (*.??m)|*.??m|ALL (*.*)|*.*} @{file_path} flash curfile");

   set_Cursor("wait");
	GLOB->CMDH->GetAlias("curfile",name);

	if ( !name.IsEmpty() )
   {
		try {
			GLOB->CMDH->DoCommand("savefile \"%s\" flash","@{curfile}");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
		   set_Cursor("ready");
		}
	}
   set_Cursor("ready");
}

// response function to menuitem save EMASK
void GDSMainWindow::CmFileSaveEMASK()
{
	wxString name;

   GLOB->CMDH->DoCommand("askfile  %s",
   	" {Save EMASK file} SAVE {EMASK file (*.emk)|*.emk|ALL (*.*)|*.*} @{file_path} emask curfile");

   set_Cursor("wait");
	GLOB->CMDH->GetAlias("curfile",name);

	if ( !name.IsEmpty() )
   {
		try {
			GLOB->CMDH->DoCommand("savefile \"%s\" emask","@{curfile}");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
		   set_Cursor("ready");
		}
	}
   set_Cursor("ready");
}

// response function to menuitem save KEY
void GDSMainWindow::CmFileSaveKEY()
{
	wxString name;

   GLOB->CMDH->DoCommand("askfile  %s"," {Save Key file} SAVE {KEY file (*.key)|*.key|ALL (*.*)|*.*} @{file_path} key curfile");
   set_Cursor("wait");

	GLOB->CMDH->GetAlias("curfile",name);

	if ( !name.IsEmpty() )
   {
		try {
			GLOB->CMDH->DoCommand("savefile \"%s\" key","@{curfile}");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
		   set_Cursor("ready");
		}
	}
   set_Cursor("ready");
}

// response function to menuitem save KEY
void GDSMainWindow::CmFileSaveSVG()
{
	wxString name;

   GLOB->CMDH->DoCommand("askfile  %s"," {Save SVG file} SAVE {SVG file (*.svg)|*.svg|ALL (*.*)|*.*} @{file_path} svg curfile");
   set_Cursor("wait");

	GLOB->CMDH->GetAlias("curfile",name);

	if ( !name.IsEmpty() )
   {
		try {
			GLOB->CMDH->DoCommand("savefile \"%s\" svg","@{curfile}");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
		   set_Cursor("ready");
		}
	}
   set_Cursor("ready");
}

// response function to menuitem save CNC
void GDSMainWindow::CmFileSaveCNC()
{
	wxString name;

   GLOB->CMDH->DoCommand("askfile  %s"," {Save CNC file} SAVE {CNC file (*.cnc)|*.cnc|ALL (*.*)|*.*} @{file_path} cnc curfile");
   set_Cursor("wait");

	GLOB->CMDH->GetAlias("curfile",name);

	if ( !name.IsEmpty() )
   {
		try {
			GLOB->CMDH->DoCommand("savefile \"%s\" cnc","@{curfile}");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
		   set_Cursor("ready");
		}
	}
   set_Cursor("ready");
}

// response function to menuitem save GBR
void GDSMainWindow::CmFileSaveGBR()
{
	wxString name;

   GLOB->CMDH->DoCommand("askfile  %s"," {Save GBR file} SAVE {GBR file (*.gbr)|*.gbr|ALL (*.*)|*.*} @{file_path} gbr curfile");
   set_Cursor("wait");

	GLOB->CMDH->GetAlias("curfile",name);

	if ( !name.IsEmpty() )
   {
		try {
			GLOB->CMDH->DoCommand("savefile \"%s\" gbr","@{curfile}");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
		   set_Cursor("ready");
		}
	}
   set_Cursor("ready");
}

// response function to menuitem save Exelon
void GDSMainWindow::CmFileSaveExelon()
{
	wxString name;

   GLOB->CMDH->DoCommand("askfile  %s"," {Save Exelon file} SAVE {EXL file (*.exl)|*.exl|ALL (*.*)|*.*} @{file_path} exelon curfile");
   set_Cursor("wait");

	GLOB->CMDH->GetAlias("curfile",name);

	if ( !name.IsEmpty() )
   {
		try {
			GLOB->CMDH->DoCommand("savefile \"%s\" exelon","@{curfile}");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
		   set_Cursor("ready");
		}
	}
   set_Cursor("ready");
}

void GDSMainWindow::CmFilePNG()
{
	wxString name;
   GLOB->CMDH->DoCommand("askfile  %s"," {Save as PNG file} SAVE {*.png} @{file_path} png pngfile");
   set_Cursor("wait");

	GLOB->CMDH->GetAlias("pngfile",name);

	if ( !name.IsEmpty() )
   {
		try {
			GLOB->CMDH->DoCommand("savefile \"%s\" png","@{pngfile}");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
		   set_Cursor("ready");
		}
	}
   set_Cursor("ready");
}

void GDSMainWindow::CmFileBMP()
{
	wxString name;
   GLOB->CMDH->DoCommand("askfile  %s"," {Save as BMP file} SAVE {*.bmp} @{file_path} pmp bmpfile");
   set_Cursor("wait");

	GLOB->CMDH->GetAlias("bmpfile",name);

	if ( !name.IsEmpty() )
   {
		try {
			GLOB->CMDH->DoCommand("savefile \"%s\" bmp","@{bmpfile}");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
		   set_Cursor("ready");
		}
	}
   set_Cursor("ready");
}

void GDSMainWindow::SavePNG(const char* filename)
{
    GLOB->CANVAS->GetCanvasBitmap()->SaveFile(filename,wxBITMAP_TYPE_PNG,(wxPalette*)NULL);
}

void GDSMainWindow::SaveBMP(const char* filename)
{
   GLOB->CANVAS->GetCanvasBitmap()->SaveFile(filename,wxBITMAP_TYPE_BMP);
}

// response function to menuitem save
// also used when it's button is pressed on the controlbar
void GDSMainWindow::CmFileSave()
{
	if (_messagehandler->question_yesno("Are you sure you want to save?", "Application")) {
		try {
   		set_Cursor("wait");
         //search for the type
			GLOB->CMDH->DoCommand("savefile  \"%s\" %s", GLOB->CMDH->Get_FileName().c_str(),GLOB->CMDH->GetFileType().c_str());
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
		   set_Cursor("ready");
		}
	   set_Cursor("ready");
	}
}

// response function to menuitem Close
void GDSMainWindow::CmFileClose ()
{
   set_Cursor("wait");
	GLOB->CMDH->DoCommand("closefile");
	// reset statusbar and clear the screen
	_statusbar->Reset();
   CmDistanceEnable(FALSE);
   set_Cursor("ready");
}

// response function to menuitem load setupfile
// also used when it's button is pressed on the controlbar
void GDSMainWindow::CmLoadSetup()
{
	wxString name;
   GLOB->CMDH->DoCommand("askfile  %s"," {Open Setup file} OPEN {*.set} @{setupdir} @{setup_ext} setupfile");
   set_Cursor("wait");

	GLOB->CMDH->GetAlias("setupfile",name);

	if ( !name.IsEmpty() )
   {
		try {
			GLOB->CMDH->DoCommand("loadsetup  %s", "@{setupfile}");
			GLOB->CMDH->DoCommand("draw");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
		   set_Cursor("ready");
		}
	}
   set_Cursor("ready");
}

// response function to menuitem save setupfile
void GDSMainWindow::CmSaveSetup()
{
	wxString name;
   GLOB->CMDH->DoCommand("askfile  %s", "{Save Setup file} SAVE {*.set} @{setupdir} @{setup_ext} setupfile");
   set_Cursor("wait");

	GLOB->CMDH->GetAlias("setupfile",name);

	if ( !name.IsEmpty() )
   {
		try {
			GLOB->CMDH->DoCommand("savesetup  %s","@{setupfile}");
		}
		catch (GDS_Error& _error) {
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
		   set_Cursor("ready");
		}
	}
   set_Cursor("ready");
}

// response function to menuitem load processfile
// also used when it's button is pressed on the controlbar
void GDSMainWindow::CmLoadProces()
{
	wxString name;
   GLOB->CMDH->DoCommand("askfile  %s", "{Open Process file} OPEN {*.prs} @{procdir} @{proc_ext} processfile");
   set_Cursor("wait");

	GLOB->CMDH->GetAlias("processfile",name);

	if ( !name.IsEmpty() )
   {
		try {
         if (GLOB->CMDH->GetStepMode())
         {
            _debugerDialog->Init((char*)name.c_str());
         }
         else
         {
			   GLOB->CMDH->SetContinue(true);
            _stopDialog->Show(true);
			   ::wxYield(); //need to give stopdialog a change to built itself
				GLOB->CMDH->DoCommand("loadprocess  %s","@{processfile}");
            _stopDialog->Show(false);
			   GLOB->CMDH->SetContinue(true);
         }
        	GLOB->CMDH->AddFileToHistory(name.c_str(),"prs");
		}
		catch (GDS_Error& _error) {
	      CmFileClose();
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
		   set_Cursor("ready");
		}
	}
   set_Cursor("ready");
}

// response function to menuitem to switch stepmode
void GDSMainWindow::CmStepMode(bool onoff)
{
	if (!onoff)
   {
		GetMenuBar()->Check(AUTOMATIC_STEPMODE,false);
		GLOB->CMDH->DoCommand("stepmode false");
	}
   else
   {
		GetMenuBar()->Check(AUTOMATIC_STEPMODE,true);
		GLOB->CMDH->DoCommand("stepmode true");
   }
}



// response function to menuitem Start or Stop recording
void GDSMainWindow::CmRecord()
{
	char buf[30];

	wxString namewx;
   wxMenuBar* menu=GetMenuBar();
	namewx=menu->GetLabel(AUTOMATIC_SAVE);
	strcpy(buf,namewx);

	// is there a structure on the screen or is it the topstructure from the gdsii
	if (strstr(buf, "Start process recording"))
   {
		wxString name;
	   GLOB->CMDH->DoCommand("askfile  %s"," {Record Process file} SAVE {*.prs} @{procdir} @{proc_ext} processfile");
	   set_Cursor("wait");

		GLOB->CMDH->GetAlias("processfile",name);

		if(name)
      {
			GetMenuBar()->SetLabel(AUTOMATIC_SAVE, "&Stop process recording");
			GLOB->CMDH->DoCommand("record  %s","@{processfile}");
		}
	   set_Cursor("ready");
	}
   else
   {
	   set_Cursor("wait");
		GLOB->CMDH->DoCommand("record");
		GetMenuBar()->SetLabel(AUTOMATIC_SAVE, "&Start process recording");
	   set_Cursor("ready");
	}
}

void GDSMainWindow::CmFilePrint()
{
	_printing = 1;
#if wxUSE_PRINTING_ARCHITECTURE
	if (!_printer->Print(this,_printout,TRUE)) _messagehandler->info("unable to print","print error");
#else
	_messagehandler->info("printing support was not compiled in","print error");
#endif //wxUSE_PRINTING_ARCHITECTURE
	_printing = 0;
}

void GDSMainWindow::CmFilePrintSetup()
{
	_printing = 1;
#if wxUSE_PRINTING_ARCHITECTURE
	_printer->Setup(this);
#endif //wxUSE_PRINTING_ARCHITECTURE
	_printing = 0;
}

void GDSMainWindow::CmFileQuit()
{
	GLOB->INITIALIZED=false;
	if (GLOB->CMDH->Get_GDSII())
   	CmFileClose(); // close the active file (needed when program is not closed using the menu)
	this->Close(TRUE);
}

void GDSMainWindow::ShowFactors(wxCommandEvent&)
{
   try{
		_factorsDialog->SetupWindow();
		_factorsDialog->ShowModal();
   }
   catch (GDS_Error& _error) {
      _messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
   }
   catch(...) {
      _messagehandler->info("SOMETHING WENT WRONG leaf program","NON HANDLED ERROR");
      return;
   }
}

void GDSMainWindow::CmShowGridSettings()
{
	_gridDialog->SetupWindow();
}

void GDSMainWindow::CmShowUnitsSettings()
{
	_unitsDialog->SetupWindow();
   _unitsDialog->ShowModal();
}

void GDSMainWindow::CmShowNewfileDialog()
{
	_newfileDialog->SetupWindow();
   _newfileDialog->ShowModal();
}

void GDSMainWindow::ShowTrans(bool visible)
{
	_transDialog->SetupWindow(visible);
}

void GDSMainWindow::ShowCursorDlg(wxCommandEvent&)
{
	_cursorDialog->SetupWindow();
}

void GDSMainWindow::ShowSettings(wxCommandEvent&)
{
	_settingsDialog->SetupWindow();
   _settingsDialog->ShowModal();
}

void GDSMainWindow::ShowGroups(wxCommandEvent&)
{

	_groupDialog->SetupWindow(GLOB->SETUP,
									  GLOB->CMDH->Get_A(),
									  GLOB->CMDH->Get_B(),
									  GLOB->CMDH->Get_Destination(),
                             GLOB->CMDH->Get_Destination_Add()
                            );
   _groupDialog->ShowModal();
}

void GDSMainWindow::CmSelectedOnly()
{
	if (GLOB->CMDH->GetSelectedOnly())
   {
		GetMenuBar()->Check(SELECTED_ONLY,false);
		GLOB->CMDH->DoCommand("selected  0");
   }
   else
   {
		GetMenuBar()->Check(SELECTED_ONLY,true);
		GLOB->CMDH->DoCommand("selected  1");
   }
}

void GDSMainWindow::CmMakeStructure(bool named)
{
	if (!named)
   {
		GLOB->CMDH->DoCommand("makestructure my_S");
   }
   else
   {
      wxString result="my_";
      Ask("give the name for the new structure:",result);
		GLOB->CMDH->DoCommand("makestructure %s",result.c_str());
   }
}


void GDSMainWindow::CmVisibleOnly()
{
	if (GLOB->CMDH->GetVisibleOnly())
   {
		GetMenuBar()->Check(VISIBLE_ONLY,false);
		GLOB->CMDH->DoCommand("savevisible  0");
	}
   else
   {
		GetMenuBar()->Check(VISIBLE_ONLY,true);
		GLOB->CMDH->DoCommand("savevisible  1");
   }
}

void GDSMainWindow::CmSaveSetupOnExit()
{
	if (GLOB->CMDH->GetSaveSetUpOnExit())
   {
		GetMenuBar()->Check(SAVE_SETUP_ON_EXIT,false);
		GLOB->CMDH->DoCommand("savesetuponexit  0");
	}
   else
   {
		GetMenuBar()->Check(SAVE_SETUP_ON_EXIT,true);
		GLOB->CMDH->DoCommand("savesetuponexit  1");
   }
}

void GDSMainWindow::CmShowGrid()
{
	if (GLOB->CMDH->GetGridVisible())
   {
		GetMenuBar()->Check(SHOW_GRID,false);
		GLOB->CMDH->DoCommand("showgrid  0");
		GLOB->CMDH->DoCommand("draw");
	}
   else
   {
		GetMenuBar()->Check(SHOW_GRID,true);
		GLOB->CMDH->DoCommand("showgrid	  1");
		GLOB->CMDH->DoCommand("draw");
   }
}


void GDSMainWindow::Operation(char* operation)
{
   set_Cursor("wait");
	try {
		GLOB->CMDH->DoCommand(operation);
	}
	catch (GDS_Error &_error) {
		_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
	   set_Cursor("ready");
		return;
	}

   DrawStyle backup=GLOB->SCRDRV->GetDrawStyle();
   if (strstr(operation,"select all true"))
		GLOB->SCRDRV->SetDrawStyle(SELECTED);

	Redraw();

   if (strstr(operation,"select all true"))
		GLOB->SCRDRV->SetDrawStyle(backup);

   set_Cursor("ready");
}


// Begin Settings Functions -------------------------------


// response function to menuitem view coordinates
void GDSMainWindow::CmViewCoordinates()
{
	// pass command to the command handler
	if (GLOB->CMDH->GetViewCoord() != true)	GLOB->CMDH->DoCommand("showcoords  1");
	else GLOB->CMDH->DoCommand("showcoords  0");
}

void GDSMainWindow::ShowExecDlg()
{

	if (_execDlgVisible)
   {
		GetMenuBar()->Check(EXEC_DIALOG_SHOW,false);
   	_execDlgVisible = false;
      _execDialog->Show(FALSE);
   } else {
		GetMenuBar()->Check(EXEC_DIALOG_SHOW,true);
   	_execDlgVisible = true;
		_execDialog->SetFocus();
      _execDialog->Show(TRUE);
   }
}

void GDSMainWindow::ShowInsertLayerDlg()
{
	if (_insertDlgVisible)
   {
		GetMenuBar()->Check(TOOL_INSERTLAYER,false);
   	_insertDlgVisible = false;
      _insertDialog->Show(FALSE);
   } else {
		GetMenuBar()->Check(TOOL_INSERTLAYER,true);
   	_insertDlgVisible = true;
		_insertDialog->SetupWindow();
      _insertDialog->Show(TRUE);
   }
}

void GDSMainWindow::ShowDistDlg()
{
	if (_distDlgVisible)
   {
		GetMenuBar()->Check(DIST_DIALOG_SHOW,false);
   	_distDlgVisible = false;
      _distDialog->Show(FALSE);
   }
   else
   {
		GetMenuBar()->Check(DIST_DIALOG_SHOW,true);
   	_distDlgVisible = true;
      _distDialog->Show(TRUE);
   }
}

void GDSMainWindow::ShowIdentDlg()
{
	if (_identDlgVisible)
   {
		GetMenuBar()->Check(IDENT_DIALOG_SHOW,false);
   	_identDlgVisible = false;
      _identDialog->Show(FALSE);
   } else {
		GetMenuBar()->Check(IDENT_DIALOG_SHOW,true);
   	_identDlgVisible = true;
      _identDialog->Show(TRUE);
   }
}


void GDSMainWindow::SetPoint(double x, double y)
{
	_point_x = x;
   _point_y = y;
}

void GDSMainWindow::SetBackGroundColor(long _back_colour)
{
   GLOB->CANVAS->SetBackGroundColor(_back_colour);
	GLOB->CMDH->DoCommand("draw");
}

void GDSMainWindow::CmViewStructure(wxCommandEvent&)
{
	_structDialog->SetupWindow(GLOB->CMDH->Get_GDSII());
}
// response function to menuitem view outline
void GDSMainWindow::CmViewWireFrame()
{
   if (GLOB->INITIALIZED)
   	GetMenuBar()->Check(VIEW_DRAW_ONLY_OUTLINES,!GLOB->CMDH->GetViewOutline());

	// pass command to the command handler
	if (GLOB->CMDH->GetViewOutline() != true)
	   GLOB->CMDH->DoCommand("drawoutline  1");
	else
   	GLOB->CMDH->DoCommand("drawoutline  0");

	GLOB->CMDH->DoCommand("draw");
}

void GDSMainWindow::ShowLayers(wxCommandEvent&)
{
	_layerpropertiesDialog->SetupWindow(GLOB->SETUP);
}

void GDSMainWindow::SetRelStart(wxCommandEvent&)
{
	GLOB->CMDH->DoCommand("startpoint  %f %f",_point_x,_point_y);
}

void GDSMainWindow::CalcDistance(wxCommandEvent&)
{
	if (!_distDlgVisible)
   {
   	GLOB->CANVAS->SetCursor(*wxCROSS_CURSOR);
      _distDlgVisible = true;
      _distDialog->Show(TRUE);
   }
   //set_Cursor("wait");
	GLOB->CMDH->DoCommand("relativepoint  %f %f",_point_x,_point_y);
   //set_Cursor("ready");
}


// response function to menuitem Drawing order
void GDSMainWindow::ShowLayerOrder(wxCommandEvent&)
{
//	_layerorderDialog->MakeModal(true);
	_layerorderDialog->SetupWindow();
	_layerorderDialog->MakeModal(false);
	_layerorderDialog->Centre(wxBOTH);
	_layerorderDialog->Show(true);
}

void GDSMainWindow::CmCancel(wxCommandEvent&)
{
   set_Cursor("wait");
	GLOB->CMDH->DoCommand("endcommand");
   set_Cursor("ready");
}

void GDSMainWindow::CmUndo(wxCommandEvent&)
{
   set_Cursor("wait");
	GLOB->CMDH->DoCommand("undo");
   set_Cursor("ready");
}

// response function to menuitem zoom out
// also used when it's button is pressed on the controlbar
void GDSMainWindow::CmZoomOut(wxCommandEvent&)
{
	if (!GLOB->CMDH->Get_GDSII()) return;
   set_Cursor("wait");
	GLOB->CMDH->DoCommand("zoomout");
	_statusbar->SetZoom(GLOB->CMDH->Get_ScaleFactor());
	CmZoomOutEnable(TRUE);
   set_Cursor("ready");
}

void GDSMainWindow::CmZoomOut2(wxCommandEvent&)
{
	if (!GLOB->CMDH->Get_GDSII()) return;
   set_Cursor("wait");
	GLOB->CMDH->DoCommand("zoomout2");
	_statusbar->SetZoom(GLOB->CMDH->Get_ScaleFactor());
	CmZoomOutEnable(TRUE);
   set_Cursor("ready");
}

void GDSMainWindow::CmZoomBack(wxCommandEvent&)
{
	if (!GLOB->CMDH->Get_GDSII()) return;
   set_Cursor("wait");
	GLOB->CMDH->DoCommand("zoomback");
	_statusbar->SetZoom(GLOB->CMDH->Get_ScaleFactor());
   set_Cursor("ready");
}

// response function to menuitem about
void GDSMainWindow::CmHelpAbout ()
{
	_aboutDialog->ShowModal();
}

// Begin of "normal" Functions -------------------------------------------------

// Add a filename to the filehistory menu
bool GDSMainWindow::UpdateHistoryMenu()
{
	if (! file_menu) return false;
	DL_Iter<Alias*> I(GLOB->CMDH->_file_history);

	int numadded = 0;

	I.toroot();

	if (_nbLoadedFiles==0) file_menu->AppendSeparator();
	while (I.iterate())
   {
		if (numadded < _nbLoadedFiles)
      	file_menu->SetLabel(BFILE_HISTORY+numadded,I.item()->GetAlias());
		else
      	file_menu->Append(BFILE_HISTORY+numadded,I.item()->GetAlias());
		numadded++;
	}

	 _nbLoadedFiles = I.count();

   return true;
}



// view coordinates switch
void GDSMainWindow::ViewCoordinates(bool buf)
{
	GLOB->CMDH->SetViewCoord(buf);
	if (GLOB->INITIALIZED) {
		GetMenuBar()->Check(VIEW_COORDINATES,buf);
		if (! buf) _statusbar->ResetCoord();
	}
}

void GDSMainWindow::ErrorMessage(char* message, char* header, int, int)
{
	_messagehandler->info(message, header);
}

void GDSMainWindow::Pause(const wxString& text)
{
	if (!text)	{
		_messagehandler->info("Paused. Press OK to continue.", "Command Processor");
	} else {
      wxString buf;
		buf.Printf("Paused. Press OK to continue\n\n%s", text.c_str());
		_messagehandler->info((char*)buf.c_str(), "Command Processor");
	}
}

void GDSMainWindow::ShowWaitDialog(const char* message)
{
	_waitDialog->SetMessage((char*)message);
	_waitDialog->Show(TRUE);
}

void GDSMainWindow::GetClientWindowSize(int& x1, int& y1, int& x2, int& y2)
{
	int w,h;
	GetClientSize(&w,&h);
	x1 = 0;
	y1 = 0;
	x2 = w;
	y2 = h;
}


// here are the functions defined that are used for menuitem enabling or disabling
// menu item are grayed or black
void GDSMainWindow::GDS_loaded(bool loaded)
{
	_mousemenu->Enable(CM_VIEWSTRUCTURE,loaded);
	_mousemenu->Enable(CM_GDSLAYERS,loaded);
   CmDistanceEnable(loaded);


   if (GLOB->CMDH->GetViewer())
   {
      GetMenuBar()->Enable(BFILE_SAVE,FALSE);
      GetMenuBar()->Enable(BFILE_SAVE_AS,FALSE);						// menus do not seem to disable correctly
      GetMenuBar()->Enable(BFILE_SAVE_AS_GDSII,FALSE);
      GetMenuBar()->Enable(BFILE_SAVE_AS_KEY,FALSE);
      GetMenuBar()->Enable(BFILE_SAVE_AS_FLASH,FALSE);
   }
   else
   {
      GetMenuBar()->Enable(BFILE_SAVE,loaded);
      GetMenuBar()->Enable(BFILE_SAVE_AS,loaded);						// menus do not seem to disable correctly
      GetMenuBar()->Enable(BFILE_SAVE_AS_GDSII,loaded);
      GetMenuBar()->Enable(BFILE_SAVE_AS_KEY,loaded);
      GetMenuBar()->Enable(BFILE_SAVE_AS_FLASH,loaded);
   }
	GetMenuBar()->Enable(BFILE_IMPORT_GDSII,loaded);
	GetMenuBar()->Enable(BFILE_IMPORT_KEY,loaded);
	GetMenuBar()->Enable(BFILE_IMPORT_FLASH,loaded);
	GetMenuBar()->Enable(BFILE_CLOSE,loaded);
	GetMenuBar()->Enable(BFILE_PRINT,loaded);
	GetMenuBar()->Enable(DEFINEGROUPS,loaded);
	GetMenuBar()->Enable(GROUPS_DEFINE,loaded);
	GetMenuBar()->Enable(GROUPASELECTED,loaded);
	GetMenuBar()->Enable(GROUPBSELECTED,loaded);
	GetMenuBar()->Enable(GROUPDESTINATION,loaded);
	GetMenuBar()->Enable(OPERATION_SELECT,loaded);
	GetMenuBar()->Enable(OPERATION_UNSELECT,loaded);
	GetMenuBar()->Enable(OPERATION_DELETE,loaded);
	GetMenuBar()->Enable(OPERATION_FLATTEN,loaded);
	GetMenuBar()->Enable(OPERATION_TRANSFORM,loaded);
	GetMenuBar()->Enable(OPERATIONA,loaded);
	GetMenuBar()->Enable(OPERATIONA_DELETE,loaded);
	GetMenuBar()->Enable(OPERATIONA_TRANSFORM,loaded);
   GetMenuBar()->Enable(OPERATIONA2D,loaded);
	GetMenuBar()->Enable(OPERATIONA2D_CREATE_RING,loaded);
	GetMenuBar()->Enable(OPERATIONA2D_CORRECTION,loaded);
	GetMenuBar()->Enable(OPERATIONA2D_COPY,loaded);
	GetMenuBar()->Enable(OPERATIONA2D_MOVE,loaded);
	GetMenuBar()->Enable(OPERATIONA2D_MERGE,loaded);
	GetMenuBar()->Enable(OPERATIONA2D_TO_ARCS,loaded);
	GetMenuBar()->Enable(OPERATIONA2D_TO_LINES,loaded);
	GetMenuBar()->Enable(OPERATIONA2D_SMOOTHEN,loaded);
	GetMenuBar()->Enable(OPERATIONAB2D_INTERSECT,loaded);
	GetMenuBar()->Enable(OPERATIONAB2D_MERGE,loaded);
	GetMenuBar()->Enable(OPERATIONAB2D_AMINB,loaded);
	GetMenuBar()->Enable(OPERATIONAB2D_BMINA,loaded);
	GetMenuBar()->Enable(OPERATIONAB2D_EXOR,loaded);
	GetMenuBar()->Enable(VIEW_LAYER_PROPERTIES,loaded);
	GetMenuBar()->Enable(VIEW_LAYER_ORDER,loaded);
	GetMenuBar()->Enable(VIEW_STRUCTURE,loaded);
	GetMenuBar()->Enable(SHOW_GRID,loaded);
	GetMenuBar()->Enable(GRID_SET,loaded);
	_mousemenu->Enable(CM_GDSZOOMBACK,loaded);
	_mousemenu->Enable(CM_GDSZOOMOUT,loaded);
	_mousemenu->Enable(CM_GDSZOOMOUT2,loaded);

/*
	GetToolBar()->EnableTool(2,loaded);
	GetToolBar()->EnableTool(3,loaded);
	GetToolBar()->EnableTool(5,loaded);
	GetToolBar()->EnableTool(6,loaded);
	GetToolBar()->EnableTool(7,loaded);
	GetToolBar()->EnableTool(8,loaded);
*/
   if (loaded)
   {
	   SetPoint(0,0);
      wxCommandEvent a = wxCommandEvent();
   	SetRelStart(a);
   }
}


void GDSMainWindow::CmZoomOutEnable(bool flag)
{
	_mousemenu->Enable(CM_GDSZOOMBACK,flag);
	_mousemenu->Enable(CM_GDSZOOMOUT,flag);
	_mousemenu->Enable(CM_GDSZOOMOUT2,flag);
}

void GDSMainWindow::CmDistanceEnable(bool flag)
{
	_mousemenu->Enable(CM_RELATIVE_START,flag);
   _mousemenu->Enable(CM_CALC_DISTANCE,flag);
}

// check if the file "file" exists
bool GDSMainWindow::IsFile(char* file)
{
	FILE* fp;
	if ((fp = fopen(file, "rt")) != NULL)
	{
		fclose(fp);
		return true;
	}
	return false;
}


bool GDSMainWindow::AskFilename(const wxString& title,const wxString& type,const wxString& filter,const wxString& dir, wxString& filename, const wxString& ext)
{
	if (strcmp(type,"SAVE")==0)
   {
      wxFileDialog dialog(this, title, dir, filename, filter ,wxSAVE|wxOVERWRITE_PROMPT);
      if (dialog.ShowModal() == wxID_OK)
      {
         filename=dialog.GetPath();
         filename.Replace("\\","/");
         return true;
      }
      else
      {
         filename.Empty();
         return false;
      }
   }
   else
   {
      wxFileDialog dialog(this, title, dir, filename, filter ,wxOPEN);
      if (dialog.ShowModal() == wxID_OK)
      {
         filename=dialog.GetPath();
         filename.Replace("\\","/");
         return true;
      }
      else
      {
         filename.Empty();
         return false;
      }
   }
}


bool GDSMainWindow::Ask(const wxString& message, wxString& aliasvalue)
{
	wxString text = wxGetTextFromUser(message,"input from user");
	if (text)
   {
		aliasvalue=text;
      return true;
	}
   else
   {
   	aliasvalue.Empty();
      return false;
	}
}

void GDSMainWindow::ShowAlias(const wxString& alias)
{
	wxString messbuf;
	wxString buf;
	if (alias)
	{
		if (GLOB->CMDH->GetAlias(alias,buf))
			messbuf.Printf(" Alias: %c%s = %s", '%', alias.c_str(), buf.c_str());
		else
			messbuf.Printf(" Alias: %c%s does not exist", '%', alias.c_str());
		_messagehandler->info((char*)messbuf.c_str(), "Show alias");
	}
	else
	{
		/*
		int no = GLOB->CMDH->GetNrAlias();
		char	aliasname[LINELENGTH];
		char	aliasstring[LINELENGTH];
		cout << "Printing all aliasses:" << endl;
		for( int i=1; i<=no; i++)
		{
			GLOB->CMDH->GetAliasNr( i, aliasname, aliasstring);
			cout << " %";
			cout.width(15);
			cout.setf(ios::left);
			cout << aliasname;
			cout.width(0);
			cout << " = " << aliasstring << endl;
		}
		*/
	}
}



void GDSMainWindow::ShowEnv(const wxString& env)
{
	wxString messbuf;
	wxString buf;
	if (GLOB->CMDH->GetEnv(env,buf))
		messbuf.Printf(" Environment: %s = %s", env.c_str(), buf.c_str());
	else
		messbuf.Printf(" Environment: %s does not exist", env.c_str());
	_messagehandler->info((char*)messbuf.c_str(), "Show environment variable");
}

void GDSMainWindow::ShowString(const wxString& thestring)
{
		_messagehandler->info((char*)thestring.c_str(),"output");
}

void GDSMainWindow::ShowHelp(const wxString& help_on)
{
	wxString helpkeyword=help_on;
	helpkeyword.MakeLower();

	if (!helpkeyword || helpkeyword.Cmp("commands") )
	{
		wxString helpbuf = " Command summary:\n"
			"Main:             Operations:       Factors settings: CommandLine Commands:\n"
			" askfile           flatten           order             showgroupsdlg   \n"
			" ask               delete            snapfactor        showfactorsdlg  \n"
			" loadfile          makering          correctionfactor  showalias    \n"
			" savefile          correction        roundfactor       showenv      \n"
			" dumpfile          copy              correctionaber    showlayersdlg   \n"
			" closefile         move              roundtype         showfilename \n"
			" loadprocess       intersect         toarcrmin                      \n"
			" record            merge             toarcrmax        Use/Set Alias:\n"
			" exit/quit         a_subtract_b      arcaccur          @            \n"
			"                   b_subtract_a      polyaccur         @{}          \n"
			"Layer Settings:    tolines           smoothaccur                    \n"
			" setlayer          toarcs            maxlinemerge     Use/Set Env:  \n"
			" name              smoothen          displayaber       $            \n"
			" bordercolor       exor             Groups:            ${}          \n"
			" fillcolor                           group_a                        \n"
			" brush                               group_b          help (this screen)\n"
			" visible           pause             destination      help <command>\n"
			" addfile           showcoords       drawoutline";
		_messagehandler->text((char*)helpbuf.c_str(), "Help");
	}
	else if (!helpkeyword.Cmp("help"))
		_messagehandler->info(" Help is used to give you information about"
			" the available commands\n Syntax: help <command>", "Help on");
	else
	{
		_messagehandler->info(" Sorry, no extended help available for this command\n"
			" you have to write it yourself :)", "Help on");
	}
}

void GDSMainWindow::ShowKeyBNF()
{
	char *helpbuf = " Key-file Bachus-Naur Form:\n"
   "<stream format>     ::=  <header> <bgnlib> <library> <endlib>\n"


   "<header>               ::=  HEADER\n"



   "<bgnlib>                ::=  BGNLIB LASTMOD LASTACC\n"



   "<library>               ::=  <libname> [<reflibs>] [<fonts>] [<attrtable>] [<generations>] [<FormatType>] <units> {<Structure>}* ENDLIB\n"



   "<libname>              ::=  LIBNAME\n"



   "<reflibs>                ::=  NOT_IMPLEMENTED\n"



   "<fonts>                  ::=  NOT_IMPLEMENTED\n"



   "<attrtable>             ::=  NOT_IMPLEMENTED\n"



   "<generations>         ::=  NOT_IMPLEMENTED\n"



   "<FormatType>       ::=  NOT_IMPLEMENTED\n"



   "<units>                   ::=  UNITS USERUNITS PHYSUNITS\n"



   "<Structure>            ::=  <bgnstr> STRNAME [<strclass>] {<element>}* ENDSTR\n"



   "<bgnstr>                ::=  BGNSTR CREATION LASTMOD\n"



   "<strclass>              ::=  NOT_IMPLEMENTED\n"



   "<element>             ::=  {<boundary> | <path> | <SREF> | <AREF> | <text> |  <node> | <box> | <arc> | <circle> } {<property>}* ENDEL\n"



   "<boundary>          ::=  BOUNDARY [ELFLAGS] [PLEX] LAYER [DATATYPE] [WIDTH]\n"

   "                                  XY {X Y [Xm Ym X0 Y0]}+ X Y\n"



   "<path>                  ::=  PATH [ELFLAGS] [<plex>] LAYER [DATATYPE] [PATHTYPE]\n"

   "                                  [WIDTH] [BGNEXTN] [ENDEXTN] XY {X Y [Xm Ym X0 Y0]}+ X Y\n"



   "<SREF>                ::=  SREF [ELFLAGS] [PLEX] SNAME [<strans>] XY X Y\n"



   "<AREF>               ::=  AREF [ELFLAGS] [PLEX] SNAME [<strans>] COLROW XY X Y\n"



   "<text>                   ::=  TEXT [ELFLAGS] [PLEX] LAYER <textbody> XY X Y\n"



   "<node>                 ::=  NODE [ELFLAGS] [PLEX] LAYER [NODETYPE] [WIDTH] XY X Y\n"



   "<box>                  ::=  BOX [ELFLAGS] [PLEX] LAYER [BOXTYPE] [WIDTH] XY {X Y}+\n"



   "<arc>                   ::=  ARC [ELFLAGS] [PLEX] LAYER [DATATYPE] [WIDTH] XY X Y Xm Ym X0 Y0 X Y\n"



   "<circle>                ::=  CIRCLE [ELFLAGS] [PLEX] LAYER [DATATYPE] [WIDTH] XY X Y RADIUS\n"



   "<textbody>           ::=  TEXTTYPE [PRESENTATION] [PATHTYPE] [WIDTH] [<strans>] XY X Y STRING\n"



   "<strans>               ::=  STRANS [MAG] [ANGLE]\n"



   "<property>          ::=  NOT_IMPLEMENTED\n"



   "<plex>                 ::=  NOT_IMPLEMENTED\n";
	_messagehandler->text(helpbuf, "Show Key-BNF");
}

void GDSMainWindow::ShowProcessBNF()
{
	char *helpbuf = " Process-file Bachus-Naur Form:\n"
   "<commands>\t::= [<blanks>] { { <command> }* }\n"


   "<command>\t::= [<blanks>] {<comment> | {<word> {<blanks> | <endcommand}}* }*\n"



   "<endcommand>\t::= {';' | '\\n' | '\\0' }\n"



   "<word>\t\t::= { <quotedstring> | <bracedstring> | <multipartword> }\n"



   "<quotedstring>\t::= ('\"') {CHAR | '\"' | '\\n' | aliasref | envref }* ('\"')\n"



   "<bracedstring>\t::= ('{') {CHAR | '\\{' | '\\}' | '\\n'}* ('}')\n"



   "<multipartword>\t::= { <aliasref> | <envref> | {CHAR | '\\n'}* }*\n"



   "<aliasref>\t\t::= ('@') { <name> <blank> | ('{') <name> ('}') }\n"



   "<envref>\t\t::= ('$') { <name> <blank> | ('{') <name> ('}') }\n"



   "<name>\t\t::= { LETTER | DIGIT | UNDERSCORE | MINUS }*\n"



   "<blanks>\t\t::= { TAB | SPACE | '\\n'}*\n"



   "<comment>\t::= # {CHAR}* ('\\0')\n";



   _messagehandler->text(helpbuf, "Show Process-/Command-BNF");
}

void GDSMainWindow::MakeLower(char* name)
{
	if( name==0 )
		return;
	unsigned int l=strlen(name);
	if( l==0 )
		return;
	for( unsigned int i=0; name[i]!=0 && i<l; i++ )
		name[i]=tolower(name[i]);
}

void GDSMainWindow::ProcessArgs(char* ftype, char* filetoload, char* processfile, bool viewer )
{
   wxString name;

   if (strlen(filetoload)>0)
   {
	   set_Cursor("wait");
      char* c=filetoload;
      while (*c != '\0')
      {
         if (*c=='\\') *c='/';
         c++;
      }

      GLOB->CMDH->DoCommand("setalias curfile \"%s\"",filetoload);
      GLOB->CMDH->GetAlias("curfile",name);

      try {
         GLOB->CMDH->DoCommand("loadfile \"%s\" %s","@{curfile}",ftype);
        	GLOB->CMDH->AddFileToHistory(name.c_str(),ftype);
			GLOB->CMDH->DoCommand("draw");
      }
      catch (GDS_Error& _error) {
		   set_Cursor("ready");
         _messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());

         // if there is memory allocated for the file -> delete it
         if (GLOB->CMDH->Get_GDSII()) GLOB->CMDH->DoCommand("closefile");
         return;
      }
      catch(...) {
		   set_Cursor("ready");
         _messagehandler->info("Unknown error while loading file.","CmFileOpen(), Load error");
         return;
      }
	   set_Cursor("ready");
   }

   if (viewer)
      GLOB->CMDH->DoCommand("viewer true");
   else
      GLOB->CMDH->DoCommand("viewer false");

	if( strlen(processfile)>0 )
   {
	   set_Cursor("wait");
      char* c=processfile;
      while (*c != '\0')
      {
         if (*c=='\\') *c='/';
         c++;
      }
		GLOB->CMDH->DoCommand("setalias processfile %s",processfile);
		try {
         if (GLOB->CMDH->GetStepMode())
         {
            _debugerDialog->Init(processfile);
         }
         else
         {
			   GLOB->CMDH->SetContinue(true);
				GLOB->CMDH->DoCommand("loadprocess  %s","@{processfile}");
         }
        	GLOB->CMDH->AddFileToHistory(processfile,"prs");
			GLOB->CMDH->DoCommand("draw");
		   set_Cursor("ready");
		}
		catch (GDS_Error& _error) {
		   set_Cursor("ready");
			_messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
		}
	}
}


bool GDSMainWindow::GetCheckViewOutline()
{
	return GLOB->CMDH->GetViewOutline();
}

bool GDSMainWindow::GetCheckViewCoordinates()
{
	return GLOB->CMDH->GetViewCoord();
}

bool GDSMainWindow::GetCheckGridVisible()
{
	return GLOB->CMDH->GetGridVisible();
}


void GDSMainWindow::CmHelp()
{
	GLOB->CMDH->DoCommand("help");
}

void GDSMainWindow::CmContextHelp()
{
	set_Cursor("help");
}

void GDSMainWindow::CmRegisterApplication()
{
#ifndef __UNIX__
	HKEY mainKey = NULL;
	HKEY subKey_1 = NULL;
	HKEY subKey_2 = NULL;
	HKEY subKey_3 = NULL;
	HKEY subKey_4 = NULL;
   //UCHAR buf[LINELENGTH];
   char buf[LINELENGTH];

	if (RegCreateKeyEx(HKEY_CLASSES_ROOT,(LPCTSTR)".cal",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&mainKey,NULL)== ERROR_SUCCESS)
   {
      strcpy(buf,"GDSfile");
      RegSetValueEx(mainKey,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);

      if (RegCreateKeyEx(HKEY_CLASSES_ROOT,(LPCTSTR)buf,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&mainKey,NULL)== ERROR_SUCCESS)
      {
         strcpy(buf,"GDS-II file");
         RegSetValueEx(mainKey,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);

         if (RegCreateKeyEx(mainKey,(LPCTSTR)"Shell",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_1,NULL)== ERROR_SUCCESS)
         {
            strcpy(buf,"");
            RegSetValueEx(subKey_1,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);

            if (RegCreateKeyEx(subKey_1,(LPCTSTR)"open",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_2,NULL)== ERROR_SUCCESS)
            {
               if (RegCreateKeyEx(subKey_2,(LPCTSTR)"command",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_3,NULL)== ERROR_SUCCESS)
               {
                  wxString buf2;
                  GLOB->CMDH->GetAlias("gdsroot",buf2);
                  strcpy(buf,buf2.c_str());
                  strcat(buf,"edit.exe %1");
                  RegSetValueEx(subKey_3,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);
                  RegCloseKey(subKey_3);
               }
               if (RegCreateKeyEx(subKey_2,(LPCTSTR)"ddeexec",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_3,NULL)== ERROR_SUCCESS)
               {
                  strcpy(buf,"%1 gds-ii");
                  RegSetValueEx(subKey_3,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);
                  if (RegCreateKeyEx(subKey_3,(LPCTSTR)"Application",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_4,NULL)== ERROR_SUCCESS)
                  {
                     strcpy(buf,"Boolean");
                     RegSetValueEx(subKey_4,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);
                     RegCloseKey(subKey_4);
                  }
                  if (RegCreateKeyEx(subKey_3,(LPCTSTR)"topic",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_4,NULL)== ERROR_SUCCESS)
                  {
                     strcpy(buf,"loadfile");
                     RegSetValueEx(subKey_4,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);
                     RegCloseKey(subKey_4);
                  }
                  RegCloseKey(subKey_3);
               }
               RegCloseKey(subKey_2);
            }
            RegCloseKey(subKey_1);
         }
         if (RegCreateKeyEx(mainKey,(LPCTSTR)"DefaultIcon",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_1,NULL)== ERROR_SUCCESS)
         {
            wxString buf2;
            GLOB->CMDH->GetAlias("gdsroot",buf2);
            strcpy(buf,buf2.c_str());
            strcat(buf,"edit.exe,1");
            RegSetValueEx(subKey_1,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);
            RegCloseKey(subKey_1);
         }
         RegCloseKey(mainKey);
      }
   }


	if (RegCreateKeyEx(HKEY_CLASSES_ROOT,(LPCTSTR)".key",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&mainKey,NULL)== ERROR_SUCCESS)
   {
      strcpy(buf,"Keyfile");
      RegSetValueEx(mainKey,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);

      if (RegCreateKeyEx(HKEY_CLASSES_ROOT,(LPCTSTR)buf,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&mainKey,NULL)== ERROR_SUCCESS)
      {
         strcpy(buf,"Key file");
         RegSetValueEx(mainKey,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);

         if (RegCreateKeyEx(mainKey,(LPCTSTR)"Shell",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_1,NULL)== ERROR_SUCCESS)
         {
            strcpy(buf,"");
            RegSetValueEx(subKey_1,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);

            if (RegCreateKeyEx(subKey_1,(LPCTSTR)"open",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_2,NULL)== ERROR_SUCCESS)
            {
               if (RegCreateKeyEx(subKey_2,(LPCTSTR)"command",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_3,NULL)== ERROR_SUCCESS)
               {
                  wxString buf2;
                  GLOB->CMDH->GetAlias("gdsroot",buf2);
                  strcpy(buf,buf2.c_str());
                  strcat(buf,"win32/bin/boolean.exe %1");
                  RegSetValueEx(subKey_3,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);
                  RegCloseKey(subKey_3);
               }
               if (RegCreateKeyEx(subKey_2,(LPCTSTR)"ddeexec",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_3,NULL)== ERROR_SUCCESS)
               {
                  strcpy(buf,"%1 key");
                  RegSetValueEx(subKey_3,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);
                  if (RegCreateKeyEx(subKey_3,(LPCTSTR)"Application",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_4,NULL)== ERROR_SUCCESS)
                  {
                     strcpy(buf,"Boolean");
                     RegSetValueEx(subKey_4,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);
                     RegCloseKey(subKey_4);
                  }
                  if (RegCreateKeyEx(subKey_3,(LPCTSTR)"topic",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_4,NULL)== ERROR_SUCCESS)
                  {
                     strcpy(buf,"loadfile");
                     RegSetValueEx(subKey_4,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);
                     RegCloseKey(subKey_4);
                  }
                  RegCloseKey(subKey_3);
               }
               RegCloseKey(subKey_2);
            }
            RegCloseKey(subKey_1);
         }
         if (RegCreateKeyEx(mainKey,(LPCTSTR)"DefaultIcon",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_1,NULL)== ERROR_SUCCESS)
         {
            wxString buf2;
            GLOB->CMDH->GetAlias("gdsroot",buf2);
            strcpy(buf,buf2.c_str());
            strcat(buf,"win32/bin/boolean.exe,1");
            RegSetValueEx(subKey_1,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);
            RegCloseKey(subKey_1);
         }
         RegCloseKey(mainKey);
      }
   }

	if (RegCreateKeyEx(HKEY_CLASSES_ROOT,(LPCTSTR)".prs",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&mainKey,NULL)== ERROR_SUCCESS)
   {
      strcpy(buf,"Processfile");
      RegSetValueEx(mainKey,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);

      if (RegCreateKeyEx(HKEY_CLASSES_ROOT,(LPCTSTR)buf,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&mainKey,NULL)== ERROR_SUCCESS)
      {
         strcpy(buf,"Boolean process file");
         RegSetValueEx(mainKey,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);

         if (RegCreateKeyEx(mainKey,(LPCTSTR)"Shell",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_1,NULL)== ERROR_SUCCESS)
         {
            strcpy(buf,"");
            RegSetValueEx(subKey_1,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);

            if (RegCreateKeyEx(subKey_1,(LPCTSTR)"open",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_2,NULL)== ERROR_SUCCESS)
            {
               if (RegCreateKeyEx(subKey_2,(LPCTSTR)"command",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_3,NULL)== ERROR_SUCCESS)
               {
                  wxString buf2;
                  GLOB->CMDH->GetAlias("gdsroot",buf2);
                  strcpy(buf,buf2.c_str());
                  strcat(buf,"win32/bin/boolean.exe %1");
                  RegSetValueEx(subKey_3,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);
                  RegCloseKey(subKey_3);
               }
               if (RegCreateKeyEx(subKey_2,(LPCTSTR)"ddeexec",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_3,NULL)== ERROR_SUCCESS)
               {
                  strcpy(buf,"%1");
                  RegSetValueEx(subKey_3,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);
                  if (RegCreateKeyEx(subKey_3,(LPCTSTR)"Application",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_4,NULL)== ERROR_SUCCESS)
                  {
                     strcpy(buf,"Boolean");
                     RegSetValueEx(subKey_4,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);
                     RegCloseKey(subKey_4);
                  }
                  if (RegCreateKeyEx(subKey_3,(LPCTSTR)"topic",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_4,NULL)== ERROR_SUCCESS)
                  {
                     strcpy(buf,"loadprocess");
                     RegSetValueEx(subKey_4,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);
                     RegCloseKey(subKey_4);
                  }
                  RegCloseKey(subKey_3);
               }
               RegCloseKey(subKey_2);
            }
            RegCloseKey(subKey_1);
         }
         if (RegCreateKeyEx(mainKey,(LPCTSTR)"DefaultIcon",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&subKey_1,NULL)== ERROR_SUCCESS)
         {
            wxString buf2;
            GLOB->CMDH->GetAlias("gdsroot",buf2);
            strcpy(buf,buf2.c_str());
            strcat(buf,"win32/bin/boolean.exe,2");
            RegSetValueEx(subKey_1,NULL,0,REG_SZ,(LPBYTE)buf,strlen(buf)+1);
            RegCloseKey(subKey_1);
         }
         RegCloseKey(mainKey);
      }
   }

  	_messagehandler->info("File information was added to the Registry. You can now open files by double-clicking them in Explorer", "Info");
#else
  	_messagehandler->info("This is a Microsoft Windows function. It's not available on Unix", "Info");
#endif
}

void GDSMainWindow::OnToolEnter(wxCommandEvent& event)
{
	switch(event.GetSelection()) {

		case	0: _statusbar->SetProcess("Open an existing GDS-II file"); break;
		case	1: _statusbar->SetProcess("Load a process file to initialize the program"); break;
		case	2: _statusbar->SetProcess("Save the modified GDS-II file"); break;
		case	3: _statusbar->SetProcess("Start or Stop the recording of commands"); break;
		case	4: _statusbar->SetProcess("Load a file containing recorded commands"); break;
		case	5: _statusbar->SetProcess("Edit the properties of the layers like color and names"); break;
		case	6: _statusbar->SetProcess("View a selected structure"); break;
		case	7: _statusbar->SetProcess("Dump the GDS-II file as a textfile"); break;
		case	8: _statusbar->SetProcess("Print the GDS-II structure"); break;
		case	9: _statusbar->SetProcess("About the GDS-II application"); break;
		case 10: _statusbar->SetProcess("exit GDS-II postprocessing application"); break;
      case	11: _statusbar->SetProcess("draw polygon");break;
      case	12: _statusbar->SetProcess("draw polyline");break;
      case	13: _statusbar->SetProcess("draw spline polyline");break;
      case	14: _statusbar->SetProcess("draw spline polygon");break;
      case	15: _statusbar->SetProcess("draw rectangle");break;
      case	16: _statusbar->SetProcess("draw circle");break;
      case	17: _statusbar->SetProcess("draw circle2");break;
      case	18: _statusbar->SetProcess("draw text");break;
      case	19: _statusbar->SetProcess("select");break;
      case	20: _statusbar->SetProcess("deselect");break;
      case	21: _statusbar->SetProcess("move");break;
      case	22: _statusbar->SetProcess("copy");break;
      case	23: _statusbar->SetProcess("delete");break;
      case	24: _statusbar->SetProcess("edit");break;
      case	25: _statusbar->SetProcess("inslayerdlg");break;
      case	26: _statusbar->SetProcess("settings");break;

      default: break;
	}
}

void GDSMainWindow::OnToolLeftClick(wxCommandEvent& event)
{

	switch(m_current_toolbar_id)
   {
      case TOOLBAR_DRAW   :
         switch(event.GetId())
         {
            case	11:
               GLOB->CMDH->DoCommand("setspline 0");
               GLOB->CMDH->DoCommand("tool polygon");break;
            case	12:
               GLOB->CMDH->DoCommand("setspline 0");
               GLOB->CMDH->DoCommand("tool polyline"); break;
            case	13:
               GLOB->CMDH->DoCommand("setspline 1");
               GLOB->CMDH->DoCommand("tool polygon");break;
            case	14:
               GLOB->CMDH->DoCommand("setspline 1");
               GLOB->CMDH->DoCommand("tool polyline"); break;
            case	15: GLOB->CMDH->DoCommand("tool rectangle"); break;
            case	16: GLOB->CMDH->DoCommand("tool circle"); break;
            case	17: GLOB->CMDH->DoCommand("tool circle2"); break;
            case	18: GLOB->CMDH->DoCommand("tool text"); break;
            case	19: GLOB->CMDH->DoCommand("tool select"); break;
            case	20: GLOB->CMDH->DoCommand("tool deselect"); break;
            case	21: GLOB->CMDH->DoCommand("tool move"); break;
            case	22: GLOB->CMDH->DoCommand("tool copy"); break;
            case	23: GLOB->CMDH->DoCommand("tool delete"); break;
         	case  24: GLOB->CMDH->DoCommand("tool edit"); break;
         	case  25: GLOB->CMDH->DoCommand("inslayerdlg"); break;
         	case  26: GLOB->CMDH->DoCommand("pathtypedlg"); break;
            default: break ;
         }
      	break;
		case TOOLBAR_GENERAL:
      default:
         wxCommandEvent a = wxCommandEvent(); 
         switch(event.GetId())
         {
            case	0: CmFileOpen();break;
            case	1: CmLoadSetup(); break;
            case	2: CmFileSave(); break;
            case	3: CmLoadProces(); break;
            case	4: CmRecord(); break;
            case	5: ShowLayers(a); break;
            case	6: CmViewStructure(a); break;
            case	7: CmFileSaveKEY(); break;
            case	8: CmFilePrint(); break;
            case	9: CmHelpAbout(); break;
            case 10: CmFileQuit(); break;
            default: break ;
         }
      	break;
   }
	return;
}

bool GDSMainWindow::SetToolbar(ToolBarSet a_toolbar_id)
{
   if (a_toolbar_id == TOOLBAR_NOCHANGE) return true;
   if (a_toolbar_id == m_current_toolbar_id) return true;

   SetToolBar(NULL);

   switch(a_toolbar_id)
   {
      case TOOLBAR_DRAW   :
      {
         m_toolbar2->Show(false);
         m_toolbar1->Show(true);
         m_toolbar2->Reparent(m_dummyFrame);
         m_toolbar1->Reparent(this);
         SetToolBar(m_toolbar1);
			m_current_toolbar_id=TOOLBAR_DRAW;
         break;
      }
      case TOOLBAR_GENERAL:
      {
         m_toolbar1->Show(false);
         m_toolbar2->Show(true);
         m_toolbar1->Reparent(m_dummyFrame);
         m_toolbar2->Reparent(this);
         SetToolBar(m_toolbar2);
			m_current_toolbar_id=TOOLBAR_GENERAL;
         break;
      }
      default:
      break;
   }

  return TRUE;
}

bool GDSMainWindow::InitToolbar()
{
	m_current_toolbar_id=TOOLBAR_DRAW;

   int i;
   wxBitmap*		buttonGfx[30];

	wxString	pathfile;
   wxString gdsroot = wxGetenv("GDSROOT");
	if ( gdsroot.IsEmpty() ) exit(0);

	gdsroot = gdsroot + "/default/gfx/";
   gdsroot = ConvertToUnixPath( gdsroot );

	for (i=0;i<=26;i++) 
   {
		pathfile.Printf( "%stool%i.bmp", gdsroot.c_str(), i );

		wxImage image;
   	image.LoadFile(pathfile,wxBITMAP_TYPE_BMP);

		buttonGfx[i] = new wxBitmap( image );
	}

         SetToolBar(NULL);

         m_toolbar1 = new wxToolBar(this,ID_TOOLBAR,wxDefaultPosition,wxDefaultSize,wxNO_BORDER|wxTB_HORIZONTAL|wxTB_FLAT|wxTB_DOCKABLE);
         m_toolbar1->SetMargins( 2, 2 );
         int w = buttonGfx[0]->GetWidth(),x = 5;
#ifndef __UNIX__
         m_toolbar1->SetToolBitmapSize(wxSize(w,w));
#endif
         m_toolbar1->AddTool(11, *buttonGfx[11],wxNullBitmap,false,x,3, (wxObject *) NULL, "draw polygon"); x += w + 5;
         m_toolbar1->AddTool(12, *buttonGfx[12],wxNullBitmap,false,x,3, (wxObject *) NULL, "draw polygon"); x += w + 5;
         m_toolbar1->AddTool(13, *buttonGfx[24],wxNullBitmap,false,x,3, (wxObject *) NULL, "draw polygon"); x += w + 5;
         m_toolbar1->AddTool(14, *buttonGfx[25],wxNullBitmap,false,x,3, (wxObject *) NULL, "draw polyline"); x += w + 5;
         m_toolbar1->AddTool(15, *buttonGfx[13],wxNullBitmap,false,x,3, (wxObject *) NULL, "draw rectangle"); x += w + 5;
         m_toolbar1->AddTool(16, *buttonGfx[14],wxNullBitmap,false,x,3, (wxObject *) NULL, "draw circle"); x += w + 5;
         m_toolbar1->AddTool(17, *buttonGfx[15],wxNullBitmap,false,x,3, (wxObject *) NULL, "draw circle2"); x += w + 5;
         m_toolbar1->AddTool(18, *buttonGfx[16],wxNullBitmap,false,x,3, (wxObject *) NULL, "draw text"); x += w + 5;
         m_toolbar1->AddTool(19, *buttonGfx[17],wxNullBitmap,false,x,3, (wxObject *) NULL, "select"); x += w + 5;
         m_toolbar1->AddTool(20, *buttonGfx[18],wxNullBitmap,false,x,3, (wxObject *) NULL, "deselect"); x += w + 5;
         m_toolbar1->AddTool(21, *buttonGfx[19],wxNullBitmap,false,x,3, (wxObject *) NULL, "move"); x += w + 5;
         m_toolbar1->AddTool(22, *buttonGfx[20],wxNullBitmap,false,x,3, (wxObject *) NULL, "copy"); x += w + 5;
         m_toolbar1->AddTool(23, *buttonGfx[21],wxNullBitmap,false,x,3, (wxObject *) NULL, "delete"); x += w + 5;
         m_toolbar1->AddTool(24, *buttonGfx[22],wxNullBitmap,false,x,3, (wxObject *) NULL, "edit"); x += w + 5;
         m_toolbar1->AddTool(25, *buttonGfx[23],wxNullBitmap,false,x,3, (wxObject *) NULL, "layer"); x += w + 5;
         m_toolbar1->AddTool(26, *buttonGfx[26],wxNullBitmap,false,x,3, (wxObject *) NULL, "settings"); x += w + 5;
         m_toolbar1->Realize();

         m_toolbar2 = new wxToolBar(this,ID_TOOLBAR,wxDefaultPosition,wxDefaultSize,wxNO_BORDER|wxTB_HORIZONTAL|wxTB_FLAT|wxTB_DOCKABLE);

         m_toolbar2->SetMargins( 2, 2 );
         w = buttonGfx[0]->GetWidth(),x = 5;
   #ifndef __UNIX__
         m_toolbar2->SetToolBitmapSize(wxSize(w,w));
   #endif

         m_toolbar2->AddTool(0, *buttonGfx[0],wxNullBitmap,false,x,3, (wxObject *) NULL, "Open an existing GDS-II file"); x += w + 5;
         m_toolbar2->AddTool(1, *buttonGfx[1],wxNullBitmap,false,x,3, (wxObject *) NULL, "Load a process file to initialize the program"); x += w + 5;
         m_toolbar2->AddTool(2, *buttonGfx[2],wxNullBitmap,false,x,3, (wxObject *) NULL, "Save the modified GDS-II file"); x += w + 5;
         m_toolbar2->AddSeparator(); x += 10;
         m_toolbar2->AddTool(3, *buttonGfx[6],wxNullBitmap,false,x,3, (wxObject *) NULL, "Start or Stop the recording of commands"); x += w + 5;
         m_toolbar2->AddTool(4, *buttonGfx[7],wxNullBitmap,false,x,3, (wxObject *) NULL, "Load a file containing recorded commands"); x += w + 5;
         m_toolbar2->AddSeparator(); x += 10;
         m_toolbar2->AddTool(5, *buttonGfx[8],wxNullBitmap,false,x,3, (wxObject *) NULL, "Edit the properties of the layers like color and names"); x += w + 5;
         m_toolbar2->AddTool(6, *buttonGfx[9],wxNullBitmap,false,x,3, (wxObject *) NULL, "View a selected structure"); x += w + 5;
         m_toolbar2->AddSeparator(); x += 10;
         m_toolbar2->AddTool(7, *buttonGfx[3],wxNullBitmap,false,x,3, (wxObject *) NULL, "Print the GDS-II structure"); x += w + 5;
         m_toolbar2->AddTool(8, *buttonGfx[4],wxNullBitmap,0,x,3,(wxObject *) NULL, "Print the GDS-II structure"); x += w + 5;
         m_toolbar2->AddSeparator(); x += 10;
         m_toolbar2->AddTool(9, *buttonGfx[10],wxNullBitmap,false,x,3, (wxObject *) NULL, "About the GDS-II application"); x += w + 5;
         m_toolbar2->AddTool(10,*buttonGfx[5],wxNullBitmap,false,x,3, (wxObject *) NULL, "exit GDS-II postprocessing application");
         m_toolbar2->Realize();

         m_toolbar1->Reparent(m_dummyFrame);
         m_toolbar2->Reparent(this);
         m_toolbar1->Show(false);
         m_toolbar2->Show(true);
        	SetToolBar(m_toolbar2);
			m_current_toolbar_id=TOOLBAR_GENERAL;

   // Can delete the bitmaps since they're reference counted
   for (i = 0; i <= 26; i++)
     	delete buttonGfx[i];

   return true;
}


