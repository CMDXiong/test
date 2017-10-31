//! author="A.Rona"
//! version="1.0"
/*
 * Definition of class for dialog to specify width, units and pathtype.
*/

#ifndef POLYEDIT_H
#define POLYEDIT_H
#ifdef __GNUG__
#pragma interface
#endif

//! codefiles="textinsdlg.cpp"
//! headerfiles="chandbas.h mainwin.h string.h gdsapp.h"

//:defenition
//!c_des: Class PolyeditDialog.
//!c_des: This class is used to show a settings dialog, in which the width
//!c_des: and the units can be given by the user.

class PolyeditDialog: public wxDialog
{
	public:
   	// Constructor.
		PolyeditDialog(GDSMainWindow* parent, long style = (wxTHICK_FRAME | wxCAPTION));

      // Windows initialization.
      void				SetupWindow();

      // Return width.
      double 			GetWidth();

      char* GetUnit();

      void SetUnit(char* buf);

      // Close window if OK-button is pressed.
		void				CmOk();

      // Use new settings without closing the window.
      void 				CmApply();

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
      wxStaticBox*		_widthbox;

      // wxWindows class.
      wxTextCtrl*			_width;

      // wxWindows class.
      wxChoice*         _settings_list;

      // wxWindows class.
      wxString				_choices[9];

      // String which contains acurracy setting.
      char					_acc[10];

      // String which contains unitsetting.
      char					_unit[10];

      // Declare used events.
      DECLARE_EVENT_TABLE()
};

#endif
