//! author="The unknown"
//! version="1.0"
/*
 * Definition of class for dialog to view the used structures in a GDSII-file.
*/

#ifndef STRUCDLG_H
#define STRUCDLG_H

#ifdef __GNUG__
#pragma interface
#endif

#include "gdsii.h"

//! codefiles="strucdlg.cpp"
//! headerfiles="struclst.h"

//:defenition
//!c_des: Class StructuresDialog.
//!c_des: This class is used to show a structures dialog. When a file is loaded, all structures are drawn on the screen.
//!c_des: It's also possible to view the structures seperately. Select one of the structures and click Ok to view this structure.
class StructuresDialog: public wxDialog
{
	public:

   	// constructor.
		StructuresDialog(GDSMainWindow* parent, long style= (wxTHICK_FRAME | wxCAPTION));

      // destructor.
		~StructuresDialog();

      // Pointer to structure class.
		Structure*		GetStructure();

      // Windows initialisation
		void 				SetupWindow(GdsII* gdsii);

      // Close window if OK-button is pressed.
		void				CmOk(wxCommandEvent&);

      // Close window if CANCEL-button is pressed.
		void				CmCancel(wxCommandEvent&);

      // View selected structure.
		void				CmShow(wxCommandEvent&);

      // Close window if EXIT \-button is pressed.
     	void 				OnCloseWindow(wxCloseEvent& event);

	protected:
		void 				Update(wxUpdateUIEvent &);

      // wxWindows class.
		wxPanel*			_panel;

      // wxWindows class.
		wxStaticBox*		_gb;

      // wxWindows class.
		wxStaticText*		_mess1;

      // wxWindows class.
		wxStaticText*		_mess2;

      // wxWindows class.
		wxListBox*		_listbox;

      // wxWindows class.
		wxButton*		_button1;

      // wxWindows class.
		wxButton*		_button2;

      // wxWindows class.
		wxButton*		_buttonSHOW;

      // Pointer to mainwindow.
		GDSMainWindow*	_parent;

      // Pointer to structurelist class
		GdsII*			_structurelist;

      // Pointer to structure class which holds the choosen structure.
  		Structure*		_structure;

      // Declare used events.
      DECLARE_EVENT_TABLE()


};

//:defenition
//!c_des: Class StructuresDialog.
//!c_des: This class is used to show a structures dialog. When a file is loaded, all structures are drawn on the screen.
//!c_des: It's also possible to view the structures seperately. Select one of the structures and click Ok to view this structure.
class StructuresDialog2: public wxDialog
{
	public:

   	// constructor.
		StructuresDialog2(GDSMainWindow* parent, long style= (wxTHICK_FRAME | wxCAPTION));

      // destructor.
		~StructuresDialog2();

      // Pointer to structure class.
		Structure*		GetStructure();

      // Windows initialisation
		void 				SetupWindow(GdsII* gdsii);

      // Close window if OK-button is pressed.
		void				CmOk(wxCommandEvent&);

      // Close window if CANCEL-button is pressed.
		void				CmCancel(wxCommandEvent&);

      // Close window if EXIT \-button is pressed.
     	void 				OnCloseWindow(wxCloseEvent& event);

	protected:

      // wxWindows class.
		wxPanel*			_panel;

      // wxWindows class.
		wxStaticBox*		_gb;

      // wxWindows class.
		wxStaticText*		_mess1;

      // wxWindows class.
		wxStaticText*		_mess2;

      // wxWindows class.
		wxListBox*		_listbox;

      // wxWindows class.
		wxButton*		_button1;

      // wxWindows class.
		wxButton*		_button2;

      // Pointer to mainwindow.
		GDSMainWindow*	_parent;

      // Pointer to structurelist class
		GdsII*			_structurelist;

      // Pointer to structure class which holds the choosen structure.
  		Structure*		_structure;

      // Declare used events.
      DECLARE_EVENT_TABLE()


};
#endif

