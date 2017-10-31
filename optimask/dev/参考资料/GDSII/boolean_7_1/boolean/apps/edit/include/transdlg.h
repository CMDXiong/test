//! author "M. Reemst, last modified by A. Rona"
//! version="1.0"
/*
 *Definition of class for dialog to specify transformation values.
*/

#ifndef TRANSDLG_H
#define TRANSDLG_H

#ifdef __GNUG__
#pragma interface
#endif

//! codefiles="transdlg.cpp"
//! headerfiles="chandbas.h mainwin.h string.h gdsapp.h"

//:defenition
//!c_des: Class Transformations.
//!c_des: This class is used to show a transformations dialog, in which can be mirrored and moved horizontally and
//!c_des: vertically, scaled(factor), rotated(degrees) and the direction can be given (clockwise or counter clockwise).
class Transformations: public wxDialog
{
	public:

   	// Constructor.
		Transformations(GDSMainWindow* parent,long style = (wxTHICK_FRAME | wxCAPTION));

      // Destructor.
		~Transformations();

      // Windows initialization.
		void				SetupWindow(bool);

      // Close window if OK-button is pressed.
		void				CmOk(wxCommandEvent &);

      // Set default values if RESET-button is pressed.
		void				CmReset(wxCommandEvent &);

      // Use new settings without closing the window.
	   void        	CmApply(wxCommandEvent &);

      // Restore last settings without closing the window.
    	void        	CmUndo(wxCommandEvent &);

      // Called when checkbox "_item" was clicked.
    	void				EnableItem(char*, bool);

      // Close window if EXIT-button is pressed.
    	void 				OnCloseWindow(wxCloseEvent& event);

	protected:
      // Should the action be peformed on Group A (false) or the all layers structures (true)
   	bool			_all_layers;

      // Pointer to mainwindow.
		GDSMainWindow*	_parent;

      // wxWindows class.
		wxPanel*			_panel;

      // wxWindows class.
		wxButton*		_ok;

      // wxWindows class.
		wxButton*		_reset;

      // wxWindows class.
	   wxButton*		_apply;

      // wxWindows class.
    	wxButton*   	_undo;

      // wxWindows class.
		wxStaticBox*		_movebox;

      // wxWindows class.
		wxStaticBox*		_mirrorbox;

      // wxWindows class.
		wxStaticBox*		_scalebox;

      // wxWindows class.
		wxStaticBox*		_rotatebox;

      // wxWindows class.
		wxStaticBox*		_selectbox;

      // wxWindows class.
		wxRadioBox*			_ed_cwORccw;

      // wxWindows class.
		wxTextCtrl*			_ed_scale_h;

      // wxWindows class.
		wxTextCtrl*			_ed_degrees;

      // wxWindows class.
		wxTextCtrl*			_ed_move_h;

      // wxWindows class.
		wxTextCtrl*			_ed_move_v;

      // wxWindows class.
      wxCheckBox*		_check_move;

      // wxWindows class.
      wxCheckBox*		_check_mirror_h;

      // wxWindows class.
      wxCheckBox*		_check_mirror_v;

      // wxWindows class.
		wxCheckBox*		_check_scale_h;

      // wxWindows class.
		wxCheckBox*		_check_ratio;

      // wxWindows class.
      wxCheckBox*		_check_rotate;

      // wxWindows class.
		wxCheckBox*		_check_select;

      // String which contains scale value.
      wxString			_scale_h_ratio;

      // String which contains degree value.
      wxString			_degr;

      // String which contains horizontal move value.
		wxString			_move_h;

      // String which contains vertical move value.
      wxString			_move_v;

      // Horizontal mirror value.
      bool			_mirror_x;

      // Vertical mirror value.
      bool			_mirror_y;

      // Declare used events.
      DECLARE_EVENT_TABLE()
};

#endif
