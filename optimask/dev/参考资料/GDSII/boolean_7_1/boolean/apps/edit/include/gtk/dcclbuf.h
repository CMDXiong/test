/////////////////////////////////////////////////////////////////////////////
// Name:        dcbuf.cpp
// Purpose:     wxDCbuf class
// Author:      Klaas Holwerda
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTK_BUFDCCLIENTH__
#define __GTK_BUFDCCLIENTH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/window.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------
struct WXDLLEXPORT wxPaintDCInfo;
class WXDLLEXPORT wxWindowDCbuf;
class WXDLLEXPORT wxDrawableObject
{
public:

    wxDrawableObject()
    {
        ResetBoundingBox();
    }

    virtual ~wxDrawableObject() { }

    virtual void Draw(wxWindowDCbuf& dc) const { }

    virtual void CalcBoundingBox(wxCoord x, wxCoord y)
    {
      if ( m_isBBoxValid )
      {
         if ( x < m_minX ) m_minX = x;
         if ( y < m_minY ) m_minY = y;
         if ( x > m_maxX ) m_maxX = x;
         if ( y > m_maxY ) m_maxY = y;
      }
      else
      {
         m_isBBoxValid = TRUE;

         m_minX = x;
         m_minY = y;
         m_maxX = x;
         m_maxY = y;
      }
    }

    void ResetBoundingBox()
    {
        m_isBBoxValid = FALSE;

        m_minX = m_maxX = m_minY = m_maxY = 0;
    }

    // Get the final bounding box of the PostScript or Metafile picture.

    wxCoord MinX() const { return m_minX; }
    wxCoord MaxX() const { return m_maxX; }
    wxCoord MinY() const { return m_minY; }
    wxCoord MaxY() const { return m_maxY; }

    //to define the type of object for derived objects
    virtual int GetType()=0;

protected:
    //for boundingbox calculation
    bool m_isBBoxValid:1;
    //for boundingbox calculation
    wxCoord m_minX, m_minY, m_maxX, m_maxY;
};



class wxTransparentPolygon : public wxDrawableObject
{
public:
    wxTransparentPolygon(int type)
    {
       m_type=type;
    }

    virtual void Draw(wxWindowDCbuf& dc) const;
    int GetType(){return m_type;};
    int m_type;
    int n;
    wxPoint* points;
};

class wxWindowDCbuf;
class wxPaintDCbuf;
class wxClientDCbuf;

//-----------------------------------------------------------------------------
// wxWindowDCbuf
//-----------------------------------------------------------------------------

//:defenition
//!c_des: this class is a base class for wxClientDCbuf and wxPaintDCbuf
//!c_des: this class is equivalent to wxWindowDC but maintains a bitmap
//!c_des: of all that is drawn on the window.
//!c_des: The bitmap must be selected into the DC.|
//!c_des: This bitmap can be used to quickly repaint damaged parts of the window.|
//!c_des: Reduce flickering.|
//!c_des: The futher usage is in all aspects the same as for wxClientDC and wxPaintDC.
//!rel_type: class: wxClientDCbuf
//!rel_type: class: wxPaintDCbuf
//!rel_type: class: wxClientDC
//!rel_type: class: wxPaintDC
//!rel_type: class: wxDC
class WXDLLEXPORT wxWindowDCbuf : public wxWindowDC
{
    DECLARE_DYNAMIC_CLASS(wxWindowDCbuf)

public:
    wxWindowDCbuf();

    // Create a double buffered DC corresponding to the whole window
    wxWindowDCbuf(wxWindow *win);

    //virtual destructor
    virtual ~wxWindowDCbuf();

    //get the wxMemoryDC that is used inside class to maintain the bitmap
    //that is selected into the DC. Use this to access the bitmap.
    wxDC&        GetBufferDC(){return m_buffer_dc;};

    //see wxDC class
    void 		  SetTextForeground(const wxColour& colour);
    //see wxDC class
	 void 		  SetTextBackground(const wxColour& colour);

    //to select a bitmap into the DC that will be used to draw on at the same time
    //as the window. The user must asure that it is the same size as the window
    //that is drawn up on. Override wxWindow::Onsize can be use to achive this in general.
    //In general then window attached to this DC will be used to declare/resize the bitmap.
    virtual void SelectObject(wxBitmap& bitmap);

    //see wxDC class
    virtual void Clear();
    //see wxDC class
    virtual void DestroyClippingRegion();

    //see wxDC class
    virtual void SetFont(const wxFont& font);
    //see wxDC class
    virtual void SetPen(const wxPen& pen);
    //see wxDC class
    virtual void SetBrush(const wxBrush& brush);
    //see wxDC class
    virtual void SetBackground(const wxBrush& brush);
    //see wxDC class
    virtual void SetBackgroundMode(int mode);
    //see wxDC class
    virtual void SetPalette(const wxPalette& palette);

    //see wxDC class
    virtual void SetMapMode(int mode);
    //see wxDC class
    virtual void SetUserScale(double x, double y);
    //see wxDC class
    virtual void SetSystemScale(double x, double y);
    //see wxDC class
    virtual void SetLogicalScale(double x, double y);
    //see wxDC class
    virtual void SetLogicalOrigin(wxCoord x, wxCoord y);
    //see wxDC class
    virtual void SetDeviceOrigin(wxCoord x, wxCoord y);
    //see wxDC class
    virtual void SetAxisOrientation(bool xLeftRight, bool yBottomUp);
    //see wxDC class
    virtual void SetLogicalFunction(int function);


    //clear the clipping region using the current backgrund color
	 void ClearClippingRegion();

    virtual void DrawableObject(wxDrawableObject* drawobject);

    virtual void TransParentPolyon(wxTransparentPolygon* thepolygon);
	 

protected:
	
    virtual void DoGetSize(int *width, int *height) const;
    virtual bool DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             int style = wxFLOOD_SURFACE);

    virtual bool DoGetPixel( wxCoord x1, wxCoord y1, wxColour *col ) const;

    //see wxDC class
    virtual void DoDrawPoint(wxCoord x, wxCoord y);

    //see wxDC class
    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);

    //see wxDC class
    virtual void DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc);
    //see wxDC class
    virtual void DoDrawCheckMark(wxCoord x, wxCoord y,
                                 wxCoord width, wxCoord height);
    //see wxDC class
    virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea);

    //see wxDC class
    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    //see wxDC class
    virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height,
                                        double radius);
    //see wxDC class
    virtual void DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height);

    //see wxDC class
    virtual void DoCrossHair(wxCoord x, wxCoord y);

    //see wxDC class
    virtual void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y);
    //see wxDC class
    virtual void DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask = FALSE);

    //see wxDC class
    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y);
    //see wxDC class
    virtual void DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                                   double angle);

    //see wxDC class
    //! \sa wxDC class
    virtual bool DoBlit(wxCoord xdest, wxCoord ydest,
                        wxCoord width, wxCoord height,
                        wxDC *source, wxCoord xsrc, wxCoord ysrc,
                        int rop = wxCOPY, bool useMask = FALSE, wxCoord xsrcMask = -1, wxCoord ysrcMask = -1);

    //see wxDC class
    virtual void DoSetClippingRegionAsRegion(const wxRegion& region);
    //see wxDC class
    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height);

    //see wxDC class
    virtual void DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset);

    //see wxDC class (optimized for speed using blit from the bitmap actions)
    virtual void DoDrawPolygon(int n, wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               int fillStyle = wxODDEVEN_RULE);

#if wxUSE_SPLINES
    //see wxDC class
	 virtual void DoDrawSpline(wxList *list);
#endif // wxUSE_SPLINES

    //see wxDC class
    // common part of DoDrawText() and DoDrawRotatedText()
    void DrawAnyText(const wxString& text, wxCoord x, wxCoord y);

    //memory DC used internally to draw on the selected bitmap
    wxMemoryDC m_buffer_dc;
    wxBitmap*  m_bitmap;
};


//-----------------------------------------------------------------------------
// wxPaintDCbuf
//-----------------------------------------------------------------------------

class wxPaintDCbuf : public wxWindowDCbuf
{
public:
    wxPaintDCbuf();
    wxPaintDCbuf( wxWindow *win );

private:
    DECLARE_DYNAMIC_CLASS(wxPaintDCbuf)
};

//-----------------------------------------------------------------------------
// wxClientDCbuf
//-----------------------------------------------------------------------------

//:defenition
//!c_des: In Addition to the normal wxClientDC's this DC's also support functions
//!c_des: to Move bitmaps and a drawing on the screen.
//!c_des: In such a manner that there is no flickering and modeless dialogs or other windows
//!c_des: may overlap the window.
class WXDLLEXPORT wxClientDCbuf : public wxWindowDCbuf
{
private:
    DECLARE_DYNAMIC_CLASS(wxClientDCbuf)

public:
    wxClientDCbuf();

    // Create a DC corresponding to the client area of the window
    wxClientDCbuf(wxWindow *win);

    //allows to move a bitmap accros the screen. The DC does not need to be in scope for this.
    //This means the program may perform other tasks in between.
    //The main idea is to be able to call this funtion many times, using background info returned
    //at the first time it is called.
    //it will also work in case of modeless dialogs or other windows covering the DC its window
    //!param: xdest: x coordinate to move to
    //!param: xdest: y coordinate to move to
    //!param: source: bitmap that will be moved
    //!param: withmask: use the mask attached to the bitmap to draw transparent
    //!param: use_redrawonmove: do use the virtual function ReDrawOnMove to redraw what is
    //behind the bitmap. If false the backgrnd bitmap will be used. Or if not available,
    //the backgrnd will simply be cleared using the current background color.
    //The size of the bitmap must be equal to the bitmap that is moved.
    //!param: xsrc: x coordinate to move from
    //!param: ysrc: y coordinate to move from
    //!param: backgrnd: after a move this bitmap will contain the drawing that was behind the
    //bitmap before it was drawn at the new position, use this bitmap in sequential moves.
    //Make this NULL if not needed.
    //!param: returnbackgrnd: true means do fill backgrnd after a move.
	 virtual bool MoveObject(wxCoord xdest, wxCoord ydest,wxBitmap& source,
		                      bool use_redrawonmove,
									 wxCoord xsrc, wxCoord ysrc,
                            wxBitmap* backgrnd,
                            bool returnbackgrnd
                           );

    //allows to move a drawing accros the screen. The DC does not need to be in scope for this.
    //This means the program may perform other tasks in between.
    //The main idea is to be able to call this funtion many times, using background info returned
    //at the first time it is called.
    //It will also work in case of modeless dialogs or other windows covering the DC its window.
    //Derive a class from wxClientDCbuf to redefine the DrawOnMove and ReDrawOnMove functions,
    //since they will be used to draw the drawing and restore/redraw the window after a move.
    //The virtual function DrawOnMove, is called to draw the drawing to move.
    //the DrawOnMove should draw anything you like in the window using this DC, all that is within
    //the rectangle (xdest,ydest,w,h) will be used to move as drawing.
    //!param: xdest: x coordinate to move to
    //!param: xdest: y coordinate to move to
    //!param: w: width of the drawing that will be moved. It maybe smaller, in which case only
    //part of the drawing will be used to move.
    //!param: h: height of the drawing that will be moved It maybe smaller, in which case only
    //part of the drawing will be used to move.
    //!param: use_redrawonmove: do use the virtual function ReDrawOnMove to redraw what is
    //behind the drawing. If false the backgrnd bitmap will be used. Or if not available,
    //the backgrnd will simply be cleared using the current background color.
    //The size of the bitmap must be w*h.
    //!param: xsrc: x coordinate to move from
    //!param: ysrc: y coordinate to move from
    //!param: backgrnd: after a move this bitmap will contain the drawing that was behind the
    //bitmap before it was drawn at the new position, use this bitmap in sequential moves.
    //Make this NULL if not needed.
    //!param: returnbackgrnd: true means do fill backgrnd after a move.
    virtual bool MoveObject(wxCoord xdest, wxCoord ydest,int w, int h,
                         bool use_redrawonmove,
      						 wxCoord xsrc, wxCoord ysrc,
                         wxBitmap* backgrnd,
                         bool returnbackgrnd
                        );

    //used within MoveDrawing, overide this funtion in a derived class to define
    //the drawing to move.
    virtual void DrawOnMove(int x,int y){};

    //used within MoveDrawing, overide this funtion in a derived class to define
    //the redrawing function after a move.
    virtual void RedrawOnMove(){};

    void InitiateDragObject(int first_x,int first_y,wxBitmap* source ,bool withmask, bool use_redraw);
    void InitiateDragObject(int first_x,int first_y,int w, int h ,bool use_redraw);

	 bool DragObject(wxCoord xdest, wxCoord ydest);

    void EndDragObject();

private:
    virtual bool DoMoveObject(wxCoord xdest, wxCoord ydest,int w, int h,
                         bool use_redrawonmove,
                         wxCoord xsrc, wxCoord ysrc,
                         wxBitmap* backgrnd,
                         bool returnbackgrnd
                        );
    bool m_withmask;
    wxBitmap* m_backgrnd;
    wxBitmap* m_bitmaptomove;
    bool m_first_move;
    bool m_use_redraw;
    wxCoord m_last_x;
    wxCoord m_last_y;

};


#endif // __GTK_BUFDCCLIENTH__
