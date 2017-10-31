/*
	Program		GDSDUMP.CPP
	Purpose		Creates an output file from GDS-II in plain text
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "exlout.h"

#include "arc.h"
#include "boundary.h"
#include "elemlst.h"
#include "gdsii.h"
#include "path.h"
#include "segment.h"
#include "polyline.h"
#include "referenc.h"
#include "structur.h"
#include "text.h"
#include <iomanip>
#include "units.h"
#include "proplist.h"

const double INCHorMM = 1.0;

int holeSorter(Element* a, Element* b)
{
	double radiusa = -1;
   double radiusb = 1;

   switch (a->GetType())
   {
      case CIRCLE:
      {
          Circle* a_circle = (Circle*)a;
          if ( a_circle->HasPropList() && a_circle->GetPropList()->HasProperty("hole") )
          {
              radiusa = a_circle->GetRadius()*2;
          }
      }
      break;
      case PATH:
      {
         Path* a_path = (Path*)a;

         TDLI<Segment> Iter=TDLI<Segment>( a_path->GetPolyline() ); // first make iterator
         Iter.tohead();
         if ( Iter.count() == 2 && a_path->HasPropList() && a_path->GetPropList()->HasProperty("hole") )
         {
            Iter++;
            Point secondPoint( Iter.item()->GetX(),Iter.item()->GetY() );
            Iter--;
            double dx = Iter.item()->GetX()-secondPoint.GetX();
            double dy = Iter.item()->GetY()-secondPoint.GetY();
            radiusa = sqrt( dx*dx + dy*dy );
         }
      }
   }
   switch (b->GetType())
   {
      case CIRCLE:
      {
          Circle* a_circle = (Circle*)b;
          if ( a_circle->HasPropList() && a_circle->GetPropList()->HasProperty("hole") )
          {
              radiusb = a_circle->GetRadius()*2;
          }
      }
      break;
      case PATH:
      {
         Path* a_path = (Path*)b;

         TDLI<Segment> Iter=TDLI<Segment>( a_path->GetPolyline() ); // first make iterator
         Iter.tohead();
         if ( Iter.count() == 2 && a_path->HasPropList() && a_path->GetPropList()->HasProperty("hole") )
         {
            Iter++;
            Point secondPoint( Iter.item()->GetX(),Iter.item()->GetY() );
            Iter--;
            double dx = Iter.item()->GetX()-secondPoint.GetX();
            double dy = Iter.item()->GetY()-secondPoint.GetY();
            radiusb = sqrt( dx*dx + dy*dy );
         }
      }
   }


	if(radiusa >  radiusb) return(1);
	if(radiusa == radiusb) return(0);

	return(-1);
}


// constructor with the filename
EXL_driver_out::EXL_driver_out(const wxString& _filename, bool only_visible)
{
   _only_visible = only_visible;
	points_written = 0;
   m_tool_count_for_holes=0;
   EXL_out_filename=_filename;
	EXL_out_file.open(EXL_out_filename, ios::out);
	_AbsPolygon = new G_Polygon();
   _RadiusC=0;

	_matrix = new wxTransformMatrix();
	if (_matrix == 0)
		throw GDS_Error("Cannot allocate memory for a matrix",
			"Screendriver Error", 0, 1);

}

EXL_driver_out::~EXL_driver_out()
{
	EXL_out_file.close();
	if (_AbsPolygon) delete _AbsPolygon;
	delete _matrix;
}

void EXL_driver_out::WriteHeader()
{  //whatever needed
}

void EXL_driver_out::WriteNumber(double big)
{
   if (big<0)
   {
		EXL_out_file << "-";
	   EXL_out_file.width(7);
   	EXL_out_file.fill('0');
      #ifdef __UNIX__
            EXL_out_file << (long)(-big*GLOB->PHYSUNITS()*1e6/INCHorMM + 0.5);  
      #else
            EXL_out_file << setprecision(4) << (long)(-big*GLOB->PHYSUNITS()*1e6/INCHorMM + 0.5); 
      #endif
   } else
   {
	   EXL_out_file.width(7);
   	EXL_out_file.fill('0');
      #ifdef __UNIX__
            EXL_out_file << (long)(big*GLOB->PHYSUNITS()*1e6/INCHorMM + 0.5);  
      #else
            EXL_out_file << setprecision(4) << (long)(big*GLOB->PHYSUNITS()*1e6/INCHorMM + 0.5);  
      #endif
   }
}


// Arrayreference -----------------------------------------------------------
void EXL_driver_out::Write(ArrayReference* _ArrayReference)
{
}

// Boundary -----------------------------------------------------------------
void EXL_driver_out::Write(Boundary* _Boundary)
{
//	EXL_out_file << endl << "BOUNDARY; ";

//	EXL_out_file << "LAYER " << _Boundary->GetLayer() << ";  ";
//	EXL_out_file << "DATATYPE " << _Boundary->GetDatatype() << ";  ";

//	if( _Boundary->GetWidth() != DEFAULT_WIDTH )
//		EXL_out_file << "WIDTH " << _Boundary->GetWidth() << "; ";

	Write( _Boundary->GetPolygon());

//	EXL_out_file << endl << "ENDEL; " << endl;
}

// Box -----------------------------------------------------------------
void EXL_driver_out::Write(Box* _Box)
{
/*
	EXL_out_file << endl << "BOX; ";
	if (_Box->GetElflags())
		*this	<< _Box->GetElflags();

	EXL_out_file << "LAYER " << _Box->GetLayer() << ";  ";
	EXL_out_file << "BOXTYPE " << _Box->GetBoxtype() << ";  ";

	if( _Box->GetWidth() != DEFAULT_WIDTH )
		EXL_out_file << "WIDTH " << _Box->GetWidth() << "; ";
*/
   G_Polygon* orgi = new G_Polygon();
	_Box->Convert(orgi,0);
	Write(orgi);
   delete orgi;

//	EXL_out_file << endl << "ENDEL; " << endl;
}

// Circle -----------------------------------------------------------------
void EXL_driver_out::Write(Circle* _Circle)
{

   bool hole=false;
   if (_Circle->HasPropList())
   {
      if (_Circle->GetPropList()->HasProperty("hole"))
         hole=true;
   }
   if (!hole)
   {
      // Convert circle to polygon
      _Circle->Convert(_AbsPolygon, GLOB->DISPLAYABER());

      _AbsPolygon->Transform(_matrix);

      _AbsPolygon->ConvertArcs(GLOB->DISPLAYABER());

      Write(_AbsPolygon);
   }
   else
   {
      double tmpr =floor((_Circle->GetRadius()*GLOB->PHYSUNITS()*1e6 + 0.01 * 0.5) / 0.01) * 0.01;
      if (tmpr != _RadiusC) //different from the old value
      {
		   m_tool_count_for_holes++;
	      EXL_out_file <<  "T" <<  m_tool_count_for_holes;
	      EXL_out_file << endl;
         _RadiusC=tmpr;
      }
      EXL_out_file <<  "X";
      WriteNumber(_Circle->GetMid().GetX());
      EXL_out_file <<  "Y";
      WriteNumber(_Circle->GetMid().GetY());
      EXL_out_file << endl;
   }
}

// GdsII --------------------------------------------------------------------
void EXL_driver_out::Write(GdsII* _GdsII)
{
   m_tool_count_for_holes=0;

   m_flatcopy= new GdsII();
	*m_flatcopy = *_GdsII;

   m_flatcopy->Flatten(false,true);

   { //scope to get rid of iterator before delete of m_flatcopy
      TDLI<Element> S = TDLI<Element>( m_flatcopy->GetTopStructure() );
      S.tohead();
      while(!S.hitroot())
      {
         switch (S.item()->GetType())
         {
            case CIRCLE:
            case PATH:
               S++;
            break;
            default: 
               delete S.item();
               S.remove();              
            break;
         }
      }

	   S.mergesort( holeSorter );

      Write( m_flatcopy->GetLibrary());

	   EXL_out_file << "M48" << endl;

      double rad=0;
      S.tohead();
      while(!S.hitroot())
      {
         switch (S.item()->GetType())
         {
            case CIRCLE:
            {
               Circle* a_circle = (Circle*)S.item();
               if (!_only_visible || (_only_visible && GLOB->SETUP->Get_Visible(a_circle->GetLayer())))
               {
                   if ( a_circle->HasPropList() && a_circle->GetPropList()->HasProperty("hole") )
                   {
                      double tmpr;
                      tmpr = floor((a_circle->GetRadius()*GLOB->PHYSUNITS()*1e6 + 0.01 * 0.5) / 0.01) * 0.01;

                      if (rad != tmpr)
                      {
                         m_tool_count_for_holes++;
                         rad = tmpr;
                         EXL_out_file << "T" << m_tool_count_for_holes << "C";
                         EXL_out_file.width(7);
                         EXL_out_file.fill('0');
                         #ifdef __UNIX__
                               EXL_out_file << rad*2;
                         #else
                               EXL_out_file << setprecision(4) << rad*2;
                         #endif
                         EXL_out_file << endl;
                      }
                   }
                }
            }
            break;
            case PATH:
            {
               Path* a_path = (Path*)S.item();
               if (!_only_visible || (_only_visible && GLOB->SETUP->Get_Visible(a_path->GetLayer())))
               {
                   TDLI<Segment> Iter=TDLI<Segment>( a_path->GetPolyline() ); // first make iterator
                   Iter.tohead();
                   if ( Iter.count() == 2 && 
                        a_path->HasPropList() && 
                        a_path->GetPropList()->HasProperty("hole") 
                      )
                   {
                      Iter++;
                      Point secondPoint( Iter.item()->GetX(),Iter.item()->GetY() );
                      Iter--;
                      double dx = Iter.item()->GetX()-secondPoint.GetX();
                      double dy = Iter.item()->GetY()-secondPoint.GetY();
                      double radiusHole = sqrt( dx*dx + dy*dy )/2;

                      double tmpr = floor((radiusHole*GLOB->PHYSUNITS()*1e6 + 0.01 * 0.5) / 0.01) * 0.01;

                      if (rad != tmpr)
                      {
                         m_tool_count_for_holes++;
                         rad=tmpr;
                         EXL_out_file << "T" << m_tool_count_for_holes << "C";
                         EXL_out_file.width(7);
                         EXL_out_file.fill('0');
                         #ifdef __UNIX__
                               EXL_out_file << rad*2;
                         #else
                               EXL_out_file << setprecision(4) << rad*2;
                         #endif
                         EXL_out_file << endl;
                      }
                   }
                }
            }
            break;
            default: 
            break;
         }
         S++;
      }

	   EXL_out_file << "%" << endl;
	   EXL_out_file << "G90" << endl;

      m_tool_count_for_holes=0;
      {
         TDLI<Structure> I(m_flatcopy);

         I.toroot();
         while (I.iterate())
            Write( I.item());
      }
	   EXL_out_file << "M00" << endl;
   }

   delete m_flatcopy;
}

// Library ------------------------------------------------------------------
void EXL_driver_out::Write(Library* _Library)
{
//	EXL_out_file << "LIBNAME " << _Library->GetLibraryName() << "; " << endl;
//	*this	<< _Library->GetUnits();
}

// Path ---------------------------------------------------------------------
void EXL_driver_out::Write(Path* _Path)
{
   TDLI<Segment> 	Iter=TDLI<Segment>(_Path->GetPolyline()); // first make iterator

   bool hole=false;
   if ( Iter.count() == 2 && 
        _Path->HasPropList() && 
        _Path->GetPropList()->HasProperty("hole") 
      )
         hole=true;

   if (!hole)
   {
   	Write(_Path->GetPolyline());
   }
   else
   {
      Iter.tohead();
      Iter++;
      Point secondPoint( Iter.item()->GetX(),Iter.item()->GetY() );
      Iter--;
      double dx = Iter.item()->GetX()-secondPoint.GetX();
      double dy = Iter.item()->GetY()-secondPoint.GetY();
      double radiusHole = sqrt( dx*dx + dy*dy ) / 2.0;

      double tmpr =floor((radiusHole*GLOB->PHYSUNITS()*1e6 + 0.01 * 0.5) / 0.01) * 0.01;
      if (tmpr != _RadiusC) //different from the old value
      {
		   m_tool_count_for_holes++;
	      EXL_out_file <<  "T" <<  m_tool_count_for_holes;
	      EXL_out_file << endl;
         _RadiusC=tmpr;
      }
      //center of circle
      dx = (Iter.item()->GetX()+secondPoint.GetX())/2.0;
      dy = (Iter.item()->GetY()+secondPoint.GetY())/2.0;
      EXL_out_file <<  "X";
      WriteNumber( dx );
      EXL_out_file <<  "Y";
      WriteNumber( dy );
      EXL_out_file << endl;
   }
}

// Arcseg ----------------------------------------------------------------------
void EXL_driver_out::Write(Arcseg* _Arcseg)
{
//	EXL_out_file  << endl << "ARC; ";

//	EXL_out_file << "LAYER " << _Arcseg->GetLayer() << "; ";
//	EXL_out_file << "DATATYPE " << _Arcseg->GetDatatype() << "; ";

//	if( _Arcseg->GetWidth() != DEFAULT_WIDTH )
//		EXL_out_file << "WIDTH " << _Arcseg->GetWidth() << "; ";

	Write( _Arcseg->GetPolyline());

//	EXL_out_file << endl << "ENDEL; " << endl;
}

// Point --------------------------------------------------------------------
void EXL_driver_out::Write(Point *a_point)
{
	points_written++;

   if (_first)
   {
		EXL_out_file <<  "M16" << endl;
		EXL_out_file <<  "G00X";
   }
   else
		EXL_out_file <<  "G01X";
	WriteNumber(a_point->GetX());
   EXL_out_file << "Y";
	WriteNumber(a_point->GetY());
	EXL_out_file << endl;
   if (_first)
		EXL_out_file <<  "M15" << endl;


}

// Segment  -----------------------------------------------------------------
void EXL_driver_out::Write(Segment *_Segment)
{
	points_written++;

   if (_first)
   {
		EXL_out_file <<  "M16" << endl;
		EXL_out_file <<  "G00X";
   }
   else
		EXL_out_file <<  "G01X";
	WriteNumber(_Segment->GetX());
   EXL_out_file <<"Y";
   WriteNumber(_Segment->GetY());
   EXL_out_file << endl;
   if (_first)
		EXL_out_file <<  "M15" << endl;

	if (_Segment->GetType()==ARC )
	{
		points_written++;
		Arc_Seg* aseg=(Arc_Seg*) _Segment;
      //middle gives direction
      if (_midrad < _endrad)
			EXL_out_file <<  "G03X"; //counterclockwise
      else
			EXL_out_file <<  "G02X"; //Clockwise
		WriteNumber(_next.GetX());
		EXL_out_file << "Y";
		WriteNumber(_next.GetY());
		EXL_out_file << "I";

		WriteNumber(_mid.GetX()-aseg->GetX());
		EXL_out_file << "J";
		WriteNumber(_mid.GetY()-aseg->GetY());
		EXL_out_file << endl;
	}

}

// Polygon ------------------------------------------------------------------
void EXL_driver_out::Write(G_Polygon* _Polygon)
{
   TDLI<Segment> 	Iter=TDLI<Segment>(_Polygon); // first make iterator


	//EXL_out_file  << endl << "   XY " << (points+1) << endl;

   _first=true;
	points_written = 0;
	Iter.tohead();
	while (!Iter.hitroot())
	{
		if (Iter.item()->GetType()==ARC )
      {
         Iter.next_wrap();
         _next.SetX(Iter.item()->GetX());
         _next.SetY(Iter.item()->GetY());
         Iter.prev_wrap();
         //calculate temp data to be able to write arc segment correctly
			_Polygon->CalcArc(&Iter,_mid,_Radius,_beginrad,_midrad,_endrad,_phit);
      }
		Write(Iter.item());
	   _first=false;
		Iter++;
	}
	Iter.tohead();

	Point end(Iter.item()->GetX(),Iter.item()->GetY());

	// write the first point again to close the polygon
	Write(&end);
}

// Polyline -----------------------------------------------------------------
void EXL_driver_out::Write(G_Polyline* _Polyline)
{
   TDLI<Segment> 	Iter=TDLI<Segment>(_Polyline); // first make iterator

   _first=true;
	points_written = 0;
	Iter.tohead();
	while (!Iter.hitroot())
	{
		if (Iter.item()->GetType()==ARC )
      {
         Iter.next_wrap();
         _next.SetX(Iter.item()->GetX());
         _next.SetY(Iter.item()->GetY());
         Iter.prev_wrap();
         //calculate temp data to be able to write arc segment correctly
			_Polyline->CalcArc(&Iter,_mid,_Radius,_beginrad,_midrad,_endrad,_phit);
      }
		Write(Iter.item());
	   _first=false;
		Iter++;
	}
}

// StructureReference -------------------------------------------------------
void EXL_driver_out::Write(StructureReference* _StructureReference)
{
}

// Structure ----------------------------------------------------------------
void EXL_driver_out::Write(Structure* _Structure)
{

//	EXL_out_file << "STRNAME " << _Structure->GetStructureName() << "; " << endl;

	if (_Structure)
		OWrite((ElementList*) _Structure);

//	EXL_out_file << endl << "ENDSTR " << _Structure->GetStructureName() << "; " << endl;
}

// Text ---------------------------------------------------------------------
void EXL_driver_out::Write(Text* _Text)
{
}




