/*
Program	SCRNDRV.CPP
Purpose	Virtual base class for all screendrivers (header)
		derived drivers must overload Draw functions!
		Mapping and clipping is done here
*/


#ifdef __GNUG__
#pragma implementation
#endif

#include "polyline.h"
#include "boundary.h"
#include "referenc.h"
#include "elemlst.h"
#include "gdsii.h"
#include "path.h"
#include "text.h"
#include "arc.h"

#include "scrndrv.h"
#include "windrv.h"
#include "units.h"
#include "wxwin.h"
#include "dcclbuf.h"
#include "structur.h"


class StructureIter;

// contructor  of screendriver
// makes a matrix and set boundingboxes
DrawDriver::DrawDriver()
{
   m_dc=0;
	// holds temporary matrix from a structurereference
	// this will be modified in the references to calculate absolute positions
	_matrix = new wxTransformMatrix();
	if (_matrix == 0)
		throw GDS_Error("Cannot allocate memory for a matrix",
			"Screendriver Error", 0, 1);

	_only_visible = true;
	_visible = true;
	_clip_status = _OUT;
	_drawstyle = FILLED;
	_clipit = false;
	_Selected = false;
	_SRefSelected = false;

	_AbsPolygon = new G_Polygon();

   _AI			= new TDLI<Segment>();

   _busydrawing=false; //not drawing in progress

	_penSelected = 0;
	_pen = 0;
	_brush = 0;
	_pengrid = 0;
	_penorigin = 0;

	todraw = new wxPoint[MAX_POINTS_TODRAW+1];

   m_scrolled = false;

}

// destructor: frees dynamic allocated memory
DrawDriver::~DrawDriver()
{
	delete _matrix;
   delete _AI;

	if (_AbsPolygon) delete _AbsPolygon;

	if (todraw)
		delete[] todraw;
	delete _penSelected;
	delete _pen;
	delete _brush;
	delete _pengrid;
	delete _penorigin;
	delete _pentrans;
	delete _pentrans_sel;
	//Added from here on
	delete _textSelectedColor;
	delete _textColor;
	//delete _brushbitm;
	delete _currentbitmap;
}

void DrawDriver::SetMappingScroll(
										double vx1,
										double vy1,
										double vx2,
										double vy2,
										bool border)
{
   int dx,dy;
   GLOB->CANVAS->GetClientSize(&dx,&dy);
   GLOB->CANVAS->Set_Valid_drawing(false);

   SetMapping(dx,dy,vx1,vy1,vx2,vy2,border);

   if (m_scrolled)
      SetScroll(m_virtm_minX,m_virtm_minY,m_virtm_maxX,m_virtm_maxY);

   int dx2,dy2;
   GLOB->CANVAS->GetClientSize(&dx2,&dy2);
   if ( dx != dx2 || dy != dy2) //scrollbar is/became empty
      SetMapping(dx2,dy2,vx1,vy1,vx2,vy2,border);
}


void DrawDriver::SetScroll(double vx1,double vy1,double vx2,double vy2)
{
    m_virtm_minX=vx1;
    m_virtm_minY=vy1;
    m_virtm_maxX=vx2;
    m_virtm_maxY=vy2;

	 double dvx = m_virt_maxX - m_virt_minX;
	 double dvy = m_virt_maxY - m_virt_minY;
	 double dmvx = m_virtm_maxX - m_virtm_minX;
	 double dmvy = m_virtm_maxY - m_virtm_minY;


    GLOB->CANVAS->AdjustScrollBars((m_virt_minX-m_virtm_minX)/dmvx *1000,dvx/dmvx *1000,1000,
                                   (m_virtm_maxY-m_virt_maxY)/dmvy *1000,dvy/dmvy *1000,1000);

    m_scrolled=true;
}

void DrawDriver::SetClippingRegionBox(BoundingBox&	a_box)
{
   double ymin,xmin,xmax,ymax;
   xmax=a_box.GetMax().GetX();
   xmin=a_box.GetMin().GetX();
   ymax=a_box.GetMax().GetY(); //just the oposite
   ymin=a_box.GetMin().GetY();
   SetClippingRegion(xmin,ymin,xmax,ymax);
}

// the next three function are used to set or query the drawstyle.
// The drawstyle is used to determine how figures should be drawn on screen.
// There are curently three defined drawstyle:
//		WIREFRAME	= 	draw the outlines only
//		FILLED		= 	draw outlines and fill the polygons
//		SELECTED		=	draw only a selectedline around already existing objects
//
// P.S. With the drawstyle wireframe or filled selected objects are also drawn
void DrawDriver::SetDrawStyle(DrawStyle value)
{
   if (value == STORED)
 		_drawstyle=_stored_drawstyle;
   else
   {
   	_stored_drawstyle=_drawstyle;
		_drawstyle = value;
   }
}

DrawStyle DrawDriver::GetDrawStyle()
{
	return _drawstyle;
}

bool DrawDriver::IsDrawStyle(DrawStyle value)
{
	return (bool)(value == _drawstyle);
}

// return the boundingbox from the GDS-II
BoundingBox& DrawDriver::GetVirtualSizeScreen()
{
   m_virtualsize.SetMin(m_virt_minX,m_virt_minY);
   m_virtualsize.SetMax(m_virt_maxX,m_virt_maxY);
   m_virtualsize.SetValid(true);
	return m_virtualsize;
}


// multiplies the relative matrix with the X and Y
void DrawDriver::Transform(double &x,
										  double &y)
{
   double xh,yh;
   xh=x;
   yh=y;
	_matrix->TransformPoint(xh,yh, x, y);
}


// see comment above
void DrawDriver::Transform(double &x1,
										  double &y1,
										  double &x2,
										  double &y2)
{
	Transform(x1, y1);
	Transform(x2, y2);
}

bool DrawDriver::NeedToDraw(int layer)
{
	// check if this layer must be written
	if (_drawstyle == INVERT_WIREFRAME)
   	return true;
	if ((_drawstyle == FILLED) && (_ActiveLayer != layer))
   	return false;
	return true;
}


// Boundary -----------------------------------------------------------------
void DrawDriver::WriteDirect(Boundary* _Boundary)
{
   _matrix->Identity();
   _SRefSelected=false;

   _clipit=ClipIt(_Boundary);
   _Boundary->Write(*this);
}



void DrawDriver::WriteDirect(Path* _Path)
{
   _matrix->Identity();
   _SRefSelected=false;

   _clipit=ClipIt(_Path);
   _Path->Write(*this);
}

void DrawDriver::WriteDirect(Arcseg* _Arcseg)
{
   _matrix->Identity();
   _SRefSelected=false;
	_clip_status = _ON;
   _clipit=ClipIt(_Arcseg);
   _Arcseg->Write(*this);
}

void DrawDriver::WriteDirect(SLine* line)
{
   _matrix->Identity();
   _SRefSelected=false;
	_clip_status = _ON;
   _clipit=ClipIt(line);
   line->Write(*this);
}

void DrawDriver::WriteDirect(Box* _Box)
{
   _matrix->Identity();
   _SRefSelected=false;
	_clip_status = _ON;
   _clipit=ClipIt(_Box);
   _Box->Write(*this);
}

void DrawDriver::WriteDirect(Circle* _Circle)
{
   _matrix->Identity();
   _SRefSelected=false;
	_clip_status = _ON;
   _clipit=ClipIt(_Circle);
   _Circle->Write(*this);
}

void DrawDriver::WriteDirect(Text* _Text)
{
   _matrix->Identity();
   _SRefSelected=false;
	_clip_status = _ON;
   _clipit=ClipIt(_Text);
   _Text->Write(*this);
}


void DrawDriver::WriteDirect(ElementList* _ElementList)
{
   _matrix->Identity();
   _SRefSelected=false;
	_clip_status = _ON;
   _ElementList->Write(*this);
}


// GdsII --------------------------------------------------------------------
void DrawDriver::Write(GdsII* _GdsII)
{
   _GdsII->Write(*this);
}

void DrawDriver::Write(Point* _Point)
{
	DrawPoint(_Point->GetX(), _Point->GetY());
}


void DrawDriver::Write(Segment* _Segment)
{
	DrawPoint(_Segment->GetX(), _Segment->GetY());
}

// Polygon -----------------------------------------------------------------
void DrawDriver::WriteDirect(G_Polygon* _Polygon)
{
   m_filled=false;
   _matrix->Identity();
   _SRefSelected=false;

   Write(_Polygon);
}

// Polygon ------------------------------------------------------------------
void DrawDriver::Write(G_Polygon* _Polygon)
{

	// are there points anyway?
	if (_Polygon->count() < 2 )
   	return;

   _AI->Attach(_Polygon);
   int points=_AI->count();
   if (_clipit || (points > MAX_POINTS_TODRAW))
   {
      // now draw the wireframe
      _AI->tohead();
      double x,y;
      double old_x = _AI->item()->GetX();
      double old_y = _AI->item()->GetY();

      (*_AI)++;
      while (!_AI->hitroot())
      {
         x = _AI->item()->GetX();
         y = _AI->item()->GetY();
         if (_drawstyle != SELECTED)
	         m_dc->SetPen(*_pen);
         if (_Selected ||(_SRefSelected))
            m_dc->SetPen(*_penSelected);
         GDSDrawLine(old_x, old_y, x, y);
         old_x = x;
         old_y = y;
         (*_AI)++;
      }
      _AI->tohead();
      x = _AI->item()->GetX();
      y = _AI->item()->GetY();
      // draw the line to close the end- and beginpoints
      if (_drawstyle != SELECTED)
         m_dc->SetPen(*_pen);
      if (_Selected ||(_SRefSelected))
         m_dc->SetPen(*_penSelected);
      GDSDrawLine(old_x, old_y, x, y);
   }
   else
   {  int pat=_pattern;
      if (!m_filled)
         _pattern=0;
      if (_Polygon->ContainsHoles() && (_drawstyle != INVERT_WIREFRAME))
      {
         WindowsPolygon_NoOutLine();
         // now draw the wireframe
         _AI->tohead();
         double x,y;
         double old_x = _AI->item()->GetX();
         double old_y = _AI->item()->GetY();
         HOLEFLAGS prev=_AI->item()->GetHoleType();

         (*_AI)++;
         while (!_AI->hitroot())
         {
            x = _AI->item()->GetX();
            y = _AI->item()->GetY();
            if (_drawstyle != SELECTED)
               m_dc->SetPen(*_pen);
            if (_Selected ||(_SRefSelected))
               m_dc->SetPen(*_penSelected);
            if (prev!=LINK_SEG)
               GDSDrawLine(old_x, old_y, x, y);
            prev=_AI->item()->GetHoleType();
            old_x = x;
            old_y = y;
            (*_AI)++;
         }
         _AI->tohead();
         x = _AI->item()->GetX();
         y = _AI->item()->GetY();
         // draw the line to close the end- and beginpoints
         if (_drawstyle != SELECTED)
            m_dc->SetPen(*_pen);
         if (_Selected ||(_SRefSelected))
            m_dc->SetPen(*_penSelected);
         if (prev!=LINK_SEG)
               GDSDrawLine(old_x, old_y, x, y);
      }
      else
         WindowsPolygon();
      _pattern=pat;

   }
   _AI->Detach();
}


// Polyline -----------------------------------------------------------------
void DrawDriver::Write(G_Polyline* _Polyline)
{
	// are there points anyway?
	if (_Polyline->count() < 2 )
   	return;

	_AI->Attach(_Polyline);
	int points = _AI->count();


	if (_clipit || points > MAX_POINTS_TODRAW)
	{
      // now draw the wireframe
      _AI->tohead();
      double old_x = _AI->item()->GetX();
      double old_y = _AI->item()->GetY();
      double x,y;

      (*_AI)++;
      while (!_AI->hitroot())
      {
         x = _AI->item()->GetX();
         y = _AI->item()->GetY();

         // draw the line
         if (_drawstyle != SELECTED)
	         m_dc->SetPen(*_pen);
         if (_Selected ||(_SRefSelected))
            m_dc->SetPen(*_penSelected);
         GDSDrawLine(old_x, old_y, x, y);

         old_x = x;
         old_y = y;
         (*_AI)++;
      }
   }
	else WindowsPolyline();

	_AI->Detach();
}

void DrawDriver::DrawOrigin()
{
	int length = 10; // length (in pixels) of each line
    double x1=0,y1=0;

	if (_clip_status == _ON)
		if (!Clipping(x1, y1))
			return;

   m_dc->SetPen(*_penorigin);

	x1=LogicalToDeviceX(0);
	y1=LogicalToDeviceY(0);

	m_dc->DrawLine((long)x1,(long)y1-length,(long)x1,(long)y1+length);
	m_dc->DrawLine((long)x1-length,(long)y1,(long)x1+length,(long)y1);
	m_dc->SetPen(wxNullPen);
}



bool DrawDriver::ClipIt(Element* a_ele)
{
	double Xmin,Ymin,Xmax,Ymax;

	Xmin=LogicalToDeviceX(a_ele->GetBoundingBox().GetMin().GetX());
	Ymin=LogicalToDeviceY(a_ele->GetBoundingBox().GetMin().GetY());
	Xmax=LogicalToDeviceX(a_ele->GetBoundingBox().GetMax().GetX());
	Ymax=LogicalToDeviceY(a_ele->GetBoundingBox().GetMax().GetY());

   //careful the YMAX and YMIN are reversed because (0.0) FOR THE window TopLeft
   return (bool) (Xmin < MININT || Ymax < MININT || Xmax > MAXINT || Ymin >MAXINT);
}



void DrawDriver::WindowsPolygon()
{

   // are there points anyway?

	int points = _AI->count();
	if (!points)
		return;

    bool isBBoxValid=false;
    int minX,minY,maxX,maxY;

    minX=minY=maxX=maxY=0;
	// now draw the wireframe
	_AI->tohead();
	double x,y;
	int i=0;
	while (!_AI->hitroot())
	{
   	x=LogicalToDeviceX(_AI->item()->GetX());
	   y=LogicalToDeviceY(_AI->item()->GetY());

          if ( isBBoxValid )
          {
             if ( x < minX ) minX = x;
             if ( y < minY ) minY = y;
             if ( x > maxX ) maxX = x;
             if ( y > maxY ) maxY = y;
          }
          else
          {
             isBBoxValid = true;

             minX = x;
             minY = y;
             maxX = x;
             maxY = y;
          }

		todraw[i].x=(long)x;
		todraw[i].y=(long)y;
		i++;
		(*_AI)++;
	}

	m_dc->SetPen(*_pen);

	if ((_drawstyle == FILLED))
   {
      switch (_pattern)
      {
         case 1:
            {
               if(_Selected || _SRefSelected)
                  m_dc->SetPen(*_penSelected);
               else
                  m_dc->SetPen(*_pen);
		         m_dc->SetLogicalFunction(wxCOPY);
               m_dc->SetBrush(*_brush);
               m_dc->DrawPolygon(points,todraw,0,0,wxWINDING_RULE);
               m_dc->SetBrush(wxNullBrush);
            }
            break;
         case 0:   //only outlines needed
            break;
         default:
            {
                 if(_Selected || _SRefSelected)
	                 m_dc->SetPen(*_pentrans_sel);
                 else
	                 m_dc->SetPen(*_pentrans);

                 m_dc->SetBrush(*_brush);

                 wxClientDCbuf* he=wxDynamicCast(m_dc, wxClientDCbuf);
                 if (he)
                 {
                        wxTransparentPolygon object(1);
                        object.points=todraw;
                        object.n=points;
                        object.CalcBoundingBox(minX,minY);
                        object.CalcBoundingBox(maxX,maxY);
		                  he->TransParentPolyon(&object);
                 }
                 else
                 {
                    m_dc->SetTextForeground(*wxBLACK);            // 0s --> 0x000000 (black)
                    m_dc->SetTextBackground(*wxWHITE);    // 1s --> 0xFFFFFF (white)
                    m_dc->SetLogicalFunction(wxAND_INVERT);

                    // BLACK OUT the opaque pixels and leave the rest as is
                    m_dc->DrawPolygon(points,todraw,0,0,wxWINDING_RULE);

                    // Set background and foreground colors for fill pattern
                    //the previous blacked out pixels are now merged with the layer color
                    //while the non blacked out pixels stay as they are.
                    m_dc->SetTextBackground(_brush->GetColour());
                    if(_Selected || _SRefSelected)
                        m_dc->SetPen(*_penSelected);
                    else
                        m_dc->SetPen(*_pen);
                    m_dc->SetLogicalFunction(wxOR);
                    m_dc->DrawPolygon(points,todraw,0,0,wxWINDING_RULE);
                    m_dc->SetLogicalFunction(wxCOPY);
                 }
                 m_dc->SetBrush(wxNullBrush);
            }
      }
   }
   if (points > 2)
   {
		todraw[points] = todraw[0];//for drawing the polyline around the polygon
      points++;
   }

	m_dc->SetPen(wxNullPen);

   switch (_drawstyle)
   {
      case INVERT_WIREFRAME:
         m_dc->SetLogicalFunction(wxINVERT);
         m_dc->SetPen(*_pen);
         m_dc->SetBrush(*_brush);
         m_dc->DrawLines (points,todraw,0,0);
         m_dc->SetLogicalFunction(wxCOPY);
         m_dc->SetBrush(wxNullBrush);
         m_dc->SetPen(wxNullPen);
         break;
      case FILLED:
//				if (_pattern==0)
            {
               if(_Selected || _SRefSelected)
                  m_dc->SetPen(*_penSelected);
               else
                  m_dc->SetPen(*_pen);
		         m_dc->SetLogicalFunction(wxCOPY);
               m_dc->SetBrush(*_brush);
               m_dc->DrawLines (points,todraw,0,0);
               m_dc->SetBrush(wxNullBrush);
               m_dc->SetPen(wxNullPen);
            }
            break;
      case WIREFRAME:
	   case SELECTED:
         if(_Selected || _SRefSelected)
            m_dc->SetPen(*_penSelected);
         else
         	m_dc->SetPen(*_pen);
         m_dc->SetLogicalFunction(wxCOPY);
         m_dc->SetBrush(*_brush);
         m_dc->DrawLines (points,todraw,0,0);
         m_dc->SetBrush(wxNullBrush);
         m_dc->SetPen(wxNullPen);
   }
}

void DrawDriver::WindowsPolygon_NoOutLine()
{
// are there points anyway?

    bool isBBoxValid=false;
    int minX,minY,maxX,maxY;

    minX=minY=maxX=maxY=0;

	int points = _AI->count();
	if (!points)
		return;

	// now draw the wireframe
	_AI->tohead();
	double x,y;
	int i=0;
	while (!_AI->hitroot())
	{
   	x=LogicalToDeviceX(_AI->item()->GetX());
	   y=LogicalToDeviceY(_AI->item()->GetY());

          if ( isBBoxValid )
          {
             if ( x < minX ) minX = x;
             if ( y < minY ) minY = y;
             if ( x > maxX ) maxX = x;
             if ( y > maxY ) maxY = y;
          }
          else
          {
             isBBoxValid = true;

             minX = x;
             minY = y;
             maxX = x;
             maxY = y;
          }

		todraw[i].x=(long)x;
		todraw[i].y=(long)y;
		i++;
		(*_AI)++;
	}

   wxPen pentrans(*wxWHITE,0,wxTRANSPARENT);
	m_dc->SetPen(pentrans);
//	m_dc->SetPen(*_pen);

	if ((_drawstyle == FILLED))
   {
      switch (_pattern)
      {
         case 1:
            {
               m_dc->SetBrush(*_brush);
		         m_dc->SetLogicalFunction(wxCOPY);
               m_dc->DrawPolygon(points,todraw,0,0,wxWINDING_RULE);
               m_dc->SetBrush(wxNullBrush);
            }
            break;
         case 0:   //only outlines needed
            break;
         default:
            {
                 m_dc->SetBrush(*_brush);

                 wxClientDCbuf* he=wxDynamicCast(m_dc, wxClientDCbuf);
                 if (he)
                 {
                        wxTransparentPolygon object(1);
                        object.points=todraw;
                        object.n=points;
                        object.CalcBoundingBox(minX,minY);
                        object.CalcBoundingBox(maxX,maxY);
		                  he->TransParentPolyon(&object);
                 }
                 else
                 {
                    m_dc->SetTextForeground(*wxBLACK);            // 0s --> 0x000000 (black)
                    m_dc->SetTextBackground(*wxWHITE);    // 1s --> 0xFFFFFF (white)
                    m_dc->SetLogicalFunction(wxAND_INVERT);

                    // BLACK OUT the opaque pixels and leave the rest as is
                    m_dc->DrawPolygon(points,todraw,0,0,wxWINDING_RULE);

                    // Set background and foreground colors for fill pattern
                    //the previous blacked out pixels are now merged with the layer color
                    //while the non blacked out pixels stay as they are.
                    m_dc->SetTextForeground(*wxBLACK);            // 0s --> 0x000000 (black)
                    m_dc->SetTextBackground(_brush->GetColour());
                    m_dc->SetLogicalFunction(wxOR);
                    m_dc->DrawPolygon(points,todraw,0,0,wxWINDING_RULE);
                    m_dc->SetLogicalFunction(wxCOPY);
                 }
                 m_dc->SetBrush(wxNullBrush);
            }
      }
   }
   if (points > 2)
   {
		todraw[points] = todraw[0];//for drawing the polyline around the polygon
      points++;
   }

	m_dc->SetPen(wxNullPen);
}

void DrawDriver::WindowsPolyline()
{
// are there points anyway?

	int points = _AI->count();
	if (!points)
		return;

	// now draw the wireframe
	_AI->tohead();
	double x,y;
	int i=0;
	while (!_AI->hitroot())
	{
   	x=LogicalToDeviceX(_AI->item()->GetX());
	   y=LogicalToDeviceY(_AI->item()->GetY());

		todraw[i].x=(long)x;
		todraw[i].y=(long)y;
		i++;
		(*_AI)++;
	}

   switch (_drawstyle)
   {
      case INVERT_WIREFRAME:
         m_dc->SetLogicalFunction(wxINVERT);
         m_dc->SetPen(*_pen);
         m_dc->SetBrush(*_brush);
         m_dc->DrawLines (points,todraw,0,0);
         m_dc->SetBrush(wxNullBrush);
         m_dc->SetPen(wxNullPen);
         break;
      case FILLED:
      case WIREFRAME:
	   case SELECTED:
      default:
         if(_Selected || _SRefSelected)
            m_dc->SetPen(*_penSelected);
         else
         	m_dc->SetPen(*_pen);
         m_dc->SetBrush(*_brush);
         m_dc->SetLogicalFunction(wxCOPY);
         m_dc->DrawLines (points,todraw,0,0);
         m_dc->SetBrush(wxNullBrush);
         m_dc->SetPen(wxNullPen);
   }
}

void DrawDriver::WindowsSpline()
{
    wxList sppoints;


// are there points anyway?

	int points = _AI->count();
	if (!points)
		return;

	// now draw the wireframe
	_AI->tohead();
	double x,y;
	while (!_AI->hitroot())
	{
   	 x=LogicalToDeviceX(_AI->item()->GetX());
	    y=LogicalToDeviceY(_AI->item()->GetY());

       wxPoint *point = new wxPoint;
       point->x = (int) x;
       point->y = (int) y;
       sppoints.Append((wxObject*)point);
		 (*_AI)++;
	}

   switch (_drawstyle)
   {
      case INVERT_WIREFRAME:
         m_dc->SetLogicalFunction(wxINVERT);
         m_dc->SetPen(*_pen);
         m_dc->SetBrush(*_brush);
         m_dc->DrawSpline(&sppoints);
         m_dc->SetBrush(wxNullBrush);
         m_dc->SetPen(wxNullPen);
         break;
      case FILLED:
      case WIREFRAME:
	   case SELECTED:
      default:
         if(_Selected || _SRefSelected)
            m_dc->SetPen(*_penSelected);
         else
         	m_dc->SetPen(*_pen);
         m_dc->SetBrush(*_brush);
         m_dc->SetLogicalFunction(wxCOPY);
         m_dc->DrawSpline(&sppoints);
         m_dc->SetBrush(wxNullBrush);
         m_dc->SetPen(wxNullPen);
   }

    for(wxNode *node = sppoints.GetFirst(); node; node = sppoints.GetFirst())
    {
        wxPoint *point = (wxPoint *)node->GetData();
        delete point;
        delete node;
    }
}



void	DrawDriver::DrawRect(int x1, int y1, int x2, int y2)
{
   switch (_drawstyle)
   {
      case INVERT_WIREFRAME:
      {
			wxBrush transp(*wxBLACK,wxTRANSPARENT); //dummy brush om mee de kleur van te zetten
         m_dc->SetBrush(transp);
//         m_dc->SetBrush(*_brush);
         m_dc->SetPen(*_pen);
         m_dc->SetLogicalFunction(wxINVERT);
         if (abs(x2-x1) < 1 && abs(y2-y1) < 1 )
            m_dc->DrawPoint(x1,y1); // uses the current pen
         else
         {
            long dx,dy;

            if (abs(x2-x1)<1) dx=1; else dx=abs(x2-x1);

            if (abs(y2-y1)<1) dy=1; else dy=y2-y1;
            m_dc->DrawRectangle(x1,y2,dx,-dy);

         }
         m_dc->SetBrush(wxNullBrush);
         m_dc->SetPen(wxNullPen);
         break;
      }
      case FILLED:
      case WIREFRAME:
	   case SELECTED:
      default:
         if(_Selected || _SRefSelected)
            m_dc->SetPen(*_penSelected);
         else
         	m_dc->SetPen(*_pen);
         m_dc->SetBrush(*_brush);
         m_dc->SetLogicalFunction(wxCOPY);
         if (abs(x2-x1) < 1 && abs(y2-y1) < 1 )
            m_dc->DrawPoint(x1,y1); // uses the current pen
         else
         {
            long dx,dy;

            if (abs(x2-x1)<1) dx=1; else dx=x2-x1;

            if (abs(y2-y1)<1) dy=1; else dy=y2-y1;
            m_dc->DrawRectangle(x1,y2,dx,-dy);

         }
         m_dc->SetBrush(wxNullBrush);
         m_dc->SetPen(wxNullPen);
   }
}

void DrawDriver::DrawGrid(double space, long color, int size)
{
	double _space = (space / GLOB->PHYSUNITS());
	long max_x = (long)GetVirtualSizeScreen().GetMax().GetX();
	long max_y = (long)GetVirtualSizeScreen().GetMax().GetY();
	long min_x = (long)GetVirtualSizeScreen().GetMin().GetX();
	long min_y = (long)GetVirtualSizeScreen().GetMin().GetY();
   while ( abs(max_x - min_x)/_space > 100.0 )
   	_space *= 10;

	long temp_x = (long)floor(min_x/_space);
	long temp_y = (long)floor(min_y/_space);
	min_x = (long)(temp_x*_space);
	min_y = (long)(temp_y*_space);

	wxColour gridcol;
   ltowxc(color,gridcol);
   _pengrid->SetColour(gridcol);
   m_dc->SetPen(*_pengrid);
	m_dc->SetLogicalFunction(wxCOPY);

	for (double i = min_x; i < max_x; i += _space)
   {
   	for (double j = min_y; j < max_y; j += _space)
      {
		   DrawPoint(i,j);
		}
	}
   m_dc->SetPen(wxNullPen);
}

// conversion from long (rgb encoded) to wxColour
void DrawDriver::ltowxc(long l,wxColour& color)
{
	unsigned char	red,green,blue;

	blue 	= (unsigned char)(l/65536);
	green = (unsigned char)((l%65536)/ 256);
	red 	= (unsigned char)(l%256);

	color.Set(red,green,blue);
}

// layer change: pen colors can be different
void	DrawDriver::SetLayer (int layer)
{
	static wxColour tmp(0,0,0);
	static wxColour backtmp(0,0,0);
	static wxColour backtmp2(255,255,255);
   if (layer==-1)
   {
	   _layer=layer;
      return;
   }
   if (_layer==layer)
      return;
   _layer=layer;

	_pattern = GLOB->SETUP->Get_Brush(layer);
	ltowxc(GLOB->SETUP->Get_BorderColor(layer),tmp);
	_pen->SetColour(tmp);
	ltowxc(GLOB->SETUP->Get_FillColor(layer),tmp);

  if (_pattern==1 || _pattern==0)
  {
  		_brush=_brushnorm;
  		_brush->SetColour(tmp);
  }
  else
  {
     _brushbitm->SetStipple(*(GLOB->FILLPAT->GetPattern(_pattern)));
//   _brushbitm->SetStyle(wxSTIPPLE_MASK_TRANSPARANT);
       _brushbitm->SetStyle(wxSTIPPLE_MASK_OPAQUE);
  		_brush=_brushbitm;
  		_brush->SetColour(tmp);
  }

  ltowxc(GLOB->SETUP->Get_BorderColor(layer),*_textColor);
}

// layer change: pen colors can be different
void	DrawDriver::UnSetLayer(int layer)
{
}

// pass the setup and make a device context
void DrawDriver::Init()
{
   //a dummy bitmap to create a wxSTIPPLE style
	_currentbitmap = new wxBitmap(32,32,1);
	_brushbitm = new wxBrush(*_currentbitmap);

	// create a pen and other stuff and select it
	wxColour	tmp("WHITE");
	_pen     = new wxPen(tmp,0,wxSOLID);
   _brushnorm = new wxBrush(tmp,wxSOLID); //dummy brush om mee de kleur van te zetten
	_pengrid = new wxPen(tmp,0,wxSOLID);
   _brush=_brushnorm;

	wxColour	tmp2("BLACK");
	_penorigin = new wxPen(tmp2,0,wxDOT);

	wxColour	tmp3("YELLOW");
//	_penSelected = new wxPen(tmp3,2,wxDOT);
	_penSelected = new wxPen(tmp3,2,wxSOLID);

   _pentrans= new wxPen(*wxWHITE,0,wxSOLID);
   _pentrans_sel= new wxPen(*wxWHITE,2,wxSOLID);

	_textColor = new wxColour("BLACK");
	_textSelectedColor = new wxColour("YELLOW");

   _layer=-1;
}

bool DrawDriver::Pending()
{
#ifndef __UNIX__
  //::wxYield();
  //TEST THE _keepdrawing MEMBER
  //if (_stopdrawing)
  //   return true;
 // return false;


  MSG current_msg;
  if (::PeekMessage(&current_msg, NULL, NULL, NULL, PM_NOREMOVE))
  {  //THERE ARE MESSAGES FOR THIS APPLICATION
     switch (current_msg.message )
     {
      case WM_LBUTTONDOWN   :   //keep this message to proceed later with it for zooming
      case WM_RBUTTONDOWN   :   //keep this message to show the popup dialog later
      {

  	  	  return true;
      }
      default:
            //all other messages need to be removed/ignored, to be able to be able
            //to peek for the above two messages
				::PeekMessage(&current_msg, NULL, NULL, NULL, PM_REMOVE);
     }
  }

  return false;

#else
/*
   ::wxYield();
  //TEST THE _keepdrawing MEMBER
  if (_stopdrawing)
     return true;
*/
  return false;
/*
    XEvent current_event;

    Display* display=XtDisplay(wxTheApp->topLevel);

    if (XCheckMaskEvent(display,ButtonPressMask,&current_event))
    {
      XPutBackEvent(display,&current_event);
      return true;
    }
*/
#endif

}
// draw text using the windows fonts
void	DrawDriver::GDSDrawText(const wxString& text,double x_min,double y_min,double x_max,double y_max,Text* the_text, wxTransformMatrix* _relative_matrix)
{
	double x_1, y_1, x_2, y_2, x_3, y_3;
	double height;

   int FontFamily=wxMODERN; //default does not rotate if to small

   switch (the_text->GetFont())
   {
      case 0 : FontFamily = wxMODERN; break;
//         case 0 : FontFamily = wxDEFAULT; break;
      case 1 : FontFamily = wxROMAN; break;
      case 2 : FontFamily = wxSWISS; break;
      case 3 : FontFamily = wxDECORATIVE; break;
      default: FontFamily = wxMODERN; break;
   }

   double rot_angle = atan2(_relative_matrix->GetValue(0,1),_relative_matrix->GetValue(0,0))*180/M_PI;
//   rot_angle = Check_int(rot_angle); 	 // kijk of dit een integer is (met bepaalde toegestane afwijking)

   Point	corners[4];
   Point textrot;
   corners[0].Set(x_min, y_min);   // corners[0] is left_bottom
   corners[1].Set(x_min, y_max);   // corners[1] is left_top
   corners[2].Set(x_max, y_min);   // corners[2] is right_bottom
   corners[3].Set(x_max, y_max);   // corners[3] is right_top

#ifdef __UNIX__
   textrot.Set(x_min, y_max);
#else
   textrot.Set(x_min, y_max+0.4);
#endif

   textrot.Transform(_relative_matrix);
   // Multiply the points with the relative matrix
   for (int i = 0; i < 4; i++)
   {
      corners[i].Transform(_relative_matrix);
   }
   x_min= corners[0].GetX();
   y_min= corners[0].GetY();
   x_1= corners[1].GetX();
   y_1= corners[1].GetY();
   x_2= corners[2].GetX();
   y_2= corners[2].GetY();
   x_3= corners[3].GetX();
   y_3= corners[3].GetY();

   height=sqrt(pow(x_min-x_1,2)+pow(y_min-y_1,2));
   double realheight = (int)((height) * m_scalefactor_x);         // adjust the 1.2 to your own taste

   if (realheight < 4 ) //always draw rectangle if small
   {
		if((_Selected || _SRefSelected))
	   	_pen->SetColour(*_textSelectedColor);
      else
		   _pen->SetColour(*_textColor);
      GDSDrawLine (x_min,y_min,x_1,y_1);
      GDSDrawLine (x_1,y_1,x_3,y_3);
      GDSDrawLine (x_3,y_3,x_2,y_2);
      GDSDrawLine (x_2,y_2,x_min,y_min);
   }
   else
   {
		if((_Selected || _SRefSelected))
      {
	   	_pen->SetColour(*_textSelectedColor);
         GDSDrawLine (x_min,y_min,x_1,y_1);
         GDSDrawLine (x_1,y_1,x_3,y_3);
         GDSDrawLine (x_3,y_3,x_2,y_2);
         GDSDrawLine (x_2,y_2,x_min,y_min);
      }
   }

   //	if (realheight > 8 && rot_angle==0) // if you can't read it, why draw it?
	if (realheight > 8)
   {
   	if (_drawstyle!=INVERT_WIREFRAME) //inverted text does not work
      {
         _font = new wxFont(realheight,FontFamily,wxNORMAL,wxNORMAL);
         m_dc->SetBrush(*_brush);
         m_dc->SetBackgroundMode(wxTRANSPARENT);
         m_dc->SetFont(*_font);
         if((_Selected || _SRefSelected))
            m_dc->SetTextForeground(*_textSelectedColor);
         else
            m_dc->SetTextForeground(*_textColor);

//         double xt=x_1;
//         double yt=y_1;
         double xt;
         double yt;
      	xt=LogicalToDeviceX(textrot.GetX());
	      yt=LogicalToDeviceY(textrot.GetY());

         //make not zero because it does not work alike
         if (rot_angle == 0)
	         m_dc->DrawRotatedText(text.c_str(),(long)xt,(long)(yt),rot_angle+0.1);
         else
	         m_dc->DrawRotatedText(text.c_str(),(long)xt,(long)(yt),rot_angle);
//this has another refernce point
//	      m_dc->DrawText(text,(long)xt,(long)(yt-realheight));
         m_dc->SetBackgroundMode(wxSOLID);
         m_dc->SetFont(wxNullFont);
         m_dc->SetBrush(wxNullBrush);
         delete _font;
         _font = NULL;
      }
   }
   _pen->SetColour(*_textColor);

}

// draw a line
void	DrawDriver::GDSDrawLine(double x1, double y1, double x2, double y2)
{

	if (_clip_status == _ON)
		if (!Clipping(x1, y1, x2, y2))
			return;

   switch (_drawstyle)
   {
      case INVERT_WIREFRAME:
         SetLogicalFunction(wxINVERT);
         SetPen(*_pen);
         SetBrush(*_brush);
			DrawLine(x1,y1,x2,y2);
         SetBrush(wxNullBrush);
         SetPen(wxNullPen);
         break;
      case FILLED:
      case WIREFRAME:
	   case SELECTED:
      default:
         if(_Selected || _SRefSelected)
            SetPen(*_penSelected);
         else
         	SetPen(*_pen);
         SetBrush(*_brush);
         SetLogicalFunction(wxCOPY);
			DrawLine(x1,y1,x2,y2);
         SetBrush(wxNullBrush);
         SetPen(wxNullPen);
   }
}

FillPatterns::FillPatterns()
{
	wxString	pathfile;
   wxString gdsroot = wxGetenv("GDSROOT");
	if ( gdsroot.IsEmpty() ) exit(0);

	gdsroot = gdsroot + "/default/gfx/";
   gdsroot = ConvertToUnixPath( gdsroot );

	for (int i = 0; i < NR_PATTERNS; i++)
   {

		pathfile.Printf( "%spat%-2.2d.bmp", gdsroot.c_str(), i+1 );

		wxImage image;
   	image.LoadFile(pathfile,wxBITMAP_TYPE_BMP);

   	_fillbitmaps[i] = new wxBitmap( image );

      //set mask to monochrome bitmap based on color bitmap
		wxColour white("WHITE");
		wxColour	black("BLACK");

      wxMask* monochrome_mask = new wxMask(*_fillbitmaps[i],black);
      _fillbitmaps[i]->SetMask(monochrome_mask);
    }
}

FillPatterns::~FillPatterns()
{
	for (int i = 0; i < NR_PATTERNS; i++)
		delete _fillbitmaps[i];
}

wxBitmap* FillPatterns::GetPattern(short patternnr)
{
	return _fillbitmaps[patternnr];
}

