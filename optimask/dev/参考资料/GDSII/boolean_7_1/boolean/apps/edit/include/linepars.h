//! author="klaas"
//! version="1.0"
/*
Program			LINE_PARS.H
Purpose			Reads lines
*/

#ifndef Line_Parser_H
#define Line_Parser_H

#ifdef __GNUG__
#pragma interface
#endif

#include <ctype.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "cparser.h"

//!used to parse command strings comming from files
/*!
\sa Evalstring
\sa CommandParser
*/
class Line_Parser : public CommandParser , public std::ifstream
{
	public:
		//! constructor
		Line_Parser(AliasList* aliaslist,char* the_file);
		//! destructor
		~Line_Parser();
      //!read a command, if not the required one, it is stored
		bool 	 ReadItem(char* type);
      //!current line number
		int 		 GetLineNumber(){return _linenumber;};
      //!get the n'th argument
		const wxString* GetValue(int i);

      //!get opened file
		char*		 GetFileName(){return _line_file;}

      //!start from beginning of the file
		void 		 Reset();

      //!get the next command
		bool	 GetNextCommand();

	protected:
      //!reading characters from the file
 		void IncC();
      //!peek at next character
		char PeekNextC();

      //!current line number
      int  	    _linenumber;
      //! holds the filename from the layer file
		char*	    _line_file;
      //!last read command
		const wxString*	 _keyword;
      //!is the last command stored
		bool 	 _back;
};
#endif

