/*
Program	PATTERNS.CPP
Purpose	Sets the patterndialog on the screen
		This class is a typical Windows class
	05-11-97 This class has been changed. it now creates a dialog for the patterns
   	that can be used with wxWindows. All the OWL functions have been removed.
Programmers	R. Spekreijse & Chris Mollien
Created	late 1995
Last Update	05-11-1997
*/

#ifdef __GNUG__
#pragma implementation
#endif
#include "patdlg.h"
#include "units.h"

#define PAT_COLLUMNS 6
#define PAT_ROWS 6
#define PAT_ROWS_VIS 6

   #define PD_PANEL1_X	 0
   #define PD_PANEL1_Y   0
   #define PD_PANEL1_W   (PAT_COLLUMNS*40 + 20)
   #define PD_PANEL1_H   (PAT_ROWS_VIS*40 )

   #define PD_BUTTON_W 32
   #define PD_BUTTON_H 32


BEGIN_EVENT_TABLE(PatternDialog, wxDialog)
  EVT_CUSTOM_RANGE(wxEVT_COMMAND_BUTTON_CLICKED, 800, 800+NR_PATTERNS, PatternDialog::Cm_P_BitB)

  EVT_CLOSE    (PatternDialog::OnCloseWindow)

END_EVENT_TABLE()



// the contructor calls it's parent
PatternDialog::PatternDialog(LayerPropertiesDialog* parent,char* title,long style,char* name):
		wxDialog(parent,-1,title,wxPoint(0,0),wxSize(PD_PANEL1_W,PD_PANEL1_H),style,name)
{
	_parent = parent;

   _choosen=0;

   _panel1 = new wxPanel(this,-1,wxPoint(PD_PANEL1_X,PD_PANEL1_Y),wxSize(PD_PANEL1_W,PD_PANEL1_H));
   _scroll    = new wxScrolledWindow(_panel1,-1,wxPoint(PD_PANEL1_X,PD_PANEL1_Y),wxSize(PD_PANEL1_W,PD_PANEL1_H), wxVSCROLL,"scrolledWindow");
   _scroll->SetScrollbars( 40, 40,6,12 );


	for (int i = 0; i < NR_PATTERNS; i++)
   {
		wxMemoryDC dcmemtemp2;
		wxColour white("WHITE");
		wxColour black("BLACK");

	  	wxBitmap help = wxBitmap(32,32,-1);
      dcmemtemp2.SelectObject(help);
      wxBrush _brushbitm(*(GLOB->FILLPAT->GetPattern(i)));
      dcmemtemp2.SetBrush(_brushbitm);

      //convert monogrome to color bitmap
      //in WinNt any monochrome bitmapbrush is drawn to the dest. dc back and fore ground colors
	   dcmemtemp2.SetTextForeground(white);
	   dcmemtemp2.SetTextBackground(black);
      dcmemtemp2.DrawRectangle(0,0,32,32);
   	dcmemtemp2.SelectObject(wxNullBitmap);

		_patternbut[i]		= new	wxBitmapButton(_scroll,800+i,help,
	     						  wxPoint(((i%PAT_COLLUMNS)*40),((i / PAT_ROWS) *40)));
//	     						  wxPoint(PD_BUTTON_X  + ((i%PAT_COLLUMNS)*40),PD_BUTTON_Y + ((i / PAT_ROWS) *40)));
   }
   Fit();
	Center(wxBOTH);
   Redraw();
}

// free used memory
PatternDialog::~PatternDialog()
{
}

// draw each pattern in a userbutton
void PatternDialog::Redraw()
{
}

wxColour* PatternDialog::ltowxc(long colour)
{
	unsigned char	red,green,blue;

	blue 	= colour / 65536;
	green = (colour % 65536) / 256;
	red 	= colour % 256;

	return new wxColour(red,green,blue);
}

void PatternDialog::Cm_P_BitB( wxEvent& event)
{
   _choosen=event.GetId()-800;
   EndModal(_choosen);
}

void PatternDialog::OnCloseWindow(wxCloseEvent& event)
{
}
