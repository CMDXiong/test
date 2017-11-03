//! author="the unknown"
//! version="1.0"
/*
 * Definition off class for dialog to display the type of the selected structure.
*/

#ifndef IDENTDLG_H
#define IDENTDLG_H

#ifdef __GNUG__
#pragma interface
#endif

//! codefiles="identdlg.cpp"
//! headerfiles="assert.h stdlib.h mainwin.h wxwin.h eventids.h chandbas.h struclst.h gdsii.h shape.h referenc.h elemlst.h proper.h propedit.h"

//:defenition
//!c_des: Class IdentDlg.
//!c_des: This class is used to show an identification dialog, which can be used to display a selected
//!c_des: list which shows all selected structures, polygons, etc. in the drawing. When some elements become (de)selected, you'll have to click
//!c_des: the Update button to get the actual information. 


class IdentDlg: public wxDialog
{
	public:

   	// Constructor.
		IdentDlg(GDSMainWindow*,char*,long style = (wxTHICK_FRAME | wxCAPTION),char *name="identification dialog");

      // Destructor.
		~IdentDlg();

      // Hide window if HIDE-button is pressed.
      void				hide(wxCommandEvent&);

      // Close window if EXIT-button is pressed.
      void 				OnCloseWindow(wxCloseEvent& event);

      // Update information about selection.
      void				Update(wxCommandEvent&);

      // Make editing of selection possible.
      void				Edit(wxCommandEvent&);

	protected:

   	// Pointer to mainwindow.
		GDSMainWindow*	_parent;

      // wxWindows class.
		wxPanel*			_panel;

      // wxWindows class.
		wxButton*		_updateButton;

      // wxWindows class.
		wxButton*		_hideButton;

      // wxWindows class.
		wxButton*		_editButton;

      // wxWindows class.
		wxListBox*		_lb;

      // Declare used events.
      DECLARE_EVENT_TABLE()

};

#endif
