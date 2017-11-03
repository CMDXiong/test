//! author="klaas"
//! version="1.0"
/*
Program	SETUP.H
Purpose	Holds setup information about each layer
Programmers	R. Spekreijse
Last Update	22-12-1995
*/

#ifndef LSETUP_H
#define LSETUP_H

#ifdef __GNUG__
#pragma interface
#endif

#define MAX_LAYERS 200

#include <stdlib.h>
#include <stdio.h>

#include "layerinf.h"
#include "misc.h"
#include "gdserr.h"

//!contains the setup for the layers, the setup contains for each layer number
//!all settings like color, fill paterns etc.
//!\sa LayerInfo
class Setup
{
	public:
		Setup();
		~Setup();

		char*	Get_Name(int layernumber);
		int		Get_Number(const char* name);
		long		Get_BorderColor(int layernumber);
		long		Get_FillColor(int layernumber);
		bool	Get_Visible(int layernumber);
		bool	Get_Selectable(int layernumber);
		int		Get_Brush(int layernumber);
		bool	Get_Available(int layernumber);
		int		Get_Order(int layernumber);
		bool  Get_Read(int layernumber);
		int		Get_GdsInMapping(int layernumber);
		int		Get_GdsOutMapping(int layernumber);

		void		Set_Name(int layernumber, const char* name);
		void		Set_BorderColor(int layernumber, unsigned char R, unsigned char G, unsigned char B);
		void		Set_BorderColor(int layernumber, long color = 0);
		void		Set_FillColor(int layernumber, unsigned char R, unsigned char G, unsigned char B);
		void		Set_FillColor(int layernumber, long color = 0x00FFFFFFl);
		void		Set_Visible(int layernumber, bool = true);
		void		Set_Selectable(int layernumber, bool = true);
		void		Set_Brush(int layernumber, int brush = 1);
		void		Set_Available(int layernumber, bool = true);
		void		Set_Order(int layernumber, int status);
		void		Set_Read(int layernumber, bool = true);
		void		Set_GdsInMapping(int layernumber,int layer);
		void		Set_GdsOutMapping(int layernumber,int layer);


		//! set all layers visible
		void				Set_All_Layers_Visible();
		//! set all layers invisible
		void				Set_All_Layers_Invisible();

		//! set all layers readon
		void				Set_All_Layers_ReadOn();
		//! set all layers readoff
		void				Set_All_Layers_ReadOff();

		//! set all visible layers selectable
		void				Set_All_Layers_Selectable();
		//! set all visible layers not selectable
		void				Set_All_Layers_NotSelectable();

		Setup&			operator=(const Setup&);
		int				operator==(const Setup&) const;
		int				operator!=(const Setup&) const;

      //!update layers available needed
		bool	Get_UpdateAvailableLayers(){return m_update_available_layers;};
		void		Set_UpdateAvailableLayers(bool value){m_update_available_layers=value;};

	protected:

		bool							m_update_available_layers;

      //! the array with layer information records
		LayerInfo*		_layers[MAX_LAYERS];
};


#endif


