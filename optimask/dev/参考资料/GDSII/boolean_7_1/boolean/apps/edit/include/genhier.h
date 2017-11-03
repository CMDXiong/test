/*
Program			SCRNDRV.H
Purpose			Virtual base class for all screendrivers (header)
					derived drivers must overload Draw functions!
					Mapping and clipping is done here
*/

#pragma interface
#ifndef _SCREENDRIVER
#define _SCREENDRIVER

#define MAX_POINTS_TODRAW 40000
#define MININT -30000
#define MAXINT  30000
// #endif

#include "b_lnk_itr.h"
#include "drivout.h"
#include "setup.h"
#include "matrix.h"
#include "bbox.h"
#include "shape.h"

#include "polyline.h"


// now declare the real class
class HierDriver: public Driver_out
{
	public:
		virtual bool Pending()=0;
		// constructors and destructor
		HierDriver();
		~HierDriver();

		// overloaded functions from class Driver_out
		// Some are not used for screen output -> {}
		// overloaded here because derived classes don't need them and
		// there are useless here because we don't draw them

		void 				Write(Box* _Box);
		void 				Write(Circle* _Circle);
		void 				Write(ArrayReference*);
		void 				Write(Bgnlib*) {};
		void 				Write(Bgnstr*) {};
		void				Write(Element*);
		void				Write(Boundary*);
		void				Write(ElementList*);
		void 				Write(ColumnsRows*) {};
		void 				Write(Elflags*) {};
		void				Write(GdsII*);
		void 				Write(Header*) {};
		void 				Write(Library*) {};
		void				Write(Path*);
		void				Write(Arcseg*);
		void 				Write(Pathtype*) {};
		void				Write(Point*);
		void				Write(Segment*);
		void				Write(G_Polygon*);
		void				Write(G_Polyline*);
		void 				Write(Presentation*) {};
		void 				Write(Strans*) {};
		void 				Write(StructureList*) {};
		void 				Write(StructureReference*);
		void				Write(Structure*);
		void				Write(Text*);
		void 				Write(Texttype*) {};
		void 				Write(Units*) {};

		void 								SetUnits(Units* units);

	protected:

		virtual void					SetLayer(int layer) = 0;
		virtual void					UnSetLayer()= 0;
		virtual void					GDSDrawText(char* text, double x, double y, double x2, double y2, Presentation*, Matrix<double>*)=0;

		void								Transform(EIGHT_G_BYTE_REAL&,
															 EIGHT_G_BYTE_REAL&);
		void								Transform(EIGHT_G_BYTE_REAL&,
															 EIGHT_G_BYTE_REAL&,
															 EIGHT_G_BYTE_REAL&,
															 EIGHT_G_BYTE_REAL&);

		bool							NeedToDraw(int);

      Structure*                 _showstructure;   // top of visible structures
      Structure*                 _drawstructure;   // structure to be drawn

		Matrix<EIGHT_G_BYTE_REAL>*	_matrix;				// holds the matrix from structure reference

		bool							_visible;			// is this structure visible?
		Units*  				         _units;	         // holds the units from the library

		G_Polygon*                 _AbsPolygon;      // for containing the absolute polygon
	   TDLI<Segment>* 				_AI;

		int								_ActiveLayer;
};

#endif
