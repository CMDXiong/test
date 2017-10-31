//! author="The Unknown"
//! version="1.0"
/*
 * Definition of class for dialog to specify the factor settings.
*/

#ifndef FACDLG_H
#define FACDLG_H

#ifdef __GNUG__
#pragma interface
#endif

#include <wx/notebook.h>

//! codefiles="facdlg.cpp"
//! headerfiles="chandbas.h mainwin.h string.h"

//:defenition
//!c_des: Class FactorsDialog.
//!c_des: This class  is used to show a factors dialog, in which factors, arc/polygon conversion,
//!c_des: smoothing and display options can be set. 

class Factors: public wxDialog
{
	public:

   	// Constructor.
		Factors(GDSMainWindow* parent,long style = (wxTHICK_FRAME | wxCAPTION));

      // Destructor.
		~Factors();

      // windows initialization.
		void				SetupWindow();

      // Close window if OK-button is pressed.
		void				CmOk(wxCommandEvent&);

      // Close window if CANCEL-button is pressed.
		void				CmCancel(wxCommandEvent&);

      // Close window if EXIT-button is pressed.
      void 				OnCloseWindow(wxCloseEvent& event);

	protected:

   	// get values from the controls.
		void				StoreValues();

      // wxWindows class.
      wxNotebook*    m_notebook;

      // Pointer to mainwindow.
		GDSMainWindow*	_parent;

      // wxWindows class.
		wxPanel*			_panel;

      // wxWindows class.
		wxPanel*			_panel2;

      // wxWindows class.
		wxPanel*			_panel3;

      // wxWindows class.
		wxPanel*			_panel4;

      // wxWindows class.
		wxButton*		_ok;

      // wxWindows class.
		wxButton*		_cancel;

      // wxWindows class.
		wxCheckBox*		_drawprim;

      // wxWindows class.
		wxRadioBox*			_ed_circleORmedial;

      // wxWindows class.
		wxTextCtrl*			_ed_selectmarge;

      // wxWindows class.
		wxTextCtrl*			_ed_snapfactor;

      // wxWindows class.
		wxTextCtrl*			_ed_correctionfactor;

      // wxWindows class.
		wxTextCtrl*			_ed_roundfactor;

      // wxWindows class.
		wxTextCtrl*			_ed_correctionaber;

      // wxWindows class.
		wxTextCtrl*			_ed_poly2arcrmin;

      // wxWindows class.
		wxTextCtrl*			_ed_poly2arcrmax;

      // wxWindows class.
		wxTextCtrl*			_ed_poly2arcaber;

      // wxWindows class.
		wxTextCtrl*			_ed_arc2polyaber;

      // wxWindows class.
		wxTextCtrl*			_ed_smoothaber;

      // wxWindows class.
		wxTextCtrl*			_ed_displayaber;

      // wxWindows class.
		wxTextCtrl*			_ed_maxlinemerge;

      // wxWindows class.
		wxTextCtrl*			_ed_structhres;

      // wxWindows class.
		wxTextCtrl*			_ed_primthres;

      // wxWindows class.
		wxStaticText*		_m_selectmarge;

      // wxWindows class.
		wxStaticText*		_m_selectmarge_comm;

      // wxWindows class.
		wxStaticText*		_m_snapfactor;

      // wxWindows class.
		wxStaticText*		_m_correctionfactor;

      // wxWindows class.
		wxStaticText*		_m_roundfactor;

      // wxWindows class.
		wxStaticText*		_m_correctionaber;

      // wxWindows class.
		wxStaticText*		_m_poly2arcrmin;

      // wxWindows class.
		wxStaticText*		_m_poly2arcrmax;

      // wxWindows class.
		wxStaticText*		_m_poly2arcaber;

      // wxWindows class.
		wxStaticText*		_m_arc2polyaber;

      // wxWindows class.
		wxStaticText*		_m_smoothaber;

      // wxWindows class.
		wxStaticText*		_m_displayaber;

      // wxWindows class.
		wxStaticText*		_m_maxlinemerge;

      // wxWindows class.
		wxStaticText*		_m_structhres;

      // wxWindows class.
		wxStaticText*		_m_primthres;

		char				_selectmarge[LINELENGTH];
		char				_snapfactor[LINELENGTH];
		char				_correctionfactor[LINELENGTH];
		char				_roundfactor[LINELENGTH];
		char				_correctionaber[LINELENGTH];
		char				_roundtype[LINELENGTH];
		char				_poly2arcrmin[LINELENGTH];
		char				_poly2arcrmax[LINELENGTH];
		char				_poly2arcaber[LINELENGTH];
		char				_arc2polyaber[LINELENGTH];
		char				_smoothaber[LINELENGTH];
		char				_displayaber[LINELENGTH];
		char				_maxlinemerge[LINELENGTH];
		char				_primthres[LINELENGTH];
		char				_structhres[LINELENGTH];

      bool         _drawsmallprim;

      // Declare used events.
      DECLARE_EVENT_TABLE()

};

#endif
