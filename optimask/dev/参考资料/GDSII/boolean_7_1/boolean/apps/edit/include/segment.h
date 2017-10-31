//! author="klaas"
//! version="1.0"
/*
Program	POINT.H
Purpose	Definition of GDSII double structure
Last Update	12-12-1995
*/

#ifndef SEGMENT_H
#define SEGMENT_H

#ifdef __GNUG__
#pragma interface
#endif

#include "gdserr.h"
#include "misc.h"
#include "point.h"

enum SEGTYPE {LINE,ARC};
enum HOLEFLAGS {NORMAL_SEG,LINK_SEG,HOLE_SEG};

//! Base class Segment
/*!
 Segments are part of polygons and polylines
 and can be Line_Segment or Arc_Segment.
\ca Line_Seg
\ca Arc_Seg
\ca G_Polygon
\ca G_Polyline
*/
class Segment
{
	public:
		Segment();
		virtual ~Segment();
		virtual double	GetX()=0;
		virtual double	GetY()=0;
		virtual void 		SetX(double)=0;
		virtual void		SetY(double)=0;
		virtual SEGTYPE   GetType()=0;
      inline HOLEFLAGS GetHoleType() {return m_holetype;};
      inline bool      GetBin()      {return m_bin;};
      inline void SetHoleType(HOLEFLAGS a) {m_holetype=a;};
      inline void SetBin(bool bin) {m_bin=bin;};

   protected:
		bool	m_bin   	: 1;				// Marker for walking over the graph
		HOLEFLAGS  m_holetype : 2;				// Is this a part of hole ?
};


//! line Segment
/*!
 Segments are part of polygons and polylines
 and can be Line_Segment or Arc_Segment.
\ca Arc_Seg
\ca G_Polygon
\ca G_Polyline
*/
class Line_Seg : public Segment
{
	public:

		Line_Seg();
		~Line_Seg();
		Line_Seg(double const X, double const Y);
		Line_Seg(Point* const);
      Line_Seg(Line_Seg* other);

//	   void*				operator new(size_t size);
//		void 				operator delete(void* segmentptr);

		void			Set(const double,const double);

		double	GetX();
		double	GetY();
		void 		SetX(double);
		void			SetY(double);
				 SEGTYPE 	GetType();
		Line_Seg&			operator=(Line_Seg&);

	protected:
		Point   _point;
};

//! Arc Segment
/*!
 Segments are part of polygons and polylines
 and can be Line_Segment or Arc_Segment.
\ca Line_Seg
\ca G_Polygon
\ca G_Polyline
*/
class Arc_Seg : public Segment
{
	public:

		Arc_Seg();
		~Arc_Seg();
		Arc_Seg(double const X, double const Y,double const AX, double const AY, double const OX, double const OY);
		Arc_Seg(Point* const, Point* const, Point* const);
      Arc_Seg(Arc_Seg* other);

//      void* 				operator new(size_t size);
//		void 					operator delete(void* segmentptr);

		void			Set(const double,const double,double const AX, double const AY, double const OX, double const OY);

		double	GetX();
		double	GetY();
		double	GetAX();
		double	GetAY();
		double	GetOX();
		double	GetOY();
		void 		   SetX(double);
		void			SetY(double);
		void 		   SetAX(double);
		void			SetAY(double);
		void 		   SetOX(double);
		void			SetOY(double);
      SEGTYPE 	   GetType();
      Arc_Seg&    operator=(Arc_Seg &other);

	protected:
		Point   _point;
		Point   _Apoint;
		Point   _Opoint;
};

#endif
