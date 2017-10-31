//! author="klaas"
//! version="1.0"
/*
Program			PARSER.H
Purpose			General parser with enviroment replacements
					See comment in CPP file for further details
Programmer		R. Spekreijse
Last Update		20-12-1995
*/

#ifndef CPARSER_H
#define CPARSER_H

#ifdef __GNUG__
#pragma interface
#endif


#include "wx/string.h"
#ifdef __UNIX__
#include <sstream>
#include <fstream>
#include <iostream>
#else
#include <fstream>
#include <iostream>
#include <sstream>
#endif
#include <stdio.h>
#include <stdlib.h>

#include "b_dl_itr.h"

// this is for the added alias features
#include "alias.h"

//!base class used to parse command
/*! strings according to the BNF below.
\sa Evalstring
\sa Line_Parser
\verbatim

Below you can see the Bachus Naur representation of the enhanced parser:

<commands> 			::= [<blanks>]
                      {	   	{ <command> }*
                      }

<command> 			::= [<blanks>] {<comment> | {<word> {<blanks> | <endcommand}}* }*

<endcommand>   	::=  {';' | '\n' | '\0' }

<word>				::=  { <quotedstring> | <bracedstring> | <multipartword> }

<quotedstring>		::= ('"') {CHAR | '\"' | '\\n' | aliasref | envref }* ('"')

<bracedstring>		::= ('{') {CHAR | '\{' | '\}' | '\\n'}* ('}')

<multipartword>	::=  { <aliasref> | <envref> | {CHAR | '\\n'}* }*

<aliasref>			::= ('%') { <name> <blank> | ('{') <name> ('}') }

<envref>				::= ('$') { <name> <blank> | ('{') <name> ('}') }

<name>				::= { LETTER | DIGIT | UNDERSCORE | MINUS }*

<blanks>			::= { TAB | SPACE | '\\n'}*

<comment>		::= # {CHAR}* ('\0')

note:
	[ ]	entity CAN occur zero or more times
	( )	entity MUST occur
	{ }	pick one of the entities within the braces
	{ }*	entities within the braces CAN occur zero ore more times
	{ }+	entities within the braces MUST occur one ore more times
	 |		OR
\endverbatim
*/
class CommandParser
{
	public:
		CommandParser();
		virtual ~CommandParser();

      //!read one command from the buffer
		bool		 GetCommand();
      //!flush command buffer
		bool		FlushArg();

      //!get the error message if parsing went wrong
		wxString    GetErrorMes();

      //!get environment variable
		bool 		GetEnv(const wxString& envname, wxString& envstring);

      //!list of command arguments found
		DL_List<const wxString*>* GetArgList();
      //!iterator to traverse the list of command arguments found
		DL_Iter<const wxString*>*  GetArgIter();

      //!get the parses command
		wxString		GetCommandParsed();

	protected:
		bool		Blanks();
      bool      Word();
		bool		MultiPartWord();
		bool		QuotedString();
		bool		BracedString();
		bool		Comment();
		bool	   VarRef(char type);
		bool		Name();

      //to make derived classes work
		virtual char PeekNextC()=0;
      virtual void IncC()=0;

	protected:
		char					a;
		AliasList* 			_aliaslist;
      DL_List<const wxString*>*   _arglist;
      DL_Iter<const wxString*>*   _I;
		wxString				_commandsofar;

   private:
		wxString				_error_mes;
      wxString 			_b;
      wxString				_varref;

};


//!used to parse command strings and evaluate the aliases in it.
/*!
 At the same time the parameters are stores into a list
\sa Line_Parser
\sa CommandParser
*/
class Evalstring : public CommandParser, public std::ostringstream
{
	public:
		Evalstring( wxString& tostring, AliasList* aliaslist);
		~Evalstring();

		bool 	Parse(wxString& tostring);
		const wxString*		GetValue(int i);
		bool 	GetNextCommand();


	protected:

 		void 		IncC();
		char 		PeekNextC();

  private:
      int  		_linenumber;
      char*		_commands;
      char* 	_c;

};

#endif

