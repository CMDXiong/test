#ifdef __GNUG__
#pragma implementation "doubledc.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
   #include "wx/window.h"
#endif

#ifdef __WXMSW__
   #include "wx/msw/private.h"
#endif

#include "wx/dc.h"

#include "doubledc.h"


wxDoubleDCBase::wxDoubleDCBase()
{
    m_fontsize=0;

    m_clipping = FALSE;
    m_ok = TRUE;

    m_dc=0;

    ResetBoundingBox();

    m_logicalFunction = wxCOPY;

    m_backgroundMode = wxTRANSPARENT;

    m_mappingMode = wxMM_TEXT;

    m_backgroundBrush = *wxTRANSPARENT_BRUSH;

    m_textForegroundColour = *wxBLACK;
    m_textBackgroundColour = *wxWHITE;

    m_colour = wxColourDisplay();
}

wxDoubleDCBase::~wxDoubleDCBase()
{
}

wxDC* wxDoubleDCBase::SetDC(wxDC* newdc)
{
    wxDC* old=m_dc;
    m_dc=newdc;
    if (m_clipping)
    {
      //a little bigger then strictly needed
        SetClippingRegion(m_clipX1,m_clipY1,m_clipX2,m_clipY2);
    }
    else
    {
        DestroyClippingRegion();
    }
    ResetBoundingBox();

    SetLogicalFunction(m_logicalFunction);

    SetBackgroundMode(m_backgroundMode);

    m_mappingMode=wxMM_TEXT;
    if (m_dc)
	    m_dc->SetMapMode(m_mappingMode);

    SetBackground(m_backgroundBrush);

    SetTextForeground(m_textForegroundColour);
    SetTextBackground(m_textBackgroundColour);

    m_colour = wxColourDisplay();

    return old;
}

void wxDoubleDCBase::ExtendClippingRegion(double x, double y)
{
    if (m_clipping)
    {
        if ( x < m_clipX1 ) m_clipX1 = x;
        if ( y < m_clipY1 ) m_clipY1 = y;
        if ( x > m_clipX2 ) m_clipX2 = x;
        if ( y > m_clipY2 ) m_clipY2 = y;
    }
    else
    {
        m_clipping = true;

        m_clipX2=x;
        m_clipX1=x;
        m_clipY2=y;
        m_clipY1=y;
    }

}

void wxDoubleDCBase::SetClippingRegionWin(wxCoord minx,wxCoord miny,wxCoord maxx,wxCoord maxy)
{
   m_clipping=true;

   m_clipX1=DeviceToLogicalX(minx-2);
   m_clipX2=DeviceToLogicalX(maxx+2);
   m_clipY2=DeviceToLogicalY(miny-2);
   m_clipY1=DeviceToLogicalY(maxy+2);
   if (m_dc)
   {
        m_dc->SetClippingRegion(minx-2,maxy-2,maxx-minx+4, maxy-miny+4);
   }
}

void wxDoubleDCBase::SetClippingRegion(double minx,double miny,double maxx,double maxy)
{
   m_clipping=true;

   m_clipX2=maxx;
   m_clipX1=minx;
   m_clipY2=maxy;
   m_clipY1=miny;

   if (m_dc)
   {
        minx=LogicalToDeviceX(minx);
        miny=LogicalToDeviceY(miny);
        maxx=LogicalToDeviceX(maxx);
        maxy=LogicalToDeviceY(maxy);
        m_dc->SetClippingRegion(minx-2,maxy-2,maxx-minx+4, miny-maxy+4);
   }
}



void wxDoubleDCBase::DestroyClippingRegion()
{
    m_clipping=false;
    m_clipX2=m_virt_maxX;
    m_clipX1=m_virt_minX;
    m_clipY2=m_virt_maxY;
    m_clipY1=m_virt_minY;
    if (m_dc)
    {
        m_dc->DestroyClippingRegion();
    }
}

void wxDoubleDCBase::ClearClippingRegion()
{
    if (!m_dc)
        return;
    wxPen _clearpen(m_dc->GetBackground().GetColour(),1,wxSOLID);
    m_dc->SetBrush(m_backgroundBrush);
    m_dc->SetLogicalFunction(wxCOPY);

    m_dc->SetPen(_clearpen);
    if (m_clipX1 && m_clipX2 && m_clipY1 && m_clipY2)
    {
        double ymin,xmin,xmax,ymax;
        xmin=LogicalToDeviceX(m_clipX1);
        ymin=LogicalToDeviceY(m_clipY1);
        xmax=LogicalToDeviceX(m_clipX2);
        ymax=LogicalToDeviceY(m_clipY2);

        m_dc->DrawRectangle((wxCoord)(xmin-2),(wxCoord)(ymax-2),(wxCoord)(xmax-xmin+4),(wxCoord)(ymin-ymax+4));
    }
    else
        m_dc->Clear();
   //restore to what it was
   SetLogicalFunction(wxCOPY);
	m_dc->SetBrush(wxNullBrush);
	m_dc->SetPen(wxNullPen);
}

void wxDoubleDCBase::Clear()
{
	m_dc->SetBackground(m_backgroundBrush);
	m_dc->Clear();
	m_dc->SetBackground(wxNullBrush);
}

// maps the virtual window (Real drawing to the window coordinates
// also used for zooming
void wxDoubleDCBase::SetMapping(  int dwxi, int dwyi,
										double vx1,
										double vy1,
										double vx2,
										double vy2,
										bool border)
{
    if (vx2==vx1) vx2=vx1+100000;
    if (vy2==vy1) vy2=vy1+100000;
    m_virt_minX=vx1;
    m_virt_minY=vy1;
    m_virt_maxX=vx2;
    m_virt_maxY=vy2;

    double dwx=dwxi;
    double dwy=dwyi;
    if (dwx==0) dwx=1;
    if (dwy==0) dwy=1;

	 double dvx = m_virt_maxX - m_virt_minX;
	 double dvy = m_virt_maxY - m_virt_minY;

	 // calculate the scaling factor for the virtual window
	 double temp_x=0;
	 double temp_y=0;
    if ((dvy / dvx) < (dwy / dwx))
    {
        dvy = dvx * (dwy / dwx);
        // calculate the change in the coordinates
        temp_y = (dvy - (m_virt_maxY - m_virt_minY) )/ 2.0;
    }
    else
    {
        dvx = dvy * (dwx / dwy);
        // calculate the change in the coordinates
        temp_x = (dvx - (m_virt_maxX - m_virt_minX) )/ 2.0;
    }

    // add or substract the change from the original coordinates
    m_virt_minX=m_virt_minX-temp_x;
    m_virt_minY=m_virt_minY-temp_y;

    m_virt_maxX=m_virt_maxX+temp_x;
    m_virt_maxY=m_virt_maxY+temp_y;

    // initialize the mapping_matrix used for mapping the
    // virtual windows to the drawing window

	 // make mappingmatrix
    m_mapping_matrix.Identity();
    if (!border)
        m_mapping_matrix.Translate(-m_virt_minX,-m_virt_maxY);
    else
    {
        // make a small white border around the drawing
        m_virt_minX=m_virt_minX- 0.05 * dvx;
        m_virt_minY=m_virt_minY- 0.05 * dvy;

        m_virt_maxX=m_virt_maxX+ 0.05 * dvx;
        m_virt_maxY=m_virt_maxY+ 0.05 * dvy;

        // translate the drawing to 0,0
        m_mapping_matrix.Translate(-m_virt_minX,-m_virt_maxY);
    }

    m_scalefactor_x = dwx;
    m_scalefactor_x /= (m_virt_maxX - m_virt_minX);

    m_scalefactor_y = dwy;
    m_scalefactor_y /= (m_virt_maxY - m_virt_minY);

    // scale the drawing so it fit's in the window
    m_mapping_matrix.Scale(m_scalefactor_x, m_scalefactor_y, 0, 0);

    // because of coordinate change mirror over X
    // 0,0 in graphic computerscreens: upperleft corner
    // 0,0 in cartesian: lowerleft corner
    m_mapping_matrix.Mirror();

    // make inverse of mapping matrix
    // this is to set coordinates in the statusbar
    // and the calculate screencoordinates to world coordinates used
    // in zooming
    m_inverse_mapping=m_mapping_matrix;
    m_inverse_mapping.Invert();
 	 DestroyClippingRegion();
}


// return the boundingbox of the canvas in world coordinates
void wxDoubleDCBase::GetVirtualSize(double& x, double& y,
                                  double& w, double& h) const
{
    if ( m_isVirtualValid )
    {
      x = m_virt_minX;
      y = m_virt_minY;
      w = m_virt_maxX-m_virt_minX;
      h = m_virt_maxY-m_virt_minY;
    }
    else
    {
      x = y = w = h = 0;
    }
}


// return the inverse mapping matrix for zooming or coordinates
wxTransformMatrix wxDoubleDCBase::GetInverseMappingMatrix()
{
	return m_inverse_mapping;
}

wxTransformMatrix wxDoubleDCBase::GetMappingMatrix()
{
	return m_mapping_matrix;
}

// Clipping is done here
// Function return a value of where the point is in the space.
// The screen is the one with the double lines below (in the middle)
// If the given point is on the right of this window -> 2 is returned
int 	wxDoubleDCBase::GetClipCode(double x, double y)
{
	//    9    |     8    |    10
	//         |          |
	//  ------============--------- Y-max
	//    1   ||     0   ||     2
	//        ||         ||
	//  ------============--------- Y-min
	//    5    |     4    |     6
	//         |          |
	//        X-min      X-max
	//
	int p = 0;
	if (x < m_clipX1)
		p |= 0x0001;
	else if (x > m_clipX2)
		p |= 0x0002;
	if (y < m_clipY1)
		p |= 0x0004;
	else if (y > m_clipY2)
		p |= 0x0008;

	return p;
}


// return true if the point is inside the window
bool wxDoubleDCBase::Clipping(double &x, double &y)
{
	if (!GetClipCode(x, y))
		return true;	// draw the point
	return false;		// point is outside the window: do not draw
}


// return true if the line is inside the window
bool wxDoubleDCBase::Clipping(double &x1,
									double &y1,
									double &x2,
									double &y2)
{
	int clipcode1, clipcode2;
	int nr_of_intersections = 0;

	// intersection points are used if the line falls from the screen
	double intersectionpoint1[2];
	double intersectionpoint2[2];
	double A, B, temppoint;
	double RC = 0;
	double T = 0;

	// call clipcode from each point from the line
	clipcode1 = GetClipCode(x1, y1);
	clipcode2 = GetClipCode(x2, y2);

	// are both points within the window?
	if (!clipcode1 && !clipcode2)	return true;
		// then draw the line

	// is point (x1,y1) inside the window?
	if (!clipcode1) {
		intersectionpoint1[0] = x1;
		intersectionpoint2[0] = y1;

		nr_of_intersections = 1;
	}

	// is point (x2,y2) inside the window?
	if (!clipcode2) {
		intersectionpoint1[0] = x2;
		intersectionpoint2[0] = y2;

		nr_of_intersections = 1;
	}

	clipcode1 ^= clipcode2;

	// We can make the following linefunction:
	// 	y = (A/B)x + C
	// calculate the slope of the line
	A = y2 - y1;
	B = x2 - x1;

	if (B) {
		RC = A/B;	// RC is slope
		T = y1 - x1 * RC;
	}

	if (clipcode1 & 0x0004) {
		// intersect lowerbound with the line
		if (B) temppoint = (m_clipY1 - T) / RC;
		else temppoint = x1;		// perpendicular line

		// calculate intersection points
		if (temppoint >= m_clipX1 && temppoint <= m_clipX2) {
			intersectionpoint1[nr_of_intersections] = temppoint;
			intersectionpoint2[nr_of_intersections++] = m_clipY1;
		}
	}

	if (clipcode1 & 0x0008)	{
		// intersect upperbound with the line
		if (B) temppoint = (m_clipY2 - T) / RC;
		else temppoint = x1;		// loodrechte lijn

		if (temppoint >= m_clipX1 && temppoint <= m_clipX2) {
			intersectionpoint1[nr_of_intersections] = temppoint;
			intersectionpoint2[nr_of_intersections++] = m_clipY2;
		}
	}

	if (clipcode1 & 0x0001)	{
		// intersect leftbound with the line
		temppoint = RC * m_clipX1 + T;
		if (temppoint > m_clipY1 && temppoint < m_clipY2) {
			intersectionpoint2[nr_of_intersections] = temppoint;
			intersectionpoint1[nr_of_intersections++] = m_clipX1;
		}
	}

	if (clipcode1 & 0x0002)	{
		// intersect rightbound with the line
		temppoint = RC * m_clipX2 + T;
		if (temppoint > m_clipY1 && temppoint < m_clipY2) {
			intersectionpoint2[nr_of_intersections] = temppoint;
			intersectionpoint1[nr_of_intersections++] = m_clipX2;
		}
	}

	if (nr_of_intersections != 2)	return false;

	// set the new values in the parameters (these are references)
	x1 = intersectionpoint1[0];
	y1 = intersectionpoint2[0];
	x2 = intersectionpoint1[1];
	y2 = intersectionpoint2[1];

	return true;
}

void wxDoubleDCBase::GetClippingBox(double& x, double& y,
                            double& w, double& h) const
{
   x = m_clipX1;
   y = m_clipY1;
   w = m_clipX2 - m_clipX1;
   h = m_clipY2 - m_clipY1;
}

void wxDoubleDCBase::FloodFill(double x, double y, const wxColour& col,int style )
{
   m_dc->FloodFill(LogicalToDeviceX(x), LogicalToDeviceY(y), col,style );
}

void wxDoubleDCBase::FloodFill(const wxPoint2DDouble& pt, const wxColour& col, int style )
{
   m_dc->FloodFill(LogicalToDeviceX(pt.m_x), LogicalToDeviceY(pt.m_y), col,style );
}

bool wxDoubleDCBase::GetPixel(double x, double y, wxColour *col)
{
   return m_dc->GetPixel(LogicalToDeviceX(x), LogicalToDeviceY(y), col);
}

bool wxDoubleDCBase::GetPixel(const wxPoint2DDouble& pt, wxColour *col)
{
   return m_dc->GetPixel(LogicalToDeviceX(pt.m_x), LogicalToDeviceY(pt.m_y), col);
}

void wxDoubleDCBase::DrawLine(double x1, double y1, double x2, double y2)
{
	if (!Clipping(x1, y1, x2, y2))
			return;

    m_dc->DrawLine(LogicalToDeviceX(x1), LogicalToDeviceY(y1),LogicalToDeviceX(x2), LogicalToDeviceY(y2));

}

void wxDoubleDCBase::DrawLine(const wxPoint2DDouble& pt1, const wxPoint2DDouble& pt2)
{
    m_dc->DrawLine(LogicalToDeviceX(pt1.m_x), LogicalToDeviceY(pt1.m_y),
                   LogicalToDeviceX(pt2.m_x), LogicalToDeviceY(pt2.m_y));
}

void wxDoubleDCBase::CrossHair(double x, double y)
{
    m_dc->CrossHair(LogicalToDeviceX(x), LogicalToDeviceY(y));
}

void wxDoubleDCBase::CrossHair(const wxPoint2DDouble& pt)
{
    m_dc->CrossHair(LogicalToDeviceX(pt.m_x), LogicalToDeviceY(pt.m_y));
}

void wxDoubleDCBase::DrawArc(double x1, double y1, double x2, double y2,
           double xc, double yc)
{
    m_dc->DrawArc(LogicalToDeviceX(x1), LogicalToDeviceY(y1),LogicalToDeviceX(x2), LogicalToDeviceY(y2),
                  LogicalToDeviceX(xc), LogicalToDeviceY(yc));
}

void wxDoubleDCBase::DrawArc(const wxPoint2DDouble& pt1, const wxPoint2DDouble& pt2, const wxPoint2DDouble& centre)
{
    m_dc->DrawArc(LogicalToDeviceX(pt1.m_x), LogicalToDeviceY(pt1.m_y),
                  LogicalToDeviceX(pt2.m_x), LogicalToDeviceY(pt2.m_y),
                  LogicalToDeviceX(centre.m_x), LogicalToDeviceY(centre.m_y));
}

void wxDoubleDCBase::DrawCheckMark(double x, double y,double width, double height)
{
   width=width*m_scalefactor_x;
   height=height*m_scalefactor_y;
   m_dc->DrawCheckMark(LogicalToDeviceX(x), LogicalToDeviceY(y),width,height);

}

void wxDoubleDCBase::DrawCheckMark(const wxRect2DDouble& rect)
{
   double width = rect.m_width;
   double height = rect.m_height;
   width=width*m_scalefactor_x;
   height=height*m_scalefactor_y;
   m_dc->DrawCheckMark(LogicalToDeviceX(rect.m_x), LogicalToDeviceY(rect.m_y),width,height);
}


void wxDoubleDCBase::DrawEllipticArc(double x, double y, double w, double h,double sa, double ea)
{
    m_dc->DrawEllipticArc(LogicalToDeviceX(x), LogicalToDeviceY(y),
                          LogicalToDeviceX(w), LogicalToDeviceY(h),
                          LogicalToDeviceX(sa), LogicalToDeviceY(ea));
}

void wxDoubleDCBase::DrawEllipticArc(const wxPoint2DDouble& pt, const wxSize& sz,double sa, double ea)
{
    m_dc->DrawEllipticArc(LogicalToDeviceX(pt.m_x), LogicalToDeviceY(pt.m_y),
                          LogicalToDeviceX(sz.GetWidth()), LogicalToDeviceY(sz.GetHeight()),
                          LogicalToDeviceX(sa), LogicalToDeviceY(ea));
}

void wxDoubleDCBase::DrawPoint(double x, double y)
{
    m_dc->DrawPoint(LogicalToDeviceX(x), LogicalToDeviceY(y));
}

void wxDoubleDCBase::DrawPoint(const wxPoint2DDouble& pt)
{
    m_dc->DrawPoint(LogicalToDeviceX(pt.m_x), LogicalToDeviceY(pt.m_y));
}

void wxDoubleDCBase::DrawLines(int n, wxPoint2DDouble points[],double xoffset, double yoffset )
{
    wxPoint *cpoints = new wxPoint[n];
    int i;
    for (i = 0; i < n; i++)
    {
        CalcBoundingBox(points[i].m_x+xoffset, points[i].m_y+yoffset);
        cpoints[i].x = (int)LogicalToDeviceX(points[i].m_x);
        cpoints[i].y = (int)LogicalToDeviceY(points[i].m_y);
    }
    m_dc->DrawLines(n,cpoints,(int)LogicalToDeviceXRel(xoffset),(int)LogicalToDeviceYRel(yoffset));
    delete [] cpoints;
}

void wxDoubleDCBase::DrawLines(const wxList *list,double xoffset , double yoffset )
{
    int n = list->GetCount();
    wxPoint* cpoints = new wxPoint[n];

    int i = 0;
    for ( wxNode *node = list->GetFirst(); node; node = node->GetNext(), i++ )
    {
        wxPoint2DDouble *point = (wxPoint2DDouble *)node->GetData();
        cpoints[i].x = (int)LogicalToDeviceX(point->m_x);
        cpoints[i].y = (int)LogicalToDeviceY(point->m_y);
    }
    m_dc->DrawLines(n,cpoints,(int)LogicalToDeviceXRel(xoffset),(int)LogicalToDeviceYRel(yoffset));

    delete [] cpoints;
}


void wxDoubleDCBase::DrawPolygon(int n, wxPoint2DDouble points[],
               double xoffset , double yoffset,
               int fillStyle)
{
    wxPoint* cpoints = new wxPoint[n];

    int i;
    for (i = 0; i < n; i++)
    {
        CalcBoundingBox(points[i].m_x+xoffset, points[i].m_y+yoffset);
        cpoints[i].x = (int)LogicalToDeviceX(points[i].m_x);
        cpoints[i].y = (int)LogicalToDeviceY(points[i].m_y);
    }
    m_dc->DrawPolygon(n,cpoints,(int)LogicalToDeviceXRel(xoffset),(int)LogicalToDeviceYRel(-yoffset),fillStyle);
    delete [] cpoints;
}


void wxDoubleDCBase::DrawPolygon(const wxList *list,
               double xoffset , double yoffset,
               int fillStyle )
{
    int n = list->GetCount();
    wxPoint *cpoints = new wxPoint[n];

    int i = 0;
    for ( wxNode *node = list->GetFirst(); node; node = node->GetNext(), i++ )
    {
        wxPoint2DDouble *point = (wxPoint2DDouble *)node->GetData();
        cpoints[i].x = (int)LogicalToDeviceX(point->m_x);
        cpoints[i].y = (int)LogicalToDeviceY(point->m_y);
    }

    m_dc->DrawPolygon(n,cpoints,(int)LogicalToDeviceXRel(xoffset),(int)LogicalToDeviceXRel(yoffset),fillStyle);
    delete [] cpoints;

}


void wxDoubleDCBase::DrawRectangle(double x, double y, double width, double height)
{
   width=width*m_scalefactor_x;
   height=-height*m_scalefactor_y;
   m_dc->DrawRectangle(LogicalToDeviceX(x), LogicalToDeviceY(y),width,height);
}

void wxDoubleDCBase::DrawRectangle(const wxPoint2DDouble& pt, const wxSize& sz)
{
   double width = sz.GetWidth();
   double height =sz.GetHeight();
   width=width*m_scalefactor_x;
   height=-height*m_scalefactor_y;
   m_dc->DrawRectangle(LogicalToDeviceX(pt.m_x), LogicalToDeviceY(pt.m_y),width,height);
}

void wxDoubleDCBase::DrawRectangle(const wxRect2DDouble& rect)
{
   double width = rect.m_width;
   double height = rect.m_height;
   width=width*m_scalefactor_x;
   height=-height*m_scalefactor_y;
   m_dc->DrawRectangle(LogicalToDeviceX(rect.m_x), LogicalToDeviceY(rect.m_y),width,height);
}


void wxDoubleDCBase::DrawRoundedRectangle(double x, double y, double width, double height,
                        double radius)
{
   width=width*m_scalefactor_x;
   height=-height*m_scalefactor_y;
   radius=radius*m_scalefactor_x;
   m_dc->DrawRoundedRectangle(LogicalToDeviceX(x), LogicalToDeviceY(y),width,height,radius);
}

void wxDoubleDCBase::DrawRoundedRectangle(const wxPoint2DDouble& pt, const wxSize& sz,
                       double radius)
{
   double width = sz.GetWidth();
   double height =sz.GetHeight();
   width=width*m_scalefactor_x;
   height=-height*m_scalefactor_y;
   radius=radius*m_scalefactor_x;
   m_dc->DrawRoundedRectangle(LogicalToDeviceX(pt.m_x), LogicalToDeviceY(pt.m_y),width,height,radius);
}

void wxDoubleDCBase::DrawRoundedRectangle(const wxRect2DDouble& rect, double radius)
{
   double width = rect.m_width;
   double height = rect.m_height;
   width=width*m_scalefactor_x;
   height=-height*m_scalefactor_y;
   radius=radius*m_scalefactor_x;
   m_dc->DrawRoundedRectangle(LogicalToDeviceX(rect.m_x), LogicalToDeviceY(rect.m_y),width,height,radius);
}


void wxDoubleDCBase::DrawCircle(double x, double y, double radius)
{
   radius=radius*m_scalefactor_x;
   m_dc->DrawCircle(LogicalToDeviceX(x), LogicalToDeviceY(y),radius);
}

void wxDoubleDCBase::DrawEllipse(double x, double y, double width, double height)
{
   width=width*m_scalefactor_x;
   height=-height*m_scalefactor_y;
   m_dc->DrawEllipse(LogicalToDeviceX(x), LogicalToDeviceY(y),width,height);
}

void wxDoubleDCBase::DrawEllipse(const wxPoint2DDouble& pt, const wxSize& sz)
{
   double width = sz.GetWidth();
   double height =sz.GetHeight();
   width=width*m_scalefactor_x;
   height=-height*m_scalefactor_y;
   m_dc->DrawEllipse(LogicalToDeviceX(pt.m_x), LogicalToDeviceY(pt.m_y),width,height);
}

void wxDoubleDCBase::DrawEllipse(const wxRect2DDouble& rect)
{
   double width = rect.m_width;
   double height = rect.m_height;
   width=width*m_scalefactor_x;
   height=-height*m_scalefactor_y;
   m_dc->DrawEllipse(LogicalToDeviceX(rect.m_x), LogicalToDeviceY(rect.m_y),width,height);
}


/*
void wxDoubleDCBase::DrawIcon(const wxIcon& icon, double x, double y)
{
}

void wxDoubleDCBase::DrawIcon(const wxIcon& icon, const wxPoint2DDouble& pt)
{

}


void wxDoubleDCBase::DrawBitmap(const wxBitmap &bmp, double x, double y,
              bool useMask)
{

}

void wxDoubleDCBase::DrawBitmap(const wxBitmap &bmp, const wxPoint2DDouble& pt,
              bool useMask)
{
}
*/

void wxDoubleDCBase::DrawText(const wxString& text, double x, double y)
{
    //now set the font size according to the current scale factors
    double height=m_fontsize*m_scalefactor_y;
    m_font.SetPointSize(height);
    m_dc->SetFont(m_font);
    m_dc->DrawText(text.c_str(),LogicalToDeviceX(x), LogicalToDeviceY(y));
}

void wxDoubleDCBase::DrawText(const wxString& text, const wxPoint2DDouble& pt)
{
    //now set the font size according to the current scale factors
    double height=m_fontsize*m_scalefactor_y;
    m_font.SetPointSize(height);
    m_dc->SetFont(m_font);
    m_dc->DrawText(text.c_str(),LogicalToDeviceX(pt.m_x), LogicalToDeviceY(pt.m_y)-height);
}


void wxDoubleDCBase::DrawRotatedText(const wxString& text, double x, double y, double angle)
{
    //now set the font size according to the current scale factors
    double height=m_fontsize*m_scalefactor_y;
    m_font.SetPointSize(height);
    m_dc->SetFont(m_font);
    m_dc->DrawRotatedText(text.c_str(),LogicalToDeviceX(x), LogicalToDeviceY(y),angle);
}

void wxDoubleDCBase::DrawRotatedText(const wxString& text, const wxPoint2DDouble& pt, double angle)
{
    //now set the font size according to the current scale factors
    double height=m_fontsize*m_scalefactor_y;
    m_font.SetPointSize(height);
    m_dc->SetFont(m_font);
    m_dc->DrawRotatedText(text.c_str(),LogicalToDeviceX(pt.m_x), LogicalToDeviceY(pt.m_y),angle);
}


#if wxUSE_SPLINES
void wxDoubleDCBase::DrawSpline(int n, wxPoint2DDouble points[])
{
    wxList list;
    for (int i =0; i < n; i++)
    {
        points[i].m_x=LogicalToDeviceX(points[i].m_x);
        points[i].m_y=LogicalToDeviceX(points[i].m_y);
        list.Append((wxObject*)&points[i]);
    }

    m_dc->DrawSpline(&list);


}

/*
void wxDoubleDCBase::DrawSpline(wxList *points)
{



}
*/
#endif // wxUSE_SPLINES

// global DC operations
// --------------------

// set objects to use for drawing
// ------------------------------

void wxDoubleDCBase::SetTextForeground(const wxColour& colour)
{
   m_textForegroundColour = colour;
   if (m_dc)
      m_dc->SetTextForeground(m_textForegroundColour);
}

void wxDoubleDCBase::SetTextBackground(const wxColour& colour)
{
   m_textBackgroundColour = colour;
   if (m_dc)
      m_dc->SetTextBackground(m_textBackgroundColour);
}

void wxDoubleDCBase::SetFont(const wxFont& font,double size)
{
	m_font=font;
    m_fontsize=size;

    if (m_dc)
       m_dc->SetFont(m_font);
}

void wxDoubleDCBase::SetPen(const wxPen& pen)
{
	m_pen=pen;
    if (m_dc)
       m_dc->SetPen(m_pen);
}

void wxDoubleDCBase::SetBrush(const wxBrush& brush)
{
	m_brush=brush;
    if (m_dc)
       m_dc->SetBrush(m_brush);
}

void wxDoubleDCBase::SetBackground(const wxBrush& brush)
{
	m_backgroundBrush=brush;
    if (m_dc)
       m_dc->SetBackground(m_backgroundBrush);
}

void wxDoubleDCBase::SetBackgroundMode(int mode)
{
	m_mappingMode=mode;
    if (m_dc)
       m_dc->SetBackgroundMode(m_mappingMode);
}

void wxDoubleDCBase::SetPalette(const wxPalette& palette)
{
	m_palette=palette;
    if (m_dc)
       m_dc->SetPalette(m_palette);

}

// text extent
// -----------
/*
double wxDoubleDCBase::GetCharHeight() const
{

}

double wxDoubleDCBase::GetCharWidth() const
{
}


void wxDoubleDCBase::GetTextExtent(const wxString& string,
                 double *x, double *y,
                 double *descent,
                 double *externalLeading,
                 wxFont *theFont) const
{
}


// size and resolution
// -------------------

// in device units
void wxDoubleDCBase::GetSize(int *width, int *height) const
{

}



wxSize wxDoubleDCBase::GetSize() const
{

}

// in mm
void wxDoubleDCBase::GetSizeMM(int* width, int* height) const
{

}




wxSize wxDoubleDCBase::GetSizeMM() const
{

}
*/

// coordinates conversions
// -----------------------
double wxDoubleDCBase::DeviceToLogicalX(double x) const
{
	return m_inverse_mapping.GetValue(0,0) * x + m_inverse_mapping.GetValue(2,0) + 0.5;
}

double wxDoubleDCBase::DeviceToLogicalY(double y) const
{
	return m_inverse_mapping.GetValue(1,1) * y + m_inverse_mapping.GetValue(2,1) + 0.5;
}

double wxDoubleDCBase::DeviceToLogicalXRel(double x) const
{
	return x*m_scalefactor_x;
}

double wxDoubleDCBase::DeviceToLogicalYRel(double y) const
{
	return y*m_scalefactor_y;
}

double wxDoubleDCBase::LogicalToDeviceX(double x) const
{
	return m_mapping_matrix.GetValue(0,0) * x + m_mapping_matrix.GetValue(2,0) + 0.5;
}

double wxDoubleDCBase::LogicalToDeviceY(double y) const
{
	return m_mapping_matrix.GetValue(1,1) * y + m_mapping_matrix.GetValue(2,1) + 0.5;
}

double wxDoubleDCBase::LogicalToDeviceXRel(double x) const
{
	return x*m_scalefactor_x;
}

double wxDoubleDCBase::LogicalToDeviceYRel(double y) const
{
	return y*m_scalefactor_y;
}

/*
// Resolution in Pixels per inch
wxSize wxDoubleDCBase::GetPPI() const
{

}
*/

void wxDoubleDCBase::SetLogicalFunction(int function)
{
	m_logicalFunction=function;
   if (m_dc)
      m_dc->SetLogicalFunction(m_logicalFunction);
}



