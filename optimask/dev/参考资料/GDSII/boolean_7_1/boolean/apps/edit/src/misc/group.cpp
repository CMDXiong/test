// name		:	group.cpp
// creator	: 	O. Kerpershoek
// date		:	8-9-1997
// function	: 	used to define groups of layers
//					a layer belongs completely to a group or, if the selected falg is
//					set, only the selected object of that layer.

#include "group.h"

group::group()         										// constructor
{
	for (int i=0;i < MAX_LAYERS;i++) layer[i] = false;
	_onlySelected = false;
	_numberOfLayers = 0;
}

group::~group()												// destructor
{

}

group& group::operator=(group& other)
{
	for (int i=0;i < MAX_LAYERS;i++) layer[i] = other.layer[i];
	_onlySelected = other._onlySelected;
	_numberOfLayers = other._numberOfLayers;

	return *this;
}

void group::Clear()											// clears the group (no layers selected)
{
	for (int i=0;i < MAX_LAYERS;i++) layer[i] = false;
	_onlySelected = false;
	_numberOfLayers = 0;
}

void group::SetLayer(int layernr,bool	value)	// add or remove a layer from the group
{
	if (value != layer[layernr]) _numberOfLayers += (value?1:-1);
	layer[layernr] = value;
}

bool group::GetLayer(int layernr)						// check if a layer belongs to the group
{
	return layer[layernr];
}

void group::SetOnlySelected(bool 	value)			// indicate if only selected objects should be used
{
	_onlySelected = value;
}

bool group::GetOnlySelected()							// check if only selected objects should be used
{
	return _onlySelected;
}

int group::GetNumberOfLayers()
{
	return _numberOfLayers;
}


