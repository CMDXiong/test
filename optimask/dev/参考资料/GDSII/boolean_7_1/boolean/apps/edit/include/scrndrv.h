//! author="klaasje"
//! version="1.0"
/*
Program			SCRNDRV.H
Purpose			Virtual base class for all screendrivers (header)
					derived drivers must overload Draw functions!
					Mapping and clipping is done here
*/

#ifndef _SCREENDRIVER
#define _SCREENDRIVER

#ifdef __GNUG__
#pragma interface
#endif

//!drawing style codes
enum DrawStyle { STORED,WIREFRAME, FILLED, SELECTED, INVERT_WIREFRAME };
#define MAX_POINTS_TODRAW 40000
#define MININT -30000
#define MAXINT  30000

#include "wx/dynarray.h"
#include <wx/object.h>

#include "b_lnk_itr.h"
#include "drivout.h"
#include "wxmatrix.h"
#include "bbox.h"
#include "shape.h"
#include "arc.h"
#include "wxwin.h"
#include "wx/image.h"

#include "polyline.h"
#include "doubledc.h"

//! Drawdriver is the base class for drawing
/*! the internal database contents to
 the screen. For every Object in the database there are members to write it to
 a screen type device.  For instance the display or a printer
 for doing the low level drawing.
 All function are using a DC that can be set in front for the driver.
 This makes the class one big selection of drawing functions using a wxDC.
\sa Canvas
\sa PrintDriver
*/
class DrawDriver:  public wxDoubleDCBase
{
	public:

		//!detect interruption of drawing routines
		bool Pending();

		//! constructors
		DrawDriver();

		//! destructor
		~DrawDriver();

      //!write the object to the device without history on the current transform matrix
      //!this is used in interactive drawing modes
		void				WriteDirect(ElementList*);
      //!write the object to the device without history on the current transform matrix
      //!this is used in interactive drawing modes
		void 				WriteDirect(Path* _Path);
      //!write the object to the device without history on the current transform matrix
      //!this is used in interactive drawing modes
		void 				WriteDirect(Boundary* _Boundary);

      void           WriteDirect(G_Polygon* _Polygon);

      //!write the object to the device without history on the current transform matrix
      //!this is used in interactive drawing modes
		void 				WriteDirect(Box* _Box);
      //!write the object to the device without history on the current transform matrix
      //!this is used in interactive drawing modes
		void 				WriteDirect(SLine* line);
      //!write the object to the device without history on the current transform matrix
      //!this is used in interactive drawing modes
		void 				WriteDirect(Circle* _Circle);
      //!write the object to the device without history on the current transform matrix
      //!this is used in interactive drawing modes
		void				WriteDirect(Text*);
      //!write the object to the device without history on the current transform matrix
      //!this is used in interactive drawing modes
		void				WriteDirect(Arcseg*);




      //!draw the (0,0) crosshair
		void				DrawOrigin();

      //!draw the drawing grid on the canvas
		void	DrawGrid(double,long,int);


      //!initialize the device to draw on
		void					Init();


      //!return a Boundingbox in virtual coordinates being the visual size of the client window
		BoundingBox&					GetVirtualSizeScreen();


      //!set the drawing style (filled, outline, inverted etc.)
      /*! The drawstyle is used to determine how figures should be drawn on screen.
         - WIREFRAME	= 	draw the outlines only
         - FILLED		= 	draw outlines and fill the polygons
         - SELECTED	=	draw only a selectedline around already existing objects
         - INVERTED  =  draw the object in a color being the inverse of the screencolor where the object is drawn

      P.S. With the drawstyle wireframe or filled selected objects are also drawn
      */
		void								SetDrawStyle(DrawStyle);

      //!get the current drawing style (filled, outline, inverted etc.)
		DrawStyle						GetDrawStyle();

      //!is this the current drawstyle?
		bool							IsDrawStyle(DrawStyle);

      //!clear the client window
		void					ClearScreen();

	public:

      //!write given object to window
		void				Write(GdsII*);
      //!write given object to window
		void				Write(Point*);
      //!write given object to window
		void				Write(Segment*);
      //!write given object to window
		void				Write(G_Polygon*);
      //!write given object to window
		void				Write(G_Polyline*);

      //!given a layer number, initialize all needed to draw on this layer
      //!like colors fill patterns etc.
		void					SetLayer(int layer);
      //!given a layer number, remove all that was needed to draw on this layer
      //!like colors fill patterns etc.
		void					UnSetLayer(int layer);

      //!draw text on the screen
		void					GDSDrawText(const wxString& text, double x, double y, double x2, double y2, Text*, wxTransformMatrix*);


      //!draw a line on the screen  (using dubbel buffering)
		void					GDSDrawLine(double x1,
														double y1,
														double x2,
														double y2);

      //!transform relative coordinates to absolute coordinates
      //!using the current relative matrix.
      //!it is used to draw the contents of structures that are referenced from the top level
		void								Transform(double&,
															 double&);

      //!transform relative coordinates to absolute coordinates
      //!using the current relative matrix.
      //!it is used to draw the contents of structures that are referenced from the top level
		void								Transform(double&,
															 double&,
															 double&,
															 double&);

      //!to set the total area that can be scrolled
      void SetScroll(double vx1,double vy1,double vx2,double vy2);

      void SetMappingScroll(double vx1,double vy1,double vx2,double vy2,bool border);

      //!is this layer visible, and needs drawing?
		bool							NeedToDraw(int layer);

      //!draw simple polygons but no outline
		void WindowsPolygon_NoOutLine();

      //!draw simple polygons
		void  					WindowsPolygon(void);

      //!draw simple polylines
		void  					WindowsPolyline(void);

      //!draw simple polylines
		void  					WindowsSpline(void);

      //!draw a rectangle in window coordinates
		void 				   DrawRect(int x1, int y1, int x2, int y2);

      //!does this element need clipping to MAXINT and MININT (not window)
		bool 							ClipIt(Element* a_ele);


      G_Polygon* GetAbsPolygon(){return _AbsPolygon;};
      void SetFilled(bool filled){m_filled=filled;};
      void SetSelected(bool selected){_Selected=selected;};
      wxTransformMatrix* GetMatrix(){return _matrix;};
      void SetMatrix(wxTransformMatrix& matrix){*_matrix=matrix;};

      wxPen& GetSelectPen(){return *_penSelected;};
      wxPen& GetDrawPen(){return *_pen;};
      bool GetParentSelect(){return _SRefSelected;};
      void SetParentSelect(bool select){_SRefSelected=select;};
      void SetClipStatus(OVERLAP stat) {_clip_status=stat;};
      OVERLAP GetClipStatus() {return _clip_status;};
      void SetClipIt(bool clipit){_clipit=clipit;};
      void SetBusyDrawing(bool busy){_stopdrawing=false;_busydrawing=busy;};
      bool GetStopDrawing(){return _stopdrawing;};
      void SetActiveLayer(int lay){_ActiveLayer=lay;};
      int GetActiveLayer(){return _ActiveLayer;};

      //! top of visible structures
      Structure*                 _showstructure;

    	//! true if the element should be drawn selected
		bool							_Selected;

  		//! true if the reference to the element was selected
		bool							_SRefSelected;

  		//! set true if the element to be drawn needs to be drawn filled
		bool							m_filled;

    	//! holds the size of the virtual window
		BoundingBox					m_virtualsize;

 		//! used for calculating absolute boundingboxes
		BoundingBox					_absbox;

		//! holds the matrix from structure reference
		wxTransformMatrix*	_matrix;

      //! use for checking clipping necessity
		OVERLAP 							_clip_status;

     	//! is this structure visible?
		bool							_visible;

      //!does this element/structure etc. need clipping to MAXINT and MININT (not window)
		bool							_clipit;

      //! for containing the absolute polygon to draw
		G_Polygon*                 _AbsPolygon;

       //!iterator for absolute polygon
	   TDLI<Segment>* 				_AI;

      //!active layer that is currently drawn
		int								_ActiveLayer;

      //!current drawstyle
		DrawStyle						_drawstyle;
      //!previous drawstyle
		DrawStyle						_stored_drawstyle;

      //! interrupt in drawing
      bool                     _stopdrawing;
      //! is drawing going on
      bool                     _busydrawing;

      //!temporary storage of polygons converted to windows coordinates
     	wxPoint* 		todraw;


      //! holds the drawing color for text
		wxColour*		_textColor;
      //! holds the drawing color selected text
		wxColour*		_textSelectedColor;
      //! holds the fill color
		wxColour*		_fillcolor;
      //! the pen used for drawing a border around selected elements
		wxPen*			_penSelected;

		wxPen*			_pentrans;

		wxPen*			_pentrans_sel;

      //! the pen used for drawing
		wxPen*			_pen;
      //! the pen used for grid drawing
		wxPen*			_pengrid;
      //! the pen used for origin drawing
		wxPen*			_penorigin;
      //! the brush used for filling
		wxBrush*			_brush;
      //!used in case of bitmap hatched filling
		wxBrush*			_brushbitm;
      //!used in case of SOLID filling
		wxBrush*			_brushnorm;
      //!font for text elements
		wxFont*			_font;

      //!the bitmap brush initial bitmap
		wxBitmap* 		_currentbitmap;

      //!active layer to optimize drawing
      int 				_layer;

		//! holds the patternnumber
		int								_pattern;

      //!convert colors from rgb to wxcolours
		void ltowxc(long l,wxColour& color);

      //!set clipping region using x y values of virtual window coordinates
      void SetClippingRegionBox(BoundingBox&	a_box);

      bool m_scrolled;

      //!virtual coordinates of total drawing
      double m_virtm_minX, m_virtm_minY, m_virtm_maxX, m_virtm_maxY;

      bool _only_visible;

      Image hulp2;
};

#define NR_PATTERNS 72

/*!
 list of bitmaps for drawing patterns in a polygon
*/
class FillPatterns
{
   public:
      //! constructor
      FillPatterns();
      //! destructor
      ~FillPatterns();
      wxBitmap*      GetPattern(short patternnr);
   private:
      //! array for the different fill pitmaps.
		wxBitmap*		_fillbitmaps[NR_PATTERNS];
};



#endif
