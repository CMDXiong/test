//! author="klaas"
//! version="1.0"
/*
Program	POLYLINE.H
Purpose	GdsII polyline structure definition (header)
Last Update	12-12-1995
*/

#ifndef POLYLINE_H
#define POLYLINE_H

#ifdef __GNUG__
#pragma interface
#endif

#define KLEIN 1.0e-30
#define GROOT 1.0e30
#define PHI_MARGE 0.3  //must be lager than 0, 0.3 -> 30%  (graden)
#define SEG_MARGE 0.3  //must be lager than 0, 0.3 -> 30%
#define 	OUTSIDE_POLY		0
#define 	INSIDE_POLY			1
#define 	ON_POLY         	2
// select procedure defines
#define	SELECT_DPHI			10


#include <math.h>

#include "misc.h"
#include "wxmatrix.h"
#include "bbox.h"

#include "segment.h"
#include "b_lnk_itr.h"

//!Polygon as list of segments, where each segment can be
/*!a Line_Segment or Arc_Segment
\saSegment
\saLine_Seg
\saArc_Seg
*/
class G_Polyline :public DL_List<void*>
{
	public:
		// constructors and destructors
		G_Polyline();
		virtual ~G_Polyline();

		void 				MakeEmpty();

		bool 				RemoveZero();
		bool 				ContainsArcs();
      bool				CheckForArcs();
      void				SetArcs(bool val){_hasarcs=val;};
		bool 				CheckForPerfectArc(Point& middle, double& radius);
		BoundingBox* 	CalcBoundingBox(BoundingBox* a_box);
		void   			Transform(wxTransformMatrix* _Matrix);
		void   			ConvertArcs(double aber);
		void 	 			ConvertArcs2(double dphi,int minseg = 6);
      void           ConvertSplined(double Aber);
      void   			CalcArc(TDLI<Segment>* where,Point& mid,double& Radius,double& beginrad,double& midrad,double& endrad,double& phit);
		void   			Copy(G_Polyline* PIT);
		void 	         AddSegment(Segment* a);
		void   			ConvertLine(double aber,double Rmin,double Rmax);
		bool 				PointOnPolyline(Point*, double);
		bool 				PointOnArcseg(Point*,double);
      double         CalcLength();
		G_Polyline&		operator=(G_Polyline&);

	protected:
		int 			TestArc(TDLI<Segment>*,double aber,double Rmin,double Rmax,Point& center_p);
		void  		insertArc(TDLI<Segment>*,int n, Point& center_p);

		static TDLI<Segment> 	_PI;
      bool _hasarcs;
};


//!Polyline as list of segments, where each segment can be
/*!
a Line_Segment or Arc_Segment
\saSegment
\saLine_Seg
\saArc_Seg
*/
class G_Polygon : public G_Polyline
{
	public:
		G_Polygon();
		~G_Polygon();
		G_Polygon& operator=(G_Polygon &other);

		double 	CalcArea();
		bool		DirectionIsClockWise();
		void   	ConvertLine(double aber,double Rmin,double Rmax);
   	int 		PointInPolygon(const Point&, double);
		bool 		ContainsHoles();
      bool		CheckForHoles();
		bool 		CheckForPerfectArc(Point& middle, double& radius);
      void		SetHoles(bool val){_hasholes=val;};
		bool 		RemoveZero();
		void 		ConvertSplined(double Aber);
      double   CalcLength();

	protected:
		void  GetLRO(const Point& P, const Point& p1, const Point& p2, int &LRO1, int &LRO2,const double marge);
		int  	TestArc_back(TDLI<Segment>*,double aber,double Rmin,double Rmax, Point& center_p);
      bool _hasholes;

};

#endif
