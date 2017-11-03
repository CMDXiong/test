//! author="klaas"
//! version="1.0"
/*
 * Definition of class for dialog to run/create a projectfile, in which commands can be executed seperately.
*/
#ifndef DEBUGDLG_H
#define DEBUGDLG_H

#ifdef __GNUG__
#pragma interface
#endif

//! codefiles="debugdlg.cpp"
//! headerfiles="assert.h stdlib.h mainwin.h gdsapp.h wxwin.h chandbas.h"

//: defenition
//!c-des: Class DebugDialog.
//!c_des: This class is used to show a debug dialog,
//!c_des: in which commands from a process file or  from history can be given to the commandline
//!c_des: to be executed by pressing the EXECUTE-button.
class Line_Parser;
class DebugDialog: public wxDialog
{
	public:

		// Constructor.
		DebugDialog(GDSMainWindow*,char*,long style = (wxTHICK_FRAME | wxCAPTION),char *name="frame");

      // Destructor.
		~DebugDialog();


      void 				OnCloseWindow(wxCloseEvent& event);
      void           SetCommand(char* BaseCommand);
		void				Hide(wxCommandEvent&);
		void				Stop(wxCommandEvent&);
      void				Execute(wxCommandEvent&);
		void				Continue(wxCommandEvent&);
      void				NextProc(wxCommandEvent&);
      void				Copy(wxCommandEvent&);
		void 				Init(char* processfile);
		void 			   OnIdle(wxIdleEvent& event);

	protected:

      // Pointer to mainwindow.
		GDSMainWindow*	m_parent;

      // wxWindows class.
		wxPanel*			m_panel;

      // wxWindows class.
      wxStaticText*  m_history;

      // wxWindows class.
      wxStaticText*  m_comline;


      // wxWindows class.
		wxButton*		m_execButton;

      // wxWindows class.
      wxButton*		m_copyButton;

      // wxWindows class.
		wxButton*		m_hideButton;

      // wxWindows class.
      wxTextCtrl*		m_editField;

      // wxWindows class.
		wxListBox*		m_lb;

      // wxWindows class.
		wxButton*		m_stopButton;

      // wxWindows class.
      wxButton*		m_continueButton;

      // wxWindows class.
		wxButton*		m_nextprocButton;

      char 	 m_commandstring[1000];
      bool m_is_executed;
   	bool m_endoffile;
      bool m_stop_at_next_file;
      //to read and parse lines from the process file
		Line_Parser*		m_lineparser;

  		DL_List<void*>*   _process_file_stack;

      // Declare used events.
      DECLARE_EVENT_TABLE()

};

class StopDialog: public wxDialog
{
	public:

		// Constructor.
		StopDialog(GDSMainWindow*,char*,long style = (wxTHICK_FRAME | wxCAPTION),char *name="frame");

      // Destructor.
		~StopDialog();


      void 				OnCloseWindow(wxCloseEvent& event);
		void				Stop(wxCommandEvent&);

	protected:

      // Pointer to mainwindow.
		GDSMainWindow*	m_parent;

      // wxWindows class.
		wxPanel*			m_panel;

      // wxWindows class.
		wxButton*		m_stopButton;

      DECLARE_EVENT_TABLE()

};

#endif
