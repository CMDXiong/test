// author="klaas"
// version="1.0"
// name		:	group.h
// creator	: 	O. Kerpershoek
// date		:	8-9-1997
// function	: 	used to define groups of layers
//					a layer belongs completely to a group or, if the selected falg is
//					set, only the selected object of that layer.

#ifndef	GROUP_H
#define	GROUP_H

#ifdef __GNUG__
#pragma interface
#endif

#include "lsetup.h"

//!maintains a list of layer numbers, that will be used in boolean operations
//!if the onlyselected flag is set, only the selected primitives will be taken
//!from the layers
class	group {
 public:
	group();
	~group();

   group&	operator=(group&);
   //! clears the group (no layers selected)
	void		Clear();
   //! add or remove a layer from the group
	void		SetLayer(int, bool);
   //! check if a layer belongs to the group
	bool		GetLayer(int);
   //! returns the number of layers in this group
   int		GetNumberOfLayers();
   //! indicate if only selected objects should be used
	void		SetOnlySelected(bool);
   //! check if only selected objects should be used
	bool		GetOnlySelected();
 protected:

	bool	layer[MAX_LAYERS];
	bool	_onlySelected;
   //! counts how many layers are in this group
	int		_numberOfLayers;
};

#endif
