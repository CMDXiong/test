/*
Program			GRIDDLG.CPP
Purpose        The dialog to specify the grid values
Programmer		M. Reemst
Last Update		23-03-98
*/


#ifdef __GNUG__
#pragma implementation
#endif

#include "chandbas.h"
#include "mainwin.h"
#include "string.h"
#include "gdsapp.h"
#include "griddlg.h"

const int    GRID_BUTTON_OK     = wxID_HIGHEST + 4601 ;
const int    GRID_BUTTON_CANCEL = wxID_HIGHEST + 4602 ;
const int    GRID_EDIT_CHANGED  = wxID_HIGHEST + 4604 ;
const int    GRID_G_BITB		  = wxID_HIGHEST + 4605 ;
const int    GRID_B_BITB		  = wxID_HIGHEST + 4606 ;


BEGIN_EVENT_TABLE(Gridsettings, wxDialog)
  EVT_BUTTON    (GRID_BUTTON_OK,      Gridsettings::CmOk)
  EVT_BUTTON    (GRID_BUTTON_CANCEL,  Gridsettings::CmCancel)
  EVT_BUTTON    (GRID_G_BITB,  		  Gridsettings::Cm_g_Bitb)
  EVT_BUTTON    (GRID_B_BITB,  		  Gridsettings::Cm_b_Bitb)
  EVT_CLOSE     (Gridsettings::OnCloseWindow)
  EVT_CHAR		 (Gridsettings::OnChar)
  EVT_CHAR_HOOK (Gridsettings::OnCharHook)
END_EVENT_TABLE()


Gridsettings::Gridsettings(GDSMainWindow* parent,long style):
		wxDialog(parent,-1,"Grid settings",wxPoint(0,0),wxSize(160,270),style,"Gridsettings dialog")
{

	_choices[0] = "pm";
	_choices[1] = "nm";
	_choices[2] = "um";
	_choices[3] = "mm";
	_choices[4] = "cm";
	_choices[5] = "dm";
	_choices[6] = "m";
	_choices[7] = "inch";
	_choices[8] = "foot";

	_parent = parent;
   _panel 	= new wxPanel(this,-1);

   _units_list = new wxChoice(_panel,-1,wxDefaultPosition,wxDefaultSize,9,_choices);

	wxColour	tmp("WHITE");

   _g_brush 	= new wxBrush(); // must be done.
   _b_brush 	= new wxBrush(); // must be done.
	_cdata 		= new wxColourData();

   _g_brush->SetColour(tmp);
   _b_brush->SetColour(tmp);


	// no standard colours
	// set extra part of dialog true
	_cdata->SetChooseFull(TRUE);
	// make colour dialog
	_cd = new wxColourDialog(this,_cdata);


  	wxMemoryDC dcmemtemp;
   _b_bitm    = new wxBitmap(20,20,-1);
   _g_bitm    = new wxBitmap(20,20,-1);
   Redraw();

	_ok		= new wxButton(_panel,GRID_BUTTON_OK,"Ok");
	_cancel	= new	wxButton(_panel,GRID_BUTTON_CANCEL,"Cancel");

   _ed_space   = new wxTextCtrl(_panel,-1,"ok");

   _m_colour	= new wxStaticText(_panel,-1,"colour" );
	_g_bitb	   = new	wxBitmapButton(_panel,GRID_G_BITB,*_g_bitm);

   _m_bcolour	= new wxStaticText(_panel,-1,"background colour");
	_b_bitb	   = new	wxBitmapButton(_panel,GRID_B_BITB,*_b_bitm);

   _colour_box = new wxStaticBox(_panel,-1,"Colour");
   _space_box  = new wxStaticBox(_panel,-1,"Spacing");

	wxString radio_strings[2];
	radio_strings[0] = "background";
   radio_strings[1] = "foreground";
	_ed_backORfore	= new wxRadioBox(_panel,-1,"Placement",wxDefaultPosition,wxDefaultSize,2,radio_strings,1,wxHORIZONTAL);

   *_space = 0;
   _gcolour = new wxColour(0,0,0);
   _bcolour = new wxColour(0,0,0);

   this->SetAutoLayout(TRUE);

   // Set constraints for panel subwindow
   wxLayoutConstraints *c1 = new wxLayoutConstraints;
   c1->left.SameAs       (this, wxLeft,1);
   c1->top.SameAs        (this, wxTop,1);
   c1->right.SameAs      (this, wxRight, 1);
   c1->bottom.SameAs     (this, wxBottom, 1);
   _panel->SetConstraints(c1);


//Buttons

   //ok button
   wxLayoutConstraints *c8 = new wxLayoutConstraints;
   c8->left.SameAs		 (_panel, wxLeft,5);
   c8->bottom.SameAs		 (_panel, wxBottom,5);
   c8->height.AsIs();
   c8->width.AsIs();
   _ok->SetConstraints(c8);

   //cancel button
   wxLayoutConstraints *c9 = new wxLayoutConstraints;
   c9->left.SameAs		 (_ok, wxRight,5);
   c9->bottom.SameAs		 (_panel, wxBottom,5);
   c9->height.AsIs();
   c9->width.AsIs();
   _cancel->SetConstraints(c9);


//Colour

   //Colourbox
   wxLayoutConstraints *c11 = new wxLayoutConstraints;
   c11->left.SameAs     (_panel, wxLeft, 5);
   c11->top.SameAs      (_space_box, wxBottom, 5);
   c11->right.SameAs    (_panel, wxRight, 5);
   _colour_box->SetSize(50,90);
   c11->height.AsIs();
   _colour_box->SetConstraints(c11);

	//Background
   wxLayoutConstraints *c7 = new wxLayoutConstraints;
   c7->left.SameAs       (_colour_box, wxLeft, 5);
   c7->top.SameAs        (_colour_box, wxTop, 20);
   c7->height.AsIs();
   c7->width.AsIs();
   _m_bcolour->SetConstraints(c7);

   wxLayoutConstraints *c15 = new wxLayoutConstraints;
   c15->left.SameAs       (_m_bcolour,wxRight, 5);
   c15->top.SameAs        (_colour_box, wxTop, 20);
   c15->height.AsIs();
   c15->width.AsIs();
   _b_bitb->SetConstraints(c15);

   //Grid
   wxLayoutConstraints *c3 = new wxLayoutConstraints;
   c3->left.SameAs       (_colour_box, wxLeft, 5);
   c3->top.SameAs        (_b_bitb, wxBottom, 5);
   c3->height.AsIs();
   c3->width.AsIs();
   _m_colour->SetConstraints(c3);

   wxLayoutConstraints *c5 = new wxLayoutConstraints;
   c5->left.SameAs       (_m_bcolour,wxRight, 5);
   c5->top.SameAs        (_b_bitb, wxBottom, 5);
   c5->height.AsIs();
   c5->width.AsIs();
   _g_bitb->SetConstraints(c5);


//spacing

	wxLayoutConstraints *c10 = new wxLayoutConstraints;
   c10->left.SameAs     (_panel, wxLeft, 5);
   c10->top.SameAs      (_panel, wxTop, 5);
   c10->right.SameAs    (_panel, wxRight, 5);
   _space_box->SetSize(50,40);
   c10->height.AsIs();
   _space_box->SetConstraints(c10);

   wxLayoutConstraints *c21 = new wxLayoutConstraints;
   c21->top.SameAs      (_space_box, wxTop, 15);
   c21->right.SameAs    (_space_box, wxRight, 5);
   c21->height.AsIs();
   c21->width.AsIs();
   _units_list->SetConstraints(c21);

   wxLayoutConstraints *c4 = new wxLayoutConstraints;
   c4->left.SameAs      (_space_box, wxLeft, 5);
   c4->top.SameAs       (_space_box, wxTop, 15);
   c4->right.SameAs     (_units_list, wxLeft, 5);
   c4->height.AsIs();
   _ed_space->SetConstraints(c4);


//Placement

   //radio
   wxLayoutConstraints *c6 = new wxLayoutConstraints;
   c6->left.SameAs       (_panel,wxLeft, 5);
   c6->top.SameAs        (_colour_box, wxBottom, 5);
   c6->height.AsIs();
   c6->width.AsIs();
   _ed_backORfore->SetConstraints(c6);

   Layout();

}



Gridsettings::~Gridsettings()
{
	delete _bcolour;
	delete _b_brush;
	delete _g_brush;
	delete _gcolour;
	delete _cdata;
   delete _g_bitm;
   delete _b_bitm;

   delete _cd;
}

void Gridsettings::SetupWindow()
{
   Show(TRUE);

   wxString unit=GLOB->CMDH->GetGridSpace().GetMultiplierString();
	if (unit.Len()==0)
      unit="m"; //set it to meters

	_units_list->SetStringSelection(unit);

   Set_Colour_Grid(GLOB->CMDH->GetGridColour());
   Set_Colour_Back(GLOB->CMDH->GetBackColour());

   _ed_space->SetValue(GLOB->CMDH->GetGridSpace().GetNumberString());
	_ed_backORfore->SetSelection(GLOB->CMDH->GetGridPlacement());
   Centre(wxBOTH);
   Redraw();
   SetFocus();
}

// respond to the ok button
void Gridsettings::CmOk(wxCommandEvent&)
{
   Show(FALSE);

   try{
      unsigned char red;
      unsigned char green;
      unsigned char blue;
      blue  = _gcolour->Blue();
      green = _gcolour->Green();
      red   = _gcolour->Red();
		char buf[30];
		strcpy(buf,_ed_space->GetValue());
      strcat(buf," ");
      strcat(buf,_units_list->GetStringSelection() );
      GLOB->CMDH->DoCommand("gridspace  {%s}", buf);
      GLOB->CMDH->DoCommand("gridcolour  %ld %ld %ld", red, green, blue);
      GLOB->CMDH->DoCommand("gridplacement  %i", _ed_backORfore->GetSelection());
      blue  = _bcolour->Blue();
      green = _bcolour->Green();
      red   = _bcolour->Red();
      GLOB->CMDH->DoCommand("backgroundcolour  %ld %ld %ld", red, green, blue);
      if (GLOB->CMDH->GetGridVisible())
         GLOB->CMDH->DoCommand("draw");
   }
   catch (GDS_Error& _error) {
      _messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
   }
   catch(...) {
      _messagehandler->info("SOMETHING WENT WRONG leaf program","NON HANDLED ERROR");
      return;
   }
}

void Gridsettings::CmCancel(wxCommandEvent&)
{
	Show(FALSE);
}

void Gridsettings::Cm_b_Bitb(wxCommandEvent&)
{
   if (_cd->ShowModal()==wxID_OK)
   {
      *_cdata = _cd->GetColourData();
      *_bcolour = _cdata->GetColour();
	   _b_brush->SetColour(*_bcolour);
      Redraw();
   }
   SetFocus();
   Show(true);
   Refresh(true);
}

void Gridsettings::Cm_g_Bitb(wxCommandEvent&)
{
   if (_cd->ShowModal()==wxID_OK)
   {
      *_cdata = _cd->GetColourData();
      *_gcolour = _cdata->GetColour();
	   _g_brush->SetColour(*_gcolour);
      Redraw();
   }
   SetFocus();
   Show(true);
   Refresh(true);
}

void Gridsettings::OnCloseWindow(wxCloseEvent& event)
{
	Show(FALSE);
}

void Gridsettings::Set_Colour_Grid(long colour)
{
	unsigned char	red,green,blue;

	blue 	= colour / 65536;
	green = (colour % 65536) / 256;
	red 	= colour % 256;

	_gcolour->Set(red,green,blue);
   _g_brush->SetColour(*_gcolour);
}

void Gridsettings::Set_Colour_Back(long colour)
{
	unsigned char	red,green,blue;

	blue 	= colour / 65536;
	green = (colour % 65536) / 256;
	red 	= colour % 256;

	_bcolour->Set(red,green,blue);
   _b_brush->SetColour(*_bcolour);
}

void Gridsettings::Redraw()
{
  	wxMemoryDC dcmemtemp;
   dcmemtemp.SelectObject(*_b_bitm);
	dcmemtemp.SetBackground(*_b_brush);
   dcmemtemp.Clear();
   dcmemtemp.SelectObject(wxNullBitmap);

   dcmemtemp.SelectObject(*_g_bitm);
	dcmemtemp.SetBackground(*_g_brush);
   dcmemtemp.Clear();
   dcmemtemp.SelectObject(wxNullBitmap);
}

void Gridsettings::OnChar(wxKeyEvent& event)
{
    event.Skip();
}

void Gridsettings::OnCharHook(wxKeyEvent& event)
{
    event.Skip();
}
