/*
Program			DISTDLG.CPP
Purpose        The dialog to display distances between points
Programmer		M. Reemst
Last Update		03-04-98
*/
#ifdef __GNUG__
#pragma implementation
#endif

#include <stdlib.h>
#include <math.h>
#include "mainwin.h"
#include "wxwin.h"
#include <wx/clipbrd.h>

#include "distdlg.h"
#include "copylist.h"
#include "chandbas.h"

#define DISTDLG_HSIZE		400
#define DISTDLG_VSIZE      210

const int    DIST_BUTTON_CLEAR  = wxID_HIGHEST + 4701 ;
const int    DIST_BUTTON_HIDE = wxID_HIGHEST + 4702 ;
const int    DIST_EDIT_CHANGED  = wxID_HIGHEST + 4704 ;

BEGIN_EVENT_TABLE(DistanceDlg, wxDialog)
  EVT_BUTTON    (DIST_BUTTON_CLEAR,DistanceDlg::ClearMe)
  EVT_BUTTON    (DIST_BUTTON_HIDE ,DistanceDlg::hide)
  EVT_TEXT_ENTER(DIST_EDIT_CHANGED,DistanceDlg::ClearMe)
  EVT_CLOSE     (DistanceDlg::OnCloseWindow)
END_EVENT_TABLE()



DistanceDlg::DistanceDlg(GDSMainWindow* parent,char *title,long style,char *name):
	wxDialog(parent,-1,title,wxDefaultPosition,wxSize(DISTDLG_HSIZE,DISTDLG_VSIZE),style,name)
{
	_parent = parent;

   *_start_point = 0;
	_start_x = 0;
   _start_y = 0;

	_panel 		= new wxPanel		(this,-1,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL|wxSTATIC_BORDER ,"panel");
	_lb			= new CopywxListBox	(_panel,-1,wxDefaultPosition,wxDefaultSize,0,NULL,wxLB_SINGLE);
	_clearButton = new wxButton	(_panel,DIST_BUTTON_CLEAR,"Clear");
	_hideButton = new wxButton		(_panel,DIST_BUTTON_HIDE,"Hide Dialog");
  	_m_start		= new wxStaticText(_panel,-1,"start");
  	_m_lb			= new wxStaticText(_panel,-1,"relative");
	_editField	= new wxTextCtrl	(_panel,DIST_EDIT_CHANGED," ");

  this->SetAutoLayout(TRUE);

  // Set constraints for panel subwindow
  wxLayoutConstraints *c1 = new wxLayoutConstraints;

  c1->left.SameAs       (this, wxLeft,1);

  c1->top.SameAs        (this, wxTop,1);

  c1->right.SameAs      (this, wxRight, 1);

  c1->bottom.SameAs     (this, wxBottom, 1);

  _panel->SetConstraints(c1);



  //startbutton

  wxLayoutConstraints *c2 = new wxLayoutConstraints;

  c2->left.SameAs       (_panel, wxLeft);

  c2->top.SameAs        (_panel, wxTop);

  c2->height.AsIs();

  c2->width.AsIs();

  _m_start->SetConstraints(c2);



  //editField

  wxLayoutConstraints *c7 = new wxLayoutConstraints;

  c7->left.SameAs       (_panel, wxLeft);

  c7->top.SameAs        (_m_start, wxBottom);

  c7->right.SameAs      (_panel, wxRight,2);

  c7->height.AsIs();

  _editField->SetConstraints(c7);



  //relativebutton

  wxLayoutConstraints *c6 = new wxLayoutConstraints;

  c6->left.SameAs       (_panel, wxLeft);

  c6->top.SameAs        (_editField, wxBottom,2);

  c6->height.AsIs();

  c6->width.AsIs();

  _m_lb->SetConstraints(c6);



  //listbox

  wxLayoutConstraints *c3 = new wxLayoutConstraints;

  c3->left.SameAs       (_panel, wxLeft);

  c3->top.SameAs        (_m_lb, wxBottom);

  c3->right.SameAs      (_panel, wxRight,2);

  c3->bottom.SameAs     (_hideButton,wxTop,2);

  _lb->SetConstraints(c3);



  //hidebutton

  wxLayoutConstraints *c4 = new wxLayoutConstraints;

  c4->left.SameAs       (_panel, wxLeft,5);

  c4->bottom.SameAs     (_panel, wxBottom,5);

  c4->height.AsIs();

  c4->width.AsIs();

  _hideButton->SetConstraints(c4);



  //clearbutton

  wxLayoutConstraints *c5 = new wxLayoutConstraints;

  c5->left.RightOf      (_hideButton,10);

  c5->bottom.SameAs     (_panel, wxBottom,5);

  c5->height.AsIs();

  c5->width.AsIs();

  _clearButton->SetConstraints(c5);


   Layout();

}


DistanceDlg::~DistanceDlg()
{
}

void DistanceDlg::addCommand(char* BaseCommand)
{
	_lb->Append(BaseCommand);
	_lb->SetFirstItem(_lb->GetCount()-1);
}

void DistanceDlg::setStartPoint(double x, double y)
{
	_start_x = x;
   _start_y = y;
	int exp = (int)floor(log10(GLOB->USERUNITS()<0 ? -GLOB->USERUNITS() : GLOB->USERUNITS()));
	sprintf(_start_point, "(%.*f, %.*f) ", -exp, x, -exp, y);
   strcat(_start_point,GLOB->CMDH->GetDefaultUnit().GetMultiplierString());

   _editField->SetValue(_start_point);
	_lb->Clear();
	if (_lb->GetCount() > 1)
		_lb->SetSelection(_lb->GetCount()-1,true);
//		_lb->SetFirstItem(_lb->GetCount()-1);
}

void DistanceDlg::setRelPoint(double x, double y)
{
	int exp = (int)floor(log10(GLOB->USERUNITS()<0 ? -GLOB->USERUNITS() : GLOB->USERUNITS()));
   double 	rel_x,
   			rel_y,
            length,
            angle;

   rel_x = x - _start_x;
   rel_y = y - _start_y;
   length = sqrt(pow(rel_x,2) + pow(rel_y,2));
   if (rel_x != 0 && rel_y != 0)
	   angle = asin(rel_y/length)*180/M_PI;
	else
   	angle = 0;
   if (rel_x < 0)
   	angle = 180 - angle;
	sprintf(_temp_char, "(%.*f, %.*f) %s dx:%.*f dy:%.*f  l:%.*f  angle:%.*f", -exp, x, -exp, y,
			   GLOB->CMDH->GetDefaultUnit().GetMultiplierString().c_str(), -exp, rel_x, -exp, rel_y, -exp, length, -exp, angle);

   _lb->Append(_temp_char);
	_lb->SetSelection(_lb->GetCount()-1,true);
}

void DistanceDlg::hide(wxCommandEvent&)
{
	_parent->ShowDistDlg();
}

void DistanceDlg::OnCloseWindow(wxCloseEvent& event)
{
	_parent->ShowDistDlg();
}

void DistanceDlg::ClearMe(wxCommandEvent&)
{
	setStartPoint(0,0);
}


