/*
Program	wxLine.H
Purpose	Mainy used for calculating crossings
Last Update	12-12-1995
*/

#ifndef wxLineR_H
#define wxLineR_H

#ifdef __GNUG__
#pragma interface
#endif

enum OUTPRODUCT{R_IS_LEFT,R_IS_ON,R_IS_RIGHT};

#include "point.h"

// Status of a point to a wxLine
enum R_PointStatus {R_LEFT_SIDE, R_RIGHT_SIDE, R_ON_AREA, R_IN_AREA};

class G_Polygon;

class wxLine
{
	public:
		// constructors and destructor
		wxLine();
		wxLine(const Point& a,const Point& b);
		~wxLine();

		Point					GetBeginPoint();							   // Get the beginpoint from a wxLine
		Point					GetEndPoint();   							   // Get the endpoint from a wxLine
		int					CheckIntersect(wxLine&, double Marge);	// Check if two wxLines intersects
		int					Intersect(wxLine&,  Point& bp ,Point& ep ,double Marge) ;   // Intersects two wxLines
		bool 				Intersect(wxLine& lijn,Point& crossing); //intersect two (infinit) lines
		R_PointStatus		PointOnLine(const Point& a_Point, double& Distance, double Marge ); //For an infinite wxLine
		R_PointStatus		PointInLine(const Point& a_Point, double& Distance, double Marge ); //For a non-infinite wxLine
		OUTPRODUCT		 	OutProduct(const wxLine& two,double accur);              // outproduct of two wxLines
		double				Calculate_Y(double X); 								// Caclulate Y if X is known
		void              Virtual_Point(Point& a_point,double distance) const;
		wxLine& 				operator=(const wxLine&); 						 		// assignment operator
/*
		Point* 				OffsetContour_rounded(wxLine* const nextline,Point _last_ins, double factor,G_Polygon *shape);
*/
		void  CalculateLineParameters(); 									// Calculate the parameters if nessecary
      void   OffsetContour(const wxLine& nextline,double factor,Point& offsetpoint) const;

	private:

		int   ActionOnTable1(R_PointStatus,R_PointStatus); 							// Function needed for Intersect
		int   ActionOnTable2(R_PointStatus,R_PointStatus); 							// Function needed for Intersect

		double 		m_AA;
		double		m_BB;
		double		m_CC;
      Point       m_a;
      Point       m_b;
		bool		_valid_parameters;
};

#endif
