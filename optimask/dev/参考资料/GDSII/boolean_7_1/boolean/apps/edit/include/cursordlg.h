//! author="A.Rona"
//! version="1.0"
/*
 * Definition of class for dialog to set the cursorposition.
*/
#ifndef CURSORDLG_H
#define CURSORDLG_H

#ifdef __GNUG__
#pragma interface
#endif

#include "wxwin.h"
#include "mainwin.h"
//! codefiles=""
//! headerfiles="chandbas.h mainwin.h string.h gdsapp.h"

//:defenition
//!c_des: Class CursorDialog.
//!c_des: This class is used to show a cursorsettings dialog, in which the cursorposition
//!c_des: can be given by the user.

class CursorDialog: public wxDialog
{
	public:
   	// Constructor.
//		CursorDialog(GDSMainWindow* parent, long style = (wxTHICK_FRAME | wxCAPTION));
		CursorDialog(GDSMainWindow* parent, long style);

      // Windows initialization.
      void				SetupWindow();

      // Return width.
      double 			GetCursorX();

      // Get Unitsetting.
      double 			GetCursorY();

      // Set X position of cursor.
      void 				SetCursorX(double);

      // Set Y position of cursor.
      void 				SetCursorY(double);

      // Close window if OK-button is pressed.
		void				CmOk(wxCommandEvent &);

      // Use new settings without closing the window.
      void 				CmApply(wxCommandEvent &);

      // Close window if EXIT-button is pressed.
    	void 				OnCloseWindow(wxCloseEvent& event);

	protected:

      // Pointer to mainwindow.
		GDSMainWindow*		_parent;

      // wxWindows class.
		wxPanel*				_panel;

      // wxWindows class.
		wxButton*			_ok;

      // wxWindows class.
      wxButton*			_apply;

      // wxWindows class.
      wxStaticBox*		_XYbox;

      // wxWindows class.
      wxStaticBox*		_units_box;

      // wxWindows class.
      wxTextCtrl*			_XX;
		
		//wxWindows class.
		wxTextCtrl*			_YY;
		
		wxTextCtrl*			_accuracy;

      // wxWindows class.
      wxChoice*			_units_list;
		
      // wxWindows class.
      wxString 	  		_choices[9];

      // X-coordinate of cursorposition.
      double				  	X;

      // Y-coordinate of cursorposition.
      double					Y;

      // String which contains the accuracy value.
      char					_acc[10];

      // String which contains the unitsetting.
      char					_unit[10];


      // Declare used events.
      DECLARE_EVENT_TABLE()
};

#endif
