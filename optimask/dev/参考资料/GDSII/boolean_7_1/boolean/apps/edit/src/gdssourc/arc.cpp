/*
	Program	ARC.CPP
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "arc.h"
#include "liner.h"
#include "boundary.h"
#include "structur.h"
#include "lsetup.h"
#include "units.h"
#include "proplist.h"
#include "drivout.h"
#include "scrndrv.h"

// Arc ======================================================================
Arcseg::Arcseg(int layernr)
{
	m_layer=layernr;
	_datatype = 0;
	m_width = 0;

	_polyline = new G_Polyline();
	if (!_polyline)
		throw GDS_Error("Out of memory!\n\nCannot create Polyline",
			"Fatal Creation Error", 0, 1);
}

Arcseg::~Arcseg()
{
	delete _polyline;
}

void Arcseg::OWrite(Driver_out& dr)
{
   dr.Write(this);
}

void Arcseg::DoWrite(DrawDriver& dr)
{
   dr.SetFilled(GetFilled());

 	G_Polygon* todraw = dr.GetAbsPolygon();

	// Convert arcseg to polygon
	Convert(todraw, GLOB->DISPLAYABER());

   todraw->Transform(dr.GetMatrix());

	dr.SetSelected(GetSelect());

	todraw->ConvertArcs(GLOB->DISPLAYABER());

 	dr.Write(todraw);
}



Element* Arcseg::Duplicate()
{
		Arcseg* a_arcseg= new Arcseg;
		*a_arcseg=*this;

		return a_arcseg;
}

Arcseg& Arcseg::operator=(Arcseg &other)
{
   (Shape &)*this=(Shape &)other;
	*_polyline=*other._polyline;
	_datatype = other._datatype;

	return *this;
}

wxPropertySheet* Arcseg::GetPropList()
{
   Shape::GetPropList();
	return m_wxpropsheet;
}

void Arcseg::UpdatePropList()
{
   if (!m_wxpropsheet)
      return;
   Shape::UpdatePropList();
	return;
}

bool Arcseg::Select(Point* P)
{
   bool result=false;

	if (_polyline->PointOnArcseg(P,m_width))
   	result=true;

	return result;
}

bool Arcseg::Select(BoundingBox* area,wxTransformMatrix* m)
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

int	Arcseg::GetDatatype()
{
	return _datatype;
}

void Arcseg::SetDatatype(int new_value)
{
	_datatype = new_value;
}

BoundingBox& Arcseg::CalcBoundingBox()
{

	m_boundingbox.Reset();
	m_boundingbox.SetValid(true);

	_polyline->CalcBoundingBox(&m_boundingbox);

	if (m_width)
	{
		double _wid2 = m_width*0.75;
		double xmin, ymin, xmax, ymax;

		xmin=m_boundingbox.GetMin().GetX()-_wid2;
		ymin=m_boundingbox.GetMin().GetY()-_wid2;
		xmax=m_boundingbox.GetMax().GetX()+_wid2;
		ymax=m_boundingbox.GetMax().GetY()+_wid2;

		m_boundingbox.SetMin(xmin,ymin);
		m_boundingbox.SetMax(xmax,ymax);
	}
	return m_boundingbox;
}

G_Polyline *Arcseg::GetPolyline()
{
	return _polyline;
}

void Arcseg::SetPolyline(G_Polyline *new_value)
{
	if (_polyline)
		delete _polyline;
	_polyline = new_value;
}

void Arcseg::Transform(wxTransformMatrix* _Matrix)
{
	_polyline->Transform(_Matrix);
	if (m_width)
	   m_width =  _Matrix->Get_scaleX()*m_width;
}

void Arcseg::Convert(G_Polygon* _converted, double)
{
   _converted->MakeEmpty();

   TDLI<Segment> 	org=TDLI<Segment>(_polyline);

	Point Mid;
	double r,x,y,ax,ay,ox,oy,wd2;
	double beginrad,endrad,midrad,phit;
	Arc_Seg* aseg;
	Line_Seg* lseg;

	wd2=m_width/2.0;

	org.tohead();
	_polyline->CalcArc(&org,Mid,r,beginrad,midrad,endrad,phit);

	r=r+wd2;

	ox=Mid.GetX();
	oy=Mid.GetY();

	x=ox+r*cos(beginrad);
	y=oy+r*sin(beginrad);
	ax=ox+r*cos(midrad);
	ay=oy+r*sin(midrad);
	aseg = new Arc_Seg(x,y,ax,ay,ox,oy);
	_converted->insend(aseg);

	x=Mid.GetX()+r*cos(endrad);
	y=Mid.GetY()+r*sin(endrad);
	lseg = new Line_Seg(x,y);
	_converted->insend(lseg);

	r=r-wd2-wd2;

	x=Mid.GetX()+r*cos(endrad);
	y=Mid.GetY()+r*sin(endrad);
	ax=Mid.GetX()+r*cos(midrad);
	ay=Mid.GetY()+r*sin(midrad);
	aseg = new Arc_Seg(x,y,ax,ay,ox,oy);
	_converted->insend(aseg);

	x=Mid.GetX()+r*cos(beginrad);
	y=Mid.GetY()+r*sin(beginrad);
	lseg = new Line_Seg(x,y);
	_converted->insend(lseg);
}


// Box ======================================================================
Box::Box(int layernr)
{
	m_layer=layernr;
	_boxtype = 0;
	m_width = 0;
   _w = 0;
   _h = 0;
}

Box::~Box()
{
}

void Box::OWrite(Driver_out& dr)
{
   dr.Write(this);
}

void Box::DoWrite(DrawDriver& dr)
{
   dr.SetFilled(GetFilled());

 	G_Polygon* todraw = dr.GetAbsPolygon();

	// Convert arcseg to polygon
	Convert(todraw, GLOB->DISPLAYABER());

   todraw->Transform(dr.GetMatrix());

	dr.SetSelected(GetSelect());

 	dr.Write(todraw);
}


void Box::SetRelativeMatrix(wxTransformMatrix* m)
{
	_relative_matrix = *m;
}

wxTransformMatrix* Box::GetRelativeMatrix()
{
	return &_relative_matrix;
}


double Box::GetH()
{
	return _h;
}

double Box::GetW()
{
	return _w;
}

void Box::SetH(double h)
{
	_h=h;
}

void Box::SetW(double w)
{
	_w=w;
}

Element* Box::Duplicate()
{
		Box* a_box= new Box;
		*a_box=*this;

		return a_box;
}

Box& Box::operator=(Box &other)
{
   (Shape &)*this=(Shape &)other;
	_w=other._w;
	_h=other._h;
	_boxtype=other._boxtype;
   _relative_matrix = other._relative_matrix;

	return *this;
}

wxPropertySheet* Box::GetPropList()
{
   Shape::GetPropList();
	return m_wxpropsheet;
}

void Box::UpdatePropList()
{
   if (!m_wxpropsheet)
      return;
   Shape::UpdatePropList();
	return;
}


BoundingBox& Box::CalcBoundingBox()
{
	m_boundingbox.Reset();
	m_boundingbox.SetValid(true);
	m_boundingbox.SetMin(-m_width/2.0,-m_width/2.0);
	m_boundingbox.SetMax(_w+m_width/2.0,_h+m_width/2.0);

	m_boundingbox.MapBbox(&_relative_matrix);
	return m_boundingbox;
}

int	Box::GetBoxtype()
{
	return _boxtype;
}

void Box::SetBoxtype(int new_value)
{
	_boxtype = new_value;
}

void Box::Set(G_Polygon* a_rec_poly)
{
   double	X[3];	double	Y[3];   // to store points(x,y)
   int Points_Written = 0;
 	TDLI<Segment> 	Iter=TDLI<Segment>(a_rec_poly);
   Iter.tohead();
   int i;
   for (i = 0;i<3;i++) // first three points.
   {
      X[Points_Written] = Iter.item()->GetX();
      Y[Points_Written] = Iter.item()->GetY();
      Points_Written++;
      Iter++;
   }

   double 	a;

   if (X[1] - X[0] == 0)
   {
      if (Y[1]-Y[0] > 0 )
          a=M_PI/2.0;
      else
          a=-M_PI/2.0;
   }
   else
      a = atan2(Y[1]-Y[0],X[1]-X[0]);

   _w = sqrt((X[1]-X[0])*(X[1]-X[0]) + (Y[1]-Y[0])*(Y[1]-Y[0]));
   _h = sqrt((X[2]-X[1])*(X[2]-X[1]) + (Y[2]-Y[1])*(Y[2]-Y[1]));
   _relative_matrix.Scale(1,1,0,0);
   _relative_matrix.Rotate(a/M_PI*180.0,0,0);
   _relative_matrix.Translate(X[0],Y[0]);
}

void Box::Set(double x_centre,double y_centre,double ang_rad, double w, double h)
{

   _w = w;
   _h = h;

   wxTransformMatrix translate;

	translate.SetValue(2,0,- w/2.0);
	translate.SetValue(2,1,- h/2.0);
	_relative_matrix = translate * _relative_matrix;
	translate.Identity();

	// now rotate
	double cos_rotation = cos(ang_rad);
	double sin_rotation = sin(ang_rad);

	translate.SetValue(0,0,cos_rotation);
	translate.SetValue(0,1,sin_rotation);

	translate.SetValue(1,0,-sin_rotation);
	translate.SetValue(1,1,cos_rotation);

	translate.SetValue(2,0,x_centre);
	translate.SetValue(2,1,y_centre);

	_relative_matrix = translate * _relative_matrix;
}

bool Box::Select(BoundingBox* area, wxTransformMatrix* m)
{
	// check if the boundingBox of the polygon is in the selected boundingBox
	BoundingBox	*backup = new BoundingBox(CalcBoundingBox());
	backup->MapBbox(m);

   Point min,max,amin,amax;
	min = backup->GetMin();
	max = backup->GetMax();
	amin = area->GetMin();
	amax = area->GetMax();

	delete backup;

	if ((min.GetX() < amin.GetX()) || (min.GetY() < amin.GetY()) ||
		(max.GetX() > amax.GetX()) || (max.GetY() > amax.GetY())) return false;

	return true;
}

bool Box::Select(Point* P)
{
	if (CalcBoundingBox().PointInBox(*P))
		return true;

	return false;
}

void Box::Transform(wxTransformMatrix* _Matrix)
{

   _relative_matrix = *_Matrix * _relative_matrix;

	if (m_width)
	   m_width =  _Matrix->Get_scaleX()*m_width;
}

void Box::Convert(G_Polygon* _converted, double)
{
	_converted->MakeEmpty();
   Line_Seg* lseg=new Line_Seg(0,0);
   _converted->insend(lseg);
   lseg=new Line_Seg(_w,0);
   _converted->insend(lseg);
   lseg=new Line_Seg(_w,_h);
   _converted->insend(lseg);
   lseg=new Line_Seg(0,_h);
   _converted->insend(lseg);
   _converted->Transform(&_relative_matrix);

   int count;
   double width=m_width/2.0;

   //convert to polygon with a width if it has a width
   if( m_width!=0 )
   {
      TDLI<Segment> 	conv=TDLI<Segment>(_converted);
      // make from each point in the polygon a node
      Point _first;
      Point _middle;
      Point _end;
      Point offsetpointleft;
      Point offsetpointright;
      Point offsetpoint;

      conv.tohead();
      Line_Seg* lseg=new Line_Seg((Line_Seg*)conv.item());
      conv.insend(lseg);
      conv.next_wrap();
      lseg=new Line_Seg((Line_Seg*)conv.item());
      conv.insend(lseg);
      conv.next_wrap();
      lseg=new Line_Seg((Line_Seg*)conv.item());
      conv.insend(lseg);
      conv.tohead();
      count=_converted->count()-2;
      while (count)
      {
         _first.Set(conv.item()->GetX(),conv.item()->GetY());
         conv.next_wrap();
         _middle.Set(conv.item()->GetX(),conv.item()->GetY());
         conv.next_wrap();
         _end.Set(conv.item()->GetX(),conv.item()->GetY());

         wxLine line1(_first,_middle);
         wxLine line2(_middle,_end);
         line1.CalculateLineParameters();
         line2.CalculateLineParameters();

         line1.OffsetContour(line2,width,offsetpoint);
         lseg=new Line_Seg(offsetpoint.GetX(),offsetpoint.GetY());
         line1.OffsetContour(line2,-width,offsetpoint);
         conv.insbegin(lseg);
         lseg=new Line_Seg(offsetpoint.GetX(),offsetpoint.GetY());
         conv.insend(lseg);
         conv.prev_wrap();
         conv.prev_wrap();
         delete conv.item();
         conv.remove();
         count--;
      }
      delete conv.item();
      conv.remove();
      delete conv.item();
      conv.remove();

   }
}

// Circle ===================================================================
Circle::Circle(int layernr)
{
	m_layer=layernr;
	_datatype = 0;
	m_width = 0;
}

Circle::~Circle()
{
}

void Circle::OWrite(Driver_out& dr)
{
   dr.Write(this);
}

void Circle::DoWrite(DrawDriver& dr)
{
   dr.SetFilled(GetFilled());

 	G_Polygon* todraw = dr.GetAbsPolygon();

	// Convert arcseg to polygon
	Convert(todraw, GLOB->DISPLAYABER());

   todraw->Transform(dr.GetMatrix());

	dr.SetSelected(GetSelect());

	todraw->ConvertArcs(GLOB->DISPLAYABER());

 	dr.Write(todraw);
}


Element* Circle::Duplicate()
{
		Circle* a_circle= new Circle;
		*a_circle=*this;

		return a_circle;
}


Circle& Circle::operator=(Circle &other)
{
   (Shape &)*this=(Shape &)other;
	_datatype = other._datatype;
	_radius=other._radius;
	_mid=other._mid;

	return *this;
}

wxPropertySheet* Circle::GetPropList()
{
   double x=_mid.GetX();
   double y=_mid.GetY();
   float  rad=_radius;

   Shape::GetPropList();
   if (!m_wxpropsheet->HasProperty("x"))
   {
	   wxProperty* theprop=new wxProperty();
	   wxPropertyValue aval(x);
	   wxString tmp = wxString("x");
      theprop->SetName( tmp );
      theprop->SetValue(aval);
      tmp = wxString("real");
      theprop->SetRole( tmp );
	   theprop->GetValue().SetModified(false);
   	m_wxpropsheet->AddProperty(theprop);
   }
   if (!m_wxpropsheet->HasProperty("y"))
   {
	   wxProperty* theprop=new wxProperty();
      wxPropertyValue aval(y);
	   wxString tmp = wxString("y");
      theprop->SetName( tmp );
      tmp = wxString("real");
      theprop->SetRole( tmp );
	   theprop->GetValue().SetModified(false);
   	m_wxpropsheet->AddProperty(theprop);
   }
   if (!m_wxpropsheet->HasProperty("radius"))
   {
	   wxProperty* theprop=new wxProperty();
      wxPropertyValue aval(rad);
	   wxString tmp = wxString("radius");
      theprop->SetName( tmp );
      theprop->SetValue(aval);
      tmp = wxString("real");
      theprop->SetRole( tmp );
      tmp = wxString("real");
	   theprop->GetValue().SetModified(false);
   	m_wxpropsheet->AddProperty(theprop);
   }

	return m_wxpropsheet;
}

void Circle::UpdatePropList()
{
   if (!m_wxpropsheet)
      return;
   Shape::UpdatePropList();
   if (m_wxpropsheet->HasProperty("x"))
   {
	   _mid.SetX(m_wxpropsheet->GetProperty("x")->GetValue().RealValue());
	   if (m_wxpropsheet->GetProperty("x")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
		m_wxpropsheet->RemoveProperty("x");
   }
   if (m_wxpropsheet->HasProperty("y"))
   {
	   _mid.SetY(m_wxpropsheet->GetProperty("y")->GetValue().RealValue());
	   if (m_wxpropsheet->GetProperty("y")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
		m_wxpropsheet->RemoveProperty("y");
   }
   if (m_wxpropsheet->HasProperty("radius"))
   {

	   _radius=m_wxpropsheet->GetProperty("radius")->GetValue().RealValue();
	   if (m_wxpropsheet->GetProperty("radius")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
		m_wxpropsheet->RemoveProperty("radius");
   }
	return;
}

BoundingBox& Circle::CalcBoundingBox()
{
	m_boundingbox.Reset();
	m_boundingbox.SetValid(true);

	double _rad;
   if (m_width)
		_rad= _radius + m_width/2;
	else
		_rad= _radius ;

	double xmin, ymin, xmax, ymax;
	xmin=_mid.GetX()-_rad;
	ymin=_mid.GetY()-_rad;
	xmax=_mid.GetX()+_rad;
	ymax=_mid.GetY()+_rad;

	m_boundingbox.SetMin(xmin,ymin);
	m_boundingbox.SetMax(xmax,ymax);

	return m_boundingbox;
}

bool Circle::Select(Point* P)
{
	double	a,b,c,width = m_width/2.0;

	a = fabs(P->GetX() - _mid.GetX());
	b = fabs(P->GetY() - _mid.GetY());
	c = sqrt(a*a + b*b);
	if (width != 0)
   {
		if ((_radius + width > c) && (_radius - width < c))
			return true;
   }
	else
   {
		if (_radius + GLOB->SELECT_SNAPFACTOR() > c)
			return true;
   }

	return false;
}

bool Circle::Select(BoundingBox* area,wxTransformMatrix* m)
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

void Circle::Transform(wxTransformMatrix* _Matrix)
{
	_mid.Transform(_Matrix);
	if (m_width)
	   m_width =  _Matrix->Get_scaleX()*m_width;
	_radius =  _Matrix->Get_scaleX()*_radius;
}

Point& Circle::GetMid()
{
	return _mid;
}

double Circle::GetRadius()
{
	return _radius;
}

void Circle::SetRadius(double radius)
{
	_radius=radius;
}

void Circle::SetMid(const Point& a_point)
{
	SetMid(a_point.GetX(), a_point.GetY());
}

void Circle::SetMid(double X,double Y)
{
	_mid.Set(X,Y);
}

int	Circle::GetDatatype()
{
	return _datatype;
}

void Circle::SetDatatype(int new_value)
{
	_datatype = new_value;
}

void Circle::Convert(G_Polygon* _polygon, double)
{
	_polygon->MakeEmpty();

   if( m_width==0 )
	{
      Arc_Seg* seg=new Arc_Seg();
      seg->SetX(_mid.GetX()-_radius);
      seg->SetY(_mid.GetY());

		seg->SetAX(_mid.GetX());
      seg->SetAY(_mid.GetY()+_radius);

		seg->SetOX(_mid.GetX());
		seg->SetOY(_mid.GetY());

		_polygon->insend(seg);

		seg=new Arc_Seg();
		seg->SetX(_mid.GetX()+_radius);
		seg->SetY(_mid.GetY());

		seg->SetAX(_mid.GetX());
		seg->SetAY(_mid.GetY()-_radius);

		seg->SetOX(_mid.GetX());
		seg->SetOY(_mid.GetY());

		_polygon->insend(seg);
	}
	else
	{
		double _rad1 = _radius - m_width/2.0;
		double _rad2 = _radius + m_width/2.0;

		// Line to Inner Circle
		Line_Seg* lseg=new Line_Seg();
		lseg->SetX(_mid.GetX()-_rad2);
		lseg->SetY(_mid.GetY());
		_polygon->insend(lseg);

		// Inner Circle, upper arc
		Arc_Seg* aseg=new Arc_Seg();
		aseg->SetX(_mid.GetX()-_rad1);
		aseg->SetY(_mid.GetY());
		aseg->SetAX(_mid.GetX());
		aseg->SetAY(_mid.GetY()+_rad1);
		aseg->SetOX(_mid.GetX());
		aseg->SetOY(_mid.GetY());
		_polygon->insend(aseg);

		// Inner Circle, lower arc
		aseg=new Arc_Seg();
		aseg->SetX(_mid.GetX()+_rad1);
		aseg->SetY(_mid.GetY());
		aseg->SetAX(_mid.GetX());
		aseg->SetAY(_mid.GetY()-_rad1);
		aseg->SetOX(_mid.GetX());
		aseg->SetOY(_mid.GetY());
		_polygon->insend(aseg);

		// Line to Outer Circle
		lseg=new Line_Seg();
      lseg->SetX(_mid.GetX()-_rad1);
		lseg->SetY(_mid.GetY());
      _polygon->insend(lseg);

      // Outer Circle, lower arc
      aseg=new Arc_Seg();
      aseg->SetX(_mid.GetX()-_rad2);
		aseg->SetY(_mid.GetY());
		aseg->SetAX(_mid.GetX());
      aseg->SetAY(_mid.GetY()-_rad2);
		aseg->SetOX(_mid.GetX());
		aseg->SetOY(_mid.GetY());
      _polygon->insend(aseg);

      // Outer Circle, upper arc
      aseg=new Arc_Seg();
		aseg->SetX(_mid.GetX()+_rad2);
      aseg->SetY(_mid.GetY());
      aseg->SetAX(_mid.GetX());
      aseg->SetAY(_mid.GetY()+_rad2);
		aseg->SetOX(_mid.GetX());
		aseg->SetOY(_mid.GetY());
      _polygon->insend(aseg);
	}
}


// Image ===================================================================
Image::Image(int layernr,const wxString& path)
{
	m_layer=layernr;

   m_path = path;

	m_image.LoadFile(m_path, wxBITMAP_TYPE_GIF );
}

Image::Image()
{
	m_layer=0;

   m_path = "";
   m_width=0;
   m_type="";
}

Image::~Image()
{
}

void Image::OWrite(Driver_out& dr)
{
   dr.Write(this);
}

void Image::DoWrite(DrawDriver& dr)
{
   dr.SetFilled(GetFilled());
	dr.SetSelected(GetSelect());

/*
   wxBitmap bmp ;

	double Xmin,Ymin,Xmax,Ymax;

	Xmin=LogicalToDeviceX(_absbox.GetMin().GetX());
	Ymin=LogicalToDeviceY(_absbox.GetMin().GetY());
	Xmax=LogicalToDeviceX(_absbox.GetMax().GetX());
	Ymax=LogicalToDeviceY(_absbox.GetMax().GetY());

   if (_drawstyle != INVERT_WIREFRAME && _drawstyle != SELECTED)
   {
      hulp2=*_Image;
      hulp2.Transform(_matrix);

      double xmin,ymin,xmax,ymax;
      xmin=LogicalToDeviceX(hulp2.GetPosition().GetX());
      ymin=LogicalToDeviceY(hulp2.GetPosition().GetY());
      xmax=LogicalToDeviceX(hulp2.GetPosition().GetX()+hulp2.GetW());
      ymax=LogicalToDeviceY(hulp2.GetPosition().GetY()+hulp2.GetH());

      wxPoint centr(xmin-(ymin-ymax)*sin(hulp2.GetRotation()/180.0 *M_PI),
      ymin-(xmax-xmin)*sin(hulp2.GetRotation()/180.0 *M_PI)-
      (ymin-ymax)*cos(hulp2.GetRotation()/180.0 *M_PI));
      wxPoint centr2(0,0);

      if (_Image->GetRotation())
      {
      	bmp=hulp2.GetImage().Scale(xmax-xmin,ymin-ymax).Rotate(hulp2.GetRotation()/180.0 *M_PI,centr2, TRUE,  NULL).ConvertToBitmap();
      }
      else
      {
     		bmp = hulp2.GetImage().Scale(xmax-xmin,ymin-ymax).ConvertToBitmap();
      }



      wxMask* trans=new wxMask(bmp,*wxBLACK);
      bmp.SetMask(trans);
      m_dc->DrawBitmap(bmp,centr,true );
   }

   Box bbox(0);
   bbox.SetSelect(_Selected);
   bbox.SetFilled(m_filled);
   if (_Selected)
   {
	   bbox.SetFilled(false);
	   bbox.SetWidth(0);
   }
   else
	   bbox.SetWidth(_Image->GetWidth());
   bbox.SetRelativeMatrix(_Image->GetRelativeMatrix());
   bbox.SetW(_Image->GetW());
   bbox.SetH(_Image->GetH());
   bbox.CalcBoundingBox();
   Write(&bbox);
*/
}


Element* Image::Duplicate()
{
		Image* a_image= new Image(m_layer,m_path);
		*a_image=*this;

		return a_image;
}


Image& Image::operator=(Image &other)
{
   (Shape &)*this=(Shape &)other;
   _relative_matrix = other._relative_matrix;
   m_w=other.m_w;
   m_h=other.m_h;
   m_image=other.m_image;
	return *this;
}

wxPropertySheet* Image::GetPropList()
{
   double x=_relative_matrix.GetValue(2,0);
   double y=_relative_matrix.GetValue(2,1);

   Shape::GetPropList();
   if (!m_wxpropsheet->HasProperty("x"))
   {
	   wxProperty* theprop=new wxProperty();
      wxPropertyValue aval(x);
	   wxString tmp = wxString("x");
      theprop->SetName( tmp );
      theprop->SetValue(aval);
      tmp = wxString("real");
      theprop->SetRole( tmp );
	   theprop->GetValue().SetModified(false);
   	m_wxpropsheet->AddProperty(theprop);
   }
   if (!m_wxpropsheet->HasProperty("y"))
   {
	   wxProperty* theprop=new wxProperty();
      wxPropertyValue aval(y);
	   wxString tmp = wxString("y");
      theprop->SetName( tmp );
      theprop->SetValue(aval);
      tmp = wxString("real");
      theprop->SetRole( tmp );
	   theprop->GetValue().SetModified(false);
   	m_wxpropsheet->AddProperty(theprop);
   }
   if (!m_wxpropsheet->HasProperty("imageheight"))
   {
	   wxProperty* theprop=new wxProperty();
      wxPropertyValue aval(GetH());
	   wxString tmp = wxString("imageheight");
      theprop->SetName( tmp );
      theprop->SetValue(aval);
      tmp = wxString("real");
      theprop->SetRole( tmp );
	   theprop->GetValue().SetModified(false);
   	m_wxpropsheet->AddProperty(theprop);
   }
   if (!m_wxpropsheet->HasProperty("imagewidth"))
   {
	   wxProperty* theprop=new wxProperty();
      wxPropertyValue aval(GetW());
	   wxString tmp = wxString("imagewidth");
      theprop->SetName( tmp );
      theprop->SetValue(aval);
      tmp = wxString("real");
      theprop->SetRole( tmp );
	   theprop->GetValue().SetModified(false);
   	m_wxpropsheet->AddProperty(theprop);
   }
   if (!m_wxpropsheet->HasProperty("rotation"))
   {
	   wxProperty* theprop=new wxProperty();
      wxPropertyValue aval(GetRotation());
	   wxString tmp = wxString("rotation");
      theprop->SetName( tmp );
      theprop->SetValue(aval);
      tmp = wxString("real");
      theprop->SetRole( tmp );
	   theprop->GetValue().SetModified(false);
   	m_wxpropsheet->AddProperty(theprop);
   }

	return m_wxpropsheet;
}

void Image::UpdatePropList()
{
   if (!m_wxpropsheet)
      return;
   Shape::UpdatePropList();
   if (m_wxpropsheet->HasProperty("x"))
   {
	   _relative_matrix.SetValue(2,0,m_wxpropsheet->GetProperty("x")->GetValue().RealValue());
	   if (m_wxpropsheet->GetProperty("x")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
		m_wxpropsheet->RemoveProperty("x");
   }
   if (m_wxpropsheet->HasProperty("y"))
   {
	   _relative_matrix.SetValue(2,1,m_wxpropsheet->GetProperty("y")->GetValue().RealValue());
	   if (m_wxpropsheet->GetProperty("y")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
		m_wxpropsheet->RemoveProperty("y");
   }
   if (m_wxpropsheet->HasProperty("imageheight"))
   {
      CalcBoundingBox();
	   if (m_wxpropsheet->GetProperty("imageheight")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
      SetH(m_wxpropsheet->GetProperty("imageheight")->GetValue().RealValue());
		m_wxpropsheet->RemoveProperty("imageheight");
   }
   if (m_wxpropsheet->HasProperty("imagewidth"))
   {
      CalcBoundingBox();
	   if (m_wxpropsheet->GetProperty("imagewidth")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
      SetW(m_wxpropsheet->GetProperty("imagewidth")->GetValue().RealValue());
		m_wxpropsheet->RemoveProperty("imagewidth");
   }
   if (m_wxpropsheet->HasProperty("rotation"))
   {
      CalcBoundingBox();
	   if (m_wxpropsheet->GetProperty("rotation")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
      SetRotation(m_wxpropsheet->GetProperty("rotation")->GetValue().RealValue());
		m_wxpropsheet->RemoveProperty("rotation");
   }

	return;
}

BoundingBox& Image::CalcBoundingBox()
{
	m_boundingbox.Reset();
	m_boundingbox.SetValid(true);
	m_boundingbox.SetMin(-m_width/2.0,-m_width/2.0);
	m_boundingbox.SetMax(m_w+m_width/2.0,m_h+m_width/2.0);

	m_boundingbox.MapBbox(&_relative_matrix);
	return m_boundingbox;

}

bool Image::Select(Point* P)
{
	if (CalcBoundingBox().PointInBox(*P))
		return true;
	return false;
}

bool Image::Select(BoundingBox* area,wxTransformMatrix* m)
{
	Point			min,max,amin,amax;

	// check if the boundingBox of the polygon is in the selected boundingBox
	BoundingBox	backup =CalcBoundingBox();
	backup.MapBbox(m);

	min = backup.GetMin();
	max = backup.GetMax();
	amin = area->GetMin();
	amax = area->GetMax();

	if ((min.GetX() < amin.GetX()) || (min.GetY() < amin.GetY()) ||
		(max.GetX() > amax.GetX()) || (max.GetY() > amax.GetY())) return false;

	return true;
}

void Image::Transform(wxTransformMatrix* _Matrix)
{
   _relative_matrix = *_Matrix * _relative_matrix;
   m_width=_Matrix->Get_scaleX()*m_width;
   m_w=_Matrix->Get_scaleX()*m_w;
   m_h=_Matrix->Get_scaleX()*m_h;
}

Point Image::GetPosition()
{
   Point offsetpoint;
   offsetpoint.SetX(_relative_matrix.GetValue(2,0));
   offsetpoint.SetY(_relative_matrix.GetValue(2,1));
	return offsetpoint;
}

void Image::SetPosition(Point *a_point)
{
	SetPosition(a_point->GetX(), a_point->GetY());
}

void Image::SetPosition(double X,double Y)
{
	  _relative_matrix.SetValue(2,0,X);
     _relative_matrix.SetValue(2,1,Y);
}

void Image::Convert(G_Polygon* _polygon, double aber)
{
	_polygon->MakeEmpty();
}

void Image::SetPictureType(const wxString& type)
{
	m_type=type;
}

void Image::SetPath(const wxString& path)
{
	m_path=path;
	m_image.LoadFile(m_path, wxBITMAP_TYPE_GIF );
}

void Image::SetH(double height)
{
   m_h=height;
}

double Image::GetH()
{
   return m_h;
}

void Image::SetW(double width)
{
   m_w=width;
}

double Image::GetW()
{
   return m_w;
}

double Image::GetRotation()
{
   return GetRelativeMatrix()->GetRotation();
}

void Image::SetRotation(double rotation)
{
   _relative_matrix.SetRotation(rotation);
}

void Image::SetRelativeMatrix(wxTransformMatrix* m)
{
	_relative_matrix = *m;
}

wxTransformMatrix* Image::GetRelativeMatrix()
{
	return &_relative_matrix;
}

wxString Image::GetPictureType()
{
	return m_type;
}

wxString Image::GetPath()
{
 	return m_path;
}

// Line ======================================================================
SLine::SLine(int layernr)
{
	m_pathtype = PATH_END_SQAURE;
	m_layer=layernr;
	m_width = 0;
   m_x1 = 0;
   m_y1 = 0;
   m_x2 = 0;
   m_y2 = 0;
/*
   G_Polygon* p= new G_Polygon();
   p->AddSegment(new Line_Seg(200,100));
   p->AddSegment(new Line_Seg(0,0));
   p->AddSegment(new Line_Seg(200,-100));
   Boundary* b= new Boundary(m_layer);
   b->SetPolygon(p);
   m_arrow= new Structure("__arrow");
   m_arrow->insend(b);
   m_arrow->CalcBoundingBox();
*/
}

SLine::~SLine()
{
}

void SLine::OWrite(Driver_out& dr)
{
   dr.Write(this);
/*
   wxTransformMatrix to;
   to.SetRotation(30);
   to.Translate(m_x1,m_y1);
   m_arrow
   where.Write(m_arrow);
   to.Identity();
   to.SetRotation(-30);
   to.Translate(m_x2,m_y2);
   where.Write(m_arrow);
*/
}

void SLine::DoWrite(DrawDriver& dr)
{
   dr.SetFilled(GetFilled());
   bool ident = dr.GetMatrix()->IsIdentity();

	dr.SetSelected(GetSelect());

   if (dr.GetDrawStyle() == INVERT_WIREFRAME)
      dr.SetLogicalFunction(wxINVERT);
   else
      dr.SetLogicalFunction(wxCOPY);

	// Convert path to polygon with width
	if ((m_width != 0) || (! ident))
   {
      SLine tmp=*this;
      tmp.Transform(dr.GetMatrix());
   	if (GetSelect() || dr.GetParentSelect())
      {
         dr.SetPen(dr.GetSelectPen());
         dr.GetSelectPen().SetWidth(tmp.GetWidth());
      }
      else
      {
         dr.SetPen(dr.GetPen());
         dr.GetDrawPen().SetWidth(tmp.GetWidth());
      }
      dr.DrawLine(tmp.GetX1(),tmp.GetY1(),tmp.GetX2(),tmp.GetY2());
   }
   else
   {
   	if (GetSelect()|| dr.GetParentSelect())
         dr.SetPen(dr.GetSelectPen());
      dr.DrawLine(GetX1(),GetY1(),GetX2(),GetY2());
   }
}

void SLine::Set(double x1,double y1,double x2, double y2)
{
   m_x1 = x1;
   m_y1 = y1;
   m_x2 = x2;
   m_y2 = y2;
}


Element* SLine::Duplicate()
{
		SLine* a_Line= new SLine(0);
		*a_Line=*this;

		return a_Line;
}

SLine& SLine::operator=(SLine &other)
{
   (Shape &)*this=(Shape &)other;
   m_x1 = other.m_x1;
   m_y1 = other.m_y1;
   m_x2 = other.m_x2;
   m_y2 = other.m_y2;
	m_pathtype = PATH_END_SQAURE;

	return *this;
}

PATHTYPE SLine::GetPathtype()
{
	return m_pathtype;
}

void SLine::SetPathtype(PATHTYPE p)
{
	m_pathtype = p;
}

wxPropertySheet* SLine::GetPropList()
{
   Shape::GetPropList();
	return m_wxpropsheet;
}

void SLine::UpdatePropList()
{
   if (!m_wxpropsheet)
      return;
   Shape::UpdatePropList();
	return;
}


BoundingBox& SLine::CalcBoundingBox()
{
	m_boundingbox.Reset();
	m_boundingbox.SetValid(true);
	m_boundingbox.SetMin(m_x1,m_y1);
	m_boundingbox.SetMax(m_x1,m_y1);
	m_boundingbox.Expand(m_x2,m_y2);
	m_boundingbox.EnLarge(m_width/2.0);
	return m_boundingbox;
}

bool SLine::Select(BoundingBox* area, wxTransformMatrix* m)
{
	// check if the boundingLine of the polygon is in the selected boundingLine
	BoundingBox	*backup = new BoundingBox(CalcBoundingBox());
	backup->MapBbox(m);

   Point min,max,amin,amax;
	min = backup->GetMin();
	max = backup->GetMax();
	amin = area->GetMin();
	amax = area->GetMax();

	delete backup;

	if ((min.GetX() < amin.GetX()) || (min.GetY() < amin.GetY()) ||
		(max.GetX() > amax.GetX()) || (max.GetY() > amax.GetY())) return false;

	return true;
}

bool SLine::Select(Point* P)
{
   Point one(m_x1,m_y1);
   Point two(m_x2,m_y2);
   wxLine a(one,two);
   double dist;
   if (a.PointInLine(*P,dist,(m_width==0?GLOB->SELECT_SNAPFACTOR():m_width))==R_IN_AREA)
		return true;

	return false;
}

void SLine::Transform(wxTransformMatrix* _Matrix)
{
   Point one(m_x1,m_y1);
   Point two(m_x2,m_y2);
	one.Transform(_Matrix);
	two.Transform(_Matrix);
   m_x1=one.GetX();
   m_y1=one.GetY();
   m_x2=two.GetX();
   m_y2=two.GetY();
	if (m_width)
	   m_width =  _Matrix->Get_scaleX()*m_width;
}

void SLine::Convert(G_Polygon* _converted, double)
{
	_converted->MakeEmpty();
   Line_Seg* lseg=new Line_Seg(m_x1,m_y1);
   _converted->insend(lseg);
   lseg=new Line_Seg(m_x2,m_y2);
   _converted->insend(lseg);
}

