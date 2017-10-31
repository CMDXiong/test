/*
Program			PRCSPARS.CPP
Purpose			Reads a processfile and executes it (source)
					A processfile can be written by yourself or recorded by the application
					This file contains commands which will be processed sequentially
					See PARSER.CPP for BNF of parsed file
Last Update		21-12-1995
*/
#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/string.h"

#include "prcspars.h"
#include "chandbas.h"
#include "mainwin.h"
#include "debugdlg.h"
#include "linepars.h"
#include "misc.h"

// contructor
ProcessParser::ProcessParser(char* processfile,AliasList* aliaslist)
{
   _lineparser = new Line_Parser(aliaslist,processfile);
}

// destructor
ProcessParser::~ProcessParser()
{
   delete _lineparser;
}

// load a processfile and process it
bool ProcessParser::LoadProcess()
{
	try
	{
      bool stop=false;
		while(_lineparser->GetNextCommand()&& !stop)
//		while(_lineparser->GetNextCommand())
      {
#ifdef __UNIX__
         if (GLOB->MAINWINDOW->_stopDialog->IsShown())
         {
            wxSafeYield(GLOB->MAINWINDOW->_stopDialog);
            stop=   !GLOB->CMDH->GetContinue();
		   }
#else
         if (GLOB->MAINWINDOW->_stopDialog->IsShown() && wxCheckForInterrupt((wxWindow*)GLOB->MAINWINDOW->_stopDialog))
         {
            wxSafeYield(GLOB->MAINWINDOW->_stopDialog);
            stop=   !GLOB->CMDH->GetContinue();
		   }
#endif

         if (_lineparser->GetArgList()->count())
         {
            if (_lineparser->GetArgList()->headitem()->CmpNoCase("record") != 0)
            {  //we are reading from a process file so ignore record commands
               if (!GLOB->CMDH->DoCommand("%s",_lineparser->GetCommandParsed().c_str()))
               {
                  wxString help;
                  help.Printf("Could not execute command at line: %d >> %s\n",_lineparser->GetLineNumber(),(char*)_lineparser->GetCommandParsed().c_str());
                  throw GDS_Error((char*)help.c_str(), "Command Error",9,1);
               }
            }
			}

		   _lineparser->FlushArg();

		}// end of while loop
	}
	catch (GDS_Error& _error) // error from the parser: process here
	{
		_messagehandler->info(_error.GetErrorMessage(),
									  "ProcessParser::LoadProcess(): Fatal Error");
		return false;
	}

	return true;
}




