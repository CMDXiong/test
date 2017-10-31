/*

Program	ALIAS.H
Purpose	Alias definition
Programmer	Raymond Heukers
Last Update	02-05-1997
*/

#ifndef PROP_H
#define PROP_H

#ifdef __GNUG__
#pragma interface
#endif

#include <string.h>
#include <stdlib.h>

#include "b_lnk_itr.h"

class Property
{
	public:
		// Constructor / Destructor
		Property();
		Property(char* property,char* value,char* type);
		~Property();

		//	Memberfunctions
		char* 		 GetName();
		char* 		 GetValue();
		char* 		 GetType();
		void 			 SetName(char*);
		void 			 SetValue(char*);
		void 			 SetType(char*);

		Property&	 operator=(Property&);

	protected:
		char*						_property;
		char*						_value;
		char*						_type;
};


class PropertyList:	public DL_List<void*>
{
	public:
				PropertyList();
				~PropertyList();
   bool 	GetProperty(char* property, char** value,char** type);
   bool 	SetProperty(char* property, char* value,char* type);
	bool   DeleteProperty(char* Propertyname);
				PropertyList& 		 operator=(PropertyList&);
};

//#include <wx/list.h>
//#include "prop.h"
//WX_DECLARE_LIST(wxPropertyValue,wxPropertyValues);

//#include <wx/listimpl.cpp>
//WX_DEFINE_LIST(wxPropertyValues);

#endif
