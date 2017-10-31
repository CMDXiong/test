#ifdef __GNUG__
#pragma implementation
#endif

#include <stdlib.h>
#include "toolcont.h"
#include "tool.h"
#include "chandbas.h"
#include "arc.h"
#include "elemlst.h"
#include "text.h"
#include "boundary.h"
#include "referenc.h"
#include "path.h"
#include "b_dl_itr.h"
#include "propedit.h"
#include "strucdlg.h"
#include "structur.h"
#include "command.h"
#include "textinsdlg.h"
#include "settingsdlg.h"
#include "polyeditdlg.h"
#include "circleditdlg.h"
#include <math.h>

#define HANDLE 6
#define HANDLE2 3

/*
bool Tool::IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack)
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
      case SELECT_TOOL:
      case DESELECT_TOOL:
      case RECTANGLE_TOOL:
      case POLYGON_TOOL:
      case TEXT_TOOL:
      case POLYLINE_TOOL:
      case MOVE_TOOL:
      case ROTATE_TOOL:
      case COPY_TOOL:
      case CIRCLE_TOOL:
      case CIRCLE2_TOOL:
      case DELETE_TOOL:
      case EDITPOLYGON_TOOL:
      case EDITCIRCLE_TOOL:
      case EDITREC_TOOL:
      case EDITPOLYLINE_TOOL:
      case EDIT_TOOL:
      case EDITPROPERTY_TOOL:
      case INSERTREF_TOOL:
      case INSERTSTRUC_TOOL:
         return true;
      default:
         return true;
   }
}
*/

BaseTool::BaseTool(ToolControl* controller)
{
	m_toolbar = TOOLBAR_GENERAL;
   _controller=controller;
   _timeevent=0;
   _busy=false;
   m_maxx=0;
   m_minx=0;
   m_maxy=0;
   m_miny=0;
   m_valid_bbox=false;
}

BaseTool::~BaseTool()
{
}

void	BaseTool::DrawHandle(wxDC& dc,double x, double y,int type)
{
   if (type == 0)
   {
   //   wxBrush transp(*wxBLACK,wxTRANSPARENT);
      wxBrush transp(*wxBLACK,wxSOLID);
      wxPen pen(*wxBLACK,1,wxSOLID);
      dc.SetBrush(transp);
      dc.SetPen(pen);
      dc.SetLogicalFunction(wxINVERT);
      int xd = (int) GLOB->SCRDRV->LogicalToDeviceX(x);
      int yd = (int) GLOB->SCRDRV->LogicalToDeviceY(y);

   //   dc.DrawRectangle(x-HANDLE,y-HANDLE,2*HANDLE+1,2*HANDLE+1);
      wxPen pen1(*wxBLACK,1,wxSOLID);
      dc.SetPen(pen1);
      dc.DrawLine(xd-HANDLE,yd,xd+HANDLE,yd);
      dc.DrawLine(xd,yd-HANDLE,xd,yd+HANDLE);
      dc.SetBrush(wxNullBrush);
      dc.SetPen(wxNullPen);
   }
   else
   {
   //   wxBrush transp(*wxBLACK,wxTRANSPARENT);
      wxBrush transp(*wxBLACK,wxSOLID);
      wxPen pen(*wxBLACK,1,wxSOLID);
      dc.SetBrush(transp);
      dc.SetPen(pen);
      dc.SetLogicalFunction(wxINVERT);
      int xd = (int) GLOB->SCRDRV->LogicalToDeviceX(x);
      int yd = (int) GLOB->SCRDRV->LogicalToDeviceY(y);
      dc.DrawRectangle(xd-HANDLE2,yd-HANDLE2,2*HANDLE2+1,2*HANDLE2+1);
      wxPen pen1(*wxBLACK,1,wxSOLID);
      dc.SetPen(pen1);
      dc.SetBrush(wxNullBrush);
      dc.SetPen(wxNullPen);
   }
}

void BaseTool::CalculateCurrent()
{
   double xh,yh;
   xh=_currentX;
   yh=_currentY;
	GLOB->SCRDRV->GetInverseMappingMatrix().TransformPoint(xh,yh, _realcurrentX, _realcurrentY);
}

void BaseTool::CalcVirtual(long x,long y,double& xr,double& yr)
{
   double xh,yh;
   xh=x;
   yh=y;
	GLOB->SCRDRV->GetInverseMappingMatrix().TransformPoint(xh,yh, xr,yr);
}

void BaseTool::Grid()
{
	if (GLOB->CMDH->GetGridVisible())
	{
      CalculateCurrent();

		double grid = GLOB->CMDH->GetGridSpace().GetValue();

      _grid_x = (long)((floor((_realcurrentX*GLOB->PHYSUNITS() / grid) + 0.5) * grid)/GLOB->PHYSUNITS());
      _grid_y = (long)((floor((_realcurrentY*GLOB->PHYSUNITS() / grid) + 0.5) * grid)/GLOB->PHYSUNITS());

      wxTransformMatrix _temp;
      _temp.SetValue(0,0,_grid_x);
      _temp.SetValue(0,1,_grid_y);
      _temp.SetValue(0,2,1);

      wxTransformMatrix _temp2;
      _temp2 = GLOB->SCRDRV->GetMappingMatrix();
      _temp = _temp2 * _temp;
      _currentX = (long)_temp.GetValue(0,0);
      _currentY = (long)_temp.GetValue(0,1);
	}
}

void BaseTool::OnMouseEvent(wxMouseEvent& event)
{
   _event=event;
	SetStatusBar();

   GLOB->CANVAS->SetCursor(_toolcursor);
	if (event.Leaving())
   {
		return;
	}
	if (event.Entering())
   {
		return;
	}

/*
	if (_timeevent > event.GetTimestamp())
       return;
   else
		_timeevent=event.GetTimestamp();
*/
   event.GetPosition(&_currentX, &_currentY);

   if (event.LeftDClick())
   {
		EvLeftDClick();
		return;
   }
	if (event.Dragging())
   {
		EvDrag();
		return;
	}
	if (event.Moving())
   {
		EvMove(event.m_leftDown);
		return;
	}
	if (event.LeftDown())
   {
		EvLD();
		return;
	}
	if (event.RightDown())
   {
     	EvRD();
		return;
	}
	if (event.LeftUp())
   {
		EvLU();
		return;
	}
}

void	BaseTool::OnKeyDown(wxKeyEvent& WXUNUSED(event))
{
}

void	BaseTool::OnKeyUp(wxKeyEvent& WXUNUSED(event))
{
}

void BaseTool::OnChar(wxKeyEvent& WXUNUSED(event))
{
}


ToolSet BaseTool::GetType()

{
  return _tooltype;
}

bool BaseTool::IsAllowedTool(ToolSet WXUNUSED(tool_to_start_next),bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
	return true;
}

void BaseTool::SetStatusBar()
{
		char buf[80];
      sprintf(buf,"level: %d :",_controller->NumberOfTools());
      strcat(buf,GetToolInfo().c_str());
		_statusbar->SetProcess(buf);
}

void BaseTool::EvRD()

{
      CalculateCurrent();

	   GLOB->MAINWINDOW->SetPoint(_realcurrentX,_realcurrentY);
   	GLOB->CANVAS->PopupMenu(GLOB->MAINWINDOW->_mousemenu,_currentX, _currentY);
		return;
}

void BaseTool::EvLeftDClick()

{}

void BaseTool::EvDrag()
{}

void BaseTool::EvMove(int WXUNUSED(drag))
{}

void BaseTool::EvLD()
{}

void BaseTool::EvLU()
{}



void BaseTool::Undo()

{}

void BaseTool::SetCursor(double x, double y)
{
   _realcurrentX=x;
   _realcurrentY=y;
   int xd = (int) GLOB->SCRDRV->LogicalToDeviceX(x);
   int yd = (int) GLOB->SCRDRV->LogicalToDeviceY(y);
  	GLOB->CANVAS->WarpPointer(xd,yd);
   _currentX=xd;
   _currentY=yd;
}

void  BaseTool::AddPoint(double WXUNUSED(x),double WXUNUSED(y))
{

}



void BaseTool::Begin()

{

}



void BaseTool::End()
{
}


BoxTool::BoxTool(ToolControl* controller):BaseTool(controller)

{
	_boxdrawn = false;
   box=true;

}

BoxTool::~BoxTool()
{
   if (_boxdrawn)
   {
      wxClientDC temp(GLOB->CANVAS);
      GLOB->SCRDRV->SetDC(&temp);
      temp.SetLogicalFunction(wxINVERT);
      GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
      GLOB->SCRDRV->GDSDrawLine(_ZoomRect.left,_ZoomRect.top,_ZoomRect.left,_ZoomRect.bottom);
      GLOB->SCRDRV->GDSDrawLine(_ZoomRect.left,_ZoomRect.bottom,_ZoomRect.right,_ZoomRect.bottom);
      GLOB->SCRDRV->GDSDrawLine(_ZoomRect.right,_ZoomRect.bottom,_ZoomRect.right,_ZoomRect.top);
      GLOB->SCRDRV->GDSDrawLine(_ZoomRect.left,_ZoomRect.top,_ZoomRect.right,_ZoomRect.top);
      temp.SetLogicalFunction(wxCOPY);
      GLOB->SCRDRV->SetDrawStyle(STORED);
	   GLOB->SCRDRV->SetDC(0);

   }
	_boxdrawn = false;
}

void BoxTool::OnMouseEvent(wxMouseEvent& event)
{
   _event=event;
	SetStatusBar();
	if (event.Leaving() || event.Entering())
   {
		return;
	}

   GLOB->CANVAS->SetCursor(_toolcursor);

   if (_boxdrawn)
   {
      wxClientDC temp(GLOB->CANVAS);
      GLOB->SCRDRV->SetDC(&temp);
      temp.SetLogicalFunction(wxINVERT);
      GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
      GLOB->SCRDRV->GDSDrawLine(_ZoomRect.left,_ZoomRect.top,_ZoomRect.left,_ZoomRect.bottom);
      GLOB->SCRDRV->GDSDrawLine(_ZoomRect.left,_ZoomRect.bottom,_ZoomRect.right,_ZoomRect.bottom);
      GLOB->SCRDRV->GDSDrawLine(_ZoomRect.right,_ZoomRect.bottom,_ZoomRect.right,_ZoomRect.top);
      GLOB->SCRDRV->GDSDrawLine(_ZoomRect.left,_ZoomRect.top,_ZoomRect.right,_ZoomRect.top);
      GLOB->SCRDRV->SetDrawStyle(STORED);
      temp.SetLogicalFunction(wxCOPY);
	   GLOB->SCRDRV->SetDC(0);
      _boxdrawn = false;
   }

	event.GetPosition(&_currentX, &_currentY);
   Grid();
	CalculateCurrent();

   if (event.LeftDClick())
   {
		EvLeftDClick();
		return;
   }
	if (event.Moving())
   {
  		EvMove(0);
		return;
	}
	if (event.Dragging())
   {
 		EvMove(1);
		//EvDrag();
		return;
	}
	if (event.LeftDown())
   {
		EvLD();
		return;
	}
	if (event.RightDown())
   {
     	EvRD();
		return;
	}
	if (event.LeftUp())
   {
		EvLU();
		return;
	}
}

void BoxTool::EvDrag()
{
	if (GLOB->CMDH->GetViewCoord())
   {
		_statusbar->SetXY(_realcurrentX,_realcurrentY);
	}
}

void BoxTool::EvMove(int drag)
{
   if ( GLOB->CMDH->GetViewCoord())
   {
      _statusbar->SetXY(_realcurrentX,_realcurrentY);
   }
	if (_busy)
   {
   //   	WarpPointer(200, 200);
      if (drag)
      {
         GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
         _ZoomRect.right  = _realcurrentX;
         _ZoomRect.bottom = _realcurrentY;
         if (!_boxdrawn)
         {
		      wxClientDC temp(GLOB->CANVAS);
			   GLOB->SCRDRV->SetDC(&temp);
		      temp.SetLogicalFunction(wxINVERT);
            GLOB->SCRDRV->GDSDrawLine(_ZoomRect.left,_ZoomRect.top,_ZoomRect.left,_ZoomRect.bottom);
            GLOB->SCRDRV->GDSDrawLine(_ZoomRect.left,_ZoomRect.bottom,_ZoomRect.right,_ZoomRect.bottom);
            GLOB->SCRDRV->GDSDrawLine(_ZoomRect.right,_ZoomRect.bottom,_ZoomRect.right,_ZoomRect.top);
            GLOB->SCRDRV->GDSDrawLine(_ZoomRect.left,_ZoomRect.top,_ZoomRect.right,_ZoomRect.top);
		      temp.SetLogicalFunction(wxCOPY);
            _boxdrawn = true;
            m_valid_bbox=false;
            CalcBoundingBox(temp.MaxX(),temp.MaxY());
            CalcBoundingBox(temp.MinX(),temp.MinY());
  			   GLOB->SCRDRV->SetDC(0);

         }
         GLOB->SCRDRV->SetDrawStyle(STORED);
         wxString len;
         len.Printf("W: %f H: %f ",_ZoomRect.right-_ZoomRect.left,_ZoomRect.top-_ZoomRect.bottom);
         _statusbar->SetProcess((char*)len.c_str());
      }
   }
}

void BoxTool::EvLD()
{
	_ZoomRect.left = _realcurrentX;
	_ZoomRect.top = _realcurrentY;
	_ZoomRect.right = _realcurrentX;
	_ZoomRect.bottom = _realcurrentY;
   _firstX=_currentX;
   _firstY=_currentY;
   _busy=true;
   GLOB->CMDH->ToHistory("begin");
}

void BoxTool::EvLU()
{

   box=true;

   if (_ZoomRect.left > _realcurrentX)
   {
      _ZoomRect.right = _ZoomRect.left;
      _ZoomRect.left = _realcurrentX;
   }
   else
      _ZoomRect.right = _realcurrentX;

   if (_ZoomRect.top > _realcurrentY)
   {
      _ZoomRect.bottom = _ZoomRect.top;
      _ZoomRect.top = _realcurrentY;
   }
   else
      _ZoomRect.bottom = _realcurrentY;

   if ((fabs(_firstX-_currentX) < HANDLE) ||
       (fabs(_firstY-_currentY) < HANDLE) )
   {
         box=false;
   }
}


SelectTool::SelectTool(ToolControl* controller):BoxTool(controller)
{
   _toolcursor=wxCURSOR_PENCIL;
	_tooltype = SELECT_TOOL;
	m_toolbar = TOOLBAR_DRAW;

}

SelectTool::~SelectTool()
{
}

wxString  SelectTool::GetToolInfo()
{
   return "select";
}

bool SelectTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
      case ZOOMIN_TOOL:
      case DESELECT_TOOL:
         return true;
      default:
         return false;
   }
}

void SelectTool::Draw()
{
	SetStatusBar();
}

void SelectTool::EvLD()
{
   _busy=true;
   Point a_point(_realcurrentX,_realcurrentY);
   if (GLOB->CMDH->GetTopElementList()->SelectedAtPoint(&a_point))
   {
      //put the SELECT tool on the stack and inistiate the MOVE tool
//      _controller->StartTool(MOVE_TOOL,true);
      //transfer the EvLD event to the MOVE tool
//      _controller->GetActiveTool()->OnMouseEvent(_event);
//      return;
   }

   BoxTool::EvLD();
}

void SelectTool::EvLU()
{
   BoxTool::EvLU();

   if (box)
   {
	   GLOB->CMDH->DoCommand("select %f %f %f %f %s",
                                         _ZoomRect.left, // x1
                                         _ZoomRect.top, // y1
                                         _ZoomRect.right, // x2
                                         _ZoomRect.bottom,  // y2
                                         "true"	 // selecttype
                                    );
      DrawStyle backup=GLOB->SCRDRV->GetDrawStyle();
      GLOB->SCRDRV->SetDrawStyle(SELECTED);
		GLOB->SCRDRV->SetClippingRegionWin(m_minx,m_miny,m_maxx,m_maxy);
      GLOB->CMDH->DoCommand("draw");
      m_valid_bbox=false;
      GLOB->SCRDRV->DestroyClippingRegion();
      GLOB->SCRDRV->SetDrawStyle(backup);

      _busy=false;
      _controller->EndTool();
   }
   else
   {
      GLOB->CMDH->ToHistory("select %f %f %s",
                                         _ZoomRect.left, // x1
                                         _ZoomRect.bottom, // y1
                                         "true"	 // selecttype
                                       );
      //find the selected element
	   Point a_point(_ZoomRect.left,_ZoomRect.bottom);
      Element* a_ele=GLOB->CMDH->GetTopElementList()->Select(&a_point,SELECT);
      if (a_ele)
      {
         DrawStyle backup=GLOB->SCRDRV->GetDrawStyle();
         GLOB->SCRDRV->SetDrawStyle(SELECTED);
         GLOB->SCRDRV->SetClippingRegionWin(m_minx,m_miny,m_maxx,m_maxy);
		   GLOB->SCRDRV->SetClippingRegionBox(a_ele->GetBoundingBox());
         GLOB->CMDH->DoCommand("draw");
         m_valid_bbox=false;
         GLOB->SCRDRV->DestroyClippingRegion();
         GLOB->SCRDRV->SetDrawStyle(backup);
      }
      _busy=false;
      _controller->EndTool();
   }

}

DeSelectTool::DeSelectTool(ToolControl* controller):BoxTool(controller)
{
   _toolcursor=wxCURSOR_PENCIL;
	_tooltype = DESELECT_TOOL;
	m_toolbar = TOOLBAR_DRAW;
}

DeSelectTool::~DeSelectTool()
{
}

wxString  DeSelectTool::GetToolInfo()
{
   return "de-select";
}

bool DeSelectTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
      case ZOOMIN_TOOL:
      case SELECT_TOOL:
         return true;
      default:
         return false;
   }
}

void DeSelectTool::Draw()
{
	SetStatusBar();
}

void DeSelectTool::EvLU()
{
   BoxTool::EvLU();

   if (box)
   {
	   GLOB->CMDH->DoCommand("select %f %f %f %f %s",
                                         _ZoomRect.left, // x1
                                         _ZoomRect.top, // y1
                                         _ZoomRect.right, // x2
                                         _ZoomRect.bottom,  // y2
                                         "false"	 // selecttype
                                    );
	   GLOB->SCRDRV->SetClippingRegionWin(m_minx,m_miny,m_maxx,m_maxy);
      GLOB->CMDH->DoCommand("draw");
      m_valid_bbox=false;
      GLOB->SCRDRV->DestroyClippingRegion();

      _busy=false;
      _controller->EndTool();
   }
   else
   {
      GLOB->CMDH->ToHistory("select %f %f %s",
                                         _ZoomRect.left, // x1
                                         _ZoomRect.bottom, // y1
                                         "false"	 // selecttype
                                       );
      //find the selected element
	   Point a_point(_ZoomRect.left,_ZoomRect.bottom);
      Element* a_ele=GLOB->CMDH->GetTopElementList()->Select(&a_point,DESELECT);
      if (a_ele)
      {
		   GLOB->SCRDRV->SetClippingRegionBox(a_ele->GetBoundingBox());
         GLOB->CMDH->DoCommand("draw");
         m_valid_bbox=false;
         GLOB->SCRDRV->DestroyClippingRegion();
      }
      _busy=false;
      _controller->EndTool();
   }
}

ZoomTool::ZoomTool(ToolControl* controller):BoxTool(controller)
{
   _toolcursor=wxCURSOR_MAGNIFIER;
	_tooltype = ZOOMIN_TOOL;
	m_toolbar = TOOLBAR_GENERAL;
   _zooming=false;
}

ZoomTool::~ZoomTool()
{
}

wxString  ZoomTool::GetToolInfo()
{
   return "zoom-in";
}

bool ZoomTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int tools_on_stack)
{
   if (tools_on_stack == 0 && tool_to_start_next != ZOOMIN_TOOL)
      return true;
   switch(tool_to_start_next)
   {
      case ZOOMOUT_TOOL:
      case ZOOMIN_TOOL:
      case ZOOMBACK_TOOL:
      case SELECT_TOOL:
      case DESELECT_TOOL:
         return true;
      default:
         return false;
   }
}

void ZoomTool::Draw()
{
	SetStatusBar();
}

void ZoomTool::EvLU()
{
   if (_zooming)
   {

      BoxTool::EvLU();

      BoundingBox Total=GLOB->CMDH->Get_GDSII()->GetBoundingBox();

      double DX = fabs(Total.GetMax().GetX()-Total.GetMin().GetX());
      double DY = fabs(Total.GetMax().GetY()-Total.GetMin().GetY());


		BoundingBox Visible=GLOB->SCRDRV->GetVirtualSizeScreen();
      double VDX = fabs(Visible.GetMax().GetX()-Visible.GetMin().GetX());
      double VDY = fabs(Visible.GetMax().GetY()-Visible.GetMin().GetY());

      //maximum zoomin at once at a point is a 1/5 the visible picture
      if (!box)
      {
         _ZoomRect.bottom -= VDY/5;
         _ZoomRect.top    += VDY/5;
         _ZoomRect.left   -= VDX/5;
         _ZoomRect.right  += VDX/5;
      }

      double DXZ =fabs(_ZoomRect.right-_ZoomRect.left);
      double DYZ =fabs(_ZoomRect.top-_ZoomRect.bottom);

      //maximum zoomin is a 1/100000 of the full picture
      if ((DXZ*100000.0 < DX )||(DYZ*100000.0 < DY))
      {
         // limit de zoom
         double Mx = (_ZoomRect.right+_ZoomRect.left)/2;
         double My = (_ZoomRect.top+_ZoomRect.bottom)/2;
         _ZoomRect.left= Mx - DX/200000.0; // x1
         _ZoomRect.right= Mx + DX/200000.0; // x2
         _ZoomRect.bottom= My - DY/200000.0; // y1
         _ZoomRect.top= My + DY/200000.0; // y2
      }

      // do the zoom
      GLOB->MAINWINDOW->set_Cursor("wait");
      GLOB->CMDH->DoCommand("zoom %f %f %f %f",
                                         _ZoomRect.left, // x1
                                         _ZoomRect.bottom, // y1
                                         _ZoomRect.right, // x2
                                         _ZoomRect.top  // y2
                                       );
      GLOB->CMDH->DoCommand("draw");

      _controller->EndTool();

      _statusbar->SetZoom(GLOB->CMDH->Get_ScaleFactor());
      GLOB->MAINWINDOW->CmZoomOutEnable(TRUE);
      GLOB->MAINWINDOW->set_Cursor("ready");
      _zooming=false;
   }
   _busy=false;
}

void ZoomTool::EvLD()
{
   _busy=true;
   //only when this tool is the only one on the stack
/* don't like this mode
   if (_controller->NumberOfTools() < 2)
   {
      Point a_point(_realcurrentX,_realcurrentY);
      if (GLOB->CMDH->GetTopElementList()->SelectedAtPoint(&a_point))
      {
         //put the ZOOM tool on the stack and inistiate the MOVE tool
         _controller->StartTool(MOVE_TOOL,true);
         //transfer the EvLD event to the MOVE tool
         _controller->GetActiveTool()->OnMouseEvent(_event);
         return;
      }
     _zooming=true;
   }
   else
*/
     _zooming=true;

   BoxTool::EvLD();
}

RecTangleTool::RecTangleTool(ToolControl* controller):BoxTool(controller)
{
   _toolcursor=wxCURSOR_CROSS;
	_tooltype = RECTANGLE_TOOL;
	m_toolbar = TOOLBAR_DRAW;
}

bool RecTangleTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
      case SELECT_TOOL:
      case DESELECT_TOOL:
      case MOVE_TOOL:
      case ROTATE_TOOL:
      case COPY_TOOL:
      case DELETE_TOOL:
      case EDIT_TOOL:
      case EDITPROPERTY_TOOL:
      case POLYGON_TOOL:
      case POLYLINE_TOOL:
      case CIRCLE2_TOOL:
      case CIRCLE_TOOL:
      case RECTANGLE_TOOL:
         return true;
      default:
         return false;
   }
}

RecTangleTool::~RecTangleTool()
{
}

void RecTangleTool::Draw()
{
	SetStatusBar();
}

wxString  RecTangleTool::GetToolInfo()
{
   return "draw rectangle";
}

void RecTangleTool::EvLU()
{
   BoxTool::EvLU();

   GLOB->MAINWINDOW->set_Cursor("wait");

   double x1 =_ZoomRect.left ;
   double x2 =_ZoomRect.right;
   double y1 =_ZoomRect.top;
   double y2 =_ZoomRect.bottom ;

	Box* a_box = new Box(GLOB->CMDH->GetCurrentLayer());
   a_box->GetRelativeMatrix()->Translate(x1,y1);
   a_box->SetW(x2-x1);
   a_box->SetH(y2-y1);
	a_box->SetWidth(GLOB->CMDH->GetWidth()/GLOB->PHYSUNITS());
   a_box->CalcBoundingBox();
	GLOB->CMDH->GetTopElementList()->insend(a_box);
   GLOB->SETUP->Set_Available(GLOB->CMDH->GetCurrentLayer(), true);

   Command* _the_command = new Command();
   _the_command->Set_Keyword("keystring");
	_the_command->Set_Command("   keystring {BOX; LAYER 0;  BOXTYPE 0;  WIDTH 0;  XY 5; X %f; Y %f;X %f;	Y %f; X %f; Y %f;	X %f; Y %f; X %f; Y %f; ENDEL;} false ",
                                      x1,y1,x1,y2,x2,y2,x2,y1,x1,y1);
   _the_command->Set_Tool(_tooltype);
   _the_command->Set_EventStamp(a_box->GetStamp());
   GLOB->CMDH->ToHistory(_the_command);

   _the_command = new Command();
   _the_command->Set_Keyword("end");
   _the_command->Set_Command("end");
   _the_command->Set_Tool(_tooltype);
   _the_command->Set_EventStamp(a_box->GetStamp());

   GLOB->CMDH->ToHistory(_the_command);
   GLOB->CMDH->Get_GDSII()->CalcBoundingBox();

//   GLOB->SCRDRV->SetClippingRegionWin(m_minx,m_miny,m_maxx+1,m_maxy+1);
   GLOB->SCRDRV->SetClippingRegionBox(a_box->GetBoundingBox());
   GLOB->CMDH->DoCommand("draw");
   m_valid_bbox=false;
   GLOB->SCRDRV->DestroyClippingRegion();
   _controller->EndTool();

   GLOB->MAINWINDOW->set_Cursor("ready");
   _busy=false;
}

void RecTangleTool::EvLD()
{
   BoxTool::EvLD();
}

void RecTangleTool::AddPoint(double x,double y)
{
   _realcurrentX=x;
   _realcurrentY=y;
   int xd = (int) GLOB->SCRDRV->LogicalToDeviceX(x);
   int yd = (int) GLOB->SCRDRV->LogicalToDeviceY(y);
  	GLOB->CANVAS->WarpPointer(xd,yd);
   _currentX = xd;
   _currentY = yd;
   if (!_busy)
		EvLD();
   else
      EvLU();
}


PolyTool::PolyTool(ToolControl* controller):BaseTool(controller)
{
   _toolcursor=wxCURSOR_CROSS;
   _polygon = new G_Polygon();
   _boundary = new Boundary(_polygon,0);
	_boundary->SetWidth(GLOB->CMDH->GetWidth()/GLOB->PHYSUNITS());
   _boundary->SetSpline(GLOB->CMDH->GetSpline());
   _firstpoint=false;
	_tooltype = POLYGON_TOOL;
	m_toolbar = TOOLBAR_DRAW;
}

PolyTool::~PolyTool()
{
   if (_firstpoint)
	   Draw();
   delete _boundary;
}

wxString  PolyTool::GetToolInfo()
{
   return "draw polygon";
}

bool PolyTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
      case SELECT_TOOL:
      case DESELECT_TOOL:
      case MOVE_TOOL:
      case ROTATE_TOOL:
      case COPY_TOOL:
      case DELETE_TOOL:
      case EDIT_TOOL:
      case EDITPROPERTY_TOOL:
      case POLYGON_TOOL:
      case POLYLINE_TOOL:
      case CIRCLE2_TOOL:
      case CIRCLE_TOOL:
      case RECTANGLE_TOOL:
         return true;
      default:
         return false;
   }
}

void PolyTool::Draw()
{
//	SetStatusBar();
   if (_polygon->count())
   {
      wxString len;
      len.Printf("len: %f",_boundary->GetPolygon()->CalcLength());
   	_statusbar->SetProcess((char*)len.c_str());
      m_valid_bbox=false;
      wxClientDC temp(GLOB->CANVAS);
      GLOB->SCRDRV->SetDC(&temp);
      temp.SetLogicalFunction(wxINVERT);
   	_boundary->SetWidth(GLOB->CMDH->GetWidth()/GLOB->PHYSUNITS());
	   _boundary->SetSpline(GLOB->CMDH->GetSpline());
	   GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
	   _boundary->CalcBoundingBox();
      GLOB->SCRDRV->WriteDirect(_boundary);
	   GLOB->SCRDRV->SetDrawStyle(STORED);
	   GLOB->SCRDRV->SetDC(0);
   }
}

void PolyTool::EvDrag()
{
//	  	GLOB->SCRDRV->CaptureMouse();
   	EvMove(1);
}

void PolyTool::EvMove(int WXUNUSED(drag))
{
   if (_event.ControlDown())
   {
      Grid();
      CalculateCurrent();
      _realcurrentX=_lastsegm.GetX();
   }
   else if (_event.ShiftDown())
   {
      Grid();
      CalculateCurrent();
      _realcurrentY=_lastsegm.GetY();
   }
   else
   {
      Grid();
      CalculateCurrent();
   }
   if (GLOB->CMDH->GetViewCoord())
      _statusbar->SetXY(_realcurrentX,_realcurrentY);
   if (_firstpoint)
   {
      Draw();
      _segm->SetX(_realcurrentX);
      _segm->SetY(_realcurrentY);
      //remove old Xor lines
      Draw();
	}
}

void PolyTool::EvLeftDClick()
{
   if (_polygon->count())
   {
      GLOB->MAINWINDOW->set_Cursor("wait");

	  delete _polygon->tailitem();
      _polygon->removetail();  //because an extra LD is generated at DClick
	   G_Polygon* a_polygon = new G_Polygon();
      *a_polygon = *_polygon;

      Boundary* a_boundary=new Boundary(GLOB->CMDH->GetCurrentLayer());
		a_boundary->SetPolygon(a_polygon);
		a_boundary->SetWidth(GLOB->CMDH->GetWidth()/GLOB->PHYSUNITS());
		a_boundary->SetSpline(GLOB->CMDH->GetSpline());
      a_boundary->CalcBoundingBox();
		GLOB->CMDH->GetTopElementList()->insend(a_boundary);
	   GLOB->SETUP->Set_Available(GLOB->CMDH->GetCurrentLayer(), true);
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();

      Command* _the_command = new Command();
      _the_command->Set_Keyword("end");
      _the_command->Set_Command("end");
      _the_command->Set_Tool(_tooltype);
      _the_command->Set_EventStamp(a_boundary->GetStamp());
      _polygon->MakeEmpty();

      GLOB->CMDH->ToHistory(_the_command);

      GLOB->SCRDRV->SetClippingRegionBox(a_boundary->GetBoundingBox());
      GLOB->CMDH->DoCommand("draw");
      GLOB->SCRDRV->DestroyClippingRegion();

      GLOB->MAINWINDOW->set_Cursor("ready");

      GLOB->MAINWINDOW->SetPoint(_realcurrentX,_realcurrentY);
      _firstpoint=false;
	   _controller->EndTool();
   }
   _busy=false;
}

void PolyTool::EvLD()
{
   _busy=true;
   Grid();
	CalculateCurrent();
   //if this is the first point then only add the point
   if (!_firstpoint)
   {
		_boundary->SetWidth(GLOB->CMDH->GetWidth()/GLOB->PHYSUNITS());
	   _boundary->SetSpline(GLOB->CMDH->GetSpline());
      _segm=new Line_Seg();
      _segm->SetX(_realcurrentX);
      _segm->SetY(_realcurrentY);
      _polygon->AddSegment(_segm);
		_firstpoint=true;
      _segm=new Line_Seg();
      _segm->SetX(_realcurrentX);
      _segm->SetY(_realcurrentY);
      _polygon->AddSegment(_segm);
      GLOB->CMDH->ToHistory("begin");
      char buf[40];
      sprintf(buf, "%15.3f %15.3f", _realcurrentX,_realcurrentY);
      GLOB->CMDH->ToHistory("addpoint %s ",buf);
   }
   else
   {
      //remove polygon drawn during last left down
      Draw();
      char buf[40];
      sprintf(buf, "%15.3f %15.3f", _realcurrentX,_realcurrentY);
      GLOB->CMDH->ToHistory("addpoint %s ",buf);
      //directly addpoint
      _segm=new Line_Seg();
      _segm->SetX(_realcurrentX);
      _segm->SetY(_realcurrentY);
      _polygon->AddSegment(_segm);

      //draw new polygon
      Draw();
   }
   _lastsegm=*_segm;
}

void PolyTool::EvLU()
{
//  	GLOB->SCRDRV->ReleaseMouse();
}

void PolyTool::AddPoint(double x,double y)
{
   _realcurrentX=x;
   _realcurrentY=y;
   int xd = (int) GLOB->SCRDRV->LogicalToDeviceX(x);
   int yd = (int) GLOB->SCRDRV->LogicalToDeviceY(y);
  	GLOB->CANVAS->WarpPointer( xd, yd );
   _busy=true;
   //if this is the first point then only add the point
   if (!_firstpoint)
   {
      _segm=new Line_Seg();
      _segm->SetX(_realcurrentX);
      _segm->SetY(_realcurrentY);
      _polygon->AddSegment(_segm);
		_firstpoint=true;
      _segm=new Line_Seg();
      _segm->SetX(_realcurrentX);
      _segm->SetY(_realcurrentY);
      _polygon->AddSegment(_segm);
      GLOB->CMDH->ToHistory("begin");
      char buf[40];
      sprintf(buf, "%15.3f %15.3f", _realcurrentX,_realcurrentY);
      GLOB->CMDH->ToHistory("addpoint %s ",buf);
   }
   else
   {
      //remove polygon drawn during last left down
      Draw();
      //change the current value of the extra segment use to show move of cursor
      _segm->SetX(_realcurrentX);
      _segm->SetY(_realcurrentY);
      char buf[40];
      sprintf(buf, "%15.3f %15.3f", _realcurrentX,_realcurrentY);
      GLOB->CMDH->ToHistory("addpoint %s ",buf);
      //directly addpoint
      _segm=new Line_Seg();
      _segm->SetX(_realcurrentX);
      _segm->SetY(_realcurrentY);
      _polygon->AddSegment(_segm);

      //draw new polygon
      Draw();
   }
}

void PolyTool::RemovePoint()
{
   if (_polygon->count())
   {
		CalculateCurrent();
      Draw();

  		//GLOB->SCRDRV->WarpPointer(x,y);
      TDLI<Segment> 	_PI(_polygon);
      _PI.totail();
      _PI--;
      delete _PI.item();
      _PI.remove();
      _segm=(Line_Seg*)_PI.item();

      //draw new polygon
      Draw();
   }
}

void PolyTool::End()
{
	EvLeftDClick();
}

PolyLineTool::PolyLineTool(ToolControl* controller):BaseTool(controller)
{
   _toolcursor=wxCURSOR_CROSS;
   _polyline = new G_Polyline();
   _path = new Path(_polyline,0);
	_path->SetWidth(GLOB->CMDH->GetWidth()/GLOB->PHYSUNITS());
   _path->SetPathtype((PATHTYPE)(GLOB->CMDH->GetPathtype()));
   _path->SetSpline(GLOB->CMDH->GetSpline());
   _firstpoint=false;
	_tooltype = POLYLINE_TOOL;
	m_toolbar = TOOLBAR_DRAW;
}

PolyLineTool::~PolyLineTool()
{
   if (_firstpoint)
	   Draw();
   delete _path;
}

wxString  PolyLineTool::GetToolInfo()
{
   return "draw polyline";
}

bool PolyLineTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
      case SELECT_TOOL:
      case DESELECT_TOOL:
      case MOVE_TOOL:
      case ROTATE_TOOL:
      case COPY_TOOL:
      case DELETE_TOOL:
      case EDIT_TOOL:
      case EDITPROPERTY_TOOL:
      case POLYGON_TOOL:
      case POLYLINE_TOOL:
      case CIRCLE2_TOOL:
      case CIRCLE_TOOL:
      case RECTANGLE_TOOL:
         return true;
      default:
         return false;
   }
}

void PolyLineTool::Draw()
{
   //draw new polygon
   if (_polyline->count())
   {
      wxString len;
      len.Printf("len: %f",_path->GetPolyline()->CalcLength());
   	_statusbar->SetProcess((char*)len.c_str());
      wxClientDC temp(GLOB->CANVAS);
      GLOB->SCRDRV->SetDC(&temp);
      temp.SetLogicalFunction(wxINVERT);
      _path->SetWidth(GLOB->CMDH->GetWidth()/GLOB->PHYSUNITS());
	   _path->SetSpline(GLOB->CMDH->GetSpline());
      _path->SetPathtype((PATHTYPE)(GLOB->CMDH->GetPathtype()));
	   _path->CalcBoundingBox();
      GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
      GLOB->SCRDRV->WriteDirect(_path);
      GLOB->SCRDRV->SetDrawStyle(STORED);
	   GLOB->SCRDRV->SetDC(0);
   }
//	SetStatusBar();
}

void PolyLineTool::EvDrag()
{
//	  	GLOB->SCRDRV->CaptureMouse();
   	EvMove(1);
}

void PolyLineTool::EvMove(int WXUNUSED(drag))
{
   if (_event.ControlDown())
   {
      Grid();
      CalculateCurrent();
      _realcurrentX=_lastsegm.GetX();
   }
   else if (_event.ShiftDown())
   {
      Grid();
      CalculateCurrent();
      _realcurrentY=_lastsegm.GetY();
   }
   else
   {
      Grid();
      CalculateCurrent();
   }
   if (GLOB->CMDH->GetViewCoord())
      _statusbar->SetXY(_realcurrentX,_realcurrentY);
   if (_firstpoint)
   {
      Draw();
      _segm->SetX(_realcurrentX);
      _segm->SetY(_realcurrentY);
      //remove old Xor lines
      Draw();
	}
}

void PolyLineTool::EvLeftDClick()
{

   if (_polyline->count())
   {
      GLOB->MAINWINDOW->set_Cursor("wait");

      _polyline->removetail(); //because an extra LD is generated at DClick
	   G_Polyline* a_polyline = new G_Polyline();
      *a_polyline = *_polyline;

      Path* a_path=new Path(GLOB->CMDH->GetCurrentLayer());
      a_path->SetPathtype((PATHTYPE)(GLOB->CMDH->GetPathtype()));
		a_path->SetWidth(GLOB->CMDH->GetWidth()/GLOB->PHYSUNITS());
		a_path->SetSpline(GLOB->CMDH->GetSpline());

		a_path->SetPolyline(a_polyline);
      a_path->CalcBoundingBox();
		GLOB->CMDH->GetTopElementList()->insend(a_path);
	   GLOB->SETUP->Set_Available(GLOB->CMDH->GetCurrentLayer(), true);
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();

      Command* _the_command = new Command();
      _the_command->Set_Keyword("end");
      _the_command->Set_Command("end");
      _the_command->Set_Tool(_tooltype);
      _the_command->Set_EventStamp(a_path->GetStamp());
      _polyline->MakeEmpty();

      GLOB->CMDH->ToHistory(_the_command);
      GLOB->SCRDRV->SetClippingRegionBox(a_path->GetBoundingBox());
      GLOB->CMDH->DoCommand("draw");
      GLOB->SCRDRV->DestroyClippingRegion();
      GLOB->MAINWINDOW->set_Cursor("ready");

      CalculateCurrent();
      GLOB->MAINWINDOW->SetPoint(_realcurrentX,_realcurrentY);
      _firstpoint=false;
      _controller->EndTool();
  }
   _busy=false;
}

void PolyLineTool::EvLD()
{
   _busy=true;
   Grid();
	CalculateCurrent();
   //if this is the first point then only add the point
   if (!_firstpoint)
   {
      _path->SetWidth(GLOB->CMDH->GetWidth()/GLOB->PHYSUNITS());
	   _path->SetSpline(GLOB->CMDH->GetSpline());
      _path->SetPathtype((PATHTYPE)(GLOB->CMDH->GetPathtype()));
      _segm=new Line_Seg();
      _segm->SetX(_realcurrentX);
      _segm->SetY(_realcurrentY);
      _polyline->AddSegment(_segm);
		_firstpoint=true;
      _segm=new Line_Seg();
      _segm->SetX(_realcurrentX);
      _segm->SetY(_realcurrentY);
      _polyline->AddSegment(_segm);
      GLOB->CMDH->ToHistory("begin");
      Draw();
      char buf[40];
      sprintf(buf, "%15.3f %15.3f", _realcurrentX,_realcurrentY);
      GLOB->CMDH->ToHistory("addpoint %s ",buf);
   }
   else
   {
      //remove polygon drawn during last left down
      Draw();
      char buf[40];
      sprintf(buf, "%15.3f %15.3f", _realcurrentX,_realcurrentY);
      GLOB->CMDH->ToHistory("addpoint %s ",buf);
      //directly addpoint
      _segm=new Line_Seg();
      _segm->SetX(_realcurrentX);
      _segm->SetY(_realcurrentY);
      _polyline->AddSegment(_segm);
      _path->CalcBoundingBox();
      //draw new polygon
      Draw();
   }
   _lastsegm=*_segm;
}

void PolyLineTool::EvLU()
{
//  	GLOB->SCRDRV->ReleaseMouse();
}

void PolyLineTool::AddPoint(double x,double y)
{
   _realcurrentX=x;
   _realcurrentY=y;
   int xd = (int) GLOB->SCRDRV->LogicalToDeviceX(x);
   int yd = (int) GLOB->SCRDRV->LogicalToDeviceY(y);
  	GLOB->CANVAS->WarpPointer(xd,yd);
   _busy=true;
   //if this is the first point then only add the point
   if (!_firstpoint)
   {
      _segm=new Line_Seg();
      _segm->SetX(_realcurrentX);
      _segm->SetY(_realcurrentY);
      _polyline->AddSegment(_segm);
		_firstpoint=true;
      _segm=new Line_Seg();
      _segm->SetX(_realcurrentX);
      _segm->SetY(_realcurrentY);
      _polyline->AddSegment(_segm);
      GLOB->CMDH->ToHistory("begin");
      char buf[40];
      sprintf(buf, "%15.3f %15.3f", _realcurrentX,_realcurrentY);
      GLOB->CMDH->ToHistory("addpoint %s ",buf);
   }
   else
   {
      //remove polygon drawn during last left down
      Draw();
      //change the current value of the extra segment use to show move of cursor
      _segm->SetX(_realcurrentX);
      _segm->SetY(_realcurrentY);
      char buf[40];
      sprintf(buf, "%15.3f %15.3f", _realcurrentX,_realcurrentY);
      GLOB->CMDH->ToHistory("addpoint %s ",buf);
      //directly addpoint
      _segm=new Line_Seg();
      _segm->SetX(_realcurrentX);
      _segm->SetY(_realcurrentY);
      _polyline->AddSegment(_segm);

      //draw new polygon
      Draw();
   }
}

void PolyLineTool::RemovePoint()
{
   if (_polyline->count())
   {
		CalculateCurrent();
      //remove path drawn during last left down
      Draw();

  		//GLOB->SCRDRV->WarpPointer(x,y);
      TDLI<Segment> 	_PI(_polyline);
      _PI.totail();
      _PI--;
      delete _PI.item();
      _PI.remove();
      _segm=(Line_Seg*)_PI.item();

      _path->CalcBoundingBox();
      //draw new path
      Draw();
   }
}

void PolyLineTool::End()
{
	EvLeftDClick();
}

MoveTool::MoveTool(ToolControl* controller):BaseTool(controller)
{
   _toolcursor=wxCURSOR_HAND;
  	_trans_matrix 	= new wxTransformMatrix();
   _elemlst = new 		ElementList();
	_tooltype = MOVE_TOOL;
	m_toolbar = TOOLBAR_DRAW;
}

MoveTool::~MoveTool()
{
   if (_busy)
   {
         //draw the move line at the old position just before the move
         //move and draw local ELEMENT LIST
         Draw();
         ElementList* dest=GLOB->CMDH->GetTopElementList();
         dest->takeover(_elemlst_orig);
         dest->CalcBoundingBox();
         GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
   }
   delete _elemlst;
   delete _trans_matrix;
}

wxString  MoveTool::GetToolInfo()
{
   return "move selected";
}


bool MoveTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
      case SELECT_TOOL:
      case DESELECT_TOOL:
      case ROTATE_TOOL:
      case COPY_TOOL:
      case DELETE_TOOL:
      case EDIT_TOOL:
      case EDITPROPERTY_TOOL:
            return true;
      default:
      	if (!_busy)
            return true;
         return false;
   }
}

void MoveTool::Undo()
{
   assert(_busy);
   //draw the move line at the old position just before the move
   //move and draw local ELEMENT LIST
   Draw();
   ElementList* dest=GLOB->CMDH->GetTopElementList();

   TDLI<Element> I2=TDLI<Element>(_elemlst);
   I2.delete_all();
   dest->takeover(_elemlst_orig);
   dest->CalcBoundingBox();
   GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
   GLOB->CMDH->DoCommand("draw");
   _busy=false;
}

void MoveTool::Draw()
{
//	SetStatusBar();
	if (_busy)
   {
         wxClientDC temp(GLOB->CANVAS);
         GLOB->SCRDRV->SetDC(&temp);
         temp.SetLogicalFunction(wxINVERT);
         GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
         GLOB->SCRDRV->GDSDrawLine(_oldX,_oldY,_newX,_newY);
         GLOB->SCRDRV->WriteDirect(_elemlst);
         GLOB->SCRDRV->SetDrawStyle(STORED);
   	   GLOB->SCRDRV->SetDC(0);
   }
}

void MoveTool::EvDrag()
{
//   GLOB->SCRDRV->CaptureMouse();
   Grid();
	CalculateCurrent();
	if (GLOB->CMDH->GetViewCoord())
   {
		_statusbar->SetXY(_realcurrentX,_realcurrentY);
	}
//   	WarpPointer(200, 200);
   if (_busy)
   {
         //draw the move line at the old position just before the move
         //move and draw local ELEMENT LIST
         Draw();
         _trans_matrix->Identity();

//         _trans_matrix->Mirror(0,0);
//         _trans_matrix->Scale(1.0,1.0, 0, 0);
         _trans_matrix->Translate(_realcurrentX-_newX,_realcurrentY-_newY);
//         _trans_matrix->Rotate(0,0,0);
         _elemlst->Transform(_trans_matrix,ELE_SELECTED);
         _elemlst->CalcBoundingBox();
         _newX=_realcurrentX;
         _newY=_realcurrentY;
         Draw();
   }
}

void MoveTool::EvMove(int WXUNUSED(drag))
{
   Grid();
	CalculateCurrent();
	if (GLOB->CMDH->GetViewCoord())
   {
		_statusbar->SetXY(_realcurrentX,_realcurrentY);
	}
}

void MoveTool::EvLeftDClick()
{
   Grid();
	CalculateCurrent();
   if (_busy)
   {
      Draw();
      _eventstamp=++GLOB->EVENT_COUNTER;
      _elemlst->SetTimeStamp(_eventstamp);
      ElementList* dest=GLOB->CMDH->GetTopElementList();
      _elemlst->CalcBoundingBox();
		dest->takeover(_elemlst);
		dest->SortOnBoundingBox();
      dest->CalcBoundingBox();
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();

      Command* _the_command = new Command();
      _the_command->Set_Keyword("move");
      _the_command->Set_Command("move %f %f",_realcurrentX,_realcurrentY);
      _the_command->Set_EventStamp(_eventstamp);
      _the_command->Set_ElementList(_elemlst_orig);

	   _busy=false;
      GLOB->CMDH->ToHistory(_the_command);
      GLOB->SCRDRV->SetClippingRegionBox(_elemlst_orig->GetBoundingBox());
      GLOB->CMDH->DoCommand("draw");
      GLOB->SCRDRV->SetClippingRegionBox(_elemlst->GetBoundingBox());
      GLOB->CMDH->DoCommand("draw");
      GLOB->SCRDRV->DestroyClippingRegion();

      GLOB->MAINWINDOW->set_Cursor("ready");

      CalculateCurrent();
      GLOB->MAINWINDOW->SetPoint(_realcurrentX,_realcurrentY);
	   _controller->EndTool();
   }
}

void MoveTool::EvLD()
{
   Grid();
   CalculateCurrent();
   //was the cursor already in a selected item
   if (_busy)
   	  EvDrag();
   else
   {
	   CalcVirtual(_currentX,_currentY,_oldX,_oldY);
      Point a_point(_realcurrentX,_realcurrentY);
      if (!GLOB->CMDH->GetTopElementList()->SelectedAtPoint(&a_point))
      {
         _statusbar->SetProcess("first select item Ctrl Left down");

         //NOT convenient
         //put the MOVE tool on the stack and inistiate the SELECT tool
         //_controller->StartTool(SELECT_TOOL,true);
         //transfer the EvLD event to the MOVE tool
         //_controller->GetActiveTool()->OnMouseEvent(_event);
         return;
      }

      //get the selected elements from the showstructure and move them to the local
      //element list
      bool found=false;

      TDLI<Element> I2=TDLI<Element>(GLOB->CMDH->GetTopElementList());
      I2.tohead();
      while (!I2.hitroot())
      {
         if (I2.item()->GetSelect())
         {
            Element* a_ele=I2.item();
            I2.remove();
            _elemlst->insbegin(a_ele);
            found=true;
         }
         else
            I2++;
      }
      if (found)
      {
		   _elemlst_orig = new 		ElementList();
			*_elemlst_orig=*_elemlst;
         _elemlst->CalcBoundingBox();
         _elemlst_orig->CalcBoundingBox();
			_newX=_realcurrentX;
			_newY=_realcurrentY;
		   _busy=true;
      }
   }
}

void MoveTool::EvLU()
{
   Grid();
	CalculateCurrent();
   if (_oldX!=_realcurrentX && _oldY!=_realcurrentY) //need to stop?
   {
//	  GLOB->SCRDRV->ReleaseMouse();
   }
}


CopyTool::CopyTool(ToolControl* controller):BaseTool(controller)
{
   _toolcursor=wxCURSOR_HAND;
  	_trans_matrix 	= new wxTransformMatrix();
   _elemlst = new 		ElementList();
	_tooltype = COPY_TOOL;
	m_toolbar = TOOLBAR_DRAW;
}

CopyTool::~CopyTool()
{
   if (_busy)
         Draw();
   delete _trans_matrix;
   delete _elemlst;
}

wxString  CopyTool::GetToolInfo()
{
   return "copy selected";
}

bool CopyTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
      case SELECT_TOOL:
      case DESELECT_TOOL:
      case MOVE_TOOL:
      case ROTATE_TOOL:
      case DELETE_TOOL:
      case EDIT_TOOL:
      case EDITPROPERTY_TOOL:
         return true;
      default:
      	if (!_busy)
            return true;
         return false;
   }
}

void CopyTool::Undo()
{
   assert(_busy);
   //draw the move line at the old position just before the move
   //move and draw local ELEMENT LIST
   Draw();
   TDLI<Element> I2=TDLI<Element>(_elemlst);
   I2.delete_all();
   _busy=false;
}

void CopyTool::Draw()
{
//	SetStatusBar();
	if (_busy)
   {
         wxClientDC temp(GLOB->CANVAS);
         GLOB->SCRDRV->SetDC(&temp);
         temp.SetLogicalFunction(wxINVERT);
         GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
         GLOB->SCRDRV->GDSDrawLine(_oldX,_oldY,_newX,_newY);
         GLOB->SCRDRV->WriteDirect(_elemlst);
         GLOB->SCRDRV->SetDrawStyle(STORED);
   	   GLOB->SCRDRV->SetDC(0);
   }
}

void CopyTool::EvDrag()
{
//   GLOB->SCRDRV->CaptureMouse();
   Grid();
	CalculateCurrent();
	if (GLOB->CMDH->GetViewCoord())
   {
		_statusbar->SetXY(_realcurrentX,_realcurrentY);
	}
//   	WarpPointer(200, 200);
   if (_busy)
   {
         //draw the move line at the old position just before the move
         //move and draw local ELEMENT LIST
         Draw();
         _trans_matrix->Identity();

//         _trans_matrix->Mirror(0,0);
//         _trans_matrix->Scale(1.0,1.0, 0, 0);
         _trans_matrix->Translate(_realcurrentX-_newX,_realcurrentY-_newY);
//         _trans_matrix->Rotate(0,0,0);
         _elemlst->Transform(_trans_matrix,ELE_SELECTED);
         _elemlst->CalcBoundingBox();
         _newX=_realcurrentX;
         _newY=_realcurrentY;
         Draw();
   }
}

void CopyTool::EvMove(int WXUNUSED(drag))
{
   Grid();
	CalculateCurrent();
	if (GLOB->CMDH->GetViewCoord())
   {
		_statusbar->SetXY(_realcurrentX,_realcurrentY);
	}
}

void CopyTool::EvLeftDClick()
{
   if (_busy)
   {
      Draw();
      _elemlst->ClearFlags(ELE_SELECTED);
      ElementList* dest=GLOB->CMDH->GetTopElementList();
      _elemlst->CalcBoundingBox();
		dest->takeover(_elemlst);
      dest->CalcBoundingBox();
		dest->SortOnBoundingBox();
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();

      Command* _the_command = new Command();
      _the_command->Set_Keyword("copy");
      _the_command->Set_Command("copy %f %f",_realcurrentX,_realcurrentY);
      _the_command->Set_EventStamp(_eventstamp);
      GLOB->CMDH->ToHistory(_the_command);
	   _busy=false;
      GLOB->SCRDRV->SetClippingRegionBox(_elemlst->GetBoundingBox());
      GLOB->CMDH->DoCommand("draw");
      GLOB->SCRDRV->DestroyClippingRegion();

      GLOB->MAINWINDOW->set_Cursor("ready");

      CalculateCurrent();
      GLOB->MAINWINDOW->SetPoint(_realcurrentX,_realcurrentY);
	   _controller->EndTool();
   }
}

void CopyTool::EvLD()
{
   Grid();
   CalculateCurrent();
   //was the cursor already in a selected item
   if (_busy) //need to stop?
	  EvDrag();
   else
   {
	   CalcVirtual(_currentX,_currentY,_oldX,_oldY);
      Point a_point(_realcurrentX,_realcurrentY);
      if (!GLOB->CMDH->GetTopElementList()->SelectedAtPoint(&a_point))
      {
         _statusbar->SetProcess("first select item Ctrl Left down");

         //put the COPY tool on the stack and inistiate the SELECT tool
         //_controller->StartTool(SELECT_TOOL,true);
         //transfer the EvLD event to the SELECT tool
         //_controller->GetActiveTool()->OnMouseEvent(_event);
         return;
      }

      //get the selected elements from the showstructure and move them to the local
      //element list
      bool found=false;

      //get the selected elements from the showstructure and move them to the local
      //element list
      {
         TDLI<Element> olditer=TDLI<Element>(GLOB->CMDH->GetTopElementList());

         olditer.tohead();
         while (!olditer.hitroot())
         {
            if (olditer.item()->GetSelect())
            {
               Element* a_ele=olditer.item()->Duplicate();
              	_elemlst->insend(a_ele);
	            found=true;
            }
            olditer++;
         }
      }

      if (found)
      {
         _elemlst->CalcBoundingBox();
         _eventstamp=++GLOB->EVENT_COUNTER;
         _elemlst->SetTimeStamp(_eventstamp);
         _elemlst->SetFlags(ELE_SELECTED,false);
			_newX=_realcurrentX;
			_newY=_realcurrentY;
		   _busy=true;
      }
   }
}

void CopyTool::EvLU()
{
   Grid();
	CalculateCurrent();
//   if (_oldX!=_realcurrentX && _oldY!=_realcurrentY) //need to stop?
//	  GLOB->SCRDRV->ReleaseMouse();
}

CircleTool::CircleTool(ToolControl* controller,bool mode):BaseTool(controller)
{
   _toolcursor=wxCURSOR_CROSS;
   _circle = new Circle();
   _circle->SetRadius(1);
   _circle->SetMid(0,0);
   _started=false;
   _mode=mode;
	m_toolbar = TOOLBAR_DRAW;
   if (_mode)
		_tooltype = CIRCLE_TOOL;
   else
		_tooltype = CIRCLE2_TOOL;
}

CircleTool::~CircleTool()
{
   if (_started)
	   Draw();
   delete _circle;
}

wxString  CircleTool::GetToolInfo()
{
   return "draw circle";
}


bool CircleTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
      case SELECT_TOOL:
      case DESELECT_TOOL:
      case MOVE_TOOL:
      case ROTATE_TOOL:
      case COPY_TOOL:
      case DELETE_TOOL:
      case EDIT_TOOL:
      case EDITPROPERTY_TOOL:
      case POLYGON_TOOL:
      case POLYLINE_TOOL:
      case CIRCLE2_TOOL:
      case CIRCLE_TOOL:
      case RECTANGLE_TOOL:
         return true;
      default:
         return false;
   }
}

void CircleTool::Draw()
{
   if (_circle->GetRadius() > 0)
   {
      wxClientDC temp(GLOB->CANVAS);
      GLOB->SCRDRV->SetDC(&temp);
      temp.SetLogicalFunction(wxINVERT);
      _circle->CalcBoundingBox();
	   GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
      GLOB->SCRDRV->WriteDirect(_circle);
	   GLOB->SCRDRV->SetDrawStyle(STORED);
	   GLOB->SCRDRV->SetDC(0);
   }
}

void CircleTool::EvMove(int WXUNUSED(drag))
{
   if (_started)
   {
//      GLOB->SCRDRV->CaptureMouse();
      Grid();
      CalculateCurrent();

      //remove old Xor lines
      if (GLOB->CMDH->GetViewCoord())
         _statusbar->SetXY(_realcurrentX,_realcurrentY);
      if (_circle->GetRadius() > 0)
      {
         //first remove last drawn circle
         Draw();

         if (_mode)
         {
            //calculate new circle
            double radius=sqrt(pow(_firstX-_realcurrentX,2)+ pow(_firstY-_realcurrentY,2));
            if (radius < 1) radius=1;
            _circle->SetRadius(radius);
         }
         else
         {
            //calculate new circle
            double radius=sqrt(pow(_firstX-_realcurrentX,2)+ pow(_firstY-_realcurrentY,2))/2.0;
            if (radius < 1) radius=1;
            _circle->SetRadius(radius);
            _circle->SetMid((_firstX+_realcurrentX)/2.0,(_firstY+_realcurrentY)/2.0);
         }

         Draw();
      }
   }
}

void CircleTool::EvDrag()
{
   if (_started) //only treat this event after receiving a LD event, sometimes i get Drag event first, why?
	{
//  		 GLOB->SCRDRV->CaptureMouse();
   	 Grid();
		 CalculateCurrent();

   	 //remove old Xor lines
		 if (GLOB->CMDH->GetViewCoord())
			 _statusbar->SetXY(_realcurrentX,_realcurrentY);
   	 if (_circle->GetRadius() > 0)
   	 {
      	 //first remove last drawn circle
      	 Draw();

      	 //calculate new circle
      	 if (_mode)
      	 {
         	 //calculate new circle
         	 double radius=sqrt(pow(_firstX-_realcurrentX,2)+ pow(_firstY-_realcurrentY,2));
         	 if (radius < 1) radius=1;
         	 _circle->SetRadius(radius);
      	 }
      	 else
      	 {
         	 //calculate new circle
         	 double radius=sqrt(pow(_firstX-_realcurrentX,2)+ pow(_firstY-_realcurrentY,2))/2.0;
         	 if (radius < 1) radius=1;
         	 _circle->SetRadius(radius);
         	 _circle->SetMid((_firstX+_realcurrentX)/2.0,(_firstY+_realcurrentY)/2.0);
      	 }
      	 Draw();
   	}
	}
}


void CircleTool::EvLD()
{
   if (_started) //need to stop?
       EvLU();
   else
   {
	   _busy=true;
      _started=true;
      Grid();
      CalculateCurrent();
      _firstX=_realcurrentX;
      _firstY=_realcurrentY;
      _circle->SetMid(_firstX,_firstY);
      _circle->SetRadius(1);
		_circle->SetWidth(GLOB->CMDH->GetWidth()/GLOB->PHYSUNITS());
      Draw();
   }
}

void CircleTool::EvLU()
{
   Grid();
	CalculateCurrent();
   if (_firstX!=_realcurrentX && _firstY!=_realcurrentY) //need to stop?
   {
//      GLOB->SCRDRV->ReleaseMouse();
      if (_circle->GetRadius() > 0)
      {
         GLOB->MAINWINDOW->set_Cursor("wait");

         //first remove last drawn circle
         Draw();

         Circle* a_circle=new Circle(GLOB->CMDH->GetCurrentLayer());
         a_circle->SetRadius(_circle->GetRadius());
         a_circle->SetMid(_circle->GetMid());
			a_circle->SetWidth(GLOB->CMDH->GetWidth()/GLOB->PHYSUNITS());
         a_circle->CalcBoundingBox();
         GLOB->CMDH->GetTopElementList()->insend(a_circle);
         GLOB->SETUP->Set_Available(GLOB->CMDH->GetCurrentLayer(), true);
         GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
         Command* _the_command = new Command();
         _the_command->Set_Keyword("end");
         _the_command->Set_Command("end");
         _the_command->Set_Tool(_tooltype);
         _the_command->Set_EventStamp(a_circle->GetStamp());

         _circle->SetRadius(0);
         GLOB->CMDH->ToHistory(_the_command);
         GLOB->SCRDRV->SetClippingRegionBox(a_circle->GetBoundingBox());
         GLOB->CMDH->DoCommand("draw");
         GLOB->SCRDRV->DestroyClippingRegion();
         GLOB->MAINWINDOW->set_Cursor("ready");

         GLOB->MAINWINDOW->SetPoint(_realcurrentX,_realcurrentY);
         _controller->EndTool();
		   _busy=false;
         _started=false;
      }
   }
}

DeleteTool::DeleteTool(ToolControl* controller):BaseTool(controller)
{
   _toolcursor=wxCURSOR_ARROW;
	_tooltype = DELETE_TOOL;
   _elemlst = new 		ElementList();
	m_toolbar = TOOLBAR_DRAW;
}

DeleteTool::~DeleteTool()
{
	delete _elemlst;
}

wxString  DeleteTool::GetToolInfo()
{
   return "delete selected";
}

bool DeleteTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
      case SELECT_TOOL:
      case DESELECT_TOOL:
         return true;
      default:
         return false;
   }
}

void DeleteTool::Draw()
{
	SetStatusBar();
}

void DeleteTool::EvLD()
{
   CalculateCurrent();
   Point a_point(_realcurrentX,_realcurrentY);
   //get the selected elements from the showstructure and move them to the local
   //element list
   bool found=false;

   TDLI<Element> I2=TDLI<Element>(GLOB->CMDH->GetTopElementList());
   I2.tohead();
   while (!I2.hitroot() && !found)
   {
      if (I2.item()->GetSelect() && (I2.item()->Select(&a_point)))
      {
         found=true;
         Element* a_ele=I2.item();
         _elemlst->insbegin(a_ele);
         I2.remove();
      }
      else
         I2++;
   }
   if (found)
   {
      _elemlst->CalcBoundingBox();
	   GLOB->CMDH->Get_GDSII()->CalcBoundingBox();

      Command* _the_command = new Command();
      _the_command->Set_Keyword("delete");
      _the_command->Set_Command("delete %f %f",_realcurrentX,_realcurrentY);
      _the_command->Set_ElementList(_elemlst);

      GLOB->CMDH->ToHistory(_the_command);
      GLOB->SCRDRV->SetClippingRegionBox(_elemlst->GetBoundingBox());
      GLOB->CMDH->DoCommand("draw");
      GLOB->SCRDRV->DestroyClippingRegion();
      GLOB->MAINWINDOW->set_Cursor("ready");
	   _elemlst = new 		ElementList();
   }
   _controller->EndTool();
}

EditTool::EditTool(ToolControl* controller):BaseTool(controller)
{
   _toolcursor=wxCURSOR_HAND;
	_tooltype = EDIT_TOOL;
  	strcpy(_unit,"um");
	m_toolbar = TOOLBAR_DRAW;
}

EditTool::~EditTool()
{
}

wxString  EditTool::GetToolInfo()
{
   return "edit selected";
}

bool EditTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
      case EDITPOLYGON_TOOL:
      case EDITCIRCLE_TOOL:
      case EDITREC_TOOL:
      case EDITLINE_TOOL:
      case EDITREF_TOOL:
      case EDITIMAGE_TOOL:
      case EDITPOLYLINE_TOOL:
      case EDITPROPERTY_TOOL:
         return true;
      default:
         if (!_busy)
         {
            switch(tool_to_start_next)
            {
               case SELECT_TOOL:
               case DESELECT_TOOL:
               case MOVE_TOOL:
               case ROTATE_TOOL:
               case COPY_TOOL:
               case DELETE_TOOL:
               return true;
            }
         }
         return false;
   }
}

void EditTool::Draw()
{
	SetStatusBar();
}

void EditTool::EvLD()
{
   CalculateCurrent();
   Point a_point(_realcurrentX,_realcurrentY);
   //get the first selected element from the showstructure and move them to the local
   //element
   bool found=false;
   Element* a_ele=0;

   {
      TDLI<Element> I2=TDLI<Element>(GLOB->CMDH->GetTopElementList());
      I2.tohead();
      while (!I2.hitroot() && !found)
      {
         if (I2.item()->GetSelect() && (I2.item()->Select(&a_point)))
         {
            a_ele=I2.item();
            found=true;
         }
         else
            I2++;
      }
   }
   if (found)
   {
	   _busy=true;
		switch (a_ele->GetType())
		{
			case BOX:
         {
            //put the EDIT tool on the stack and inistiate the EDITREC tool
            _controller->StartTool(EDITREC_TOOL,true);
            //transfer the EvLD event to the EDITREC tool
            _controller->GetActiveTool()->OnMouseEvent(_event);
            break;
         }
			case SLINE:
         {
            //put the EDIT tool on the stack and inistiate the EDITREC tool
            _controller->StartTool(EDITLINE_TOOL,true);
            //transfer the EvLD event to the EDITREC tool
            _controller->GetActiveTool()->OnMouseEvent(_event);
            break;
         }
			case IMAGE:
         {
            //put the EDIT tool on the stack and inistiate the EDITIMAGE tool
            _controller->StartTool(EDITIMAGE_TOOL,true);
            //transfer the EvLD event to the EDITIMAGE tool
            _controller->GetActiveTool()->OnMouseEvent(_event);
            break;
         }
			case CIRCLE:
         {
            //put the EDIT tool on the stack and inistiate the EDITCIRCLE tool
            _controller->StartTool(EDITCIRCLE_TOOL,true);
            //transfer the EvLD event to the EDITCIRCLE tool
            _controller->GetActiveTool()->OnMouseEvent(_event);

            break;
         }
//			case ARCSEG:
			case BOUNDARY:
         {
            //put the EDIT tool on the stack and inistiate the EDITPOLYGON tool
            _controller->StartTool(EDITPOLYGON_TOOL,true);
            //transfer the EvLD event to the EDITPOLYGON tool
            _controller->GetActiveTool()->OnMouseEvent(_event);
            break;
         }
			case PATH:
         {
         	//put the EDIT tool on the stack and inistiate the EDITPOLYGON tool
            _controller->StartTool(EDITPOLYLINE_TOOL,true);
            //transfer the EvLD event to the EDITPOLYLINE tool
            _controller->GetActiveTool()->OnMouseEvent(_event);
/*
				PolyeditDialog polydlg(GLOB->MAINWINDOW);
            polydlg.SetUnit(_unit);
            polydlg.SetupWindow();
            polydlg.ShowModal();

            if (polydlg.ShowModal() == wxID_OK)
            {
	               Path* a_path=new Path(GLOB->CMDH->GetCurrentLayer());
	               a_path->SetSelect(TRUE);
         			a_path->SetPathtype((PATHTYPE)(GLOB->CMDH->GetPathtype()));
						a_path->SetWidth(polydlg.GetWidth()/GLOB->PHYSUNITS());
	               a_path->CalcBoundingBox();
	               strcpy(_unit, polydlg.GetUnit());
	               GLOB->CMDH->Get_GDSII()->CalcBoundingBox();

	               Command* _the_command = new Command();
	               _the_command->Set_Keyword("end");
	               _the_command->Set_Command("end");
   	            _the_command->Set_Tool(_tooltype);
      	         _the_command->Set_EventStamp(a_path->GetStamp());
         	      GLOB->CMDH->ToHistory(_the_command);
	               GLOB->CMDH->DoCommand("draw");
           }
*/
            break;
         }
			case TEXT:
         {
				TextinsDialog textdlg(GLOB->MAINWINDOW);
            Text* _text = (Text*) a_ele;
            textdlg.SetText(_text->MyText().c_str());
			   textdlg.SetUnit(_unit);
				textdlg.SetHeight(_text->GetHeight());
            if (textdlg.ShowModal() == wxID_OK)
            {
               Text* _text_orig = new Text(0);
               *_text_orig=*_text;
               _text_orig->SetSelect(true);
               _text->SetSelect(true);
               _text->SetText(textdlg.GetText());
               _text->SetHeight(textdlg.GetHeight());
               _text->CalcBoundingBox();
			   	strcpy(_unit,textdlg.GetUnit());
			      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();

               Command* _the_command = new Command();

               _the_command->Set_Keyword("end");
               _the_command->Set_Command("end");
               _the_command->Set_Tool(_tooltype);
               _the_command->Set_EventStamp(_text->GetStamp());
               GLOB->CMDH->ToHistory(_the_command);
               GLOB->CMDH->DoCommand("draw");
            }
				break;
			}

//			case AREF:

			case SREF:
         {
            //put the EDIT tool on the stack and inistiate the EDITIMAGE tool
            _controller->StartTool(EDITREF_TOOL,true);
            //transfer the EvLD event to the EDITREC tool
            _controller->GetActiveTool()->OnMouseEvent(_event);
            break;
         }
			default:
			   _messagehandler->info("editing not supported for this element","edit tool");
            break;
		}
	   _busy=false;
   }
}


EditPolyTool::EditPolyTool(ToolControl* controller):BaseTool(controller)
{
   _toolcursor=wxCURSOR_HAND;
   _AI = new TDLI<Segment>();
   _moving=false;
	_tooltype = EDITPOLYGON_TOOL;
	m_toolbar = TOOLBAR_DRAW;
   _boundary_orig = 0;
   _boundary = 0;
	SetStatusBar();
}

EditPolyTool::~EditPolyTool()
{
   if (_boundary)
   {
	   _AI->Detach();
      GLOB->MAINWINDOW->set_Cursor("wait");

		GLOB->CMDH->GetTopElementList()->insend(_boundary_orig);
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
      _boundary=0;

      Command* _the_command = new Command();
      _the_command->Set_Keyword("end");
      _the_command->Set_Command("end");
      _the_command->Set_Tool(_tooltype);
      GLOB->CMDH->ToHistory(_the_command);
      GLOB->CMDH->DoCommand("draw");

      GLOB->MAINWINDOW->set_Cursor("ready");
   }
   delete _boundary;
   delete _AI;
}

wxString  EditPolyTool::GetToolInfo()
{
   return "edit polygon";
}

bool EditPolyTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
         return true;
      default:
         return false;
   }
}

void EditPolyTool::Draw()
{
   if (_boundary)
   {
      if (_boundary->GetPolygon()->count())
      {
         wxClientDC temp(GLOB->CANVAS);
         GLOB->SCRDRV->SetDC(&temp);
	      GLOB->SCRDRV->DestroyClippingRegion();
         temp.SetLogicalFunction(wxINVERT);
         GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
         _boundary->CalcBoundingBox();
         GLOB->SCRDRV->WriteDirect(_boundary);
         _AI->totail();
         double xold,yold;
         xold = _AI->item()->GetX();
         yold = _AI->item()->GetY();
         _AI->tohead();
         while (!_AI->hitroot())
         {
            if (_AI->item()->GetX()!=xold || _AI->item()->GetY()!=yold)
            {
               double x,y;
            	x = _AI->item()->GetX();
            	y = _AI->item()->GetY();
					DrawHandle(temp,x,y);
            }else if (_AI->count()==2 && _AI->athead())
            {
               double x,y;
            	x = _AI->item()->GetX();
            	y = _AI->item()->GetY();
					DrawHandle(temp,x,y);
            }
            xold = _AI->item()->GetX();
            yold = _AI->item()->GetY();
            (*_AI)++;
         }

         GLOB->SCRDRV->SetDrawStyle(STORED);
   	   GLOB->SCRDRV->SetDC(0);
      }
   }
}

void EditPolyTool::XorDrawMoveLines()
{
      wxClientDC temp(GLOB->CANVAS);
      GLOB->SCRDRV->SetDC(&temp);
      GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
      if (_boundary->GetWidth())
      {
                  if (_boundary->GetPolygon()->count()>1)
            GLOB->SCRDRV->GDSDrawLine(_XN,_YN,_realcurrentX,_realcurrentY);
         GLOB->SCRDRV->GDSDrawLine(_XP,_YP,_realcurrentX,_realcurrentY);
      }
      GLOB->SCRDRV->SetDrawStyle(STORED);
	   GLOB->SCRDRV->SetDC(0);
}

void EditPolyTool::XorRemoveMoveLines()
{
      wxClientDC temp(GLOB->CANVAS);
      GLOB->SCRDRV->SetDC(&temp);
      temp.SetLogicalFunction(wxINVERT);
      GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
      //remove old lines
      if (_boundary->GetWidth())
      {
         if (_boundary->GetPolygon()->count()>1)
            GLOB->SCRDRV->GDSDrawLine(_XN,_YN,_prevX,_prevY);
         GLOB->SCRDRV->GDSDrawLine(_XP,_YP,_prevX,_prevY);
      }
      GLOB->SCRDRV->SetDrawStyle(STORED);
	   GLOB->SCRDRV->SetDC(0);
}

void EditPolyTool::EvDrag()
{
//  	GLOB->SCRDRV->CaptureMouse();
   if (_moving)
   {
      if (_event.ControlDown())
      {
         _prevX=_realcurrentX;
         _prevY=_realcurrentY;
         double prevcurX=_realcurrentX;
         Grid();
         CalculateCurrent();
         _realcurrentX=prevcurX;
      }
      else if (_event.ShiftDown())
      {
         _prevX=_realcurrentX;
         _prevY=_realcurrentY;
         double prevcurY=_realcurrentY;
         Grid();
         CalculateCurrent();
         _realcurrentY=prevcurY;
      }
      else
      {
         _prevX=_realcurrentX;
         _prevY=_realcurrentY;
         Grid();
         CalculateCurrent();
      }

      XorRemoveMoveLines();
  	   Draw();
      _PM->SetX(_realcurrentX);
      _PM->SetY(_realcurrentY);
      //remove old Xor lines
      if (GLOB->CMDH->GetViewCoord())
         _statusbar->SetXY(_realcurrentX,_realcurrentY);
  	   Draw();
      XorDrawMoveLines();
	}
}

void EditPolyTool::EvLeftDClick()
{
   if (_boundary->GetPolygon()->count())
   {
      Draw();
      XorRemoveMoveLines();
      _AI->Detach();
      GLOB->MAINWINDOW->set_Cursor("wait");

      _boundary->CalcBoundingBox();
		GLOB->CMDH->GetTopElementList()->insend(_boundary);
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
      _boundary->SetSelect(true);
      _boundary->SetStamp(++GLOB->EVENT_COUNTER);

      Command* _the_command = new Command();
      _the_command->Set_Keyword("end");
      _the_command->Set_Command("end");
      _the_command->Set_Tool(_tooltype);
      _the_command->Set_EventStamp(_boundary->GetStamp());
      _the_command->Set_Element(_boundary_orig);

      BoundingBox tmp=_boundary->GetBoundingBox();
      tmp.EnLarge(HANDLE*GLOB->SCRDRV->GetScaleFactorX());
      _boundary=0;

      GLOB->CMDH->ToHistory(_the_command);
      BoundingBox tmp2=_boundary_orig->GetBoundingBox();
      tmp2.EnLarge(HANDLE*GLOB->SCRDRV->GetScaleFactorX());
      GLOB->SCRDRV->SetClippingRegionBox(tmp2);
      GLOB->CMDH->DoCommand("draw");
      GLOB->SCRDRV->SetClippingRegionBox(tmp);
      GLOB->CMDH->DoCommand("draw");
      GLOB->SCRDRV->DestroyClippingRegion();

      GLOB->MAINWINDOW->set_Cursor("ready");

      GLOB->MAINWINDOW->SetPoint(_realcurrentX,_realcurrentY);
	   _controller->EndTool();
	   _busy=false;
   }
}

void EditPolyTool::EvLD()
{
   if (!_busy)
   {
	   CalculateCurrent();
	   Point a_point(_realcurrentX,_realcurrentY);
      //get the first boundary from the showstructure and move them to the local
      //polygon
      bool found=false;

      TDLI<Element> I2=TDLI<Element>(GLOB->CMDH->GetTopElementList());
      I2.tohead();
      while (!I2.hitroot() && !found)
      {
         if (I2.item()->GetSelect() && I2.item()->GetType()==BOUNDARY && (I2.item()->Select(&a_point)))
         {
            Element* a_ele=I2.item();
            I2.remove();
            _boundary = (Boundary*) a_ele;
			   _boundary_orig = new Boundary(0);
            *_boundary_orig=*_boundary;
            _boundary_orig->SetSelect(true);
            _boundary->SetSelect(false);
            found=true;
         }
         else
            I2++;
      }
      if (found)
      {
	      _AI->Attach(_boundary->GetPolygon());
   	   Draw();
		   _busy=true;
      }
  }
  else
  {
      bool found=false;
      double x,y;
		int xd, yd;

      //are we at a point of the boundary?
      _AI->tohead();
      while (!_AI->hitroot())
      {
         x = _AI->item()->GetX();
         y = _AI->item()->GetY();
         xd=(int) GLOB->SCRDRV->LogicalToDeviceX(x);
         yd=(int) GLOB->SCRDRV->LogicalToDeviceY(y);

         if (abs(_currentX-xd) < HANDLE && abs(_currentY-yd) < HANDLE)
         {
            found=true;
            break;
         }
         (*_AI)++;
      }

      if (found) //yes then start moving the point (draging or move)
      {
         _AI->prev_wrap();
         _XP = _AI->item()->GetX();
         _YP = _AI->item()->GetY();
         _AI->next_wrap();
         _PM = _AI->item();
         _AI->next_wrap();
         _XN= _AI->item()->GetX();
         _YN= _AI->item()->GetY();
         _AI->prev_wrap();

         _currentX=xd;
         _currentY=yd;
		   CalculateCurrent();
         _prevX=_realcurrentX;
         _prevY=_realcurrentY;
//         XorRemoveMoveLines();
         Grid();
         XorDrawMoveLines();
         _moving=true;
      }
   }
}

void EditPolyTool::EvLU()
{
   if (_busy && _moving)
   {
      _moving=false;
      _prevX=_realcurrentX;
      _prevY=_realcurrentY;
      XorRemoveMoveLines();
      _PM->SetX(_realcurrentX);
      _PM->SetY(_realcurrentY);
//      GLOB->SCRDRV->ReleaseMouse();
   }
}

void EditPolyTool::AddPoint(double x,double y)
{
   _realcurrentX=x;
   _realcurrentY=y;
   int xd=(int) GLOB->SCRDRV->LogicalToDeviceX(x);
   int yd=(int) GLOB->SCRDRV->LogicalToDeviceY(y);

  	GLOB->CANVAS->WarpPointer(xd,yd);
	EvLD();
}

void EditPolyTool::RemovePoint()
{
//   if (_polygon->count())
   {
	   _prevX=_realcurrentX;
   	_prevY=_realcurrentY;
		CalculateCurrent();
		XorRemoveMoveLines();
       //remove polygon drawn during last left down
      Draw();

/*
  		//GLOB->SCRDRV->WarpPointer(x,y);
      TDLI<Segment> 	_PI(_polygon);
      _PI.totail();
      delete _PI.item();
      _PI.remove();
      _PI.totail();
	   _last_P_X=_PI.item()->GetX();
   	_last_P_Y=_PI.item()->GetY();
*/
      //draw new polygon
      Draw();
   }
}

EditPolyLineTool::EditPolyLineTool(ToolControl* controller):BaseTool(controller)
{
   _toolcursor=wxCURSOR_HAND;
   _AI = new TDLI<Segment>();
   _moving=false;
	_tooltype = EDITPOLYLINE_TOOL;
	m_toolbar = TOOLBAR_DRAW;
   _path_orig = 0;
   _path = 0;
}

EditPolyLineTool::~EditPolyLineTool()
{
   if (_path)
   {
	   _AI->Detach();
      GLOB->MAINWINDOW->set_Cursor("wait");

		GLOB->CMDH->GetTopElementList()->insend(_path_orig);
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
      _path=0;

      Command* _the_command = new Command();
      _the_command->Set_Keyword("end");
      _the_command->Set_Command("end");
      _the_command->Set_Tool(_tooltype);
      GLOB->CMDH->ToHistory(_the_command);
      GLOB->CMDH->DoCommand("draw");

      GLOB->MAINWINDOW->set_Cursor("ready");
   }
   delete _path;
   delete _AI;
}

wxString  EditPolyLineTool::GetToolInfo()
{
   return "edit polyline";
}

bool EditPolyLineTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
         return true;
      default:
         return false;
   }
}

void EditPolyLineTool::Draw()
{
//	SetStatusBar();
   if (_path)
   {
      if (_path->GetPolyline()->count())
      {
         wxClientDC temp(GLOB->CANVAS);
         GLOB->SCRDRV->SetDC(&temp);
	      GLOB->SCRDRV->DestroyClippingRegion();
         temp.SetLogicalFunction(wxINVERT);
         GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
         _path->CalcBoundingBox();
         GLOB->SCRDRV->WriteDirect(_path);
         _AI->totail();
         double xold,yold;
         xold = _AI->item()->GetX();
         yold = _AI->item()->GetY();
         _AI->tohead();
         while (!_AI->hitroot())
         {
            if (_AI->item()->GetX()!=xold || _AI->item()->GetY()!=yold)
            {
               double x,y;
            	x = _AI->item()->GetX();
            	y = _AI->item()->GetY();
					DrawHandle(temp,x,y);
            }else if (_AI->count()==2 && _AI->athead())
            {
               double x,y;
            	x = _AI->item()->GetX();
            	y = _AI->item()->GetY();
					DrawHandle(temp,x,y);
            }
            xold = _AI->item()->GetX();
            yold = _AI->item()->GetY();
            (*_AI)++;
         }

         GLOB->SCRDRV->SetDrawStyle(STORED);
      }
	   GLOB->SCRDRV->SetDC(0);
   }
}

void EditPolyLineTool::XorDrawMoveLines()
{
      wxClientDC temp(GLOB->CANVAS);
      GLOB->SCRDRV->SetDC(&temp);
      temp.SetLogicalFunction(wxINVERT);
      GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
      if (_path->GetWidth())
      {
         if (_path->GetPolyline()->count()>1)
            GLOB->SCRDRV->GDSDrawLine(_XN,_YN,_realcurrentX,_realcurrentY);
         GLOB->SCRDRV->GDSDrawLine(_XP,_YP,_realcurrentX,_realcurrentY);
      }
      GLOB->SCRDRV->SetDrawStyle(STORED);
	   GLOB->SCRDRV->SetDC(0);
}

void EditPolyLineTool::XorRemoveMoveLines()
{
      wxClientDC temp(GLOB->CANVAS);
      GLOB->SCRDRV->SetDC(&temp);
      temp.SetLogicalFunction(wxINVERT);
      GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
      //remove old lines
      if (_path->GetWidth())
      {
         if (_path->GetPolyline()->count()>1)
	      	GLOB->SCRDRV->GDSDrawLine(_XN,_YN,_prevX,_prevY);
	      GLOB->SCRDRV->GDSDrawLine(_XP,_YP,_prevX,_prevY);
      }
      GLOB->SCRDRV->SetDrawStyle(STORED);
	   GLOB->SCRDRV->SetDC(0);
}

void EditPolyLineTool::EvDrag()
{
//  	GLOB->SCRDRV->CaptureMouse();
   if (_moving)
   {
      if (_event.ControlDown())
      {
         _prevX=_realcurrentX;
         _prevY=_realcurrentY;
         double prevcurX=_realcurrentX;
         Grid();
         CalculateCurrent();
         _realcurrentX=prevcurX;
      }
      else if (_event.ShiftDown())
      {
         _prevX=_realcurrentX;
         _prevY=_realcurrentY;
         double prevcurY=_realcurrentY;
         Grid();
         CalculateCurrent();
         _realcurrentY=prevcurY;
      }
      else
      {
         _prevX=_realcurrentX;
         _prevY=_realcurrentY;
         Grid();
         CalculateCurrent();
      }

      XorRemoveMoveLines();
  	   Draw();
     	_PM->SetX(_realcurrentX);
      _PM->SetY(_realcurrentY);
      //remove old Xor lines
      if (GLOB->CMDH->GetViewCoord())
         _statusbar->SetXY(_realcurrentX,_realcurrentY);
      XorDrawMoveLines();
  	   Draw();
	}
}

void EditPolyLineTool::EvLeftDClick()
{
   if (_path->GetPolyline()->count())
   {
      XorRemoveMoveLines();
  	   Draw();

      _AI->Detach();
      GLOB->MAINWINDOW->set_Cursor("wait");

      _path->CalcBoundingBox();
		GLOB->CMDH->GetTopElementList()->insend(_path);
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
      _path->SetSelect(true);

      _path->SetStamp(++GLOB->EVENT_COUNTER);

      Command* _the_command = new Command();
      _the_command->Set_Keyword("end");
      _the_command->Set_Command("end");
      _the_command->Set_Tool(_tooltype);
      _the_command->Set_EventStamp(_path->GetStamp());
      _the_command->Set_Element(_path_orig);

      BoundingBox tmp=_path->GetBoundingBox();
      _path=0;

      tmp.EnLarge(HANDLE*GLOB->SCRDRV->GetScaleFactorX());

      GLOB->CMDH->ToHistory(_the_command);
      GLOB->SCRDRV->SetClippingRegionBox(_path_orig->GetBoundingBox());
      GLOB->CMDH->DoCommand("draw");
      GLOB->SCRDRV->SetClippingRegionBox(tmp);
      GLOB->CMDH->DoCommand("draw");
      GLOB->SCRDRV->DestroyClippingRegion();
      GLOB->MAINWINDOW->set_Cursor("ready");

      GLOB->MAINWINDOW->SetPoint(_realcurrentX,_realcurrentY);
	   _controller->EndTool();
	   _busy=false;
   }
}

void EditPolyLineTool::EvLD()
{
   if (!_busy)
   {
	   CalculateCurrent();
	   Point a_point(_realcurrentX,_realcurrentY);
      //get the first boundary from the showstructure and move them to the local
      //polygon
      bool found=false;

      TDLI<Element> I2=TDLI<Element>(GLOB->CMDH->GetTopElementList());
      I2.tohead();
      while (!I2.hitroot() && !found)
      {
         if (I2.item()->GetSelect() && I2.item()->GetType()==PATH && (I2.item()->Select(&a_point)))
         {
            Element* a_ele=I2.item();
            I2.remove();
            _path = (Path*) a_ele;
			   _path_orig = new Path(0);
            *_path_orig=*_path;
            _path_orig->SetSelect(true);
            _path->SetSelect(false);
            found=true;
         }
         else
            I2++;
      }
      if (found)
      {
	      _AI->Attach(_path->GetPolyline());
   	   Draw();
		   _busy=true;
      }
  }
  else
  {
      bool found=false;
      double x,y;
		int xd, yd;

      //are we at a point of the boundary?
      _AI->tohead();
      while (!_AI->hitroot())
      {
         x = _AI->item()->GetX();
         y = _AI->item()->GetY();
         xd=(int) GLOB->SCRDRV->LogicalToDeviceX(x);
         yd=(int) GLOB->SCRDRV->LogicalToDeviceY(y);

         if (abs(_currentX-xd) < HANDLE && abs(_currentY-yd) < HANDLE)
         {
            found=true;
            break;
         }
         (*_AI)++;
      }

      if (found) //yes then start moving the point (draging or move)
      {
         _AI->prev_wrap();
         _XP = _AI->item()->GetX();
         _YP = _AI->item()->GetY();
         _AI->next_wrap();
         _PM = _AI->item();
         _AI->next_wrap();
         _XN= _AI->item()->GetX();
         _YN= _AI->item()->GetY();
         _AI->prev_wrap();

         _currentX=xd;
         _currentY=yd;
		   CalculateCurrent();
         _prevX=_realcurrentX;
         _prevY=_realcurrentY;
         Grid();
         XorDrawMoveLines();
         _moving=true;
      }
   }
}

void EditPolyLineTool::EvLU()
{
   if (_busy && _moving)
   {
     _moving=false;
      _prevX=_realcurrentX;
      _prevY=_realcurrentY;
      XorRemoveMoveLines();
      _PM->SetX(_realcurrentX);
      _PM->SetY(_realcurrentY);
//      GLOB->SCRDRV->ReleaseMouse();
   }
}

void EditPolyLineTool::AddPoint(double x,double y)
{
   _realcurrentX=x;
   _realcurrentY=y;
   int xd=(int) GLOB->SCRDRV->LogicalToDeviceX(x);
   int yd=(int) GLOB->SCRDRV->LogicalToDeviceY(y);
  	GLOB->CANVAS->WarpPointer(xd,yd);
	EvLD();
}

void EditPolyLineTool::RemovePoint()
{
//   if (_polygon->count())
   {
	   _prevX=_realcurrentX;
   	_prevY=_realcurrentY;
		CalculateCurrent();
		XorRemoveMoveLines();
       //remove polygon drawn during last left down
      Draw();
/*
  		//GLOB->SCRDRV->WarpPointer(x,y);
      TDLI<Segment> 	_PI(_polygon);
      _PI.totail();
      delete _PI.item();
      _PI.remove();
      _PI.totail();
	   _last_P_X=_PI.item()->GetX();
   	_last_P_Y=_PI.item()->GetY();
*/
      //draw new polygon
      Draw();
   }
}

EditPropertyTool::EditPropertyTool(ToolControl* controller):BaseTool(controller)
{
   _toolcursor=wxCURSOR_HAND;
	_tooltype = EDITPROPERTY_TOOL;
}

EditPropertyTool::~EditPropertyTool()
{
}

wxString  EditPropertyTool::GetToolInfo()
{
   return "edit element properties";
}


bool EditPropertyTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
      case SELECT_TOOL:
      case DESELECT_TOOL:
      case RECTANGLE_TOOL:
      case POLYGON_TOOL:
      case TEXT_TOOL:
      case POLYLINE_TOOL:
      case MOVE_TOOL:
      case ROTATE_TOOL:
      case COPY_TOOL:
      case CIRCLE_TOOL:
      case CIRCLE2_TOOL:
      case DELETE_TOOL:
      case EDITPOLYGON_TOOL:
      case EDITCIRCLE_TOOL:
      case EDITREC_TOOL:
      case EDITPOLYLINE_TOOL:
      case EDIT_TOOL:
      case EDITPROPERTY_TOOL:
      case INSERTREF_TOOL:
      case INSERTSTRUC_TOOL:
         return true;
      default:
         return false;
   }
}


void EditPropertyTool::Draw()
{
}

void EditPropertyTool::EvLD()
{
	   CalculateCurrent();
	   Point a_point(_realcurrentX,_realcurrentY);
      //get the first boundary from the showstructure and move them to the local
      //polygon
      bool found=false;

      Element* a_ele;

      TDLI<Element> I2=TDLI<Element>(GLOB->CMDH->GetTopElementList());
      I2.tohead();
      while (!I2.hitroot() && !found)
      {
         if (I2.item()->GetSelect() && (I2.item()->Select(&a_point)))
         {
            a_ele=I2.item();
            found=true;
         }
         else
            I2++;
      }
      if (found)
      {
            long style = (wxRESIZE_BORDER |wxTHICK_FRAME | wxSYSTEM_MENU | wxCAPTION);

            wxString title;
            switch (a_ele->GetType())
            {
               case BOUNDARY:
                     title="Boundary";
                     break;
               case PATH:
                     title="Path";
                     break;
               case ARCSEG:
                     title="Arcsegment";
                     break;
               case TEXT:
                     title="Text";
                     break;
               case BOX:
                     title="Rectangle";
                     break;
               case CIRCLE:
                     title="Circle";
                     break;
               case AREF:
                     title="Structure Array Reference";
                     break;
               case SREF:
                     title="Structure Reference";
                     break;

            }
            PropEditDialog* a_prop= new	PropEditDialog(GLOB->CANVAS,title,style,"propedit",a_ele->GetPropList());
				a_ele->UpdatePropList();
            delete a_prop;
      }
}

RotateTool::RotateTool(ToolControl* controller):BaseTool(controller)
{
  	_trans_matrix 	= new wxTransformMatrix();
   _elemlst = new 		ElementList();
	_tooltype = ROTATE_TOOL;
}

RotateTool::~RotateTool()
{
	if (_busy)
   {
      Draw();
      _trans_matrix->Identity();
//      _trans_matrix->Mirror(0,0);
//      _trans_matrix->Scale(1.0,1.0, 0, 0);
      double rang=0;
      if ((_newY-_rotY)!=0 &&(_newX-_rotY)!=0 )
         rang=atan2(_newY-_rotY,_newX-_rotX);
      rang = rang/ M_PI * 180;
      _trans_matrix->Rotate(-rang,_rotX,_rotY);
      _elemlst->Transform(_trans_matrix,ELE_SELECTED);
      _eventstamp=++GLOB->EVENT_COUNTER;
      _elemlst->SetTimeStamp(_eventstamp);
      ElementList* dest=GLOB->CMDH->GetTopElementList();
		dest->takeover(_elemlst);
      _elemlst->CalcBoundingBox();
      dest->CalcBoundingBox();
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();

      Command* _the_command = new Command();
      _the_command->Set_Keyword("tool");
      _the_command->Set_Command("tool rotate %f ",rang);
      _the_command->Set_EventStamp(_eventstamp);
      _the_command->Set_ElementList(_elemlst_orig);

      GLOB->CMDH->ToHistory(_the_command);
      GLOB->CMDH->DoCommand("draw");
      GLOB->MAINWINDOW->set_Cursor("ready");

      CalculateCurrent();
      GLOB->MAINWINDOW->SetPoint(_realcurrentX,_realcurrentY);
	   _busy=false;
	   _controller->EndTool();
   }

   delete _elemlst;
   delete _trans_matrix;
}

wxString  RotateTool::GetToolInfo()
{
   return "rotate selected";
}

bool RotateTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
      case SELECT_TOOL:
      case DESELECT_TOOL:
      case MOVE_TOOL:
      case COPY_TOOL:
         return true;
      default:
         return false;
   }
}

void RotateTool::Undo()
{
   assert(_busy);
   //draw the move line at the old position just before the move
   //move and draw local ELEMENT LIST
   Draw();
   ElementList* dest=GLOB->CMDH->GetTopElementList();

   TDLI<Element> I2=TDLI<Element>(_elemlst);
   I2.delete_all();
   dest->takeover(_elemlst_orig);
   dest->CalcBoundingBox();
   GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
   GLOB->CMDH->DoCommand("draw");
   _busy=false;
}

void RotateTool::Draw()
{
//	SetStatusBar();
	if (_busy)
   {
         wxClientDC temp(GLOB->CANVAS);
         GLOB->SCRDRV->SetDC(&temp);
         temp.SetLogicalFunction(wxINVERT);
         GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
         GLOB->SCRDRV->GDSDrawLine(_rotX,_rotY,_newX,_newY);
         GLOB->SCRDRV->WriteDirect(_elemlst);
         GLOB->SCRDRV->SetDrawStyle(STORED);
   	   GLOB->SCRDRV->SetDC(0);
   }
}

void RotateTool::EvDrag()
{
//   GLOB->SCRDRV->CaptureMouse();
   Grid();
	CalculateCurrent();
	if (GLOB->CMDH->GetViewCoord())
   {
		_statusbar->SetXY(_realcurrentX,_realcurrentY);
	}
//   	WarpPointer(200, 200);
   if (_busy)
   {
         //draw the move line at the old position just before the move
         //move and draw local ELEMENT LIST
         Draw();
         _trans_matrix->Identity();

//         _trans_matrix->Mirror(0,0);
//         _trans_matrix->Scale(1.0,1.0, 0, 0);
         double rang=0;
         double rang2=0;
         if ((_newY-_rotY)!=0 &&(_newX-_rotY)!=0 )
            rang=atan2(_newY-_rotY,_newX-_rotX);
         rang = rang/ M_PI * 180;
         _trans_matrix->Rotate(-rang,_rotX,_rotY);
         _newX=_realcurrentX;
         _newY=_realcurrentY;
         if ((_newY-_rotY)!=0 &&(_newX-_rotY)!=0)
            rang2=atan2(_newY-_rotY,_newX-_rotX);
         rang2 = rang2/ M_PI * 180;
         _trans_matrix->Rotate(rang2,_rotX,_rotY);
         _elemlst->Transform(_trans_matrix,ELE_SELECTED);
         _elemlst->CalcBoundingBox();
         Draw();
   }
}

void RotateTool::EvMove(int WXUNUSED(drag))
{
   Grid();
	CalculateCurrent();
	if ( GLOB->CMDH->GetViewCoord())
   {
		_statusbar->SetXY(_realcurrentX,_realcurrentY);
	}
}

void RotateTool::EvLeftDClick()
{
   Grid();
	CalculateCurrent();
   if (_busy)
   {
      Draw();
      _eventstamp=++GLOB->EVENT_COUNTER;
      _elemlst->SetTimeStamp(_eventstamp);
      ElementList* dest=GLOB->CMDH->GetTopElementList();
		dest->takeover(_elemlst);
      _elemlst->CalcBoundingBox();
      dest->CalcBoundingBox();
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();

      Command* _the_command = new Command();
      _the_command->Set_Keyword("tool");
      _the_command->Set_Command("tool rotate ");
      _the_command->Set_EventStamp(_eventstamp);
      _the_command->Set_ElementList(_elemlst_orig);
	   _busy=false;

      GLOB->CMDH->ToHistory(_the_command);
      GLOB->SCRDRV->SetClippingRegionBox(_elemlst_orig->GetBoundingBox());
      GLOB->CMDH->DoCommand("draw");
      GLOB->SCRDRV->SetClippingRegionBox(_elemlst->GetBoundingBox());
      GLOB->CMDH->DoCommand("draw");
      GLOB->SCRDRV->DestroyClippingRegion();
      GLOB->MAINWINDOW->set_Cursor("ready");

      CalculateCurrent();
      GLOB->MAINWINDOW->SetPoint(_realcurrentX,_realcurrentY);
	   _controller->EndTool();
   }
}

void RotateTool::EvLD()
{
   Grid();
   CalculateCurrent();
   //was the cursor already in a selected item
   if (_busy)
   	  EvDrag();
   else
   {
	   CalcVirtual(_currentX,_currentY,_rotX,_rotY);
      Point a_point(_realcurrentX,_realcurrentY);
      if (!GLOB->CMDH->GetTopElementList()->SelectedAtPoint(&a_point))
      {
         //put the COPY tool on the stack and inistiate the SELECT tool
         _controller->StartTool(SELECT_TOOL,true);
         //transfer the EvLD event to the MOVE tool
         _controller->GetActiveTool()->OnMouseEvent(_event);
         return;
      }

      //get the selected elements from the showstructure and move them to the local
      //element list
      bool found=false;

      TDLI<Element> I2=TDLI<Element>(GLOB->CMDH->GetTopElementList());
      I2.tohead();
      while (!I2.hitroot())
      {
         if (I2.item()->GetSelect())
         {
            Element* a_ele=I2.item();
            I2.remove();
            _elemlst->insbegin(a_ele);
            found=true;
         }
         else
            I2++;
      }
      if (found)
      {
		   _elemlst_orig = new 		ElementList();
         _elemlst->CalcBoundingBox();
			*_elemlst_orig=*_elemlst;
         _elemlst_orig->CalcBoundingBox();
			_newX=_realcurrentX;
			_newY=_realcurrentY;
		   _busy=true;
      }
   }
}

void RotateTool::EvLU()
{
   Grid();
	CalculateCurrent();
   if (_rotX!=_realcurrentX && _rotY!=_realcurrentY) //need to stop?
   {
//	  GLOB->SCRDRV->ReleaseMouse();
   }
}

EditCircleTool::EditCircleTool(ToolControl* controller):BaseTool(controller)
{
   _toolcursor=wxCURSOR_HAND;
   _circle = 0;
	_tooltype = EDITCIRCLE_TOOL;
	m_toolbar = TOOLBAR_DRAW;
   _moving=false;
   _circle_orig= 0;
   m_circle_dlg = new CircleditDialog(this,GLOB->MAINWINDOW);
   m_circle_dlg->SetUnit("um");
   m_circle_dlg->SetupWindow();
   m_circle_dlg->Show(true);
}

EditCircleTool::~EditCircleTool()
{
   if (_circle)
   {
      GLOB->MAINWINDOW->set_Cursor("wait");

      //first remove last drawn circle
      Draw();
      GLOB->CMDH->GetTopElementList()->insend(_circle_orig);
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
      Command* _the_command = new Command();
      _the_command->Set_Keyword("end");
      _the_command->Set_Command("end");
      _the_command->Set_Tool(_tooltype);
      GLOB->CMDH->ToHistory(_the_command);
      GLOB->CMDH->DoCommand("draw");
   	GLOB->MAINWINDOW->set_Cursor("ready");
      _circle=0;
	}
   delete _circle;
   delete m_circle_dlg;
}

wxString  EditCircleTool::GetToolInfo()
{
   return "edit circle";
}

wxUnit  EditCircleTool::GetRadius()
{
   if (_circle)
   {
      wxUnit a(_circle->GetRadius(),1e-6);
      return a;
   }
   else
   {
      wxUnit a(0.0,1e-6);
      return a;
   }
}

void EditCircleTool::SetRadius(wxUnit& a)
{
   Draw();
   _circle->SetRadius(a.GetValue()/GLOB->PHYSUNITS());
   Draw();
}

bool EditCircleTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
         return true;
      default:
         return false;
   }
}

void EditCircleTool::Draw()
{
//	SetStatusBar();
   if (_circle && _circle->GetRadius() > 0)
   {
      wxClientDC temp(GLOB->CANVAS);
      GLOB->SCRDRV->SetDC(&temp);
      temp.SetLogicalFunction(wxINVERT);
	   GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
      _circle->CalcBoundingBox();
      GLOB->SCRDRV->WriteDirect(_circle);
      double x=_circle->GetMid().GetX()+_circle->GetRadius();
      double y=_circle->GetMid().GetY();
 		DrawHandle(temp,x,y,1);
	   GLOB->SCRDRV->SetDrawStyle(STORED);
	   GLOB->SCRDRV->SetDC(0);
   }
}

void EditCircleTool::EvDrag()
{
   if (_moving)
   {
      Grid();
      CalculateCurrent();

      if (GLOB->CMDH->GetViewCoord())
         _statusbar->SetXY(_realcurrentX,_realcurrentY);
      if (_circle->GetRadius() > 0)
      {
         Draw();

         //calculate new circle
         double radius=sqrt(pow(_circle->GetMid().GetX()-_realcurrentX,2)+ pow(_circle->GetMid().GetY()-_realcurrentY,2));
         if (radius < 1) radius=1;
         _circle->SetRadius(radius);
         _circle->CalcBoundingBox();

         Draw();
      }
   }
}

void EditCircleTool::EvLD()
{
   if (!_busy)
   {
	   CalculateCurrent();
	   Point a_point(_realcurrentX,_realcurrentY);
      //get the first boundary from the showstructure and move them to the local
      //polygon
      bool found=false;

      TDLI<Element> I2=TDLI<Element>(GLOB->CMDH->GetTopElementList());
      I2.tohead();
      while (!I2.hitroot() && !found)
      {
         if (I2.item()->GetSelect() && I2.item()->GetType()==CIRCLE && (I2.item()->Select(&a_point)))
         {
            Element* a_ele=I2.item();
            I2.remove();
            _circle = (Circle*) a_ele;
			   _circle_orig= new Circle(0);
            *_circle_orig=*_circle;
            _circle_orig->SetSelect(true);
            _circle->SetSelect(false);
            found=true;
         }
         else
            I2++;
      }
      if (found)
      {
   	   Draw();
		   _busy=true;
      }
  }
  else
  {
      bool found=false;

      //are we at the edit point?
      double x=_circle->GetMid().GetX()+_circle->GetRadius();
      double y=_circle->GetMid().GetY();
      int xd =(int) GLOB->SCRDRV->LogicalToDeviceX(x);
      int yd =(int) GLOB->SCRDRV->LogicalToDeviceY(y);
      if (abs(_currentX-xd) < HANDLE && abs(_currentY-yd) < HANDLE)
         found=true;

      if (found) //yes then start moving the point (draging or move)
      {
		   CalculateCurrent();
         _moving=true;
      }
   }
}

void EditCircleTool::EvLU()
{
   if (_busy && _moving)
   {
      _moving=false;
//      GLOB->SCRDRV->ReleaseMouse();
   }
}


void EditCircleTool::EvLeftDClick()
{
   Grid();
	CalculateCurrent();
//   GLOB->SCRDRV->ReleaseMouse();
   if (_circle->GetRadius() > 0)
   {
      GLOB->MAINWINDOW->set_Cursor("wait");

      //first remove last drawn circle
      Draw();
      _circle->CalcBoundingBox();
      _circle->SetSelect(true);
      GLOB->CMDH->GetTopElementList()->insend(_circle);
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
      _circle->SetStamp(++GLOB->EVENT_COUNTER);

      Command* _the_command = new Command();
      _the_command->Set_Keyword("end");
      _the_command->Set_Command("end");
      _the_command->Set_Tool(_tooltype);
      _the_command->Set_EventStamp(_circle->GetStamp());
      _the_command->Set_Element(_circle_orig);

      BoundingBox tmp=_circle->GetBoundingBox();
      _circle=0;

      GLOB->CMDH->ToHistory(_the_command);
      GLOB->SCRDRV->SetClippingRegionBox(_circle_orig->GetBoundingBox());
      GLOB->CMDH->DoCommand("draw");
      GLOB->SCRDRV->SetClippingRegionBox(tmp);
      GLOB->CMDH->DoCommand("draw");
      GLOB->SCRDRV->DestroyClippingRegion();
      GLOB->MAINWINDOW->set_Cursor("ready");

      GLOB->MAINWINDOW->SetPoint(_realcurrentX,_realcurrentY);
      _controller->EndTool();
	   _busy=false;
      m_circle_dlg->Show(false);
   }
}

EditRectangleTool::EditRectangleTool(ToolControl* controller):BaseTool(controller)
{
   _toolcursor=wxCURSOR_HAND;
   _box = 0;
   _moving=false;
	_tooltype = EDITREC_TOOL;
	m_toolbar = TOOLBAR_DRAW;
	_inverse_mapping = new wxTransformMatrix();
   _box_orig =0;
}

EditRectangleTool::~EditRectangleTool()
{
   if (_box)
   {
//      GLOB->SCRDRV->ReleaseMouse();
      GLOB->MAINWINDOW->set_Cursor("wait");

      //first remove last drawn circle
      Draw();
      GLOB->CMDH->GetTopElementList()->insend(_box_orig);
      _box=0;
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
      Command* _the_command = new Command();
      _the_command->Set_Keyword("end");
      _the_command->Set_Command("end");
      _the_command->Set_Tool(_tooltype);
      GLOB->CMDH->ToHistory(_the_command);
      GLOB->CMDH->DoCommand("draw");
   	GLOB->MAINWINDOW->set_Cursor("ready");
	}
   delete _inverse_mapping;
   delete _box;
}

wxString  EditRectangleTool::GetToolInfo()
{
   return "edit rectangle";
}


bool EditRectangleTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
         return true;
      default:
         return false;
   }
}

void EditRectangleTool::Draw()
{
   if (_box)
   {
         wxClientDC temp(GLOB->CANVAS);
         GLOB->SCRDRV->SetDC(&temp);
         temp.SetLogicalFunction(wxINVERT);
         G_Polygon* orgi = new G_Polygon();
         GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
	      _box->CalcBoundingBox();
         GLOB->SCRDRV->WriteDirect(_box);

         double w=_box->GetW();
         double h=_box->GetH();
         //generate an eight point polygon
         Line_Seg* lseg=new Line_Seg(0,0);
         orgi->insend(lseg);
         lseg=new Line_Seg(w/2,0);
         orgi->insend(lseg);
         lseg=new Line_Seg(w,0);
         orgi->insend(lseg);
         lseg=new Line_Seg(w,h/2);
         orgi->insend(lseg);
         lseg=new Line_Seg(w,h);
         orgi->insend(lseg);
         lseg=new Line_Seg(w/2,h);
         orgi->insend(lseg);
         lseg=new Line_Seg(0,h);
         orgi->insend(lseg);
         lseg=new Line_Seg(0,h/2);
         orgi->insend(lseg);
         orgi->Transform(_box->GetRelativeMatrix());
         {
            TDLI<Segment>   _AI=TDLI<Segment>(orgi);
            _AI.tohead();
            while (!_AI.hitroot())
            {
               double x,y;
               x = _AI.item()->GetX();
               y = _AI.item()->GetY();
					DrawHandle(temp,x,y);
               _AI++;
            }
         }
         GLOB->SCRDRV->SetDrawStyle(STORED);
         delete orgi;
   	   GLOB->SCRDRV->SetDC(0);
   }
}

void EditRectangleTool::EvDrag()
{
//  	GLOB->SCRDRV->CaptureMouse();
   if (_moving)
   {
//      GLOB->SCRDRV->CaptureMouse();
      Grid();
      CalculateCurrent();

      if (GLOB->CMDH->GetViewCoord())
         _statusbar->SetXY(_realcurrentX,_realcurrentY);

      Draw();

      //calculate new box
      Point cur(_realcurrentX,_realcurrentY);
      *_inverse_mapping = *_box->GetRelativeMatrix();
      _inverse_mapping->Invert();
      cur.Transform(_inverse_mapping);
      wxTransformMatrix offs;
      switch (_movedpoint)
      {
        case 1:
        {
     				offs.Translate(cur.GetX(),cur.GetY());
      			*_box->GetRelativeMatrix() = *_box->GetRelativeMatrix() * offs;
        			_box->SetH(_box->GetH()-cur.GetY());
        			_box->SetW(_box->GetW()-cur.GetX());
               break;
        }
        case 2:
        {
     				offs.Translate(0,cur.GetY());
      			*_box->GetRelativeMatrix() = *_box->GetRelativeMatrix() * offs;
        			_box->SetH(_box->GetH()-cur.GetY());
               break;
        }
        case 3:
        {
     				offs.Translate(0,cur.GetY());
      			*_box->GetRelativeMatrix() = *_box->GetRelativeMatrix() * offs;
        			_box->SetH(_box->GetH()-cur.GetY());
        			_box->SetW(cur.GetX());
               break;
        }
        case 4: _box->SetW(cur.GetX());
                break;
        case 5: _box->SetW(cur.GetX());
   				 _box->SetH(cur.GetY());
	             break;
        case 6: _box->SetH(cur.GetY());
                break;
        case 7:
        			_box->SetH(cur.GetY());
     				offs.Translate(cur.GetX(),0);
      			*_box->GetRelativeMatrix() = *_box->GetRelativeMatrix() * offs;
        			_box->SetW(_box->GetW()-cur.GetX());
               break;
        case 8:
        {
     				offs.Translate(cur.GetX(),0);
      			*_box->GetRelativeMatrix() = *_box->GetRelativeMatrix() * offs;
        			_box->SetW(_box->GetW()-cur.GetX());
               break;
        }
      }
		_prevX=_realcurrentX;
		_prevY=_realcurrentY;
      Draw();
   }
}

void EditRectangleTool::EvLD()
{
   if (!_busy)
   {
	   CalculateCurrent();
	   Point a_point(_realcurrentX,_realcurrentY);
      //get the first boundary from the showstructure and move them to the local
      //polygon
      bool found=false;

      TDLI<Element> I2=TDLI<Element>(GLOB->CMDH->GetTopElementList());
      I2.tohead();
      while (!I2.hitroot() && !found)
      {
         if (I2.item()->GetSelect() && I2.item()->GetType()==BOX && (I2.item()->Select(&a_point)))
         {
            Element* a_ele=I2.item();
            I2.remove();
            _box = (Box*) a_ele;
			   _box_orig = new Box(0);
            *_box_orig=*_box;
            _box_orig->SetSelect(true);
            _box->SetFlags(ELE_GR_C);
            _box->SetSelect(false);
            found=true;
         }
         else
            I2++;
      }
      if (found)
      {
   	   Draw();
		   _busy=true;
      }
  }
  else
  {
      bool found=false;
      double x,y;
		int xd, yd;
      G_Polygon* orgi = new G_Polygon();

      double w=_box->GetW();
      double h=_box->GetH();
      //generate an eight point polygon
      Line_Seg* lseg=new Line_Seg(0,0);
      orgi->insend(lseg);
      lseg=new Line_Seg(w/2,0);
      orgi->insend(lseg);
      lseg=new Line_Seg(w,0);
      orgi->insend(lseg);
      lseg=new Line_Seg(w,h/2);
      orgi->insend(lseg);
      lseg=new Line_Seg(w,h);
      orgi->insend(lseg);
      lseg=new Line_Seg(w/2,h);
      orgi->insend(lseg);
      lseg=new Line_Seg(0,h);
      orgi->insend(lseg);
      lseg=new Line_Seg(0,h/2);
      orgi->insend(lseg);
      orgi->Transform(_box->GetRelativeMatrix());
      *_inverse_mapping = *_box->GetRelativeMatrix();
      _inverse_mapping->Invert();
      {
         _movedpoint=0;
	      //are we at a point of the box?
         TDLI<Segment>   _AI=TDLI<Segment>(orgi);
         _AI.tohead();
         while (!_AI.hitroot())
         {
            x = _AI.item()->GetX();
            y = _AI.item()->GetY();
            _movedpoint++;
            xd=(int) GLOB->SCRDRV->LogicalToDeviceX(x);
            yd=(int) GLOB->SCRDRV->LogicalToDeviceY(y);
            if (abs(_currentX-xd) < HANDLE && abs(_currentY-yd) < HANDLE)
            {
               found=true;
               break;
            }
            _AI++;
         }
      }
      delete orgi;

      if (found) //yes then start moving the point (draging or move)
      {
         _currentX=xd;
         _currentY=yd;

		   CalculateCurrent();
 			_prevX=_realcurrentX;
			_prevY=_realcurrentY;
         _moving=true;
      }
   }
}

void EditRectangleTool::EvLU()
{
   if (_busy && _moving)
   {
      _moving=false;
//      GLOB->SCRDRV->ReleaseMouse();
   }
}


void EditRectangleTool::EvLeftDClick()
{
   Grid();
	CalculateCurrent();
//   GLOB->SCRDRV->ReleaseMouse();
   GLOB->MAINWINDOW->set_Cursor("wait");

   //first remove last drawn circle
   Draw();
   _box->CalcBoundingBox();
   _box->SetSelect(true);
   GLOB->CMDH->GetTopElementList()->insend(_box);
   GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
   _box->SetStamp(++GLOB->EVENT_COUNTER);

   Command* _the_command = new Command();
   _the_command->Set_Keyword("end");
   _the_command->Set_Command("end");
   _the_command->Set_Tool(_tooltype);
   _the_command->Set_EventStamp(_box->GetStamp());
   _the_command->Set_Element(_box_orig);

   BoundingBox tmp=_box->GetBoundingBox();
   _box=0;

   GLOB->CMDH->ToHistory(_the_command);
   GLOB->SCRDRV->SetClippingRegionBox(_box_orig->GetBoundingBox());
   GLOB->CMDH->DoCommand("draw");
   GLOB->SCRDRV->SetClippingRegionBox(tmp);
   GLOB->CMDH->DoCommand("draw");
   GLOB->SCRDRV->DestroyClippingRegion();
   GLOB->MAINWINDOW->set_Cursor("ready");

   GLOB->MAINWINDOW->SetPoint(_realcurrentX,_realcurrentY);
   _controller->EndTool();
   _busy=false;
}


TextTool::TextTool(ToolControl* controller):BaseTool(controller)
{
	m_toolbar = TOOLBAR_DRAW;
	_tooltype = TEXT_TOOL;
   _height=1000;
   strcpy(_unit,"um");
}

TextTool::~TextTool()
{
}

wxString  TextTool::GetToolInfo()
{
   return "add text";
}


void TextTool::Draw()
{
}

void TextTool::EvDrag()
{
   	EvMove(1);
}

void TextTool::EvMove(int WXUNUSED(drag))
{
   Grid();
   CalculateCurrent();
   if (GLOB->CMDH->GetViewCoord())
      _statusbar->SetXY(_realcurrentX,_realcurrentY);
}

void TextTool::EvLD()
{
   _busy=true;
   Grid();
	CalculateCurrent();
   //display the text insert dialog
   //and create text at this position
   TextinsDialog textdlg(GLOB->MAINWINDOW);
   textdlg.SetUnit(_unit);
   textdlg.SetHeight(_height);
   if (textdlg.ShowModal() == wxID_OK)
   {
      Text* _text = new Text(0);

      wxTransformMatrix*_relative_matrix = new wxTransformMatrix();

      // Translate2D over XY from the structrereference
      _relative_matrix->Translate(_realcurrentX,_realcurrentY);

      _text->SetRelativeMatrix(_relative_matrix);

      _text->SetSelect(false);
      _text->SetText(textdlg.GetText());
      _text->SetHeight(textdlg.GetHeight());
   	strcpy(_unit,textdlg.GetUnit());
		_height=textdlg.GetHeight();

      _text->CalcBoundingBox();

		GLOB->CMDH->GetTopElementList()->insend(_text);
	   GLOB->SETUP->Set_Available(GLOB->CMDH->GetCurrentLayer(), true);
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();


      Command* _the_command = new Command();
      _the_command->Set_Keyword("addtext");
      _the_command->Set_Command("addtext");
      _the_command->Set_Tool(_tooltype);
      _the_command->Set_EventStamp(_text->GetStamp());
	   _the_command->Set_Element(_text);

      GLOB->CMDH->ToHistory(_the_command);
      GLOB->CMDH->DoCommand("draw");
	  delete _relative_matrix;
	 }
   _busy=false;
}

void TextTool::EvLU()
{
}


void TextTool::End()
{
}


InsertReferenceTool::InsertReferenceTool(ToolControl* controller):BaseTool(controller)
{
	_tooltype = INSERTREF_TOOL;
}

InsertReferenceTool::~InsertReferenceTool()
{
}

wxString  InsertReferenceTool::GetToolInfo()
{
   return "add reference to structure";
}

void InsertReferenceTool::Draw()
{
}

void InsertReferenceTool::EvDrag()
{
   	EvMove(1);
}

void InsertReferenceTool::EvMove(int WXUNUSED(drag))
{
   Grid();
   CalculateCurrent();
   if (GLOB->CMDH->GetViewCoord())
      _statusbar->SetXY(_realcurrentX,_realcurrentY);
}

void InsertReferenceTool::EvLD()
{
   _busy=true;
   Grid();
	CalculateCurrent();
   //display the  insert dialog
   //and create reference at this position
	if (!GLOB->CMDH->Get_GDSII()) return;
   long style = (wxRESIZE_BORDER |wxTHICK_FRAME | wxSYSTEM_MENU | wxCAPTION);
	StructuresDialog2 _structDialog(GLOB->MAINWINDOW,style);
	if (_structDialog.ShowModal() == wxID_OK)
   {
      GLOB->CMDH->DoCommand(
      "keystring {SREF;  SNAME %s; STRANS 0,0,0;MAG 1; ANGLE 0; XY 1;  X %f; Y %f;ENDEL; }",
			      _structDialog.GetStructure()->GetStructureName().c_str(),_realcurrentX,_realcurrentY);
/*
	   StructureReference* _sref = new StructureReference();
      _sref->SetStructureReferenceName(_structDialog.GetStructure()->GetStructureName());
      _sref->SetStructure(_structDialog.GetStructure());

      Matrix<double> *_relative_matrix = new Matrix<double>(3,3);
      _relative_matrix->make_identity();

      // Translate2D over XY from the structrereference
      _relative_matrix->Translate2D(_realcurrentX,_realcurrentY);

      _sref->SetRelativeMatrix(_relative_matrix);

      _sref->SetSelect(false);

      _sref->CalcBoundingBox();
		GLOB->CMDH->GetTopElementList()->insend(_sref);
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
*/
		Reference* _sref =(Reference*)GLOB->CMDH->GetTopElementList()->tailitem();

      Command* _the_command = new Command();
      _the_command->Set_Keyword("addreference");
      _the_command->Set_Command("addreference");
      _the_command->Set_Tool(_tooltype);
      _the_command->Set_EventStamp(_sref->GetStamp());

      GLOB->CMDH->ToHistory(_the_command);
      GLOB->CMDH->DoCommand("draw");
	 }
   GLOB->CANVAS->SetFocus();
   _busy=false;
}

void InsertReferenceTool::EvLU()
{
}


void InsertReferenceTool::End()
{
}

InsertStructureTool::InsertStructureTool(ToolControl* controller):BaseTool(controller)
{
	_tooltype = INSERTSTRUC_TOOL;
}

InsertStructureTool::~InsertStructureTool()
{
}

wxString  InsertStructureTool::GetToolInfo()
{
   return "add structure";
}

void InsertStructureTool::Draw()
{
}

void InsertStructureTool::EvDrag()
{
   	EvMove(1);
}

void InsertStructureTool::EvMove(int WXUNUSED(drag))
{
   Grid();
   CalculateCurrent();
   if (GLOB->CMDH->GetViewCoord())
      _statusbar->SetXY(_realcurrentX,_realcurrentY);
}

void InsertStructureTool::EvLD()
{
   _busy=true;
   Grid();
	CalculateCurrent();
   //display the  insert dialog
   //and create reference at this position
	if (!GLOB->CMDH->Get_GDSII()) return;
	wxString text = wxGetTextFromUser("enter structure name","new structure");
	if (text && !text.IsEmpty())
   {
	   Structure* l_structure = new Structure(text.c_str());
	   StructureReference* _sref = new StructureReference();
      _sref->SetStructureReferenceName(l_structure->GetStructureName());
      _sref->SetStructure(l_structure);

      wxTransformMatrix*_relative_matrix = new wxTransformMatrix();

      // Translate2D over XY from the structrereference
      _relative_matrix->Translate(_realcurrentX,_realcurrentY);

      _sref->SetRelativeMatrix(_relative_matrix);

      _sref->SetSelect(false);

      _sref->CalcBoundingBox();
		GLOB->CMDH->GetTopElementList()->insend(_sref);

      GLOB->CMDH->Get_GDSII()->insend(l_structure);


      Command* _the_command = new Command();
      _the_command->Set_Keyword("addreference");
      _the_command->Set_Command("addreference");
      _the_command->Set_Tool(_tooltype);
      _the_command->Set_EventStamp(_sref->GetStamp());

      GLOB->CMDH->ToHistory(_the_command);

      _the_command = new Command();
      _the_command->Set_Keyword("addstructure");
      _the_command->Set_Command("addstructure");
      _the_command->Set_Tool(_tooltype);
      _the_command->Set_EventStamp(l_structure->GetStamp());

      GLOB->CMDH->ToHistory(_the_command);


      GLOB->CMDH->DoCommand("draw");
	  delete _relative_matrix;
	 }
   GLOB->CANVAS->SetFocus();
   _busy=false;
}

void InsertStructureTool::EvLU()
{
}


void InsertStructureTool::End()
{
}

EditImageTool::EditImageTool(ToolControl* controller):BaseTool(controller)
{
   _toolcursor=wxCURSOR_HAND;
   _image = 0;
   _moving=false;
	_tooltype = EDITIMAGE_TOOL;
	m_toolbar = TOOLBAR_DRAW;
	_inverse_mapping = new wxTransformMatrix();
   _image_orig =0;
}

EditImageTool::~EditImageTool()
{
   if (_image)
   {
//      GLOB->SCRDRV->ReleaseMouse();
      GLOB->MAINWINDOW->set_Cursor("wait");

      //first remove last drawn circle
      Draw();
      GLOB->CMDH->GetTopElementList()->insend(_image_orig);
      _image=0;
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
      Command* _the_command = new Command();
      _the_command->Set_Keyword("end");
      _the_command->Set_Command("end");
      _the_command->Set_Tool(_tooltype);
      GLOB->CMDH->ToHistory(_the_command);
      GLOB->CMDH->DoCommand("draw");
   	GLOB->MAINWINDOW->set_Cursor("ready");
	}
   delete _inverse_mapping;
   delete _image;
}

wxString  EditImageTool::GetToolInfo()
{
   return "edit image";
}


bool EditImageTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
         return true;
      default:
         return false;
   }
}

void EditImageTool::Draw()
{
   if (_image)
   {
         wxClientDC temp(GLOB->CANVAS);
         GLOB->SCRDRV->SetDC(&temp);
         temp.SetLogicalFunction(wxINVERT);
         G_Polygon* orgi = new G_Polygon();
         GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
	      _image->CalcBoundingBox();

         double w=_image->GetW();
         double h=_image->GetH();
         //generate an eight point polygon
         Line_Seg* lseg=new Line_Seg(0,0);
         orgi->insend(lseg);
         lseg=new Line_Seg(w/2,0);
         orgi->insend(lseg);
         lseg=new Line_Seg(w,0);
         orgi->insend(lseg);
         lseg=new Line_Seg(w,h/2);
         orgi->insend(lseg);
         lseg=new Line_Seg(w,h);
         orgi->insend(lseg);
         lseg=new Line_Seg(w/2,h);
         orgi->insend(lseg);
         lseg=new Line_Seg(0,h);
         orgi->insend(lseg);
         lseg=new Line_Seg(0,h/2);
         orgi->insend(lseg);
         orgi->Transform(_image->GetRelativeMatrix());
         GLOB->SCRDRV->WriteDirect(orgi);
         {
            TDLI<Segment>   _AI=TDLI<Segment>(orgi);
            _AI.tohead();
            while (!_AI.hitroot())
            {
               double x,y;
               x = _AI.item()->GetX();
               y = _AI.item()->GetY();
					DrawHandle(temp,x,y);
               _AI++;
            }
         }
         GLOB->SCRDRV->SetDrawStyle(STORED);
         delete orgi;
   	   GLOB->SCRDRV->SetDC(0);
   }
}

void EditImageTool::EvDrag()
{
//  	GLOB->SCRDRV->CaptureMouse();
   if (_moving)
   {
//      GLOB->SCRDRV->CaptureMouse();
      Grid();
      CalculateCurrent();

      if (GLOB->CMDH->GetViewCoord())
         _statusbar->SetXY(_realcurrentX,_realcurrentY);

      Draw();

      //calculate new box
      Point cur(_realcurrentX,_realcurrentY);
      *_inverse_mapping = *_image->GetRelativeMatrix();
      _inverse_mapping->Invert();
      cur.Transform(_inverse_mapping);
      wxTransformMatrix offs;
      switch (_movedpoint)
      {
        case 1:
        {
     				offs.Translate(cur.GetX(),cur.GetY());
      			*_image->GetRelativeMatrix() = *_image->GetRelativeMatrix() * offs;
        			_image->SetH(_image->GetH()-cur.GetY());
        			_image->SetW(_image->GetW()-cur.GetX());
               break;
        }
        case 2:
        {
     				offs.Translate(0,cur.GetY());
      			*_image->GetRelativeMatrix() = *_image->GetRelativeMatrix() * offs;
        			_image->SetH(_image->GetH()-cur.GetY());
               break;
        }
        case 3:
        {
     				offs.Translate(0,cur.GetY());
      			*_image->GetRelativeMatrix() = *_image->GetRelativeMatrix() * offs;
        			_image->SetH(_image->GetH()-cur.GetY());
        			_image->SetW(cur.GetX());
               break;
        }
        case 4: _image->SetW(cur.GetX());
                break;
        case 5: _image->SetW(cur.GetX());
   				 _image->SetH(cur.GetY());
	             break;
        case 6: _image->SetH(cur.GetY());
                break;
        case 7:
        			_image->SetH(cur.GetY());
     				offs.Translate(cur.GetX(),0);
      			*_image->GetRelativeMatrix() = *_image->GetRelativeMatrix() * offs;
        			_image->SetW(_image->GetW()-cur.GetX());
               break;
        case 8:
        {
     				offs.Translate(cur.GetX(),0);
      			*_image->GetRelativeMatrix() = *_image->GetRelativeMatrix() * offs;
        			_image->SetW(_image->GetW()-cur.GetX());
               break;
        }
      }
		_prevX=_realcurrentX;
		_prevY=_realcurrentY;
      Draw();
   }
}

void EditImageTool::EvLD()
{
   if (!_busy)
   {
	   CalculateCurrent();
	   Point a_point(_realcurrentX,_realcurrentY);
      //get the first boundary from the showstructure and move them to the local
      //polygon
      bool found=false;

      TDLI<Element> I2=TDLI<Element>(GLOB->CMDH->GetTopElementList());
      I2.tohead();
      while (!I2.hitroot() && !found)
      {
         if (I2.item()->GetSelect() && I2.item()->GetType()==IMAGE && (I2.item()->Select(&a_point)))
         {
            Element* a_ele=I2.item();
            I2.remove();
            _image = (Image*) a_ele;
			   _image_orig = new Image();
            *_image_orig=*_image;
            _image_orig->SetSelect(true);
            _image->SetFlags(ELE_GR_C);
            _image->SetSelect(false);
            found=true;
         }
         else
            I2++;
      }
      if (found)
      {
   	   Draw();
		   _busy=true;
      }
  }
  else
  {
      bool found=false;
      double x,y;
		int xd, yd;
      G_Polygon* orgi = new G_Polygon();

      double w=_image->GetW();
      double h=_image->GetH();
      //generate an eight point polygon
      Line_Seg* lseg=new Line_Seg(0,0);
      orgi->insend(lseg);
      lseg=new Line_Seg(w/2,0);
      orgi->insend(lseg);
      lseg=new Line_Seg(w,0);
      orgi->insend(lseg);
      lseg=new Line_Seg(w,h/2);
      orgi->insend(lseg);
      lseg=new Line_Seg(w,h);
      orgi->insend(lseg);
      lseg=new Line_Seg(w/2,h);
      orgi->insend(lseg);
      lseg=new Line_Seg(0,h);
      orgi->insend(lseg);
      lseg=new Line_Seg(0,h/2);
      orgi->insend(lseg);
      orgi->Transform(_image->GetRelativeMatrix());
      *_inverse_mapping = *_image->GetRelativeMatrix();
      _inverse_mapping->Invert();
      {
         _movedpoint=0;
	      //are we at a point of the box?
         TDLI<Segment>   _AI=TDLI<Segment>(orgi);
         _AI.tohead();
         while (!_AI.hitroot())
         {
            x = _AI.item()->GetX();
            y = _AI.item()->GetY();
            _movedpoint++;
            xd=(int) GLOB->SCRDRV->LogicalToDeviceX(x);
            yd=(int) GLOB->SCRDRV->LogicalToDeviceY(y);
            if (abs(_currentX-xd) < HANDLE && abs(_currentY-yd) < HANDLE)
            {
               found=true;
               break;
            }
            _AI++;
         }
      }
      delete orgi;

      if (found) //yes then start moving the point (draging or move)
      {
         _currentX=xd;
         _currentY=yd;

		   CalculateCurrent();
 			_prevX=_realcurrentX;
			_prevY=_realcurrentY;
         _moving=true;
      }
   }
}

void EditImageTool::EvLU()
{
   if (_busy && _moving)
   {
      _moving=false;
//      GLOB->SCRDRV->ReleaseMouse();
   }
}


void EditImageTool::EvLeftDClick()
{
   Grid();
	CalculateCurrent();
//   GLOB->SCRDRV->ReleaseMouse();
   GLOB->MAINWINDOW->set_Cursor("wait");

   //first remove last drawn circle
   Draw();
   _image->CalcBoundingBox();
   _image->SetSelect(true);
   GLOB->CMDH->GetTopElementList()->insend(_image);
   GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
   _image->SetStamp(++GLOB->EVENT_COUNTER);

   Command* _the_command = new Command();
   _the_command->Set_Keyword("end");
   _the_command->Set_Command("end");
   _the_command->Set_Tool(_tooltype);
   _the_command->Set_EventStamp(_image->GetStamp());
   _the_command->Set_Element(_image_orig);

   BoundingBox tmp=_image->GetBoundingBox();
   _image=0;

   GLOB->CMDH->ToHistory(_the_command);
   GLOB->SCRDRV->SetClippingRegionBox(_image_orig->GetBoundingBox());
   GLOB->CMDH->DoCommand("draw");
   GLOB->SCRDRV->SetClippingRegionBox(tmp);
   GLOB->CMDH->DoCommand("draw");
   GLOB->SCRDRV->DestroyClippingRegion();
   GLOB->MAINWINDOW->set_Cursor("ready");

   GLOB->MAINWINDOW->SetPoint(_realcurrentX,_realcurrentY);
   _controller->EndTool();
   _busy=false;
}


EditReferenceTool::EditReferenceTool(ToolControl* controller):BaseTool(controller)
{
   _toolcursor=wxCURSOR_HAND;
   _sref = 0;
   _moving=false;
	_tooltype = EDITREF_TOOL;
	m_toolbar = TOOLBAR_DRAW;
	_inverse_mapping = new wxTransformMatrix();
   _sref_orig =0;
}

EditReferenceTool::~EditReferenceTool()
{
   if (_sref)
   {
//      GLOB->SCRDRV->ReleaseMouse();
      GLOB->MAINWINDOW->set_Cursor("wait");

      //first remove last drawn circle
      Draw();
      GLOB->CMDH->GetTopElementList()->insend(_sref_orig);
      _sref=0;
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
      Command* _the_command = new Command();
      _the_command->Set_Keyword("end");
      _the_command->Set_Command("end");
      _the_command->Set_Tool(_tooltype);
      GLOB->CMDH->ToHistory(_the_command);
      GLOB->CMDH->DoCommand("draw");
   	GLOB->MAINWINDOW->set_Cursor("ready");
	}
   delete _inverse_mapping;
   delete _sref;
}

wxString  EditReferenceTool::GetToolInfo()
{
   return "edit reference";
}


bool EditReferenceTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
         return true;
      default:
         return false;
   }
}

void EditReferenceTool::Draw()
{
   if (_sref)
   {
         wxClientDC temp(GLOB->CANVAS);
         GLOB->SCRDRV->SetDC(&temp);
         temp.SetLogicalFunction(wxINVERT);
         G_Polygon* orgi = new G_Polygon();
         GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
	      _sref->CalcBoundingBox();

         double w=_sref->GetStructure()->GetBoundingBox().GetWidth();
         double h=_sref->GetStructure()->GetBoundingBox().GetHeight();
         double xmin=_sref->GetStructure()->GetBoundingBox().GetMin().GetX();
         double ymin=_sref->GetStructure()->GetBoundingBox().GetMin().GetY();
         //generate an eight point polygon
         Line_Seg* lseg=new Line_Seg(xmin,ymin);
         orgi->insend(lseg);
         lseg=new Line_Seg(xmin+w/2,ymin);
         orgi->insend(lseg);
         lseg=new Line_Seg(xmin+w,ymin);
         orgi->insend(lseg);
         lseg=new Line_Seg(xmin+w,ymin+h/2);
         orgi->insend(lseg);
         lseg=new Line_Seg(xmin+w,ymin+h);
         orgi->insend(lseg);
         lseg=new Line_Seg(xmin+w/2,ymin+h);
         orgi->insend(lseg);
         lseg=new Line_Seg(xmin,ymin+h);
         orgi->insend(lseg);
         lseg=new Line_Seg(xmin,h/2+ymin);
         orgi->insend(lseg);
         orgi->Transform(_sref->GetRelativeMatrix());
         GLOB->SCRDRV->WriteDirect(orgi);
         {
            TDLI<Segment>   _AI=TDLI<Segment>(orgi);
            _AI.tohead();
            while (!_AI.hitroot())
            {
               double x,y;
               x = _AI.item()->GetX();
               y = _AI.item()->GetY();
					DrawHandle(temp,x,y);
               _AI++;
            }
         }
         GLOB->SCRDRV->SetDrawStyle(STORED);
         delete orgi;
   	   GLOB->SCRDRV->SetDC(0);
   }
}

void EditReferenceTool::EvDrag()
{
//  	GLOB->SCRDRV->CaptureMouse();
   if (_moving)
   {
//      GLOB->SCRDRV->CaptureMouse();
      Grid();
      CalculateCurrent();

      if (GLOB->CMDH->GetViewCoord())
         _statusbar->SetXY(_realcurrentX,_realcurrentY);

      Draw();

      //calculate new box
      Point cur(_realcurrentX,_realcurrentY);
      *_inverse_mapping = *_sref->GetRelativeMatrix();
      _inverse_mapping->Invert();
      cur.Transform(_inverse_mapping);
      wxTransformMatrix offs;
      wxTransformMatrix scale;
      double w=_sref->GetRelativeMatrix()->Get_scaleX()*_sref->GetStructure()->GetBoundingBox().GetWidth();
      double h=_sref->GetRelativeMatrix()->Get_scaleY()*_sref->GetStructure()->GetBoundingBox().GetHeight();         double xmin=_sref->GetStructure()->GetBoundingBox().GetMin().GetX();
      double ymin=_sref->GetStructure()->GetBoundingBox().GetMin().GetY();
      cur.SetY((cur.GetY()-ymin)*_sref->GetRelativeMatrix()->Get_scaleY());
      cur.SetX((cur.GetX()-xmin)*_sref->GetRelativeMatrix()->Get_scaleX());
      switch (_movedpoint)
      {
        case 1:
        {
//       			scale.Scale((w-cur.GetX())/w,(h-cur.GetY())/h,0,0);
//     				offs.Translate(cur.GetX(),cur.GetY());
       			scale.Scale(w-cur.GetX(),h-cur.GetY(),0,0);
       			scale.Scale(1/w,1/h,0,0);
     				offs.Translate(cur.GetX(),cur.GetY());
               break;
        }
        case 2:
        {
        			scale.Scale(1,(h-cur.GetY())/h,0,0);
     				offs.Translate(0,cur.GetY());
               break;
        }
        case 3:
        {
        			scale.Scale(cur.GetX(),(h-cur.GetY()),0,0);
       			scale.Scale(1/w,1/h,0,0);
     				offs.Translate(0,cur.GetY());
               break;
        }
        case 4:
        			scale.Scale(cur.GetX()/w,1,0,0);
               break;
        case 5:
        			scale.Scale(cur.GetX()/w,cur.GetY()/h,0,0);
	             break;
        case 6:
        			scale.Scale(1,cur.GetY()/h,0,0);
                break;
        case 7:
        			scale.Scale((w-cur.GetX())/w,cur.GetY()/h,0,0);
     				offs.Translate(cur.GetX(),0);
               break;
        case 8:
        {
        			scale.Scale((w-cur.GetX())/w,1,0,0);
     				offs.Translate(cur.GetX(),0);
               break;
        }
      }
		*_sref->GetRelativeMatrix() = *_sref->GetRelativeMatrix() * scale ;
//		*_sref->GetRelativeMatrix() = scale * *_sref->GetRelativeMatrix();
		*_sref->GetRelativeMatrix() = offs * *_sref->GetRelativeMatrix() ;
		_prevX=_realcurrentX;
		_prevY=_realcurrentY;
      Draw();
   }
}

void EditReferenceTool::EvLD()
{
   if (!_busy)
   {
	   CalculateCurrent();
	   Point a_point(_realcurrentX,_realcurrentY);
      //get the first boundary from the showstructure and move them to the local
      //polygon
      bool found=false;

      TDLI<Element> I2=TDLI<Element>(GLOB->CMDH->GetTopElementList());
      I2.tohead();
      while (!I2.hitroot() && !found)
      {
         if (I2.item()->GetSelect() && I2.item()->GetType()==SREF && (I2.item()->Select(&a_point)))
         {
            Element* a_ele=I2.item();
            I2.remove();
            _sref = (StructureReference*) a_ele;
			   _sref_orig = new StructureReference();
            *_sref_orig=*_sref;
            _sref_orig->SetSelect(true);
            _sref->SetFlags(ELE_GR_C);
            _sref->SetSelect(false);
            found=true;
         }
         else
            I2++;
      }
      if (found)
      {
   	   Draw();
		   _busy=true;
      }
  }
  else
  {
      bool found=false;
      double x,y;
		int xd,yd;
      G_Polygon* orgi = new G_Polygon();

      double w=_sref->GetStructure()->GetBoundingBox().GetWidth();
      double h=_sref->GetStructure()->GetBoundingBox().GetHeight();
      double xmin=_sref->GetStructure()->GetBoundingBox().GetMin().GetX();
      double ymin=_sref->GetStructure()->GetBoundingBox().GetMin().GetY();
      //generate an eight point polygon
      Line_Seg* lseg=new Line_Seg(xmin,ymin);
      orgi->insend(lseg);
      lseg=new Line_Seg(xmin+w/2,ymin);
      orgi->insend(lseg);
      lseg=new Line_Seg(xmin+w,ymin);
      orgi->insend(lseg);
      lseg=new Line_Seg(xmin+w,ymin+h/2);
      orgi->insend(lseg);
      lseg=new Line_Seg(xmin+w,ymin+h);
      orgi->insend(lseg);
      lseg=new Line_Seg(xmin+w/2,ymin+h);
      orgi->insend(lseg);
      lseg=new Line_Seg(xmin,ymin+h);
      orgi->insend(lseg);
      lseg=new Line_Seg(xmin,h/2+ymin);
      orgi->insend(lseg);
      orgi->Transform(_sref->GetRelativeMatrix());
      *_inverse_mapping = *_sref->GetRelativeMatrix();
      _inverse_mapping->Invert();
      {
         _movedpoint=0;
	      //are we at a point of the box?
         TDLI<Segment>   _AI=TDLI<Segment>(orgi);
         _AI.tohead();
         while (!_AI.hitroot())
         {
            x = _AI.item()->GetX();
            y = _AI.item()->GetY();
            _movedpoint++;
            xd=(int) GLOB->SCRDRV->LogicalToDeviceX(x);
            yd=(int) GLOB->SCRDRV->LogicalToDeviceY(y);
            if (abs(_currentX-xd) < HANDLE && abs(_currentY-yd) < HANDLE)
            {
               found=true;
               break;
            }
            _AI++;
         }
      }
      delete orgi;

      if (found) //yes then start moving the point (draging or move)
      {
         _currentX=xd;
         _currentY=yd;

		   CalculateCurrent();
 			_prevX=_realcurrentX;
			_prevY=_realcurrentY;
         _moving=true;
      }
   }
}

void EditReferenceTool::EvLU()
{
   if (_busy && _moving)
   {
      _moving=false;
//      GLOB->SCRDRV->ReleaseMouse();
   }
}


void EditReferenceTool::EvLeftDClick()
{
   Grid();
	CalculateCurrent();
//   GLOB->SCRDRV->ReleaseMouse();
   GLOB->MAINWINDOW->set_Cursor("wait");

   //first remove last drawn reference
   Draw();
   _sref->CalcBoundingBox();
   _sref->SetSelect(true);
   GLOB->CMDH->GetTopElementList()->insend(_sref);
   GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
   _sref->SetStamp(++GLOB->EVENT_COUNTER);

   Command* _the_command = new Command();
   _the_command->Set_Keyword("end");
   _the_command->Set_Command("end");
   _the_command->Set_Tool(_tooltype);
   _the_command->Set_EventStamp(_sref->GetStamp());
   _the_command->Set_Element(_sref_orig);

   BoundingBox tmp=_sref->GetBoundingBox();
   _sref=0;

   GLOB->CMDH->ToHistory(_the_command);
   GLOB->SCRDRV->SetClippingRegionBox(_sref_orig->GetBoundingBox());
   GLOB->CMDH->DoCommand("draw");
   GLOB->SCRDRV->SetClippingRegionBox(tmp);
   GLOB->CMDH->DoCommand("draw");
   GLOB->SCRDRV->DestroyClippingRegion();
   GLOB->MAINWINDOW->set_Cursor("ready");

   GLOB->MAINWINDOW->SetPoint(_realcurrentX,_realcurrentY);
   _controller->EndTool();
   _busy=false;
}

EditLineTool::EditLineTool(ToolControl* controller):BaseTool(controller)
{
   _toolcursor=wxCURSOR_HAND;
   _moving=false;
	_tooltype = EDITLINE_TOOL;
	m_toolbar = TOOLBAR_DRAW;
   _line_orig = 0;
   _line = 0;
	SetStatusBar();
}

EditLineTool::~EditLineTool()
{
   if (_line)
   {
      GLOB->MAINWINDOW->set_Cursor("wait");

		GLOB->CMDH->GetTopElementList()->insend(_line_orig);
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
      _line=0;

      Command* _the_command = new Command();
      _the_command->Set_Keyword("end");
      _the_command->Set_Command("end");
      _the_command->Set_Tool(_tooltype);
      GLOB->CMDH->ToHistory(_the_command);
      GLOB->CMDH->DoCommand("draw");

      GLOB->MAINWINDOW->set_Cursor("ready");
   }
   delete _line;
}

wxString  EditLineTool::GetToolInfo()
{
   return "edit polygon";
}

bool EditLineTool::IsAllowedTool(ToolSet tool_to_start_next,bool WXUNUSED(onlyonce),int WXUNUSED(tools_on_stack))
{
   switch(tool_to_start_next)
   {
      case ZOOMIN_TOOL:
      case ZOOMOUT_TOOL:
      case ZOOMBACK_TOOL:
         return true;
      default:
         return false;
   }
}

void EditLineTool::Draw()
{
   if (_line)
   {
         wxClientDC temp(GLOB->CANVAS);
         GLOB->SCRDRV->SetDC(&temp);
	      GLOB->SCRDRV->DestroyClippingRegion();
         temp.SetLogicalFunction(wxINVERT);
         GLOB->SCRDRV->SetDrawStyle(INVERT_WIREFRAME);
         _line->CalcBoundingBox();
         GLOB->SCRDRV->WriteDirect(_line);
			DrawHandle(temp,_line->GetX1(),_line->GetY1());
			DrawHandle(temp,_line->GetX2(),_line->GetY2());
         GLOB->SCRDRV->SetDrawStyle(STORED);
   	   GLOB->SCRDRV->SetDC(0);
   }
}

void EditLineTool::EvDrag()
{
//  	GLOB->SCRDRV->CaptureMouse();
   if (_moving)
   {
      Grid();
      CalculateCurrent();
  	   Draw();
      if (m_one)
      {
         _line->SetX1(_realcurrentX);
         _line->SetY1(_realcurrentY);
      }
      else
      {
         _line->SetX2(_realcurrentX);
         _line->SetY2(_realcurrentY);
      }
      //remove old Xor lines
      if (GLOB->CMDH->GetViewCoord())
         _statusbar->SetXY(_realcurrentX,_realcurrentY);
  	   Draw();
	}
}

void EditLineTool::EvLeftDClick()
{
   if (_line)
   {
      Draw();
      GLOB->MAINWINDOW->set_Cursor("wait");

      _line->CalcBoundingBox();
		GLOB->CMDH->GetTopElementList()->insend(_line);
      GLOB->CMDH->Get_GDSII()->CalcBoundingBox();
      _line->SetSelect(true);
      _line->SetStamp(++GLOB->EVENT_COUNTER);

      Command* _the_command = new Command();
      _the_command->Set_Keyword("end");
      _the_command->Set_Command("end");
      _the_command->Set_Tool(_tooltype);
      _the_command->Set_EventStamp(_line->GetStamp());
      _the_command->Set_Element(_line_orig);

      BoundingBox tmp=_line->GetBoundingBox();
      tmp.EnLarge(HANDLE*GLOB->SCRDRV->GetScaleFactorX());
      _line=0;

      GLOB->CMDH->ToHistory(_the_command);
      BoundingBox tmp2=_line_orig->GetBoundingBox();
      tmp2.EnLarge(HANDLE*GLOB->SCRDRV->GetScaleFactorX());
      GLOB->SCRDRV->SetClippingRegionBox(tmp2);
      GLOB->CMDH->DoCommand("draw");
      GLOB->SCRDRV->SetClippingRegionBox(tmp);
      GLOB->CMDH->DoCommand("draw");
      GLOB->SCRDRV->DestroyClippingRegion();

      GLOB->MAINWINDOW->set_Cursor("ready");

      GLOB->MAINWINDOW->SetPoint(_realcurrentX,_realcurrentY);
	   _controller->EndTool();
	   _busy=false;
   }
}

void EditLineTool::EvLD()
{
   if (!_busy)
   {
	   CalculateCurrent();
	   Point a_point(_realcurrentX,_realcurrentY);
      //get the first boundary from the showstructure and move them to the local
      //polygon
      bool found=false;

      TDLI<Element> I2=TDLI<Element>(GLOB->CMDH->GetTopElementList());
      I2.tohead();
      while (!I2.hitroot() && !found)
      {
         if (I2.item()->GetSelect() && I2.item()->GetType()==SLINE && (I2.item()->Select(&a_point)))
         {
            Element* a_ele=I2.item();
            I2.remove();
            _line = (SLine*) a_ele;
			   _line_orig = new SLine(0);
            *_line_orig=*_line;
            _line_orig->SetSelect(true);
            _line->SetSelect(false);
            found=true;
         }
         else
            I2++;
      }
      if (found)
      {
   	   Draw();
		   _busy=true;
      }
  }
  else
  {
      bool found=false;
		int xd,yd;

      //are we at a point of the line?
      xd=(int) GLOB->SCRDRV->LogicalToDeviceX(_line->GetX1());
      yd=(int) GLOB->SCRDRV->LogicalToDeviceY(_line->GetY1());

      if (abs(_currentX-xd) < HANDLE && abs(_currentY-yd) < HANDLE)
      {
         found=true;
         m_one=true;
      }

      //are we at a point of the line?
      xd=(int) GLOB->SCRDRV->LogicalToDeviceX(_line->GetX2());
      yd=(int) GLOB->SCRDRV->LogicalToDeviceY(_line->GetY2());

      if (abs(_currentX-xd) < HANDLE && abs(_currentY-yd) < HANDLE)
      {
         found=true;
         m_one=false;
      }

      if (found) //yes then start moving the point (draging or move)
      {
         _currentX=xd;
         _currentY=yd;
		   CalculateCurrent();
         Grid();
         _moving=true;
      }
   }
}

void EditLineTool::EvLU()
{
   if (_busy && _moving)
   {
      _moving=false;
   }
}


