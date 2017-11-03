/*

Program	Property.CPP
Purpose	Property definition
Programmer	Raymond Heukers
Last Update 	02-05-1997
*/

#ifdef __GNUG__
#pragma implementation 
#endif

#include "gdserr.h"
#include "proper.h"
#include <assert.h>

// constructor
Property::Property()
{
	_property = 0;
	_value = 0;
   _type=0;
}


// constructor
// usage: Text a_Property = new Property("extentie","cal");
Property::Property(char* property, char* value,char* type)
{
	_property = strdup(property);
	if (!_property)
		throw GDS_Error("Out of Memory./n/nCould not create Object Property._property",
						"Fatal Creation Error", 0, 1);
	_value = strdup(value);
	if (!_value)
		throw GDS_Error("Out of Memory./n/nCould not create Object Property._value",
						"Fatal Creation Error", 0, 1);
	_type = strdup(type);
	if (!_type)
		throw GDS_Error("Out of Memory./n/nCould not create Object Property._type",
						"Fatal Creation Error", 0, 1);
}


// destructor
Property::~Property()
{
	if (_property)
		delete [] _property;
	if (_value)
		delete [] _value;
	if (_type)
		delete [] _type;
}


// returns a pointer to the Property
// usage : printf("%s",a_Property->GetProperty());
char* Property::GetName()
{
	return _property;
}


// returns a pointer to the value
// usage : printf("%s",a_Property->GetValue());
char* Property::GetValue()
{
	return _value;
}

// returns a pointer to the value
// usage : printf("%s",a_Property->GetType());
char* Property::GetType()
{
	return _type;
}


// set Property name
// usage: a_Property.SetName("Property");
//	!! Using Property = 0, creates an empty Property string
void Property::SetName(char* Property)
{
	if (_property)
		delete [] _property;
	_property = strdup(Property);
	if (!_property)
		throw GDS_Error("Out of Memory./n/nCould not create Object Property._Property",
						"Fatal Creation Error", 0, 1);
}


// set Value
// usage: a_Property.SetValue("value");
//	!! Using value = 0, creates an empty value string
void Property::SetValue(char* value)
{
	if (_value)
		delete [] _value;
	_value = strdup(value);
	if (!_value)
		throw GDS_Error("Out of Memory./n/nCould not create Object Property._value",
						"Fatal Creation Error", 0, 1);
}

// set Type
// usage: a_Property.SetType("value");
//	!! Using value = 0, creates an empty value string
void Property::SetType(char* type)
{
	if (_type)
		delete [] _type;
	_type = strdup(type);
	if (!_type)
		throw GDS_Error("Out of Memory./n/nCould not create Object Property._type",
						"Fatal Creation Error", 0, 1);
}


// makes an Property same as this
// usage : Property1 = Property2;
Property& Property::operator=(Property& a_Property)
{
	if (_property)
		delete [] _property;
	_property = strdup(a_Property._property);
	if (_value)
		delete [] _value;
	_value = strdup(a_Property._value);
	if (_type)
		delete [] _type;
	_type = strdup(a_Property._type);

	return *this;
}

PropertyList::PropertyList()
{
}


// destructor
// deletes all object of the linklist
PropertyList::~PropertyList()
{
   {
      TDLI<Property> _LI=TDLI<Property>(this);

      //first empty the list
      _LI.delete_all();
   }
}

bool PropertyList::GetProperty(char* Propertyname, char** value,char** type)
{
	if (Propertyname==0 || strlen(Propertyname)==0)
		return false;	// I don't like NULL-pointers and A name is needed

  	TDLI<Property> _Propertyiter=TDLI<Property>(this);
	_Propertyiter.tohead();
	while (!_Propertyiter.hitroot())
	{
		if (strcmp(_Propertyiter.item()->GetName(),Propertyname) == 0)
		{
			*value=_Propertyiter.item()->GetValue();
			*type=_Propertyiter.item()->GetType();
			return true;
		}
		_Propertyiter++;
	}
	return false;
}

bool PropertyList::SetProperty(char* Propertyname,  char* value,char* type)
{
	if (Propertyname==0 || strlen(Propertyname)==0)
		return false;	// I don't like NULL-pointers and A name is needed

	Property* _Property_ptr;
  	TDLI<Property> _Propertyiter=TDLI<Property>(this);
	_Propertyiter.tohead();
	while (!_Propertyiter.hitroot())
	{
		if (strcmp(_Propertyiter.item()->GetName(),Propertyname) == 0)
		{
			delete _Propertyiter.item();
			_Propertyiter.remove();
			break;
		}
		_Propertyiter++;
	}
	_Property_ptr = new Property(Propertyname,value,type);
	_Propertyiter.insbefore(_Property_ptr);

	return true;	// Success
}

bool PropertyList::DeleteProperty(char* Propertyname)
{
	if (Propertyname==0 || strlen(Propertyname)==0)
		return false;	// I don't like NULL-pointers and A name is needed

  	TDLI<Property> _Propertyiter=TDLI<Property>(this);
	_Propertyiter.tohead();
	while (!_Propertyiter.hitroot())
	{
		if (strcmp(_Propertyiter.item()->GetName(),Propertyname) == 0)
		{
			delete _Propertyiter.item();
			_Propertyiter.remove();
			break;
		}
		_Propertyiter++;
	}
	return true;	// Success
}

PropertyList& PropertyList::operator=(PropertyList& other)
{
   //first remove the properties if already there
   {
      TDLI<Property> _LI=TDLI<Property>(this);

      //first empty the list
      _LI.delete_all();
   }

  	TDLI<Property> _Propertyiter=TDLI<Property>(&other);
	_Propertyiter.tohead();
	while (!_Propertyiter.hitroot())
	{
		Property* _Property_ptr;
		_Property_ptr = new Property();
      *_Property_ptr=*_Propertyiter.item();
		this->insend(_Property_ptr);
		_Propertyiter++;
	}
	return *this;
}







