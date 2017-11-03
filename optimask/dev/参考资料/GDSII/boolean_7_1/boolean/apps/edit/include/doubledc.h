/////////////////////////////////////////////////////////////////////////////
// Name:        doubledc.h
// Purpose:     wxDoubleDCBase class
//! author="Klaas Holwerda"
//! version="1.0"
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef _DOUBLEDC
#define _DOUBLEDC

#ifdef __GNUG__
#pragma interface
#endif

#define MAX_POINTS_TODRAW 40000
#define MININT -30000
#define MAXINT  30000

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <math.h>

#include "wxmatrix.h"
#include "wx/image.h"
#include "wx/geometry.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//! This class is a "sort of DC", and can be used in the same manner as
/*!
 normal DC's.
 Everything you draw using this class will be converted from world to windows coordinates.
 All coordinates are in double's.
 The class needs a real wxDC derived DC in order to draw something.
 Only one instance of this class is normal sufficient, since it can work with any wxDC
 by just setting the new DC for the class.
 The world coordinates are set using the SetMapping function.
\note
   It is important not to forget to reset the DC (when the given DC was a temporary variable)
\par Example:
   Do reset the DC!
\code
 { wxClientDC a_dc;
   wxDoubleDCBase a_doubledc;
   a_doubledc.SetDC(&a_dc);
   a_doubledc.Draw------Something();
   a_doubledc.SetDC(0);
 }
\endcode
\sa wxClientDC
\sa wxPaintDC
\sa wxDC
*/
class WXDLLEXPORT wxDoubleDCBase
{
public:

    //!constructor
    wxDoubleDCBase();

    //!destructor
    virtual ~wxDoubleDCBase();

    //!this function tells the class what wxDC derived DC should be used for output
    wxDC* SetDC(wxDC* newdc);

    //!gives the current DC in use, NULL if not set.
    wxDC* GetDC(){return m_dc;};

    //!current X scale factor for virtual to window coordinates
    double 	GetScaleFactorX(){return m_scalefactor_x;};

    //!current Y scale factor for virtual to window coordinates
    double 	GetScaleFactorY(){return m_scalefactor_y;};

    //!return clipcode for a point, clipped against the virtual window
    int 	GetClipCode(double,double);

    //!return if point is clipped against the virtual window
    bool   	Clipping(double&,double&);

    //!return if both points are clipped against the virtual window
    bool	Clipping(double&,double&,double&,double&);

    //!matrix for calculating the virtual coordinate given a screen coordinate
    wxTransformMatrix	GetInverseMappingMatrix();

    //!matrix for calculating the screen coordinate given a virtual coordinate
    wxTransformMatrix	GetMappingMatrix();

    //!given the virtual size to be displayed,
    /*!
    the mappingmatrix will be calculated
    in such a manner that it fits (same ratio in width and height) to the window size.
    The window size is normally used to intitialize the mapping, but for printing you
    would set the size of the paper etc.
    The virtual size is just an indication, it will be ajusted to fit in the dwxi and dwyi ratio
    */
    void SetMapping(int dwxi, int dwyi,double vx1,double vy1,double vx2,double vy2,bool border);

    /* ------------------
    // graphic primitives
    --------------------*/

    void FloodFill(double x, double y, const wxColour& col,
                   int style = wxFLOOD_SURFACE);
    void FloodFill(const wxPoint2DDouble& pt, const wxColour& col,
                   int style = wxFLOOD_SURFACE);
    bool GetPixel(double x, double y, wxColour *col);
    bool GetPixel(const wxPoint2DDouble& pt, wxColour *col);

    //!Draws a line from the first point to the second. The current pen is used for drawing the line.
    void DrawLine(double x1, double y1, double x2, double y2);

    //!Draws a line from the first point to the second. The current pen is used for drawing the line.
    void DrawLine(const wxPoint2DDouble& pt1, const wxPoint2DDouble& pt2);
    void CrossHair(double x, double y);
    void CrossHair(const wxPoint2DDouble& pt);
    void DrawArc(double x1, double y1, double x2, double y2,
                 double xc, double yc);
    void DrawArc(const wxPoint2DDouble& pt1, const wxPoint2DDouble& pt2, const wxPoint2DDouble& centre);

    //!Draws a check mark inside the given rectangle.
    void DrawCheckMark(double x, double y,
                       double width, double height);
    void DrawCheckMark(const wxRect2DDouble& rect);

    void DrawEllipticArc(double x, double y, double w, double h,
                         double sa, double ea);
    void DrawEllipticArc(const wxPoint2DDouble& pt, const wxSize& sz,
                         double sa, double ea);
    void DrawPoint(double x, double y);
    void DrawPoint(const wxPoint2DDouble& pt);

    //!Draws lines using an array of points of size n, or list of pointers to points,
    //!adding the optional offset coordinate. The current pen is used for drawing the lines.
    void DrawLines(int n, wxPoint2DDouble points[],
                   double xoffset = 0, double yoffset = 0);

    //!Draws lines using an array of points of size n,
    /*!or list of pointers to points,
    adding the optional offset coordinate. The current pen is used for drawing the lines.
    The programmer is responsible for deleting the list of points.
    */
    void DrawLines(const wxList *list,
                   double xoffset = 0, double yoffset = 0);

    //!Draws a filled polygon using an array of points of size n,
    /*!or list of pointers to points, adding the optional offset coordinate.
    The last argument specifies the fill rule: wxODDEVEN_RULE (the default) or wxWINDING_RULE.
    The current pen is used for drawing the outline, and the current brush for filling the shape. Using a transparent brush suppresses filling. The programmer is responsible for deleting the list of points.
    Note that wxWindows automatically closes the first and last points.
    */
    void DrawPolygon(int n, wxPoint2DDouble points[],
                     double xoffset = 0, double yoffset = 0,
                     int fillStyle = wxODDEVEN_RULE);

    void DrawPolygon(const wxList *list,
                     double xoffset = 0, double yoffset = 0,
                     int fillStyle = wxODDEVEN_RULE);

    //!Draws a rectangle with the given bottom left corner, and with the given size.
    //!The current pen is used for the outline and the current brush for filling the shape
    void DrawRectangle(double x, double y, double width, double height);
    void DrawRectangle(const wxPoint2DDouble& pt, const wxSize& sz);
    void DrawRectangle(const wxRect2DDouble& rect);

    //!Draws a rectangle with the given bottom left corner, and with the given size.
    /*!The corners are quarter-circles using the given radius. The current pen is
    used for the outline and the current brush for filling the shape.

    If radius is positive, the value is assumed to be the radius of the rounded corner.
    If radius is negative, the absolute value is assumed to be the proportion of the
    smallest dimension of the rectangle. This means that the corner can be
    a sensible size relative to the size of the rectangle, and also avoids
    the strange effects X produces when the corners are too big for the rectangle.
    */
    void DrawRoundedRectangle(double x, double y, double width, double height,
                              double radius);
    void DrawRoundedRectangle(const wxPoint2DDouble& pt, const wxSize& sz,
                             double radius);
    void DrawRoundedRectangle(const wxRect2DDouble& r, double radius);

    //!draw a circle
    void DrawCircle(double x, double y, double radius);
    //!draw an ellipse
    void DrawEllipse(double x, double y, double width, double height);
    void DrawEllipse(const wxPoint2DDouble& pt, const wxSize& sz);
    void DrawEllipse(const wxRect2DDouble& rect);

/*
    void DrawIcon(const wxIcon& icon, double x, double y);
    void DrawIcon(const wxIcon& icon, const wxPoint2DDouble& pt);

    void DrawBitmap(const wxBitmap &bmp, double x, double y,
                    bool useMask = FALSE);
    void DrawBitmap(const wxBitmap &bmp, const wxPoint2DDouble& pt,
                    bool useMask = FALSE);
*/
    void DrawText(const wxString& text, double x, double y);
    void DrawText(const wxString& text, const wxPoint2DDouble& pt);

    //!Draws the text rotated by angle degrees.
    void DrawRotatedText(const wxString& text, double x, double y, double angle);
    void DrawRotatedText(const wxString& text, const wxPoint2DDouble& pt, double angle);
/*
    bool Blit(double xdest, double ydest, double width, double height,
              wxDC *source, double xsrc, double ysrc,
              int rop = wxCOPY, bool useMask = FALSE);
    bool Blit(const wxPoint2DDouble& destPt, const wxSize& sz,
              wxDC *source, const wxPoint2DDouble& srcPt,
              int rop = wxCOPY, bool useMask = FALSE);
*/

#if wxUSE_SPLINES
    //! TODO: this API needs fixing (wxPoint2DDoubleList, why (!const) "wxList *"?)
    void DrawSpline(int n, wxPoint2DDouble points[]);

/*    void DrawSpline(wxList *points);
*/
#endif // wxUSE_SPLINES


    /*
    // global DC operations
    // --------------------
    */

    //!clear the window using the current background color
    virtual void Clear();

    virtual bool StartDoc(const wxString& WXUNUSED(message)) { return TRUE; }
    virtual void EndDoc() { }

    virtual void StartPage() { }
    virtual void EndPage() { }

    /*
    // set objects to use for drawing
    */

    //!set the font to be used for text drawing, the size is in world coordinates
    virtual void SetFont(const wxFont& font,double size);
    virtual void SetPen(const wxPen& pen) ;
    virtual void SetBrush(const wxBrush& brush) ;
    virtual void SetBackground(const wxBrush& brush);
    virtual void SetBackgroundMode(int mode);
    virtual void SetPalette(const wxPalette& palette);

    /*
    // clipping region
    */

    //!set clipping region using x y values of windows coordinates
    void SetClippingRegionWin(wxCoord minx,wxCoord miny,wxCoord maxx,wxCoord maxy);

    //!set clipping region using x y values of virtual window coordinates
    void SetClippingRegion(double minx,double miny,double maxx,double maxy);

    //!set clipping region off
    void DestroyClippingRegion();

    //!clear the clippingregion using the current background color
    void ClearClippingRegion();

    //!extend clipping region by this point
    void ExtendClippingRegion(double x, double y);

    //!what is the current clipping region
    virtual void GetClippingBox(double& x, double& y,
                              double& w, double& h) const;
    //!return a virtual coordinates being the visual size of the client window
    virtual void GetVirtualSize(double& x, double& y,
                              double& w, double& h) const;
    /*
    // text extent
    // -----------

    //    virtual double GetCharHeight() const;
    //    virtual double GetCharWidth() const;

    //    void GetTextExtent(const wxString& string,
    //                       double *x, double *y,
    //                       double *descent = NULL,
    //                       double *externalLeading = NULL,
    //                       wxFont *theFont = NULL) const;

    // size and resolution
    // -------------------

    // in device units
    //    void GetSize(int *width, int *height) const;
    //    wxSize GetSize() const;
    // in mm
    //    void GetSizeMM(int* width, int* height) const;
    //    wxSize GetSizeMM() const;

    // coordinates conversions
    // -----------------------

    // This group of functions does actual conversion of the input, as you'd
    // expect.
    */

    //!convert from window to virtual coordinates
    double DeviceToLogicalX(double x) const;
    //!convert from window to virtual coordinates
    double DeviceToLogicalY(double y) const;
    //!convert from window to virtual coordinates relatif
    double DeviceToLogicalXRel(double x) const;
    //!convert from window to virtual coordinates relatif
    double DeviceToLogicalYRel(double y) const;
    //!convert from virtual to window coordinates
    double LogicalToDeviceX(double x) const;
    //!convert from virtual to window coordinates
    double LogicalToDeviceY(double y) const;
    //!convert from virtual to window coordinates relatif
    double LogicalToDeviceXRel(double x) const;
    //!convert from virtual to window coordinates relatif
    double LogicalToDeviceYRel(double y) const;

    /*
    // Resolution in Pixels per inch
    //    virtual wxSize GetPPI() const;
    */

    virtual bool Ok() const { return m_ok; }

    /*
    // accessors
    // ---------
    */


    /* const...*/
    int GetBackgroundMode() const { return m_backgroundMode; }
    const wxBrush&  GetBackground() const { return m_backgroundBrush; }
    const wxBrush&  GetBrush() const { return m_brush; }
    const wxFont&   GetFont() const { return m_font; }
    const wxPen&    GetPen() const { return m_pen; }
    const wxColour& GetTextBackground() const { return m_textBackgroundColour; }
    const wxColour& GetTextForeground() const { return m_textForegroundColour; }

    /* ... and non const */
    wxBrush&  GetBackground() { return m_backgroundBrush; }
    wxBrush&  GetBrush() { return m_brush; }
    wxFont&   GetFont() { return m_font; }
    wxPen&    GetPen() { return m_pen; }
    wxColour& GetTextBackground() { return m_textBackgroundColour; }
    wxColour& GetTextForeground() { return m_textForegroundColour; }

    //!Sets the current text background colour for the DC.
    //!In case of stipple filled primitives using a bitmap and a mask.
    //!The TextForeground color is used for the non transparent parts
    virtual void SetTextForeground(const wxColour& colour);

    //!Sets the current text foreground colour for the DC.
    //!In case of stipple filled primitives using a bitmap and a mask.
    //!The TextBackground color is used for the transparent parts
    virtual void SetTextBackground(const wxColour& colour);

    //!Sets the current logical function for the device context.
    /*! This determines how a source pixel
    (from a pen or brush colour, or source device context if using wxDC::Blit) combines with a
    destination pixel in the current device context.

    The possible values and their meaning in terms of source and destination pixel values are as follows:

        - wxAND                 src AND dst
        - AND_INVERT          (NOT src) AND dst
        - AND_REVERSE         src AND (NOT dst)
        - CLEAR               0
        - COPY                src
        - EQUIV               (NOT src) XOR dst
        - INVERT              NOT dst
        - NAND                (NOT src) OR (NOT dst)
        - NOR                 (NOT src) AND (NOT dst)
        - NO_OP               dst
        - OR                  src OR dst
        - OR_INVERT           (NOT src) OR dst
        - OR_REVERSE          src OR (NOT dst)
        - SET                 1
        - SRC_INVERT          NOT src
        - XOR                 src XOR dst
        
    The default is wxCOPY, which simply draws with the current colour.
    The others combine the current colour and the background using a logical operation.
    wxINVERT is commonly used for drawing rubber bands or moving outlines, since drawing
    twice reverts to the original colour.
    */
    int GetLogicalFunction() const { return m_logicalFunction; }

    //! setlogical function
    virtual void SetLogicalFunction(int function) ;

    //! bounding box calculation which is updated with evrything drawn sofar
    //!\note It can be reset in between
    virtual void CalcBoundingBox(double x, double y)
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

    //!reset the boundingbox of things drawn sofar
    //!\note see CalcBoundingBox
    void ResetBoundingBox()
    {
        m_isBBoxValid = FALSE;

        m_minX = m_maxX = m_minY = m_maxY = 0;
        if (m_dc)
            m_dc->ResetBoundingBox();
    }

    //! Get the final bounding box its MinX
    double MinX() const { return m_minX; }
    //! Get the final bounding box its MaxX
    double MaxX() const { return m_maxX; }
    //! Get the final bounding box its MinY
    double MinY() const { return m_minY; }
    //! Get the final bounding box its MaxY
    double MaxY() const { return m_maxY; }

    //! for compatibility with the old code when double was long everywhere
    void GetTextExtent(const wxString& string,
                       long *x, long *y,
                       long *descent = NULL,
                       long *externalLeading = NULL,
                       wxFont *theFont = NULL) const;


    //! virtual coordinates box
    double m_virt_minX, m_virt_minY, m_virt_maxX, m_virt_maxY;

protected:

    //!holds this
    bool m_colour:1;

    //!holds this
    bool m_ok:1;

    //! is clippingbox set
    bool m_clipping:1;

    //!holds this
    bool m_isInteractive:1;

    //! is the boundingbox valid
    bool m_isBBoxValid:1;

    //!holds this
    bool m_isVirtualValid:1;

    //! bounding box
    double m_minX, m_minY, m_maxX, m_maxY;

    //! clipping box
    double m_clipX1, m_clipY1, m_clipX2, m_clipY2;

    //! holds this
    int m_logicalFunction;
    //! holds this
    int m_backgroundMode;
    //! holds this
    int m_mappingMode;
    //! holds the text height in world coordinates
    double m_fontsize;
    //! holds this
    wxPen             m_pen;
    //! holds this
    wxBrush           m_brush;
    //! holds this
    wxBrush           m_backgroundBrush;
    //! holds this
    wxColour          m_textForegroundColour;
    //! holds this
    wxColour          m_textBackgroundColour;
    //! holds this, the height of the text is in m_fontsize
    wxFont            m_font;
    //! holds this
    wxPalette         m_palette;

    //! holds the matrix for mapping from virtual to screen coordinates
    wxTransformMatrix m_mapping_matrix;

    //! holds the inverse of the mapping matrix
    wxTransformMatrix m_inverse_mapping;

    //! holds the scalefactor for the window in x
    double				m_scalefactor_x;
    //! holds the scalefactor for the window in y
    double				m_scalefactor_y;

    //!the base DC that is used by all drawing function to draw on a device.
    wxDC*			    m_dc;

    //!temporary storage of polygons converted to windows coordinates
    //! \note meant for speeding up things, NOT used yet
    wxPoint* 		    m_todraw;

private:
};

#endif

