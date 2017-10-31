/*
Program	ALIAS.H
Purpose	Alias definition
Programmer	Raymond Heukers
Last Update	02-05-1997
*/

#ifndef ALIAS_H
#define ALIAS_H

#ifdef __GNUG__
    #pragma interface "alias.h"
#endif

#include <string.h>
#include <stdlib.h>

#include "b_lnk_itr.h"
#include "wx/string.h"

class Alias
{
	public:
		// Constructor / Destructor
		Alias();
		Alias(const wxString& name,const wxString& value);
		~Alias();

		//	Memberfunctions
		const wxString&  GetAlias();
		const wxString&  GetValue();
		void 			 SetAlias(const wxString& name);
		void 			 SetValue(const wxString& name);

		Alias* 		 operator=(Alias*);

	protected:
		wxString  	 _alias;
		wxString 	 _value;
};


class AliasList:	public DL_List<void*>
{
	public:
				AliasList();
				~AliasList();
	bool 		 GetAliasNr(int i,wxString& name,wxString& value);
   bool 	GetAlias(const wxString& name,wxString& value);
   bool 	SetAlias(const wxString& name,const wxString& value);
};
#endif

