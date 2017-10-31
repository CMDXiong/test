//! author="A.Rona"
//! version="1.0"
/*
 * Definition of class for dialog to specify width, units and pathtype.
*/

#ifndef SETTINGS_H
#define SETTINGS_H

#ifdef __GNUG__
#pragma interface
#endif

//! codefiles="textinsdlg.cpp"
//! headerfiles="chandbas.h mainwin.h string.h gdsapp.h"

//:defenition
//!c_des: Class SettingsDialog.
//!c_des: This class is used to show a settings dialog, in which the pathtype, width
//!c_des: and the units can be given by the user.

class SettingsDialog: public wxDialog
{
	public:
   	// Constructor.
		SettingsDialog(GDSMainWindow* parent, long style = (wxTHICK_FRAME | wxCAPTION));

      // Windows initialization.
      void				SetupWindow();

      // Close window if OK-button is pressed.
		void				CmOk(wxCommandEvent &);

      // Use new settings without closing the window.
      void 				CmApply(wxCommandEvent &);

      // Close window if EXIT-button is pressed.
    	void 				OnCloseWindow(wxCloseEvent& event);

      double	  		GetPathtype();

      char*				GetPathwidth();

      // Get pathtype selection and give command "setpathtype" in the commandhandler, which returns TRUE
      // if a pathtype has been selected.
		void 				CmRadioButton(wxCommandEvent & event);


	protected:

		char* 			GetUnit();

      // Pointer to mainwindow.
		GDSMainWindow*		_parent;

      // wxWindows class.
		wxPanel*				_panel;

      // wxWindows class.
		wxButton*			_ok;

      // wxWindows class.
      wxButton*			_apply;

      // wxWindows class.
		wxRadioBox*			_ed_setpath;

      // wxWindows class.
		wxCheckBox*			_spline;

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
