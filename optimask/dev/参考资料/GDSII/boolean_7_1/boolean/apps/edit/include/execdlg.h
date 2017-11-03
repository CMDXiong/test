//! author="the unknown"
//! version="1.0"
/*
 * Definition of class for dialog to run/create a processfile, in which commands can be executed seperately.
*/
#ifndef EXECDLG_H
#define EXECDLG_H

#ifdef __GNUG__
#pragma interface
#endif

#include "misc.h"

//! codefiles="execdlg.cpp"
//! headerfiles="assert.h stdlib.h mainwin.h gdsapp.h wxwin.h eventids.h chandbas.h"

//: defenition
//!c-des: Class ExecDlg.
//!c_des: This class is used to show an execution dialog, in which commands from history can be given to the commandline
//!c_des: to be executed by pressing the EXECUTE-button.  

class ExecDlg: public wxDialog
{
	public:

		// Constructor.
		ExecDlg(GDSMainWindow*,char*,long style = (wxTHICK_FRAME | wxCAPTION),char *name="frame");

      // Destructor.
		~ExecDlg();

      // Don't show window if HIDE-button is pressed.
      void				hide(wxCommandEvent&);

      // Close window if EXIT-button is pressed.
      void 				OnCloseWindow(wxCloseEvent& event);

      void           SetCommand(char* BaseCommand);

      // Add command to history.
      void				addCommand(const wxString& command);

      // Copy command selected from history to commandline.
		void				Copy(wxCommandEvent&);

      // Execute command which is given in the commandline.
      void				Execute(wxCommandEvent&);
	  void				DoExecute();

      // wxWindows class.
      wxTextCtrl*		_editField;

	protected:
		void 				OnChar(wxKeyEvent& event);

		void 				OnCharHook(wxKeyEvent& event);


      // Pointer to mainwindow.
		GDSMainWindow*	_parent;

      // wxWindows class.
      wxStaticText*  _m_history;

      // wxWindows class.
      wxStaticText*  _m_comline;

      // wxWindows class.
		wxPanel*			_panel;

      // wxWindows class.
		wxButton*		_execButton;

      // wxWindows class.
      wxButton*		_copyButton;

      // wxWindows class.
		wxButton*		_hideButton;

      // wxWindows class.

      wxListBox*		_lb;

      // Declare used events.
      DECLARE_EVENT_TABLE()

};

#endif
