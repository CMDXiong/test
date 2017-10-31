/*

Program			NEWFILEDLG.CPP
Purpose     	Dialog to specify the layout- and units default values
Programmer     A.Rona
Last Update    31-08-99

*/

#ifdef __GNUG__
#pragma implementation
#endif
#include "chandbas.h"
#include "mainwin.h"
#include "string.h"
#include "gdsapp.h"

#include "newfiledlg.h"

const int    SETTINGS_BUTTON_OK     = wxID_HIGHEST + 5214;

BEGIN_EVENT_TABLE(NewFileDialog, wxDialog)
  EVT_CLOSE      (NewFileDialog::OnCloseWindow)
  EVT_BUTTON     (SETTINGS_BUTTON_OK, NewFileDialog::CmOk)
END_EVENT_TABLE()


NewFileDialog::NewFileDialog(GDSMainWindow* parent, long style):
		wxDialog(parent,-1,"Default settings",wxPoint(0,0),wxSize(150,200),style,"default dialog")
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
	_panel 				= new wxPanel(this,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER ,"Settings");

   _layoutsize_box 	= new wxStaticBox(_panel,-1,"layout size");
   _units_box		   = new wxStaticBox(_panel,-1,"units");
   _layout_x			= new wxTextCtrl(_panel,-1,"1000");
   _layout_y			= new wxTextCtrl(_panel,-1,"1000");
   _ed_acc   			= new wxTextCtrl(_panel,-1,"acc");
   _ok		  			= new wxButton(_panel,SETTINGS_BUTTON_OK,"Ok");
   _units_list 		= new wxChoice(_panel,-1,wxDefaultPosition,wxDefaultSize,9,_choices);

   *_acc = 0;
   *_unit = 0;

// Panel
   this->SetAutoLayout(TRUE);
   wxLayoutConstraints *c1 = new wxLayoutConstraints;
   c1->left.SameAs       (this, wxLeft,1);
   c1->top.SameAs        (this, wxTop,1);
   c1->right.SameAs      (this, wxRight, 1);
   c1->bottom.SameAs     (this, wxBottom, 1);
   _panel->SetConstraints(c1);

//Layout
	wxLayoutConstraints *c2 = new wxLayoutConstraints;
   c2->left.SameAs       (_panel, wxLeft, 5);
   c2->top.SameAs        (_panel, wxTop, 5);
   c2->right.SameAs		 (_panel, wxRight, 5);
   _layoutsize_box->SetSize(100,70);
   c2->height.AsIs();
   _layoutsize_box->SetConstraints(c2);

   wxLayoutConstraints *c20 = new wxLayoutConstraints;
   c20->left.SameAs     (_layoutsize_box, wxLeft, 5);
   c20->top.SameAs      (_layoutsize_box, wxTop, 15);
   c20->right.SameAs    (_layoutsize_box, wxRight, 5);
   _layout_x->SetSize(50,20);
   c20->height.AsIs();
   _layout_x->SetConstraints(c20);

   wxLayoutConstraints *c6 = new wxLayoutConstraints;
   c6->left.SameAs       (_layoutsize_box, wxLeft, 5);
   c6->top.SameAs        (_layout_x, wxBottom, 5);
   c6->right.SameAs      (_layoutsize_box, wxRight, 5);
   _layout_y->SetSize(50,20);
   c6->height.AsIs();
   _layout_y->SetConstraints(c6);


//units

	wxLayoutConstraints *c3 = new wxLayoutConstraints;
   c3->left.SameAs       (_panel, wxLeft, 5);
   c3->right.SameAs		 (_panel, wxRight, 5);
   c3->top.SameAs        (_layoutsize_box, wxBottom, 5);
   _units_box->SetSize(100,50);
   c3->height.AsIs();
   _units_box->SetConstraints(c3);

   wxLayoutConstraints *c8 = new wxLayoutConstraints;
   c8->left.SameAs    (_units_box, wxLeft, 5);
   c8->top.SameAs      (_units_box, wxTop, 15);
   _ed_acc->SetSize(34,20);
   c8->height.AsIs();
   c8->width.AsIs();
   _ed_acc->SetConstraints(c8);

   wxLayoutConstraints *c9 = new wxLayoutConstraints;
   c9->top.SameAs      (_units_box, wxTop, 15);
   c9->left.SameAs    (_ed_acc, wxRight, 5);
   c9->height.AsIs();
   c9->width.AsIs();
   _units_list->SetConstraints(c9);


//buttons
   wxLayoutConstraints *c4 = new wxLayoutConstraints;
   c4->left.SameAs       (_panel, wxLeft, 5);
   c4->top.SameAs     	 (_units_box, wxBottom, 5);
   c4->height.AsIs();
   c4->width.AsIs();
   _ok->SetConstraints(c4);

   Layout();

}

NewFileDialog::~NewFileDialog()         
{
}                                                

void NewFileDialog::SetupWindow()
{
	Centre(wxBOTH);

  	sprintf(_acc,"%f",GLOB->CMDH->GetDefaultAcc());
   _ed_acc->SetValue(_acc);

	_units_list->SetStringSelection(GLOB->CMDH->GetDefaultUnit().GetMultiplierString());
	strcpy(_unit,_units_list->GetStringSelection());
}

void NewFileDialog::CmOk(wxCommandEvent&)
{
	EndModal(FALSE);

   try{
   	char buf[50];

      strcpy(buf, _layout_x->GetValue() );
      strcat(buf, " ");

      strcat(buf, _layout_y->GetValue() );
      strcat(buf, " ");

      strcat(buf,_ed_acc->GetValue());
      strcat(buf, " ");

      strcat(buf,_units_list->GetStringSelection());

      GLOB->CMDH->DoCommand("newfile %s", buf);

   }
   catch (GDS_Error& _error) {
      _messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
   }
   catch(...) {
      _messagehandler->info("SOMETHING WENT WRONG leaf program","NON HANDLED ERROR");
      return;
   }
}


void NewFileDialog::OnCloseWindow(wxCloseEvent& event)
{
	EndModal(FALSE);
}

