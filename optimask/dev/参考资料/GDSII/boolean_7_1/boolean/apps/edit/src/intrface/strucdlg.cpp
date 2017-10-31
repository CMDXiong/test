#ifdef __GNUG__
#pragma implementation
#endif

#include "gdsapp.h"
#include "gdsii.h"
#include "mainwin.h"
#include "chandbas.h"

#include "strucdlg.h"
#include "structur.h"

#define STRUCT_HSIZE		220
#define STRUCT_VSIZE		400

const int    STRUC_BUTTON_OK     = wxID_HIGHEST + 5401 ;
const int    STRUC_BUTTON_CANCEL = wxID_HIGHEST + 5402 ;
const int    STRUC_BUTTON_SHOW   = wxID_HIGHEST + 5403 ;
const int    STRUC_UPDATE_DIALOG = wxID_HIGHEST + 5404 ;


BEGIN_EVENT_TABLE(StructuresDialog, wxDialog)


  EVT_UPDATE_UI(STRUC_UPDATE_DIALOG, StructuresDialog::Update)


  EVT_BUTTON    (STRUC_BUTTON_OK,  StructuresDialog::CmOk)

  EVT_BUTTON    (STRUC_BUTTON_CANCEL ,  StructuresDialog::CmCancel)

  EVT_BUTTON    (STRUC_BUTTON_SHOW   ,  StructuresDialog::CmShow)

  EVT_CLOSE     (StructuresDialog::OnCloseWindow)

END_EVENT_TABLE()


StructuresDialog::StructuresDialog(GDSMainWindow* parent,long style):
	wxDialog(parent,STRUC_UPDATE_DIALOG,"view structure",wxPoint(0,0),wxSize(STRUCT_HSIZE,STRUCT_VSIZE),style,"Struct dialog")
{
	_parent = parent;

	_panel 	= new wxPanel(this,-1,wxDefaultPosition,wxDefaultSize,0,"panel");
	_gb		= new wxStaticBox(	_panel,-1,"Structures",wxDefaultPosition,wxDefaultSize,0,"struct groupbox");
	_mess1	= new wxStaticText(	_panel,-1,"nop");
	_mess2	= new wxStaticText(	_panel,-1,"Select a structure in the list above");
	_listbox = new wxListBox(	_panel,-1,wxDefaultPosition,wxDefaultSize,0,NULL,wxLB_NEEDED_SB | wxLB_SINGLE);
	_button1	= new wxButton(	_panel,STRUC_BUTTON_OK,		"Hide",wxDefaultPosition,wxSize(55,20));
	_button2	= new wxButton(	_panel,STRUC_BUTTON_CANCEL,	"Cancel",wxDefaultPosition,wxSize(55,20));
	_buttonSHOW = new wxButton(	_panel,STRUC_BUTTON_SHOW,	"Show",wxDefaultPosition,wxSize(55,20));

  this->SetAutoLayout(TRUE);

  // Set constraints for panel subwindow
  wxLayoutConstraints *c1 = new wxLayoutConstraints;

  c1->left.SameAs       (this, wxLeft,1);

  c1->top.SameAs        (this, wxTop,1);

  c1->right.SameAs      (this, wxRight, 1);

  c1->bottom.SameAs     (this, wxBottom, 1);

  _panel->SetConstraints(c1);



   //staticbox

   wxLayoutConstraints *c3 = new wxLayoutConstraints;

   c3->left.SameAs       (_panel, wxLeft,2);

   c3->top.SameAs        (_panel, wxTop,2);

   c3->right.LeftOf      (_button2,5);

   c3->bottom.SameAs     (_mess2, wxTop,2);

   _gb->SetConstraints(c3);



   //mess2

   wxLayoutConstraints *c2 = new wxLayoutConstraints;

   c2->left.SameAs       (_gb, wxLeft,10);

   c2->top.SameAs        (_gb, wxTop,15);

   c2->height.AsIs();

   c2->width.AsIs();

   _mess1->SetConstraints(c2);



   //listbox

   wxLayoutConstraints *c9 = new wxLayoutConstraints;

   c9->left.SameAs       (_gb, wxLeft,12);

   c9->top.SameAs        (_mess1, wxBottom,2);

   c9->right.SameAs      (_gb, wxRight,12);

   c9->bottom.SameAs     (_gb, wxBottom,12);

   _listbox->SetConstraints(c9);



   //OKbutton

   wxLayoutConstraints *c4 = new wxLayoutConstraints;

   c4->left.RightOf      (_gb, 5);

   c4->top.SameAs     	 (_gb, wxTop,5);

   c4->height.AsIs();

   c4->width.AsIs();

   _button1->SetConstraints(c4);



   //Cancel button

   wxLayoutConstraints *c5 = new wxLayoutConstraints;

   c5->right.SameAs      (_panel,wxRight, 5);

   c5->top.SameAs     	 (_button1, wxBottom,5);

   c5->height.AsIs();

   c5->width.AsIs();

   _button2->SetConstraints(c5);



   //Show button

   wxLayoutConstraints *c15 = new wxLayoutConstraints;

   c15->right.SameAs      (_panel,wxRight, 5);


   c15->top.SameAs     	 (_button2, wxBottom,15);

   c15->height.AsIs();

   c15->width.AsIs();

   _buttonSHOW->SetConstraints(c15);



   //mess2

   wxLayoutConstraints *c8 = new wxLayoutConstraints;

   c8->left.SameAs       (_panel, wxLeft);

   c8->bottom.SameAs     (_panel, wxBottom,5);

   c8->height.AsIs();

   c8->width.AsIs();

   _mess2->SetConstraints(c8);

   Layout();


}

StructuresDialog::~StructuresDialog()
{
}


void StructuresDialog::Update(wxUpdateUIEvent &)
{

		if (IsShown())
      {
         bool bad=false;
         _structurelist = GLOB->CMDH->Get_GDSII();

         if (_structurelist->count() != _listbox->GetCount())
            bad=true;
         else
         {  //steck if they all do exist in the listbox
            TDLI<Structure> I=TDLI<Structure>(_structurelist);
            I.toroot();
            while (I.iterate()) {
               if (I.item() == GLOB->CMDH->Get_GDSII()->GetTopStructure())
               { char buf[80];
                 strcpy(buf,I.item()->GetStructureName());
                 strcat(buf,"        ==> Top");
                 if (_listbox->FindString(buf) == -1) bad=true;
               }
               else
                 if (_listbox->FindString(I.item()->GetStructureName()) == -1) bad=true;
            }
         }

         if (bad)
         {
				char	mess[80];
            _listbox->Clear();

            TDLI<Structure> I=TDLI<Structure>(_structurelist);
            I.toroot();
            while (I.iterate()) {
               if (I.item() == GLOB->CMDH->Get_GDSII()->GetTopStructure())
               { char buf[80];
                 strcpy(buf,I.item()->GetStructureName());
                 strcat(buf,"        ==> Top");
                 _listbox->Append(buf);
               }
               else
                 _listbox->Append(I.item()->GetStructureName());
            }

            sprintf(mess,"Number of structures: %i",_structurelist->count());
            _mess1->SetLabel(mess);

            // set the first structure as default and give it the focus
            _listbox->SetSelection(0,TRUE);
          }
      }
      return;
}


void StructuresDialog::SetupWindow(GdsII* gdsii)
{
	char	mess[80];

	_structurelist = gdsii;

	_listbox->Clear();

  	TDLI<Structure> I=TDLI<Structure>(_structurelist);
	I.toroot();
	while (I.iterate()) 
   {
      wxString structName = I.item()->GetStructureName();
      if (I.item() == gdsii->GetTopStructure())
      { 
         structName += "        ==> Top";
      }
      _listbox->Append(structName);
	}

	sprintf(mess,"Number of structures: %i",_structurelist->count());
	_mess1->SetLabel(mess);

   Show(TRUE);
	Centre(wxBOTH);

	// set the first structure as default and give it the focus
	_listbox->SetSelection(0,TRUE);
	_structure = NULL;
}

// OK button pressed: get the choosen structure
void StructuresDialog::CmOk(wxCommandEvent &)
{
   Show(FALSE);

	wxString selectedStruct;
   int top = 0;
	selectedStruct = _listbox->GetStringSelection();
   if ( (top = selectedStruct.Find( "        ==> Top" )) > 0 )
      selectedStruct = selectedStruct.Mid( 0, top );
	_structure = _structurelist->GetStructure(selectedStruct);
   if (_structure)
		GLOB->CMDH->DoCommand("viewstructure %s",selectedStruct);
}

// OK button pressed: get the choosen structure
void StructuresDialog::CmShow(wxCommandEvent &)
{
	wxString selectedStruct;
   int top = 0;
	selectedStruct = _listbox->GetStringSelection();
   if ( (top = selectedStruct.Find( "        ==> Top" )) > 0 )
      selectedStruct = selectedStruct.Mid( 0, top );
	_structure = _structurelist->GetStructure(selectedStruct);
   if (_structure)
		GLOB->CMDH->DoCommand("viewstructure %s",selectedStruct);
}

Structure* StructuresDialog::GetStructure()
{
	return _structure;
}

// CANCEL button pressed
void StructuresDialog::CmCancel(wxCommandEvent &)
{
	Show(FALSE);

}

void StructuresDialog::OnCloseWindow(wxCloseEvent& event)
{
   wxCommandEvent a = wxCommandEvent();
   CmCancel(a);
}

BEGIN_EVENT_TABLE(StructuresDialog2, wxDialog)
  EVT_BUTTON    (STRUC_BUTTON_OK,  StructuresDialog2::CmOk)

  EVT_BUTTON    (STRUC_BUTTON_CANCEL ,  StructuresDialog2::CmCancel)
  EVT_CLOSE     (StructuresDialog2::OnCloseWindow)
END_EVENT_TABLE()


StructuresDialog2::StructuresDialog2(GDSMainWindow* parent,long style):
	wxDialog(parent,STRUC_UPDATE_DIALOG,"view structure",wxPoint(0,0),wxSize(STRUCT_HSIZE,STRUCT_VSIZE),style,"Struct dialog")
{
	_parent = parent;

	_panel 	= new wxPanel(this,-1,wxDefaultPosition,wxDefaultSize,0,"panel");
	_gb		= new wxStaticBox(	_panel,-1,"Structures",wxDefaultPosition,wxDefaultSize,0,"struct groupbox");
	_mess1	= new wxStaticText(	_panel,-1,"nop");
	_mess2	= new wxStaticText(	_panel,-1,"Select a structure in the list above");
	_listbox = new wxListBox(	_panel,-1,wxDefaultPosition,wxDefaultSize,0,NULL,wxLB_NEEDED_SB | wxLB_SINGLE);
	_button1	= new wxButton(	_panel,STRUC_BUTTON_OK,		"Ok",wxDefaultPosition,wxSize(55,20));
	_button2	= new wxButton(	_panel,STRUC_BUTTON_CANCEL,	"Cancel",wxDefaultPosition,wxSize(55,20));

  this->SetAutoLayout(TRUE);

  // Set constraints for panel subwindow
  wxLayoutConstraints *c1 = new wxLayoutConstraints;

  c1->left.SameAs       (this, wxLeft,1);

  c1->top.SameAs        (this, wxTop,1);

  c1->right.SameAs      (this, wxRight, 1);

  c1->bottom.SameAs     (this, wxBottom, 1);

  _panel->SetConstraints(c1);



   //staticbox

   wxLayoutConstraints *c3 = new wxLayoutConstraints;

   c3->left.SameAs       (_panel, wxLeft,2);

   c3->top.SameAs        (_panel, wxTop,2);

   c3->right.LeftOf      (_button2,5);

   c3->bottom.SameAs     (_mess2, wxTop,2);

   _gb->SetConstraints(c3);



   //mess2

   wxLayoutConstraints *c2 = new wxLayoutConstraints;

   c2->left.SameAs       (_gb, wxLeft,10);

   c2->top.SameAs        (_gb, wxTop,15);

   c2->height.AsIs();

   c2->width.AsIs();

   _mess1->SetConstraints(c2);



   //listbox

   wxLayoutConstraints *c9 = new wxLayoutConstraints;

   c9->left.SameAs       (_gb, wxLeft,12);

   c9->top.SameAs        (_mess1, wxBottom,2);

   c9->right.SameAs      (_gb, wxRight,12);

   c9->bottom.SameAs     (_gb, wxBottom,12);

   _listbox->SetConstraints(c9);



   //OKbutton

   wxLayoutConstraints *c4 = new wxLayoutConstraints;

   c4->left.RightOf      (_gb, 5);

   c4->top.SameAs     	 (_gb, wxTop,5);

   c4->height.AsIs();

   c4->width.AsIs();

   _button1->SetConstraints(c4);



   //Cancel button

   wxLayoutConstraints *c5 = new wxLayoutConstraints;

   c5->right.SameAs      (_panel,wxRight, 5);

   c5->top.SameAs     	 (_button1, wxBottom,5);

   c5->height.AsIs();

   c5->width.AsIs();

   _button2->SetConstraints(c5);


   //mess2

   wxLayoutConstraints *c8 = new wxLayoutConstraints;

   c8->left.SameAs       (_panel, wxLeft);

   c8->bottom.SameAs     (_panel, wxBottom,5);

   c8->height.AsIs();

   c8->width.AsIs();

   _mess2->SetConstraints(c8);

	char	mess[80];

	_structurelist = GLOB->CMDH->Get_GDSII();

	_listbox->Clear();

  	TDLI<Structure> I=TDLI<Structure>(_structurelist);
	I.toroot();
	while (I.iterate()) {
      if (I.item() != GLOB->CMDH->Get_GDSII()->GetTopStructure())
		  _listbox->Append(I.item()->GetStructureName());
	}

	sprintf(mess,"Number of structures: %i",_structurelist->count()-1);
	_mess1->SetLabel(mess);

	Centre(wxBOTH);

	// set the first structure as default and give it the focus
	_listbox->SetSelection(0,TRUE);
	_structure = NULL;
}

StructuresDialog2::~StructuresDialog2()
{
}

// OK button pressed: get the choosen structure
void StructuresDialog2::CmOk(wxCommandEvent&)
{
   Show(FALSE);

	char buf[50];
	strcpy(buf,_listbox->GetStringSelection());
	_structure = _structurelist->GetStructure(buf);
	EndModal(wxID_OK);
}

Structure* StructuresDialog2::GetStructure()
{
	return _structure;
}

// CANCEL button pressed
void StructuresDialog2::CmCancel(wxCommandEvent&)
{
   EndModal(wxID_CANCEL);
}

void StructuresDialog2::OnCloseWindow(wxCloseEvent& event)
{
   wxCommandEvent a = wxCommandEvent();
   CmCancel(a);
}


