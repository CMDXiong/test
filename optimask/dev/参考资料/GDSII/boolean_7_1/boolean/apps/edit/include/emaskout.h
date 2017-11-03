//! author="klaas"
//! version="1.0"
/*
 *	Program		FLASHOUT.H
 *	Purpose		Creates a Mann file from a GDS-II structure
 *	Author		J.C.Mollien
 *		"Sunday morning I'm waking up, can't even focus on my coffee cup
 *     I don't know whose bed I'm in, where do I start, where do I begin?"
 *	Modified		03-09-1997
 * Created		19-08-1997
 */

#ifndef _EMASK_OUTDRIVER
#define _EMASK_OUTDRIVER

#ifdef __GNUG__
#pragma interface
#endif

#include <string.h>
#include <fstream>
#include <math.h>

using namespace std;

#include "drivout.h"
#include "misc.h"
#include "gdserr.h"
#include "lsetup.h"

//! Output driver for DAVID Mann files.
/*!
DAVID mann contains rectanguler DATA called flashes
overloaded function from class Driver_out write the internal objects in DAVID MANN specific form
most internal data is ignored
*/
class EMASK_driver_out: public Driver_out
{
	public:
		// constructor and destructor
		EMASK_driver_out(const wxString& _filename, bool only_visible=false);
		~EMASK_driver_out();

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
		void  	WriteHeader();
		void 		WriteOuttro();

		void 		ReadFlash();
		void 		WriteFlash();

		int rad2deg(double value);

	private:

		wxString	EMASK_out_filename;	// the filename
		std::ofstream	EMASK_out_file;		// the output stream

		int      Flashes_Written;
		int		Points_Written;

		int      error;               // fout element in file (!box)

		double	X[3];	double	Y[3];   // to store points(x,y)

		double 	a;
		double	w;
		double 	h;
		double	x;
		double	y;


		int 	Xpos;	  		// to store the calculated Flash
		int 	Ypos;			// prior to stuffing it in the
		int 	Width;		// file.
		int 	Height;
		int 		Angle;

		int 	lastXpos;   // to store the last Flash
		int 	lastYpos;
		int 	lastWidth;
		int 	lastHeight;
		int 		lastAngle;
};

#endif //_EMASK_OUTDRIVER
