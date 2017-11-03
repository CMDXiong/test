/*
Program	BBOX.CPP
Purpose	The BoundingBox class stores one boundingbox.
		The boundingbox is defined by two coordiates,
		a upperleft coordinate and a lowerright coordinate.
		The boundingBox will be used by many classes:
		boundary, path, text, sref, aref, structure and
		gdsii. When an error occurs an object of class will be thrown.
Last Update	12-12-1995
*/

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#ifdef __GNUG__
#pragma interface
#endif


#include "point.h"
#include "wxmatrix.h"
#include "gdserr.h"

enum OVERLAP {_IN,_ON,_OUT};

class BoundingBox
{
	public:
		// constructors
		BoundingBox();
      //destructor
		BoundingBox(BoundingBox&);

		BoundingBox(const Point&);

      //constructor boundingbox
		BoundingBox(double xmin, double ymin, double xmax, double ymax);

		bool			And(BoundingBox*, double Marge = 0);

		void				EnLarge(const double Marge);
		void				Shrink(const double Marge);

		void				Expand(const Point&,const Point&);
		void				Expand(const Point&);
		void				Expand(double x,double y);
		void				Expand(BoundingBox&);

		OVERLAP 			Intersect(BoundingBox &, double Marge = 0);
		bool			LineIntersect(const Point&,const Point&);
		bool			PointInBox(const Point&, double Marge = 0);
		bool			PointInBox(double, double, double Marge = 0);

		void				Reset();

		void				SetBoundingBox(const Point&);

		void				Translate(Point&);
		void				MapBbox(wxTransformMatrix*);
		Point	GetMin();
		Point 	GetMax();
      double   GetWidth(){return _xmax-_xmin;};
      double   GetHeight(){return _ymax-_ymin;};
		bool	GetValid();
		void		SetMin(double, double);
		void		SetMax(double, double);
		void		SetValid(bool);

		BoundingBox&	operator+(BoundingBox&);
		BoundingBox&	operator=(BoundingBox&);

	protected:
		double	Min(double, double);
		double	Max(double, double);

		double			_xmin;
		double			_ymin;
		double			_xmax;
		double			_ymax;
		bool			_valid_bbox;
};

#endif
