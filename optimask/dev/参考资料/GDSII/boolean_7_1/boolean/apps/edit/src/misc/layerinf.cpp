/*
Program	LAYERINF.CPP
Purpose	Holds information about each layer
Programmers	R. Spekreijse & N. Noorlander
Last Update	23-10-1995
*/

#ifdef __GNUG__
#pragma implementation
#endif
#include "layerinf.h"

LayerInfo::LayerInfo()
{
	_visible    = true;
	_selectable = true;
	_readlayer  = true;
	_brush      = 8;
	_order      = 0;

	Set_FillColor(255, 255, 255);
	Set_BorderColor(0, 0, 0);
	strcpy(_name, "<none specified>");
}

char* LayerInfo::Get_Name()
{
	return _name;
}

long LayerInfo::Get_BorderColor()
{
	return _bordercolor;
}

long LayerInfo::Get_FillColor()
{
	return _fillcolor;
}

int LayerInfo::Get_Brush()
{
	return _brush;
}

bool LayerInfo::Get_Visible()
{
	return _visible;
}

bool LayerInfo::Get_Selectable()
{
	return _selectable;
}

bool LayerInfo::Get_Available()
{
	return _available;
}

int LayerInfo::Get_Order()
{
	return _order;
}

bool LayerInfo::Get_ReadLayer()
{
	return _readlayer;
}

void LayerInfo::Set_Name(const char* name)
{
	// copy till 50 characters in _name. After 50 characters truncate
	strncpy(_name, name, 49);
	_name[49] = 0;
}

void LayerInfo::Set_BorderColor(unsigned char R, unsigned char G, unsigned char B)
{
	_bordercolor  = (long) R;
	_bordercolor |= ((long) G << 8);
	_bordercolor |= ((long) B << 16);
}

void LayerInfo::Set_BorderColor(long color)
{
	_bordercolor = color;
}

void  LayerInfo::Set_FillColor(unsigned char R, unsigned char G, unsigned char B)
{
	_fillcolor  = (long) R;
	_fillcolor |= ((long) G << 8);
	_fillcolor |= ((long) B << 16);
}

void LayerInfo::Set_FillColor(long color)
{
	_fillcolor = color;
}

void LayerInfo::Set_Brush(int brush)
{
	_brush = brush;
}

void LayerInfo::Set_Visible(bool status)
{
	_visible = status;
}

void LayerInfo::Set_Selectable(bool status)
{
	_selectable = status;
}

void LayerInfo::Set_Available(bool status)
{
	_available = status;
}

void LayerInfo::Set_Order(int order)
{
	_order = order;
}

void LayerInfo::Set_Read(bool status)
{
	_readlayer = status;
}

LayerInfo& LayerInfo::operator=(const LayerInfo &other)
{
	_visible = other._visible;
   _selectable = other._selectable;
	_available = other._available;
	_fillcolor = other._fillcolor;
	_bordercolor = other._bordercolor;
	_brush = other._brush;
	_order = other._order;
	_readlayer = other._readlayer;
	_gdsinmap = other._gdsinmap;
	_gdsoutmap = other._gdsoutmap;

	strcpy(_name, other._name);
	return *this;
}

// equal or not
int LayerInfo::operator==(const LayerInfo &other) const
{
   if (
         _visible == other._visible         &&
         _selectable == other._selectable   &&
         _available == other._available     &&
         _fillcolor == other._fillcolor     &&
         _bordercolor == other._bordercolor &&
         _brush == other._brush             &&
         _order == other._order             &&
         _readlayer == other._readlayer     &&
         _gdsinmap == other._gdsinmap       &&
         _gdsoutmap == other._gdsoutmap     &&
			!strcmp(_name,other._name)
       )
       return 1;

	return 0;
}

int LayerInfo::operator!=(const LayerInfo &other) const
{
	return ! (*this == other);
}

int LayerInfo::Get_GdsInMapping()
{
	return _gdsinmap;
}

int LayerInfo::Get_GdsOutMapping()
{
	return _gdsoutmap;
}

void LayerInfo::Set_GdsInMapping(int layer)
{
	_gdsinmap=layer;
}
void LayerInfo::Set_GdsOutMapping(int layer)
{
	_gdsoutmap=layer;
}
