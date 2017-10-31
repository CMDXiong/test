/*
Program		DRIVER_OUT.H
Purpose		Virtual base class for output drivers
*/
//! author="klaas"
//! version="1.0"

#ifndef _DRIVER_OUT
#define _DRIVER_OUT

#ifdef __GNUG__
#pragma interface
#endif

#include "wxwin.h"

#include "wx/dynarray.h"
#include <wx/object.h>

//! headerfiles="drivout.h misc.h lsetup.h"
//! codefiles="drivout.cpp"

#include "misc.h"
#include "lsetup.h"

class Element;
class ElementList;
class Arcseg;
class Box;
class Circle;
class SLine;
class ArrayReference;
class Boundary;
class GdsII;
class Library;
class Path;
class Point;
class Segment;
class G_Polygon;
class G_Polyline;
class Reference;
class Strans;
class StructureReference;
class Structure;
class Text;
class Image;

//!base class for all output drivers,
/*! all objects in the internal database
are written to devices and files types from classes derived from this one.
In this manner we get a device independant way of writting output
Object can be streamed in to the apropriate driver.
*/
class Driver_out
{
public:
		virtual ~Driver_out(){};

		virtual void  OWrite(Element*);

		virtual void  OWrite(ElementList* _ElementList);

		virtual void 	Write(Image*) {};

		virtual void 	Write(Boundary*)  {};
		virtual void 	Write(SLine*) {};
		virtual void 	Write(Path*)  {};
		virtual void 	Write(Box*)  {};
		virtual void 	Write(Circle*)  {};
		virtual void 	Write(Arcseg*)  {};
		virtual void 	Write(Text*)  {};
		virtual void 	Write(StructureReference*)  {};
		virtual void 	Write(ArrayReference*)  {};

protected:

      bool _only_visible;
};

#endif //_DRIVER_OUT
