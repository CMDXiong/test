/*
Program			UNITSDLG.CPP
Purpose        The dialog to specify the UNITS values
Programmer		M. Reemst
last modified  A.Rona
Last Update		30-08-99
*/

#ifdef __GNUG__
#pragma implementation
#endif
#include "chandbas.h"
#include "mainwin.h"
//#include "string.h"

#include "unitsdlg.h"

const int   ID_UNIT_OK     = wxID_HIGHEST + 4501 ;
const int   ID_UNIT_CANCEL = wxID_HIGHEST + 4502 ;

BEGIN_EVENT_TABLE(Unitssettings, wxDialog)
  EVT_BUTTON    (ID_UNIT_OK,      Unitssettings::CmOk)
  EVT_CLOSE     (Unitssettings::OnCloseWindow)
END_EVENT_TABLE()

Unitssettings::Unitssettings(GDSMainWindow* parent,long style):
		wxDialog(parent,-1,"Units settings",wxPoint(0,0),wxSize(150,105),style,"UnitsSettings dialog")
{
	_choices[0] = "pm";
	_choices[1] = "nm";
	_choices[2] = "um";
	_choices[3] = "mm";
	_choices[4] = "cm";
	_choices[5] = "dm";
	_choices[6] = "m";
	_choices[7] = "mil";
	_choices[8] = "inch";
	_choices[9] = "foot";

	_parent = parent;

	_panel 		= new wxPanel(this,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER ,"UnitsDialog");
	_ok			= new wxButton(_panel,ID_UNIT_OK,"Ok");
   _accbox     = new wxStaticBox(_panel,-1,"accuracy");

   _units_list = new wxChoice(_panel,-1,wxDefaultPosition,wxDefaultSize,10,_choices);
   _ed_acc   	= new wxTextCtrl(_panel,-1,"acc");

   *_acc = 0;
   *_unit = 0;

   this->SetAutoLayout(TRUE);
   wxLayoutConstraints *c1 = new wxLayoutConstraints;
   c1->left.SameAs       (this, wxLeft, 1);
   c1->top.SameAs        (this, wxTop, 1);
   c1->right.SameAs      (this, wxRight, 1);
   c1->bottom.SameAs     (this, wxBottom, 1);
   _panel->SetConstraints(c1);


//Accuracy

	wxLayoutConstraints *c2 = new wxLayoutConstraints;
   c2->left.SameAs       (_panel, wxLeft, 5);
   c2->top.SameAs     (_panel, wxTop, 5);
   c2->right.SameAs    (_panel, wxRight, 5);
   _accbox->SetSize(50,40);
   c2->height.AsIs();
   _accbox->SetConstraints(c2);

      wxLayoutConstraints *c3 = new wxLayoutConstraints;
   c3->left.SameAs    (_accbox, wxLeft, 5);
   c3->top.SameAs      (_accbox, wxTop, 15);
   _ed_acc->SetSize(34,20);
   c3->height.AsIs();
   c3->width.AsIs();
   _ed_acc->SetConstraints(c3);

   wxLayoutConstraints *c4 = new wxLayoutConstraints;
   c4->top.SameAs      (_accbox, wxTop, 15);
   c4->left.SameAs    (_ed_acc, wxRight, 5);
   c4->height.AsIs();
   c4->width.AsIs();
   _units_list->SetConstraints(c4);


//Buttons
	wxLayoutConstraints *c5 = new wxLayoutConstraints;
   c5->top.SameAs      (_accbox, wxBottom, 5);
   c5->left.SameAs     (_panel, wxLeft, 5);
   c5->height.AsIs();
   c5->width.AsIs();
   _ok->SetConstraints(c5);

   Layout();

}



Unitssettings::~Unitssettings()
{
}

void Unitssettings::SetupWindow()
{
  	sprintf(_acc,"%f",GLOB->CMDH->GetDefaultAcc());
   _ed_acc->SetValue(_acc);
   _units_list->SetStringSelection(GLOB->CMDH->GetDefaultUnit().GetMultiplierString());
	strcpy(_unit,_units_list->GetStringSelection());
	Centre(wxBOTH);
   //Redraw();
}

// respond to the ok button
void Unitssettings::CmOk(wxCommandEvent &)
{
	EndModal(FALSE);

   try{
        char help[30];
	strcpy(help,_ed_acc->GetValue());
	GLOB->CMDH->DoCommand("defaultaccur  %s", help);
	strcpy(help,_units_list->GetStringSelection());
	GLOB->CMDH->DoCommand("defaultunit  %s", help);
   }
   catch (GDS_Error& _error) {
      _messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
   }
   catch(...) {
      _messagehandler->info("SOMETHING WENT WRONG leaf program","NON HANDLED ERROR");
      return;
   }
}

void Unitssettings::OnCloseWindow(wxCloseEvent&  /*event*/)
{
	EndModal(FALSE);
}
