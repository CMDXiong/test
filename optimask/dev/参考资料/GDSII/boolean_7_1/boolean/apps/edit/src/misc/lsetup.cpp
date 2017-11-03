/*
Program	SETUP.CPP
Purpose	Holds setup information about each layer
Programmers	R. Spekreijse
Last Update	22-12-1995
*/

#ifdef __GNUG__
#pragma implementation
#endif
#include "lsetup.h"

Setup::Setup()
{
	m_update_available_layers=true;
	// initialize all the layers
	for (int i = 0; i < MAX_LAYERS; i++)
	{
		_layers[i] = new LayerInfo();
		Set_Order(i, i);
		Set_GdsInMapping(i, i);
		Set_GdsOutMapping(i, i);
      char buf[40];
      sprintf(buf,"layer %d",i);
		Set_Name(i, buf);
	}
}

Setup::~Setup()
{
	for (int i = 0; i < MAX_LAYERS; i++)
		delete _layers[i];
}

char* Setup::Get_Name(int layernumber)
{
	return _layers[layernumber]->Get_Name();
}

int Setup::Get_Number(const char* name)
{
   int j=-1;
	for (int i = 0; i < MAX_LAYERS; i++)
      if (!stricmp(_layers[i]->Get_Name(),name))
      {
          j=i;
          break;
      }

	return j;
}

long Setup::Get_BorderColor(int layernumber)
{
	return _layers[layernumber]->Get_BorderColor();
}

long Setup::Get_FillColor(int layernumber)
{
	return _layers[layernumber]->Get_FillColor();
}

bool Setup::Get_Visible(int layernumber)
{
	return _layers[layernumber]->Get_Visible();
}

bool Setup::Get_Selectable(int layernumber)
{
	return _layers[layernumber]->Get_Selectable();
}

int Setup::Get_Brush(int layernumber)
{
	return _layers[layernumber]->Get_Brush();
}

bool Setup::Get_Available(int layernumber)
{
	return _layers[layernumber]->Get_Available();
}

int Setup::Get_Order(int layernumber)
{
	return _layers[layernumber]->Get_Order();
}

bool Setup::Get_Read(int layernumber)
{
	return _layers[layernumber]->Get_ReadLayer();
}


void Setup::Set_Name(int layernumber, const char* name)
{
	_layers[layernumber]->Set_Name(name);
}

void Setup::Set_BorderColor(int layernumber, unsigned char R, unsigned char G, unsigned char B)
{
	_layers[layernumber]->Set_BorderColor(R, G, B);
}

void Setup::Set_BorderColor(int layernumber, long color)
{
	_layers[layernumber]->Set_BorderColor(color);
}

void Setup::Set_FillColor(int layernumber, unsigned char R, unsigned char G, unsigned char B)
{
	_layers[layernumber]->Set_FillColor(R, G, B);
}

void Setup::Set_FillColor(int layernumber, long color)
{
	_layers[layernumber]->Set_FillColor(color);
}

void Setup::Set_Visible(int layernumber, bool status)
{
	_layers[layernumber]->Set_Visible(status);
}

void Setup::Set_Selectable(int layernumber, bool status)
{
	_layers[layernumber]->Set_Selectable(status);
}

void Setup::Set_Brush(int layernumber, int brush)
{
	_layers[layernumber]->Set_Brush(brush);
}

void Setup::Set_Available(int layernumber, bool status)
{
	_layers[layernumber]->Set_Available(status);
}

void Setup::Set_Order(int layernumber, int status)
{
	_layers[layernumber]->Set_Order(status);
}

void Setup::Set_Read(int layernumber, bool status)
{
	_layers[layernumber]->Set_Read(status);
}

// copy the contents of this class in another
Setup& Setup::operator=(const Setup &other)
{
	for (int i = 0; i < MAX_LAYERS; i++)
		*_layers[i] = *other._layers[i];

	return *this;
}

// equal or not
int Setup::operator==(const Setup &other) const
{
	int result  = 1;

	for (int i = 0; i < MAX_LAYERS; i++)
   {
		if (*_layers[i] != *other._layers[i])
      {
			return 0;
      }
   }

	return result;
}

// compare a vector v with this
int Setup::operator!=(const Setup &other) const
{
	return ! (*this == other);
}

// set all layers visible
void Setup::Set_All_Layers_Visible()
{
	for (int i = 0; i < MAX_LAYERS ; i++)
		Set_Visible(i);
}

// set all layers invisible
void Setup::Set_All_Layers_Invisible()
{
	for (int i = 0; i < MAX_LAYERS ; i++)
		Set_Visible(i, false);
}

// set all layers readon
void Setup::Set_All_Layers_ReadOn()
{
	for (int i = 0; i < MAX_LAYERS ; i++)
		Set_Read(i,true);
}

// set all layers readoff
void Setup::Set_All_Layers_ReadOff()
{
	for (int i = 0; i < MAX_LAYERS ; i++)
		Set_Read(i, false);
}

// set all visible layers selectable
void Setup::Set_All_Layers_Selectable()
{
	for (int i = 0; i < MAX_LAYERS ; i++)
		if (Get_Visible(i)) Set_Selectable(i, true);
}

// set all layers not selectable
void Setup::Set_All_Layers_NotSelectable()
{
	for (int i = 0; i < MAX_LAYERS ; i++)
		Set_Selectable(i, false);
}

int  Setup::Get_GdsInMapping(int layernumber)
{
	return _layers[layernumber]->Get_GdsInMapping();
}

int  Setup::Get_GdsOutMapping(int layernumber)
{
	return _layers[layernumber]->Get_GdsOutMapping();
}

void Setup::Set_GdsInMapping(int layernumber,int layer)
{
	_layers[layernumber]->Set_GdsInMapping(layer);
}

void Setup::Set_GdsOutMapping(int layernumber,int layer)
{
	_layers[layernumber]->Set_GdsOutMapping(layer);
}

