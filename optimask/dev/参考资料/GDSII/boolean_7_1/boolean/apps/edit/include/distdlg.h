
//! author"M. Reemst"
//! version="1.0"
/*
 * Definition of class for dialog to display distances between points
*/

#ifndef DISTDLG_H
#define DISTDLG_H

#ifdef __GNUG__
#pragma interface
#endif

//! codefiles="distdlg.cpp"
//! headerfiles="stdlib.h math.h mainwin.h wxwin.h distdlg.h chandbas.h"

//:defeniton
//!c_des: Class DistanceDlg.
//!c_des: This class is used to show a distance dialog, in which the startpoint and a list of relative points are showed,
//!c_des: each with info on the distance (x, y and total) and the angle between this point and the startpoint.
//!c_des: Selecting a new startpoint, will clear all relative points.
//!c_des: When Clear is clicked, all relative points are removed and startpoint is set at (0,0). This also happens when a the program is started or when a new file is loaded.

class DistanceDlg: public wxDialog
{
	public:
   	// Constructor.
		DistanceDlg(GDSMainWindow*,char*,long style = (wxTHICK_FRAME | wxCAPTION),char *name="frame");

      // Destructor.
		~DistanceDlg();

      // Hide window if HIDE-button is pressed.
      void				hide(wxCommandEvent&);

      // Close window if EXIT-button is pressed.
      void 				OnCloseWindow(wxCloseEvent& event);

      //
      void				addCommand(char *);

      // Set startpoint x and y.
		void           setStartPoint(double, double);

      // Set relative point.
		void           setRelPoint(double, double);

 		// Clear all settings.
      void				ClearMe(wxCommandEvent&);

	protected:

   	// Pointer to mainwindow.
		GDSMainWindow*	_parent;

      // wxWindows class.
      wxStaticText*  _m_start;

      // wxWindows class.
      wxStaticText*  _m_lb;

      // wxWindows class.
		wxPanel*			_panel;

      // wxWindows class.
		wxButton*		_clearButton;

      // wxWindows class.
		wxButton*		_hideButton;

      // wxWindows class.
      wxTextCtrl*	   _editField;

      // wxWindows class.
		wxListBox*		_lb;

      //
	  	char				_temp_char[60];

      //
    	char				_start_point[30];

      // double which contains startpoint x.
      double			_start_x;

      // double which contains startpoint y.
      double			_start_y;

      // Declare used events.
      DECLARE_EVENT_TABLE()

};

#endif
