/*
Program	Path
Purpose	Definition of GDSII path structure
Programmers	R. Spekreijse & N. Noorlander
Last Update	07-12-1995
*/
#ifdef __GNUG__
#pragma implementation
#endif

#include "path.h"
#include "liner.h"
#include "lsetup.h"
#include "units.h"
#include "proplist.h"
#include "drivout.h"
#include "scrndrv.h"

// Constructors & destructor
Path::Path(G_Polyline* polyline, int layernr)
{
	Init(layernr);
	m_polyline = polyline;
	if (!m_polyline)
		throw GDS_Error("Out of Memory.\n\nCould not create Polyline in object Path.",
						"Fatal Creation Error", 0, 1);
}


Path::Path(int layernr)
{
	Init(layernr);
	m_polyline = new G_Polyline();
	if (!m_polyline)
		throw GDS_Error("Out of Memory.\n\nCould not create Polyline in object Path.",
						"Fatal Creation Error", 0, 1);
}


void Path::Init(int layernr)
{
   m_spline   = false;
	m_layer    = layernr;
	m_width    = 0;
	m_pathtype = PATH_END_SQAURE;
	m_datatype = 0;
}

Path::~Path()
{
	delete m_polyline;
}

void Path::OWrite(Driver_out& dr)
{
   dr.Write(this);
}

void Path::DoWrite(DrawDriver& dr)
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
   	todraw = (G_Polygon*)m_polyline;
   }

	// check if it is selected
	dr.SetSelected(GetSelect());

	// write it's polygon

	// draw its polyline
	if (m_width)
   	dr.Write(todraw);
	else
   	dr.Write((G_Polyline*)todraw);
}



bool Path::ContainsArcs()
{
	return m_polyline->ContainsArcs();
}

BoundingBox& Path::CalcBoundingBox()
{
	m_boundingbox.Reset();
	m_boundingbox.SetValid(true);

	m_polyline->CalcBoundingBox(&m_boundingbox);
	if (m_width)
	{
		double _wid2 = m_width*0.75;
		m_boundingbox.EnLarge(_wid2);
	}

	return m_boundingbox;
}

Path& Path::operator=(Path& a_path)
{
   (Shape &)*this=(Shape &)a_path;

	// copy the required members
	m_datatype = a_path.m_datatype;
	*m_polyline = *a_path.m_polyline;
	m_spline = a_path.m_spline;

	m_pathtype =a_path.m_pathtype;

	return *this;
}

bool Path::Select(Point* P)
{
	bool  result=false, containsarcs;

	containsarcs=m_polyline->ContainsArcs();

	if (containsarcs)
   {
		G_Polyline*	copy1 = new G_Polyline;
		*copy1 = *m_polyline;
      copy1->ConvertArcs2(SELECT_DPHI);
      if (m_spline)
      {
   		copy1->ConvertSplined(GLOB->DISPLAYABER());
      }
      // check if the point is on the polyline
   	if (copy1->PointOnPolyline(P,(m_width==0?GLOB->SELECT_SNAPFACTOR():m_width)))
            result = true;
      delete copy1;
   }
   else
   {
      if (m_spline)
      {
   		G_Polyline*	copy1 = new G_Polyline;
	   	*copy1 = *m_polyline;
		   copy1->ConvertSplined(GLOB->DISPLAYABER());
         // check if the point is on the polyline
         if (copy1->PointOnPolyline(P,(m_width==0?GLOB->SELECT_SNAPFACTOR():m_width)))
            result = true;
         delete copy1;
      }
      else
      {
         // check if the point is on the polyline
         if (m_polyline->PointOnPolyline(P,(m_width==0?GLOB->SELECT_SNAPFACTOR():m_width)))
            result = true;
      }
	}

	return result;
}

bool Path::Select(BoundingBox* area,wxTransformMatrix* m)
{
	Point			min,max,amin,amax;

	// check if the boundingBox of the polygon is in the selected boundingBox
	BoundingBox	backup = CalcBoundingBox();
	backup.MapBbox(m);

	min = backup.GetMin();
	max = backup.GetMax();
	amin = area->GetMin();
	amax = area->GetMax();

	if ((min.GetX() < amin.GetX()) || (min.GetY() < amin.GetY()) ||
		(max.GetX() > amax.GetX()) || (max.GetY() > amax.GetY())) return false;

	return true;
}

void Path::SetPathtype(PATHTYPE p)
{
	m_pathtype = p;
}

PATHTYPE Path::GetPathtype()
{
	return m_pathtype;
}

void Path::SetDatatype(int d)
{
	m_datatype = d;
}

int Path::GetDatatype()
{
	return m_datatype;
}

wxPropertySheet* Path::GetPropList()
{
   Shape::GetPropList();
   if (!m_wxpropsheet->HasProperty("pathtype"))
   {
      long pathty=m_pathtype;
      wxStringList *strings2 = new wxStringList("sqaure", "round", "square extended",NULL);

      wxProperty* theprop=new wxProperty("pathtype", strings2->Item(pathty)->GetData(), "string",new wxStringListValidator(strings2));
	   theprop->GetValue().SetModified(false);
   	m_wxpropsheet->AddProperty(theprop);
   }
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

void Path::UpdatePropList()
{
   if (!m_wxpropsheet)
      return;
   Shape::UpdatePropList();
   if (m_wxpropsheet->HasProperty("pathtype"))
   {
	   if (m_wxpropsheet->GetProperty("pathtype")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
      wxString asstr=m_wxpropsheet->GetProperty("pathtype")->GetValue().StringValue();
      if (asstr=="sqaure")
	      m_pathtype=(PATHTYPE)0;
      else if (asstr=="round")
	      m_pathtype=(PATHTYPE)1;
      else
	      m_pathtype=(PATHTYPE)2;
		m_wxpropsheet->RemoveProperty("pathtype");
   }
   if (m_wxpropsheet->HasProperty("spline"))
   {
	   if (m_wxpropsheet->GetProperty("spline")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
      SetSpline(m_wxpropsheet->GetProperty("spline")->GetValue().BoolValue());
		m_wxpropsheet->RemoveProperty("spline");
   }

	return;
}

void Path::SetPolyline(G_Polyline* p)
{
	m_polyline = p;
}

G_Polyline*	Path::GetPolyline()
{
	return m_polyline;
}

void Path::Transform(wxTransformMatrix* _Matrix)
{
	m_polyline->Transform(_Matrix);
  	if (m_width)
	   m_width = _Matrix->Get_scaleX()*m_width;

}

Element* Path::Duplicate()
{
		Path* a_path= new Path;
		*a_path=*this;

		return a_path;
}

void Path::Convert(G_Polygon* _converted, double Aber)
{
	int count;
	double width=m_width/2.0;

	_converted->MakeEmpty();


	*_converted=*((G_Polygon*)m_polyline);

	_converted->ConvertArcs(Aber);
   //call polyline removezero else to much can be removed
	((G_Polyline*)_converted)->RemoveZero();
//   if (_converted->count()<2)
//   	return;

   if (m_spline)
   {
      ((G_Polyline*)_converted)->ConvertSplined(Aber);
   }

	if( m_width!=0 )
	{  if (_converted->count()==1)
      {
         TDLI<Segment> 	conv=TDLI<Segment>(_converted);
         conv.tohead();
         switch (m_pathtype)
         {
            case PATH_END_SQAURE:
                  {
                  }
                  break;
            case PATH_END_ROUND:
                  {
                     // Inner Circle, upper arc
                     Arc_Seg* aseg=new Arc_Seg();
                     aseg->SetX(conv.item()->GetX()-width);
                     aseg->SetY(conv.item()->GetY());
                     aseg->SetAX(conv.item()->GetX());
                     aseg->SetAY(conv.item()->GetY()+width);
                     aseg->SetOX(conv.item()->GetX());
                     aseg->SetOY(conv.item()->GetY());
                     conv.insend(aseg);

                     // Inner Circle, lower arc
                     aseg=new Arc_Seg();
                     aseg->SetX(conv.item()->GetX()+width);
                     aseg->SetY(conv.item()->GetY());
                     aseg->SetAX(conv.item()->GetX());
                     aseg->SetAY(conv.item()->GetY()-width);
                     aseg->SetOX(conv.item()->GetX());
                     aseg->SetOY(conv.item()->GetY());
                     conv.insend(aseg);
                     conv.removehead();
                  }
            case PATH_END_SQAURE_EXT:
                  {
                     break;
                  }
        }
      }
      else
      {
         Point _first;
         Point _middle;
         Point _end;
         Point offsetpointleft;
         Point offsetpointright;

         TDLI<Segment> 	conv=TDLI<Segment>(_converted);
         count=conv.count()-2;
         conv.tohead();
         //first begin point
         _first.Set(conv.item()->GetX(),conv.item()->GetY());
         conv++;
         _middle.Set(conv.item()->GetX(),conv.item()->GetY());
         wxLine line1(_first,_middle);
         line1.CalculateLineParameters();
         offsetpointleft=_first;
         offsetpointright=_first;
         line1.Virtual_Point(offsetpointleft,width);
         line1.Virtual_Point(offsetpointright,-width);

         Line_Seg* lseg;

         switch (m_pathtype)
         {
            case PATH_END_SQAURE:
                  {
                     lseg=new Line_Seg(offsetpointleft.GetX(),offsetpointleft.GetY());
                     conv.insend(lseg);
                     lseg=new Line_Seg(offsetpointright.GetX(),offsetpointright.GetY());
                     conv.insbegin(lseg);
                  }
                  break;
            case PATH_END_ROUND:
                  {
                     wxLine  lineoffset(offsetpointright,offsetpointleft);
                     lineoffset.CalculateLineParameters();
                     Point offsetpointfirst;

                     offsetpointfirst=_first;
                     lineoffset.Virtual_Point(offsetpointfirst,width);
                     lseg=new Line_Seg(offsetpointleft.GetX(),offsetpointleft.GetY());
                     conv.insend(lseg);
                     Arc_Seg* aseg=new Arc_Seg(offsetpointright.GetX(),offsetpointright.GetY(),
                     								  offsetpointfirst.GetX(),offsetpointfirst.GetY(),
                                               _first.GetX(),_first.GetY()      );
                     conv.insbegin(aseg);
                     break;
                  }
            case PATH_END_SQAURE_EXT:
                  {
                     wxLine  lineoffset(offsetpointright,offsetpointleft);
                     lineoffset.CalculateLineParameters();
                     lineoffset.Virtual_Point(offsetpointleft,width);
                     lineoffset.Virtual_Point(offsetpointright,width);
                     Line_Seg* lseg=new Line_Seg(offsetpointleft.GetX(),offsetpointleft.GetY());
                     conv.insend(lseg);
                     lseg=new Line_Seg(offsetpointright.GetX(),offsetpointright.GetY());
                     conv.insbegin(lseg);
                     break;
                  }
        }

         conv--;

         while (count)
         {
            _first.Set(conv.item()->GetX(),conv.item()->GetY());
            delete conv.item();
            conv.remove();
            _middle.Set(conv.item()->GetX(),conv.item()->GetY());
            conv++;
            _end.Set(conv.item()->GetX(),conv.item()->GetY());
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

            lseg=new Line_Seg(offsetpointleft.GetX(),offsetpointleft.GetY());
            conv.insend(lseg);
            lseg=new Line_Seg(offsetpointright.GetX(),offsetpointright.GetY());
            conv.insbegin(lseg);
            conv--;
            count--;
         }

         //end point
         _first.Set(conv.item()->GetX(),conv.item()->GetY());
         delete conv.item();
         conv.remove();
         _middle.Set(conv.item()->GetX(),conv.item()->GetY());
         delete conv.item();
         conv.remove();
         wxLine line3(_first,_middle);
         line3.CalculateLineParameters();

         offsetpointleft=_middle;
         offsetpointright=_middle;
         line3.Virtual_Point(offsetpointleft,width);
         line3.Virtual_Point(offsetpointright,-width);

         switch (m_pathtype)
         {
            case PATH_END_SQAURE:
                  {
                     lseg=new Line_Seg(offsetpointleft.GetX(),offsetpointleft.GetY());
                     conv.insend(lseg);
                     lseg=new Line_Seg(offsetpointright.GetX(),offsetpointright.GetY());
                     conv.insbegin(lseg);
                  }
                  break;
            case PATH_END_ROUND:
                  {
                     wxLine  lineoffset(offsetpointleft,offsetpointright);
                     lineoffset.CalculateLineParameters();
                     Point offsetpointlast;

                     offsetpointlast=_middle;
                     lineoffset.Virtual_Point(offsetpointlast,width);
                     lseg=new Line_Seg(offsetpointleft.GetX(),offsetpointleft.GetY());
                     conv.insend(lseg);
                     Arc_Seg* aseg=new Arc_Seg(offsetpointleft.GetX(),offsetpointleft.GetY(),
                     								  offsetpointlast.GetX(),offsetpointlast.GetY(),
                                               _middle.GetX(),_middle.GetY()      );
                     conv.insend(aseg);
                     lseg=new Line_Seg(offsetpointright.GetX(),offsetpointright.GetY());
                     conv.insbegin(lseg);
                     break;
                  }
            case PATH_END_SQAURE_EXT:
                  {
                     wxLine  lineoffset(offsetpointleft,offsetpointright);
                     lineoffset.CalculateLineParameters();
                     lineoffset.Virtual_Point(offsetpointleft,width);
                     lineoffset.Virtual_Point(offsetpointright,width);
                     Line_Seg* lseg=new Line_Seg(offsetpointleft.GetX(),offsetpointleft.GetY());
                     conv.insend(lseg);
                     lseg=new Line_Seg(offsetpointright.GetX(),offsetpointright.GetY());
                     conv.insbegin(lseg);
                     break;
                  }
         }

      }
      //convert the inserted arc segments to polygon also
      if(m_pathtype==PATH_END_ROUND)
			_converted->ConvertArcs(Aber);
	}
}

