//! author="klaas"
//! version="1.0"
#ifndef _TOOLCONTR
#define _TOOLCONTR

#ifdef __GNUG__
#pragma interface
#endif

#include "misc.h"
#include "tool.h"
#include "windrv.h"
#include "wxwin.h"

//!all interactive drawing using tools is handled through this class.
/*!
 All mouse and key events on the canvas/client window are redirected to this class.
 Depending on the tool that is active, those events are redirected to the right tool.
 A stack of tools is maintained in order to allow drawing and zooming at the same time.
 Shortcuts to start tools/certain commands are also handled here.
 Undo while a tool is in action (busy drawing), is handled by the tool itself.
 The toolcontroller will receive the undo command, and redirects it to the active tool.
 The default tool will always be the ZOOMIN_TOOL. If the stack becomes empty, this tool
 will be started again.
\sa BaseTool
\sa DrawDriver
\sa WindowsDriver
*/
class ToolControl :public wxEvtHandler
{
   public:
      //constructor
      ToolControl();
      //destructor
      ~ToolControl();

      //!called on mouse events
		void 	OnMouseEvent(wxMouseEvent& event);
      //!called on key events
		void 	OnChar(wxKeyEvent& event);
      //!called on keydown events
		void 	OnKeyDown(wxKeyEvent& event);
      //!called on keyup events
		void 	OnKeyUp(wxKeyEvent& event);

      //!what is the type of the active tool
      ToolSet   GetActiveType();
      //!what tool is active at the moment
      BaseTool* GetActiveTool();

      //! get the tool that was put on the stack
		bool 	 PopTool();
      //! push active tool on the stack
		bool 	 PushTool();
      //! start a new tool
      //! If only once is true, only one action will be performed
      //! after that the tool will be ended, and the last pushed tool
      //! will be restored.
		bool 	 StartTool(ToolSet a_tool,bool onlyonce);

      //! sets a flag to stop the actove tool
		bool 	 EndTool(bool now=false);

      //! end a tool and restore a tool from the stack.
		bool 	 StopTool();
      //! end a tool and restore a tool from the stack.
      bool    StopTool(bool settoolinfo);
      //! calculate the mouse position
		void 		 CalculateCurrent();
      //!how much tools are on the stack
      int       NumberOfTools();
      //!undo for active tool
      bool    Undo();
      //draw contents of tools on the stack
		bool 	 Draw();
      //! called for every action inside a tool
		bool 	 Begin();
      //! called for every action inside a tool
		bool 	 End();

   protected:
      //!active tools or not
      bool            _toolactive;
      //!stack containing all tools that were started
      DL_List<void*>* 	_tool_stack;
      //!the active tool
      BaseTool*         _tool;
      //! mouse x in windows coordinates
  		long					_currentX;
      //! mouse y in windows coordinates
		long					_currentY;
      //! mouse x in world coordinates
      double            _realcurrentX;
      //! mouse y in world coordinates
      double            _realcurrentY;

	public:
      DECLARE_EVENT_TABLE()

};

#endif


