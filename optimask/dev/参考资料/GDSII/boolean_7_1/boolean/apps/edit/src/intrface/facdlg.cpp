
#ifdef __GNUG__
#pragma implementation
#endif

#include "chandbas.h"
#include "mainwin.h"
#include "string.h"

#include "facdlg.h"

	#define FAC_HSIZE					350
	#define FAC_VSIZE					350
	#define FAC_TEXT_H				20
	#define FAC_TEXT_W				80

	#define FAC_RADIO_W				200
	#define FAC_RADIO_H				20

const int    FACTOR_BUTTON_OK     = wxID_HIGHEST + 5601 ;
const int    FACTOR_BUTTON_CANCEL = wxID_HIGHEST + 5602 ;
const int    FACTOR_ID_NOTEBOOK   = wxID_HIGHEST + 5603 ;


BEGIN_EVENT_TABLE(Factors, wxDialog)
  EVT_BUTTON(FACTOR_BUTTON_OK,Factors::CmOk)

  EVT_BUTTON(FACTOR_BUTTON_CANCEL,Factors::CmCancel)

  EVT_CLOSE    (Factors::OnCloseWindow)

END_EVENT_TABLE()


Factors::Factors(GDSMainWindow* parent,long style):
		wxDialog(parent,-1,"Factors",wxPoint(0,0),wxSize(FAC_HSIZE,FAC_VSIZE),style,"Factors dialog")
{
	_parent = parent;

   m_notebook = new wxNotebook(this, FACTOR_ID_NOTEBOOK);
	_panel 	= new wxPanel(m_notebook,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER ,"panel1");
	_panel2 	= new wxPanel(m_notebook,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER ,"panel2");
	_panel3 	= new wxPanel(m_notebook,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER ,"panel3");
	_panel4 	= new wxPanel(m_notebook,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER ,"panel4");

	_ok		= new wxButton(this,FACTOR_BUTTON_OK,"Ok");
	_cancel	= new	wxButton(this,FACTOR_BUTTON_CANCEL,"Cancel");


	_m_snapfactor			= new wxStaticText(_panel,-1,"Snap Factor"				,wxPoint(0,10));
	_m_correctionfactor	= new wxStaticText(_panel,-1,"Correction Factor"		,wxPoint(0,30));
	_m_selectmarge			= new wxStaticText(_panel,-1,"Select Factor"				,wxPoint(0,50));
	_m_selectmarge_comm	= new wxStaticText(_panel,-1,"(select factor * screenwidth=pixels)",wxPoint(0,70));
 	_m_roundfactor			= new wxStaticText(_panel,-1,"Rounding Factor (0-inf)",wxPoint(0,90));
	_m_correctionaber		= new wxStaticText(_panel,-1,"Correction Abberation"	,wxPoint(0,110));
	_ed_snapfactor 		= new wxTextCtrl(_panel,-1,"",wxPoint(180,10),wxSize(FAC_TEXT_W,FAC_TEXT_H));
	_ed_correctionfactor = new wxTextCtrl(_panel,-1,"",wxPoint(180,30),wxSize(FAC_TEXT_W,FAC_TEXT_H));
	_ed_selectmarge 		= new wxTextCtrl(_panel,-1,"",wxPoint(180,50),wxSize(FAC_TEXT_W,FAC_TEXT_H));
	_ed_roundfactor   	= new wxTextCtrl(_panel,-1,"",wxPoint(180,90),wxSize(FAC_TEXT_W,FAC_TEXT_H));
	_ed_correctionaber 	= new wxTextCtrl(_panel,-1,"",wxPoint(180,110),wxSize(FAC_TEXT_W,FAC_TEXT_H));
	wxString radio_strings[2];
	radio_strings[0] = "Circle";
	radio_strings[1] = "Medial";
	_ed_circleORmedial 	= new wxRadioBox(_panel,-1,"Rounding type",wxPoint(30,130),wxSize(FAC_RADIO_W,FAC_RADIO_H),2,radio_strings,1,wxRA_HORIZONTAL,wxDefaultValidator,"factors radiobox");

	_m_poly2arcrmin		= new wxStaticText(_panel2,-1,"Minimum Radius"			,wxPoint(5,10));
	_m_poly2arcrmax		= new wxStaticText(_panel2,-1,"Maximum Radius"			,wxPoint(5,30));
	_m_poly2arcaber		= new wxStaticText(_panel2,-1,"Conversion Abberation"	,wxPoint(5,50));
	_m_arc2polyaber		= new wxStaticText(_panel2,-1,"Arc Conversion Abberation"	,wxPoint(5,80));
	_ed_poly2arcrmin		= new wxTextCtrl(_panel2,-1,"",wxPoint(180,10),wxSize(FAC_TEXT_W,FAC_TEXT_H));
	_ed_poly2arcrmax		= new wxTextCtrl(_panel2,-1,"",wxPoint(180,30),wxSize(FAC_TEXT_W,FAC_TEXT_H));
	_ed_poly2arcaber		= new wxTextCtrl(_panel2,-1,"",wxPoint(180,50),wxSize(FAC_TEXT_W,FAC_TEXT_H));
	_ed_arc2polyaber		= new wxTextCtrl(_panel2,-1,"",wxPoint(180,80),wxSize(FAC_TEXT_W,FAC_TEXT_H));

	_m_displayaber			= new wxStaticText(_panel4,-1,"Display Abberation"		,wxPoint(0,10));
	_ed_displayaber 	   = new wxTextCtrl(_panel4,-1,"",wxPoint(180,10),wxSize(FAC_TEXT_W,FAC_TEXT_H));
	_m_structhres			= new wxStaticText(_panel4,-1,"Structure Threshold (pixels)"	,wxPoint(0,30));
	_ed_structhres			= new wxTextCtrl(_panel4,-1,"",wxPoint(180,30),wxSize(FAC_TEXT_W,FAC_TEXT_H));
	_m_primthres			= new wxStaticText(_panel4,-1,"Primitive Threshold (pixels)" ,wxPoint(0,50));
	_ed_primthres			= new wxTextCtrl(_panel4,-1,"",wxPoint(180,50),wxSize(FAC_TEXT_W,FAC_TEXT_H));
	_drawprim   			= new wxCheckBox(_panel4,-1,"Draw Rectangle at primitive ",wxPoint(0,70),wxSize(180,20));


	_m_smoothaber			= new wxStaticText(_panel3,-1,"Abberation"					,wxPoint(0,0));
	_m_maxlinemerge		= new wxStaticText(_panel3,-1,"Max linelength to merge",wxPoint(0,30));
	_ed_smoothaber			= new wxTextCtrl(_panel3,-1,"",wxPoint(180,0),wxSize(FAC_TEXT_W,FAC_TEXT_H));
	_ed_maxlinemerge 	   = new wxTextCtrl(_panel3,-1,"",wxPoint(180,30),wxSize(FAC_TEXT_W,FAC_TEXT_H));



   this->SetAutoLayout(TRUE);

   //OKbutton
   wxLayoutConstraints *c4 = new wxLayoutConstraints;

   c4->left.SameAs       (this,wxLeft, 2);

   c4->bottom.SameAs     (this, wxBottom,2);

   c4->height.AsIs();

   c4->width.AsIs();

   _ok->SetConstraints(c4);



   //Cancel button

   wxLayoutConstraints *c5 = new wxLayoutConstraints;

   c5->left.RightOf      (_ok,5);

   c5->bottom.SameAs     (this, wxBottom,2);

   c5->height.AsIs();

   c5->width.AsIs();

   _cancel->SetConstraints(c5);



   // Set constraints for notebook subwindow
   wxLayoutConstraints *c1 = new wxLayoutConstraints;

   c1->left.SameAs       (this, wxLeft,1);

   c1->top.SameAs        (this, wxTop,1);

   c1->right.SameAs      (this, wxRight, 1);

   c1->bottom.SameAs     (_ok, wxTop, 1);

   m_notebook->SetConstraints(c1);



   m_notebook->AddPage(_panel,"factors");

   m_notebook->AddPage(_panel2,"arc/polygon conversion");
   m_notebook->AddPage(_panel3,"smoothing");
   m_notebook->AddPage(_panel4,"display");

   Layout();
}


Factors::~Factors()
{
}


void Factors::SetupWindow()
{
	strcpy(_correctionfactor,GLOB->CMDH->GetCorrectionfactor().GetValueString().c_str());
	sprintf(_roundfactor,"%f",GLOB->CMDH->GetRoundfactor());
	strcpy(_correctionaber,GLOB->CMDH->GetCorrectionaber().GetValueString().c_str());
	strcpy(_roundtype,GLOB->CMDH->GetRoundtype().c_str());
	strcpy(_poly2arcrmin,GLOB->CMDH->GetPoly2ArcRmin().GetValueString().c_str());
	strcpy(_poly2arcrmax,GLOB->CMDH->GetPoly2ArcRmax().GetValueString().c_str());
	strcpy(_poly2arcaber,GLOB->CMDH->GetPoly2ArcAber().GetValueString().c_str());
	strcpy(_arc2polyaber,GLOB->CMDH->GetArc2PolyAber().GetValueString().c_str());
	strcpy(_smoothaber,  GLOB->CMDH->GetSmoothAber().GetValueString().c_str());
	strcpy(_snapfactor,  GLOB->CMDH->GetSnapfactor().GetValueString().c_str());
	strcpy(_displayaber, GLOB->CMDH->GetDisplayaber().GetValueString().c_str());
	strcpy(_maxlinemerge,GLOB->CMDH->GetMaxlinemerge().GetValueString().c_str());
	sprintf(_selectmarge,"%f",  GLOB->CMDH->GetSelectmarge());
	sprintf(_primthres,"%d",GLOB->CMDH->GetPrimThres());
	sprintf(_structhres,"%d",GLOB->CMDH->GetStrucThres());

	_ed_selectmarge->SetValue(_selectmarge);
	_ed_snapfactor->SetValue(_snapfactor);
	_ed_correctionfactor->SetValue(_correctionfactor);
	_ed_roundfactor->SetValue(_roundfactor);
	_ed_correctionaber->SetValue(_correctionaber);
	_ed_poly2arcrmin->SetValue(_poly2arcrmin);
	_ed_poly2arcrmax->SetValue(_poly2arcrmax);
	_ed_poly2arcaber->SetValue(_poly2arcaber);
	_ed_arc2polyaber->SetValue(_arc2polyaber);
	_ed_maxlinemerge->SetValue(_maxlinemerge);
	_ed_smoothaber->SetValue(_smoothaber);
	_ed_displayaber->SetValue(_displayaber);
	_ed_primthres->SetValue(_primthres);
	_ed_structhres->SetValue(_structhres);

   if (GLOB->CMDH->GetDrawSmallPrim())
		_drawprim->SetValue(1);
   else
		_drawprim->SetValue(0);

	_ed_circleORmedial->SetSelection(strcmp(_roundtype,"circle")!=0);

//	SetSize(FAC_HSIZE,FAC_VSIZE);
	Centre(wxBOTH);
}


// respond to the ok button
void Factors::CmOk(wxCommandEvent&)
{
	StoreValues();
   try{
      GLOB->CMDH->DoCommand("# Factor setting(s)");
      GLOB->CMDH->DoCommand("selectmarge       {%s}", _selectmarge);
      GLOB->CMDH->DoCommand("snapfactor        {%s}", _snapfactor);
      GLOB->CMDH->DoCommand("correctionfactor  {%s}", _correctionfactor);
      GLOB->CMDH->DoCommand("roundfactor       {%s}", _roundfactor);
      GLOB->CMDH->DoCommand("correctionaber    {%s}", _correctionaber);
      GLOB->CMDH->DoCommand("roundtype         {%s}", _roundtype);
      GLOB->CMDH->DoCommand("toarcrmin         {%s}", _poly2arcrmin);
      GLOB->CMDH->DoCommand("toarcrmax         {%s}", _poly2arcrmax);
      GLOB->CMDH->DoCommand("arcaccur          {%s}", _poly2arcaber);
      GLOB->CMDH->DoCommand("polyaccur         {%s}", _arc2polyaber);
      GLOB->CMDH->DoCommand("smoothaccur       {%s}", _smoothaber);
      GLOB->CMDH->DoCommand("maxlinemerge      {%s}", _maxlinemerge);
      GLOB->CMDH->DoCommand("displayaber       {%s}", _displayaber);
      GLOB->CMDH->DoCommand("structhres        {%s}", _structhres);
     if (_drawsmallprim)
      GLOB->CMDH->DoCommand("primthres         {%s} {%s}", _primthres,"true");
     else
      GLOB->CMDH->DoCommand("primthres         {%s} {%s}", _primthres,"false");
   }
   catch (GDS_Error& _error) {
      _messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
   }
   catch(...) {
      _messagehandler->info("SOMETHING WENT WRONG leaf program","NON HANDLED ERROR");
   }
  EndModal(FACTOR_BUTTON_OK);
}

void Factors::CmCancel(wxCommandEvent&)
{
	EndModal(FACTOR_BUTTON_CANCEL);
}

// all the controls are read and the values are placed in the INI file
void Factors::StoreValues()
{
	strcpy(_selectmarge,_ed_selectmarge->GetValue());
	strcpy(_snapfactor,_ed_snapfactor->GetValue());
	strcpy(_correctionfactor,_ed_correctionfactor->GetValue());
	strcpy(_roundfactor,_ed_roundfactor->GetValue());
	strcpy(_correctionaber,_ed_correctionaber->GetValue());
	strcpy(_poly2arcrmin,_ed_poly2arcrmin->GetValue());
	strcpy(_poly2arcrmax,_ed_poly2arcrmax->GetValue());
	strcpy(_poly2arcaber,_ed_poly2arcaber->GetValue());
	strcpy(_arc2polyaber,_ed_arc2polyaber->GetValue());
	strcpy(_smoothaber  ,_ed_smoothaber->GetValue());
	strcpy(_maxlinemerge,_ed_maxlinemerge->GetValue());
	strcpy(_displayaber,_ed_displayaber->GetValue());
	strcpy(_primthres,_ed_primthres->GetValue());
	strcpy(_structhres,_ed_structhres->GetValue());

	_drawsmallprim=(bool)_drawprim->GetValue();

	if (_ed_circleORmedial->GetSelection() == 0)
		strcpy(_roundtype,"circle");
	else
		strcpy(_roundtype,"medial");

}


void Factors::OnCloseWindow(wxCloseEvent& /*event*/)
{
  EndModal(FACTOR_BUTTON_CANCEL);
}
