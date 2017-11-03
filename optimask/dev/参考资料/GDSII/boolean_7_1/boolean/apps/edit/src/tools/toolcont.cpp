#ifdef __GNUG__
#pragma implementation
#endif

#include "toolcont.h"
#include "chandbas.h"
#include "elemlst.h"
#include "command.h"
#include "execdlg.h"

BEGIN_EVENT_TABLE(ToolControl, wxEvtHandler)
	 EVT_MOUSE_EVENTS(ToolControl::OnMouseEvent)
    EVT_CHAR(ToolControl::OnChar)
    EVT_KEY_DOWN(ToolControl::OnKeyDown)
    EVT_KEY_UP(ToolControl::OnKeyUp)
END_EVENT_TABLE()

ToolControl::ToolControl()
{
	_tool_stack = new DL_List<void*>();
   //tool is 0 until all the GUI is started properly
   _tool 		= 0;
	_toolactive = false;
   GLOB->CANVAS->PushEventHandler(this);
}

ToolControl::~ToolControl()
{
	if (_tool)
		delete _tool;
   _tool=0;
   {  //delete all tools
		TDLI<BaseTool> I(_tool_stack);
		I.delete_all();
   }
   GLOB->CANVAS->PopEventHandler(FALSE);
   delete _tool_stack;
}

void ToolControl::CalculateCurrent()
{
   double xh,yh;
   xh=_currentX;
   yh=_currentY;
	GLOB->SCRDRV->GetInverseMappingMatrix().TransformPoint(xh,yh, _realcurrentX, _realcurrentY);
}

void ToolControl::OnMouseEvent(wxMouseEvent& event)
{
 	if (! GLOB->INITIALIZED) return;
 	if (! GLOB->CMDH->Get_GDSII()) return;

   if (_tool==0)
		StartTool(ZOOMIN_TOOL,false);

   int X,Y;
   event.GetPosition(&X, &Y);
   _currentX=X;_currentY=Y;
	CalculateCurrent();

   if (_toolactive==true)
   {
		_tool->OnMouseEvent(event);
   }
   if (_toolactive==false)
		StopTool();

   event.Skip();
}

void	ToolControl::OnKeyDown(wxKeyEvent& event)
{
	bool ctrl, shift;
	wxString tmp;

 	if (! GLOB->INITIALIZED) return;
 	if (! GLOB->CMDH->Get_GDSII()) return;

   if (_tool==0)
		StartTool(ZOOMIN_TOOL,false);

   //if (_tool->Get_Busy())
   //   return; //tool decides

   int X,Y;
   event.GetPosition(&X, &Y);

   _currentX=X;_currentY=Y;
	CalculateCurrent();
	ctrl=event.ControlDown();
	shift=event.ShiftDown();

	if (ctrl&&shift)
	{
	}
	else if (ctrl)
	{
		if (event.GetKeyCode()==WXK_CONTROL)
		{
			if (_tool->GetType()!= SELECT_TOOL)
			   StartTool(SELECT_TOOL,true);
		}
		else
		{
			if (_tool->GetType()== SELECT_TOOL)
				StopTool();
		}

		switch(event.GetKeyCode())
		{
		case WXK_UP:
		  {
			 GLOB->CMDH->DoCommand("scrollscreen ctrl up");
			 break;
		  }
		case WXK_DOWN:
		  {
			 GLOB->CMDH->DoCommand("scrollscreen ctrl down");
			 break;
		  }
		case WXK_RIGHT:
		  {
			 GLOB->CMDH->DoCommand("scrollscreen ctrl right");
			 break;
		  }
		case WXK_LEFT:
		  {
			 GLOB->CMDH->DoCommand("scrollscreen ctrl left");
			 break;
		  }
		case 'U':
		case 'u':
			 GLOB->CMDH->DoCommand("select all false");
			 GLOB->CMDH->DoCommand("draw");
			 break;
		case 'F':
		case 'f':
			   StartTool(ZOOMOUT_TOOL,false);
			 break;
		case 'S':
		case 's':
			   StartTool(SELECT_TOOL,false);
			 break;
		case 'D':
		case 'd':
			   StartTool(DESELECT_TOOL,false);
			 break;
		case 'Z':
		case 'z':
			   StartTool(ZOOMIN_TOOL,false);
			 break;
		case 'P':
		case 'p':
			   StartTool(POLYGON_TOOL,false);
			 break;
		case 'L':
		case 'l':
			   StartTool(POLYLINE_TOOL,false);
			 break;
		case 'R':
		case 'r':
			   StartTool(RECTANGLE_TOOL,false);
			 break;
		case 'M':
		case 'm':
			   StartTool(MOVE_TOOL,false);
			 break;
		case 'C':
		case 'c':
			   StartTool(COPY_TOOL,false);
			 break;
		case 'T':
		case 't':
			   StartTool(ROTATE_TOOL,false);
			 break;
		case 'O':
		case 'o':
			   StartTool(CIRCLE_TOOL,false);
			 break;
		case 'I':
		case 'i':
			   StartTool(CIRCLE2_TOOL,false);
			 break;
		case 'E':
		case 'e':
			   StartTool(EDIT_TOOL,false);
			 break;
		case 'K':
		case 'k':
			   StartTool(DELETE_TOOL,false);
			 break;
		case 'B':
		case 'b':
			   StartTool(TEXT_TOOL,false);
			 break;
		case 'Q':
		case 'q':
			   StartTool(EDITPROPERTY_TOOL,false);
			 break;
		case '1':
			GLOB->CMDH->DoCommand("group_a selected");
			break;
		case '2':
			GLOB->CMDH->DoCommand("group_b selected");
			break;
		}
	}
	else //Just typing without ctrl pressed
	{
		switch(event.GetKeyCode())
	   {
		case WXK_F5://popup menu
		  {
			 if (GLOB->CMDH->Get_GDSII())
				 GLOB->CANVAS->PopupMenu(GLOB->MAINWINDOW->_mousemenu,_currentX,_currentY);
			 break;
		  }
		case WXK_F6://zoomback
		  {
			   StartTool(ZOOMBACK_TOOL,true);
			 break;
		  }
		case WXK_F7://zoomout
		  {
			   StartTool(ZOOMOUT_TOOL,true);
			 break;
		  }
		case WXK_F8://setrelstart
		  {
				GLOB->CMDH->DoCommand("startpoint  %f %f",_realcurrentX,_realcurrentY);
			 wxCommandEvent a = wxCommandEvent();
			 GLOB->MAINWINDOW->SetRelStart(a);
			 break;
		  }
		case WXK_F9://calcdistance
		  {
			 GLOB->MAINWINDOW->SetPoint(_realcurrentX,_realcurrentY);
			 wxCommandEvent a = wxCommandEvent();
			 GLOB->MAINWINDOW->CalcDistance(a);
			 break;
		  }
		case WXK_UP:
		  {
			 GLOB->CMDH->DoCommand("scrollscreen up");
			 break;
		  }
		case WXK_DOWN:
		  {
			 GLOB->CMDH->DoCommand("scrollscreen down");
			 break;
		  }
		case WXK_RIGHT:
		  {
			 GLOB->CMDH->DoCommand("scrollscreen right");
			 break;
		  }
		case WXK_LEFT:
		  {
			 GLOB->CMDH->DoCommand("scrollscreen left");
			 break;
		  }
		case WXK_NEXT:
		  {
			 GLOB->CMDH->DoCommand("scrollscreen pagedown");
			 break;
		  }
		case WXK_PRIOR:
		  {
			 GLOB->CMDH->DoCommand("scrollscreen pageup");
			 break;
		  }
		case WXK_SHIFT:
			if (_tool->GetType()!= DESELECT_TOOL)
			   StartTool(DESELECT_TOOL,true);
		  break;
	//    case WXK_NUMPAD_ADD:
		 case WXK_ADD:
			if (_tool->GetType()!= ZOOMIN_TOOL)
			   StartTool(ZOOMIN_TOOL,true);
		  break;
	//    case WXK_NUMPAD_SUBTRACT:
		case WXK_SUBTRACT:
			   StartTool(ZOOMBACK_TOOL,true);
		  break;
	//    case WXK_NUMPAD_ENTER:
		case WXK_RETURN:
			   //StartTool(ZOOMOUT_TOOL,true);
			GLOB->MAINWINDOW->_execDialog->DoExecute();
		  break;
		case WXK_ESCAPE:
			   StopTool();
			 break;
		default:
			//tmp="";
			//tmp.Append(event.GetKeyCode());
			//GLOB->MAINWINDOW->_execDialog->_editField->WriteText(tmp);
			GLOB->MAINWINDOW->_execDialog->_editField->EmulateKeyPress(event);
	   }
	}
}

void	ToolControl::OnKeyUp(wxKeyEvent& event)
{
 	if (! GLOB->INITIALIZED) return;
 	if (! GLOB->CMDH->Get_GDSII()) return;

   if (_tool==0)
		StartTool(ZOOMIN_TOOL,false);

	switch(event.GetKeyCode())
   {
    case WXK_CONTROL:
		if (_tool->GetType() == SELECT_TOOL)
		   StopTool();
      break;
    case WXK_SHIFT:
		if (_tool->GetType() == DESELECT_TOOL)
		   StopTool();
      break;
    case WXK_ADD:
		if (_tool->GetType() != ZOOMIN_TOOL)
		   StopTool();
      break;
    case WXK_SUBTRACT:
      break;
   }
}

void ToolControl::OnChar(wxKeyEvent& event)
{
 	if (! GLOB->INITIALIZED) return;
 	if (! GLOB->CMDH->Get_GDSII()) return;

   if (_tool==0)
		StartTool(ZOOMIN_TOOL,false);

   int X,Y;
   event.GetPosition(&X, &Y);

   _currentX=X;_currentY=Y;
	CalculateCurrent();
/*
	switch(event.GetKeyCode())
   {
//     now in keydown 
      default:
         break;
   }
*/
   return;
}

ToolSet ToolControl::GetActiveType()
{
	return _tool->GetType();
}

BaseTool* ToolControl::GetActiveTool()
{
	return _tool;
}

int ToolControl::NumberOfTools()
{
	return _tool_stack->count();
}

bool ToolControl::StartTool(ToolSet a_tool,bool onlyonce)
{
   if (!_tool)
   {
      _tool 		= new ZoomTool(this);
      char buf[30];
      sprintf(buf,"zoom l_%d",_tool_stack->count());
      _statusbar->SetProcess(buf);
      GLOB->CMDH->ToHistory("tool zoom");
   }
   else if (_tool->IsAllowedTool(a_tool, onlyonce,NumberOfTools()))
   {
      switch(_tool->GetType())
      {
         case POLYGON_TOOL:
         case POLYLINE_TOOL:
         case CIRCLE2_TOOL:
         case CIRCLE_TOOL:
         case RECTANGLE_TOOL: //in those cases stop the tool that is on the stack
         							//because that is more convenient
            switch(a_tool)
            {
               case POLYGON_TOOL:
               case POLYLINE_TOOL:
               case CIRCLE2_TOOL:
               case CIRCLE_TOOL:
               case RECTANGLE_TOOL:
                     StopTool(false);
               default:
               		break;
            }
            break;
         default:
            break;
      }
      PushTool(); // put current tool on the stack
      switch(a_tool)
      {
         case ZOOMOUT_TOOL:
            PopTool(); //just get the tool pushed, back
            _statusbar->SetProcess("zoomout...");
            GLOB->CMDH->ToHistory("zoomout");
            GLOB->CMDH->ZoomOut();
			   GLOB->CANVAS->SetCursor(_tool->GetCursor());
            return true;
         case ZOOMBACK_TOOL:
            PopTool(); //just get the tool pushed, back
            _statusbar->SetProcess("zoomback...");
            GLOB->CMDH->ToHistory("zoomback");
            GLOB->CMDH->ZoomBack();
			   GLOB->CANVAS->SetCursor(_tool->GetCursor());
            return true;
         case ZOOMIN_TOOL:
         {
            _tool 		= new ZoomTool(this);
            _tool->SetStatusBar();
            GLOB->CMDH->ToHistory("tool zoom");
            break;
         }
         case SELECT_TOOL:
         {
            _tool	= new SelectTool(this);
            _tool->SetStatusBar();
            GLOB->CMDH->ToHistory("tool select");
            break;
         }
         case DESELECT_TOOL:
         {
            _tool 	= new DeSelectTool(this);
            _tool->SetStatusBar();
            GLOB->CMDH->ToHistory("tool deselect");
            break;
         }
         case RECTANGLE_TOOL:
         {
            _tool = new RecTangleTool(this);
            _tool->SetStatusBar();
            GLOB->CMDH->ToHistory("tool rectangle");
            break;
         }
         case POLYGON_TOOL:
         {
            _tool = new PolyTool(this);
            _tool->SetStatusBar();
            GLOB->CMDH->ToHistory("tool polygon");
            break;
         }
         case TEXT_TOOL:
         {
            _tool = new TextTool(this);
            _tool->SetStatusBar();
            GLOB->CMDH->ToHistory("tool text");
            break;
         }
         case POLYLINE_TOOL:
         {
            _tool 	= new PolyLineTool(this);
            _tool->SetStatusBar();
            GLOB->CMDH->ToHistory("tool polyline");
            break;
         }
         case MOVE_TOOL:
         {
            _tool   	= new MoveTool(this);
            _tool->SetStatusBar();
            GLOB->CMDH->ToHistory("tool move");
            break;
         }
         case ROTATE_TOOL:
         {
            _tool   	= new RotateTool(this);
            _tool->SetStatusBar();
            GLOB->CMDH->ToHistory("tool rotate");
            break;
         }
         case COPY_TOOL:
         {
            _tool     	= new CopyTool(this);
            _tool->SetStatusBar();
            GLOB->CMDH->ToHistory("tool copy");
            break;
         }
         case CIRCLE_TOOL:
         {
            _tool     	= new CircleTool(this,true);
            _tool->SetStatusBar();
            GLOB->CMDH->ToHistory("tool circle");
            break;
         }
         case CIRCLE2_TOOL:
         {
            _tool     	= new CircleTool(this,false);
            _tool->SetStatusBar();
            GLOB->CMDH->ToHistory("tool circle2");
            break;
         }
         case DELETE_TOOL:
         {
            _tool     	= new DeleteTool(this);
            _tool->SetStatusBar();
            GLOB->CMDH->ToHistory("tool delete");
            break;
         }
         case EDITPOLYGON_TOOL:
         {
            _tool = new EditPolyTool(this);
            _tool->SetStatusBar();
            break;
         }
         case EDITCIRCLE_TOOL:
         {
            _tool = new EditCircleTool(this);
            _tool->SetStatusBar();
            break;
         }
         case EDITREC_TOOL:
         {
            _tool = new EditRectangleTool(this);
            _tool->SetStatusBar();
            break;
         }
         case EDITLINE_TOOL:
         {
            _tool = new EditLineTool(this);
            _tool->SetStatusBar();
            break;
         }
         case EDITREF_TOOL:
         {
            _tool = new EditReferenceTool(this);
            _tool->SetStatusBar();
            break;
         }
         case EDITIMAGE_TOOL:
         {
            _tool = new EditImageTool(this);
            _tool->SetStatusBar();
            break;
         }
         case EDITPOLYLINE_TOOL:
         {
            _tool = new EditPolyLineTool(this);
            _tool->SetStatusBar();
            break;
         }
         case EDIT_TOOL:
         {
            _tool = new EditTool(this);
            _tool->SetStatusBar();
            GLOB->CMDH->ToHistory("tool edit");
            break;
         }
         case EDITPROPERTY_TOOL:
         {
            _tool = new EditPropertyTool(this);
            _tool->SetStatusBar();
            GLOB->CMDH->ToHistory("tool editproperty");
            break;
         }
         case INSERTREF_TOOL:
         {
            _tool = new InsertReferenceTool(this);
            _tool->SetStatusBar();
            GLOB->CMDH->ToHistory("tool insertreference");
            break;
         }
         case INSERTSTRUC_TOOL:
         {
            _tool = new InsertStructureTool(this);
            _tool->SetStatusBar();
            GLOB->CMDH->ToHistory("tool insertstructure");
            break;
         }
         default:
            PopTool(); //just get the tool pushed, back
			   GLOB->CANVAS->SetCursor(_tool->GetCursor());
            return false;
      }
   }
	else
   {
      _statusbar->SetProcess("not allowed in current tool");
      char buf[100];
      sprintf(buf,"not allowed in current tool or level \n (stop it by pressing Escape)\nlevel: %d :",NumberOfTools());
      char buf2[100];
      strcpy(buf2,"current  tool:");
      strcat(buf2,_tool->GetToolInfo().c_str());
	   _messagehandler->info(buf,buf2);
      return false;
   }
   GLOB->CANVAS->SetCursor(_tool->GetCursor());
   _statusbar->SetInsertLayer(GLOB->SETUP->Get_Name(GLOB->CMDH->GetCurrentLayer()));
   if (!onlyonce) //no need in this case can't be touched anyway
   	GLOB->MAINWINDOW->SetToolbar(_tool->GetToolBarType());
   _tool->SetOnlyOnce(onlyonce);
   _toolactive=true;
   return true;
}

bool ToolControl::EndTool(bool now)
{
   if (now)
	   _toolactive=false;
   if (_tool->GetOnlyOnce() )
	   _toolactive=false;
	return true;
}

bool ToolControl::StopTool()
{
   StopTool(true);
   return true;
}

bool ToolControl::StopTool(bool settoolinfo)
{
   if (_tool)
   {
      Command* _the_command = new Command();
      _the_command->Set_Keyword("tool");
      _the_command->Set_Command("tool stop");
      _the_command->Set_Tool(_tool->GetType());
      GLOB->CMDH->ToHistory(_the_command);

      delete _tool; //delete the current tool
      PopTool();    //and get the previous one from the stack
      if (_tool)
      {
		   GLOB->CANVAS->SetCursor(_tool->GetCursor());
			if (settoolinfo)
         {
            GLOB->MAINWINDOW->SetToolbar(_tool->GetToolBarType());
			   _tool->SetStatusBar();
         }
      }
   }
	return true;
}

bool ToolControl::PopTool()
{
	TDLIStack<BaseTool> I(_tool_stack);
   if (I.empty())
   {
      _tool=0;
		_toolactive = false;
   }
   else
   {
		_tool=I.pop();
	   _toolactive=true;
   }
   return true;
}

bool ToolControl::PushTool()
{

  if (_tool)
  {
      TDLIStack<BaseTool> I(_tool_stack);
      I.push(_tool);
  }
  return true;
}

bool ToolControl::Undo()
{
   if (_tool && _tool->Get_Busy())
   {
      switch(_tool->GetType())
      {
         case POLYGON_TOOL:
         {
            _tool->RemovePoint();
            break;
         }
         case POLYLINE_TOOL:
         {
            _tool->RemovePoint();
            break;
         }
         default:
            _tool->Undo();
            break;
      }
      return true;
   }
   else
      return false;
}

bool ToolControl::Draw()
{
  //current active tool
  if (_tool)
	  _tool->Draw();
  TDLI<BaseTool> I(_tool_stack);
  I.tohead();
  while (!I.hitroot())
  {
      I.item()->Draw();
      I++;
  }
  return true;
}

bool ToolControl::Begin()
{
   if (_tool && _tool->Get_Busy())
   {
       _tool->Begin();
       return true;
   }
   else
      return false;
}

bool ToolControl::End()
{
   if (_tool && _tool->Get_Busy())
   {
       _tool->End();
       return true;
   }
   else
      return false;
}

