//! author="A.Rona"
//! version="1.0"
/*
 * Definitions of classes for dialog to insert/edit text
*/

#ifndef TEXTINSDLG_H
#define TEXTINSDLG_H

#ifdef __GNUG__
#pragma interface
#endif

#include "wxwin.h"

//! codefiles="textinsdlg.cpp"
//! headerfiles="wx/textdlg.h chandbas.h mainwin.h string.h gdsapp.h text.h toolcont.h tool.h elemlst.h command.h"

//:defenition
//!c_des: Class TextinsDialog.
//!c_des: This class is used in the class TextTool to show an edit-/inserttext dialog in which
//!c_des: a textvalue, a textheight and unitsetting can be given or edited.
//!rel_type: class: TextTool
class TextinsDialog: public wxDialog
{
	public:

   	//constructor.
		TextinsDialog(GDSMainWindow* parent);

      // Windows initialization.
      void				SetupWindow();

      void 				ChooseFont(wxCommandEvent& WXUNUSED(event));

      // Close window if OK-button is pressed.
      void				CmOk(wxEvent& event);

      // Close window if EXIT-button is pressed.
      void 				OnCloseWindow(wxCloseEvent& event);

      // Set a selected textvalue in edit-/inserttext dialog.
      void				SetText(const wxString&);

      // Set heightvalue in edit-/inserttext dialog, the heightsetting depends on the unitsetting.
      //! param: the_height: a string which contains a given heightvalue.
      void 				SetHeight(double the_height);

      // Set unitvalue in edit-/inserttext dialog, this value will also be copied into the string "_units",
      // which is described earlier.
      //! param: buf: a string which temporary contains a given unitsetting.
      void 				SetUnit(const wxString& buf);

      // Get and return selected unitvalue from edit-/inserttext dialog, this value will be copied into
      // the string "_units", which is described earlier.
      wxString			GetUnit();

      // Get and return textvalue from edit-/inserttext dialog.
      wxString			GetText();

      // Get heightvalue and unitssetting from edit-/inserttext dialog and return heightvalue depending on
      // chosen unitsetting.
      double         GetHeight();

   #if defined(__WXMSW__) && wxTEST_GENERIC_DIALOGS_IN_MSW
      void ChooseFontGeneric(wxCommandEvent& WXUNUSED(event) );
   #endif

	protected:

   	// Pointer to mainwindow.
   	GDSMainWindow*		_parent;

      // string which contains textvalue.
      wxString          _text;

      // string which contains unitsetting.
      wxString          _units;

		// wxWindows class.
		wxPanel*				_panel;

      // wxWindows class.
      wxStaticBox*      _textbox;

      // wxWindows class.
      wxStaticBox*      _heightbox;

      // wxWindows class.
      wxTextCtrl*			_text_insert;

      // wxWindows class.
      wxTextCtrl*			_text_height;

      // wxWindows class.
      wxChoice*         _text_units;

      //wxWindows class.
      wxButton*			_font_button;

      //wxWindows class.
      wxButton*			_font_generic;

      // wxWindows class.
      wxButton*         _ok;

      // wxWindows class.
      wxButton*			_cancel;

      wxFont*				_font;

      // wxWindows class.
      wxString 	  		_choices[9];

      // wxWindows class.
      double				 	_width;

      // wxWindows class.
      wxFont 				_text_font;

      // wxWindows class.
      wxColour 			_text_colour;

      // Declare used events.
      DECLARE_EVENT_TABLE()
};

#endif
