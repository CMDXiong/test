/*
Program	ALIAS.CPP
Purpose	Alias definition
Programmer	Raymond Heukers
Last Update 	02-05-1997
*/

#ifdef __GNUG__
#pragma implementation "alias.h"
#endif

#include "gdserr.h"
#include "alias.h"

// constructor
Alias::Alias()
{
}


// constructor
// usage: Text a_alias = new Alias("extentie","cal");
Alias::Alias(const wxString& name,const wxString& value)
{
   _alias=name;
   _value=value;
}


// destructor
Alias::~Alias()
{
}


// returns a pointer to the alias
// usage : printf("%s",a_alias->GetAlias()->c_str());
const wxString&   Alias::GetAlias()
{
	return _alias;
}


// returns a pointer to the value
// usage : printf("%s",a_alias->GetValue()->c_str());
const wxString&   Alias::GetValue()
{
	return _value;
}


// set Alias name
// usage: a_alias.SetAlias("alias");
//	!! Using alias = 0, creates an empty alias string
void Alias::SetAlias(const wxString&  alias)
{
	_alias = alias;
}


// set Value name
// usage: a_alias.SetValue("value");
//	!! Using value = 0, creates an empty value string
void Alias::SetValue(const wxString& value)
{
	_value = value;
}


// makes an alias same as this
// usage : Alias1 = Alias2;
Alias* Alias::operator=(Alias* a_alias)
{
	_alias = a_alias->_alias;
	_value = a_alias->_value;

	return this;
}

AliasList::AliasList()
{
}


// destructor
// deletes all object of the linklist
AliasList::~AliasList()
{
   {
      TDLI<Alias> _LI=TDLI<Alias>(this);

      //first empty the list
      _LI.delete_all();
   }
}

bool AliasList::GetAlias(const wxString& aliasname,wxString& aliasstring)
{
	if (aliasname.Len()==0)
		return false;	// I don't like NULL-pointers and A name is needed

  	TDLI<Alias> _aliasiter=TDLI<Alias>(this);
	_aliasiter.tohead();
	while (!_aliasiter.hitroot())
	{
		if (_aliasiter.item()->GetAlias().CmpNoCase(aliasname) == 0)
		{
			aliasstring=_aliasiter.item()->GetValue();
			return true;
		}
		_aliasiter++;
	}
   aliasstring.Clear();
	return false;
}

bool AliasList::SetAlias(const wxString& aliasname,const wxString& aliasstring)
{
	if (strlen(aliasname)<=0)
		return false;

	Alias* _alias_ptr;
  	TDLI<Alias> _aliasiter=TDLI<Alias>(this);
	_aliasiter.tohead();
	while (!_aliasiter.hitroot())
	{
		if (strcmp(_aliasiter.item()->GetAlias(),aliasname) == 0)
		{
			delete _aliasiter.item();
			_aliasiter.remove();
			break;
		}
		_aliasiter++;
	}
	_alias_ptr = new Alias(aliasname,aliasstring);
	_aliasiter.insbefore(_alias_ptr);

	return true;	// Success
}

bool AliasList::GetAliasNr(int i, wxString& aliasname,wxString& aliasstring)
{
	if (i<1 || i>this->count())
		return false;

  	TDLI<Alias> _aliasiter=TDLI<Alias>(this);
	_aliasiter.tohead();
	for (int j=1; j<i; j++)
		_aliasiter++;

	aliasstring=_aliasiter.item()->GetValue();
	aliasname=_aliasiter.item()->GetAlias();

	return true;
}
