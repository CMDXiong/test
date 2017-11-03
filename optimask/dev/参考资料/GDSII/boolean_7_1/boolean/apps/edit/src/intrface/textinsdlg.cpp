/*
Program			textinsdlg.CPP
Purpose        Dialog to insert/edit text
Programmer		A. Rona
Last Update		17-09-99
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "wxwin.h"

#include	<wx/textdlg.h>
#include <wx/fontdlg.h>
#include <wx/font.h>
#include <wx/colordlg.h>

#if defined(__WXMSW__) && wxTEST_GENERIC_DIALOGS_IN_MSW
#include <wx/generic/fontdlgg.h>
#include <wx/generic/colrdlgg.h>
#endif

#include "chandbas.h"
#include "mainwin.h"
#include "string.h"
#include "gdsapp.h"
#include "text.h"
#include "toolcont.h"
#include "tool.h"
#include "elemlst.h"
#include "command.h"

#include "textinsdlg.h"

const int    TEXT_BUTTON_OK      = wxID_HIGHEST + 5207 ;
const int	 FONT_BUTTON		   = wxID_HIGHEST + 5208 ;
const int    FONT_GENERIC_BUTTON = wxID_HIGHEST + 5209 ;

BEGIN_EVENT_TABLE(TextinsDialog, wxDialog)
  EVT_CLOSE     (TextinsDialog::OnCloseWindow)
  EVT_BUTTON    (FONT_BUTTON,   TextinsDialog::ChooseFont)
#if defined(__WXMSW__) && wxTEST_GENERIC_DIALOGS_IN_MSW
  EVT_BUTTON    (FONT_GENERIC_BUTTON,   TextinsDialog::ChooseFontGeneric)
#endif
END_EVENT_TABLE()


TextinsDialog::TextinsDialog(GDSMainWindow* parent):
		wxDialog(parent,-1,"Edit Text",wxPoint(0,0),wxSize(295,115),wxTHICK_FRAME | wxSYSTEM_MENU | wxCAPTION,"Textinsdialog")
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

	_parent 				= parent;
	_panel 				= new wxPanel(this,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER ,"Settings");
   _textbox       	= new wxStaticBox(_panel,-1,"text");
   _heightbox     	= new wxStaticBox(_panel,-1,"height");
	_ok		  			= new wxButton(_panel, wxID_OK,"Ok ");
   _cancel		  		= new wxButton(_panel, wxID_CANCEL,"Cancel ");
   _font_button      = new wxButton(_panel, FONT_BUTTON,"Font");
#if defined(__WXMSW__) && wxTEST_GENERIC_DIALOGS_IN_MSW
   _font_generic     = new wxButton(_panel, FONT_GENERIC_BUTTON,"FontGeneric");
#endif
   _text_insert		= new wxTextCtrl(_panel,-1);
   _text_height		= new wxTextCtrl(_panel,-1);
   _text_units    	= new wxChoice(_panel,-1,wxDefaultPosition,wxDefaultSize,9,_choices);
   _font					= new wxFont(12, wxROMAN , wxNORMAL, wxNORMAL, FALSE, "testfont");
 	_text_colour 		= wxColour("BLACK");
   _text_font  		= *wxNORMAL_FONT;


   this->SetAutoLayout(TRUE);
   wxLayoutConstraints *c1 = new wxLayoutConstraints;
   c1->left.SameAs       (this, wxLeft,1);
   c1->top.SameAs        (this, wxTop,1);
   c1->right.SameAs      (this, wxRight, 1);
   c1->bottom.SameAs     (this, wxBottom, 1);
   _panel->SetConstraints(c1);


//Insert Text
   wxLayoutConstraints *c2 = new wxLayoutConstraints;
   c2->left.SameAs       (_panel, wxLeft, 5);
   c2->top.SameAs     	 (_panel, wxTop, 5);
   _textbox->SetSize(150,50) ;
   c2->height.AsIs();
   c2->width.AsIs();
   _textbox->SetConstraints(c2);

   wxLayoutConstraints *c3 = new wxLayoutConstraints;
   c3->left.SameAs       (_textbox, wxLeft, 5);
   c3->top.SameAs        (_textbox, wxTop, 15);
   c3->right.SameAs      (_textbox, wxRight, 5);
   _text_insert->SetSize(60,20);
   c3->height.AsIs();
   _text_insert->SetConstraints(c3);

//Text height
	wxLayoutConstraints *c4 = new wxLayoutConstraints;
   c4->left.SameAs       (_textbox, wxRight, 5);
   c4->top.SameAs     	 (_panel, wxTop, 5);
   c4->height.AsIs();
   c4->width.AsIs();
   _heightbox->SetSize(120,50) ;
   _heightbox->SetConstraints(c4);

   wxLayoutConstraints *c5 = new wxLayoutConstraints;
   c5->left.SameAs       (_heightbox, wxLeft, 5);
   c5->top.SameAs        (_heightbox, wxTop, 15);
   _text_height->SetSize(60,20);
   c5->height.AsIs();
   c5->width.AsIs();
   _text_height->SetConstraints(c5);


   wxLayoutConstraints *c6 = new wxLayoutConstraints;
   c6->left.SameAs       (_text_height, wxRight, 5);
   c6->top.SameAs        (_textbox, wxTop, 15);
   _text_units->SetSize(60,20);
   c6->height.AsIs();
   c6->width.AsIs();
   _text_units->SetConstraints(c6);

//Buttons
   wxLayoutConstraints *c7 = new wxLayoutConstraints;
   c7->left.SameAs       (_panel, wxLeft, 5);
   c7->bottom.SameAs     (_panel, wxBottom, 5);
   c7->height.AsIs();
   c7->width.AsIs();
   _ok->SetConstraints(c7);

   wxLayoutConstraints *c8 = new wxLayoutConstraints;
   c8->left.SameAs       (_ok, wxRight, 5);
   c8->bottom.SameAs     (_panel, wxBottom, 5);
   c8->height.AsIs();
   c8->width.AsIs();
   _cancel->SetConstraints(c8);

   wxLayoutConstraints *c9 = new wxLayoutConstraints;
   c9->left.SameAs       (_cancel, wxRight, 5);
   c9->bottom.SameAs     (_panel, wxBottom, 5);
   c9->height.AsIs();
   c9->width.AsIs();
   _font_button->SetConstraints(c9);

#if defined(__WXMSW__) && wxTEST_GENERIC_DIALOGS_IN_MSW
   wxLayoutConstraints *c10 = new wxLayoutConstraints;
   c10->left.SameAs       (_cancel, wxRight, 5);
   c10->bottom.SameAs     (_panel, wxBottom, 5);
   c10->height.AsIs();
   c10->width.AsIs();
   _font_generic->SetConstraints(c10);
#endif

	Centre(wxBOTH);
	_units="um";

   Layout();

}

void TextinsDialog::ChooseFont(wxCommandEvent& WXUNUSED(event) )
{
  wxFontData data;
  data.SetInitialFont(_text_font);
  data.SetColour(_text_colour);

  wxFontDialog dialog(this, data);
  if (dialog.ShowModal() == wxID_OK)
  {
    wxFontData retData 	= dialog.GetFontData();
    _text_font 			= retData.GetChosenFont();
    _text_colour 			= retData.GetColour();
    _font					= new wxFont(_text_font);
    _width 					= _font->GetPointSize();
//    _width              = _font->GetFontId();
  }
}

#if defined(__WXMSW__) && wxTEST_GENERIC_DIALOGS_IN_MSW
void TextinsDialog::ChooseFontGeneric(wxCommandEvent& WXUNUSED(event) )
{
  wxFontData data;
  data.SetInitialFont(_text_font);
  data.SetColour(_text_colour);

  wxGenericFontDialog dialog(this, &data);
  if (dialog.ShowModal() == wxID_OK)
  {
    wxFontData retData  = dialog.GetFontData();
    _text_font 			= retData.GetChosenFont();
    _text_colour 			= retData.GetColour();

/*
    _dc->SetFont(const wxFont& _text_font);
    strcpy(_text, GetText);
//    GetTextExtent(const wxString& _text, long *w, long *h)
    strcpy(_width, _dc->GetCharWidth());
*/
  }
}
#endif

wxString TextinsDialog::GetText()
{
	_text=_text_insert->GetValue();
   return _text;
}


void TextinsDialog::SetText(const wxString& text)
{
	_text_insert->SetValue(text);
}


double TextinsDialog::GetHeight()
{
   wxUnit height(_text_height->GetValue(),_text_units->GetStringSelection());

   double heightnum=height.GetValue();

   heightnum = heightnum / GLOB->PHYSUNITS();
   return heightnum;
}


void TextinsDialog::SetUnit(const wxString& buf)
{
	_text_units->SetStringSelection(buf);
	_units=_text_units->GetStringSelection();
}

wxString TextinsDialog::GetUnit()
{
	_units=_text_units->GetStringSelection();
	return _units;
}


void TextinsDialog::SetHeight(double the_height)
{
   the_height = the_height * GLOB->PHYSUNITS();
   wxUnit height(the_height,_units);
	_text_height->SetValue(height.GetNumberString());
}


// respond to the ok button
void TextinsDialog::CmOk(wxEvent& event)
{
	EndModal(event.GetId());
}


void TextinsDialog::OnCloseWindow(wxCloseEvent& event)
{
	EndModal(TRUE);
}





