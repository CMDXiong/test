//! author="A.Rona"
//! version="1.0"               
/*
 * Definition of class for dialog to specify newfile settings.
*/

#ifndef NEWFILEDLG_H
#define NEWFILEDLG_H

#ifdef __GNUG__
#pragma interface
#endif

//! codefiles="newfiledlg.cpp"
//! headerfiles="wx/textdlg.h chandbas.h mainwin.h string.h gdsapp.h text.h toolcont.h tool.h elemlst.h command.h"

//: defenition
//!c_des: Class NewFileDialog.
//!c_des: This class is used to show a newfilesettings dialog, in which the layoutsize and
//!c_des: unitsetting can be given by the user.
class NewFileDialog: public wxDialog
{
	public:

      // Constructor.
		NewFileDialog(GDSMainWindow* parent, long style = (wxTHICK_FRAME | wxCAPTION));

      // Destructor.
      ~NewFileDialog();

      // Windows initialization.
      void				SetupWindow();

      // Close window if EXIT-button is pressed.
    	void 				OnCloseWindow(wxCloseEvent& event);

      // Close window if OK-button is pressed.
      void				CmOk(wxCommandEvent&);

	protected:

   	// Pointer to mainwindow.
		GDSMainWindow*		_parent;

      // wxWindows class.
		wxPanel*				_panel;

      // wxWindows class.
      wxStaticBox*		_layoutsize_box;

      // wxWindows class.
      wxStaticBox*		_units_box;

      // wxWindows class.
      wxTextCtrl*    	_layout_x;

      // wxWindows class.
      wxTextCtrl*			_layout_y;

      // wxWindows class.
      wxTextCtrl*		 	_ed_acc;

      // wxWindows class.
      wxButton*			_ok;

      // wxWindows class.
      wxChoice*			_units_list;

      // wxWindows class.
      wxString 	  		_choices[9];

      // String which contains the accuracy value.
      char					_acc[10];

      // String which contains the unitsetting.
      char					_unit[10];

      // Declare used events.
      DECLARE_EVENT_TABLE()
};

#endif
