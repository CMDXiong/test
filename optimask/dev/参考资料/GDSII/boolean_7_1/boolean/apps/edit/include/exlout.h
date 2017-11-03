/*
Program		GDSDUMP.H
Purpose		Creates a plain text file from a GDS-II structure
Programmers		R. Spekreijse & N. Noorlander
Last Update		22-12-1995
*/

#ifndef _EXL_OUTDRIVER
#define _EXL_OUTDRIVER

#ifdef __GNUG__
#pragma interface
#endif

#include <string.h>
#include <fstream>

using namespace std;

#include "b_lnk_itr.h"
#include "drivout.h"
#include "misc.h"
#include "gdserr.h"
#include "lsetup.h"
#include "point.h"

class EXL_driver_out: public Driver_out
{
	public:
		// constructor and destructor
		EXL_driver_out(const wxString& _filename, bool only_visible = false);
		~EXL_driver_out();

		void  WriteNumber(double big);
		// overloaded function from class Driver
		void 	Write(ArrayReference*);
		void 	Write(Element*);
		void 	Write(Boundary*);
		void  Write(Box* _Box);
		void  Write(Circle* _Circle);
		void 	Write(GdsII*);
		void 	Write(Library*);
		void 	Write(Path*);
		void 	Write(Arcseg*);
		void 	Write(Point*);
		void  Write(Segment *_Segment);
		void 	Write(G_Polygon*);
		void 	Write(G_Polyline*);
		void 	Write(StructureReference*);
		void 	Write(Structure*);
		void 	Write(Text*);
              
	protected:
		void  WriteHeader();

      // for containing the absolute polygon
		G_Polygon*                 _AbsPolygon;

	   TDLI<Segment>* 				_AI;

      // holds the matrix from structure reference
		wxTransformMatrix*	_matrix;

		wxString EXL_out_filename;		// the filename
		ofstream	EXL_out_file;			// the output stream
		int      points_written;      // number of points on one line
      //the following data is temp data for arcsegments
      Point 	_mid;                // middle point of arc segment
      double   _RadiusC,_Radius,_beginrad,_endrad,_midrad,_phit;
      int      m_tool_count_for_holes;
      Point    _next;				   // end point of arc to write
      bool   _first;              //to tell if it is the first point of a polygon or polyline

		GdsII*	m_flatcopy;
};

#endif 
