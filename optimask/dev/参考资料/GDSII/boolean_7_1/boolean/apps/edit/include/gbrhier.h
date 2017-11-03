//! author="klaas"
//! version="1.0"
/*
Program			HIERDRV.H
Purpose			Virtual base class for all screendrivers (header)
					derived drivers must overload Draw functions!
					Mapping and clipping is done here
*/

#ifndef _GBRDRIVER
#define _GBRDRIVER
#ifdef __GNUG__
#pragma interface
#endif

#define MAX_POINTS_TODRAW 40000
#define MININT -30000
#define MAXINT  30000

//! headerfiles="drivout.h lsetup.h matrix.h bbox.h shape.h b_lnk_itr.h polyline.h"
//! codefiles="gbrhier.cpp"

#include <fstream>
#include <iostream>

using namespace std;

#include "b_lnk_itr.h"
#include "drivout.h"
#include "lsetup.h"
#include "wxmatrix.h"
#include "bbox.h"
#include "shape.h"

#include "polyline.h"

enum GBR_TOOLS {GBR_NORMAL,GBR_HOLE,GBR_LINK};

//!Output driver for GBR files, only outline
/*!
Specific objects in the internal database are written as GERBER data
overloaded functions from class Driver_out write the internal objects in GERBER specific form
It uses the special properties on primitives to output in special GERBER code.
The internal structure is written in hiearchy form (following structure references)
*/
class GBRDriver: public Driver_out
{
	public:
		//! constructor
      /*!
      \param filename: name of gerber output file
      \param only_visible: write only visible layers
      */
		GBRDriver(const wxString& _filename, bool only_visible = false);
		//! destructor
		~GBRDriver();

		void  WriteNumber(double big);

		void 				Write(Box* _Box);
		void 				Write(Circle* _Circle);
		void 				Write(ArrayReference*);
		void				Write(Element*);
		void				Write(Boundary*);
		void				Write(GdsII*);
		void 				Write(Library*) {};
		void				Write(Path*);
		void				Write(Arcseg*);
		void				Write(Point*);
		void				Write(Segment*);
		void				Write(G_Polygon*);
		void				Write(G_Polyline*);
		void 				Write(StructureReference*);
		void				Write(Structure*);
		void				Write(Text*);

	protected:
	   int								rad2deg(double value);

		virtual void					SetLayer(int layer) {};
		virtual void					UnSetLayer(){};
		bool							NeedToDraw(int);

      //! top of visible structures
      Structure*                 _showstructure;

      //! holds the matrix from structure reference
		wxTransformMatrix*	_matrix;

      //! is this structure visible?
		bool							_visible;
      //! holds the units from the library
      //! for containing the absolute polygon
		G_Polygon*                 _AbsPolygon;

	   TDLI<Segment>* 				_AI;

		int								_ActiveLayer;

		wxString	GBR_out_filename;
      //! the filename
		std::ofstream	GBR_out_file;
      //! the output stream
		int      points_written;
      //! number of points on one line

      //! middle point of arc segment
      Point 	_mid;
      double   _Radius,_beginrad,_endrad,_midrad,_phit;
      //! end point of arc to write
      Point    _next;
      //!to tell if it is the first point of a polygon or polyline
      bool   _first;
      //!to tell if it is the second point of a polygon or polyline
      bool   _second;
      //!to tell if right or left radius correction for contours
      bool   _right;
      //!to tell if in pulsed mode
      bool   _pulsed;

      GBR_TOOLS m_active;

};

#endif
