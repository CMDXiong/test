/*
Program			GDSAPP.H
Purpose			A postprocessing program for MIC
Programmers		R. Spekreijse & N. Noorlander
Last Update		22-09-1995
*/

#ifndef _GDSAPP_H
#define _GDSAPP_H

#ifdef __GNUG__
#pragma interface
#endif

#include "mainwin.h"
#include "winmess.h"

#include "misc.h"

extern StatusBar 	*_statusbar;
extern GDSMessage 	*_messagehandler;

class GDSMainWindow;


class GDS_application : public wxApp
{
	public:
	   GDS_application();
		bool OnInit(void);
		int OnRun();
	    int	OnExit(void);
		WINMessage* 	_win_messagehandler;

		virtual void 	InitMainWindow();
		GDSMainWindow*	GetMainWindow();

        bool Initialized();

	protected:
		wxMenuBar		*SetupMenuBar();
		GDSMainWindow	*_GDSMainWindow;
};
DECLARE_APP(GDS_application)

#endif

