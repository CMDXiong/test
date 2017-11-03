/////////////////////////////////////////////////////////////////////////////
// Name:        dcbuf.cpp
// Purpose:     wxClientDCbuf class
// Author:      Klaas Holwerda
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "dcclbuf.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

//*************************************************************************************
//MSW version
//*************************************************************************************
#if defined(__WXMSW__) && !defined(__GNUWIN32__)

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/string.h"
#include "wx/log.h"
#include "wx/window.h"

#include "wx/msw/private.h"

    #include "wx/dc.h"
#include "msw/dcclbuf.h"
#include <typeinfo.h>


// ----------------------------------------------------------------------------
// array/list types
// ----------------------------------------------------------------------------

struct WXDLLEXPORT wxPaintDCInfo
{
    wxPaintDCInfo(wxWindow *win, wxDC *dc)
    {
        hwnd = win->GetHWND();
        hdc = dc->GetHDC();
        count = 1;
    }

    WXHWND    hwnd;       // window for this DC
    WXHDC     hdc;        // the DC handle
    size_t    count;      // usage count
};

#include "wx/arrimpl.cpp"

//WX_DEFINE_OBJARRAY(wxArrayDCInfo);

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

    IMPLEMENT_DYNAMIC_CLASS(wxWindowDCbuf, wxDC)
    IMPLEMENT_DYNAMIC_CLASS(wxClientDCbuf, wxWindowDCbuf)
    IMPLEMENT_DYNAMIC_CLASS(wxPaintDCbuf, wxWindowDCbuf)

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

static PAINTSTRUCT g_paintStruct;

//#ifdef __WXDEBUG__
//    int g_isPainting = 0;
//#endif // __WXDEBUG__

// ===========================================================================
// implementation
// ===========================================================================

// ----------------------------------------------------------------------------
// wxWindowDCbuf
// ----------------------------------------------------------------------------

wxWindowDCbuf::wxWindowDCbuf()
{
  m_canvas = NULL;
}

wxWindowDCbuf::wxWindowDCbuf(wxWindow *the_canvas)
{
  m_canvas = the_canvas;
  m_hDC = (WXHDC) ::GetWindowDC(GetWinHwnd(the_canvas) );

  SetBackground(wxBrush(m_canvas->GetBackgroundColour(), wxSOLID));
}

wxWindowDCbuf::~wxWindowDCbuf()
{
  if (m_canvas && m_hDC)
  {
    SelectOldObjects(m_hDC);

    if ( !::ReleaseDC(GetWinHwnd(m_canvas), GetHdc()) )
    {
        wxLogLastError(wxT("ReleaseDC"));
    }

    m_hDC = 0;
  }

}

void wxWindowDCbuf::SelectObject( wxBitmap& bitmap )
{
	m_buffer_dc.SelectObject(bitmap);
    m_bitmap=(wxBitmap*)&bitmap;
}

void wxWindowDCbuf::SetTextForeground(const wxColour& colour)
{
	wxDC::SetTextForeground(colour);
   m_buffer_dc.SetTextForeground(colour);
}

void wxWindowDCbuf::SetTextBackground(const wxColour& colour)
{
	wxDC::SetTextBackground(colour);
   m_buffer_dc.SetTextBackground(colour);
}

void wxWindowDCbuf::DoSetClippingRegion(wxCoord cx, wxCoord cy, wxCoord cw, wxCoord ch)
{
	wxDC::DoSetClippingRegion(cx,cy,cw,ch);
	m_buffer_dc.SetClippingRegion(cx,cy,cw,ch);
}

void wxWindowDCbuf::DoSetClippingRegionAsRegion(const wxRegion& region)
{
	wxDC::DoSetClippingRegionAsRegion(region);
	m_buffer_dc.SetClippingRegion(region);
}

void wxWindowDCbuf::DestroyClippingRegion()
{
	wxDC::DestroyClippingRegion();
	m_buffer_dc.DestroyClippingRegion();
}

// ---------------------------------------------------------------------------
// drawing
// ---------------------------------------------------------------------------

void wxWindowDCbuf::Clear()
{
    wxDC::Clear();
    m_buffer_dc.Clear();
}

bool wxWindowDCbuf::DoFloodFill(wxCoord x, wxCoord y, const wxColour& col, int style)
{
	 wxDC::DoFloodFill(x,y,col,style);
	 return m_buffer_dc.FloodFill(x,y,col,style);
}

void wxWindowDCbuf::DoCrossHair(wxCoord x, wxCoord y)
{
	wxDC::DoCrossHair(x,y);
	m_buffer_dc.CrossHair(x,y);
}

void wxWindowDCbuf::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
	wxDC::DoDrawLine(x1,y1,x2,y2);
	m_buffer_dc.DrawLine(x1,y1,x2,y2);
}

void wxWindowDCbuf::DoDrawArc(wxCoord x1,wxCoord y1,wxCoord x2,wxCoord y2, wxCoord xc, wxCoord yc)
{
	wxDC::DoDrawArc(x1,y1,x2,y2,xc,yc);
	m_buffer_dc.DrawArc(x1,y1,x2,y2,xc,yc);
}

void wxWindowDCbuf::DoDrawCheckMark(wxCoord x1, wxCoord y1,
                           wxCoord width, wxCoord height)
{
	wxDC::DoDrawCheckMark(x1,y1,width,height);
	m_buffer_dc.DrawCheckMark(x1,y1,width,height);
}

void wxWindowDCbuf::DoDrawPoint(wxCoord x, wxCoord y)
{
	wxDC::DoDrawPoint(x,y);
	m_buffer_dc.DrawPoint(x,y);
}

void wxWindowDCbuf::DoDrawPolygon(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset,int fillStyle)
{

//after wx2.1b16

   m_buffer_dc.ResetBoundingBox();
	m_buffer_dc.DrawPolygon(n,points,xoffset,yoffset,fillStyle);
   wxDC::DoBlit(m_buffer_dc.MinX()-2, m_buffer_dc.MinY()-2,
   			  m_buffer_dc.MaxX()-m_buffer_dc.MinX()+4,
  			  m_buffer_dc.MaxY()-m_buffer_dc.MinY()+4,
   			  &m_buffer_dc,m_buffer_dc.MinX()-2, m_buffer_dc.MinY()-2,wxCOPY,false);

//else this
//	m_buffer_dc.DrawPolygon(n,points,xoffset,yoffset,fillStyle);
//	wxDC::DoDrawPolygon(n,points,xoffset,yoffset,fillStyle);
}

void wxWindowDCbuf::DoDrawLines(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset)
{
	wxDC::DoDrawLines(n,points,xoffset,yoffset);
//   m_buffer_dc.ResetBoundingBox();
	m_buffer_dc.DrawLines(n,points,xoffset,yoffset);
/*
   wxDC::Blit(m_buffer_dc.MinX(), m_buffer_dc.MinY(),
   			  m_buffer_dc.MaxX()-m_buffer_dc.MinX()+1,
   			  m_buffer_dc.MaxY()-m_buffer_dc.MinY()+1,
   			  &m_buffer_dc,m_buffer_dc.MinX(), m_buffer_dc.MinY(),wxCOPY,false);
*/
}

void wxWindowDCbuf::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
	wxDC::DoDrawRectangle(x,y,width,height);
	m_buffer_dc.DrawRectangle(x,y,width,height);
}

void wxWindowDCbuf::DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius)
{
	wxDC::DoDrawRoundedRectangle(x,y,width,height,radius);
	m_buffer_dc.DrawRoundedRectangle(x,y,width,height,radius);
}

void wxWindowDCbuf::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
	wxDC::DoDrawEllipse(x,y,width,height);
	m_buffer_dc.DrawEllipse(x,y,width,height);
}

void wxWindowDCbuf::DoDrawEllipticArc(wxCoord x,wxCoord y,wxCoord w,wxCoord h,double sa,double ea)
{
	wxDC::DoDrawEllipticArc(x,y,w,h,sa,ea);
	m_buffer_dc.DrawEllipticArc(x,y,w,h,sa,ea);
}

void wxWindowDCbuf::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{
	wxDC::DoDrawIcon(icon,x,y);
	m_buffer_dc.DrawIcon(icon,x,y);
}

void wxWindowDCbuf::DoDrawBitmap( const wxBitmap &bmp, wxCoord x, wxCoord y, bool useMask )
{
	m_buffer_dc.DrawBitmap(bmp,x,y,useMask);
//	wxDC::DoDrawBitmap(bmp,x,y,useMask);
   wxDC::DoBlit(x,y,
   			  bmp.GetWidth(),bmp.GetHeight(),
   			  &m_buffer_dc,x,y,wxCOPY,false);

}

void wxWindowDCbuf::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{
	wxDC::DoDrawText(text,x,y);
	m_buffer_dc.DrawText(text,x,y);
}


void wxWindowDCbuf::DoDrawRotatedText(const wxString& text,
                             wxCoord x, wxCoord y,
                             double angle)
{
	wxDC::DoDrawRotatedText(text,x,y,angle);
	m_buffer_dc.DrawRotatedText(text,x,y,angle);
}

// ---------------------------------------------------------------------------
// set GDI objects
// ---------------------------------------------------------------------------

void wxWindowDCbuf::SetPalette(const wxPalette& palette)
{
	wxDC::SetPalette(palette);
	m_buffer_dc.SetPalette(palette);
}

void wxWindowDCbuf::SetFont(const wxFont& the_font)
{
	wxDC::SetFont(the_font);
	m_buffer_dc.SetFont(the_font);
}

void wxWindowDCbuf::SetPen(const wxPen& pen)
{
	wxDC::SetPen(pen);
	m_buffer_dc.SetPen(pen);
}

void wxWindowDCbuf::SetBrush(const wxBrush& brush)
{
	wxDC::SetBrush(brush);
	m_buffer_dc.SetBrush(brush);
}

void wxWindowDCbuf::SetBackground(const wxBrush& brush)
{
	wxDC::SetBackground(brush);
	m_buffer_dc.SetBackground(brush);
}

void wxWindowDCbuf::SetBackgroundMode(int mode)
{
	wxDC::SetBackgroundMode(mode);
	m_buffer_dc.SetBackgroundMode(mode);
}

void wxWindowDCbuf::SetLogicalFunction(int function)
{
	wxDC::SetLogicalFunction(function);
	m_buffer_dc.SetLogicalFunction(function);
}

void wxWindowDCbuf::SetMapMode(int mode)
{
	wxDC::SetMapMode(mode);
	m_buffer_dc.SetMapMode(mode);
}

void wxWindowDCbuf::SetUserScale(double x, double y)
{
	wxDC::SetUserScale(x,y);
	m_buffer_dc.SetUserScale(x,y);
	wxDC::SetMapMode(m_mappingMode);
	m_buffer_dc.SetMapMode(m_mappingMode);
}

void wxWindowDCbuf::SetAxisOrientation(bool xLeftRight, bool yBottomUp)
{
	wxDC::SetAxisOrientation(xLeftRight,yBottomUp);
	m_buffer_dc.SetAxisOrientation(xLeftRight,yBottomUp);
	wxDC::SetMapMode(m_mappingMode);
	m_buffer_dc.SetMapMode(m_mappingMode);
}

void wxWindowDCbuf::SetSystemScale(double x, double y)
{
	wxDC::SetSystemScale(x,y);
	m_buffer_dc.SetSystemScale(x,y);
	wxDC::SetMapMode(m_mappingMode);
	m_buffer_dc.SetMapMode(m_mappingMode);
}

void wxWindowDCbuf::SetLogicalScale(double x, double y)
{
	wxDC::SetLogicalScale(x,y);
	m_buffer_dc.SetLogicalScale(x,y);
}

void wxWindowDCbuf::SetLogicalOrigin(wxCoord x, wxCoord y)
{
	wxDC::SetLogicalOrigin(x,y);
	m_buffer_dc.SetLogicalOrigin(x,y);
}

void wxWindowDCbuf::SetDeviceOrigin(wxCoord x, wxCoord y)
{
	wxDC::SetDeviceOrigin(x,y);
	m_buffer_dc.SetDeviceOrigin(x,y);
}

bool wxWindowDCbuf::DoBlit(wxCoord xdest, wxCoord ydest,
                  wxCoord width, wxCoord height,
                  wxDC *source, wxCoord xsrc, wxCoord ysrc,
                  int rop, bool useMask, wxCoord xsrcMask, wxCoord ysrcMask)
{
	m_buffer_dc.Blit(xdest,ydest,width,height,source,xsrc,ysrc,rop,useMask, xsrcMask, ysrcMask);
	return wxDC::DoBlit(xdest,ydest,width,height,source,xsrc,ysrc,rop,useMask, xsrcMask, ysrcMask);
}

#if wxUSE_SPLINES

void wxWindowDCbuf::DoDrawSpline(wxList *list)
{
    wxDC::DoDrawSpline(list);
    m_buffer_dc.DrawSpline(list);
}

#endif // wxUSE_SPLINES

void wxTransparentPolygon::Draw(wxWindowDCbuf& dc) const
{
}

void wxWindowDCbuf::TransParentPolyon(wxTransparentPolygon* thepolygon)
{
	DrawableObject(thepolygon);
}

void wxWindowDCbuf::DrawableObject(wxDrawableObject* drawobject)
{
    switch (drawobject->GetType())
    {
        case 1://transparent polygon
        {

            wxTransparentPolygon* tp =(wxTransparentPolygon*)drawobject;


            m_buffer_dc.ResetBoundingBox();
            m_buffer_dc.CalcBoundingBox(drawobject->MinX(),drawobject->MinY());
            m_buffer_dc.CalcBoundingBox(drawobject->MaxX(),drawobject->MaxY());

            m_buffer_dc.SetTextForeground(*wxBLACK);            // 0s --> 0x000000 (black)
            m_buffer_dc.SetTextBackground(*wxWHITE);    // 1s --> 0xFFFFFF (white)
            m_buffer_dc.SetLogicalFunction(wxAND_INVERT);

            // BLACK OUT the opaque pixels and leave the rest as is
            //m_buffer_dc.DrawPolygon(tp->n,tp->points,0,0,wxWINDING_RULE);

            //the above
            COLORREF colFgOld,colBgOld;

            colFgOld = ::GetTextColor((struct HDC__ *) m_buffer_dc.GetHDC());
            colBgOld = ::GetBkColor((struct HDC__ *) m_buffer_dc.GetHDC());

            int prev;

            if (m_buffer_dc.GetTextForeground().Ok())
            {   //just the oposite from what is expected see help on pattern brush
               // 1 in mask becomes bk color
               ::SetBkColor((struct HDC__ *) m_buffer_dc.GetHDC(), m_buffer_dc.GetTextForeground().GetPixel() );
            }
            if (m_buffer_dc.GetTextBackground().Ok())
            {   //just the oposite from what is expected
               // 0 in mask becomes text color
               ::SetTextColor((struct HDC__ *) m_buffer_dc.GetHDC(), m_buffer_dc.GetTextBackground().GetPixel() );
            }
            prev = SetPolyFillMode((struct HDC__ *) m_buffer_dc.GetHDC(),WINDING);
            (void)Polygon((struct HDC__ *) m_buffer_dc.GetHDC(), (POINT*)tp->points, tp->n);
            SetPolyFillMode((struct HDC__ *) m_buffer_dc.GetHDC(),prev);

            // Set background and foreground colors for fill pattern
            //the previous blacked out pixels are now merged with the layer color
            //while the non blacked out pixels stay as they are.
            m_buffer_dc.SetTextForeground(*wxBLACK);            // 0s --> 0x000000 (black)
            m_buffer_dc.SetTextBackground(GetBrush().GetColour());
            m_buffer_dc.SetLogicalFunction(wxOR);

            if (m_buffer_dc.GetTextForeground().Ok())
            {   //just the oposite from what is expected see help on pattern brush
               // 1 in mask becomes bk color
               ::SetBkColor((struct HDC__ *) m_buffer_dc.GetHDC(), m_buffer_dc.GetTextForeground().GetPixel() );
            }
            if (m_buffer_dc.GetTextBackground().Ok())
            {   //just the oposite from what is expected
               // 0 in mask becomes text color
               ::SetTextColor((struct HDC__ *) m_buffer_dc.GetHDC(), m_buffer_dc.GetTextBackground().GetPixel() );
            }
            (void)Polygon((struct HDC__ *) m_buffer_dc.GetHDC(),(POINT*)tp->points, tp->n);
            SetPolyFillMode((struct HDC__ *) m_buffer_dc.GetHDC(),prev);

            //m_buffer_dc.DrawPolygon(tp->n,tp->points,0,0,wxWINDING_RULE);
            m_buffer_dc.SetLogicalFunction(wxCOPY);

            wxDC::DoBlit(m_buffer_dc.MinX()-2, m_buffer_dc.MinY()-2,
                     m_buffer_dc.MaxX()-m_buffer_dc.MinX()+4,
                  m_buffer_dc.MaxY()-m_buffer_dc.MinY()+4,
                     &m_buffer_dc,m_buffer_dc.MinX()-2, m_buffer_dc.MinY()-2,wxCOPY,false);


            // restore the colours we changed
            ::SetBkMode((struct HDC__ *) m_buffer_dc.GetHDC(), TRANSPARENT);
            ::SetTextColor((struct HDC__ *) m_buffer_dc.GetHDC(), colFgOld);
            ::SetBkColor((struct HDC__ *) m_buffer_dc.GetHDC(), colBgOld);
            break;
        }
        case 2://trial
        {

            m_buffer_dc.SetBrush(wxNullBrush);
            wxBrush a(*(m_bitmap));
            m_buffer_dc.SetBrush(a);
            wxTransparentPolygon* tp =(wxTransparentPolygon*)drawobject;
            m_buffer_dc.ResetBoundingBox();
            m_buffer_dc.SetLogicalFunction(wxCOPY);
            m_buffer_dc.DrawPolygon(tp->n,tp->points,0,0,wxWINDING_RULE);

            wxDC::DoBlit(m_buffer_dc.MinX()-2, m_buffer_dc.MinY()-2,
                     m_buffer_dc.MaxX()-m_buffer_dc.MinX()+4,
                  m_buffer_dc.MaxY()-m_buffer_dc.MinY()+4,
                     &m_buffer_dc,m_buffer_dc.MinX()-2, m_buffer_dc.MinY()-2,wxCOPY,false);
            m_buffer_dc.SetBrush(wxNullBrush);
            break;
        }
        default:
            drawobject->Draw(*this);
    }
}
/*
void wxWindowDCbuf::DrawObject(wxDrawObject* drawobject)
{
    switch (drawobject->GetType())
    {
        case 1://transparent polygon
        {
            wxTransparentPolygon* tp =(wxTransparentPolygon*)drawobject;
            m_buffer_dc.ResetBoundingBox();
            m_buffer_dc.SetTextForeground(*wxBLACK);            // 0s --> 0x000000 (black)
            m_buffer_dc.SetTextBackground(*wxWHITE);    // 1s --> 0xFFFFFF (white)
            m_buffer_dc.SetLogicalFunction(wxAND_INVERT);

            // BLACK OUT the opaque pixels and leave the rest as is
            m_buffer_dc.DrawPolygon(tp->n,tp->points,0,0,wxWINDING_RULE);

            // Set background and foreground colors for fill pattern
            //the previous blacked out pixels are now merged with the layer color
            //while the non blacked out pixels stay as they are.
            m_buffer_dc.SetTextForeground(*wxBLACK);            // 0s --> 0x000000 (black)
            m_buffer_dc.SetTextBackground(GetBrush().GetColour());
            m_buffer_dc.SetLogicalFunction(wxOR);
            m_buffer_dc.DrawPolygon(tp->n,tp->points,0,0,wxWINDING_RULE);
            m_buffer_dc.SetLogicalFunction(wxCOPY);

            wxDC::DoBlit(m_buffer_dc.MinX()-2, m_buffer_dc.MinY()-2,
                     m_buffer_dc.MaxX()-m_buffer_dc.MinX()+4,
                  m_buffer_dc.MaxY()-m_buffer_dc.MinY()+4,
                     &m_buffer_dc,m_buffer_dc.MinX()-2, m_buffer_dc.MinY()-2,wxCOPY,false);
            break;
        }
        default:
            drawobject->Draw(*this);
    }
}
*/

//clear the clippingregion using background color of canvas
void wxWindowDCbuf::ClearClippingRegion()
{
	SetLogicalFunction(wxCOPY);
   //a little bigger then strictly needed
   m_buffer_dc.SetBrush(GetBackground());
   m_buffer_dc.DrawRectangle(m_clipX1-2,m_clipY1-2,m_clipX2-m_clipX1+4,m_clipY2-m_clipY1+4);
   m_buffer_dc.SetBrush(wxNullBrush);
   SetBrush(GetBackground());
   DrawRectangle(m_clipX1-2,m_clipY1-2,m_clipX2-m_clipX1+4,m_clipY2-m_clipY1+4);
   SetBrush(wxNullBrush);
}


// ----------------------------------------------------------------------------
// wxClientDCbuf
// ----------------------------------------------------------------------------

wxClientDCbuf::wxClientDCbuf()
{
   m_canvas = NULL;
   m_bitmaptomove=0;
   m_backgrnd=0;
   m_first_move=false;
	m_use_redraw=false;
   m_last_x=0;
   m_last_y=0;
}

wxClientDCbuf::wxClientDCbuf(wxWindow *the_canvas)
{
  m_canvas = the_canvas;
  m_hDC = (WXHDC) ::GetDC(GetWinHwnd(the_canvas));

  // the background mode is only used for text background
  // and is set in DrawText() to OPAQUE as required, other-
  // wise always TRANSPARENT, RR
  ::SetBkMode( GetHdc(), TRANSPARENT );

  SetBackground(wxBrush(m_canvas->GetBackgroundColour(), wxSOLID));
}

wxClientDCbuf::~wxClientDCbuf()
{
  if ( m_canvas && GetHdc() )
  {
    SelectOldObjects(m_hDC);

    ::ReleaseDC(GetWinHwnd(m_canvas), GetHdc());
    m_hDC = 0;
  }
}

bool wxClientDCbuf::MoveObject(wxCoord xdest, wxCoord ydest,wxBitmap& bitmap,
                         bool use_redrawonmove,
      						 wxCoord xsrc, wxCoord ysrc,
                         wxBitmap* backgrnd,
                         bool returnbackgrnd
                        )
{
   int w=bitmap.GetWidth();
   int h=bitmap.GetHeight();
   m_bitmaptomove=&bitmap;
	return DoMoveObject(xdest,ydest,w,h,use_redrawonmove,xsrc,ysrc,backgrnd,returnbackgrnd);
}

bool wxClientDCbuf::MoveObject(wxCoord xdest, wxCoord ydest,int w, int h,
                         bool use_redrawonmove,
      						 wxCoord xsrc, wxCoord ysrc,
                         wxBitmap* backgrnd,
                         bool returnbackgrnd
                        )
{
   m_bitmaptomove=0;
	return DoMoveObject(xdest,ydest,w,h,use_redrawonmove,xsrc,ysrc,backgrnd,returnbackgrnd);
}

bool wxClientDCbuf::DoMoveObject(wxCoord xdest, wxCoord ydest,int w, int h,
                         bool use_redrawonmove,
      						 wxCoord xsrc, wxCoord ysrc,
                         wxBitmap* backgrnd,
                         bool returnbackgrnd
                        )
{
   wxMemoryDC tmp;
   //first redraw what should be behind the drawing to Move
   if (use_redrawonmove)
   {
      SetClippingRegion(xsrc,ysrc,w,h);
      //only using the memory DC, later it will be blitted to the canvas also
		SetLogicalFunction(wxCOPY);
      //m_buffer_dc.SetBrush(GetBackground());
      //m_buffer_dc.DrawRectangle(xsrc-2,ysrc-2,w+4,h+4);
      //m_buffer_dc.SetBrush(wxNullBrush);
	   RedrawOnMove();
      DestroyClippingRegion();
   }
   else if (backgrnd)
   {
	   wxASSERT_MSG( backgrnd->Ok(), _T("invalid backgrnd bitmap in wxWindowDCbuf::MoveBitmap") );
      //redraw with backgrnd bitmap what is at the xsrc,ysrc
	   tmp.SelectObject(*backgrnd);
	   m_buffer_dc.Blit(xsrc,ysrc,w,h,&tmp,0,0,wxCOPY,false);
	   tmp.SelectObject(wxNullBitmap);
   }
   else
   {
       //clear the background (the area behind the bitmap) using background color of canvas
       //a little bigger then strictly needed
       SetClippingRegion(xsrc,ysrc,w,h);
		 SetLogicalFunction(wxCOPY);
       m_buffer_dc.SetBrush(GetBackground());
       m_buffer_dc.DrawRectangle(xsrc-2,ysrc-2,w+4,h+4);
       m_buffer_dc.SetBrush(wxNullBrush);
       DestroyClippingRegion();
   }

   //the backgrnd will be filled with the m_buffer contents at the newposition of the moved bitmap
   //before it is really moved
   if( returnbackgrnd )
   {
	   wxASSERT_MSG( backgrnd->Ok(), _T("invalid backgrnd bitmap in wxWindowDCbuf::MoveBitmap") );
	   tmp.SelectObject(*backgrnd);
	   tmp.Blit(0,0,w,h,&m_buffer_dc,xdest,ydest,wxCOPY,false);
	   tmp.SelectObject(wxNullBitmap);
   }

	wxInt32 leftu,rightu,bottomu,topu ;
	leftu = wxMin ( xsrc , xdest ) ;
	rightu = wxMax (xsrc+w, xdest+w) ;
	topu = wxMin ( ysrc , ydest ) ;
	bottomu = wxMax (ysrc+h, ydest+h) ;

   if (m_bitmaptomove)
   {
      //blit the bitmap at the new position to the m_buffer
      m_buffer_dc.DrawBitmap(*m_bitmaptomove,xdest,ydest,m_withmask);
      //or like this
      //tmp.SelectObject(bitmap);
      //m_buffer_dc.Blit(xdest,ydest,w,h,&tmp,0,0,wxCOPY,withmask);
      //tmp.SelectObject(wxNullBitmap);
   }
   else //moving a drawing
   {
      //draw the object
      //only using the memory DC, later it will be blitted to the canvas also
      SetLogicalFunction(wxCOPY);
      SetClippingRegion(xdest,ydest,w,h);
      DrawOnMove(xdest,ydest);
      DestroyClippingRegion();
   }

	if ( rightu - leftu < 2*w && bottomu - topu < 2*h )
	{  //intersecting
      //the union of both the new place and the redraw at the old place at once to the canvas
		wxDC::DoBlit(leftu,topu,rightu - leftu,bottomu - topu,&m_buffer_dc,leftu,topu,wxCOPY,false);
	}
   else
   {  //do them seperate
      //now first redraw what should be at the old position in the canvas
      wxDC::DoBlit(xsrc,ysrc,w,h,&m_buffer_dc,xsrc,ysrc,wxCOPY,false);
	   //blit the bitmap to the new place in the canvas
      wxDC::DoBlit(xdest,ydest,w,h,&m_buffer_dc,xdest,ydest,wxCOPY,false);
   }


   return true;
}

void wxClientDCbuf::InitiateDragObject(int first_x,int first_y,wxBitmap* source ,bool withmask,bool use_redraw)
{
   //prepare a bitmap to be used for drawing what was behind the bitmap
   m_bitmaptomove=source;
   wxMemoryDC tmp;
   int w=source->GetWidth();
   int h=source->GetHeight();
   m_backgrnd= new wxBitmap(w,h);
   //the backgrnd will be filled with the m_buffer contents at the first position
   tmp.SelectObject(*m_backgrnd);
   tmp.Blit(0,0,w,h,&m_buffer_dc,first_x,first_y,wxCOPY,false);
   tmp.SelectObject(wxNullBitmap);
   m_withmask= withmask;
   m_first_move=true;
	m_use_redraw=use_redraw;
   m_last_x=first_x;
   m_last_y=first_y;
}

void wxClientDCbuf::InitiateDragObject(int first_x,int first_y,int w, int h,bool use_redraw)
{
   //prepare a bitmap to be used for drawing what was behind the bitmap
   m_bitmaptomove=0;
   wxMemoryDC tmp;
   m_backgrnd= new wxBitmap(w,h);
   //the backgrnd will be filled with the m_buffer contents at the first position
   tmp.SelectObject(*m_backgrnd);
   tmp.Blit(0,0,w,h,&m_buffer_dc,first_x,first_y,wxCOPY,false);
   tmp.SelectObject(wxNullBitmap);
   m_first_move=true;
	m_use_redraw=use_redraw;
   m_last_x=first_x;
   m_last_y=first_y;
}

void wxClientDCbuf::EndDragObject()
{
   m_first_move=true;
   //restore what is in the backgrnd bitmap which was behind the bitmap
   //moved to the last position
   DrawBitmap(*m_backgrnd,m_last_x,m_last_y,0);
   delete m_backgrnd;
}

bool wxClientDCbuf::DragObject(wxCoord xdest, wxCoord ydest)
{
   wxASSERT_MSG( m_bitmaptomove->Ok(), _T("invalid bitmap in wxClientDCbuf::DragBitmap") );
   wxASSERT_MSG( m_backgrnd->Ok(), _T("invalid m_backgrnd bitmap in wxClientDCbuf::DragObject") );
   int w=m_backgrnd->GetWidth();
   int h=m_backgrnd->GetHeight();

   wxMemoryDC tmp;
   if (m_first_move && m_use_redraw)
   {
	   m_first_move=false;
      //first redraw what should be behind the bitmap to drag
      //redraw the background using the RedrawOnMove function
      SetClippingRegion(m_last_x,m_last_y,w,h);
      //only using the memory DC, later it will be blitted to the canvas also
      SetLogicalFunction(wxCOPY);
      //m_buffer_dc.SetBrush(GetBackground());
      //m_buffer_dc.DrawRectangle(xsrc-2,ysrc-2,w+4,h+4);
      //m_buffer_dc.SetBrush(wxNullBrush);
      RedrawOnMove();
      DestroyClippingRegion();
   }
   else
   {
      wxASSERT_MSG( m_backgrnd->Ok(), _T("invalid backgrnd bitmap in wxClientDCbuf::DragObject") );
      //redraw with backgrnd bitmap what is at the xsrc,ysrc
      tmp.SelectObject(*m_backgrnd);
      m_buffer_dc.Blit(m_last_x,m_last_y,w,h,&tmp,0,0,wxCOPY,false);
      tmp.SelectObject(wxNullBitmap);
   }

   //the backgrnd will be filled with the m_buffer contents at the newposition of the moved object
   //before it is really moved
   tmp.SelectObject(*m_backgrnd);
   tmp.Blit(0,0,w,h,&m_buffer_dc,xdest,ydest,wxCOPY,false);
   tmp.SelectObject(wxNullBitmap);

	wxInt32 leftu,rightu,bottomu,topu ;
	leftu = wxMin ( m_last_x, xdest ) ;
	rightu = wxMax (m_last_x+w, xdest+w) ;
	topu = wxMin ( m_last_y , ydest ) ;
	bottomu = wxMax (m_last_y+h, ydest+h) ;

   if (m_bitmaptomove)
   {
      //blit the bitmap at the new position to the m_buffer
      m_buffer_dc.DrawBitmap(*m_bitmaptomove,xdest,ydest,m_withmask);
      //or like this
      //tmp.SelectObject(bitmap);
      //m_buffer_dc.Blit(xdest,ydest,w,h,&tmp,0,0,wxCOPY,withmask);
      //tmp.SelectObject(wxNullBitmap);
   }
   else //moving a drawing
   {
      //draw the object
      //only using the memory DC, later it will be blitted to the canvas also
      SetLogicalFunction(wxCOPY);
      SetClippingRegion(xdest,ydest,w,h);
      DrawOnMove(xdest,ydest);
      DestroyClippingRegion();
   }

	if ( rightu - leftu < 2*w && bottomu - topu < 2*h )
	{  //intersecting
      //the union of both the new place and the redraw at the old place at once to the canvas
		wxDC::DoBlit(leftu,topu,rightu - leftu,bottomu - topu,&m_buffer_dc,leftu,topu,wxCOPY,false);
	}
   else
   {  //do them seperate
      //now first redraw what should be at the old position in the canvas
      wxDC::DoBlit(m_last_x,m_last_y,w,h,&m_buffer_dc,m_last_x,m_last_y,wxCOPY,false);
	   //blit the bitmap to the new place in the canvas
	   //blit the bitmap to the new place in the canvas
      wxDC::DoBlit(xdest,ydest,w,h,&m_buffer_dc,xdest,ydest,wxCOPY,false);
   }
   m_last_x=xdest;
   m_last_y=ydest;

   return true;
}



wxArrayDCInfo wxPaintDCbuf::ms_cache;

wxPaintDCbuf::wxPaintDCbuf()
{
    m_canvas = NULL;
    m_hDC = 0;
}

wxPaintDCbuf::wxPaintDCbuf(wxWindow *canvas)
{
    wxCHECK_RET( canvas, wxT("NULL canvas in wxPaintDCbuf ctor") );

#ifdef __WXDEBUG__
    if ( g_isPainting <= 0 )
    {
        wxFAIL_MSG( wxT("wxPaintDCbuf may be created only in EVT_PAINT handler!") );

        return;
    }
#endif // __WXDEBUG__

    m_canvas = canvas;

    // do we have a DC for this window in the cache?
    wxPaintDCInfo *info = FindInCache();
    if ( info )
    {
        m_hDC = info->hdc;
        info->count++;
    }
    else // not in cache, create a new one
    {
        m_hDC = (WXHDC)::BeginPaint(GetWinHwnd(m_canvas), &g_paintStruct);
        ms_cache.Add(new wxPaintDCInfo(m_canvas, this));
    }

    // the background mode is only used for text background
    // and is set in DrawText() to OPAQUE as required, other-
    // wise always TRANSPARENT, RR
    ::SetBkMode( GetHdc(), TRANSPARENT );

    SetBackground(wxBrush(m_canvas->GetBackgroundColour(), wxSOLID));
}

wxPaintDCbuf::~wxPaintDCbuf()
{
    if ( m_hDC )
    {
        SelectOldObjects(m_hDC);

        size_t index;
        wxPaintDCInfo *info = FindInCache(&index);

        wxCHECK_RET( info, wxT("existing DC should have a cache entry") );

        if ( !--info->count )
        {
            ::EndPaint(GetHwndOf(m_canvas), &g_paintStruct);

            ms_cache.RemoveAt(index);

            // Reduce the number of bogus reports of non-freed memory
            // at app exit
            if (ms_cache.IsEmpty())
                ms_cache.Clear();
        }
        //else: cached DC entry is still in use

        // prevent the base class dtor from ReleaseDC()ing it again
        m_hDC = 0;
    }
}

wxPaintDCInfo *wxPaintDCbuf::FindInCache(size_t *index) const
{
    wxPaintDCInfo *info = NULL;
    size_t nCache = ms_cache.GetCount();
    for ( size_t n = 0; n < nCache; n++ )
    {
        info = &ms_cache[n];
        if ( info->hwnd == m_canvas->GetHWND() )
        {
            if ( index )
                *index = n;
            break;
        }
    }

    return info;
}

//*************************************************************************************
//GTK version
//*************************************************************************************
#elif defined(__WXGTK__) || defined(__GNUWIN32__)


#include "gtk/dcclbuf.h"
#include "wx/dcmemory.h"
#include "wx/image.h"
#include "wx/module.h"


#include <math.h>               // for floating-point functions


//-----------------------------------------------------------------------------
// local defines
//-----------------------------------------------------------------------------

#define USE_PAINT_REGION 1

//-----------------------------------------------------------------------------
// local data
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static inline double dmax(double a, double b) { return a > b ? a : b; }
static inline double dmin(double a, double b) { return a < b ? a : b; }

static inline double DegToRad(double deg) { return (deg * M_PI) / 180.0; }

//-----------------------------------------------------------------------------
// temporary implementation of the missing GDK function
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// wxWindowDCbuf
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxWindowDCbuf, wxDC)

void wxTransparentPolygon::Draw(wxWindowDCbuf& dc) const
{
}


wxWindowDCbuf::wxWindowDCbuf():wxWindowDC()
{

}

wxWindowDCbuf::wxWindowDCbuf( wxWindow *window ):wxWindowDC( window )
{
  
}

wxWindowDCbuf::~wxWindowDCbuf()
{
 
}

void wxWindowDCbuf::DoGetSize(int *width, int *height) const
{
	m_buffer_dc.GetSize( width, height);
}

bool wxWindowDCbuf::DoGetPixel( wxCoord x1, wxCoord y1, wxColour *col ) const
{
	m_buffer_dc.GetPixel( x1, y1, col ); 
}

void wxWindowDCbuf::SelectObject(wxBitmap& bitmap)
{
	m_buffer_dc.SelectObject(bitmap);
}

void wxWindowDCbuf::DoDrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 )
{
	wxWindowDC::DoDrawLine(x1,y1,x2,y2);
	m_buffer_dc.DrawLine(x1,y1,x2,y2); 
}

bool wxWindowDCbuf::DoFloodFill(wxCoord x, wxCoord y, const wxColour& col, int style)
{
	 wxWindowDC::DoFloodFill(x,y,col,style);
	 return m_buffer_dc.FloodFill(x,y,col,style);
}

void wxWindowDCbuf::DoCrossHair( wxCoord x, wxCoord y )
{
	wxWindowDC::DoCrossHair(x,y);
	m_buffer_dc.CrossHair(x,y);
}

void wxWindowDCbuf::DoDrawCheckMark(wxCoord x1, wxCoord y1,
                           wxCoord width, wxCoord height)
{
	wxWindowDC::DoDrawCheckMark(x1,y1,width,height);
	m_buffer_dc.DrawCheckMark(x1,y1,width,height);
}


void wxWindowDCbuf::DoDrawArc( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2,
                            wxCoord xc, wxCoord yc )
{
	wxWindowDC::DoDrawArc(x1,y1,x2,y2,xc,yc);
	m_buffer_dc.DrawArc(x1,y1,x2,y2,xc,yc);
}

void wxWindowDCbuf::DoDrawEllipticArc( wxCoord x, wxCoord y, wxCoord width, wxCoord height, double sa, double ea )
{
	wxWindowDC::DoDrawEllipticArc(x,y,width,height,sa,ea);
	m_buffer_dc.DrawEllipticArc(x,y,width,height,sa,ea);
}

void wxWindowDCbuf::DoDrawPoint( wxCoord x, wxCoord y )
{
	wxWindowDC::DoDrawPoint(x,y);
	m_buffer_dc.DrawPoint(x,y); 
}

void wxWindowDCbuf::DoDrawLines( int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset )
{
	wxWindowDC::DoDrawLines(n,points,xoffset,yoffset);
	m_buffer_dc.DrawLines(n,points,xoffset,yoffset);
}

void wxWindowDCbuf::DoDrawPolygon(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset,int fillStyle)
{
//	wxWindowDC::DoDrawPolygon(n,points,xoffset,yoffset,fillStyle);
   m_buffer_dc.ResetBoundingBox();
	m_buffer_dc.DrawPolygon(n,points,xoffset,yoffset,fillStyle);
   wxWindowDC::DoBlit(m_buffer_dc.MinX()-2, m_buffer_dc.MinY()-2,
   			  m_buffer_dc.MaxX()-m_buffer_dc.MinX()+4,
   			  m_buffer_dc.MaxY()-m_buffer_dc.MinY()+4,
   			  &m_buffer_dc,m_buffer_dc.MinX()-2, m_buffer_dc.MinY()-2,wxCOPY,false);
}

void wxWindowDCbuf::DoDrawRectangle( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
	wxWindowDC::DoDrawRectangle(x,y,width,height);
	m_buffer_dc.DrawRectangle(x,y,width,height);
}

void wxWindowDCbuf::DoDrawRoundedRectangle( wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius )
{
	wxWindowDC::DoDrawRoundedRectangle(x,y,width,height,radius);
	m_buffer_dc.DrawRoundedRectangle(x,y,width,height,radius);
}

void wxWindowDCbuf::DoDrawEllipse( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
	wxWindowDC::DoDrawEllipse(x,y,width,height);
	m_buffer_dc.DrawEllipse(x,y,width,height);
}

void wxWindowDCbuf::DoDrawIcon( const wxIcon &icon, wxCoord x, wxCoord y )
{
	wxWindowDC::DoDrawIcon(icon,x,y);
	m_buffer_dc.DrawIcon(icon,x,y);
}

void wxWindowDCbuf::DoDrawBitmap( const wxBitmap &bitmap,
                               wxCoord x, wxCoord y,
                               bool useMask )
{
	wxWindowDC::DoDrawBitmap(bitmap,x,y,useMask);
	m_buffer_dc.DrawBitmap(bitmap,x,y,useMask);
}

void wxWindowDCbuf::SetLogicalScale(double x, double y)
{
	wxWindowDC::SetLogicalScale(x,y);
	m_buffer_dc.SetLogicalScale(x,y);
}

void wxWindowDCbuf::SetMapMode(int mode)
{
	wxWindowDC::SetMapMode(mode);
	m_buffer_dc.SetMapMode(mode);
}

void wxWindowDCbuf::SetUserScale(double x, double y)
{
	wxWindowDC::SetUserScale(x,y);
	m_buffer_dc.SetUserScale(x,y);
	wxWindowDC::SetMapMode(m_mappingMode);
	m_buffer_dc.SetMapMode(m_mappingMode);
}

void wxWindowDCbuf::SetAxisOrientation(bool xLeftRight, bool yBottomUp)
{
	wxWindowDC::SetAxisOrientation(xLeftRight,yBottomUp);
	m_buffer_dc.SetAxisOrientation(xLeftRight,yBottomUp);
	wxWindowDC::SetMapMode(m_mappingMode);
	m_buffer_dc.SetMapMode(m_mappingMode);
}

void wxWindowDCbuf::SetSystemScale(double x, double y)
{
}

void wxWindowDCbuf::SetLogicalOrigin(wxCoord x, wxCoord y)
{
	wxWindowDC::SetLogicalOrigin(x,y);
	m_buffer_dc.SetLogicalOrigin(x,y);
}

void wxWindowDCbuf::SetDeviceOrigin(wxCoord x, wxCoord y)
{
	wxWindowDC::SetDeviceOrigin(x,y);
	m_buffer_dc.SetDeviceOrigin(x,y);
}

bool wxWindowDCbuf::DoBlit(wxCoord xdest, wxCoord ydest,
                  wxCoord width, wxCoord height,
                  wxDC *source, wxCoord xsrc, wxCoord ysrc,
                  int rop, bool useMask, wxCoord xsrcMask, wxCoord ysrcMask)
{
	m_buffer_dc.Blit(xdest,ydest,width,height,source,xsrc,ysrc,rop,useMask, xsrcMask, ysrcMask);
	return wxWindowDC::DoBlit(xdest,ydest,width,height,source,xsrc,ysrc,rop,useMask, xsrcMask, ysrcMask);
}

void wxWindowDCbuf::DoDrawText( const wxString &text, wxCoord x, wxCoord y )
{
	wxWindowDC::DoDrawText(text,x,y);
	m_buffer_dc.DrawText(text,x,y);
}

void wxWindowDCbuf::DoDrawRotatedText( const wxString &text, wxCoord x, wxCoord y, double angle )
{
	wxWindowDC::DoDrawRotatedText(text,x,y,angle);
	m_buffer_dc.DrawRotatedText(text,x,y,angle);
}


void wxWindowDCbuf::Clear()
{
    wxWindowDC::Clear();
    m_buffer_dc.Clear();
}

void wxWindowDCbuf::SetFont( const wxFont &font )
{
	wxWindowDC::SetFont(font);
	m_buffer_dc.SetFont(font);
}

void wxWindowDCbuf::SetPen( const wxPen &pen )
{
	wxWindowDC::SetPen(pen);
	m_buffer_dc.SetPen(pen); 
}

void wxWindowDCbuf::SetBrush( const wxBrush &brush )
{
	wxWindowDC::SetBrush(brush);
	m_buffer_dc.SetBrush(brush); 
}

void wxWindowDCbuf::SetBackground( const wxBrush &brush )
{
	wxWindowDC::SetBackground(brush);
	m_buffer_dc.SetBackground(brush);
}

void wxWindowDCbuf::SetLogicalFunction( int function )
{
	wxWindowDC::SetLogicalFunction(function);
	m_buffer_dc.SetLogicalFunction(function);
}

void wxWindowDCbuf::SetTextForeground( const wxColour &col )
{
	wxWindowDC::SetTextForeground(col);
   m_buffer_dc.SetTextForeground(col);
}

void wxWindowDCbuf::SetTextBackground( const wxColour &col )
{
	wxWindowDC::SetTextBackground(col);
   m_buffer_dc.SetTextBackground(col);
} 

void wxWindowDCbuf::SetBackgroundMode( int mode )
{
	wxWindowDC::SetBackgroundMode(mode);
	m_buffer_dc.SetBackgroundMode(mode);
}

void wxWindowDCbuf::SetPalette( const wxPalette& palette)
{
	wxWindowDC::SetPalette(palette);
}

void wxWindowDCbuf::DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{ 
	wxWindowDC::DoSetClippingRegion(x,y,width,height);
	m_buffer_dc.SetClippingRegion(x,y,width,height); 
}

void wxWindowDCbuf::DoSetClippingRegionAsRegion( const wxRegion &region  )
{
	wxWindowDC::DoSetClippingRegionAsRegion(region);
	m_buffer_dc.SetClippingRegion(region);
 
}

void wxWindowDCbuf::DestroyClippingRegion()
{
	wxWindowDC::DestroyClippingRegion();
	m_buffer_dc.DestroyClippingRegion();
}

#if wxUSE_SPLINES

void wxWindowDCbuf::DoDrawSpline( wxList *points )
{
	wxWindowDC::DoDrawSpline(points);
	m_buffer_dc.DrawSpline(points);
}

void wxWindowDCbuf::TransParentPolyon(wxTransparentPolygon* thepolygon)
{
	DrawableObject(thepolygon);
}


void wxWindowDCbuf::DrawableObject(wxDrawableObject* drawobject)
{
    switch (drawobject->GetType())
    {
        case 1://transparent polygon
        {
            wxTransparentPolygon* tp =(wxTransparentPolygon*)drawobject;
            m_buffer_dc.ResetBoundingBox();
            m_buffer_dc.SetTextForeground(*wxBLACK);            // 0s --> 0x000000 (black)
            m_buffer_dc.SetTextBackground(*wxWHITE);    // 1s --> 0xFFFFFF (white)
            m_buffer_dc.SetLogicalFunction(wxAND_INVERT);

            // BLACK OUT the opaque pixels and leave the rest as is
            m_buffer_dc.DrawPolygon(tp->n,tp->points,0,0,wxWINDING_RULE);

            // Set background and foreground colors for fill pattern
            //the previous blacked out pixels are now merged with the layer color
            //while the non blacked out pixels stay as they are.
            m_buffer_dc.SetTextForeground(*wxBLACK);            // 0s --> 0x000000 (black)
            m_buffer_dc.SetTextBackground(GetBrush().GetColour());
            m_buffer_dc.SetLogicalFunction(wxOR);
            m_buffer_dc.DrawPolygon(tp->n,tp->points,0,0,wxWINDING_RULE);
            m_buffer_dc.SetLogicalFunction(wxCOPY);

            wxWindowDC::DoBlit(m_buffer_dc.MinX()-2, m_buffer_dc.MinY()-2,
                     m_buffer_dc.MaxX()-m_buffer_dc.MinX()+4,
                  m_buffer_dc.MaxY()-m_buffer_dc.MinY()+4,
                     &m_buffer_dc,m_buffer_dc.MinX()-2, m_buffer_dc.MinY()-2,wxCOPY,false);
            break;
        }
        case 2://transparent polygon
        {

            m_buffer_dc.SetBrush(wxNullBrush);
            wxBrush a(*(m_bitmap));
            m_buffer_dc.SetBrush(a);
            wxTransparentPolygon* tp =(wxTransparentPolygon*)drawobject;
            m_buffer_dc.ResetBoundingBox();
            m_buffer_dc.SetLogicalFunction(wxCOPY);
            m_buffer_dc.DrawPolygon(tp->n,tp->points,0,0,wxWINDING_RULE);

            wxWindowDC::DoBlit(m_buffer_dc.MinX()-2, m_buffer_dc.MinY()-2,
                     m_buffer_dc.MaxX()-m_buffer_dc.MinX()+4,
                  m_buffer_dc.MaxY()-m_buffer_dc.MinY()+4,
                     &m_buffer_dc,m_buffer_dc.MinX()-2, m_buffer_dc.MinY()-2,wxCOPY,false);
            m_buffer_dc.SetBrush(wxNullBrush);
            break;
        }
        default:
            drawobject->Draw(*this);
    }
}


#endif // wxUSE_SPLINE

//clear the clippingregion using background color of canvas
void wxWindowDCbuf::ClearClippingRegion()
{
	SetLogicalFunction(wxCOPY);
   //a little bigger then strictly needed
   m_buffer_dc.SetBrush(GetBackground());
   m_buffer_dc.DrawRectangle(m_clipX1-2,m_clipY1-2,m_clipX2-m_clipX1+4,m_clipY2-m_clipY1+4);
   m_buffer_dc.SetBrush(wxNullBrush);
   SetBrush(GetBackground());
   DrawRectangle(m_clipX1-2,m_clipY1-2,m_clipX2-m_clipX1+4,m_clipY2-m_clipY1+4);
   SetBrush(wxNullBrush);
}


//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPaintDCbuf,wxWindowDCbuf)

wxPaintDCbuf::wxPaintDCbuf()
  : wxWindowDCbuf()
{
}

wxPaintDCbuf::wxPaintDCbuf( wxWindow *win )
  : wxWindowDCbuf( win )
{
#if USE_PAINT_REGION
    if (!win->m_clipPaintRegion)
        return;
        
    m_paintClippingRegion = win->GetUpdateRegion();
    GdkRegion *region = m_paintClippingRegion.GetRegion();
    if ( region )
    {
        m_currentClippingRegion.Union( m_paintClippingRegion );

		DoSetClippingRegionAsRegion( m_currentClippingRegion );
    }
#endif
}

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClientDCbuf,wxWindowDCbuf)

wxClientDCbuf::wxClientDCbuf()
  : wxWindowDCbuf()
{
}

wxClientDCbuf::wxClientDCbuf( wxWindow *win )
  : wxWindowDCbuf( win )
{
}

bool wxClientDCbuf::MoveObject(wxCoord xdest, wxCoord ydest,wxBitmap& bitmap,
                         bool use_redrawonmove,
      						 wxCoord xsrc, wxCoord ysrc,
                         wxBitmap* backgrnd,
                         bool returnbackgrnd
                        )
{
   int w=bitmap.GetWidth();
   int h=bitmap.GetHeight();
   m_bitmaptomove=&bitmap;
	return DoMoveObject(xdest,ydest,w,h,use_redrawonmove,xsrc,ysrc,backgrnd,returnbackgrnd);
}

bool wxClientDCbuf::MoveObject(wxCoord xdest, wxCoord ydest,int w, int h,
                         bool use_redrawonmove,
      						 wxCoord xsrc, wxCoord ysrc,
                         wxBitmap* backgrnd,
                         bool returnbackgrnd
                        )
{
   m_bitmaptomove=0;
	return DoMoveObject(xdest,ydest,w,h,use_redrawonmove,xsrc,ysrc,backgrnd,returnbackgrnd);
}

bool wxClientDCbuf::DoMoveObject(wxCoord xdest, wxCoord ydest,int w, int h,
                         bool use_redrawonmove,
      						 wxCoord xsrc, wxCoord ysrc,
                         wxBitmap* backgrnd,
                         bool returnbackgrnd
                        )
{
   wxMemoryDC tmp;
   //first redraw what should be behind the drawing to Move
   if (use_redrawonmove)
   {
      SetClippingRegion(xsrc,ysrc,w,h);
      //only using the memory DC, later it will be blitted to the canvas also
		SetLogicalFunction(wxCOPY);
      //m_buffer_dc.SetBrush(GetBackground());
      //m_buffer_dc.DrawRectangle(xsrc-2,ysrc-2,w+4,h+4);
      //m_buffer_dc.SetBrush(wxNullBrush);
	   RedrawOnMove();
      DestroyClippingRegion();
   }
   else if (backgrnd)
   {
//	   wxASSERT_MSG( backgrnd->Ok(), _T("invalid backgrnd bitmap in wxWindowDCbuf::MoveBitmap") );
      //redraw with backgrnd bitmap what is at the xsrc,ysrc
	   tmp.SelectObject(*backgrnd);
	   m_buffer_dc.Blit(xsrc,ysrc,w,h,&tmp,0,0,wxCOPY,false);
	   tmp.SelectObject(wxNullBitmap);
   }
   else
   {
       //clear the background (the area behind the bitmap) using background color of canvas
       //a little bigger then strictly needed
       SetClippingRegion(xsrc,ysrc,w,h);
		 SetLogicalFunction(wxCOPY);
       m_buffer_dc.SetBrush(GetBackground());
       m_buffer_dc.DrawRectangle(xsrc-2,ysrc-2,w+4,h+4);
       m_buffer_dc.SetBrush(wxNullBrush);
       DestroyClippingRegion();
   }

   //the backgrnd will be filled with the m_buffer contents at the newposition of the moved bitmap
   //before it is really moved
   if( returnbackgrnd )
   {
//	   wxASSERT_MSG( backgrnd->Ok(), _T("invalid backgrnd bitmap in wxWindowDCbuf::MoveBitmap") );
	   tmp.SelectObject(*backgrnd);
	   tmp.Blit(0,0,w,h,&m_buffer_dc,xdest,ydest,wxCOPY,false);
	   tmp.SelectObject(wxNullBitmap);
   }

	wxInt32 leftu,rightu,bottomu,topu ;
	leftu = wxMin ( xsrc , xdest ) ;
	rightu = wxMax (xsrc+w, xdest+w) ;
	topu = wxMin ( ysrc , ydest ) ;
	bottomu = wxMax (ysrc+h, ydest+h) ;

   if (m_bitmaptomove)
   {
      //blit the bitmap at the new position to the m_buffer
      m_buffer_dc.DrawBitmap(*m_bitmaptomove,xdest,ydest,m_withmask);
      //or like this
      //tmp.SelectObject(bitmap);
      //m_buffer_dc.Blit(xdest,ydest,w,h,&tmp,0,0,wxCOPY,withmask);
      //tmp.SelectObject(wxNullBitmap);
   }
   else //moving a drawing
   {
      //draw the object
      //only using the memory DC, later it will be blitted to the canvas also
      SetLogicalFunction(wxCOPY);
      SetClippingRegion(xdest,ydest,w,h);
      DrawOnMove(xdest,ydest);
      DestroyClippingRegion();
   }

	if ( rightu - leftu < 2*w && bottomu - topu < 2*h )
	{  //intersecting
      //the union of both the new place and the redraw at the old place at once to the canvas
		wxWindowDC::DoBlit(leftu,topu,rightu - leftu,bottomu - topu,&m_buffer_dc,leftu,topu,wxCOPY,false);
	}
   else
   {  //do them seperate
      //now first redraw what should be at the old position in the canvas
      wxWindowDC::DoBlit(xsrc,ysrc,w,h,&m_buffer_dc,xsrc,ysrc,wxCOPY,false);
	   //blit the bitmap to the new place in the canvas
      wxWindowDC::DoBlit(xdest,ydest,w,h,&m_buffer_dc,xdest,ydest,wxCOPY,false);
   }


   return true;
}

void wxClientDCbuf::InitiateDragObject(int first_x,int first_y,wxBitmap* source ,bool withmask,bool use_redraw)
{
   //prepare a bitmap to be used for drawing what was behind the bitmap
   m_bitmaptomove=source;
   wxMemoryDC tmp;
   int w=source->GetWidth();
   int h=source->GetHeight();
   m_backgrnd= new wxBitmap(w,h);
   //the backgrnd will be filled with the m_buffer contents at the first position
   tmp.SelectObject(*m_backgrnd);
   tmp.Blit(0,0,w,h,&m_buffer_dc,first_x,first_y,wxCOPY,false);
   tmp.SelectObject(wxNullBitmap);
   m_withmask= withmask;
   m_first_move=true;
	m_use_redraw=use_redraw;
   m_last_x=first_x;
   m_last_y=first_y;
}

void wxClientDCbuf::InitiateDragObject(int first_x,int first_y,int w, int h,bool use_redraw)
{
   //prepare a bitmap to be used for drawing what was behind the bitmap
   m_bitmaptomove=0;
   wxMemoryDC tmp;
   m_backgrnd= new wxBitmap(w,h);
   //the backgrnd will be filled with the m_buffer contents at the first position
   tmp.SelectObject(*m_backgrnd);
   tmp.Blit(0,0,w,h,&m_buffer_dc,first_x,first_y,wxCOPY,false);
   tmp.SelectObject(wxNullBitmap);
   m_first_move=true;
	m_use_redraw=use_redraw;
   m_last_x=first_x;
   m_last_y=first_y;
}

void wxClientDCbuf::EndDragObject()
{
   m_first_move=true;
   //restore what is in the backgrnd bitmap which was behind the bitmap
   //moved to the last position
   DrawBitmap(*m_backgrnd,m_last_x,m_last_y,0);
   delete m_backgrnd;
}

bool wxClientDCbuf::DragObject(wxCoord xdest, wxCoord ydest)
{
//   wxASSERT_MSG( m_bitmaptomove->Ok(), _T("invalid bitmap in wxClientDCbuf::DragBitmap") );
//   wxASSERT_MSG( m_backgrnd->Ok(), _T("invalid m_backgrnd bitmap in wxClientDCbuf::DragObject") );
   int w=m_backgrnd->GetWidth();
   int h=m_backgrnd->GetHeight();

   wxMemoryDC tmp;
   if (m_first_move && m_use_redraw)
   {
	   m_first_move=false;
      //first redraw what should be behind the bitmap to drag
      //redraw the background using the RedrawOnMove function
      SetClippingRegion(m_last_x,m_last_y,w,h);
      //only using the memory DC, later it will be blitted to the canvas also
      SetLogicalFunction(wxCOPY);
      //m_buffer_dc.SetBrush(GetBackground());
      //m_buffer_dc.DrawRectangle(xsrc-2,ysrc-2,w+4,h+4);
      //m_buffer_dc.SetBrush(wxNullBrush);
      RedrawOnMove();
      DestroyClippingRegion();
   }
   else
   {
//      wxASSERT_MSG( m_backgrnd->Ok(), _T("invalid backgrnd bitmap in wxClientDCbuf::DragObject") );
      //redraw with backgrnd bitmap what is at the xsrc,ysrc
      tmp.SelectObject(*m_backgrnd);
      m_buffer_dc.Blit(m_last_x,m_last_y,w,h,&tmp,0,0,wxCOPY,false);
      tmp.SelectObject(wxNullBitmap);
   }

   //the backgrnd will be filled with the m_buffer contents at the newposition of the moved object
   //before it is really moved
   tmp.SelectObject(*m_backgrnd);
   tmp.Blit(0,0,w,h,&m_buffer_dc,xdest,ydest,wxCOPY,false);
   tmp.SelectObject(wxNullBitmap);

	wxInt32 leftu,rightu,bottomu,topu ;
	leftu = wxMin ( m_last_x, xdest ) ;
	rightu = wxMax (m_last_x+w, xdest+w) ;
	topu = wxMin ( m_last_y , ydest ) ;
	bottomu = wxMax (m_last_y+h, ydest+h) ;

   if (m_bitmaptomove)
   {
      //blit the bitmap at the new position to the m_buffer
      m_buffer_dc.DrawBitmap(*m_bitmaptomove,xdest,ydest,m_withmask);
      //or like this
      //tmp.SelectObject(bitmap);
      //m_buffer_dc.Blit(xdest,ydest,w,h,&tmp,0,0,wxCOPY,withmask);
      //tmp.SelectObject(wxNullBitmap);
   }
   else //moving a drawing
   {
      //draw the object
      //only using the memory DC, later it will be blitted to the canvas also
      SetLogicalFunction(wxCOPY);
      SetClippingRegion(xdest,ydest,w,h);
      DrawOnMove(xdest,ydest);
      DestroyClippingRegion();
   }

	if ( rightu - leftu < 2*w && bottomu - topu < 2*h )
	{  //intersecting
      //the union of both the new place and the redraw at the old place at once to the canvas
		wxWindowDC::DoBlit(leftu,topu,rightu - leftu,bottomu - topu,&m_buffer_dc,leftu,topu,wxCOPY,false);
	}
   else
   {  //do them seperate
      //now first redraw what should be at the old position in the canvas
      wxWindowDC::DoBlit(m_last_x,m_last_y,w,h,&m_buffer_dc,m_last_x,m_last_y,wxCOPY,false);
	   //blit the bitmap to the new place in the canvas
	   //blit the bitmap to the new place in the canvas
      wxWindowDC::DoBlit(xdest,ydest,w,h,&m_buffer_dc,xdest,ydest,wxCOPY,false);
   }
   m_last_x=xdest;
   m_last_y=ydest;

   return true;
}

#elif defined(__WXMOTIF__)

#elif defined(__WXQT__)

#elif defined(__WXMAC__)

#elif defined(__WXPM__)

#elif defined(__WXSTUBS__)

#endif

