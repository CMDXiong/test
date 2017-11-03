#ifndef __wxsvg_H
#define __wxsvg_H
#include <wx/wfstream.h>
#include <wx/string.h>

#define wxSVGVersion "v0004"

class wxSVGFileDC : public wxDC
{

    private:
        wxFileOutputStream * outfile ;
        bool isOK ;
        // store pen and brush for later reuse
        wxPen m_prevpen , m_oldpen ;                          // m_oldpen not same as m_oldPen
        wxBrush m_prevbrush, m_oldbrush ;
        wxFont m_prevfont, m_oldfont ;			

        void Init (wxString f, int Width, int Height, float dpi);

    public:

        wxSVGFileDC (wxString f);
        wxSVGFileDC (wxString f, int Width, int Height);
        wxSVGFileDC (wxString f, int Width, int Height, float dpi);
        ~wxSVGFileDC();

        bool Ok() const {return isOK;};

        // these functions were abstract in wxGTK

        bool CanDrawBitmap() const
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::CanDrawBitmap Call not yet implemented"); return true ; };

        bool CanGetTextExtent() const { return true ; };

        int GetDepth() const
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::GetDepth Call not yet implemented"); return -1 ; }; 

        bool DoFloodFill(wxCoord, wxCoord, const class wxColour &, int = 1)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::DoFloodFill Call not yet implemented"); return TRUE ; };

        bool DoGetPixel(wxCoord, wxCoord, class wxColour *) const
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::DoGetPixel Call not yet implemented"); return true ; };

        void DoDrawPoint(wxCoord, wxCoord) ;

		virtual void DoGetSize(int *width, int *height) const
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::DoGetSize Call not yet implemented");  };

		void DoDrawArc(wxCoord, wxCoord, wxCoord, wxCoord, wxCoord, wxCoord);

        void DoCrossHair(wxCoord, wxCoord)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::Blit Call not yet implemented"); return  ; };

        void DoDrawIcon(const class wxIcon &, wxCoord, wxCoord)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::DoDrawIcon Call not yet implemented"); return  ; };

        void DoDrawBitmap(const class wxBitmap &, wxCoord, wxCoord, bool = 0)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::DoDrawBitmap Call not yet implemented"); return  ; };

	     //! \sa wxDC class
   	  virtual bool DoBlit(wxCoord xdest, wxCoord ydest,
                            wxCoord width, wxCoord height,
                            wxDC *source, wxCoord xsrc, wxCoord ysrc,
                            int rop = wxCOPY, bool useMask = FALSE, wxCoord xsrcMask = -1, wxCoord ysrcMask = -1)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::DoBlit Call not yet implemented"); return false ; };

        void DoSetClippingRegionAsRegion(const class wxRegion &)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::DoSetClippingRegionAsRegion Call not yet implemented"); return  ; };

        void DoDrawSpline(wxList *points) ;

        // these are from the HTML docs

        void BeginDrawing() { return;};

        bool Blit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height, wxDC* source, wxCoord xsrc, wxCoord ysrc, int logicalFunc = wxCOPY, bool useMask = FALSE)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::Blit Call not yet implemented"); return false ; };


        void Clear()
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::Clear() Call not implemented \nNot sensible for an output file?"); return ; };

        void CrossHair(wxCoord x, wxCoord y)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::Clear()  Call not yet implemented"); return ; };

        void DestroyClippingRegion()
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::void Call not yet implemented"); return ; };

        wxCoord DeviceToLogicalX(wxCoord x)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::DeviceToLogicalX Call not yet implemented"); return x; };

        wxCoord DeviceToLogicalXRel(wxCoord x)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::DeviceToLogicalXRel Call not yet implemented"); return x; };

        wxCoord DeviceToLogicalY(wxCoord y)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::DeviceToLogicalY Call not yet implemented"); return y; };

        wxCoord DeviceToLogicalYRel(wxCoord y)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::DeviceToLogicalYRel Call not yet implemented"); return y ; };

        void DrawBitmap(const wxBitmap& bitmap, wxCoord x, wxCoord y, bool transparent)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::DrawBitmap Call not yet implemented"); return ; };

        void DoDrawCheckMark(wxCoord x, wxCoord y, wxCoord width, wxCoord height) ;

        void DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height) ;

        void DoDrawEllipticArc(wxCoord x,wxCoord y,wxCoord w,wxCoord h,double sa,double ea) ;

        void DoDrawLine (wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)  ;

        void DoDrawPolygon(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset,int fillStyle) ;

        void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height) ;

        void DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius = 20)  ;

        void DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle) ;

        void DoDrawText(const wxString& text, wxCoord x, wxCoord y);

        void DoGetTextExtent(const wxString& string, wxCoord *w, wxCoord *h,
            wxCoord *descent = NULL, wxCoord *externalLeading = NULL, wxFont *font = NULL) const ;

        void DrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::DrawIcon Call not yet implemented"); return ; };

        void DoDrawLines(int n, wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0) ;

        void EndDoc()
            { return ; };

        void EndDrawing()
            { return ; };

        void EndPage()
            {  return ; };

        void FloodFill(wxCoord x, wxCoord y, wxColour *colour, int style=wxFLOOD_SURFACE)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::FloodFill Call not yet implemented"); return ; };

        wxCoord GetCharHeight() const;

        wxCoord GetCharWidth() const;

        void GetClippingBox(wxCoord *x, wxCoord *y, wxCoord *width, wxCoord *height)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::GetClippingBox Call not yet implemented"); return ; };

        int GetLogicalFunction()
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::GetLogicalFunction()  Call not yet implemented"); return wxCOPY ; };

        int GetMapMode()
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::GetMapMode() Call not yet implemented"); return wxMM_TEXT; };

        bool GetOptimization()
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::GetOptimization()  Call not yet implemented"); return TRUE ; };

        bool GetPixel(wxCoord x, wxCoord y, wxColour *colour)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::GetPen()  Call not yet implemented"); return FALSE; };

        void GetUserScale(double *x, double *y) const
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::GetUserScale()  Call not yet implemented"); return ; };

        wxCoord LogicalToDeviceX(wxCoord x)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::LogicalToDeviceX Call not yet implemented"); return x ; };

        wxCoord LogicalToDeviceXRel(wxCoord x)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::LogicalToDeviceXRel Call not yet implemented"); return x; };

        wxCoord LogicalToDeviceY(wxCoord y)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::LogicalToDeviceY Call not yet implemented"); return y; };

        wxCoord LogicalToDeviceYRel(wxCoord y)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::LogicalToDeviceYRel Call not yet implemented"); return y; };

        //void ResetBoundingBox() ;;

        void SetDeviceOrigin(wxCoord x, wxCoord y)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::SetDeviceOrigin()  Call not yet implemented"); return ; };


        void SetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::SetClippingRegion Call not yet implemented"); return ; };

        void SetPalette(const wxPalette& palette)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::SetPalette Call not yet implemented"); return ; };

	void SetBackground( const wxBrush &brush ) ;

	void SetBackgroundMode( int mode ) ;

        void SetBrush(const wxBrush& brush) ;

        void SetFont(const wxFont& font) ;

        void SetLogicalFunction(int function)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::SetLogicalFunction Call not yet implemented"); return ; };

        void SetMapMode(int anint)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::SetMapMode Call not yet implemented"); return ; };

        void SetOptimization(bool optimize) { return ; };

        void SetPen(const wxPen& pen)  ;

        void SetUserScale(double xScale, double yScale)
            { wxASSERT_MSG (FALSE, "wxSVGFILEDC::SetUserScale Call not yet implemented"); return ; };

        bool StartDoc(const wxString& message)
            {  return FALSE; };

        void StartPage()
            {  return ; };

};
#endif
