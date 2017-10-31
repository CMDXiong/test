//! author="klaas"
//! version="1.0"
/*
Program			ELEMENT.H
Purpose			base class for elements
Last Update		24-09-96
*/

#ifndef ELEMENT_H
#define ELEMENT_H

#ifdef __GNUG__
#pragma interface
#endif

enum SelectType { DESELECT, SELECT, SELECTALL, DESELECTALL,QUERY };
enum TYPE {NON,BOUNDARY,PATH,TEXT,SREF,AREF,ARCSEG,BOX,CIRCLE,IMAGE,SLINE};
enum EleFlags{ELE_NON=0x0000,ELE_SELECTED=0x0001,ELE_GR_A=0x0002,ELE_GR_B=0x0004,
				  ELE_GR_C=0x0008,ELE_GR_AB=0x0006,ELE_FILLED=0x0010,ELE_ALL=0xFFFF};
enum PATHTYPE {PATH_END_SQAURE,PATH_END_ROUND,PATH_END_SQAURE_EXT};

#include "misc.h"
#include "bbox.h"

class wxPropertySheet;

class DrawDriver;
class Driver_out;


typedef struct element_bitflags
{

#if wxBYTE_ORDER == wxLITTLE_ENDIAN
		unsigned	m_select : 1;
		unsigned	m_group_a: 1;
		unsigned	m_group_b: 1;
		unsigned	m_group_c: 1;
		unsigned	m_filled : 1;
		unsigned	m_template:1 ;
		unsigned	m_external :1 ;
		unsigned	m_used :1 ;
		unsigned	reserved  :	 8;
#else
		unsigned	reserved  :	 8;
		unsigned	m_used :1 ;
		unsigned	m_external :1 ;
		unsigned	m_template:1 ;
		unsigned	m_filled : 1;
		unsigned	m_group_c: 1;
		unsigned	m_group_b: 1;
		unsigned	m_group_a: 1;
		unsigned	m_select : 1;
#endif
} element_bitflags;

class Structure;

//!pure virtual Base class for all data types, that can be in a drawing.
/*!
References to structures and Shapes are derived from this class
In general Elements are part of an ElementList, the ElementList is part of a Structure.
A Refrence Element is Referencing a Struture. This way hiearchy is created.
\sa Shape
\sa Reference
\sa Structure
\sa ElementList
*/
class Element
{
	public:
		Element();

		virtual ~Element();

		Element& operator=(Element &other);

      void Write(Driver_out& dr);
      virtual void OWrite(Driver_out& dr)=0;

      virtual void Write(DrawDriver& dr);
      virtual void DoWrite(DrawDriver& dr)=0;

		virtual Element*     			Duplicate()=0;

      //!calculate the bounding box
		virtual BoundingBox&				CalcBoundingBox()=0;
      //!get the boundingbox
		virtual BoundingBox&				GetBoundingBox()=0;
      //!tranform the element given a matrix
		virtual void 						Transform( wxTransformMatrix* _Matrix)=0;

      //!get the element type NON,BOUNDARY,PATH,TEXT,SREF,AREF,ARCSEG,BOX,CIRCLE
		virtual TYPE						GetType()=0;
      //!select the element if Point is within it
		virtual bool						Select(Point*)=0;
      //!select the element if is within the box.
      //!matrix is used to calculate the relative BoundingBox within structures
		virtual bool						Select(BoundingBox*,wxTransformMatrix*)=0;

      //!set the filled flag
		void		  					      SetFilled(bool);
      //!get the filled flag
		bool	  					      	GetFilled();
      //!set the select flag
		void		  					      SetSelect(bool);
      //!get the select flag
		bool	  					      	GetSelect();
      //!get the groupA flag
		bool 									GetGroupA();
      //!get the groupB flag
		bool 									GetGroupB();
      //!get the groupC flag
		bool 									GetGroupC();
      //!set the groupA flag
		void 									SetGroupA(bool value);
      //!set the groupB flag
		void 									SetGroupB(bool value);
      //!set the groupC flag
		void 									SetGroupC(bool value);
      //!GDSII compatible
		void									SetFlags(short int which);
      //!reset all flags in which
		void									ClearFlags(short int which);
      //!get flags mask
		short int 						   GetFlags();
      //!has property list
		virtual wxPropertySheet* 		HasPropList();
      //!get property list
      virtual wxPropertySheet*		GetPropList();
      //!update property list , removinf predefined properties (mostly members of object)
      virtual void						UpdatePropList();
      //!set a property
		void 									AddProperty(const wxString& name,const wxString& type,const wxString& value);
      //!set event stamp
      void 									SetStamp(long event);
      //!get event stamp
      long 									GetStamp();

      virtual Structure*  					GetStructure(){return 0;};

		void		SetTemplate(bool b = true);
		void		SetExternal(bool b = true);
		void		SetUsed(bool b = true);

		bool	GetTemplate();
		bool	GetExternal();
		bool	GetUsed();

   protected:
      //!event stamp that is unique for every element
		long 					 m_stamp;
      //!mask of flags for select groups etc.
		union  {
               short int _all;
					element_bitflags bits;
				 } bit_flags;
      //!user defined properties attached to the element
      wxPropertySheet*   m_wxpropsheet;
      //!boundingbox of primitive
		BoundingBox	m_boundingbox;

};
#endif

