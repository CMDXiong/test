//! author="klaas"
//! version="1.0"
/*
Program	BOUNDARY.H
Purpose	Contains a boundary (Header)
Last Update	12-12-1995
*/

#ifndef BOUNDARY_H
#define BOUNDARY_H

#ifdef __GNUG__
#pragma interface
#endif

#include "misc.h"
#include "wxmatrix.h"
#include "polyline.h"
#include "gdserr.h"
#include "shape.h"

class DrawDriver;
class Driver_out;

//!Element of type Boundary (G_Polygon with a width parameter)
/*! See also class Shape
\sa ElementList
\sa G_Polygon
*/
class Boundary : public Shape
{
	public:
		Boundary(int layernr = 0);
		Boundary(G_Polygon*, int layernr = 0);
		~Boundary();

      void OWrite(Driver_out& where);
      void DoWrite(DrawDriver& where);
      void WriteEdit(Driver_out& where);
      void WriteMove(Driver_out& where);

		BoundingBox&			CalcBoundingBox();
		int			 			GetDatatype();
		G_Polygon*	 	      GetPolygon();
		void			 			SetDatatype(int);
		void			 	      SetPolygon(G_Polygon*);
		TYPE					  	GetType() {return BOUNDARY;};
		void 						Transform(wxTransformMatrix* _Matrix);
		void                 Convert(G_Polygon* converted, double Aber);

		Element*					Duplicate();
		Boundary& 				operator=(Boundary &other);

		bool			      Select(Point*);
		bool			      Select(BoundingBox*,wxTransformMatrix*);

      //does the polygon contain arc segments?
      bool					ContainsArcs();
      wxPropertySheet*		GetPropList();
		void 						UpdatePropList();

      bool        GetSpline(){return m_spline;};
      void        SetSpline(bool spline){m_spline=spline;};

	protected:
		void						Init(int);

		G_Polygon*				m_polygon;
		int						m_datatype;
      bool                 m_spline;
};

#endif
