/*
Program	ERROR.CPP
Purpose	Holds an error message
Programmers	R. Spekreijse & N. Noorlander
Last Update	21-09-1995
*/

#ifdef __GNUG__
#pragma implementation
#endif
#include "gdserr.h"
#include "misc.h"

GDS_Error::GDS_Error(char* message, char* header, int degree, int fatal)
{
	if (message)
   {
		_message = new char[strlen(message)+1];
		strcpy(_message, message);
   }
	else
   {
		_message = new char[strlen("non specified")+1];
		strcpy(_message,"non specified");
   }

	if (header)
   {
		_header  = new char[strlen(header)+1];
		strcpy(_header, header);
   }
	else
   {
		_header  = new char[strlen("non specified")+1];
		strcpy(_header,"non specified");
   }
	_degree = degree;
	_fatal = fatal;

}

GDS_Error::GDS_Error(const GDS_Error& a)
{
	if (a._message)
	{
		_message = new char[ strlen(a._message)+1 ];
		strcpy( _message, a._message );
	}
	else
	{
		_message = new char[ strlen("non specified")+1 ];
		strcpy( _message, "non specified" );
	}

	if (a._header)
	{
		_header = new char[ strlen(a._header)+1 ];
		strcpy( _header, a._header );
	}
	else
	{
		_header = new char[ strlen("non specified")+1 ];
		strcpy( _header, "non specified" );
	}

	_degree = a._degree;
	_fatal = a._fatal;

}

GDS_Error::~GDS_Error()
{
	strcpy(_message,"");
	strcpy(_header,"");
	delete _message;
	delete _header;
}

char* GDS_Error::GetErrorMessage()
{
	return _message;
}

char* GDS_Error::GetHeaderMessage()
{
	return _header;
}

int GDS_Error::GetErrorDegree()
{
	return _degree;
}

int GDS_Error::GetFatal()
{
	return _fatal;
}


