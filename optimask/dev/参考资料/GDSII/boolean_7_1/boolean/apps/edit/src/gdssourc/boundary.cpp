/*
Program	BOUNDARY.CPP
Purpose	Contains a boundary
Last Update	5-9-1997
*/

#ifdef __GNUG__
#pragma implementation
#endif
#include "boundary.h"
#include "liner.h"
#include "bbox.h"
#include "path.h"
#include "units.h"
#include "proplist.h"
#include "scrndrv.h"

Boundary::Boundary(int layernr)
{
	Init(layernr);
	m_polygon = new G_Polygon();

	if (!m_polygon)
		throw GDS_Error("Out of memory!\n\nCannot create Polygon",
			"Fatal Creation Error", 0, 1);
}

Boundary::Boundary(G_Polygon* polygon, int layernr)
{
	Init(layernr);
	m_polygon = polygon;
}

void Boundary::Init(int layernr)
{
   m_spline   = false;
	m_layer=layernr;
	m_datatype = 0;
	m_width = 0;
}

Boundary::~Boundary()
{
	delete m_polygon;
}

void Boundary::OWrite(Driver_out& dr)
{
   dr.Write(this);
}

void Boundary::DoWrite(DrawDriver& dr)
{
	// There are two cases where a copy of the boundary's polygon should be made,
   // if the polygon is not absolute or if the polygon contains arc's. If one of
   // these condintions is true (or both) a copy should be made to perform operations on.

   dr.SetFilled(GetFilled());
   bool ident = dr.GetMatrix()->IsIdentity();
   G_Polygon* todraw;

	// Convert boundary to polygon with width
	if ((m_width != 0) || m_spline ||(ContainsArcs()) || (! ident))
   {
   	todraw = dr.GetAbsPolygon();
		Convert(todraw, GLOB->DISPLAYABER());
		todraw->Transform(dr.GetMatrix());
   } else {
   	todraw = m_polygon;
   }

	// check if it is selected
	dr.SetSelected(GetSelect());

	// write it's polygon
	dr.Write(todraw);
}

void Boundary::WriteEdit(Driver_out& where)
{
   where.Write(this);
}

void Boundary::WriteMove(Driver_out& where)
{
   where.Write(this);
}


bool Boundary::ContainsArcs()
{
	return m_polygon->ContainsArcs();
}

wxPropertySheet* Boundary::GetPropList()
{
   Shape::GetPropList();
   if (!m_wxpropsheet->HasProperty("spline"))
   {
	   wxProperty* theprop=new wxProperty();
      wxPropertyValue aval(m_spline);
	   wxString tmp = wxString("spline");
      theprop->SetName( tmp );
      theprop->SetValue(aval);
      tmp = wxString("real");
      theprop->SetRole( tmp );
	   theprop->GetValue().SetModified(false);
   	m_wxpropsheet->AddProperty(theprop);
   }
	return m_wxpropsheet;
}

void Boundary::UpdatePropList()
{
   if (!m_wxpropsheet)
      return;
   Shape::UpdatePropList();

   if (m_wxpropsheet->HasProperty("spline"))
   {
	   if (m_wxpropsheet->GetProperty("spline")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
      SetSpline(m_wxpropsheet->GetProperty("spline")->GetValue().BoolValue());
		m_wxpropsheet->RemoveProperty("spline");
   }

	return;
}


// function returns true if there was a succesfull (de)select else false will be returned
bool Boundary::Select(Point* P)
{
	bool		result = false;

	// check if point is within the boundingBox
	if (! m_boundingbox.PointInBox(*P)) return false;

	// only polygons without arc's can be processed by the PointInPolygon function.
	// If a polygon contains one or more arc's a copy of the polygon will be converted
	// to a polygon without arc's and the copy will be checked instead.

	if (ContainsArcs())
   {
		G_Polygon*	copy1 = new G_Polygon;
		*copy1 = *m_polygon;
      {
         copy1->ConvertArcs2(SELECT_DPHI);
         // check if the point is in the polygon
         if (copy1->PointInPolygon(*P,m_width) != OUTSIDE_POLY)
            result = true;
		}
      delete copy1;
	}
   else
   {
		// check if the point is in the polygon
		if (m_polygon->PointInPolygon(*P,m_width) != OUTSIDE_POLY)
			result = true;
	}

	return result;
}

bool Boundary::Select(BoundingBox* area,wxTransformMatrix* m)
{
	Point			min,max,amin,amax;

	// check if the boundingBox of the polygon is in the selected boundingBox
	BoundingBox	backup = m_boundingbox;
	backup.MapBbox(m);

	min = backup.GetMin();
	max = backup.GetMax();
	amin = area->GetMin();
	amax = area->GetMax();

	if ((min.GetX() < amin.GetX()) || (min.GetY() < amin.GetY()) ||
		(max.GetX() > amax.GetX()) || (max.GetY() > amax.GetY())) return false;

	return true;
}

Element* Boundary::Duplicate()
{
		Boundary* a_boundary= new Boundary;
		*a_boundary=*this;

		return a_boundary;
}

Boundary& Boundary::operator=(Boundary &other)
{
   (Shape &)*this=(Shape &)other;

	*m_polygon=*other.m_polygon;
	m_datatype=other.m_datatype;
	m_spline = other.m_spline;
	return *this;
}


BoundingBox& Boundary::CalcBoundingBox()
{
	m_boundingbox.Reset();
	m_boundingbox.SetValid(true);

	m_polygon->CalcBoundingBox(&m_boundingbox);

	if (m_width)
	{
		double _wid2 = m_width*0.75;
		m_boundingbox.EnLarge(_wid2);
   }

	return m_boundingbox;
}

int	Boundary::GetDatatype()
{
	return m_datatype;
}

G_Polygon *Boundary::GetPolygon()
{
	return m_polygon;
}

void Boundary::SetDatatype(int new_value)
{
	m_datatype = new_value;
}

void Boundary::SetPolygon(G_Polygon *new_value)
{
	if (m_polygon)
		delete m_polygon;
	m_polygon = new_value;
}

void Boundary::Transform( wxTransformMatrix* _Matrix)
{
	m_polygon->Transform(_Matrix);
	if (m_width)
	   m_width = _Matrix->Get_scaleX()*m_width;
}

void Boundary::Convert(G_Polygon* _converted, double Aber)
{
	int count;
	double width=m_width/2.0;

	_converted->MakeEmpty();

	*_converted=*m_polygon;

	_converted->ConvertArcs(Aber);
	_converted->RemoveZero();
   if (_converted->count()<2)
   	return;

   if (m_spline)
   {
      _converted->ConvertSplined(Aber);
   }

	if( m_width!=0 )
	{
      if (_converted->count()==2)
      {
			G_Polygon* tmp=new G_Polygon() ;
         *tmp=*_converted;
         {
            Path* a_path = new Path((G_Polyline*) tmp,0);
            a_path->SetWidth(m_width);
            a_path->Convert(_converted,Aber);
			delete a_path; 
         }
      }
      else
      {
	      TDLI<Segment> 	_Ipoly=TDLI<Segment>(_converted);
         // make from each point in the polygon a node
         Point _first;
         Point _middle;
         Point _end;
         Point offsetpointleft;
         Point offsetpointright;

         _Ipoly.tohead();
         Line_Seg* lseg=new Line_Seg((Line_Seg*)_Ipoly.item());
         _Ipoly.insend(lseg);
         _Ipoly.next_wrap();
         lseg=new Line_Seg((Line_Seg*)_Ipoly.item());
         _Ipoly.insend(lseg);
         _Ipoly.next_wrap();
         lseg=new Line_Seg((Line_Seg*)_Ipoly.item());
         _Ipoly.insend(lseg);

         _Ipoly.tohead();
         count=_Ipoly.count()-2;
         while (count)
         {
            _first.Set(_Ipoly.item()->GetX(),_Ipoly.item()->GetY());
            delete _Ipoly.item();
            _Ipoly.remove();
            _middle.Set(_Ipoly.item()->GetX(),_Ipoly.item()->GetY());
            _Ipoly++;
            _end.Set(_Ipoly.item()->GetX(),_Ipoly.item()->GetY());

            wxLine line1(_first,_middle);
            wxLine line2(_middle,_end);
            line1.CalculateLineParameters();
            line2.CalculateLineParameters();

            OUTPRODUCT _outproduct;
            _outproduct= line1.OutProduct(line2,1e-9);

            switch (_outproduct)
            {
               // Line 2 lies on  leftside of this line
               case R_IS_RIGHT :
               {
                  line1.OffsetContour(line2,width,offsetpointleft);
                  line1.OffsetContour(line2,-width,offsetpointright);
               }
               break;
               case R_IS_LEFT :
               {
                  line1.OffsetContour(line2,width,offsetpointleft);
                  line1.OffsetContour(line2,-width,offsetpointright);
               }
               break;
               // Line 2 lies on this line
               case R_IS_ON	 :
               {
                  offsetpointleft=_middle;
                  offsetpointright=_middle;
                  line1.Virtual_Point(offsetpointleft,width);
                  line1.Virtual_Point(offsetpointright,-width);
               }
               break;
            }//end switch

            Line_Seg* lseg=new Line_Seg(offsetpointleft.GetX(),offsetpointleft.GetY());
               _Ipoly.insend(lseg);
            lseg=new Line_Seg(offsetpointright.GetX(),offsetpointright.GetY());
               _Ipoly.insbegin(lseg);
            _Ipoly--;
            count--;
         }
         delete _Ipoly.item();
         _Ipoly.remove();
         delete _Ipoly.item();
         _Ipoly.remove();

      }

	}
}


