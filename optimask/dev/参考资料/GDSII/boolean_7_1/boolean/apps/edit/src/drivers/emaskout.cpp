/*
 *	Program		FLASHOUT.CPP
 *	Purpose		Creates an output file from GDS-II in flash format
 *	Author		J.C.Mollien
 *		"Sunday morning I'm waking up, can't even focus on my coffee cup
 *     I don't know whose bed I'm in, where do I start, where do I begin?"
 * Modified		03-09-1997
 * Created		19-08-1997
*/


#ifdef __GNUG__
#pragma implementation
#endif

#include "emaskout.h"
#include "units.h"

#define inch            2.54

#include "arc.h"
#include "boundary.h"
#include "elemlst.h"
#include "gdsii.h"
#include "path.h"
#include "point.h"
#include "segment.h"
#include "polyline.h"
#include "referenc.h"
#include "structur.h"
#include "text.h"

// constructor with the filename
EMASK_driver_out::EMASK_driver_out(const wxString& _filename, bool only_visible)
{
   _only_visible = only_visible;
	Flashes_Written = 0; // necessary for checksum
   EMASK_out_filename=_filename;
	EMASK_out_file.open(EMASK_out_filename, ios::out);
	//bc = 0;
	error = 0;

	int i;
	for (i=0;i<3;i++)	X[i]=Y[i]=0.0;

	Xpos=Ypos=Width=Height=Angle=0;
	lastXpos=lastYpos=lastWidth=lastHeight=lastAngle=-1;
}

EMASK_driver_out::~EMASK_driver_out()
{
	EMASK_out_file.close();
}

void EMASK_driver_out::WriteHeader()
{  // This header may NOT be modified
	//	if you do then i can't guarantee the
	// proper behavior of the flashin driver
	EMASK_out_file << "<" << endl;
}

void EMASK_driver_out::WriteOuttro() // like an intro but at the end! :)
{
	char _end_job[80];
//	sprintf(_end_job,"ZZ.");
	sprintf(_end_job,"\" END JOB %8i FLASHES \"ZZ.", Flashes_Written);
	EMASK_out_file << _end_job;
}

// write operations for GDSII-dumpfiles

// Arrayreference -----------------------------------------------------------
void EMASK_driver_out::Write(ArrayReference*)// _ArrayReference)
{
	if (!error)
	{
		_messagehandler->info("!No rectangles will not be written to output",
			"emaskout.cpp: wrong elements for EMASK");
		error = true;
	}
}

// Boundary -----------------------------------------------------------------
void EMASK_driver_out::Write(Boundary*)// _Boundary)
{
	if (!error)
	{
		_messagehandler->info("!No rectangles will not be written to output",
			"emaskout.cpp: wrong elements for EMASK");
		error = true;
	}
}

// Box -----------------------------------------------------------------
void EMASK_driver_out::Write(Box* _Box)
{
	if (_Box->GetWidth() != DEFAULT_WIDTH)
	{
		if (!error)
		{
			_messagehandler->info("!No rectangles will not be written to output",
				"emaskout.cpp: wrong elements for EMASK");
			error = true;
		}
	}
	else
	{
   	G_Polygon* orgi = new G_Polygon();
		_Box->Convert(orgi,0);
		Write( orgi);
		ReadFlash();
		WriteFlash();
	   delete orgi;
	}
/* Voorbeeld van het omzetten van een box met width naar flash formaat.
		 verdeling van de originele flash in vier kleinere flashes.
			|---------------|
			|       1       |
			|---------------|
			| 3 |       | 4 |
			|---------------|
			|       2       |
			|---------------|

		ReadFlash(); // originele waarden zetten in de stores.

		// eerste vierkant berekenen.
		// Xpos is gelijk aan die van originele flash.
		Ypos = floor(y + 0.5*h + 0.5);
		Width = floor(w + boxwidth + 0.5);
		Height = floor(boxwidth +  0.5); // kan een width ook niet gehele getallen bevatten
		Angle = rad2deg(a); 	// angle blijft voor alle flashes hetzelfde.
		WriteFlash();

		// tweede vierkant berekenen
		// Xpos is nog steeds gelijk.
		Ypos = floor(y - 0.5 * h + 0.5);
		// rest is hetzelfde
		WriteFlash();

		// derde vierkant bereken.
		Xpos = floor(x - 0.5 * w + 0.5);
		Ypos = floor(y + 0.5);
		Width = floor(boxwidth + 0.5);
		Height = floor(h - 2.0 * boxwidth +0.5);
		WriteFlash();

		// vierde vierkant bereken.
		Xpos = floor(x + 0.5 * w +0.5);
		// rest blijft hetzelfde.
		WriteFlash();*/

}

// Circle -----------------------------------------------------------------
void EMASK_driver_out::Write(Circle*)// _Circle)
{
	if (!error)
	{
		_messagehandler->info("!No rectangles will not be written to output",
			"emaskout.cpp: wrong elements for EMASK");
		error = true;
	}
}

// GdsII --------------------------------------------------------------------
void EMASK_driver_out::Write(GdsII* _GdsII)
{
		WriteHeader();
      TDLI<Structure> I(_GdsII);

      I.toroot();
      while (I.iterate())
         Write( I.item());
		WriteOuttro();
}


// Library ------------------------------------------------------------------
void EMASK_driver_out::Write(Library*)// _Library)
{}

// Path ---------------------------------------------------------------------
void EMASK_driver_out::Write(Path*)// _Path)
{
	if (!error)
	{
		_messagehandler->info("!No rectangles will not be written to output",
			"emaskout.cpp: wrong elements for EMASK");
		error = true;
	}
}

// Arcseg ----------------------------------------------------------------------
void EMASK_driver_out::Write(Arcseg*)// _Arcseg)
{
	if (!error)
	{
		_messagehandler->info("!No rectangles will not be written to output",
			"emaskout.cpp: wrong elements for EMASK");
		error = true;
	}
}

// Point --------------------------------------------------------------------
void EMASK_driver_out::Write(Point*)// a_point)
{}

// Segment  -----------------------------------------------------------------
void EMASK_driver_out::Write(Segment* _Segment)
{
	X[Points_Written] = _Segment->GetX();
	Y[Points_Written] = _Segment->GetY();

	if (_Segment->GetType()==ARC )
		_messagehandler->info("Een Arc in een Box???",
			"flashout.cpp : Vage fout"); // should not occur
}

// Polygon ------------------------------------------------------------------
void EMASK_driver_out::Write(G_Polygon* _Polygon)
{
 	TDLI<Segment> 	Iter=TDLI<Segment>(_Polygon);

	long points;
	points=Iter.count();

	if (points!=4)
	{
		if (!error)
		{
			_messagehandler->info("!No rectangles will not be written to output",
				"emaskout.cpp: wrong elements for EMASK");
			error = true;
		}
	}
	else
	{
		Points_Written = 0;
		Iter.tohead();
		int i;
		for (i = 0;i<3;i++) // eerste drie punten zijn belangrijk.
		{
			Write( Iter.item());
			Points_Written++;
			Iter++;
		}
	}
// end of Write(G_Polygon*..)
}

// Polyline -----------------------------------------------------------------
void EMASK_driver_out::Write(G_Polyline*)// _Polyline)
{}

// StructureReference -------------------------------------------------------
void EMASK_driver_out::Write(StructureReference*)// _StructureReference)
{
	if (!error)
	{
		_messagehandler->info("!No rectangles will not be written to output",
			"emaskout.cpp: wrong elements for EMASK");
		error = true;
	}
}

// Structure ----------------------------------------------------------------
void EMASK_driver_out::Write(Structure* _Structure)
{
	if (_Structure)
		OWrite( (ElementList*)_Structure);
}

// Text ---------------------------------------------------------------------
void EMASK_driver_out::Write(Text*)// _Text)
{
	if (!error)
	{
		_messagehandler->info("!No rectangles will not be written to output",
			"emaskout.cpp: wrong elements for EMASK");
		error = true;
	}
}

void EMASK_driver_out::ReadFlash()
{

   if (X[1] - X[0] == 0)
   {
      if (Y[1]-Y[0] > 0 )
          a=M_PI/2.0;
      else
          a=-M_PI/2.0;
   }
   else
		a = atan2(Y[1]-Y[0],X[1]-X[0]);

	w = sqrt((X[1]-X[0])*(X[1]-X[0]) + (Y[1]-Y[0])*(Y[1]-Y[0]));
	h = sqrt((X[2]-X[1])*(X[2]-X[1]) + (Y[2]-Y[1])*(Y[2]-Y[1]));
	//x = X[0] + 0.5*w*cos(a) + 0.5*h*sin(a);
	//y = Y[0] + 0.5*w*sin(a) + 0.5*h*cos(a);
	x = (X[0]+X[2])/2;
	y = (Y[0]+Y[2])/2;

	// juiste punten zijn uitgerekend. is getest.
   x=x*GLOB->PHYSUNITS()*1e6;
   y=y*GLOB->PHYSUNITS()*1e6;
   w=w*GLOB->PHYSUNITS()*1e6;
   h=h*GLOB->PHYSUNITS()*1e6;

	Xpos = (int)floor(x +  0.5);
	Ypos = (int)floor(y +  0.5);
	Width = (int)floor(w +  0.5);
	Height = (int)floor(h +  0.5);
	Angle = (short)rad2deg(a);
}

void EMASK_driver_out::WriteFlash()
{
   static int i=0;
	char _flash[80];
	char _temp[80];
	strcpy (_flash,"");

	if (Xpos != lastXpos)
		{
			lastXpos = Xpos;
			sprintf(_temp,"X%li",Xpos);
			strcat(_flash,_temp);
		}
	if (Ypos != lastYpos)
		{
			lastYpos = Ypos;
			sprintf(_temp,"Y%li",Ypos);
			strcat(_flash,_temp);
		}
	if (Width != lastWidth)
		{
			lastWidth = Width;
			sprintf(_temp,"W%i",Width);
			strcat(_flash,_temp);
		}
	if (Height != lastHeight)
		{
			lastHeight = Height;
			sprintf(_temp,"V%i",Height);
			strcat(_flash,_temp);
		}
	if (Angle != lastAngle)
		{
			lastAngle = Angle;
			sprintf(_temp,"U%i",Angle);
			strcat(_flash,_temp);
		}
	strcat(_flash,"S");
	EMASK_out_file << _flash << endl;
//	EMASK_out_file << _flash;
	Flashes_Written++;
   i++;
   if (i>770)
   {
	 	EMASK_out_file << ".<" << endl;
      i=0;
   }
}

int EMASK_driver_out::rad2deg(double value)
{
	return (int)(floor(value *10/M_PI*180 +  0.5));
//	return 0;
}

