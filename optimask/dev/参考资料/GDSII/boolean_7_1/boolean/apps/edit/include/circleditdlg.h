//! author="A.Rona"
//! version="1.0"
/*
 * Definition of class for dialog to specify radius and units.
*/

#ifndef CIRCLEDIT_H
#define CIRCLEDIT_H
#ifdef __GNUG__
#pragma interface
#endif

//! codefiles="textinsdlg.cpp"
//! headerfiles="chandbas.h mainwin.h string.h gdsapp.h"

//:defenition
//!c_des: Class CircleditDialog.
//!c_des: This class is used to show a settings dialog, in which the radius
//!c_des: and the units can be given by the user.
#include "tool.h"

class CircleditDialog: public wxDialog
{
	public:
   	// Constructor.
		CircleditDialog(EditCircleTool* tool,GDSMainWindow* parent, long style = (wxTHICK_FRAME | wxCAPTION));

      // Windows initialization.
      void				SetupWindow();

      // Return width.
      double 			GetRadius();

      // Get Unitsetting.
      char* GetUnit();

      // Set Unitvalue.
      void SetUnit(char* buf);

      // Close window if OK-button is pressed.
		void				CmOk(wxCommandEvent &);

      // Use new settings without closing the window.
      void 				CmApply(wxCommandEvent &);

      // Close window if EXIT-button is pressed.
    	void 				OnCloseWindow(wxCloseEvent& event);

      void Update(wxUpdateUIEvent&);
      void FocusSet(wxFocusEvent &);
      void FocusKill(wxFocusEvent &);
      void OnActivate(wxActivateEvent& event);

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

      EditCircleTool*   _tool;

      bool _focus;
      
      // Declare used events.
      DECLARE_EVENT_TABLE()
};

#endif
