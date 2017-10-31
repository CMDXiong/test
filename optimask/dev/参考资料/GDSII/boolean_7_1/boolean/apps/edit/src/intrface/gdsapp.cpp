#ifdef __GNUG__
#pragma implementation
#endif

#include "mainwin.h"
#include "chandbas.h"
#include "toolcont.h"
#include "misc.h"
#include "winmess.h"
#include "wxwin.h"
#include "gdsapp.h"
#include <stdlib.h>
#include "wx/icon.h"
#include "eventids.h"
#include <wx/utils.h>


StatusBar* 	_statusbar;
GDSMessage* _messagehandler;
Global*		 GLOB;

wxMenu		*file_menu = NULL;			// needed for file history

char filename[LINELENGTH]="";
char processfile[LINELENGTH]="";

IMPLEMENT_APP(GDS_application)

GDS_application::GDS_application()
{
	SetUseBestVisual(true);
}


bool GDS_application::Initialized()
{
    if (GetTopWindow())
        return TRUE;
    else
        return FALSE;
}


// first function to be called
bool GDS_application::OnInit()
{
#if wxUSE_DEBUG_CONTEXT 
   wxDebugContext::SetCheckpoint();
#endif

   bool silent=false;
   bool viewer=false;

   char type[80]="gds-ii";
	// parse commandline
	if (argc>4) {
		wxMessageBox("Command line error", "Too many arguments on the commandline\n\nsyntax: Boolean [-h | /?] [filename] [processfile]", wxOK);
		return false;
	}

   wxString gdsroot = wxGetenv("GDSROOT");
   gdsroot = ConvertToUnixPath( gdsroot );
   if( gdsroot.IsEmpty() )
   {
#ifndef _GUNIX
		wxMessageBox("GDSROOT variable not set\nPlease set the GDSROOT environment string to the correct dir.\n\nExample : GDSROOT=c:/boolean/6.0 \n\n","environment error", wxOK);
#else
		wxMessageBox("GDSROOT variable not set\nPlease set the GDSROOT environment string to the correct dir.\n\nExample : GDSROOT=c:/boolean/6.0; export GDSROOT \n\n","environment error", wxOK);
//		printf("\nenvironment error : GDSROOT directory not set\nPlease set the GDSROOT environment string to the correct dir.\n\nExample : GDSROOT=/usr/boolean/6.0; export GDSROOT;\n\n");
#endif
		exit(0);
	}


   wxString homeuser = wxGetenv("HOME");
   homeuser = ConvertToUnixPath( homeuser );
   if( homeuser.IsEmpty() )
   {
#ifndef _GUNIX
		wxMessageBox("HOME variable not set\nPlease set the HOME environment string.\n\nExample : HOME=/home/useraccount\n\n","environment error", wxOK);
#else
		wxMessageBox("HOME variable not set\nPlease set the HOME environment string.\n\nExample : HOME=c:/user\n\n","environment error", wxOK);
#endif
   }

   //wxMessageBox("GDSROOT" + gdsroot,"environment", wxOK);
   //wxMessageBox("HOME" + homeuser,"environment error", wxOK);

	for (int i=0;i<argc;i++) {
		if (strstr(argv[i],".prs"))
      {
         strcpy(processfile,argv[i]);
      }
		else if ( strstr( argv[i], ".key") )
      {
      	strcpy( filename, argv[i]);
         strcpy(type,"key");
      }
		else if ( strstr(argv[i],".cal"))
      {
      	strcpy( filename, argv[i]);
         strcpy(type,"gds-ii");
      }
		else if ( strstr(argv[i],".gds"))
      {
      	strcpy( filename, argv[i]);
         strcpy(type,"gds-ii");
      }
		else if ( strstr(argv[i],".DWM"))
      {
      	strcpy( filename, argv[i]);
      }
		else if ( strstr(argv[i],".dwm")) strcpy(filename,argv[i]);
		else if ( strstr(argv[i],".emk")) strcpy(filename,argv[i]);
		else if ( strstr(argv[i],"-silent")) silent=true;
		else if ( strstr(argv[i],"-viewer")) viewer=true;
		else if ( strstr(argv[i],"-prs"))
        {
            i++;
            if (i < argc)
                strcpy(processfile,argv[i]);
            else
    			return false;
        }
		else if ( strstr(argv[i],"-gds-ii"))
        {
            strcpy(type,"gds-ii");
            i++;
            if (i < argc)
                strcpy(filename,argv[i]);
            else
    			return false;
        }
		else if ( strstr(argv[i],"-key"))
        {
            strcpy(type,"key");
            i++;
            if (i < argc)
                strcpy(filename,argv[i]);
            else
    			return false;
        }
		else if ( strstr(argv[i],"-flash"))
        {
            strcpy(type,"flash");
            i++;
            if (i < argc)
                strcpy(filename,argv[i]);
            else
    			return false;
        }
		else if ( strstr(argv[i],"-h") || strstr(argv[i],"-?") || strstr(argv[i],"/h") || strstr(argv[i],"/?") )	{
			wxMessageBox("Help", "syntax: Boolean [-h | /?] [filename] [processfile]",wxOK);
			return false;
		}
	}

	GLOB = new Global();
	_win_messagehandler = new WINMessage();
	_messagehandler = (GDSMessage *) _win_messagehandler;

	int hsize=806, vsize=614;
#ifdef wx_msw
		SetPrintMode(wxPRINT_WINDOWS);
#else
		SetPrintMode(wxPRINT_POSTSCRIPT);
#endif

	_GDSMainWindow = new GDSMainWindow((wxFrame *) NULL,PROGRAM_NAME,0,0,hsize,vsize);
   GLOB->MAINWINDOW=_GDSMainWindow;
	GLOB->MAINWINDOW->SetUpWindow();

	// initialize Commandhandler
	GLOB->CMDH = new CommandHandler();
	GLOB->CMDH->SetupEnv();
	GLOB->CMDH->SetFileEnv(filename);
	GLOB->CMDH->SetProcEnv(processfile);
	_GDSMainWindow->SetMenuBar(SetupMenuBar());

   // Give it an icon
//   _GDSMainWindow->SetIcon(wxICON(mondrian));
	// Show the frame
	_GDSMainWindow->GDS_loaded(FALSE);
	_GDSMainWindow->CmZoomOutEnable(FALSE);

   //wxSizeEvent(wxSize(-1,-1),_GDSMainWindow->GetId());
   //_GDSMainWindow->OnSize(event);

 	if (!silent)
	 	_GDSMainWindow->Show(TRUE);

   SetTopWindow(_GDSMainWindow);
//	while (Pending())
//		Dispatch();

	GLOB->INITIALIZED = true;
  	_GDSMainWindow->ProcessArgs(type,filename,processfile,viewer);

	if (_GDSMainWindow->_exit)
    {
		_GDSMainWindow->Close(true);
    	return TRUE;   
    }

	// Return the main frame window
	return TRUE;
}

int GDS_application::OnRun()
{
	if (_GDSMainWindow->_exit)
		return 0;
	return wxAppBase::OnRun();
}

int GDS_application::OnExit(void)
{
   if (GLOB->CMDH)
		delete GLOB->CMDH;

   delete GLOB;

#if wxUSE_DEBUG_CONTEXT 
   wxDebugContext::Dump();
   wxDebugContext::PrintStatistics();
#endif

	delete _win_messagehandler;
//	exit(0);
	return 0;
}


// ====== GDS_application ======================================================

GDSMainWindow*	GDS_application::GetMainWindow()
{
	return _GDSMainWindow;
}

wxMenuBar* GDS_application::SetupMenuBar()
{
// NIEUWE MENU BAR MAKEN.
	wxMenuBar *menu_bar = new wxMenuBar;
// ALLE SUBMENU'S MAKEN.
	// -- FILE --
	file_menu = new wxMenu; // NIEUW SUBMENU
	file_menu->Append(BFILE_NEW, "&New","start new drawing");
		wxMenu *file_open = new wxMenu; // NIEUW POPUPMENU
		file_open->Append(BFILE_OPEN_GDSII, "GDS-II","load a GDS-II/calma file");
		file_open->Append(BFILE_OPEN_KEY, "KEY","load a KEY file");
		file_open->Append(BFILE_OPEN_FLASH, "FLASH","load a DAVID Mann Flash file");
		file_open->Append(BFILE_OPEN_EMASK, "EMASK","load a EMASK Flash file");
	file_menu->Append(BFILE_OPEN,"&Open...",file_open,"Open...");
	file_menu->Append(BFILE_SAVE,"&Save","save current file in memory to disk");
  		wxMenu *file_save_as = new wxMenu; // NIEUW POPUPMENU
		file_save_as->Append(BFILE_SAVE_AS_GDSII, "GDS-II","save as GDS-II/calma file");
		file_save_as->Append(BFILE_SAVE_AS_KEY, "KEY", "save as KEY file");
		file_save_as->Append(BFILE_SAVE_AS_SVG, "SVG","save as SVG file");
		file_save_as->Append(BFILE_SAVE_AS_FLASH,"FLASH", "SAVE as DAVID Mann Flash file");
		file_save_as->Append(BFILE_SAVE_AS_EMASK,"EMASK", "SAVE as EMASK Flash file");
		file_save_as->Append(BFILE_SAVE_AS_CNC, "CNC", "save as CNC file");
		file_save_as->Append(BFILE_SAVE_AS_GBR, "GBR", "save as GBR file");
		file_save_as->Append(BFILE_SAVE_AS_EXL, "Exelon", "save as exelon file");
		file_save_as->AppendSeparator();
		file_save_as->Append(BFILE_SAVE_AS_PNG, "PNG bitmap","save as PNG bitmap file");
		file_save_as->Append(BFILE_SAVE_AS_BMP, "BMP bitmap","save as BMP bitmap file");
	file_menu->Append(BFILE_SAVE_AS,"Save &As...",file_save_as,"Save As...");
		wxMenu *file_import = new wxMenu; // NIEUW POPUPMENU
		file_import->Append(BFILE_IMPORT_GDSII, "GDS-II","import a GDS-II/calma file");
		file_import->Append(BFILE_IMPORT_KEY, "KEY","import a KEY file");
		file_import->Append(BFILE_IMPORT_FLASH, "FLASH","import a DAVID Mann Flash file");
		file_import->Append(BFILE_IMPORT_EMASK, "EMASK","import a EMASK Flash file");
	file_menu->Append(BFILE_IMPORT,"&Import...",file_import,"Import...");
	file_menu->Append(BFILE_CLOSE,"&Close","clear file from memory");
		file_menu->AppendSeparator();
	file_menu->Append(BFILE_LOAD_SETUP,"&Load setup file...","load setup file for layers settings/factors etc.");
	file_menu->Append(BFILE_SAVE_SETUP,"&Save setup file...","save setup file for layers settings/factors etc.");
		file_menu->AppendSeparator();
	file_menu->Append(BFILE_PRINT,"&Print...","print what is visible on the screen");
	file_menu->Append(BFILE_PRINT_SETUP,"P&rint setup...","setup printer");
		file_menu->AppendSeparator();
	file_menu->Append(BFILE_EXIT, "E&xit","exit the program");

	_GDSMainWindow->UpdateHistoryMenu();
	// -- AUTOMATIC --
	wxMenu *automatic_menu = new wxMenu; // NIEUW SUBMENU
	automatic_menu->Append(AUTOMATIC_LOAD,"&Load process file","load and replay a recorded process file");
	automatic_menu->Append(AUTOMATIC_SAVE,"&Start process recording","recording of commands to file",TRUE);
	automatic_menu->Append(AUTOMATIC_STEPMODE,"&Step mode","step through process file while loading one",TRUE);
	automatic_menu->Append(EXEC_DIALOG_SHOW,"Show &Execution Dialog","show execution dialog for entering/viewing commands",TRUE);

	// -- GROUPS --
	wxMenu *groups_menu = new wxMenu; // NIEUW SUBMENU
		wxMenu *groupsdefine_menu = new wxMenu; // NEW POPUPMENU for group defenition
		groupsdefine_menu->Append(GROUPS_DEFINE, "&Group dialog","set groups for boolean algorithms");
		groupsdefine_menu->AppendSeparator();
		groupsdefine_menu->Append(GROUPASELECTED, "selected group &A","group A will be set to selected items");
		groupsdefine_menu->Append(GROUPBSELECTED, "selected group &B","group B will be set to selected items");
		groupsdefine_menu->Append(GROUPDESTINATION, "&Destination","set destination to given layer");
	groups_menu->Append(DEFINEGROUPS,"&Define Groups",groupsdefine_menu,"define groups for boolean algorithms");

   groups_menu->AppendSeparator();

	// -- OPERATIONS ON GROUPS ONLY --
		wxMenu *operationa_menu = new wxMenu; // NEW POPUPMENU
		operationa_menu->Append(OPERATIONA_DELETE, "&Delete","delete what is in group A");
		operationa_menu->Append(OPERATIONA_ADDPROP, "&AddProperty","addproperty to layers in group A");
     	operationa_menu->Append(OPERATIONA_TRANSFORM, "&Transform","transform what is in group A");
     	operationa_menu->Append(OPERATIONA_CALCAREA, "&Area calculation","area of layers in group A");
	groups_menu->Append(OPERATIONA,"&Operation(A)",operationa_menu,"Operation(A)");

		wxMenu *operationa2d_menu = new wxMenu; // NIEUW POPUPMENU
		operationa2d_menu->Append(OPERATIONA2D_CREATE_RING, "Create &Ring","create rings on items in groupA to destination" );
		operationa2d_menu->Append(OPERATIONA2D_CORRECTION, "&Correction","create correction on items in groupA to destination");
		operationa2d_menu->Append(OPERATIONA2D_COPY, "co&Py","copy items in groupA to destination");
		operationa2d_menu->Append(OPERATIONA2D_MOVE, "&Move","move items in groupA to destination");
		operationa2d_menu->Append(OPERATIONA2D_TO_ARCS, "to &Arcs","detect arcs in items in groupA");
		operationa2d_menu->Append(OPERATIONA2D_TO_LINES, "to &Lines","convert to polygons,items in groupA to destination");
		operationa2d_menu->Append(OPERATIONA2D_SMOOTHEN, "&Smoothen","remove small lines and redundant points");
		operationa2d_menu->Append(OPERATIONA2D_MERGE, "&Merge","Merge items in groupA to destination");
      // not (A & B)-> D but A -> D
	groups_menu->Append(OPERATIONA2D,"&Operation(A)->D",operationa2d_menu,"Operation(A)->D");
		wxMenu *operationab2d_menu = new wxMenu; // NIEUW POPUPMENU
		operationab2d_menu->Append(OPERATIONAB2D_INTERSECT, "&Intersect","Intersect items in groupA&B to destination");
		operationab2d_menu->Append(OPERATIONAB2D_MERGE, "&Merge","Merge items in groupA&B to destination");
		operationab2d_menu->Append(OPERATIONAB2D_AMINB, "&A-B","Subtract items in groupA - groupB to destination");
		operationab2d_menu->Append(OPERATIONAB2D_BMINA, "&B-A","Subtract items in groupB - groupA to destination");
		operationab2d_menu->Append(OPERATIONAB2D_EXOR, "e&Xor","Exor items in groupA&B to destination");
	groups_menu->Append(OPERATIONAB2D,"&Operation(A,B)->D",operationab2d_menu,"Operation(A,B)->D");

	// -- OPERATIONS --
	wxMenu *operations_menu = new wxMenu; // NIEUW SUBMENU
   operations_menu->Append(OPERATION_SELECT, "&Select all","select everything in drawing, if selectable");
   operations_menu->Append(OPERATION_UNSELECT, "&UnSelect all","un_select everything in drawing, if selectable");
   operations_menu->AppendSeparator();
	operations_menu->Append(SELECTED_ONLY, "Selected only","commands in this menu only for selected items",TRUE);
   operations_menu->AppendSeparator();
   operations_menu->Append(OPERATION_DELETE, "&Delete","delete (selected) items");
		wxMenu *structure_menu2 = new wxMenu; // NEW POPUPMENU
		structure_menu2->Append(OPERATION_STRUCTURE_NAMED, "&Named","name the structure");
		structure_menu2->Append(OPERATION_STRUCTURE_DEFAULT, "&Default name","use self generated name");
   operations_menu->Append(OPERATION_STRUCTURE, "&Structure",structure_menu2,"group into a structure (selected) items");
   operations_menu->Append(OPERATION_FLATTEN_ONE, "&Flatten one","flatten/remove one level hierarchy for (selected) items");
   operations_menu->Append(OPERATION_FLATTEN, "&Flatten deep","flatten/remove total hierarchy for (selected) items");
   operations_menu->Append(OPERATION_PROPERTY, "&AddProperty","add a propety to (selected) items");
   operations_menu->Append(OPERATION_TRANSFORM, "&Transform","tranforms (selected) items");

	// -- VIEW --
	wxMenu *view_menu = new wxMenu; // NIEUW SUBMENU
	view_menu->Append(VIEW_LAYER_PROPERTIES,"Layer &Properties...");
	view_menu->Append(VIEW_LAYER_ORDER,"Layer &Order...","change the order that the layers are drawn");
		view_menu->AppendSeparator();
	view_menu->Append(VIEW_STRUCTURE,"View &Structure","Set the top structure to start drawing");
	view_menu->Append(VIEW_DRAW_ONLY_OUTLINES,"&Draw only outlines","only draw outlines for all items",TRUE);
		view_menu->AppendSeparator();
	view_menu->Append(VIEW_COORDINATES,"View &Coordinates","view coordinates in status bar",TRUE);
#ifdef DEBUG_DRIVER
	view_menu->Append(VIEW_DEBUG_SCREEN,"View de&Bug screen","",TRUE);
#endif

	// -- draw --
	wxMenu *tools_menu = new wxMenu; // NIEUW SUBMENU
		tools_menu->Append(TOOL_ZOOMIN, "&Zoom in (z,+)","zoom in on drawing");
		tools_menu->Append(TOOL_ZOOMOUT, "&Zoom out (enter)","zoom out drawing");
		tools_menu->Append(TOOL_ZOOMBACK, "&Zoom back (-)","zoom back on drawing");
		tools_menu->Append(TOOL_SELECT, "&Select (s)","select part of drawing");
		tools_menu->Append(TOOL_DESELECT, "&DeSelect (d)","de_select part of drawing");
   	tools_menu->Append(TOOL_MOVE, "&Move (m)","move selection");
      tools_menu->Append(TOOL_COPY,	"&Copy (c)","copy selection");
      tools_menu->Append(TOOL_CURSOR, "&Set cursor","set cursorposition");
		tools_menu->Append(IDENT_DIALOG_SHOW,"Show &Identification Dialog","to show/identify selected items/primitives",TRUE);
		tools_menu->Append(DIST_DIALOG_SHOW,"Show &Distance Dialog","measure distances in the drawing",TRUE);
   	tools_menu->AppendSeparator();
      wxMenu *primdraw_settings = new wxMenu;  //submenu
         primdraw_settings->Append(TOOL_INSERTLAYER, "Set &Insert Layer","Draw on this layer",true);
         primdraw_settings->Append(TOOL_PATHTYPE, "Settings","path end type");
		tools_menu->Append(TOOL_SET,"Settings",primdraw_settings,"Settings for draw");
      wxMenu *primdraw_menu = new wxMenu; // NEW POPUPMENU for drawing primitives defenition
         primdraw_menu->Append(TOOL_POLYGON, "Polygon (p)","draw a polygon");
         primdraw_menu->Append(TOOL_POLYLINE, "Polyline (l)","draw a polyline");
         primdraw_menu->Append(TOOL_RECTANGLE, "Rectangle (r)","draw a rectangle");
         primdraw_menu->Append(TOOL_CIRCLE, "Circle (o)","draw a circle mode 1");
         primdraw_menu->Append(TOOL_CIRCLE2, "Circle (i)","draw a circle mode 2");
         primdraw_menu->Append(TOOL_TEXT, "Text (b)","insert a text");
		tools_menu->Append(TOOL_PRIM,"Draw Primitive",primdraw_menu,"Draw primitives");
      wxMenu *structure_menu = new wxMenu; //thing related to structures
         structure_menu->Append(TOOL_INSERT_REF, "Insert a reference","insert a reference to an existing structure");
         structure_menu->Append(TOOL_INSERT_STRUC, "Insert a new structure","insert a new (empty) structure");
		tools_menu->Append(TOOL_STRUC,"Structures",structure_menu,"references & structures");
		tools_menu->Append(TOOL_EDIT,"Edit Primitive (e)","Edit Selected primitives");
		tools_menu->Append(TOOL_DELETE,"Delete Primitive (k)","Delete Selected primitives");
		tools_menu->Append(TOOL_EDITPROPERTY,"Add Property (q)","Add properties to Selected primitive");
	// -- PREFERENCES --
	wxMenu *preferences_menu = new wxMenu; // NIEUW SUBMENU
	preferences_menu->Append(PREFERENCES_FACTORS,"&Factors","change factors");
	preferences_menu->Append(VISIBLE_ONLY,"&Save visible","save only layers that are visible",TRUE);
	preferences_menu->Append(SHOW_GRID,"Show &grid","show the grid",TRUE);
	preferences_menu->Append(GRID_SET,"G&rid settings","change grid settings");
	preferences_menu->Append(UNITS_SET,"&Units settings","change unit settings");
	preferences_menu->Append(SAVE_SETUP_ON_EXIT,"Save Set&Up","save setup of layers on exit?",TRUE);

   // -- HELP --
	wxMenu *help_menu = new wxMenu; // NIEUW SUBMENU
	help_menu->Append(HELP,"&Help","for showing help in the browser");
//	help_menu->Append(CONTEXT_HELP,"&Context sensitive help");
   help_menu->AppendSeparator();
	help_menu->Append(REGISTER,"&Register","add boolean to the windows register, to view files quickly");
   help_menu->AppendSeparator();
	help_menu->Append(HELP_ABOUT,"&About...","which idiots give away this for free" );
// EINDE ALLE SUBMENU'S MAKEN
// ALLE SUBMENU'S IN JUISTE VOLGORDE AAN MENUBAR PLAKKEN.
	menu_bar->Append(file_menu, "&File");
	menu_bar->Append(automatic_menu, "&Automatic");
	menu_bar->Append(groups_menu, "&Groups");
	menu_bar->Append(operations_menu, "&Operations");
	menu_bar->Append(tools_menu, "&Tools");
	menu_bar->Append(view_menu, "&View");
	menu_bar->Append(preferences_menu, "&Preferences");
	menu_bar->Append(help_menu, "&Help");

   view_menu->Check(VIEW_COORDINATES,_GDSMainWindow->GetCheckViewCoordinates());
	view_menu->Check(VIEW_DRAW_ONLY_OUTLINES,_GDSMainWindow->GetCheckViewOutline());
	preferences_menu->Check(SHOW_GRID,_GDSMainWindow->GetCheckGridVisible());
	preferences_menu->Check(VISIBLE_ONLY,GLOB->CMDH->GetVisibleOnly());
	preferences_menu->Check(SAVE_SETUP_ON_EXIT,GLOB->CMDH->GetSaveSetUpOnExit());
	operations_menu->Check(SELECTED_ONLY,GLOB->CMDH->GetSelectedOnly());
// MENU BAR TERUG GEVEN.
	return menu_bar;
}




void GDS_application::InitMainWindow ()
{

}



