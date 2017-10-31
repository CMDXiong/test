/*
Program	ERROR.H
Purpose	Holds an error message
Last Update	29-09-1996
*/

#ifndef GDS_ERROR_H
#define GDS_ERROR_H

#ifdef __GNUG__
#pragma interface
#endif

#include <wx/string.h>

class GDS_Error
{
	public:
		GDS_Error(char* message, char* header=0, int degree = 9, int fatal = 0);
		GDS_Error(const GDS_Error& a);
		~GDS_Error();
		char*	   GetErrorMessage();
		char*  	GetHeaderMessage();
		int		GetErrorDegree();
		int		GetFatal();

	protected:
		char*	_message;
		char*	_header;
		int 	_degree;
		int 	_fatal;
};

#endif
