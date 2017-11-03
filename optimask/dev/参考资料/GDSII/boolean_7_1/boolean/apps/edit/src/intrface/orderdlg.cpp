/*
Name				: orderdlg.cpp
Purpose			: implementation of the layer order dialog
author			: J.C. Mollien
created			: 97-10-09
last modified	: 98-03-03, by M. Reemst
*/

#ifdef __GNUG__
#pragma implementation
#endif
#include "mainwin.h"
#include "gdsapp.h"
#include "lsetup.h"
#include "wxwin.h"
#include "chandbas.h"

#include "orderdlg.h"
const int    ORDER_BUTTON_OK     = wxID_HIGHEST + 5101 ;
const int    ORDER_BUTTON_CANCEL = wxID_HIGHEST + 5102 ;

const int    ORDER_BUTTON_UP     = wxID_HIGHEST + 5103 ;

const int    ORDER_BUTTON_DOWN   = wxID_HIGHEST + 5104 ;

const int    ORDER_BUTTON_SHOW   = wxID_HIGHEST + 5106 ;

const int    ORDER_LIST_SELECT   = wxID_HIGHEST + 5105 ;

const int    ORDER_UPDATE_DIALOG = wxID_HIGHEST + 5107 ;


BEGIN_EVENT_TABLE(LayerOrder, wxDialog)


  EVT_UPDATE_UI(ORDER_UPDATE_DIALOG,  LayerOrder::Update)


  EVT_BUTTON   (ORDER_BUTTON_OK,      LayerOrder::CmOk)

  EVT_BUTTON   (ORDER_BUTTON_CANCEL,  LayerOrder::CmCancel)

  EVT_BUTTON	(ORDER_BUTTON_UP,   LayerOrder::CmUp)

  EVT_BUTTON	(ORDER_BUTTON_DOWN,   LayerOrder::CmDown)

  EVT_BUTTON	(ORDER_BUTTON_SHOW,   LayerOrder::CmShow)

  EVT_LISTBOX  (ORDER_LIST_SELECT,   LayerOrder::CmListBox)

  EVT_CLOSE    (LayerOrder::OnCloseWindow)

END_EVENT_TABLE()


LayerOrder::LayerOrder(GDSMainWindow* parent,char* title, long style,char* name):
	wxDialog(parent,ORDER_UPDATE_DIALOG,title,wxPoint(0,0),wxSize(200,500),style,name)
{
	_parent = parent;

	_panel1 			= new wxPanel(this,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER ,"6");
	_listbox			= new wxListBox(_panel1,-1,wxDefaultPosition,wxDefaultSize,0,NULL,(wxLB_SINGLE|wxLB_NEEDED_SB));
	_text1 			= new wxStaticText(_panel1,-1,"change to order you prefer");
	_buttonUP 		= new wxButton(_panel1,ORDER_BUTTON_UP,		"Up",wxDefaultPosition,wxSize(55,20));
	_buttonDOWN 	= new wxButton(_panel1,ORDER_BUTTON_DOWN,	"Down",wxDefaultPosition,wxSize(55,20));
	_buttonOK 		= new wxButton(_panel1,ORDER_BUTTON_OK,		"Hide",wxDefaultPosition,wxSize(55,20));
	_buttonSHOW		= new wxButton(_panel1,ORDER_BUTTON_SHOW,		"Show",wxDefaultPosition,wxSize(55,20));
	_buttonCANCEL 	= new wxButton(_panel1,ORDER_BUTTON_CANCEL,"Cancel",wxDefaultPosition,wxSize(55,20));
	_groupbox 		= new wxStaticBox(_panel1,-1,"Layers",wxDefaultPosition,wxDefaultSize,0,"3");

   this->SetAutoLayout(TRUE);
   // Set constraints for panel subwindow
   wxLayoutConstraints *c1 = new wxLayoutConstraints;

   c1->left.SameAs       (this, wxLeft,1);

   c1->top.SameAs        (this, wxTop,1);

   c1->right.SameAs      (this, wxRight, 1);

   c1->bottom.SameAs     (this, wxBottom, 1);

   _panel1->SetConstraints(c1);



   //staticbox

   wxLayoutConstraints *c3 = new wxLayoutConstraints;

   c3->left.SameAs       (_panel1, wxLeft,2);

   c3->top.SameAs        (_panel1, wxTop,2);

   c3->right.LeftOf      (_buttonCANCEL,5);

   c3->bottom.SameAs     (_text1, wxTop,2);

   _groupbox->SetConstraints(c3);



   //listbox

   wxLayoutConstraints *c9 = new wxLayoutConstraints;

   c9->left.SameAs       (_groupbox, wxLeft,12);

   c9->top.SameAs        (_groupbox, wxTop,17);

   c9->right.SameAs      (_groupbox, wxRight,12);

   c9->bottom.SameAs     (_groupbox, wxBottom,12);

   _listbox->SetConstraints(c9);



   //OKbutton

   wxLayoutConstraints *c4 = new wxLayoutConstraints;

   c4->left.RightOf      (_groupbox, 5);

   c4->top.SameAs     	 (_groupbox, wxTop,5);

   c4->height.AsIs();

   c4->width.AsIs();

   _buttonOK->SetConstraints(c4);



   //Cancel button

   wxLayoutConstraints *c5 = new wxLayoutConstraints;

   c5->right.SameAs      (_panel1,wxRight, 5);

   c5->top.SameAs     	 (_buttonOK, wxBottom,5);

   c5->height.AsIs();

   c5->width.AsIs();

   _buttonCANCEL->SetConstraints(c5);



   //buttonUP

   wxLayoutConstraints *c6 = new wxLayoutConstraints;

   c6->left.RightOf      (_groupbox, 5);

   c6->top.SameAs     	 (_buttonCANCEL, wxBottom,25);

   c6->height.AsIs();

   c6->width.AsIs();

   _buttonUP->SetConstraints(c6);



   //buttonDOWN

   wxLayoutConstraints *c7 = new wxLayoutConstraints;

   c7->left.RightOf      (_groupbox, 5);

   c7->top.SameAs     	 (_buttonUP, wxBottom,5);

   c7->height.AsIs();

   c7->width.AsIs();

   _buttonDOWN->SetConstraints(c7);



   //buttonSHOW

   wxLayoutConstraints *c19 = new wxLayoutConstraints;

   c19->left.RightOf      (_groupbox, 5);

   c19->top.SameAs     	 (_buttonDOWN, wxBottom,5);

   c19->height.AsIs();

   c19->width.AsIs();

   _buttonSHOW->SetConstraints(c19);



   //text

   wxLayoutConstraints *c8 = new wxLayoutConstraints;

   c8->left.SameAs       (_panel1, wxLeft);

   c8->bottom.SameAs     (_panel1, wxBottom,5);

   c8->height.AsIs();

   c8->width.AsIs();

   _text1->SetConstraints(c8);


   Layout();

}

LayerOrder::~LayerOrder()
{
}

void LayerOrder::SetupWindow()
{
	_listbox->Clear();
	// set the items in the right order in the listbox
	for (int i = 0; i < MAX_LAYERS; i++)
	{
		for (int x = 0; x < MAX_LAYERS; x++)
		{
			if (GLOB->SETUP->Get_Order(x) == i)
			{
				char _buf[60];
				// insert the layer in the listbox
				sprintf(_buf, "%03d  %s", x, GLOB->SETUP->Get_Name(x));
				_listbox->Append(_buf);//AddString(buf);
			}
		}
	}
}

void LayerOrder::Update(wxUpdateUIEvent&)
{
   if (IsShown())
   {
      bool bad=false;

      // set the items in the right order in the listbox
      for (int i = 0; i < MAX_LAYERS; i++)
      {
            char _buf[60];
            sprintf(_buf, "%03d  %s",i, GLOB->SETUP->Get_Name(i));
            if (_listbox->FindString(_buf) == -1)
            {
               bad=true;
               break;
            }
      }
      if (bad)
      {
			SetupWindow();
      }
   }
}


void LayerOrder::StoreOrder()
{
	char* _buf = new char[100];
	for (int i = 0; i < MAX_LAYERS; i++)
	{
		strcpy(_buf,_listbox->GetString(i));
		_buf[3] = '\0';
		GLOB->SETUP->Set_Order(atoi(_buf), i);
	}
	delete _buf;
}

void LayerOrder::CmOk(wxCommandEvent&)
{
	int selection = _listbox->GetSelection();
	_listbox->SetSelection(selection, FALSE);

	StoreOrder();
	Show(FALSE);
	_parent->Redraw();
}

void LayerOrder::CmShow(wxCommandEvent&)
{
	int selection = _listbox->GetSelection();
	_listbox->SetSelection(selection, FALSE);

	StoreOrder();
	_parent->Redraw();
}

void LayerOrder::OnCloseWindow(wxCloseEvent& event)
{
   wxCommandEvent a = wxCommandEvent();
   CmCancel(a);
}

void LayerOrder::CmCancel(wxCommandEvent&)
{
	Show(FALSE);

// listbox schoon maken.
	_listbox->Clear();

// oude waarden uit de setup lezen.
	SetupWindow();
	_parent->Redraw();
}

void LayerOrder::CmUp(wxCommandEvent&)
{
	int selection = _listbox->GetSelection();
	if (selection == -1)
		_messagehandler->info("You must select a layer first.","No Layer Selected");
	else
	{
		if (selection!=0)
		{
			if (selection == 99)	_buttonDOWN->Enable(TRUE);

			char* _string = new char[100];
			strcpy(_string,_listbox->GetString(_listbox->GetSelection()-1));

			_listbox->SetString(selection-1,_listbox->GetString(selection));
			_listbox->SetString(selection,_string);
			_listbox->Show(TRUE);
			delete _string;

			// to keep the original selection selected
			_listbox->SetSelection(selection, FALSE);
			_listbox->SetSelection(selection-1, TRUE);
			if (selection == 1)
         	_buttonUP->Enable(FALSE);
		}
		else
		{
			_messagehandler->info("layer is allready at the highest possible order.","Error:orderdlg.cpp");
		}
	}
}

void LayerOrder::CmDown(wxCommandEvent&)
{
	int selection = _listbox->GetSelection();

	if (selection == -1)
		_messagehandler->info("You must select a layer first.","No Layer Selected");
	else
	{
		if (selection!=99)
		{
			if (selection == 0)	_buttonUP->Enable(TRUE);

			char* _string = new char[100];
			strcpy(_string,_listbox->GetString(_listbox->GetSelection()+1));

			_listbox->SetString(selection+1,_listbox->GetString(selection));
			_listbox->SetString(selection,_string);
			_listbox->Show(TRUE);
			delete _string;

			// to keep the original selection selected
			_listbox->SetSelection(selection, FALSE);
			_listbox->SetSelection(selection+1, TRUE);

			if (selection == 98)	_buttonDOWN->Enable(FALSE);
		}
		else
		{
			_messagehandler->info("layer is allready at the lowest possible order.","Error:orderdlg.cpp");
		}
	}
}

void LayerOrder::CmListBox(wxCommandEvent&)
{
	int selection = _listbox->GetSelection();
	_buttonUP->Enable(selection != 0);
	_buttonDOWN->Enable(selection != 99);
}

