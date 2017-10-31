#ifndef _MAINWIN_H
#define _MAINWIN_H

#ifdef __GNUG__
#pragma interface
#endif

#define SLIDEMARGE            50
#define MAX_FILES					5

#include "wxwin.h"
#include "wx/print.h"
#include "alias.h"
#include "b_lnk_itr.h"

class		Win_CommandHandler; // the commandhandler
class		WindowsDriver;	  	  // the windows driver
class 	WinStatusBar;
class		About;
class		ExecDlg;
class		DebugDialog;
class		StopDialog;
class		InsLayer;
class		DistanceDlg;
class		IdentDlg;
class		Wait;
class		StructuresDialog;
class		GroupDialog;
class 	LayerOrder;
class 	LayerPropertiesDialog;
class		NewFileDialog;
class 	PatternDialog;
class		Transformations;
class		CursorDialog;
class    SettingsDialog;
class		TextinsDialog;
class		Gridsettings;
class		Unitssettings;
class		Factors;
class		wxPrinter;
class		wxPrintout;
class		ButtonBar;


extern wxMenu	*file_menu;
extern bool	initialized;
enum ToolBarSet{ TOOLBAR_GENERAL,TOOLBAR_DRAW,TOOLBAR_NOCHANGE};
class GDSMainWindow : public wxFrame
{
	public:
		GDSMainWindow(wxFrame *frame, char *title, int x, int y, int w, int h );
		~GDSMainWindow();
      void SetUpWindow();
		bool 						InitToolbar();
		bool 						SetToolbar(ToolBarSet a_toolbar_id);
		void 						OnToolEnter(wxCommandEvent& event);
		void 						OnToolLeftClick(wxCommandEvent& event);


		void 						CmMenu(wxCommandEvent& event);

      void						Exit();
		void 						OnCloseWindow(wxCloseEvent &);
		void 						OnSize(wxSizeEvent& event);
		void 						Redraw();
		void						set_Cursor(char* type);
		void 						GetClientWindowSize(int&, int&, int&, int&);
		void 						ErrorMessage(char*, char*, int, int);
		void 						Pause(const  wxString&);
		void 						ViewCoordinates(bool);
		bool					UpdateHistoryMenu();
		void 						MakeLower(char*);
		void 						AskForFilename(const  wxString&);
		bool               AskFilename(const wxString&,const  wxString&,const  wxString&,const  wxString&, wxString&,const  wxString&);
		bool               Ask(const  wxString&, wxString&);
		void 						ShowString(const  wxString& thestring);
      void						ShowTrans(bool);
      void                 ShowSettings(wxCommandEvent&);
      void						ShowCursorDlg(wxCommandEvent&);
      void 						ShowTextDlg(wxCommandEvent&);
		void 						ShowGroups(wxCommandEvent&);
		void 						ShowFactors(wxCommandEvent&);
		void 						ShowLayers(wxCommandEvent&);
		void                 ShowLayerOrder(wxCommandEvent&);
		void 						ShowAlias(const  wxString& alias);
		void						ShowEnv(const  wxString& env);
		void 						ShowHelp(const  wxString& help_on);
		void 						ShowKeyBNF();
		void						ShowProcessBNF();
		void 						ShowPathtypeDlg();
		void 						ShowWidthDlg();
      void						ShowNewFileDlg();
		wxMenuBar*				SetupMenuBar();
		wxMenu*					_mousemenu;						// the floating menu
      void                 ProcessArgs(char* type, char* filetoload, char* processfile, bool viewer );
      void		            SetRelStart(wxCommandEvent&);
      void                 CalcDistance(wxCommandEvent&);
      void						SetPoint(double,double);
		void 						SetBackGroundColor(long _back_colour);


		bool 					GetCheckViewOutline();
		bool               GetCheckViewCoordinates();
		bool               GetCheckGridVisible();

		// the dialog screens (should be public so that callback functions can access them)
		About*						_aboutDialog;
      DistanceDlg*				_distDialog;
      ExecDlg*						_execDialog;
      DebugDialog*				_debugerDialog;
      StopDialog*				   _stopDialog;
      InsLayer*					_insertDialog;
      IdentDlg*					_identDialog;
		Wait*							_waitDialog;
		StructuresDialog*			_structDialog;
		GroupDialog*				_groupDialog;
		LayerOrder*					_layerorderDialog;
		Factors*						_factorsDialog;
      Transformations*			_transDialog;
      CursorDialog*        	_cursorDialog;
      SettingsDialog*      	_settingsDialog;
      Gridsettings*				_gridDialog;
      Unitssettings*				_unitsDialog;
		LayerPropertiesDialog* 	_layerpropertiesDialog;
      NewFileDialog*       	_newfileDialog;
      TextinsDialog*				_textinsDialog;

		// command enable functions
		void						CmSelectedOnly();
      void                 CmMakeStructure(bool named);
		void						CmShowGrid();
      void						CmDestination();
		void 						CmTransform();
		void 						CmCalcArea();
		void 						CmTransformA();
		void                 GDS_loaded(bool);
		void                 CmZoomOutEnable(bool);
		void                 CmDistanceEnable(bool);
		void 						Operation(char*);

		void 						CmShowGridSettings();
		void 						CmShowUnitsSettings();

		void						ShowDistDlg();
		void						ShowExecDlg();
		void						ShowIdentDlg();
      void						ShowInsertLayerDlg();
      void						CmShowNewfileDialog();
      void 						CmProperty(bool atlayer);

		// event handler functions
		void 						CmUndo(wxCommandEvent&);
		void 						CmCancel(wxCommandEvent&);
		void 						CmZoomOut(wxCommandEvent&);
		void 						CmZoomOut2(wxCommandEvent&);
		void 						CmZoomBack(wxCommandEvent&);
		void 						CmViewStructure(wxCommandEvent&);
		void                 CmFileOpen();
		void                 CmFileOpenGDS();
		void                 CmFileOpenKEY();
		void                 CmFileOpenFLASH();
		void                 CmFileOpenEMASK();
		void 						CmFilePrint();
		void 						CmFileQuit();
		void                 CmOpenOldFile(wxCommandEvent& event);
		void                 CmFileClose();
		void                 CmLoadSetup();
		void                 CmSaveSetup();
		void                 CmFileSaveGDS();
		void                 CmFileSaveKEY();
      void                 CmFileSaveSVG();
		void                 CmFileSaveFLASH();
		void                 CmFileSaveEMASK();
		void                 CmFileSaveCNC();
		void                 CmFileSaveGBR();
		void                 CmFileSaveExelon();
		void                 CmFileSave();
		void                 CmFileSaveAs();
		void                 CmFilePNG();
		void                 CmFileBMP();
		void 						SavePNG(const char* filename);
		void 						SaveBMP(const char* filename);
		void                 CmFileImportGDS();
		void                 CmFileImportKEY();
		void                 CmFileImportFLASH();
		void                 CmFileImportEMASK();
		void                 CmFilePrintSetup();
		void                 CmRecord();
		void                 CmStepMode(bool onoff);
		void                 CmLoadProces();
		void                 CmViewCoordinates();
		void                 CmViewWireFrame();
      void						CmVisibleOnly();
      void						CmSaveSetupOnExit();
		void                 CmHelpAbout();
      void                 CmHelp();
		void                 CmContextHelp();
		void 						ShowWaitDialog(const char* message);
      void                 CmRegisterApplication();
		void 						OnIdle(wxIdleEvent& event);
      bool					_exit;                     //exit the application
		int						_printing;						// boolean when printing

private:

		bool					IsFile(char*);
      bool					_execDlgVisible;
      bool					_insertDlgVisible;
      bool					_distDlgVisible;
      bool					_identDlgVisible;
		wxCursor					_cursorWait;
		wxCursor					_cursorArrow;
		wxCursor					_cursorHelp;
		wxPen*					_zoom_pen;						// zoom rectangle style
		ButtonBar*				_buttonbar;
#if wxUSE_PRINTING_ARCHITECTURE
		wxPrinter*				_printer;
		wxPrintout*				_printout;
#endif //wxUSE_PRINTING_ARCHITECTURE
		wxDC*						_dragDC;							// Device context for zooming (was clientDC)
		wxRect  					_ZoomRect;						// zoom rectangle
		WinStatusBar* 			_winstatusbar;
		int						_nbLoadedFiles;				// for file-history
      double				   _point_x;                  // Set point of rightmouse button
      double			      _point_y;

		ToolBarSet 				m_current_toolbar_id;
      wxToolBar*           m_toolbar1;
      wxToolBar*           m_toolbar2;
      wxFrame*             m_dummyFrame;
      DECLARE_EVENT_TABLE()

};

#endif


