//! author="klaas holwerda"
//! version="1.0"
#ifndef _PRINTERDRIVER
#define _PRINTERDRIVER

#ifdef __GNUG__
#pragma interface
#endif


//! headerfiles="scrndrv.h lsetup.h gdsii.h mainwin.h"
//! codefiles="printer.cpp"

#include "wx/prntbase.h"
#include "scrndrv.h"
#include "lsetup.h"
#include "gdsii.h"
#include "mainwin.h"

#if wxUSE_PRINTING_ARCHITECTURE

//!low level drawing function for primitives to draw to the printer
/*!
it is derived from wxPrintout as well as the drawdriver.
And therefore the mediator between the internal database and the printer device
see for more info related types.
\sa WindowsDriver
*/
class PrintDriver : public wxPrintout
{
	public:
		PrintDriver(GDSMainWindow*,char *);
		~PrintDriver(void);

      bool 	OnPrintPage(int);
		bool 	OnBeginDocument(int startPage, int endPage);
		bool 	HasPage(int page);
		void 	GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

	private:

      //! the device context for drawing
		wxDC*	  				_dc;

		int					_w,_h;
      bool            _crosshair;

	private:
		GDSMainWindow*	_parent;
};

#endif //wxUSE_PRINTING_ARCHITECTURE


#endif

