//! author="klaas"
//! version="1.0"

#ifndef _TOOLS
#define _TOOLS

#ifdef __GNUG__
#pragma interface
#endif

#include "wxwin.h"
#include "mainwin.h"
#include "windrv.h"
#include "units.h"
class ToolControl;
class CircleditDialog;
enum ToolSet{ SELECT_TOOL, DESELECT_TOOL, ZOOMIN_TOOL , RECTANGLE_TOOL, POLYGON_TOOL,POLYLINE_TOOL,
				  MOVE_TOOL, ZOOMOUT_TOOL, ZOOMBACK_TOOL,	COPY_TOOL,CIRCLE2_TOOL,CIRCLE_TOOL,
              EDIT_TOOL, EDITPOLYGON_TOOL,EDITPOLYLINE_TOOL,DELETE_TOOL,EDITPROPERTY_TOOL,
              ROTATE_TOOL , EDITCIRCLE_TOOL ,EDITREC_TOOL, TEXT_TOOL, SET_CURSOR,
              INSERTREF_TOOL,INSERTSTRUC_TOOL,EDITIMAGE_TOOL,EDITREF_TOOL,EDITLINE_TOOL
				};


//! all interactive drawing using tools are derived from this base class.
/*!
 All mouse and key events on the canvas/client window are redirected to this class,
 via the ToolControl class.
 The tool derived from this base class, defines what needs to be done with the events.
\sa ToolControl
\sa DrawDriver
\sa WindowsDriver
\sa BoxTool
\sa SelectTool
\sa DeSelectTool
\sa ZoomTool
\sa RecTangleTool
\sa PolyTool
\sa CircleTool
\sa DeleteTool
\sa TextTool
\sa MoveTool
\sa CopyTool
\sa RotateTool
\sa PolyLineTool
\sa EditTool
\sa EditCircleTool
\sa EditPolyTool
\sa EditPolyLineTool
\sa EditRectangleTool
\sa EditPropertyTool
*/
class BaseTool
{
   public:
      // constructor and destructor
      BaseTool(ToolControl* controller);
      virtual ~BaseTool();
		virtual void 	OnMouseEvent(wxMouseEvent& event);
		virtual void 	OnChar(wxKeyEvent& event);
		virtual void 	OnKeyDown(wxKeyEvent& event);
		virtual void 	OnKeyUp(wxKeyEvent& event);
      ToolSet GetType();
		void SetStatusBar();
		virtual void   Draw()=0;
		virtual void   Undo();
      void				SetCursor(double x, double y);
      wxCursor			GetCursor(){return _toolcursor;};
  		virtual void   AddPoint(double x,double y);
		virtual void 	RemovePoint(){};
      bool 	GetOnlyOnce(){return _onlyonce;};
      void 		SetOnlyOnce(bool onlyonce){_onlyonce = onlyonce;};
      bool         Get_Busy(){return _busy;};
      virtual void 	Begin();
      virtual void 	End();
      virtual wxString  GetToolInfo()=0;
		ToolBarSet		GetToolBarType(){return m_toolbar;};
      //is the tool that wants to be started allowed within this tools context
		virtual bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);

   protected:
		void			 DrawHandle(wxDC& dc,double x,double y,int type=0);
		virtual void EvRD();
		virtual void EvLeftDClick();
		virtual void EvDrag();
		virtual void EvMove(int drag);
		virtual void EvLD();
		virtual void EvLU();
      void Grid();

		void CalculateCurrent();
		void CalcVirtual(long x,long y,double& xr,double& yr);


		ToolControl* 		_controller;

      wxMouseEvent		_event;

		long					_grid_x;
		long					_grid_y;
		long					_currentX;
		long					_currentY;
      double            _realcurrentX;
      double            _realcurrentY;
      ToolSet           _tooltype;
      long 					_timeevent;
      bool              _onlyonce;
      bool              _busy;
      wxCursor          _toolcursor;
		ToolBarSet			m_toolbar;
      wxCoord				m_maxx;
      wxCoord				m_minx;
      wxCoord				m_maxy;
      wxCoord				m_miny;
      bool					m_valid_bbox;

      virtual void CalcBoundingBox(wxCoord x, wxCoord y)
      {
	     if (m_valid_bbox)
        {
           if ( x < m_minx ) m_minx = x;
           if ( y < m_miny ) m_miny = y;
           if ( x > m_maxx ) m_maxx = x;
           if ( y > m_maxy ) m_maxy = y;
        }
        else
        {
           m_valid_bbox=true;
           m_minx = x;
           m_miny = y;
           m_maxx = x;
           m_maxy = y;
        }
      }


};

//!
//! used for tools that work with a rectangle area on the screen.
class BoxTool : public BaseTool

{
   public:
      // constructor and destructor
      BoxTool(ToolControl* controller);
      ~BoxTool();
		void  OnMouseEvent(wxMouseEvent& event);

   protected:
		void EvDrag();
      void EvMove(int drag);
      void EvLD();
      void EvLU();

		bool				_boxdrawn;
		struct ZoomRect {
			double	left;
			double	top;
			double	right;
			double	bottom;
		};
		ZoomRect				_ZoomRect;
	   bool  box;
		double				_firstX;
		double				_firstY;

};

//!
//! select primitives at a point are within a rectangle area on the canvas
class SelectTool : public BoxTool
{
   public:
      // constructor and destructor
      SelectTool(ToolControl* controller);
      ~SelectTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void Draw();
      wxString  GetToolInfo();

   protected:
      void EvLU();
      void EvLD();
};

//!
//! DEselect primitives at a point are within a rectangle area on the canvas
class DeSelectTool : public BoxTool
{
   public:
      // constructor and destructor
      DeSelectTool(ToolControl* controller);
      ~DeSelectTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
      void BeginTool();
      void EndTool();
		void Draw();
      wxString  GetToolInfo();

   protected:
      void EvLU();

};

//!
//! zoom in at a point are within a rectangle area on the canvas
class ZoomTool : public BoxTool
{
   public:
      // constructor and destructor
      ZoomTool(ToolControl* controller);
      ~ZoomTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void Draw();
      wxString  GetToolInfo();

   protected:
	   bool _zooming;
      void EvLU();
		void EvLD();
};

//!
//! draw a rectangle
class RecTangleTool : public BoxTool
{
   public:
      // constructor and destructor
      RecTangleTool(ToolControl* controller);
      ~RecTangleTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
      void BeginTool();
      void EndTool();
		void Draw();
		void  AddPoint(double x,double y);
      wxString  GetToolInfo();

   protected:
      void EvLU();
		void EvLD();
};

//!
//! draw a polygon
class PolyTool : public BaseTool
{
   public:
      // constructor and destructor
      PolyTool(ToolControl* controller);
      ~PolyTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void  AddPoint(double x,double y);
		void 	RemovePoint();
		void Draw();
      void End();
      wxString  GetToolInfo();

   protected:
		void EvLeftDClick();
		void EvDrag();
      void EvLD();
      void EvLU();
      void EvMove(int drag);

		bool				_firstpoint;

		G_Polygon* 			_polygon;
      Line_Seg*         _segm;
      Line_Seg          _lastsegm;

		Boundary*			_boundary;
};

//! TextTool
//! class used for inserting text given by the user in an insert-/edittext dialog.
//! There is also a possibility to give a heightvalue in combination with the unitsetting to specify the textheight.
class TextTool : public BaseTool
{
   public:
      // constructor and destructor
      TextTool(ToolControl* controller);
      ~TextTool();
		void Draw();
      void End();
      wxString  GetToolInfo();

   protected:
		void	 EvDrag();
      void   EvLD();
      void   EvLU();
      void   EvMove(int drag);
		Text*  _text;
      double _height;
      char 	 _unit[20];

};

//!
//! draw a polygonline
class PolyLineTool : public BaseTool
{
   public:
      // constructor and destructor
      PolyLineTool(ToolControl* controller);
      ~PolyLineTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void  AddPoint(double x,double y);
		void 	RemovePoint();
		void Draw();
      void End();
      wxString  GetToolInfo();

   protected:
		void EvLeftDClick();
		void EvDrag();
      void EvLD();
      void EvLU();
      void EvMove(int drag);

      G_Polyline* 		_polyline;
      bool				_firstpoint;
      Line_Seg*         _segm;
      Line_Seg          _lastsegm;
		Path*					_path;
};

//!
//! move selected elements
class MoveTool : public BaseTool
{
   public:
      // constructor and destructor
      MoveTool(ToolControl* controller);
      ~MoveTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void Draw();
		void Undo();
      wxString  GetToolInfo();

   protected:
		void EvLeftDClick();
		void EvDrag();
      void EvMove(int drag);
      void EvLD();
      void EvLU();

      wxTransformMatrix*	_trans_matrix;
      long 					_eventstamp;

		ElementList* _elemlst;
		ElementList* _elemlst_orig;

		double				_oldX;
		double				_oldY;
		double				_newX;
		double				_newY;
};

//!
//! rotate selected elements
class RotateTool : public BaseTool
{
   public:
      // constructor and destructor
      RotateTool(ToolControl* controller);
      ~RotateTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void Draw();
		void Undo();
      wxString  GetToolInfo();

   protected:
		void EvLeftDClick();
		void EvDrag();
      void EvMove(int drag);
      void EvLD();
      void EvLU();

      wxTransformMatrix*	_trans_matrix;
		ElementList* _elemlst;
		ElementList* _elemlst_orig;

      long 					_eventstamp;

		double  				_rotX;
		double				_rotY;
		double				_newX;
		double				_newY;
};

//!
//! copy selected elements
class CopyTool : public BaseTool
{
   public:
      // constructor and destructor
      CopyTool(ToolControl* controller);
      ~CopyTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void Draw();
		void Undo();
      wxString  GetToolInfo();

   protected:
		void EvLeftDClick();
		void EvDrag();
      void EvMove(int drag);
      void EvLD();
      void EvLU();

      wxTransformMatrix*	_trans_matrix;
		ElementList* _elemlst;

		double				_oldX;
		double				_oldY;
		double				_newX;
		double				_newY;
      long 					_eventstamp;
};

//!
//! draw circle  (two ways of doing)
class CircleTool : public BaseTool
{
   public:
      // constructor and destructor
      CircleTool(ToolControl* controller,bool mode);
      ~CircleTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void Draw();
      wxString  GetToolInfo();

   protected:
		void EvDrag();
      void EvLD();
      void EvLU();
      void EvMove(int drag);
		double  				_firstX;
		double				_firstY;
      bool 				_started;
      bool 				_mode;

		Circle* 				_circle;
};


//!
//! edit circle     of selected element that is left down clicked
class EditCircleTool : public BaseTool
{
   public:
      // constructor and destructor
      EditCircleTool(ToolControl* controller);
      ~EditCircleTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void Draw();
      wxString  GetToolInfo();
      wxUnit GetRadius();
      void SetRadius(wxUnit& rad);

   protected:
		void EvLeftDClick();
		void EvDrag();
      void EvLD();
      void EvLU();

		bool				_moving;

      CircleditDialog*   m_circle_dlg;

		Circle* 				_circle;
		Circle* 				_circle_orig;
};

//!
//! edit rectangle    of selected element that is left down clicked
class EditRectangleTool : public BaseTool
{
   public:
      // constructor and destructor
      EditRectangleTool(ToolControl* controller);
      ~EditRectangleTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void Draw();
      wxString  GetToolInfo();

   protected:
		void EvLeftDClick();
		void EvDrag();
      void EvLD();
      void EvLU();

		bool				_moving;
		int					_movedpoint;
		wxTransformMatrix*	_inverse_mapping;
		double 				_prevX;
		double 				_prevY;

		Box* 					_box;
		Box* 					_box_orig;
};

//!
//! edit image  of selected element that is left down clicked
class EditImageTool : public BaseTool
{
   public:
      // constructor and destructor
      EditImageTool(ToolControl* controller);
      ~EditImageTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void Draw();
      wxString  GetToolInfo();

   protected:
		void EvLeftDClick();
		void EvDrag();
      void EvLD();
      void EvLU();

		bool				    _moving;
		int					 _movedpoint;
		wxTransformMatrix* _inverse_mapping;
		double 				 _prevX;
		double 				 _prevY;

		Image* 				 _image;
		Image* 				 _image_orig;
};

//!
//! edit reference of selected element that is left down clicked
class EditReferenceTool : public BaseTool
{
   public:
      // constructor and destructor
      EditReferenceTool(ToolControl* controller);
      ~EditReferenceTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void Draw();
      wxString  GetToolInfo();

   protected:
		void EvLeftDClick();
		void EvDrag();
      void EvLD();
      void EvLU();

		bool				    _moving;
		int					 _movedpoint;
		wxTransformMatrix* _inverse_mapping;
		double 				 _prevX;
		double 				 _prevY;

		StructureReference* 				 _sref;
		StructureReference* 				 _sref_orig;
};

//!
//! edit polygon   of selected element that is left down clicked
class EditPolyTool : public BaseTool
{
   public:
      // constructor and destructor
      EditPolyTool(ToolControl* controller);
      ~EditPolyTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void  XorDrawMoveLines();
		void  XorRemoveMoveLines();
		void  AddPoint(double x,double y);
		void 	RemovePoint();
		void Draw();
      wxString  GetToolInfo();

   protected:
		void EvLeftDClick();
		void EvDrag();
      void EvLD();
      void EvLU();

		bool				_moving;

		Boundary*			_boundary;
		Boundary* 			_boundary_orig;

		double 				_prevX;
		double 				_prevY;
		double  				_XN;
		double 				_YN;
		double 				_XP;
		double 				_YP;
  	   TDLI<Segment>*    _AI;
  	   Segment*    		_PM;
};

//!
//! edit polyline    of selected element that is left down clicked
class EditPolyLineTool : public BaseTool
{
   public:
      // constructor and destructor
      EditPolyLineTool(ToolControl* controller);
      ~EditPolyLineTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void  XorDrawMoveLines();
		void  XorRemoveMoveLines();
		void  AddPoint(double x,double y);
		void 	RemovePoint();
		void Draw();
      wxString  GetToolInfo();

   protected:
		void EvLeftDClick();
		void EvDrag();
      void EvLD();
      void EvLU();

		bool				_moving;

		Path*					_path;
		Path*					_path_orig;

		double 				_prevX;
		double 				_prevY;
		double  				_XN;
		double 				_YN;
		double 				_XP;
		double 				_YP;
  	   TDLI<Segment>*    _AI;
  	   Segment*    		_PM;
};

//!
//! edit polyline    of selected element that is left down clicked
class EditLineTool : public BaseTool
{
   public:
      // constructor and destructor
      EditLineTool(ToolControl* controller);
      ~EditLineTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void Draw();
      wxString  GetToolInfo();

   protected:
		void EvLeftDClick();
		void EvDrag();
      void EvLD();
      void EvLU();

		bool				   _moving;
      bool              m_one;

		SLine*					_line;
		SLine*					_line_orig;
};

//!
//! edit properties  of selected element that is left down clicked
class EditPropertyTool : public BaseTool
{
   public:
      // constructor and destructor
      EditPropertyTool(ToolControl* controller);
      ~EditPropertyTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void Draw();
      wxString  GetToolInfo();

   protected:
      void EvLD();
};

//!
//! delete tool (deletion of selected element that is left down clicked)
class DeleteTool : public BaseTool
{
   public:
      // constructor and destructor
      DeleteTool(ToolControl* controller);
      ~DeleteTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void Draw();
      wxString  GetToolInfo();

   protected:
      void EvLD();

		ElementList* _elemlst;
};

//!
//! entry to all other edit tools (depending on the selected element and where the mouse
//! is clicked, the right edit tool will be started.
class EditTool : public BaseTool
{
   public:
      // constructor and destructor
      EditTool(ToolControl* controller);
      ~EditTool();
		bool IsAllowedTool(ToolSet tool_to_start_next,bool onlyonce,int tools_on_stack);
		void Draw();
      wxString  GetToolInfo();

   protected:
      void EvLD();
      char 	 _unit[20];
      G_Polyline* 		_polyline;
      bool				_firstpoint;
      Line_Seg*         _segm;
		Path*					_path;
      Circle* 				_circle;
      bool 				_started;
};

//!
//! insert structure reference at left down click
class InsertReferenceTool : public BaseTool
{
   public:
      // constructor and destructor
      InsertReferenceTool(ToolControl* controller);
      ~InsertReferenceTool();
		void Draw();
      void End();
      wxString  GetToolInfo();

   protected:
		void	 EvDrag();
      void   EvLD();
      void   EvLU();
      void   EvMove(int drag);
};

//!
//! insert structure reference at left down click
class InsertStructureTool : public BaseTool
{
   public:
      // constructor
      InsertStructureTool(ToolControl* controller);
      // destructor
      ~InsertStructureTool();
		void Draw();
      void End();
      wxString  GetToolInfo();

   protected:
		void	 EvDrag();
      void   EvLD();
      void   EvLU();
      void   EvMove(int drag);
};
#endif

