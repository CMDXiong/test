/*
Program			CIRCLEDITDLG.CPP
Purpose        Dialog to specify radius and unit values
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
#include "units.h"

#include "circleditdlg.h"

const int    SETTINGS_BUTTON_OK     = 5201 ;
const int	 SETTINGS_BUTTON_APPLY	= 5202 ;
const int    RADIO_BUTTON_ID        = 5203 ;
const int    CIRCLE_UPDATE_DIALOG   = 5204 ;

BEGIN_EVENT_TABLE(CircleditDialog, wxDialog)
  EVT_BUTTON    (SETTINGS_BUTTON_OK,      CircleditDialog::CmOk)
  EVT_BUTTON    (wxID_OK,   CircleditDialog::CmApply)
  EVT_CLOSE     (CircleditDialog::OnCloseWindow)
  EVT_UPDATE_UI (CIRCLE_UPDATE_DIALOG, CircleditDialog::Update)
  EVT_SET_FOCUS (CircleditDialog::FocusSet)
  EVT_KILL_FOCUS (CircleditDialog::FocusKill)
  EVT_ACTIVATE(CircleditDialog::OnActivate)

END_EVENT_TABLE()


CircleditDialog::CircleditDialog(EditCircleTool* tool,GDSMainWindow* parent, long style):
		wxDialog(parent,CIRCLE_UPDATE_DIALOG,"Edit Circle",wxPoint(0,0),wxSize(200,115),style,"EditCircle dialog")
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

   _tool=tool;
   _focus=false;

	_parent = parent;
	_panel 			= new wxPanel(this,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER ,"Settings");
   _settings_list = new wxChoice(_panel,-1,wxDefaultPosition,wxDefaultSize,9,_choices);
   _width			= new wxTextCtrl(_panel,-1,"1");
	_widthbox		= new wxStaticBox(_panel,-1,"radius");
	_ok		  		= new wxButton(_panel,SETTINGS_BUTTON_OK,"Hide");
   _apply         = new wxButton(_panel,wxID_OK,"Apply");

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

//Buttons
   wxLayoutConstraints *c6 = new wxLayoutConstraints;
   c6->left.SameAs       (_panel, wxLeft, 5);
   c6->top.SameAs     (_widthbox, wxBottom, 5);
   c6->height.AsIs();
   c6->width.AsIs();
   _ok->SetConstraints(c6);

   wxLayoutConstraints *c7 = new wxLayoutConstraints;
   c7->left.SameAs       (_ok, wxRight, 5);
   c7->top.SameAs     	 (_widthbox, wxBottom, 5);
   c7->height.AsIs();
   c7->width.AsIs();
   _apply->SetConstraints(c7);

   strcpy(_unit,"um");

   Layout();
}

void CircleditDialog::SetupWindow()
{
	_settings_list->SetStringSelection(_tool->GetRadius().GetMultiplierString());

   _width->SetValue(_tool->GetRadius().GetNumberString());
}

void CircleditDialog::SetUnit(char* buf)
{
	_settings_list->SetStringSelection(buf);
	strcpy(_unit,_settings_list->GetStringSelection());
}

double CircleditDialog::GetRadius()
{
 double width;
 width = atof(_width->GetValue());
 return width;
}

char* CircleditDialog::GetUnit()
{
	strcpy(_unit,_settings_list->GetStringSelection());
	return _unit;
}

void CircleditDialog::CmOk(wxCommandEvent &)
{
	MakeModal(false);
	Show(false);
}

void CircleditDialog::CmApply(wxCommandEvent &)
{

   wxUnit help(_width->GetValue(),_settings_list->GetStringSelection());
   _tool->SetRadius(help);
}

void CircleditDialog::OnCloseWindow(wxCloseEvent& event)
{
	MakeModal(false);
	Show(false);
}

void CircleditDialog::Update(wxUpdateUIEvent&)
{
		if (IsShown() && !_focus)
      {
         _settings_list->SetStringSelection(_tool->GetRadius().GetMultiplierString());

         _width->SetValue(_tool->GetRadius().GetNumberString());
      }
}

void CircleditDialog::FocusSet(wxFocusEvent &)
{
//   _focus=true;
}

void CircleditDialog::FocusKill(wxFocusEvent &)
{
//   _focus=false;
}

void CircleditDialog::OnActivate(wxActivateEvent& event)
{
   if (event.GetActive())
      _focus=true;
   else
      _focus=false;

}

