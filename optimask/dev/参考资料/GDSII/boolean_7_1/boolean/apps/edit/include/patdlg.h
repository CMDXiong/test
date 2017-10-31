//! author="R. Spekreijse & Chris Mollien"
//! version="1.0"
/*
 * Definition of class for dialog to specify pattern settings.
*/

#ifndef PATTERNDLG_H
#define PATTERNDLG_H

#ifdef __GNUG__
#pragma interface
#endif

#include "mainwin.h"
#include "lsetup.h"
#include "layprops.h"
#include "windrv.h"


//! codefiles="patdlg.cpp"
//! headerfiles="windrv.h"

//:defenition
//!c_des: Class PatternDialog.
//!c_des: Sets the patterndialog on the screen.
//!c_des: This class is a typical Windows class.
//!c_des: extensive changes for this class to work with wxWindows.
//!c_des: This class is no longer a typical windows class...

class PatternDialog: public wxDialog
{
	public:
		// Constructor.
		PatternDialog(LayerPropertiesDialog* parent,char* title,long style =0 ,char* name = "main_pattern_dlg");

      // Destructor.
		~PatternDialog();

	protected:

      // Close window if EXIT-button is pressed.
      void 	OnCloseWindow(wxCloseEvent& event);

      // Draw for each button its own pattern.
      void	Redraw();

      // Get chosen pattern.
		void 	Cm_P_BitB(wxEvent& event);

      // wxWindows class.
		wxColour* ltowxc(long colour);

      // wxWindows class.
      wxPanel*		_panel1;

      wxScrolledWindow* _scroll;

      // Pointer to parent, holds the parent window.
		LayerPropertiesDialog*	_parent;

      // wxWindows class, layer PATTERN  buttons.
		wxBitmapButton* 	_patternbut[NR_PATTERNS];

      //the choosen pattern.
      int 					_choosen;

      // Declare used events.
      DECLARE_EVENT_TABLE()

};

#endif

