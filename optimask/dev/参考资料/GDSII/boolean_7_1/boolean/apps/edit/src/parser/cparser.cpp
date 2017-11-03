/*
 *	Program			PARSER.CPP
 *	Purpose			General command parser
 *						See comments and BNF below for further details
 */

#ifdef __GNUG__
#pragma implementation
#endif


#include "cparser.h"
// our error class
#include "gdserr.h"
#include "misc.h"

// constructor
CommandParser::CommandParser()
{
   _arglist 		= new DL_List<const wxString*>();
   _I			 		= new DL_Iter<const wxString*>(_arglist);
	_aliaslist     = 0;
	_error_mes.Clear();
   _error_mes.Alloc(LINELENGTH);
   _commandsofar.Clear();
   _commandsofar.Alloc(LINELENGTH);
   _b.Clear();
   _b.Alloc(LINELENGTH);
   _varref.Clear();
   _varref.Alloc(LINELENGTH);
}

CommandParser::~CommandParser()
{
   FlushArg();
   delete _I;
   delete _arglist;
}

wxString CommandParser::GetCommandParsed()
{
    return _commandsofar;
}

DL_List<const wxString*>* CommandParser::GetArgList()
{return _arglist;
};

DL_Iter<const wxString*>* CommandParser::GetArgIter()
{return _I;
};

bool CommandParser::FlushArg()
{
   _commandsofar.Empty();
   _I->tohead();
   while (!_I->hitroot())
   {  const wxString* item= _I->item();
      delete item;
      _I->remove();
   }
   return true;
}

bool CommandParser::GetCommand()
{
	_error_mes.Empty();
   _commandsofar.Empty();

   IncC(); //get the first character;
   while(a == '\n') //skip empty command
   {
	  _commandsofar.Empty();
     IncC();
   }

	Blanks();

   if (a=='\0') //end of file no error will be set
      return false;

   else if (a == '#')
      Comment();    	//it is comment
   else
   {
   	while (Word())	  	//is there a command (being several words)
      {
         bool found=Blanks();
        	if (!found && !( a=='\0' || a=='\n' || a==';')) //whitespace needed
         {
            _error_mes=" whitespace expected or end command missing";
            return false;
         }
	      if( a=='\0' || a=='\n' || a==';') //end of _command ?
            break;
      }

      if ( !_error_mes.IsEmpty()  )
	      return false;

		// <endcommand>   ::= '\0' | ';'
      if( a!='\0' && a!='\n' && a!=';') //not at end of _command
      {
         _error_mes="; or EOL expected";
         return false;
      }
   }
   //if last character is \n skip it here
   if  (a=='\n')
	   _commandsofar.RemoveLast();
	return true;
}

// <word> ::=  { <quotedstring> | <bracedstring> | <multipartword> }
bool CommandParser::Word()
{
	Blanks();
   _b.Empty();

   if (a == '"')
   {
      if (!QuotedString())
   		return false;
   }
   else if (a == '{')
   {
      if (!BracedString())
   		return false;
   }
   else
   {
   	if ( !MultiPartWord())
	   	return false;
   }

   if (!_b.IsEmpty())
   {
      wxString* buf=new wxString(_b);
	   _I->insend(buf);
   }
  	return true;
}


//<multipartword>	::=  { <aliasref> | <envref> | {CHAR}* }*
bool CommandParser::MultiPartWord()
{
	while( a!=' ' && a!='\t' && a!= '\0' && a!= '\n' && a!=';')
	{
      if (a== '\\') //take the next character
      { 	IncC();
         if ( a == '\n')
            IncC(); //treat it as if it was a space

         if ( a==' ' || a=='\t' || a== '\0' || a==';')
            break; //treat it as word seperator

         //all other backslash character take them
			_b +=a;
         IncC();
         continue;
      }

      if ( (a== '$') || (a== '@') )
      {
         if (!VarRef(a))
         	return false;
         continue;
      }

      //all other characters
		_b+=a;
		IncC();
	}
	return true;
}


//	<comment>		::= # {CHAR}*
bool CommandParser::Comment()
{
	if( a!= '#' ) //quote must be there
   {
		_error_mes= _T("wrong comment");
		return false;
   }
	_b.Empty();

   _b+=a;
   IncC();

	while(a!= '\0')
	{
      if (a== '\\')
      {
         IncC();
         if (a == '\n')
         {
				_b+=' ';
            IncC();
            continue;
         }
      }
      if (a == '\n')
         break; //end of comment

		_b+=a;
		IncC();
	}

   wxString* buf=new wxString(_b);
   _I->insend(buf);
	return true;
}


// strips all the spaces and tabs from the left of the string
//	<blanks>			::= { TAB | SPACE }*
bool CommandParser::Blanks()
{
   int i=0;
	// delete blanks at the beginning
	while ( a ==' ' || a =='\t' || a =='\\' || a ==0xd  )
   {  if ( a == '\\')
      {
         if (PeekNextC()!= '\n')
             break;
         else
				IncC(); i++;
      }
		IncC(); i++;
   }

	return (i != 0);
}


wxString	CommandParser::GetErrorMes()
{
	return _error_mes;
}

//	<name>			::= { LETTER | DIGIT | UNDERSCORE | MINUS }*
// Check Name specification
bool CommandParser::Name()
{
   _varref.Empty();
	if ( !( isalnum(a) || a=='_' ||  a=='-') )
	{
		_error_mes=_T("wrong name");
		return false;	// invalid name (begin char)
	}
   _varref+=a;
	IncC();
   while(isalnum(a) || a == '_')
   {
     _varref+=a;
     IncC();
   }
	return true;	// name is validated
}


// Get Alias value
// <aliasref>			::= {('@')} { <name> <blank> | ('{') <name> ('}') }
// Get Environment string
//	<envstring>		::= ('$') { <name> <blank> | ('{') <name> ('}') }
bool CommandParser::VarRef(char type)
{
	if( a != type ) // '$' or '@' must be there
   {
		_error_mes="wrong environment $ or @ missing";
		return false;
   }

	IncC();
	if( a == '{' ) // brace could be there
	{
		IncC();
		if( !Name() )
			return false; // error, wrong alias name
		if( a != '}' ) // brace must be there
		{
			_error_mes="wrong environment name, end brace missing";
			return false; // error, wrong alias, endbrace missing or wrong name
		}
		IncC();
	}
	else
	{
		if( !Name() )
			return false; // error, wrong alias name
	}

   wxString evaluated;
   if (type == '$')
   {
      if( !GetEnv(_varref, evaluated) )
      {
         _error_mes="wrong environment name, environment does not exist";
         return false;// error, alias does not exist
      }
   }
   else
   {
      if( !_aliaslist->GetAlias(_varref,evaluated) )
      {
         _error_mes= "wrong environment name, environment does not exist";
         return false;// error, alias does not exist
      }
   }

   _b+=evaluated;
	return true;
}

//<quotedstring>		::= ('"') {CHAR | '\"' | '\\n' | aliasref | envref }* ('"')
bool CommandParser::QuotedString()
{
	if( a != '"' ) //quote must be there
	{
		_error_mes="wrong quotedstring, begin quote missing";
		return false;
	}
	IncC();

	while( a !='"' && a != '\0' )
	{
      if (a == '\\') //take the next character
      { 	IncC();
         if (a == '\0')
            break; //error handled later

			_b+=a;
         IncC();
         continue;
      }

      if ( (a == '$') || (a == '@'))
      {
         if (!VarRef(a))
         	return false;
         continue;
      }

      //all other characters
		_b+=a;
		IncC();
	}

	if( a != '"' ) //quote must be there
	{
		_error_mes="wrong quotedstring, end quote missing";
		return false;
	}
	IncC();

	return true;
}


//<bracedstring>		::= ('{') {CHAR | '\{' | '\}' | '\\n'}* ('}')
bool CommandParser::BracedString()
{
	if( a != '{' ) //quote must be there
	{
		_error_mes= "wrong bracedstring, begin brace missing";
		return false;
	}

	IncC();

   int brnr=0;
	while( (a !='}' || brnr) && a != '\0' )
	{
      if (a == '\\') //take the next character
      { 	IncC();
         if (a == '\0')
            break; //error handled later

			_b+=a;
         IncC();
         continue;
      }

      //nested braces are allowed
      if (a == '{') //nested brace
         brnr++;

      if (a == '}') //nested brace
         brnr--;

      //all other characters
		_b+=a;
		IncC();
	}

	if( a != '}' ) //quote must be there
	{
		_error_mes="wrong bracedstring, end brace missing";
		return false;
	}
	IncC();

	return true;
}

// Set of Get functions for handling Environment Variables
bool CommandParser::GetEnv(const wxString& envname, wxString& envstring)
{
	if (envname.Len()==0)
		return false;	// A name is needed

	envstring = getenv(envname);

	if( !envstring)
		return false;	// Environment name not found

	return true;
}

Evalstring::Evalstring(wxString& tostring, AliasList* aliaslist)
{
	_aliaslist = aliaslist;
   _linenumber=0;
	_commands = (char*) tostring.c_str();
   _c=0;
   a = ' ';
}

// frees all the dynamic data
Evalstring::~Evalstring()
{
}

void Evalstring::IncC()
{
   if (_c == 0) //get first character from buffer
   {
      _linenumber++;
      _c	= _commands;
   }
   else
   {  if (*_c!='\0')
	   	_c++;
   }
   a=*_c;
}

char Evalstring::PeekNextC()
{
   char p='\0';
   if (_c == 0) //get first character from buffer
       p=*_commands;
   else
   {  if (*_c!='\0')
      {
	   	_c++;
		   p=*_c--;
      }
   }
   return p;
}

bool Evalstring::Parse( wxString& tostring )
{
   FlushArg();
	try
	{
		while(1)
      {
			if( !GetCommand() )
			{
         	if (strlen(GetErrorMes()) > 0 )
            {
               wxString buffer;
               buffer.Printf(buffer,"Could not Parse line %d: \n %s \n Error: %s",
                     _linenumber,_commands
                     , GetErrorMes().c_str() );
               throw GDS_Error((char*)buffer.c_str(), "Value Parsing Error",9,1);
            }
            break;
			}

         //copy it to the string as words seperated by spaced and as endcommand \n
         if (tostring)
         {
            tostring.Clear();
            if (_I->count())
            {
               _I->tohead();
               tostring+=*_I->item();
               (*_I)++;
               while (!_I->hitroot())
               {
                  tostring+= " " + *_I->item();
                  (*_I)++;
               }
            }
         }
		}// end of while loop
	}
	catch (GDS_Error& _error) // error from the parser: process here
	{
		_messagehandler->info(_error.GetErrorMessage(),
									  "DoCommand::LoadProcess(): Fatal Error");
		return false;
	}

	return true;
}

const wxString* Evalstring::GetValue(int i)
{
   int j=0;
   _I->tohead();
   while(!_I->hitroot())
   {
      if (j==i)
      {
		   _I->Detach();
      	return _I->item();
      }
      j++;
      (*_I)++;
   }
   static const wxString non(" ");
   return &non;
}

bool Evalstring::GetNextCommand()
{
   wxString buf;
   FlushArg();
   if( !GetCommand() )
   {
      if (strlen(GetErrorMes()) > 0 )
      {
         buf.Printf("Could not Parse line %d: \n Error: %s",
               _linenumber,GetErrorMes().c_str() );
         throw GDS_Error((char*)buf.c_str(), "Value Parsing Error",9,0);
      }
      return false;
   }
   return true;
}

