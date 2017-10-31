// DDEHandler.h : main header file for the NETACCESS application
//

#ifndef DDEHANDLER_H
#define DDEHANDLER_H

#ifdef __GNUG__
#pragma interface
#endif

#ifndef __UNIX__
#include <windows.h>
#include <ddeml.h>
#endif

#include <iostream>
#include "misc.h"
#include "wx/string.h"

class CommandHandler;

class DDEHandler
{
	public:
      DDEHandler(CommandHandler*);
		~DDEHandler();

#ifndef __UNIX__
      void startLoadingWin(const wxString& csLocation);
		void StartBrowser(const wxString& csLocation);
//      void Reset();
#endif
      void startLoadingUnix(const wxString&);

	protected:
#ifndef __UNIX__
	   HSZ hszServName;
      HCONV hConv;
		bool _connected;
#endif
};

#ifndef __UNIX__
HDDEDATA CALLBACK LocationDdeCallBack(UINT type, UINT fmt, HCONV hconv, HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD dwData1,DWORD dwData2);
void ChangeSlashes(char* buf);
#endif

#endif
