#ifdef __GNUG__
#pragma implementation
#endif

#include <fstream>

#include "wxwin.h"

#include <assert.h>
#include <stdlib.h>
#include "mainwin.h"
#include "gdsapp.h"
#include "about.h"

#define ABOUT_VSIZE			300
#define ABOUT_HSIZE			300

const int    ABOUT_BUTTON_OK     = wxID_HIGHEST + 5801 ;


BEGIN_EVENT_TABLE(About, wxDialog)
  EVT_BUTTON    (ABOUT_BUTTON_OK,      About::CmOk)

  EVT_CLOSE     (About::OnCloseWindow)

END_EVENT_TABLE()


About::About(GDSMainWindow* parent,char *title,long style,char *name):
	wxDialog(parent,-1,title,wxPoint(0,0),wxSize(ABOUT_HSIZE,ABOUT_VSIZE),style,name)
{
	_parent = parent;

	wxString programmers[13];

	programmers[0] = "Klaas Holwerda";
	programmers[1] = "Faust Nijhuis";
	programmers[2] = "Angelo Starink      (1994)";
	programmers[3] = "Martijn de Haas     (1995)";
	programmers[4] = "Patrick Devick      (1995)";
	programmers[5] = "Remon Spekreijse    (1996)";
	programmers[6] = "Raymond J. Heukers  (1997)";
	programmers[7] = "Marcel P. Sterling  (1997)";
	programmers[8] = "Chris Mollien       (1997)";
	programmers[9] = "Ocker Kerpershoek   (1997)";
	programmers[10]= "Martijn Reemst      (1998)";
	programmers[11]= "Björn Theissen      (1998)";
   wxString msg;
   msg.Printf(wxVERSION_STRING);
	programmers[12]= msg;

	_panel 		= new wxPanel	(this,-1,wxDefaultPosition,wxDefaultSize);
	_lb			= new wxListBox(_panel,-1,wxDefaultPosition,wxDefaultSize,13,programmers,wxLB_MULTIPLE);
	_okButton 	= new wxButton	(_panel,ABOUT_BUTTON_OK,"Ok");
	_text1 		= new wxStaticText(_panel,-1,PROGRAM_NAME);
	_text2 		= new wxStaticText(_panel,-1,"1-5-1999");

   this->SetAutoLayout(TRUE);

   // Set constraints for panel subwindow
   wxLayoutConstraints *c1 = new wxLayoutConstraints;

   c1->left.SameAs       (this, wxLeft,1);

   c1->top.SameAs        (this, wxTop,1);

   c1->right.SameAs      (this, wxRight, 1);

   c1->bottom.SameAs     (this, wxBottom, 1);

   _panel->SetConstraints(c1);



   //program text

   wxLayoutConstraints *c2 = new wxLayoutConstraints;

   c2->left.SameAs       (_panel, wxLeft);

   c2->top.SameAs        (_panel, wxTop);

   c2->height.AsIs();

   c2->width.AsIs();

   _text1->SetConstraints(c2);



   //listbox

   wxLayoutConstraints *c3 = new wxLayoutConstraints;

   c3->left.SameAs       (_panel, wxLeft);

   c3->top.SameAs        (_text1, wxBottom);

   c3->right.LeftOf      (_okButton,2);

   c3->bottom.SameAs     (_text2, wxTop,2);

   _lb->SetConstraints(c3);



   //date text

   wxLayoutConstraints *c4 = new wxLayoutConstraints;

   c4->left.SameAs       (_panel, wxLeft);

   c4->bottom.SameAs     (_panel, wxBottom);

   c4->height.AsIs();

   c4->width.AsIs();

   _text2->SetConstraints(c4);



   //ok button

   wxLayoutConstraints *c8 = new wxLayoutConstraints;

   c8->right.SameAs      (_panel, wxRight,5);

   c8->top.SameAs        (_panel, wxTop,5);

   c8->height.AsIs();

   c8->width.AsIs();

   _okButton->SetConstraints(c8);

   Centre(wxBOTH);


}

About::~About()
{
}

void About::CmOk(wxCommandEvent &)
{
	EndModal(FALSE);
}

void About::OnCloseWindow(wxCloseEvent& event)
{
    wxCommandEvent a = wxCommandEvent();
	CmOk( a );
}

