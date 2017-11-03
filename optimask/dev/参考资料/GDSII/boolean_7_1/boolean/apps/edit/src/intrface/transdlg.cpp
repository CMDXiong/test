/*
Program			TRANSDLG.CPP
Purpose        The dialog to specify the transformation values
Programmer		M. Reemst
last modified  A. Rona
Last Update		26-08-99
*/

#ifdef __GNUG__
#pragma implementation
#endif
#include "chandbas.h"
#include "mainwin.h"
#include "string.h"
#include "gdsapp.h"

#include "transdlg.h"

const int    TRANS_BUTTON_OK     = wxID_HIGHEST + 5231 ;
const int    TRANS_BUTTON_APPLY  = wxID_HIGHEST + 5232 ;
const int    TRANS_BUTTON_RESET  = wxID_HIGHEST + 5233 ;
const int    TRANS_EDIT_CHANGED  = wxID_HIGHEST + 5234 ;
const int    TRANS_BUTTON_CHECK  = wxID_HIGHEST + 5235 ;
const int    TRANS_BUTTON_UNDO   = wxID_HIGHEST + 5236 ;

BEGIN_EVENT_TABLE(Transformations, wxDialog)
  EVT_BUTTON    (TRANS_BUTTON_OK,      Transformations::CmOk)
  EVT_BUTTON    (TRANS_BUTTON_RESET,   Transformations::CmReset)
  EVT_BUTTON    (TRANS_BUTTON_APPLY,   Transformations::CmApply)
  EVT_BUTTON    (TRANS_BUTTON_UNDO,    Transformations::CmUndo)
  //EVT_CHECKBOX  (TRANS_BUTTON_CHECK,   Transformations::EnableItem)

  EVT_CLOSE     (Transformations::OnCloseWindow)
END_EVENT_TABLE()


Transformations::Transformations(GDSMainWindow* parent,long style):
		wxDialog(parent,-1,"Transform",wxPoint(0,0),wxSize(120,380),style,"Transformation dialog")
{
	_parent = parent;

	_panel 	= new wxPanel(this,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER ,"transpanel");

	_ok		  = new wxButton(_panel,TRANS_BUTTON_OK,"Hide ",wxDefaultPosition,wxSize(45,20));
	_reset	= new	wxButton(_panel,TRANS_BUTTON_RESET,"Reset",wxDefaultPosition,wxSize(45,20));
	_apply	= new wxButton(_panel,TRANS_BUTTON_APPLY,"Apply ",wxDefaultPosition,wxSize(45,20));
	_undo		= new wxButton(_panel,TRANS_BUTTON_UNDO,"Undo",wxDefaultPosition,wxSize(45,20));

	_movebox		= new wxStaticBox(_panel,-1,"Move (dX,dY)");
	_mirrorbox	= new wxStaticBox(_panel,-1,"Mirror");
	_scalebox		= new wxStaticBox(_panel,-1,"Scale (factor)");
	_rotatebox	= new wxStaticBox(_panel,-1,"Rotate (degrees)");

	_ed_move_h				= new wxTextCtrl(_panel,-1,"0");
	_ed_move_v				= new wxTextCtrl(_panel,-1,"0");
	_ed_scale_h				= new wxTextCtrl(_panel,-1,"1");

	_ed_degrees				= new wxTextCtrl(_panel,-1,"0");

   _check_mirror_h		= new wxCheckBox(_panel,-1,"horizontal");
   _check_mirror_v		= new wxCheckBox(_panel,-1,"vertical");

	wxString radio_strings[2];
	radio_strings[0] = "Clockwise";
	radio_strings[1] = "Cntr clockwise";
	_ed_cwORccw 	= new wxRadioBox(_panel,-1,"Direction",wxDefaultPosition,wxDefaultSize,2,radio_strings,1,wxRA_HORIZONTAL,wxDefaultValidator,"Rotate radiobox");


   this->SetAutoLayout(TRUE);
   wxLayoutConstraints *c1 = new wxLayoutConstraints;
   c1->left.SameAs       (this, wxLeft,1);
   c1->top.SameAs        (this, wxTop,1);
   c1->right.SameAs      (this, wxRight, 1);
   c1->bottom.SameAs     (this, wxBottom, 1);
   _panel->SetConstraints(c1);

//Mirror
   wxLayoutConstraints *c2 = new wxLayoutConstraints;
   c2->left.SameAs     (_panel, wxLeft, 5);
   c2->top.SameAs      (_panel, wxTop, 5);
   c2->right.SameAs    (_panel, wxRight, 5);
   c2->height.AsIs();
   _mirrorbox->SetSize(50,60);
   c2->height.AsIs();
	 _mirrorbox->SetConstraints(c2);

   wxLayoutConstraints *c10 = new wxLayoutConstraints;
   c10->left.SameAs      (_mirrorbox, wxLeft, 5);
   c10->top.SameAs       (_mirrorbox, wxTop,  15);
   c10->right.SameAs     (_mirrorbox, wxRight, 5);
   c10->height.AsIs();
   _check_mirror_h->SetConstraints(c10);

   wxLayoutConstraints *c11 = new wxLayoutConstraints;
   c11->left.SameAs      (_mirrorbox, wxLeft, 5);
   c11->top.SameAs       (_check_mirror_h, wxBottom, 0);
   c11->right.SameAs     (_mirrorbox, wxRight, 5);
   c11->height.AsIs();
   _check_mirror_v->SetConstraints(c11);


//Move
   wxLayoutConstraints *c3 = new wxLayoutConstraints;
   c3->left.SameAs       (_panel, wxLeft, 5);
   c3->top.SameAs     (_mirrorbox, wxBottom, 5);
   c3->right.SameAs    (_panel, wxRight, 5);
   _movebox->SetSize(50,75);
   c3->height.AsIs();
   _movebox->SetConstraints(c3);

   wxLayoutConstraints *c17 = new wxLayoutConstraints;
   c17->left.SameAs     (_movebox, wxLeft, 5);
   c17->top.SameAs      (_movebox, wxTop, 15);
   c17->right.SameAs    (_movebox, wxRight, 5);
   c17->height.AsIs();
	 _ed_move_h->SetConstraints(c17);

   wxLayoutConstraints *c18 = new wxLayoutConstraints;
   c18->left.SameAs     (_movebox, wxLeft, 5);
   c18->top.SameAs      (_ed_move_h, wxBottom, 5);
   c18->right.SameAs    (_movebox, wxRight, 5);
   c18->height.AsIs();
	 _ed_move_v->SetConstraints(c18);


//Scale
   wxLayoutConstraints *c4 = new wxLayoutConstraints;
   c4->left.SameAs       (_panel, wxLeft, 5);
   c4->top.SameAs     (_movebox, wxBottom, 5);
   c4->right.SameAs    (_panel, wxRight, 5);
   _scalebox->SetSize(50,40);
   c4->height.AsIs();
   _scalebox->SetConstraints(c4);

   wxLayoutConstraints *c20 = new wxLayoutConstraints;
   c20->left.SameAs     (_scalebox, wxLeft, 5);
   c20->top.SameAs      (_scalebox, wxTop, 15);
   c20->right.SameAs    (_scalebox, wxRight, 5);
   _ed_scale_h->SetSize(30,20);
   c20->height.AsIs();
	 _ed_scale_h->SetConstraints(c20);


//rotate
	 wxLayoutConstraints *c5 = new wxLayoutConstraints;
   c5->left.SameAs      (_panel, wxLeft, 5);
   c5->top.SameAs    (_scalebox, wxBottom, 5);
   c5->right.SameAs    (_panel, wxRight, 5);
   _rotatebox->SetSize(50,40);
   c5->height.AsIs();
   _rotatebox->SetConstraints(c5);

	 wxLayoutConstraints *c19 = new wxLayoutConstraints;
   c19->left.SameAs     (_rotatebox, wxLeft, 5);
   c19->top.SameAs      (_rotatebox, wxTop, 15);
   c19->right.SameAs    (_rotatebox, wxRight, 5);
   c19->height.AsIs();
   _ed_degrees->SetSize(30,20);
	_ed_degrees->SetConstraints(c19);


//Direction
   wxLayoutConstraints *c16 = new wxLayoutConstraints;
   c16->left.SameAs     (_panel, wxLeft, 5);
   c16->top.SameAs      (_rotatebox,wxBottom, 5);
   c16->height.AsIs();
   c16->right.SameAs    (_panel, wxRight, 5);
	 _ed_cwORccw->SetConstraints(c16);

//Buttons
   wxLayoutConstraints *c6 = new wxLayoutConstraints;
   c6->left.SameAs       (_panel, wxLeft, 5);
//   c6->bottom.SameAs     (_panel, wxBottom, 5);
   c6->top.SameAs     (_ed_cwORccw, wxBottom, 5);
   c6->height.AsIs();
   c6->width.AsIs();
   _ok->SetConstraints(c6);

   wxLayoutConstraints *c7 = new wxLayoutConstraints;
   c7->left.RightOf      (_ok, 5);
//   c7->bottom.SameAs     (_panel, wxBottom, 5);
   c7->top.SameAs     (_ed_cwORccw, wxBottom, 5);
   c7->height.AsIs();
   c7->width.AsIs();
   _apply->SetConstraints(c7);

   wxLayoutConstraints *c8 = new wxLayoutConstraints;
   c8->left.SameAs      (_panel, wxLeft, 5);
//   c8->bottom.SameAs     (_panel, wxBottom, 5);
   c8->top.SameAs     (_ok, wxBottom, 5);
   c8->height.AsIs();
   c8->width.AsIs();
   _undo->SetConstraints(c8);

   wxLayoutConstraints *c9 = new wxLayoutConstraints;
   c9->left.RightOf      (_undo, 5);
//   c9->bottom.SameAs     (_panel, wxBottom, 5);
   c9->top.SameAs     (_ok, wxBottom, 5);
   c9->height.AsIs();
   c9->width.AsIs();
   _reset->SetConstraints(c9);

   Layout();

}

Transformations::~Transformations()
{
}

void Transformations::SetupWindow(bool all_layers)
{
  	_all_layers = all_layers;

  	_scale_h_ratio.Printf("%6.3f",GLOB->CMDH->GetScale());
   _degr.Printf("%6.3f",GLOB->CMDH->GetRotate());
   _move_h=GLOB->CMDH->GetMoveHor().GetValueString();
   _move_v=GLOB->CMDH->GetMoveVer().GetValueString();

   _mirror_x =GLOB->CMDH->GetMirrorX();
   _mirror_y =GLOB->CMDH->GetMirrorY();

	_ed_cwORccw->SetSelection(!GLOB->CMDH->GetDirection());
   _check_mirror_h->SetValue(_mirror_y);
   _check_mirror_v->SetValue(_mirror_x);

   _ed_move_h->SetValue(_move_h);
   _ed_move_v->SetValue(_move_v);
   _ed_scale_h->SetValue(_scale_h_ratio);
   _ed_degrees->SetValue(_degr);

	Centre(wxBOTH);
   Show(true);
}

// respond to the ok button
void Transformations::CmOk(wxCommandEvent &)
{
   Show(false);
}

void Transformations::CmReset(wxCommandEvent &)
{

	_ed_cwORccw->SetSelection(GLOB->CMDH->GetDirection());
   _check_mirror_h->SetValue(FALSE);
   _check_mirror_v->SetValue(FALSE);

   _ed_move_h->SetValue("0");
   _ed_move_v->SetValue("0");
   _ed_scale_h->SetValue("1");
   _ed_degrees->SetValue("0");
	GLOB->CMDH->DoCommand("resettrans");
}

// Called when checkbox "_item" was clicked
void Transformations::EnableItem(char* _item, bool waarde)
{
   if (strcmp(_item,"mirrorh")==0)
   {
   	_mirror_y = waarde;
   }
   else if (strcmp(_item,"mirrorv")==0)
   {
   	_mirror_x = waarde;
   }
}

void Transformations::CmApply(wxCommandEvent &)
{
   _mirror_x = _check_mirror_h->GetValue();
   _mirror_y = _check_mirror_v->GetValue();

   _move_h=_ed_move_h->GetValue();
   _move_v=_ed_move_v->GetValue();
   _scale_h_ratio=_ed_scale_h->GetValue();
   _degr=_ed_degrees->GetValue();

	GLOB->CMDH->DoCommand("mirrorx  %s"    , (_mirror_x) ? "true" : "false");
	GLOB->CMDH->DoCommand("mirrory  %s"    , (_mirror_y) ? "true" : "false");
	GLOB->CMDH->DoCommand("movehor  {%s}"  , (_move_h == "") ? "0" : _move_h.c_str());
	GLOB->CMDH->DoCommand("movever  {%s}"  , (_move_v == "") ? "0" : _move_v.c_str());
	GLOB->CMDH->DoCommand("scale  %s"      , (_scale_h_ratio == "") ? "1" : _scale_h_ratio.c_str());
	GLOB->CMDH->DoCommand("rotate  %s"     , (_degr == "") ? "0" : _degr.c_str());
  char buf[50];
  strcpy(buf,_ed_cwORccw->GetStringSelection());
	GLOB->CMDH->DoCommand("direction  {%s}", buf);
   if (_all_layers)
		_parent->Operation("transform");
   else
		_parent->Operation("transform_a");
}

void Transformations::CmUndo(wxCommandEvent &)
{
   if (_all_layers)
		_parent->Operation("undotrans");
   else
		_parent->Operation("undotrans_a");
}

void Transformations::OnCloseWindow(wxCloseEvent& /*event*/)
{
   Show(false);
}

