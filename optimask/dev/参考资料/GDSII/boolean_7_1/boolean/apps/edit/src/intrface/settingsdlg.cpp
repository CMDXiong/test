/*
Program			SETTINGSDLG.CPP
Purpose        Dialog to specify the path- and width values
Programmer		A. Rona
Last Update		31-08-99
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "chandbas.h"
#include "mainwin.h"
#include "string.h"
#include "gdsapp.h"
#include "tool.h"

#include "settingsdlg.h"

const int    SETTINGS_BUTTON_OK     = wxID_HIGHEST + 5211 ;
const int	 SETTINGS_BUTTON_APPLY	= wxID_HIGHEST + 5212 ;
const int    RADIO_BUTTON_ID        = wxID_HIGHEST + 5213 ;

BEGIN_EVENT_TABLE(SettingsDialog, wxDialog)
  EVT_BUTTON    (SETTINGS_BUTTON_OK,      SettingsDialog::CmOk)
  EVT_BUTTON    (SETTINGS_BUTTON_APPLY,   SettingsDialog::CmApply)
  EVT_CLOSE     (SettingsDialog::OnCloseWindow)
  EVT_RADIOBOX	 (RADIO_BUTTON_ID, SettingsDialog::CmRadioButton)
END_EVENT_TABLE()


SettingsDialog::SettingsDialog(GDSMainWindow* parent, long style):
		wxDialog(parent,-1,"Settings",wxPoint(0,0),wxSize(200,215),style,"SetPath dialog")
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

	_panel 			= new wxPanel(this,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER ,"Settings");

   _settings_list = new wxChoice(_panel,-1,wxDefaultPosition,wxDefaultSize,9,_choices);
   _width			= new wxTextCtrl(_panel,-1,"1");
	_widthbox		= new wxStaticBox(_panel,-1,"Width");
	_ok		  		= new wxButton(_panel,SETTINGS_BUTTON_OK,"Hide");
   _apply         = new wxButton(_panel,SETTINGS_BUTTON_APPLY,"Apply");
	_spline 			= new wxCheckBox(_panel,-1,"Spline",wxDefaultPosition,wxSize(180,20));


	wxString radio_strings[3];
	radio_strings[0] = "square";
	radio_strings[1] = "rounded";
   radio_strings[2] = "rectangular";
	_ed_setpath 	= new wxRadioBox(_panel,RADIO_BUTTON_ID,"Pathtype",wxDefaultPosition,wxDefaultSize,3,radio_strings,1,wxRA_HORIZONTAL,wxDefaultValidator,"Set Pathtype");


   this->SetAutoLayout(TRUE);
   wxLayoutConstraints *c1 = new wxLayoutConstraints;
   c1->left.SameAs       (this, wxLeft,1);
   c1->top.SameAs        (this, wxTop,1);
   c1->right.SameAs      (this, wxRight, 1);
   c1->bottom.SameAs     (this, wxBottom, 1);
   _panel->SetConstraints(c1);


//Width
   wxLayoutConstraints *c4 = new wxLayoutConstraints;
   c4->left.SameAs       (_panel, wxLeft, 5);
   c4->top.SameAs     (_panel, wxTop, 5);
   c4->right.SameAs    (_panel, wxRight, 5);
   _widthbox->SetSize(100,45);
   c4->height.AsIs();
   _widthbox->SetConstraints(c4);

   wxLayoutConstraints *c20 = new wxLayoutConstraints;
   c20->left.SameAs     (_widthbox, wxLeft, 5);
   c20->top.SameAs      (_widthbox, wxTop, 15);
   c20->right.SameAs    (_widthbox, wxRight, 15);
   _width->SetSize(80,20);
   c20->height.AsIs();
   c20->width.AsIs();
   _width->SetConstraints(c20);

   wxLayoutConstraints *c21 = new wxLayoutConstraints;
   c21->top.SameAs      (_widthbox, wxTop, 15);
   c21->right.SameAs    (_widthbox, wxRight, 5);
   c21->height.AsIs();
   c21->width.AsIs();
   _settings_list->SetConstraints(c21);


//Pathtype
   wxLayoutConstraints *c16 = new wxLayoutConstraints;
   c16->left.SameAs     (_panel, wxLeft, 5);
   c16->top.SameAs      (_widthbox, wxBottom, 5);
   c16->height.AsIs();
   c16->right.SameAs    (_panel, wxRight, 5);
	 _ed_setpath->SetConstraints(c16);

   wxLayoutConstraints *c8 = new wxLayoutConstraints;
   c8->left.SameAs       (_ok, wxRight, 5);
   c8->top.SameAs     	 (_ed_setpath, wxBottom, 5);
   c8->height.AsIs();
   c8->width.AsIs();
   _spline->SetConstraints(c8);

//Buttons
   wxLayoutConstraints *c6 = new wxLayoutConstraints;
   c6->left.SameAs       (_panel, wxLeft, 5);
   c6->top.SameAs     (_spline, wxBottom, 5);
   c6->height.AsIs();
   c6->width.AsIs();
   _ok->SetConstraints(c6);

   wxLayoutConstraints *c7 = new wxLayoutConstraints;
   c7->left.SameAs       (_ok, wxRight, 5);
   c7->top.SameAs     	 (_spline, wxBottom, 5);
   c7->height.AsIs();
   c7->width.AsIs();
   _apply->SetConstraints(c7);

   Layout();

}


void SettingsDialog::SetupWindow()
{
   wxString unit=GLOB->CMDH->GetWidth().GetMultiplierString();
	if (unit.Len()==0)
      unit="m"; //set it to meters

	_settings_list->SetStringSelection(unit);

   _width->SetValue(GLOB->CMDH->GetWidth().GetNumberString());
	Show(TRUE);
	Centre(wxBOTH);
//   _parent->Redraw();
}

void SettingsDialog::CmOk(wxCommandEvent &)
{
	Show( FALSE);
}

void SettingsDialog::CmApply(wxCommandEvent &)
{
	Show(TRUE);

   try{
        char help[30];
		strcpy(help,_width->GetValue());
  		strcat(help,_settings_list->GetStringSelection());
		GLOB->CMDH->DoCommand("setwidth {%s}", help);

		if ((bool)_spline->GetValue())
			GLOB->CMDH->DoCommand("setspline 1");
      else
			GLOB->CMDH->DoCommand("setspline 0");

      int y =_ed_setpath->GetSelection();
      GLOB->CMDH->DoCommand("setpathtype %d",y);

//  		_parent->Redraw();
   }
   catch (GDS_Error& _error) {
      _messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
   }
   catch(...) {
      _messagehandler->info("SOMETHING WENT WRONG leaf program","NON HANDLED ERROR");
      return;
   }
}

void SettingsDialog::CmRadioButton(wxCommandEvent & event)
{
}

void SettingsDialog::OnCloseWindow(wxCloseEvent& event)
{
	Show(FALSE);
}
