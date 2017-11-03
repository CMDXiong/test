/*
Program			LAYER_PARS.CPP
Purpose			Reads a layer def file and initializes the layer info
					See CPARSER.CPP for BNF of parsed file
Programmer	   Michaël Kamp
Last Update		3-11-1998
*/

#ifdef __GNUG__
#pragma implementation 
#endif

#include "linepars.h"
#include "misc.h"
using namespace std;

// contructor
Line_Parser::Line_Parser(AliasList* aliaslist,char* the_file)
{

	_line_file = new char[ strlen(the_file)+1 ];
	strcpy( _line_file, the_file );
	_aliaslist = aliaslist;

	// try to open the file
	open(_line_file);
	// if opening fails throw an error class
	if (bad())
	{
		char errbuf[100];
		sprintf(errbuf, "Cannot open file %s!", _line_file);
		throw GDS_Error(errbuf, "Parser: File Error", 9, 0);
	}

   _linenumber=0;
	_keyword = 0;
	_back=false;
   a = ' ';
}

// destructor
Line_Parser::~Line_Parser()
{
   delete [] _line_file;
   close();
}

// reset
void Line_Parser::Reset()
{
   close();
	open(_line_file);
	// if opening fails throw an error class
	if (bad())
	{
		char errbuf[100];
		sprintf(errbuf, "Cannot open file %s!", _line_file);
		throw GDS_Error(errbuf, "Parser: File Error", 9, 0);
	}
   _linenumber=0;
	_keyword = 0;
	_back=false;
   a = ' ';
   _commandsofar.Clear();
}

void Line_Parser::IncC()
{
   a = (char) get();
   if (eof())
       a='\0';
   if (a==EOF)
       a='\0';
	if (a=='\r')
   	a=' ';

   _commandsofar+=a;
}

char Line_Parser::PeekNextC()
{
   char p = (char) peek();
   return p;
}


// ReadItem (parser) --------------------------------------------------------
bool Line_Parser::ReadItem(char* type)
{
//	<item>		::=	KEYWORD VALUE
	if (_back)
	{
		if (!_keyword->CmpNoCase(type))
			_back = false;
		else
			_back = true;
		return bool(!_back);
	}

   bool Next=true;
   do
   {
	   FlushArg();
      if( !GetCommand() )
      {
         if (strlen(GetErrorMes()) > 0 )
         {
            sprintf((char*)_commandsofar.c_str(),"Could not Parse line %d: \n Error: %s",
                  _linenumber,GetErrorMes().c_str() );
            throw GDS_Error((char*)_commandsofar.c_str(), "Value Parsing Error",9,0);
         }
         return false;
      }
      _I->tohead();
      if (!_I->count())
         Next=true;
      else if (  *(_I->item()) == '#' ) //it is comment read next are empty line
         Next=true;
      else
         Next=false;
   }
   while (Next);

   _linenumber++;
   _keyword=_I->item();

   if (!_keyword->CmpNoCase(type))
      _back = false;
   else
      _back = true;
   return bool(!_back);
}

bool Line_Parser::GetNextCommand()
{
   FlushArg();
   if( !GetCommand() )
   {
      if (strlen(GetErrorMes()) > 0 )
      {
         sprintf((char*)_commandsofar.c_str(),"Could not Parse line %d: \n Error: %s",
               _linenumber,GetErrorMes().c_str() );
         throw GDS_Error((char*)_commandsofar.c_str(), "Value Parsing Error",9,0);
      }
      return false;
   }
   return true;
}

const wxString* Line_Parser::GetValue(int i)
{
   int j=0;
   _I->tohead();
   while(!_I->hitroot())
   {
      if (j==i)
      	return _I->item();
      j++;
      _I++;
   }
   static const wxString non(" ");
   return &non;
}

