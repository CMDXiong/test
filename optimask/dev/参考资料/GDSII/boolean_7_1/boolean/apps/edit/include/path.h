//! author="klaas"
//! version="1.0"
/*
Program	PATH.H
Purpose	Defines GDSII path structure (Header)
Programmers	R. Spekreijse & N. Noorlander
Last Update	12-12-1995
*/

#ifndef PATH_H
#define PATH_H
#ifdef __GNUG__
#pragma interface
#endif

#include "misc.h"
#include "wxmatrix.h"
#include "shape.h"
#include "gdserr.h"
#include "polyline.h"

class DrawDriver;
class Driver_out;

//!Element of type Path (G_Polyline with a width parameter)
/*!
\sa Shape
\sa ElementList
\sa G_Polyline
*/
class Path: public Shape
{
	public:
		Path(int layernr = 0);
		Path(G_Polyline* polyline, int layernr = 0);
		~Path();

      void OWrite(Driver_out& where);
      void DoWrite(DrawDriver& dr);

		void				Init(int);
		BoundingBox&	CalcBoundingBox();
		Path&				operator=(Path&);
		Element*			Duplicate();

		void			  	SetPathtype(PATHTYPE);
		PATHTYPE	  	   GetPathtype();
		void			  	SetDatatype(int);
		int			  	GetDatatype();
		void			  	SetPolyline(G_Polyline*);
		G_Polyline*  	GetPolyline();
		TYPE				GetType() {return PATH;};
		void 				Transform(wxTransformMatrix* _Matrix);
		void           Convert(G_Polygon* _converted, double Aber/*=ARCTOPOLYACCUR*/);

		bool			Select(Point*);
		bool			Select(BoundingBox*,wxTransformMatrix*);

      bool			ContainsArcs();
		wxPropertySheet* GetPropList();
      void 				UpdatePropList();

      bool        GetSpline(){return m_spline;};
      void        SetSpline(bool spline){m_spline=spline;};

	protected:
		G_Polyline*			 m_polyline;
		PATHTYPE				 m_pathtype;
		int					 m_datatype;
      bool               m_has_arcs;
      bool               m_spline;
};


#endif
