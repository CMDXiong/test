/*
Program	STRUCTUR.H
Purpose	GdsII structure definition (header)
*/

#ifndef STRUCTURE_H
#define STRUCTURE_H

#ifdef __GNUG__
#pragma interface
#endif

#include <string.h>

#include "misc.h"
#include "wxmatrix.h"
#include "bbox.h"
#include "gdserr.h"
#include "gdsii.h"
//#include "element.h"
#include "proper.h"
#include "elemlst.h"

class DrawDriver;
class Driver_out;

class ModeTime;

class Structure : public ElementList
{
	public:
		// constructors & destructors
		Structure();
		Structure(const wxString&);
		~Structure();

      void Write(DrawDriver& dr);

		// public memberfunctions
		void			Init(const wxString& name);
		void			DecrementReferences(int nr = 1);
		void			IncrementReferences();
		void 			LinkReferences(GdsII* structurelist);

      void        Rename(const wxString& oud, const wxString& nieuw);
		void  		Rename(GdsII* _structurelist);

		Structure&	operator=(Structure&);

		// get and set functions
		int					GetReferenced();
		void					SetReferenced(int);
		void 				   ResetReferenced();
		const wxString&   GetStructureName();
		void					SetStructureName(const wxString&);
		ModeTime*			GetBgnstr();
		void					SetBgnstr(ModeTime*);

		int					      GetExplodeReferenced();
		void					      SetExplodeReferenced(int);
		void 			          	ResetExplodeReferenced();
		void							DecrementExplodeReferences(int nr = 1);
		void							IncrementExplodeReferences();
		void 							Explode(Structure* _Top,bool selectedonly,GdsII* _structurelist);
		void 							Flatten(Structure* _Top,wxTransformMatrix* _Matrix,bool, bool , EleFlags,bool deep);

      //!set event stamp
      void 							SetStamp(long event);
      //!get event stamp
      long 							GetStamp();

	protected:
		void 							Redirect(Structure* _old,Structure* _new);

		wxString						_structurename;
		ModeTime*					_bgnstr;
		int							_nr_of_references;
		int 							_nr_of_explode_references;

      //!event stamp that is unique for every element
		long 					      _stamp;
};

#endif
