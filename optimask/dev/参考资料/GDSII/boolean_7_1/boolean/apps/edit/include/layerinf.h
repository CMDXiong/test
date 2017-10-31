//! author="klaas"
//! version="1.0"
/*
Program	LAYERINF.H
Purpose	Holds information about each layer
Programmers	R. Spekreijse & N. Noorlander
Last Update	12-12-1995
*/

#ifndef LAYERINF_H
#define LAYERINF_H

#ifdef __GNUG__
#pragma interface
#endif

//! headerfiles="string.h layerinf.h misc.h"
//! codefiles="layerinf.cpp"

#include <string.h>
#include "misc.h"

//!contains the layer properties for one layer,
//!settings like color, fill paterns etc.
//!\sa Setup
class LayerInfo
{
	public:
		LayerInfo();

		char*		   Get_Name();
		long			Get_BorderColor();
		long			Get_FillColor();
		int			Get_Brush();
		bool		Get_Visible();
		bool		Get_Selectable();
		bool		Get_Available();
		int			Get_Order();
		bool      Get_ReadLayer();
		int			Get_GdsInMapping();
		int			Get_GdsOutMapping();

		void			Set_Name(const char* name);
		void			Set_BorderColor(unsigned char R = 0, unsigned char G = 0, unsigned char B = 0);
		void			Set_BorderColor(long color = 0);
		void			Set_FillColor(unsigned char R = 255, unsigned char G = 255, unsigned char B = 255);
		void			Set_FillColor(long color = 0x00FFFFFFl);
		void			Set_Visible(bool status = true);
		void			Set_Selectable(bool status);
		void			Set_Brush(int brush = 1);
		void			Set_Available(bool status = false);
		void			Set_Order(int order);
		void			Set_Read(bool status = true);
		void			Set_GdsInMapping(int layer);
		void			Set_GdsOutMapping(int layer);

		LayerInfo&			operator=(const LayerInfo&);
		int 					operator==(const LayerInfo &other) const;
		int 					operator!=(const LayerInfo &other) const;

	protected:
      //!is the layer selectable
		bool				_selectable;
      //!is the layer visible
		bool				_visible;
      //!is the layer filled with primitives
		bool				_available;
      //!will the layer be read from a file
		bool				_readlayer;
      //!fill color for the layer
      long 					_fillcolor;
      //!border color for the layer
		long					_bordercolor;
      //!layer name
		char					_name[50];
      //!fill pattern
		int					_brush;
      //!drawing order number
		int					_order;
      //!mapping to this layer from the GDSII layer in the input file
		int					_gdsinmap;
      //!mapping to this layer from the GDSII layer in the output file
		int					_gdsoutmap;
};
#endif

