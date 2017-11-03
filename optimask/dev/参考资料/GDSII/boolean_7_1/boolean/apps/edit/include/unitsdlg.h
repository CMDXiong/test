//! author="M. Reemst, last modified by A.Rona"
//! version="1.0"
/*
 *  Definition of class for dialog to specify the transformation values.
*/

#ifndef UNITSDLG_H
#define UNITSDLG_H

#ifdef __GNUG__
#pragma interface
#endif

#include "chandbas.h"
#include "wxwin.h"

//! codefiles="unitsdlg.cpp"
//! headerfiles="chandbas.h mainwin.h"

//: defenition
//!c_des: Class Unitssettings.
//!c_des: This class is used to show a unitssettings dailog, in which the accuracy and
//!c_des: the unittype can be given by the user.

class Unitssettings: public wxDialog
{
	public:

   	// Constructor.
		Unitssettings(GDSMainWindow* parent,long style = (wxTHICK_FRAME | wxCAPTION));

		// Destructor.
		~Unitssettings();

      // windows initialization.
		void				SetupWindow();

      // Close window if OK-Button is pressed.
		void				CmOk(wxCommandEvent &);

      // Close window if EXIT-button is pressed.
      void 				OnCloseWindow(wxCloseEvent& event);

	protected:

   	// Pointer to Mainwindow.
		GDSMainWindow*		_parent;

      // wxWindows class.
		wxPanel*			_panel;

      // wxWindows class.
		wxButton*		_ok;

      // wxWindows class.
      wxTextCtrl*		_ed_acc;

      // wxWindows class.
		wxChoice*		_units_list;

      // wxWindows class.
      wxStaticBox*   _accbox;

      // wxWindows class.
      wxString 	  	_choices[10];

		// String which contains the unitsetting.
      char				_acc[10];

      // String which contains the accuracy value
      char				_unit[10];

		// Declare used events.
      DECLARE_EVENT_TABLE()
};

#endif
