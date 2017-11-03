//! author="klaas"
//! version="1.0"
/*
Program			HIERDRV.H
Purpose			Virtual base class for all screendrivers (header)
					derived drivers must overload Draw functions!
					Mapping and clipping is done here
*/

#ifndef _HIERDRIVER
#define _HIERDRIVER
#ifdef __GNUG__
#pragma interface
#endif

#define MAX_POINTS_TODRAW 40000
#define MININT -30000
#define MAXINT  30000

#include "b_lnk_itr.h"
#include "drivout.h"
#include "lsetup.h"
#include "wxmatrix.h"
#include "bbox.h"
#include "shape.h"

#include "polyline.h"

using namespace std;

//!Output driver for CNC files, used far laser machinery
/*!
Specific objects in the internal database are written as CNC data
overloaded functions from class Driver_out write the internal objects in CNC specific form
It uses the special properties on primitives to output in special CNC code.
The internal structure is written in hiearchy form (following structure references)
*/
class HierDriver: public Driver_out
{
	public:
		//! constructor
      /*!
      \param filename: name of GDSII output file
      \param only_visible: write only visible layers
      */
		HierDriver(const wxString& _filename, bool only_visible = false);
		//! destructer
		~HierDriver();

		void  WriteNumber(double big);

		void 				Write(Box* _Box);
		void 				Write(Circle* _Circle);
		void 				Write(ArrayReference*);
		void				Write(Element*);
		void				Write(Boundary*);
		void				Write(ElementList*);
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
		void 								AsHole(G_Polyline* _Polyline);


		virtual void					SetLayer(int layer) {};
		virtual void					UnSetLayer(){};

		bool							NeedToDraw(int);

      //! top of visible structures
      Structure*                 _showstructure;
      //! structure to be drawn
      Structure*                 _drawstructure;

      //! holds the matrix from structure reference
		wxTransformMatrix*	_matrix;

      //! is this structure visible?
		bool							_visible;

      //! for containing the absolute polygon
		G_Polygon*                 _AbsPolygon;

	   TDLI<Segment>* 				_AI;

		int								_ActiveLayer;

		wxString	CNC_out_filename;
      //! the filename
		ofstream	CNC_out_file;
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
      //!to tell if right or left radius correction for contours
      bool   _right;

      //!to tell if radius correction for contours
      bool   _radcor;

      //!to tell if in pulsed mode
      bool   _pulsed;

      //!relative placement of elements within structure
      double m_px;
      //!relative placement of elements within structure
      double m_py;
};

#endif
