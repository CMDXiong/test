
#ifdef __GNUG__
#pragma implementation
#endif
#include "ddehand.h"
#include "chandbas.h"
#include "wxwin.h"
#ifndef __UNIX__
#include <process.h>
DWORD dwidInst;
#endif

CommandHandler* _chandler;

DDEHandler::DDEHandler(CommandHandler* chandler)
{
#ifndef __UNIX__
	hConv = NULL;
	_connected = false;
	dwidInst = 0ul;
	if(!DdeInitialize(&dwidInst, LocationDdeCallBack, APPCMD_FILTERINITS, 0))
   {
	   hszServName = DdeCreateStringHandle(dwidInst, "Boolean", CP_WINANSI);
   	DdeNameService(dwidInst, hszServName, 0L, DNS_REGISTER);
	}
   _chandler = chandler;
#endif
}

DDEHandler::~DDEHandler()
{
#ifndef __UNIX__
	DdeUninitialize(dwidInst);
#endif
}

#ifndef __UNIX__

/*void DDEHandler::Reset()
{
   _connected=false;
}*/

void DDEHandler::startLoadingWin(const wxString& csLocation)
{
	char csItem[LINELENGTH];
	char csTemp[LINELENGTH];
   csItem[0] = '\0';
   csTemp[0] = '\0';

	if(dwidInst == 0UL){
			MessageBox(NULL,"Dde Initialization Failed...","Error", MB_OK);
			return;
	}

	//Connect to the Netscape or IExplore server.
	char* browser="NETSCAPE";

	HSZ hszService = DdeCreateStringHandle(dwidInst, browser, CP_WINANSI);

	HSZ hszTopic = DdeCreateStringHandle(dwidInst, "WWW_OpenURL", CP_WINANSI);

	hConv = DdeConnect(dwidInst, hszService, hszTopic, NULL);



	// Check for the connection

	if(hConv == 0UL)

	{

		browser="IExplore";

		HSZ hszService = DdeCreateStringHandle(dwidInst, browser, CP_WINANSI);

		HSZ hszTopic = DdeCreateStringHandle(dwidInst, "WWW_OpenURL", CP_WINANSI);

		hConv = DdeConnect(dwidInst, hszService, hszTopic, NULL);

	}



	// Check for the connection

	if(hConv == 0UL)

	{

		StartBrowser(csLocation); // There was no open browser found, so start one

		return;

	}



	// Add quotes to the URL argument as it's of type qcs (quoted string)



	if (strcmp(browser,"NETSCAPE")==0)

	{

		strcpy(csTemp,"\""); //Left Quote.

		strcat(csTemp,csLocation);

		strcat(csTemp,"\"");//Right Quote.

	}

	else

	{

		strcpy(csTemp,"\""); //Left Quote.

		strcat(csTemp,csLocation);

		strcat(csTemp,"\""); //Right Quote.

		strcat(csTemp,",,-1,,,,,");

	}



	strcat(csTemp,",,0xFFFFFFFF,0,,,NETREPORT");//0xFFFFFFFF for last active Window. 0 for new window



	//Convert the Item into string handle and pass it to server.

	HSZ hszItem = DdeCreateStringHandle(dwidInst, csTemp, CP_WINANSI);

	DdeClientTransaction(NULL, 0ul, hConv, hszItem, CF_TEXT, XTYP_REQUEST, 1000L, NULL);
}

void DDEHandler::StartBrowser(const wxString& csLocation)
{

	char buf[200];

	char buf2[200];

	char browser[200];

	HKEY keyh=NULL;

	long grootte_b = (long)sizeof(browser);

	RegOpenKeyEx(HKEY_CLASSES_ROOT,(LPCTSTR)".html",0,KEY_QUERY_VALUE,&keyh);

	RegQueryValue(HKEY_CLASSES_ROOT,(LPCTSTR)".html",browser,&grootte_b);



	strcat(browser,"\\shell\\open\\command");

	long grootte = (long)sizeof(buf);

	RegOpenKeyEx(HKEY_CLASSES_ROOT,(LPCTSTR)browser,0,KEY_QUERY_VALUE,&keyh);

	RegQueryValue(HKEY_CLASSES_ROOT,(LPCTSTR)browser,buf,&grootte);

	int i = 0;

	int j = 0;

	while (i < strlen(buf))

	{

		if (buf[i] != '\"')

		{

			buf2[j]=buf[i];

			j++;

		}

		i++;

	}

	while (buf2[strlen(buf2)-1] != ' ')

		buf2[strlen(buf2)-1] = '\0';

	spawnl(P_NOWAITO,buf2,buf2,csLocation.c_str(),NULL);

}


//Dde call back function.

HDDEDATA CALLBACK LocationDdeCallBack(UINT uType, UINT fmt,
	HCONV hconv, HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD dwData1,
	DWORD dwData2)	{

    switch (uType) {
        case XTYP_REGISTER:

        case XTYP_UNREGISTER:

            return (HDDEDATA) NULL;



        case XTYP_ADVREQ:

            return (HDDEDATA) NULL;

        case XTYP_ADVSTART:
            return (HDDEDATA) NULL;

        case XTYP_ADVSTOP:
            return (HDDEDATA) NULL;

        case XTYP_CONNECT:
            return (HDDEDATA)TRUE;

        case XTYP_CONNECT_CONFIRM:
            return (HDDEDATA) TRUE;

        case XTYP_ERROR:
            return (HDDEDATA) NULL;

        case XTYP_POKE:
            return (HDDEDATA) NULL;

        case XTYP_REQUEST:
            return (HDDEDATA) NULL;

        case XTYP_WILDCONNECT:
            return (HDDEDATA) NULL;



        case XTYP_ADVDATA:

            return (HDDEDATA) DDE_FACK;



        case XTYP_XACT_COMPLETE:

            return (HDDEDATA) NULL;



        case XTYP_DISCONNECT:

            return (HDDEDATA) TRUE;



        case XTYP_EXECUTE:

        		char szExec[LINELENGTH];

            char commando[LINELENGTH];

            DdeGetData(hData, (LPBYTE)szExec, LINELENGTH, 0);

            szExec[LINELENGTH - 1] = '\0';

		      ChangeSlashes(szExec);



				DdeQueryString(dwidInst,hsz1,commando,LINELENGTH,CP_WINANSI);

            _chandler->DoCommand("%s %s",commando,szExec);

				_chandler->DoCommand("draw");

/*        		if (DdeCmpStringHandles(hsz1,hszLoadFile)==0)

	            _chandler->DoCommand("%s %s",szExec);

   	  		else if (DdeCmpStringHandles(hsz1,hszLoadProcess)==0)

	            _chandler->DoCommand("%s %s",szExec);*/

        		return (HDDEDATA) TRUE;



        default:

            return (HDDEDATA) NULL;

    }

}

void ChangeSlashes(char* buf)
{
   for (int i = 0; buf[i]!='\0'; i++)
   {
   	if (buf[i]=='\\')
      	buf[i] = '/';
	}
}

#else
#endif

void DDEHandler::startLoadingUnix(const wxString& csLocation)
{
	wxString locatie;
   locatie.Clear();
   locatie+="netscape -remote ";
   locatie+= "openURL(";
   locatie+=csLocation;
   locatie+=")'";

   wxExecute(locatie,false,NULL);
}

