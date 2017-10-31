/*
Program	WINDRV.CPP
Purpose	Windows driver inherited from class Driver (Source)
		Draws GDSII structures in a window using Microsoft Windows
		This file is NOT portable. You have to write your own with
		the functions in this file for another platform
Last Update	20-10-1997 some changes for wxWindows
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdlib.h>
#include <math.h>
#include "windrv.h"
#include "gdserr.h"
#include "chandbas.h"
#include "scrndrv.h"
#include "toolcont.h"
#include "dcclbuf.h"

#ifndef __UNIX__
//#include <windows.h>
#else
//#include <X11/Xlib.h>
//#include <X11/X.h>
//#include <wx_main.h>
#endif

BEGIN_EVENT_TABLE(Canvas, wxWindow)
//	 EVT_SCROLLWIN_THUMBTRACK(Canvas::OnScroll)
	 EVT_SCROLLWIN(Canvas::OnScroll)
	 EVT_MOUSE_EVENTS(Canvas::OnMouseEvent)
    EVT_CHAR(Canvas::OnChar)
    EVT_KEY_DOWN(Canvas::OnKeyDown)
    EVT_KEY_UP(Canvas::OnKeyUp)
    EVT_PAINT(Canvas::OnPaint)
	 EVT_SIZE(Canvas::OnSize)
END_EVENT_TABLE()

Canvas::Canvas(GDSMainWindow *frame, int x, int y, int w, int h, long style = wxRETAINED,char* name = "MyCanvas"):
	wxWindow(frame,-1,wxPoint(x,y),wxSize(-1,-1),style,name)
{
	SetBackgroundColour(*wxWHITE);
	_backup = new wxBitmap(w,h);
	_w = w;
	_h = h;

   AdjustScrollBars(0,1000,1000,0,1000,1000);
}

// delete dynamic data
Canvas::~Canvas(void)
{

	delete _backup;
}

void Canvas::AdjustScrollBars(int posx,int dx, int totx,int posy,int dy, int toty)
{
   SetScrollbar(wxHORIZONTAL,posx,dx,totx,true);
   SetScrollbar(wxVERTICAL,posy,dy,toty,true);
}


void Canvas::Set_Valid_drawing(bool value)
{
   m_valid_drawing=value;
}

bool Canvas::Get_Valid_drawing()
{
   return m_valid_drawing;
}



// conversion from long (rgb encoded) to wxColour
void Canvas::ltowxc(long l,wxColour& color)
{
	unsigned char	red,green,blue;

	blue 	= l/65536;
	green = (l%65536)/ 256;
	red 	= l%256;

	color.Set(red,green,blue);
}

void Canvas::SetBackGroundColor(long color)
{
   wxColour _backgroundcolor;
   ltowxc(color,_backgroundcolor);
   SetBackgroundColour(_backgroundcolor);
	ClearBackground();
	wxMemoryDC _memDC;
	_memDC.SelectObject(*_backup);
   wxBrush _clearbrush(_backgroundcolor,wxSOLID);
   _memDC.SetBackground(_clearbrush);
   _memDC.Clear();
	_memDC.SelectObject(wxNullBitmap);
   GLOB->SCRDRV->SetBackground(_clearbrush);
}



void Canvas::OnMouseEvent(wxMouseEvent& event)
{
	if (!GLOB->MAINWINDOW->_printing)
	   SetFocus(); //otherwise printer dialog moves to back
	if (! GLOB->INITIALIZED) return;
/*
   if (_busydrawing==true)  //drawing is going on
   {
      if (event.RightDown())
      {
	   	_stopdrawing=true;
         return;
      }
      return;
   }
*/
}

void	Canvas::OnKeyDown(wxKeyEvent& event)
{
 	if (! GLOB->INITIALIZED) return;
}

void	Canvas::OnKeyUp(wxKeyEvent& event)
{
 	if (! GLOB->INITIALIZED) return;
}

void Canvas::OnChar(wxKeyEvent& event)
{
 	if (! GLOB->INITIALIZED) return;
}

void Canvas::Update()
{
   if (! GLOB->INITIALIZED) return;
	int w, h;
	GetClientSize(&w, &h);

   wxClientDCbuf _memDC(this);
   _statusbar->SetProcess("drawing update");
   _memDC.SelectObject(*_backup);
   if (GLOB->CMDH->Get_GDSII())
   {
      GLOB->SCRDRV->SetDC(&_memDC);
      if (GLOB->SCRDRV->GetDrawStyle()!=SELECTED)
         GLOB->SCRDRV->ClearClippingRegion();
      if (GLOB->CMDH->GetGridVisible() && (GLOB->CMDH->GetGridPlacement() == 0))
         GLOB->SCRDRV->DrawGrid(GLOB->CMDH->GetGridSpace().GetValue(), GLOB->CMDH->GetGridColour(), 1);
      // write the GDS-II file to the screen
      GLOB->SCRDRV->Write(GLOB->CMDH->Get_GDSII());
      if (GLOB->CMDH->GetGridVisible() && (GLOB->CMDH->GetGridPlacement() == 1))
         GLOB->SCRDRV->DrawGrid(GLOB->CMDH->GetGridSpace().GetValue(), GLOB->CMDH->GetGridColour(), 1);

      GLOB->SCRDRV->DrawOrigin();
      GLOB->SCRDRV->SetDC(0);

      m_valid_drawing=true;
/*
#ifndef __WXMSW__
      wxRect cliprec;
      _memDC.GetClippingBox(cliprec);
      if (cliprec.width==0 || cliprec.height==0)
         Refresh(true,NULL);
      else
         Refresh(true,&cliprec);
      _memDC.SelectObject(wxNullBitmap);
#endif
*/
   }
   _memDC.SelectObject(wxNullBitmap);
   _w = w;
   _h = h;
   _statusbar->SetProcess();
}


void Canvas::OnScroll(wxScrollWinEvent& event)
{
   /*
   wxEVT_SCROLLWIN_TOP =                     wxEVT_FIRST + 320,
    wxEVT_SCROLLWIN_BOTTOM =                  wxEVT_FIRST + 321,
    wxEVT_SCROLLWIN_LINEUP =                  wxEVT_FIRST + 322,
    wxEVT_SCROLLWIN_LINEDOWN =                wxEVT_FIRST + 323,
    wxEVT_SCROLLWIN_PAGEUP =                  wxEVT_FIRST + 324,
    wxEVT_SCROLLWIN_PAGEDOWN =                wxEVT_FIRST + 325,
    wxEVT_SCROLLWIN_THUMBTRACK =              wxEVT_FIRST + 326,
    wxEVT_SCROLLWIN_THUMBRELEASE =            wxEVT_FIRST + 327,
  */
   if (event.GetEventType()==wxEVT_SCROLLWIN_THUMBRELEASE)
   {
       if (event.GetOrientation()==wxHORIZONTAL)
       {
          double x=GLOB->SCRDRV->m_virtm_minX+event.GetPosition()/1000.0*(GLOB->SCRDRV->m_virtm_maxX-GLOB->SCRDRV->m_virtm_minX);
          x=x-GLOB->SCRDRV->m_virt_minX;
          ScrollIt(x,false);
       }
       else
       {
          double y=GLOB->SCRDRV->m_virtm_maxY-event.GetPosition()/1000.0*(GLOB->SCRDRV->m_virtm_maxY-GLOB->SCRDRV->m_virtm_minY);
          y=y-GLOB->SCRDRV->m_virt_maxY;
          ScrollIt(y ,true);
       }
   }
   else if (event.GetEventType()==wxEVT_SCROLLWIN_PAGEUP)
   {
       if (event.GetOrientation()==wxHORIZONTAL)
       {
          double x=GLOB->SCRDRV->m_virt_minX-GLOB->SCRDRV->m_virt_maxX;
          ScrollIt(x,false);
       }
       else
       {
          double y=GLOB->SCRDRV->m_virt_maxY-GLOB->SCRDRV->m_virt_minY;
          ScrollIt(y ,true);
       }
   }
   else if (event.GetEventType()==wxEVT_SCROLLWIN_PAGEDOWN)
   {
       if (event.GetOrientation()==wxHORIZONTAL)
       {
          double x=GLOB->SCRDRV->m_virt_maxX-GLOB->SCRDRV->m_virt_minX;
          ScrollIt(x,false);
       }
       else
       {
          double y=GLOB->SCRDRV->m_virt_minY-GLOB->SCRDRV->m_virt_maxY;
          ScrollIt(y ,true);
       }
   }
   else if (event.GetEventType()==wxEVT_SCROLLWIN_LINEUP)
   {
       if (event.GetOrientation()==wxHORIZONTAL)
       {
          double x=GLOB->SCRDRV->m_virt_minX-GLOB->SCRDRV->m_virt_maxX;
          ScrollIt(x/20,false);
       }
       else
       {
          double y=GLOB->SCRDRV->m_virt_maxY-GLOB->SCRDRV->m_virt_minY;
          ScrollIt(y/20,true);
       }
   }
   else if (event.GetEventType()==wxEVT_SCROLLWIN_LINEDOWN)
   {
       if (event.GetOrientation()==wxHORIZONTAL)
       {
          double x=GLOB->SCRDRV->m_virt_maxX-GLOB->SCRDRV->m_virt_minX;
          ScrollIt(x/20,false);
       }
       else
       {
          double y=GLOB->SCRDRV->m_virt_minY-GLOB->SCRDRV->m_virt_maxY;
          ScrollIt(y/20,true);
       }
   }

}

void Canvas::ScrollIt(double xy,bool vertical)
{

   if (! GLOB->INITIALIZED) return;

	BoundingBox bbox = GLOB->SCRDRV->GetVirtualSizeScreen();
	double vx1, vy1, vx2, vy2;
	vx1 = bbox.GetMin().GetX();
	vx2 = bbox.GetMax().GetX();
	vy1 = bbox.GetMin().GetY();
	vy2 = bbox.GetMax().GetY();

   int xy_pix=abs((int)(xy*GLOB->SCRDRV->GetScaleFactorX()));
   xy_pix=xy_pix/8*8;
   if (xy >0)
	   xy=xy_pix/GLOB->SCRDRV->GetScaleFactorX();
   else
	   xy=-xy_pix/GLOB->SCRDRV->GetScaleFactorX();
	// calculate the change in the coordinates
	if (!vertical)
	{
   	vx1 += xy;	vx2 += xy;
   }
	else
	{
   	vy1 += xy;	vy2 += xy;
   }

	GetClientSize(&_w, &_h);
   GLOB->SCRDRV->SetMappingScroll( vx1, vy1, vx2, vy2, // Virtual
															  false);   // Border?


   if ((vertical && xy_pix < _h) || (!vertical && xy_pix < _w))
   {
      //just a repaint using the bitmap
      wxMemoryDC _memDC2;
      wxBitmap *help= new wxBitmap(_w,_h,-1);
      _memDC2.SelectObject(*help);

      //only the buffer copy to the help buffer
      wxMemoryDC _memDC4;
      _memDC4.SelectObject(*_backup);
      _memDC2.Blit (0,0,_w,_h,&_memDC4, 0,0);
      _memDC4.SelectObject(wxNullBitmap);

      wxClientDCbuf dcbuf(this);
      dcbuf.SelectObject(*_backup);
      //remember pixel the next lines are in pixel coordinates
      if (vertical)
      {
         if (xy < 0)
            dcbuf.Blit (0,0,_w,_h-xy_pix,&_memDC2, 0,xy_pix);
         else
            dcbuf.Blit (0,xy_pix,_w,_h-xy_pix,&_memDC2, 0,0);
      }
      else
      {
         if (xy < 0)
            dcbuf.Blit (xy_pix,0,_w-xy_pix,_h,&_memDC2, 0,0);
         else
            dcbuf.Blit (0,0,_w-xy_pix,_h,&_memDC2, xy_pix,0);
      }
      _memDC2.SelectObject(wxNullBitmap);
      dcbuf.SelectObject(wxNullBitmap);
      delete help;
   }

   wxClientDCbuf dcbuf(this);
   dcbuf.SelectObject(*_backup);

   _statusbar->SetProcess("drawing update");
   GLOB->SCRDRV->DestroyClippingRegion();

	if (vertical)
   {
      if (xy < 0)
		    GLOB->SCRDRV->SetClippingRegion(vx1,vy1,vx2,vy1-xy);
      else
		    GLOB->SCRDRV->SetClippingRegion(vx1,vy2-xy,vx2,vy2);
   }
   else
   {
      if (xy < 0)
		    GLOB->SCRDRV->SetClippingRegion(vx1,vy1,vx1-xy,vy2);
      else
		    GLOB->SCRDRV->SetClippingRegion(vx2-xy,vy1,vx2,vy2);
   }

   if (GLOB->CMDH->Get_GDSII())
   {
      GLOB->SCRDRV->SetDC(&dcbuf);
      if (GLOB->SCRDRV->GetDrawStyle()!=SELECTED)
         GLOB->SCRDRV->ClearClippingRegion();
      if (GLOB->CMDH->GetGridVisible() && (GLOB->CMDH->GetGridPlacement() == 0))
         GLOB->SCRDRV->DrawGrid(GLOB->CMDH->GetGridSpace().GetValue(), GLOB->CMDH->GetGridColour(), 1);
      // write the GDS-II file to the screen
      GLOB->SCRDRV->Write(GLOB->CMDH->Get_GDSII());
      if (GLOB->CMDH->GetGridVisible() && (GLOB->CMDH->GetGridPlacement() == 1))
         GLOB->SCRDRV->DrawGrid(GLOB->CMDH->GetGridSpace().GetValue(), GLOB->CMDH->GetGridColour(), 1);

      GLOB->SCRDRV->DrawOrigin();
      GLOB->SCRDRV->SetDC(0);

      m_valid_drawing=true;
   }
   dcbuf.SelectObject(wxNullBitmap);
   _statusbar->SetProcess();
   GLOB->SCRDRV->DestroyClippingRegion();

}

void Canvas::OnSize(wxSizeEvent& event)
{
   if (! GLOB->INITIALIZED) return;

	int w, h;
	GetClientSize(&w, &h);

   if (w && h)
   {
      //only draw if the difference in size is reasonable
      if ((abs(_w-w) > 30) ||(abs(_h-h) > 30))
      {
         if (GLOB->CMDH)
         if (GLOB->CMDH->Get_GDSII())
         {

            BoundingBox bbox;
         //		*bbox = *GLOB->CMDH->Get_GDSII()->GetBoundingBox();
            bbox = GLOB->SCRDRV->GetVirtualSizeScreen();

            GLOB->SCRDRV->SetMapping(  w,h,
                     bbox.GetMin().GetX(),
                     bbox.GetMin().GetY(),
                     bbox.GetMax().GetX(),
                     bbox.GetMax().GetY(),
                     false);
         }
         delete _backup;
         _backup = new wxBitmap(w+30,h+30);
         wxColour tmp(GetBackgroundColour());
         wxBrush _clearbrush(tmp,wxSOLID);
		   wxMemoryDC _memDC;
         _memDC.SelectObject(*_backup);
         _memDC.SetBackground(_clearbrush);
         _memDC.Clear();
         _memDC.SelectObject(wxNullBitmap);
         m_valid_drawing=false;
         //now update the drawing
         Update();
      }
   }
}

void Canvas::OnPaint(wxPaintEvent&)
{
   wxPaintDC dc(this);

	if (! GLOB->CMDH->Get_GDSII()) //data to be drawn?
   {
		dc.Clear();
      m_valid_drawing=true;
		return;
	}
   else
   {
      if (!m_valid_drawing)
      {
#ifndef __WXMSW__
//			dc.Clear();
#endif
         Update();
      }
      else
      {
         //just a repaint using the bitmap
         wxMemoryDC _memDC;
         _memDC.SelectObject(wxNullBitmap);
         _memDC.SelectObject(*_backup);

         wxRegionIterator region (GetUpdateRegion());
         while (region)
         {
           dc.Blit (region.GetX(), region.GetY(), region.GetW(), region.GetH(),&_memDC, region.GetX(), region.GetY());
           region++;
         }
         _memDC.SelectObject(wxNullBitmap);
      }
	}

   //this is fast enough
   GLOB->TOOLCTRL->Draw();
}


wxBitmap* Canvas::GetCanvasBitmap()
{

	int w, h;
	GetClientSize(&w, &h);

	// the device context for drawing to screen buffer
	wxMemoryDC _memDC;

	_memDC.SelectObject(*_backup);
   wxColour tmp(GetBackgroundColour());
   wxBrush _clearbrush(tmp,wxSOLID);
   _memDC.SetBackground(_clearbrush);
   _memDC.Clear();
	if (GLOB->CMDH->Get_GDSII())
   {
	   GLOB->SCRDRV->SetDC(&_memDC);
		// write the GDS-II file to the screen
      GLOB->SCRDRV->Write(GLOB->CMDH->Get_GDSII());
		GLOB->SCRDRV->DrawOrigin();
      GLOB->SCRDRV->SetDC(0);
	}
   return _backup;
};

