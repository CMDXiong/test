/*
Program	ELEMLST.H
Purpose	Defines a list of elements (Header)
Last Update	04-04-1996
*/
#ifndef ELEMLST_H
#define ELEMLST_H

#ifdef __GNUG__
#pragma interface
#endif

#include "misc.h"
#include "b_lnk_itr.h"

#include "bbox.h"
#include "element.h"
#include "shape.h"
#include "wxmatrix.h"
#include "gdserr.h"
#include "group.h"
#include "proper.h"

class DrawDriver;
class Driver_out;

enum OPS { AND, OR, EXOR, A_SUB_B, B_SUB_A, MAKE_RING, CORRECTION, SMOOTHEN};

class Structure;

class ElementList : public DL_List<void*>
{
	public:

		ElementList();
		~ElementList();

      void Write(Driver_out& where);
      void Write(DrawDriver& dr);

		void           SetBoundingBox(BoundingBox& b){ _boundingbox = b   ;};
		BoundingBox&	GetBoundingBox()				  { return _boundingbox;};
		ElementList&  	operator=(ElementList&);

		BoundingBox&	CalcBoundingBox();

		bool 				CalcArea(double* area,EleFlags mask);
		void				SortOnBoundingBox();
		void				SortOnXYmin();
		void 				SetReferenceNr();
		void 				SetExplodeReferenceNr();

		bool 			Delete_A(bool groupmode,EleFlags mask);
		bool 			Delete(TYPE a_type,bool OnlySelected);
      bool 			Delete(TYPE a_type,EleFlags mask);
		bool 			Delete(int whichlayer);
		void				Transform(wxTransformMatrix* _trans,EleFlags mask);
		bool 			SelectedAtPoint(Point* P);
		bool  		AddProperty(const wxString& name,const wxString& type,const wxString& value,bool _selectedonly);
		bool 			AddProperty(EleFlags mask,const wxString& name,const wxString& type,const wxString& value);
		void 			SetTimeStamp(long time);
		void 			ClearFlags(short int mask);
		void 			SetFlags(EleFlags mask,bool onlyselected);
		Element*		Select(Point* ,SelectType);
		bool 			Select(BoundingBox* ,SelectType,wxTransformMatrix*);

		bool        ToLines(int destination,EleFlags mask);
		bool 			ToArcs(int destination,EleFlags mask);
		bool 			Move(int destination,EleFlags mask);
		bool 			Copy(int destination,EleFlags mask);
		void 			SetGroupFlags(group* a_group,EleFlags mask,bool srefselected);
		void			SetGroupFlagsRefs(EleFlags mask,bool srefselected);
		void 			SetAvailable();
		void        Flatten(Structure* _Top,wxTransformMatrix* _Matrix,bool,bool select,EleFlags refgroup,bool deep);
		void 			Operation(OPS operation,EleFlags maskA,EleFlags maskB,int destination,bool m_destination_add);

      void 			SetBinHere(bool bin){m_binhere=bin;}
		bool 			GetBinHere(){ return m_binhere;}

      void 			SetSortedOnBox(bool value){m_sortedOnBbox=value;}

      protected:

		BoundingBox	_boundingbox;
      bool        m_sortedOnBbox;
      bool        m_binhere;

};

#endif
