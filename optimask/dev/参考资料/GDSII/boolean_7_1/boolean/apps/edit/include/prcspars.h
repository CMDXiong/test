//! author="klaas"
//! version="1.0"
/*
Program			PRCSPARS.H
Purpose			Reads a processfile and executes it (header)
					A processfile can be written by yourself or recorded by the application
					This file contains commands which will be processed sequentially
Programmers		R. Spekreijse & N. Noorlander
Last Update		21-12-1995
*/

#ifndef PROCESSPARSER_H
#define PROCESSPARSER_H

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/string.h"

#include "misc.h"

#include "gdserr.h"
#include "gdsmes.h"

#include "alias.h"
#include "cparser.h"
#include "linepars.h"

//! used to parse command strings comming from files
//! \sa CommandHandler
class ProcessParser
{
	public:
		//! constructor
		ProcessParser(char*,AliasList* aliaslist);
		//! destructor
		~ProcessParser();

		//!load and starts the processing from the processfile
		bool	LoadProcess();

	protected:
      //!to read and parse lines from the process file
		Line_Parser*		_lineparser;
};

#endif

