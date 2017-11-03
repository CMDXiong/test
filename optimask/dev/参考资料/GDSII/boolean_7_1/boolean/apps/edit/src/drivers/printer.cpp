// source	: printer.cpp
#ifdef __GNUG__
#pragma implementation
#endif

#include "wxwin.h"

#include "wx/prntbase.h"
#include "windrv.h"
#include "mainwin.h"
#include "chandbas.h"
#include "printer.h"
#include "structur.h"

#if wxUSE_PRINTING_ARCHITECTURE

using namespace std;

PrintDriver::PrintDriver(GDSMainWindow* parent,char *title) : wxPrintout(title)
{
	_parent = parent;

   _crosshair = false;
}

PrintDriver::~PrintDriver(void)
{
}

bool PrintDriver::OnBeginDocument(int startPage, int endPage)
{
  if (!wxPrintout::OnBeginDocument(startPage, endPage))
	 return FALSE;

  return TRUE;
}

bool PrintDriver::HasPage(int pageNum)
{
  return (pageNum == 1);
}

void PrintDriver::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
  *minPage = 1;
  *maxPage = 1;
  *selPageFrom = 1;
  *selPageTo = 1;
}

bool PrintDriver::OnPrintPage(int)
{
	if (! GLOB->SETUP) return FALSE;

	_dc = GetDC();
	int w,h;
	_dc->GetSize(&w, &h);
	_w = w;
	_h = h;

	_dc->GetSize(&w, &h);
   if (GLOB->CMDH->Get_ZoomHistoryEmpty())
   {
  		GLOB->SCRDRV->SetMapping(w,h,
         GLOB->CMDH->Get_GDSII()->GetShowStructure()->GetBoundingBox().GetMin().GetX(),
         GLOB->CMDH->Get_GDSII()->GetShowStructure()->GetBoundingBox().GetMin().GetY(),
         GLOB->CMDH->Get_GDSII()->GetShowStructure()->GetBoundingBox().GetMax().GetX(),
         GLOB->CMDH->Get_GDSII()->GetShowStructure()->GetBoundingBox().GetMax().GetY(),
         true);
   }
  	else
	{
   	int x,y,mX,mY;
		_parent->GetClientWindowSize(x,y,mX,mY);

      Point	corners[4];
      // corners[0] is left_bottom
      // corners[1] is left_top
      // corners[2] is right_bottom
      // corners[3] is right_top

      corners[0].Set(x, y);
      corners[1].Set(x, mY);
      corners[2].Set(mX, y);
      corners[3].Set(mX, mY);

      // Multiply the points with the relative matrix
      for (int i = 0; i < 4; i++)
      {
         corners[i].Transform(&GLOB->SCRDRV->GetInverseMappingMatrix());
      }
      x = (int) corners[0].GetX();
      y = (int) corners[0].GetY();
      mX= (int) corners[3].GetX();
      mY= (int) corners[3].GetY();

  		GLOB->SCRDRV->SetMapping(w,h,x,y,mX,mY,true);
	}

	double marginX = 25;
	double marginY = 25;

	// Add the margin to the graphic size
	double maxX = w + (2*marginX);
	double maxY = h + (2*marginY);

	double scaleX=(w/maxX);
	double scaleY=(h/maxY);

	double actualScale = wxMin(scaleX,scaleY);

	// Calculate the position on the DC for centring the graphic
	double posX = ((w - (maxX*actualScale))/2.0);
	double posY = ((h - (maxY*actualScale))/2.0);

	wxFont _font = wxFont((int)(h * 0.015),wxDEFAULT,wxNORMAL,wxNORMAL);
	wxColour*	_textColor = new wxColour("BLACK");

	wxString tmp;
	tmp << "file : " << GLOB->CMDH->Get_FileName();

	_dc->SetBackgroundMode(wxTRANSPARENT);
	_dc->SetFont(_font);
	_dc->SetTextForeground(*_textColor);
	_dc->DrawText(PROGRAM_NAME,(w * 0.01),(h * 0.01));
	_dc->DrawText(tmp.c_str(),(w * 0.01),(h * 0.03));
	_dc->SetBackgroundMode(wxSOLID);
//	dc->SetFont(NULL);
   delete _textColor;

	// Set the scale and origin
	_dc->SetUserScale(actualScale, actualScale);
	_dc->SetDeviceOrigin(posX, posY);
	GLOB->SCRDRV->SetDC(_dc);
   GLOB->SCRDRV->Write(GLOB->CMDH->Get_GDSII());

	return TRUE;
}

#endif //wxUSE_PRINTING_ARCHITECTURE







