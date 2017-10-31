//! author="The Unknown"
//! version="1.0"
/*
 * Definition of class for dialog to specify the groupsettings.
*/

#ifndef GROUPDLG_H
#define GROUPDLG_H

#ifdef __GNUG__
#pragma interface
#endif


//! codefiles="groupdlg.cpp"
//! headerfiles="misc.h gdsapp.h lsetup.h misc.h mainwin.h chandbas.h wx/dynarray.h"

//:defenition
//!c_des: Class GroupDialog.
//!c_des: This class is used to show a groups dialog, in which the layers according to group A and Group B
//!c_des: can be specified.

#include "wxwin.h"
class GDSMainWindow;
class Setup;
class group;

class GroupDialog: public wxDialog
{
	public:

   	// Constructor.
		GroupDialog(GDSMainWindow* parent,long style = (wxTHICK_FRAME | wxCAPTION));

      // Destructor.
		~GroupDialog();

		// Return string group A.
		char*             Get_Group_A(char* group_str);

      // Return string group B.
		char*             Get_Group_B(char* group_str);

      // Return string destination.
		char*             Get_Destination(char* group_str);

      // add item to group a.
		void					CmAdd_A(wxCommandEvent&);

      // add item to group b.
		void			      CmAdd_B(wxCommandEvent&);

      // add item to destination.
		void			      CmAdd_D(wxCommandEvent&);

      // remove item from a.
		void			      CmRemove_A(wxCommandEvent&);

      // remove item from b.
		void			      CmRemove_B(wxCommandEvent&);

      // remove item from destination.
		void			      CmRemove_D(wxCommandEvent&);

      // remove all items from a.
		void			      CmClear_A(wxCommandEvent&);

      // remove all items from b.
		void			      CmClear_B(wxCommandEvent&);

      // Close window if OK-button is pressed.
		void			      CmOk(wxCommandEvent&);

      // Close window if CANCEL-button is pressed.
		void					CmCancel(wxCommandEvent&);

  		// for the dialog initialization
		void					SetupWindow(Setup* setup, group* old_A, group* old_B, int old_D,bool old_add);

      // Close window if EXIT-button is pressed.
      void 				   OnCloseWindow(wxCloseEvent& event);

	protected:

   	// is the layer in a.
		bool		      In_A(int);

      // is the layer in b.
		bool		      In_B(int);

      // wxWindows class.
		wxPanel*				_panel;

      // wxWindows class.
		wxButton*			_ok;

      // wxWindows class.
		wxButton*			_cancel;

      // wxWindows class.
		wxButton*			_adda;

      // wxWindows class.
		wxButton*			_rema;

      // wxWindows class.
		wxButton*			_clra;

      // wxWindows class.
		wxButton*			_addb;

      // wxWindows class.
		wxButton*			_remb;

      // wxWindows class.
		wxButton*			_clrb;

      // wxWindows class.
		wxButton*			_addd;

      // wxWindows class.
		wxButton*			_remd;

      // wxWindows class.
		wxStaticBox*			_gb1;

      // wxWindows class.
		wxStaticBox*			_gb2;

      // wxWindows class.
		wxStaticBox*			_gb3;

      // wxWindows class.
		wxStaticBox*			_gb4;

      // wxWindows class.
		wxCheckBox*			_onlyselect_A;

      // wxWindows class.
		wxCheckBox*			_onlyselect_B;

      // wxWindows class, the main listbox control.
		wxListBox*			_listbox_main;

      // wxWindows class, the a listbox control.
		wxListBox*			_listbox_a;

      // wxWindows class, the b listbox control.
		wxListBox*			_listbox_b;

      // wxWindows class, the destination edit control.
		wxTextCtrl*				_destination;

      // wxWindows class.
		wxCheckBox*			m_add_to_destination;

      // Pointer to class setup.
  		Setup*				_setup;

      // Pointer to mainwindow.
		GDSMainWindow*		_parent;

      // group a array (dynamic)
		group*				group_A;

      // group b array (dynamic)
		group*				group_B;

      // the destination layer
		int					_destinationnr;

      // Declare used events.
      DECLARE_EVENT_TABLE()

};

#endif

