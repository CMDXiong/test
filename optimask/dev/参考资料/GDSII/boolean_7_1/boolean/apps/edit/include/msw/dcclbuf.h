/////////////////////////////////////////////////////////////////////////////
// Name:        dcclbuf.h
// Purpose:     wxXXXDCbuf classes
//! author="Klaas Holwerda"
//! version="1.1"
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCCLIENT_BUF_H_
#define _WX_DCCLIENT_BUF_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma interface "dcclientbuf.h"
#endif

#include "wx/dynarray.h"
#include <wx/object.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>

// ----------------------------------------------------------------------------
// array types
// ----------------------------------------------------------------------------

// this one if used by wxPaintDC only

// ----------------------------------------------------------------------------
// DC classes
// ----------------------------------------------------------------------------
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

//! this class is a base class for wxClientDCbuf and wxPaintDCbuf
/*!
  this class is equivalent to wxWindowDC but maintains a bitmap
  of all that is drawn on the window.
  The bitmap must be selected into the DC.
  This bitmap can be used to quickly repaint damaged parts of the window.
  Reduce flickering.
  The futher usage is in all aspects the same as for wxClientDC and wxPaintDC.
\sa  wxClientDCbuf
\sa  wxPaintDCbuf
\sa  wxClientDC
\sa  wxPaintDC
\sa  wxDC
*/
class WXDLLEXPORT wxWindowDCbuf : public wxDC
{
public:
    wxWindowDCbuf();

    //! Create a double buffered DC corresponding to the whole window
    wxWindowDCbuf(wxWindow *win);

    //!virtual destructor
    virtual ~wxWindowDCbuf();

    //!get the wxMemoryDC that is used inside class to maintain the bitmap
    //!that is selected into the DC. Use this to access the bitmap.
    wxDC&        GetBufferDC(){return m_buffer_dc;};

    //! \sa wxDC class
    void 		  SetTextForeground(const wxColour& colour);
    //! \sa wxDC class
	 void 		  SetTextBackground(const wxColour& colour);

    //!to select a bitmap into the DC that will be used to draw on at the same time
    /*!as the window. The user must asure that it is the same size as the window
    that is drawn up on. Override wxWindow::Onsize can be use to achive this in general.
    In general then window attached to this DC will be used to declare/resize the bitmap.
    */
    virtual void SelectObject(wxBitmap& bitmap);

    //! \sa wxDC class
    virtual void Clear();
    //! \sa wxDC class
    virtual void DestroyClippingRegion();

    //! \sa wxDC class
    virtual void SetFont(const wxFont& font);
    //! \sa wxDC class
    virtual void SetPen(const wxPen& pen);
    //! \sa wxDC class
    virtual void SetBrush(const wxBrush& brush);
    //! \sa wxDC class
    virtual void SetBackground(const wxBrush& brush);
    //! \sa wxDC class
    virtual void SetBackgroundMode(int mode);
    //! \sa wxDC class
    virtual void SetPalette(const wxPalette& palette);

    //! \sa wxDC class
    virtual void SetMapMode(int mode);
    //! \sa wxDC class
    virtual void SetUserScale(double x, double y);
    //! \sa wxDC class
    virtual void SetSystemScale(double x, double y);
    //! \sa wxDC class
    virtual void SetLogicalScale(double x, double y);
    //! \sa wxDC class
    virtual void SetLogicalOrigin(wxCoord x, wxCoord y);
    //! \sa wxDC class
    virtual void SetDeviceOrigin(wxCoord x, wxCoord y);
    //! \sa wxDC class
    virtual void SetAxisOrientation(bool xLeftRight, bool yBottomUp);
    //! \sa wxDC class
    virtual void SetLogicalFunction(int function);


    //!clear the clipping region using the current background color
    void ClearClippingRegion();

    virtual void DrawableObject(wxDrawableObject* drawobject);

    virtual void TransParentPolyon(wxTransparentPolygon* thepolygon);

    //! \sa wxDC class
    virtual bool DoBlit(wxCoord xdest, wxCoord ydest,
                        wxCoord width, wxCoord height,
                        wxDC *source, wxCoord xsrc, wxCoord ysrc,
                        int rop = wxCOPY, bool useMask = FALSE, wxCoord xsrcMask = -1, wxCoord ysrcMask = -1);

protected:

	virtual void DoGetSize(int *width, int *height) const {};
	virtual void DoGetSizeMM(int* width, int* height) const {};
    //! \sa wxDC class
    virtual bool DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             int style = wxFLOOD_SURFACE);

    //! \sa wxDC class
    virtual void DoDrawPoint(wxCoord x, wxCoord y);

    //! \sa wxDC class
    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);

    //! \sa wxDC class
    virtual void DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc);
    //! \sa wxDC class
    virtual void DoDrawCheckMark(wxCoord x, wxCoord y,
                                 wxCoord width, wxCoord height);
    //! \sa wxDC class
    virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea);

    //! \sa wxDC class
    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    //! \sa wxDC class
    virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height,
                                        double radius);
    //! \sa wxDC class
    virtual void DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height);

    //! \sa wxDC class
    virtual void DoCrossHair(wxCoord x, wxCoord y);

    //! \sa wxDC class
    virtual void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y);
    //! \sa wxDC class
    virtual void DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask = FALSE);

    //! \sa wxDC class
    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y);
    //! \sa wxDC class
    virtual void DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                                   double angle);

    //! \sa wxDC class
    virtual void DoSetClippingRegionAsRegion(const wxRegion& region);

    //! \sa wxDC class
    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height);

    //! \sa wxDC class
    virtual void DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset);

    //! \sa wxDC class (optimized for speed using blit from the bitmap actions)
    virtual void DoDrawPolygon(int n, wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               int fillStyle = wxODDEVEN_RULE);

#if wxUSE_SPLINES
    //! \sa wxDC class
	 virtual void DoDrawSpline(wxList *list);
#endif // wxUSE_SPLINES

    //! \sa wxDC class
    //! common part of DoDrawText() and DoDrawRotatedText()
    void DrawAnyText(const wxString& text, wxCoord x, wxCoord y);

    //!memory DC used internally to draw on the selected bitmap
    wxMemoryDC m_buffer_dc;
    wxBitmap*  m_bitmap;

    DECLARE_DYNAMIC_CLASS(wxWindowDCbuf)


};

//!In Addition to the normal wxClientDC's this DC's also support functions
/*!
  to Move and Drag bitmaps,drawings on the screen.
  In such a manner that there is no flickering and modeless dialogs or other windows
  may overlap the window.
*/
class WXDLLEXPORT wxClientDCbuf : public wxWindowDCbuf
{
public:
    wxClientDCbuf();

    //! Create a DC corresponding to the client area of the window
    wxClientDCbuf(wxWindow *win);

    virtual ~wxClientDCbuf();

    //!allows to move a bitmap accros the screen.
    /*!The DC does not need to be in scope for this.
    This means the program may perform other tasks in between.
    The main idea is to be able to call this funtion many times, using background info returned
    at the first time it is called.
    it will also work in case of modeless dialogs or other windows covering the DC its window
    \param xdest: x coordinate to move to
    \param xdest: y coordinate to move to
    \param source: bitmap that will be moved
    \param use_redrawonmove: do use the virtual function ReDrawOnMove to redraw what is
    behind the bitmap. If false the backgrnd bitmap will be used. Or if not available,
    the backgrnd will simply be cleared using the current background color.
    The size of the bitmap must be equal to the bitmap that is moved.
    \param xsrc: x coordinate to move from
    \param ysrc: y coordinate to move from
    \param backgrnd: after a move this bitmap will contain the drawing that was behind the
    bitmap before it was drawn at the new position, use this bitmap in sequential moves.
    Make this NULL if not needed.
    \param returnbackgrnd: true means do fill backgrnd after a move.
    */
	 virtual bool MoveObject(wxCoord xdest, wxCoord ydest,wxBitmap& source,
		                      bool use_redrawonmove,
									 wxCoord xsrc, wxCoord ysrc,
                            wxBitmap* backgrnd,
                            bool returnbackgrnd
                           );

    //!allows to move a drawing accros the screen.
    /*!
    The DC does not need to be in scope for this.
    This means the program may perform other tasks in between.
    The main idea is to be able to call this funtion many times, using background info returned
    at the first time it is called.
    It will also work in case of modeless dialogs or other windows covering the DC its window.
    Derive a class from wxClientDCbuf to redefine the DrawOnMove and ReDrawOnMove functions,
    since they will be used to draw the drawing and restore/redraw the window after a move.
    The virtual function DrawOnMove, is called to draw the drawing to move.
    the DrawOnMove should draw anything you like in the window using this DC, all that is within
    the rectangle (xdest,ydest,w,h) will be used to move as drawing.
    \param xdest: x coordinate to move to
    \param xdest: y coordinate to move to
    \param w: width of the drawing that will be moved. It maybe smaller, in which case only
               part of the drawing will be used to move.
    \param h: height of the drawing that will be moved It maybe smaller, in which case only
               part of the drawing will be used to move.
    \param use_redrawonmove: do use the virtual function ReDrawOnMove to redraw what is
      behind the drawing. If false the backgrnd bitmap will be used. Or if not available,
      the backgrnd will simply be cleared using the current background color.
      The size of the bitmap must be w*h.
    \param xsrc: x coordinate to move from
    \param ysrc: y coordinate to move from
    \param backgrnd: after a move this bitmap will contain the drawing that was behind the
      bitmap before it was drawn at the new position, use this bitmap in sequential moves.
    \param Make this NULL if not needed.
    \param returnbackgrnd: true means do fill backgrnd after a move.
    */
    virtual bool MoveObject(wxCoord xdest, wxCoord ydest,int w, int h,
                         bool use_redrawonmove,
      						 wxCoord xsrc, wxCoord ysrc,
                         wxBitmap* backgrnd,
                         bool returnbackgrnd
                        );

    /*!used within MoveObject and DragObject, overide this function in a derived class to define
    the drawing to move.
    \param x: x coordinate the object needs to be drawn
    \param y: y coordinate the object needs to be drawn
    */
    virtual void DrawOnMove(int x,int y){};

    //!used within MoveObject and DragObject, overide this function in a derived class to define
    //!the redrawing function after a move.
    virtual void RedrawOnMove(){};


    /*!prepare the DC for dragging an object (here a bitmap) with DragObject
    \param first_x: x coordinate where the object will be drawn first
    \param first_y: y coordinate where the object will be drawn first
    \param source: bitmap that will be moved
    \param withmask: use the mask attached to the bitmap to draw transparent
    \param use_redraw: do use the virtual function ReDrawOnMove to redraw what is
                       behind the bitmap when the object is dragged the first time.
    */
    void InitiateDragObject(int first_x,int first_y,wxBitmap* source ,bool withmask, bool use_redraw);

    /*!prepare the DC for dragging an object (here a drawing defined with DrawOnMove) with DragObject
    \param first_x: x coordinate where the object will be drawn first
    \param first_y: y coordinate where the object will be drawn first
    \param w: width of the drawing that will be moved. It maybe smaller, in which case only
               part of the drawing will be used to move.
    \param h: height of the drawing that will be moved It maybe smaller, in which case only
               part of the drawing will be used to move.
    \param use_redraw: do use the virtual function ReDrawOnMove to redraw what is
    \param behind the bitmap when the object is dragged the first time.
    */
    void InitiateDragObject(int first_x,int first_y,int w, int h ,bool use_redraw);

    /*!drag the object defined with InitiateDragObject to a new position |
    for this function the DC must stay within scope.
    \param xdest: x coordinate where the object will be moved to
    \param ydest: y coordinate where the object will be moved to
    */
	 bool DragObject(wxCoord xdest, wxCoord ydest);

    //!end a dragging sequence
    void EndDragObject();

private:

    //!used internal
    virtual bool DoMoveObject(wxCoord xdest, wxCoord ydest,int w, int h,
                         bool use_redrawonmove,
                         wxCoord xsrc, wxCoord ysrc,
                         wxBitmap* backgrnd,
                         bool returnbackgrnd
                        );
    //!used internal to tell to drag with a mask or not
    bool m_withmask;

    //!used internal to save the information behind the object dragged
    wxBitmap* m_backgrnd;
    //!what is the object dragged
    wxBitmap* m_bitmaptomove;
    //!dragging the first time for this object
    bool m_first_move;
    //!use redraw function when dragged the first time
    bool m_use_redraw;
    //!last drag x position
    wxCoord m_last_x;
    //!last drag y position
    wxCoord m_last_y;

    DECLARE_DYNAMIC_CLASS(wxClientDCbuf)

    
};

class WXDLLEXPORT wxPaintDCbuf : public wxWindowDCbuf
{
public:
    wxPaintDCbuf();

    //! Create a DC corresponding for painting the window in OnPaint()
    wxPaintDCbuf(wxWindow *win);

    virtual ~wxPaintDCbuf();

protected:
    static wxArrayDCInfo ms_cache;

    //! find the entry for this DC in the cache (keyed by the window)
    wxPaintDCInfo *FindInCache(size_t *index = NULL) const;

    DECLARE_DYNAMIC_CLASS(wxPaintDCbuf)

};

#endif
// _WX_DCCLIENT_BUF_H_
