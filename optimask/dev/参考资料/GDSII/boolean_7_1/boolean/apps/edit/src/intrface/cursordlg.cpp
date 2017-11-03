                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      /*
Program			CURSORDLG.CPP
Purpose        Dialog to specify cursorposition.
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

#include "cursordlg.h"

const int    CURSOR_BUTTON_OK     	= wxID_HIGHEST + 5201 ;
const int    RADIO_BUTTON_ID        = wxID_HIGHEST + 5203 ;

BEGIN_EVENT_TABLE(CursorDialog, wxDialog)
  EVT_BUTTON     (CURSOR_BUTTON_OK,      CursorDialog::CmOk)
  EVT_CLOSE      (CursorDialog::OnCloseWindow)
  EVT_BUTTON	  (wxID_APPLY,				  CursorDialog::CmApply)
END_EVENT_TABLE()


CursorDialog::CursorDialog(GDSMainWindow* parent, long style):
		wxDialog(parent,-1,"Set Cursor",wxPoint(0,0),wxSize(200,115),style,"cursor dialog")
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
   _XX					= new wxTextCtrl(_panel,-1);
   _YY					= new wxTextCtrl(_panel,-1);
	_XYbox			= new wxStaticBox(_panel,-1,"Coordinates (XY)");
   _units_box		= new wxStaticBox(_panel,-1,"Units");
	_ok		  		= new wxButton(_panel,CURSOR_BUTTON_OK,"Hide");
   _apply         = new wxButton(_panel,wxID_APPLY,"Apply");
   _units_list 	= new wxChoice(_panel,-1,wxDefaultPosition,wxDefaultSize,9,_choices);

   *_unit = 0;

//Panel
   this->SetAutoLayout(TRUE);
   wxLayoutConstraints *c1 = new wxLayoutConstraints;
   c1->left.SameAs       (this, wxLeft,1);
   c1->top.SameAs        (this, wxTop,1);
   c1->right.SameAs      (this, wxRight, 1);
   c1->bottom.SameAs     (this, wxBottom, 1);
   _panel->SetConstraints(c1);

//units

	wxLayoutConstraints *c3 = new wxLayoutConstraints;
   c3->right.SameAs		 (_panel, wxRight, 5);
   c3->top.SameAs        (_panel, wxTop, 5);
   _units_box->SetSize(55,45);
   c3->height.AsIs();
   c3->width.AsIs();
   _units_box->SetConstraints(c3);

   wxLayoutConstraints *c9 = new wxLayoutConstraints;
   c9->top.SameAs      (_units_box, wxTop, 15);
   c9->left.SameAs     (_units_box, wxLeft, 5);
   c9->right.SameAs	  (_units_box,	wxRight, 5);
   c9->height.AsIs();
   c9->width.AsIs();
   _units_list->SetConstraints(c9);


//XY
   wxLayoutConstraints *c4 = new wxLayoutConstraints;
   c4->left.SameAs      (_panel, wxLeft, 5);
   c4->top.SameAs     	(_panel, wxTop, 5);
   c4->right.SameAs    	(_units_box, wxLeft, 5);
   _XYbox->SetSize(100,45);
   c4->height.AsIs();
   _XYbox->SetConstraints(c4);

   wxLayoutConstraints *c20 = new wxLayoutConstraints;
   c20->top.SameAs      (_XYbox, wxTop, 15);
   c20->left.SameAs     (_XYbox, wxLeft, 5);
	c20->width.PercentOf	(_XYbox, wxWidth, 40);
   c20->height.AsIs();
   _XX->SetConstraints(c20);

   wxLayoutConstraints *c21 = new wxLayoutConstraints;
   c21->top.SameAs      (_XYbox, wxTop, 15);
   c21->right.SameAs    (_XYbox, wxRight, 5);
   c21->width.PercentOf	(_XYbox, wxWidth, 40);
   c21->height.AsIs();
   _YY->SetConstraints(c21);

//Buttons
   wxLayoutConstraints *c6 = new wxLayoutConstraints;
   c6->left.SameAs      (_panel, wxLeft, 5);
   c6->top.SameAs     	(_XYbox, wxBottom, 5);
   c6->height.AsIs();
   c6->width.AsIs();
   _ok->SetConstraints(c6);

   wxLayoutConstraints *c7 = new wxLayoutConstraints;
   c7->left.SameAs      (_ok, wxRight, 5);
   c7->top.SameAs     	(_XYbox, wxBottom, 5);
   c7->height.AsIs();
   c7->width.AsIs();
   _apply->SetConstraints(c7);

   Layout();

}

void CursorDialog::SetupWindow()
{
	MakeModal(FALSE);
	Show(TRUE);
	Centre(wxBOTH);

   SetCursorX(0);
   SetCursorY(0);

	_units_list->SetStringSelection(GLOB->CMDH->GetDefaultUnit().GetMultiplierString());
	strcpy(_unit,_units_list->GetStringSelection());
}


double CursorDialog::GetCursorX()
{
	X = atof(_XX->GetValue());
   return X;
}

double CursorDialog::GetCursorY()
{
   Y = atof(_YY->GetValue());
   return Y;
}

void CursorDialog::SetCursorX(double cX)
{
	char value_X[20];
  	sprintf(value_X,"%f",cX);
  _XX->SetValue(value_X);
}

void CursorDialog::SetCursorY(double cY)
{
   char value_Y[20];
	sprintf(value_Y,"%f",cY);
	_YY->SetValue(value_Y);
}

void CursorDialog::CmOk(wxCommandEvent &)
{
	//EndModal(FALSE);
	Show(false);
}

void CursorDialog::CmApply(wxCommandEvent &)
{
	char unitbuf[30], buf[130];

	strcpy(unitbuf, _units_list->GetStringSelection());

	sprintf(buf,"{%s %s} {%s %s}",_XX->GetValue().c_str() ,unitbuf,_YY->GetValue().c_str(),unitbuf);

//   GLOB->CMDH->DoCommand("defaultunit  %s", unitbuf);
	GLOB->CMDH->DoCommand("setcursor %s", buf);
}

void CursorDialog::OnCloseWindow(wxCloseEvent& event)
{
	EndModal(FALSE);
}
