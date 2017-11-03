//! author="klaas"
//! version="1.0"
/*
Program	REFERENC.H
Purpose	defines GDSII refence structure type (header)
Programmers	R. Spekreijse & N. Noorlander
Last Update	9-4-1998, M. Reemst
*/

#ifndef REFERENC_H
#define REFERENC_H

#ifdef __GNUG__
#pragma interface
#endif

class DrawDriver;
class Driver_out;

#include "misc.h"
#include "point.h"
#include "bbox.h"
#include "element.h"

//forward declaration
class Structure;

//! Base class for References to structures.
/*!
Each structure is a drawing by itself, a refrence within a structure,
points to another structure, to placed within the current structure
at the given point.
Reference can be of type:
   ArrayReference
   StructureReference
*/
class Reference : public Element
{
	public:
		Reference();
		Reference(Structure*);
		~Reference();

		virtual BoundingBox&			CalcBoundingBox()=0;
		virtual BoundingBox& 		GetBoundingBox();

		virtual void 	  Flatten(Structure* _Top,wxTransformMatrix* _Matrix,bool srefSelected,EleFlags refgroup,bool deep )=0;

		Structure*						GetStructure();
		void								SetStructure(Structure*);
		void								SetStructureReferenceName(const wxString&);
		const wxString&  		      GetStructureReferenceName();
		void								GetPoint(Point& offsetpoint);
      void                       SetPoint(Point& offsetpoint);

		wxTransformMatrix*			GetRelativeMatrix();
		void								SetRelativeMatrix(wxTransformMatrix*);
		void 								CalculateRelativeMatrices();

		void								SetReferencesNr();
		void								SetExplodeReferencesNr();
		wxPropertySheet*           GetPropList();
      void 				            UpdatePropList();
		virtual void               CalcArea(double* area,EleFlags mask)=0;

	protected:
		Structure* 						_structure;
		wxString       			 	_structurereferencename;
		wxTransformMatrix*			_relative_matrix;
};

//!Array Reference to structure.
/*!
references as an array a structure
\ca StructureReference
*/
class ArrayReference : public Reference
{
	public:
		ArrayReference();
		~ArrayReference();

      void OWrite(Driver_out& where);
      void DoWrite(DrawDriver& dr);

		TYPE					  	GetType() {return AREF;};
      void SetHorzSpace(Point *hor_space);
      void SetVertSpace(Point *vert_space);
		Point* GetHorzPoint();
		Point* GetVertPoint();

		// public member functions
		int	GetRows();
		void	SetRows(int i);
		int	GetColumns();
		void	SetColumns(int i);
		BoundingBox&			CalcBoundingBox();
		ArrayReference& 		operator=(ArrayReference&);
		Element*	   			Duplicate();
		void 						Transform(wxTransformMatrix* _Matrix);
		void           		CalcArea(double* area,EleFlags mask);
		void						Flatten(Structure* _Top,wxTransformMatrix* _Matrix,bool srefSelected,EleFlags refgroup,bool deep );


	protected:
		bool				  	Select(Point*);
		bool				   Select(BoundingBox*,wxTransformMatrix*);
		int					m_columns;
		int					m_rows;
		Point*					_horpoint;
		Point*					_verpoint;
};

//!Structure Reference to structure.
/*!
refrences as one structure
\ca ArrayReference
*/
class StructureReference : public Reference
{
	public:
		// constructors and destructors
		StructureReference();
		~StructureReference();

      void OWrite(Driver_out& where);
      void DoWrite(DrawDriver& dr);

		TYPE					  	GetType() {return SREF;};

		// public memberfunctions
		BoundingBox&	      CalcBoundingBox();

		// operators
		StructureReference&	operator=(StructureReference&);
		Element* 				Duplicate();
		void 						Transform(wxTransformMatrix* _Matrix);
		void 	  Flatten(Structure* _Top,wxTransformMatrix* _Matrix,bool srefSelected,EleFlags refgroup,bool deep );
		void    CalcArea(double* area,EleFlags mask);


	protected:
		bool				  	Select(Point*);
		bool				   Select(BoundingBox*,wxTransformMatrix*);

};

#endif
