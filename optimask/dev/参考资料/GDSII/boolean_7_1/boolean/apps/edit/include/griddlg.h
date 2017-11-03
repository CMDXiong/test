//! author="M. Reemst"
//! version"1.0"
/*
 * Definition of class for dialog to specify the gridsettings.
*/

#ifndef GRIDDLG_H
#define GRIDDLG_H

#ifdef __GNUG__
#pragma interface
#endif

#include <wx/colordlg.h>

//! codefiles="griddlg.cpp"
//! headerfiles="chandbas.h mainwin.h string.h gdsapp.h wx/colordlg.h"

//:defenition
//!c_des: Class griddlg.cpp.
//!c_des: This class is used to show a gridsettings dialog, in which the grid spacing, gridcolour,
//!c_des: and backgroundcolour can be given.


class Gridsettings: public wxDialog
{
	public:

   	// Constructor.
		Gridsettings(GDSMainWindow* parent, long style = (wxTHICK_FRAME | wxCAPTION));

      // Destructor.
		~Gridsettings();

      // Windows initialization
		void				SetupWindow();

      // Close window if OK-button is pressed.
		void				CmOk(wxCommandEvent&);

      // Close window if EXIT-button is pressed.
      void 				OnCloseWindow(wxCloseEvent& event);

      // Close window if CANCEL-button is pressed.
		void				CmCancel(wxCommandEvent&);

      // Set gridcolour.
		void 				Cm_g_Bitb(wxCommandEvent&);

      // Set background colour.
		void 				Cm_b_Bitb(wxCommandEvent&);

      // Set gridcolour.
		void				Set_Colour_Grid(long colour);

      // Set background colour.
		void				Set_Colour_Back(long colour);

      // Redraw grid.
      void				Redraw();

		void 				OnChar(wxKeyEvent& event);
		void				OnCharHook(wxKeyEvent& event);

	protected:

      // Pointer to mainwindow.
		GDSMainWindow*	_parent;

      // wxWindows class.
      wxStaticBox*		_space_box;

      // wxWindows class.
      wxStaticBox*		_colour_box;

      // wxWindows class.
		wxPanel*			_panel;

      // wxWindows class.
		wxButton*		_ok;

      // wxWindows class.
		wxButton*		_cancel;

      // wxWindows class.
      wxTextCtrl*		_ed_space;

      // wxWindows class.
		wxRadioBox*		_ed_backORfore;

      // wxWindows class.
      wxStaticText*	_m_colour;

      // wxWindows class.
      wxStaticText*	_m_bcolour;

      // wxWindows class.
      wxColour*		_gcolour;

      // wxWindows class.
      wxColour*		_bcolour;

      // wxWindows class.
      wxBitmap*      _g_bitm;

      // wxWindows class.
      wxBitmap*      _b_bitm;

      // wxWindows class.
		wxBitmapButton* _g_bitb;

      // wxWindows class.
		wxBitmapButton* _b_bitb;

      // wxWindows class.
		wxBrush*				_g_brush;

      // wxWindows class.
		wxBrush*				_b_brush;

      // wxWindows class.
		wxColourData* 		_cdata;

      // wxWindows class.
		wxColourDialog* 	_cd;

      // wxWindows class.
      wxChoice*         _units_list;

      // wxWindows class.
      wxString				_choices[9];

      // String which contains gridspace.
      char					_space[30];

      // Declare used events.
      DECLARE_EVENT_TABLE()

};

#endif
