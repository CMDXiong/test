//! author="klaas"
//! version="1.0"
/*
Program                 CHANDLER.H
Purpose                 Commandhandler between userinterface and commands (header)
					This file is the buffer between every command generator
					and the internal structures. It's platform independed.
					It's used by MainWindow
					This file also calls functions from the mainwindow. See
					the documentation for more details about these functions.
Last Update             20-12-1995
*/

#ifndef CHANDBAS_H
#define CHANDBAS_H

#ifdef __GNUG__
#pragma interface
#endif

#include "wxwin.h"

#include "misc.h"
#include "b_dl_itr.h"
#include "units.h"

//__UNIX__ uses
#include	<stdarg.h>
#include <fstream>

using namespace std;

#include "alias.h"

#define MAXHISTORY 10
enum OperationMethod{GROUP_LAYER,GROUP_SELECTED,SELECTED_ELEMENTS};

// forward declaration of the mainwindow
class AliasList;
class GdsII;
class Structure;
class ElementList;
class Command;
class group;
class Setup;
class wxColour;
class bbox;
class DDEHandler;
class Handler;


//pointer to function
typedef bool (CommandHandler::*lfp) ();

//! The most important class, since it processes all commands and redirects them to the right
/*!
subclasses for processing.
Commandhandler interprets (graphical/commandline) userinterface commands.
Commands are ascii strings, the first word is the keyword, followed by arguments.
Commands can be recorded to a file, for later automatic processing.
A history list of commands is maintained, allowing undo in certain cases.
This file also calls functions from the mainwindow, to show complete dialogs
See related classes "CommandParser", for the complete syntax of commands
\sa  CommandParser
\sa  ProcessParser
*/
class CommandHandler
{
	public:
		// constructors
      CommandHandler();
		//! destructor
		~CommandHandler();


      //!replace layer setup by a new setup
		bool Replace_Setup(Setup* new_setup);

		//!all commands are handled by this function
      //\return 1 if command was oke
      //\return 2 if command was if not oke
      //\return 0 if command does not exist
		int 	ExecuteCommand();

      //!used in case of formatted io command string (like printf)
		bool	DoCommand(char* Format,...);

      //!step through process files or not
		bool   GetStepMode() {return m_stepmode;};
      //!are we in debug step mode?
  		bool   StepMode();

      //!continue through process files or not?
		bool   GetContinue() {return m_continue;};
      //!continue through process files or not setting
  		bool   SetContinue(bool a){m_continue=a;return true;};
      //!continue through process files or not command
  		bool   SetContinueFlag();

      //!the factor of zoomin compared to the whole
		double							Get_ScaleFactor();
      //!is there zoom history
		int								Get_ZoomHistoryEmpty();
      //!show the whole picture
		bool							ZoomOut();
      //!show the whole picture times 2
		bool							ZoomOut2();
      //!zoom one level back
		bool							ZoomBack();
      //!redraw/draw the picture
		bool   						Draw();

      //!spline polygon/polylines
		bool                     GetSpline(){return m_spline;};

      //!spline polygon/polylines
      bool 							SetSpline();


      //!are commands recorded?
		bool                     Get_Record();
      //!groupA layers for boolean operations
		group*                     Get_A();
      //!groupB layers for boolean operations
		group*                     Get_B();
      //!get destination layer for boolean operations
		int                        Get_Destination();
      //!get destination layer for boolean operations
		bool                       Get_Destination_Add(){return m_destination_add;};
      //!pointer to internal database structure
		GdsII*                     Get_GDSII();
      //!make this datastructure the current one
		void	                     Set_GDSII(GdsII* a_gdsii);

      //only viewmode?
      bool                     GetViewer(){return m_viewer;}

      //!top structure of the current database contents
		Structure*                 Get_TopStructure();

      //!current filename
		wxString		               Get_FileName();
      //!set the current filename
		void	                     Set_FileName(const wxString& filename);

      //!set width to draw primitives with this command
      bool 							SetWidth();
      //!get the current width
      wxUnit& 							GetWidth();

      //!set pathtype used in path/polyline entry
      bool 							SetPathtype();

      //!get pathtype used in path/polyline entry
      int	 							GetPathtype();

      //!get setting for viewing coordinates in status bar or not
		bool							GetViewCoord();
      //!Set setting for viewing coordinates in status bar or not
      bool						  	SetViewCoord(bool value);

      //!get setting for viewing only outlines of primitives
		bool							GetViewOutline();
      //!Set setting for viewing only outlines of primitives
		bool							SetViewOutline(bool value);

      //!work on only selected elements?
      bool							GetSelectedOnly();

      //!save visible only?
      bool							GetVisibleOnly();

      //!save setup on exit?
      bool							GetSaveSetUpOnExit();

      //!grid visible?
      bool 							GetGridVisible();

      //!get given environment variable
		bool 							GetEnv(const wxString& envname,wxString& envstring);
      //!set given environment variable
		bool 							SetEnv();
      //!get given alias variable
		bool							GetAlias(const wxString& aliasname,wxString& aliasstring);
      //!set given alias variable
		bool 							SetAlias();

      //!get the total number of defined aliases
		int 								GetNrAlias();
      //!get the index number of the given alias
		bool 							GetAliasNr(int i,wxString& aliasname,wxString& aliasstring);

      //!transform settings for transform commands
      bool 	   						GetMirrorX();
      //!transform settings for transform commands
      bool								GetMirrorY();
      //!transform settings for transform commands
      wxUnit&							GetMoveHor();
      //!transform settings for transform commands
      wxUnit&							GetMoveVer();
      //!transform settings for transform commands
      double							GetScale();
      //!transform settings for transform commands
      double							GetRotate();
      //!transform settings for transform commands
      bool								GetDirection();

      //!get gridcolour in use
      long								GetGridColour();
      //!get background colour in use
      long								GetBackColour();
      //!get grid spacing
      wxUnit&							GetGridSpace();

      //!grid on forground or background
		int								GetGridPlacement();

      //!accuracy for selecting elements
		double							GetSelectmarge(){return _selectmarge;};
      //!snapfactor used in boolean algorithm
		wxUnit&							GetSnapfactor();
      //!correction factor used in boolean algorithm
		wxUnit&							GetCorrectionfactor();
      //!correction abberation used in boolean algorithm
		wxUnit&							GetCorrectionaber();
      //!rounding factor used in boolean algorithm
		double							GetRoundfactor();
      //!rounding type of corners, factor used in boolean algorithm
		wxString&						GetRoundtype();

      //!polygon to arc conversion factors
		wxUnit&							GetPoly2ArcRmin();
      //!polygon to arc conversion factors
		wxUnit&							GetPoly2ArcRmax();
      //!polygon to arc conversion factors
		wxUnit&							GetPoly2ArcAber();
      //!polygon to arc conversion factors
		wxUnit&							GetArc2PolyAber();

      //!factor used in smoothing polygons
		wxUnit&							GetSmoothAber();
      //!factor used while displaying arcs and circles
		wxUnit&							GetDisplayaber();
      //!smaller structure will not be displayed
		int								GetStrucThres();
      //!smaller elements will not be displayed
		int								GetPrimThres();
      //!small elements will be displayed?
		bool								GetDrawSmallPrim();
      //!factor used in smoothing polygons
		wxUnit&							GetMaxlinemerge();

      //!default accuracy used for entering elements
      double                     GetDefaultAcc(){return _default_acc;};
      //!default unit used for entering elements
		wxUnit                     GetDefaultUnit(){return _default_unit;};

      //!add slash to path
		void								AddSlash(wxString& _filename);
      //!strip slash from path
		void								StripSlash(wxString& _filename);

      //!determines the current file type
		bool							SetFileType(const wxString& _filetype);

      //!determine the current file type
		wxString						GetFileType();

      //!set path aliases
		bool							SetFileEnv(const wxString& _filetoload);
      //!set process file aliases
		bool							SetProcEnv(const wxString& _processfile);
      //!load setup file
		void								SetupEnv();

      //!set startpoint for measuring distances
		bool                     SetStartPoint();
      //!set relative point for measuring distances
		bool                     SetRelPoint();

      //!key command string for adding elements
      bool                     KeyString();

      //!select an area or point
		bool							Select();
      //!replace structure
		bool 							ReplaceStruct();

      //!get the element list of the structure that is displayed
      ElementList* 					GetTopElementList();
      //!command to put in the history list
		bool 							ToHistory(char* Format,...);
      //!command to put in the history list
		bool 							ToHistory(Command* a_command);
      //!get active layer
		int 								GetCurrentLayer();

      //!add files to the history
      bool AddFileToHistory(const wxString& file,const wxString& type);
      //!save the history files
      bool SaveHistory();

      //!the file history
      //!holds some of the files that were loaded
		DL_List<Alias*>*		_file_history;

   private:

      //!maximum number of functionpointers
      int 								maxpointer;

      //!set operation flags for operations
      void   SetOperationFlags(bool explode);

      //!select an area or point
		bool							Select(double,double,double,double,int);

   	//!Operation calls

      //!transform {selected elements}
      bool Operation_transform();
      //!undo transform {selected elements}
      bool Operation_undotrans();
      //!move layers in group A to destination
      bool Operation_move_a();
      //!copy layers in group A to destination
      bool Operation_copy_a();
      //!delete layers in group A
      bool Operation_delete_a();
      //!addproperties on layers in group A
		bool Operation_addproperty_a();
      //!transform layers in group A
      bool Operation_transform_a();
      //!undo transform layers in group A
      bool Operation_undotrans_a();
      //!calculate area of layers in group A
      bool Operation_calcarea_a();
      //!make a ring around elements on layers in group A to destination
   	bool Operation_makering();
      //!create correction/offset on elements on layers in group A to destination
      bool Operation_correction();
      //!recognize arcs in elements on layers in group A to destination
      bool Operation_toarcs();
      //!convert to lines all arcs in elements on layers in group A to destination
      bool Operation_tolines();
      //!smoothen (remove small segments) elements on layers in group A to destination
      bool Operation_smoothen();
      //!intersect elements on layers in group A and groupB to destination
      bool Operation_intersect();
      //!merge elements on layers in group A and groupB to destination
      bool Operation_merge();
      //!a_subtract_b elements on layers in group A and groupB to destination
      bool Operation_a_subtract_b();
      //!b_subtract_a elements on layers in group A and groupB to destination
      bool Operation_b_subtract_a();
      //!exor elements on layers in group A and groupB to destination
      bool Operation_exor();

      //use windingrule for filling or not
      bool SetWindingRule();

      //!start a interactive tool
		bool 							Tool();
      //!end  a interactive tool
		bool 							EndCommand();
      //!undo an action
		bool 							UndoCommand();
      //!delete (selected) elements
		bool 							Delete();
      //!delete (selected) elements
		bool 							MakeStructure();
      //!deselect all elements or area
		bool 							DeSelect();
      //!begin of action within a tool
		bool 							Begin();
      //!end of action within a tool
		bool 							End();
      //!set cursor position
      bool							SetCursor();
      //!add a point to a polygon
		bool							AddPoint();
      //!move within a tool
		bool							Move();

      //!create a newfile
		bool 							NewFile();
      //!load a file
		bool 							LoadFile();
      //!merge a file with an existing one
		bool 							MergeFile();

      //!scroll the visible part on the screen in given direction
		bool							ScrollScreen();

      //!is the zoom history available
		bool							IsZoomed();
      //!make zoomhistory empty
		void								EmptyZoomHistory();
      //!zoom commands
		bool							Zoom();
      //!print this file
		bool							PrintFile();
      //!zoom in on area or point
		double				Zoom(double,double,double,double,int);

      //!clear the screen
		bool							ClearScreen();

      //!get switch for show coordinates on and off
		bool							ShowCoords();

      //!add this file to the menu for reloading
		bool							AddMenuFile();
      //!close the current file in memory
		bool							CloseFile();
      //!remove hiearchy in file within the structure visible on the screen
		bool							Flatten();
      //!make this structure the visible one
		bool  							ViewStructure();
      //!set maaping for the given structure.
		int								View_Structure(Structure*);
      //!set draw only outlines on
		bool 							DrawOutline();
      //!set, certain operations only work on selected elements?
      bool							SetSelectedOnly();
      //!set, save only visible layers to files
      bool							SetVisibleOnly();

      //!set, save setuponexit
      bool							SetSaveSetUpOnExit();

      //!make the grid visible and snap to it
      bool							ShowGrid();
      //!reset transform settings
      bool                     ResetTrans();
      //!open the help browser
      bool                     ShowBrowser();
      //!exit the program
      bool                     Exit();

      //!set log file (output of messages/commands will go here)
      bool                     SetLogFile();

      //only use as viewer
      bool                     SetViewer();

      //!ask for a filename
		bool  	AskFile();
      //!ask for a string
		bool  	Ask();

      //!load a process file
		bool	Load_Process_File();
      //!load setup process file
		bool	LoadSetup();
      //!save setup process file
		bool	SaveSetup();
      //!save file in memory
		bool	SaveFile();

      //!read file in memory from file
      bool	GetFileType(const wxString& filename, GdsII* gdsii,const wxString& type);

      //!save file in memory as GDSII file
		bool	SaveFileGDS(const wxString& filename);
      //!save file in memory as PNG bitmap
		bool 	SaveFilePNG(const wxString& filename);
      //!save file in memory as PMP bitmap
		bool 	SaveFileBMP(const wxString& filename);
      //!save file in memory as KEY file
		bool	SaveFileKEY(const wxString& filename);
      //!save file in memory as SVG file
		bool	SaveFileSVG(const wxString& filename);
      //!save file in memory as DAVID MANN file
		bool   SaveFileFLASH(const wxString& filename);
      //!save file in memory as EMASK file
		bool   SaveFileEMASK(const wxString& filename);
      //!save file in memory as exelon file
		bool 	SaveFileExelon(const wxString& filename);
      //!save file in memory as gerber file
		bool 	SaveFileGerber(const wxString& filename);
      //!save file in memory as laser machine file
		bool 	SaveFileCnc(const wxString& filename);

      //!define group a layers
		bool	Define_A();
      //!define group b layers
		bool	Define_B();
      //!define destination layer
		bool	DefineDestination();

      //!add properties to elements
		bool 	AddProperty();

      //!record will be set
		bool 	Record();
      //!pause on dialogs will be shown
		bool   Pause();

      //!transform settings for transform commands
		bool	SetMirrorX();
      //!transform settings for transform commands
		bool	SetMirrorY();
      //!transform settings for transform commands
		bool	SetMoveHor();
      //!transform settings for transform commands
		bool	SetMoveVer();
      //!transform settings for transform commands
		bool	SetScale();
      //!transform settings for transform commands
		bool	SetRotate();
      //!transform settings for transform commands
		bool	SetDirection();

      //!set the grid spacing
      bool	SetGridSpace();
      //!set the grid color
      bool	SetGridColour();
      //!set the background color
      bool	SetBackColour();
      //!set grid on back or foreground
      bool	SetGridPlacement();
      //!set select marge as a fraction of the window width
		bool	SetSelectmarge();
      //!set snapfactor, used in boolean algorithm
		bool	SetSnapfactor();
      //!set correctionfactor, used in boolean algorithm
		bool	SetCorrectionfactor();
      //!set correction abberation, used in boolean algorithm
		bool	SetCorrectionaber();
      //!set roundfactor, used in correction operation
		bool	SetRoundfactor();
      //!set roundtype, used in correction operation
		bool	SetRoundtype();
      //!set radius minimal, used in poly to arc conversion
		bool	SetPoly2ArcRmin();
      //!set radius maximal, used in poly to arc conversion
		bool	SetPoly2ArcRmax();
      //!set abberation, used in poly to arc conversion
		bool	SetPoly2ArcAber();
      //!set abberation, used in arc to poly conversion
		bool	SetArc2PolyAber();
      //!set abberation, used in smooth operation
		bool	SetSmoothAber();
      //!set abberation, used in drawing routines
		bool	SetDisplayaber();
      //!set structure threshold, used in drawing routines
		bool 	SetStrucThres();
      //!set primitive threshold, used in drawing routines
		bool 	SetPrimThres();
      //!set maximum line to merge, used in smooth operation
		bool	SetMaxlinemerge();
      //!default accuracy
      bool   SetDefaultAcc();
      //!default unit
      bool   SetDefaultUnit();

      //! To few parameters?
		void		ParameterError(const wxString& );
      //!set the active layer
		bool 	SetCurrentLayer();
      //!set the active layer by name
		bool 	SetCurrentLayerByName();
      //!set layer name for active layer
		bool 	SetLayerName();
      //!set gdsmapping in  for active layer
		bool 	SetGdsInMap();
      //!set gdsmapping out for active layer
		bool 	SetGdsOutMap();
      //!set layer visibility for active layer
		bool 	SetLayerVisible();
      //!set layer selectable for active layer
		bool 	SetLayerSelectable();
      //!set border color for active layer
		bool 	SetLayerBorderColor();
      //!set fill color for active layer
		bool 	SetLayerFillColor();
      //!set fill pattern for active layer
		bool 	SetLayerBrush();
      //!set drawing order for active layer
		bool 	SetLayerOrder();
      //!set reading this layer from files for active layer
		bool 	SetLayerRead();

      //!show the dialog
		bool   ShowTextDlg();
      //!show the dialog
		bool 	ShowPathtypeDlg();
      //!show the dialog

      bool	ShowCursorDlg();
      //!show the dialog
		bool   ShowFactors();
      //!show the dialog
		bool   ShowOrder();
      //!show the dialog
		bool 	ShowGroups();
      //!show the dialog
		bool 	ShowAlias();
      //!show the dialog
		bool 	ShowEnv();
      //!show the dialog
		bool 	ShowLayers();
      //!show the dialog
		bool 	ShowFilename();
      //!show the dialog
		bool 	ShowKeyBNF();
      //!show the dialog
		bool 	ShowProcessBNF();
      //!show the dialog
		bool   ShowTopStructure();
      //!show the dialog
		bool 							ShowExecDlg();
      //!show the dialog
		bool 							ShowInsertLayerDlg();
      //!show the dialog
		bool 							ShowDistDlg();
      //!show the dialog
		bool 							ShowIdentDlg();
      //!show the dialog
		bool     						ShowHelp();

      //!current loaded internal database
		GdsII*                     _gdsii;
      //!current loaded important internal database
		GdsII*                     _gdsii_merge;
      //!layer setup
		Setup*                     _setup;

      //! boolean show coordinates
		bool							_view_coord;
      //! boolean show outline
		bool							_view_outline;
      //! boolean view debugscreen
		bool							_view_debug;
      //! boolean perform action only on selected items
      bool							_selected_only;
      //! boolean for output only visible layers
      bool							_visible_only;
      //! flag for savesetup on exit
      bool							_savesetuponexit;
      //! boolean for output only visible layers
      bool							_show_grid;

      //! transformation settings
		bool          _mirror_x;
      //! transformation settings
		bool          _mirror_y;
      //! transformation settings
		wxUnit          _move_hor;
      //! transformation settings
      wxUnit          _move_ver;
      //! transformation settings
      wxUnit          _scaling;
      //! transformation settings
      double          _rotate;
      //!rotation direction
      bool 				 _direction;

      //!grid space
      wxUnit							m_grid_space;
      //!value as string for eqauvalent number
      char								_str_grid_colour[30];
      //!value as string for eqauvalent number
      char								_str_back_colour[30];

		//! factor settings
		double          _selectmarge;
		//! factor settings
		wxUnit          _snapfactor;
		//! factor settings
		wxUnit          _correctionfactor;
		//! factor settings
		wxUnit          _correctionaber;
		//! factor settings
		double          _roundfactor;
		//! factor settings
		wxUnit          _poly2arcrmin;
		//! factor settings
		wxUnit          _poly2arcrmax;
		//! factor settings
		wxUnit          _poly2arcaber;
		//! factor settings
		wxUnit          _arc2polyaber;
		//! factor settings
		wxUnit          _smoothaber;
		//! factor settings
		wxUnit          _displayaber;
		//! factor settings
		wxUnit          _maxlinemerge;

		wxString        _roundtype;

      //!number of pixels a structure boundingbox needs to be in order to be drawn
		int         					_structhres;
      //!number of pixels a primitive boundingbox needs to be in order to be drawn
		int         					_primthres;
      //!do draw small primitives
		bool        					_drawsmallprim;

      //!new drawing width
      wxUnit                     _default_width;
      //!new drawing height
      wxUnit                     _default_height;
      //!units if non given
		wxUnit          				_default_unit;

      //!decimal placed when writing data
		double          				_default_acc;

      //!are we in debug step mode?
	   bool							m_stepmode;
      //!continues mode used by debugger of process files?
	   bool							m_continue;


      //!record to this file if record is on
		ofstream*                  _processing_file;

      //!active layer
		int                        _CurrentLayer;

      //!group a layers
		group*							_group_A;
      //!group b layers
		group*							_group_B;

      //!destination
		int                        _destination;

      //!destination add
		bool                       m_destination_add;

      //only viewer
      bool m_viewer;

      //!current filename
		wxString                   _filename;

      //!current filename
		wxString                   _file_type;

      //!back up for old filename
		wxString 						_name_backup;

      //!command keyword
		const wxString*   			_keyword;

      long								_grid_colour;
      long								_back_colour;
      int								_grid_placement;
      double                     _start_point_x;
      double                     _start_point_y;
		double                     _rel_point_x;
		double                     _rel_point_y;
		wxUnit                     _width;
		int	                     _pathtype;

      //!spline polygon / polyline entry
		bool          m_spline;

      //!the manner layers are handled using groups or not
      OperationMethod   			_operationmethod;

      //!command argument iterator
  		DL_Iter<const wxString*>* 	         _I;

      //!command history list
  		DL_List<Command*>* 	      _commandhistory;

 	protected:
		wxColour							ltowxc(long);
      DDEHandler*						_ddehandler;

      //!zoom history list
		DL_List<void*>* 				_zoom_history;
      //!zoom in factor
		double							_scale;

      //!array of function pointers to call funtions based on ascii commands
      Handler* _handler[200];
};

//!used to call functions in the command handler
class Handler
{
	public:
      //!the first word of a command
		wxString	keyword;
      //!function pointer for the command
	   lfp  		fpointer;


		Handler(const wxString& keyword,lfp);
   	~Handler();
};

#endif

