//! author="klaas holwerda"
//! version="1.0"
/*
Program	WINDRV.H
Purpose	Window maintaining a double buffer for drawing
*/

#ifndef _WIN_DRIVER
#define _WIN_DRIVER

#ifdef __GNUG__
#pragma interface
#endif

#define NR_PATTERNS 72

#include "wxwin.h"

#include "gdserr.h"
#include "scrndrv.h"
#include "lsetup.h"
#include "gdsii.h"
#include "mainwin.h"

class GDSMainWindow;

//! Window that is used to draw on,
/*!it maintains a bitmap of all that is drawn sofar.
It is derived from wxWindow.
The wxClientDCbuf and wxPaintDCbuf can be used to fill this bitmap while drawing
on the Canvas at the same time. The OnPaint will use the bitmap to quickly redraw if possible.
Because the inheritance from wxWindow, all mouse input and keyboard input to the window
is also intercepted here, class Toolcontrol, pushes an eventhandler to intercept event from
from this window.
Depending on the tool in action events will be
redirected to the right tool using the toolcontroller.
\sa  Toolcontrol
\sa  BaseTool
\sa  DrawDriver
*/
class Canvas: public wxWindow
{
	public:
		//! constructor
		Canvas(GDSMainWindow* parent, int , int , int , int , long , char*);

		~Canvas(void);

      //!are there events in the queue
		bool Pending();

      //!character events
		void	OnChar(wxKeyEvent& event);
      //!keydown events
		void	OnKeyDown(wxKeyEvent& event);
      //!keyup events
		void	OnKeyUp(wxKeyEvent& event);
      //!mouse events
		void 	OnMouseEvent(wxMouseEvent& event);
      //!called by onpaint event
		void	OnPaint(wxPaintEvent&);

      //!set the background color of the window
		void  SetBackGroundColor(long color);

      //!to write bitmap data to files
      wxBitmap* GetCanvasBitmap();

      //!Set if the drawing in the bitmap/canvas up to date
      //!if set to false a complete redraw will be the result, in order to
      //!bring the drawing up to date.
      void Set_Valid_drawing(bool value);

      //!is the drawing in the bitmap/canvas up to date?
      bool Get_Valid_drawing();

      //!redraw evrything, outside a Onpaint.
      //!Refresh(), is not a solution in some cases, since is a delayed OnPaint.
      //!Often the result needs to be directly vissible, before the next step in the program.
      void Update();

      //!scroll left right or up down
      void ScrollIt(double xy,bool vertical);

      //!for updating size of window, reallocates buffer for dubbel buffering
      void OnSize(wxSizeEvent& event);

      //!adjust the scrollbars to new position
      void AdjustScrollBars(int posx,int dx, int totx,int posy,int dy, int toty);

      //!scroll events
      void OnScroll(wxScrollWinEvent& event);


	private:

      //!convert color to wxWindows color
		void 				ltowxc(long l,wxColour& color);

      //! screen buffer
		wxBitmap*	  	_backup;

      //!size of window
		int					_w,_h;

      //!is the drawing valid
      bool 			m_valid_drawing;


	public:
      DECLARE_EVENT_TABLE()


};

#endif

