/*
 *	Program			layprops.cpp
 *	Purpose			implementation of the layer properties dialog for the wxWindows
 *						version of the GDSII tool.
 * Author         Mollien J.C.
 *		"Sunday morning I'm waking up, can't even focus on my coffee cup
 *     I don't know whose bed I'm in, where do I start, where do I begin?"
 * Modified			13-10-1997
 * Created			13-10-1997
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "wxwin.h"
#include "mainwin.h"
#include "lsetup.h"
#include "misc.h"		// for maximum layers
#include "chandbas.h"
#include "windrv.h"
#include "patdlg.h"

#include "layprops.h"
#include <wx/statline.h>


#define LPD_B_W 	20
#define LPD_B_H 	20

#ifdef __UNIX__
	#define LPD_PATTERNBOX_X 	530
	#define LPD_PATTERNBOX_W	50

	#define LPD_W  		730
	#define LPD_H	 	 	432

#else
	#define LPD_PATTERNBOX_X 	500
	#define LPD_PATTERNBOX_W	30

	#define LPD_W  		630
	#define LPD_H	 	 	332

#endif // ifdef __UNIX__  .. else ..


const int    LAY_BUTTON_HIDE     = wxID_HIGHEST + 4801 ;

const int    LAY_BUTTON_CANCEL   = wxID_HIGHEST + 4802 ;

const int    LAY_BUTTON_VISIBLE   = wxID_HIGHEST + 4803 ;

const int    LAY_BUTTON_READ   = wxID_HIGHEST + 4804 ;

const int    LAY_BUTTON_SELECT = wxID_HIGHEST + 4805 ;

const int    LAY_BUTTON_PREV   = wxID_HIGHEST + 4806 ;

const int    LAY_BUTTON_NEXT   = wxID_HIGHEST + 4807 ;

const int    LAY_EDIT_NAME     = wxID_HIGHEST + 4808 ;

const int    LAY_UPDATE_DIALOG    = wxID_HIGHEST + 4810 ;

const int    LAY_BUTTON_REDRAW = wxID_HIGHEST + 4811 ;

const int    ID_AVAILABLE = wxID_HIGHEST + 5000;
const int    ID_LAYERNAME = wxID_HIGHEST + 5100;
const int    ID_VISIBLE   = wxID_HIGHEST + 5200;
const int    ID_SELECTABLE = wxID_HIGHEST + 5300;
const int    ID_READ       = wxID_HIGHEST + 5400;
const int    ID_FILLCOLOR  = wxID_HIGHEST + 5500;
const int    ID_STROKECOLOR = wxID_HIGHEST + 5600;
const int    ID_PATTERN   = wxID_HIGHEST + 5700;
const int    ID_INMAP   = wxID_HIGHEST + 5800;
const int    ID_OUTMAP   = wxID_HIGHEST + 5900;

BEGIN_EVENT_TABLE(LayerPropertiesDialog, wxDialog)

  EVT_UPDATE_UI(LAY_UPDATE_DIALOG, LayerPropertiesDialog::Update)

  EVT_BUTTON    (LAY_BUTTON_HIDE, LayerPropertiesDialog::CmHide)

  EVT_BUTTON    (LAY_BUTTON_CANCEL, LayerPropertiesDialog::CmCancel)

  EVT_BUTTON    (LAY_BUTTON_VISIBLE, LayerPropertiesDialog::CmVisible)

  EVT_BUTTON    (LAY_BUTTON_READ, LayerPropertiesDialog::CmRead)

  EVT_BUTTON    (LAY_BUTTON_SELECT, LayerPropertiesDialog::CmSelect)

  EVT_BUTTON    (LAY_BUTTON_PREV, LayerPropertiesDialog::CmPrev)

  EVT_BUTTON    (LAY_BUTTON_NEXT, LayerPropertiesDialog::CmNext)

  EVT_BUTTON    (LAY_BUTTON_REDRAW, LayerPropertiesDialog::CmRedraw)

  EVT_CUSTOM_RANGE(wxEVT_COMMAND_TEXT_ENTER, ID_LAYERNAME, ID_LAYERNAME + 9, LayerPropertiesDialog::Cm_but)

  EVT_CUSTOM_RANGE(wxEVT_COMMAND_CHECKBOX_CLICKED  , ID_VISIBLE, ID_VISIBLE + 9, LayerPropertiesDialog::Cm_but)
  EVT_CUSTOM_RANGE(wxEVT_COMMAND_CHECKBOX_CLICKED  , ID_SELECTABLE, ID_SELECTABLE + 9, LayerPropertiesDialog::Cm_but)
  EVT_CUSTOM_RANGE(wxEVT_COMMAND_CHECKBOX_CLICKED  , ID_READ, ID_READ + 9, LayerPropertiesDialog::Cm_but)

  EVT_CUSTOM_RANGE(wxEVT_COMMAND_BUTTON_CLICKED, ID_FILLCOLOR, ID_FILLCOLOR + 9, LayerPropertiesDialog::Cm_F_BitB)
  EVT_CUSTOM_RANGE(wxEVT_COMMAND_BUTTON_CLICKED, ID_STROKECOLOR, ID_STROKECOLOR + 9, LayerPropertiesDialog::Cm_B_BitB)
  EVT_CUSTOM_RANGE(wxEVT_COMMAND_BUTTON_CLICKED, ID_PATTERN, ID_PATTERN + 9, LayerPropertiesDialog::Cm_P_BitB)

  EVT_CUSTOM_RANGE(wxEVT_COMMAND_TEXT_ENTER , ID_INMAP, ID_INMAP + 9, LayerPropertiesDialog::Cm_but)
  EVT_CUSTOM_RANGE(wxEVT_COMMAND_TEXT_ENTER , ID_OUTMAP, ID_OUTMAP + 9, LayerPropertiesDialog::Cm_but)

  EVT_CHAR(LayerPropertiesDialog::OnChar)
  EVT_CHAR_HOOK(LayerPropertiesDialog::OnCharHook)

END_EVENT_TABLE()


//****** IMPLEMENTATION OF THE LAYER PROPERTIES DIALOG CLASS ******

// Constructor
LayerPropertiesDialog::LayerPropertiesDialog(GDSMainWindow* parent,char* title,long style,char* name):
	wxDialog(parent,LAY_UPDATE_DIALOG,title,wxPoint(0,0),wxSize(LPD_W,LPD_H),style,name)
{

	_setup = new Setup();
	_back_setup = new Setup();
	_parent = parent;

   _brush 	= new wxBrush(); // must be done.

	// no standard colours
	// set extra part of dialog true
	_cdata 	= new wxColourData();
	_cdata->SetChooseFull(TRUE);

	long style_noresize;
#ifdef __UNIX__
	style_noresize = (wxTHICK_FRAME | wxCAPTION | wxUSER_COLOURS);
#else
	style_noresize = (wxTHICK_FRAME | wxSYSTEM_MENU | wxCAPTION);
#endif

	_patternDialog = new PatternDialog(this,"Patterns",style_noresize);

	// make colour dialog
	_cd = new wxColourDialog(this,_cdata);

   wxFlexGridSizer *alllayerline = new wxFlexGridSizer( 10 );
   alllayerline->AddGrowableCol(1);
   wxSizer *all = new wxBoxSizer( wxVERTICAL );  
   all->Add( alllayerline, 0, wxGROW );

	_numberlabel		= new wxStaticText(this,-1,"Nr" );
    alllayerline->Add( _numberlabel, 0, wxALIGN_CENTRE|wxALL, 2 );
	_namelabel			= new wxStaticText(this,-1,"Name");
    alllayerline->Add( _namelabel, 0, wxALIGN_CENTRE|wxALL, 2 );
	_visiblelabel		= new wxStaticText(this,-1,"Vis");
    alllayerline->Add( _visiblelabel, 0, wxALIGN_CENTRE|wxALL, 2 );
	_selectablelabel	= new wxStaticText(this,-1,"Sel");
    alllayerline->Add( _selectablelabel, 0, wxALIGN_CENTRE|wxALL, 2 );
	_readlayerlabel	= new wxStaticText(this,-1,"Read");
    alllayerline->Add( _readlayerlabel, 0, wxALIGN_CENTRE|wxALL, 2 );
	_filllabel			= new wxStaticText(this,-1,"Fill");
    alllayerline->Add( _filllabel, 0, wxALIGN_CENTRE|wxALL, 2 );
	_borderlabel		= new wxStaticText(this,-1,"Stroke");
    alllayerline->Add( _borderlabel, 0, wxALIGN_CENTRE|wxALL, 2 );
	_patternlabel		= new wxStaticText(this,-1,"Pattern");
    alllayerline->Add( _patternlabel, 0, wxALIGN_CENTRE|wxALL, 2 );
	_mappinglabel		= new wxStaticText(this,-1,"inMap");
    alllayerline->Add( _mappinglabel, 0, wxALIGN_CENTRE|wxALL, 2 );
	_mappinglabel		= new wxStaticText(this,-1,"outMap");
    alllayerline->Add( _mappinglabel, 0, wxALIGN_CENTRE|wxALL, 2 );

	// make the controls

	for (int i = 0; i < 10; i++)
	{
    	char nrbuf[10];

		sprintf(nrbuf,"%03d",i+1);

      _numberbox[i] = new wxStaticText( this, -1, nrbuf, wxDefaultPosition, wxDefaultSize, 0 );
      alllayerline->Add( _numberbox[i], 0, wxALIGN_CENTER_VERTICAL |wxALIGN_CENTRE|wxALL|wxGROW, 0 );

      _namebox[i] = new wxTextCtrl( this, ID_LAYERNAME + i, "", wxDefaultPosition, wxSize(50,20), 0 );
      alllayerline->Add( _namebox[i], 0, wxALIGN_CENTRE|wxALL|wxGROW, 0 );

      _visiblebox[i] = new wxCheckBox( this, ID_VISIBLE + i, "", wxDefaultPosition, wxDefaultSize, 0 );
      alllayerline->Add( _visiblebox[i], 0, wxALIGN_CENTRE|wxALL, 1 );

      _selectablebox[i] = new wxCheckBox( this, ID_SELECTABLE + i, "", wxDefaultPosition, wxDefaultSize, 0 );
      alllayerline->Add( _selectablebox[i], 0, wxALIGN_CENTRE|wxALL, 1 );

      _readlayerbox[i] = new wxCheckBox( this, ID_READ + i, "", wxDefaultPosition, wxDefaultSize, 0 );
      alllayerline->Add( _readlayerbox[i], 0, wxALIGN_CENTRE|wxALL, 1 );

      _fillbut[i] = new wxButton( this, ID_FILLCOLOR + i, _T(""), wxDefaultPosition, wxSize(20,20),wxSIMPLE_BORDER  );
      alllayerline->Add( _fillbut[i], 0, wxALIGN_CENTRE|wxALL, 1 );

      _borderbut[i] = new wxButton( this, ID_STROKECOLOR + i, _T(""), wxDefaultPosition, wxSize(20,20),wxSIMPLE_BORDER  );
      alllayerline->Add( _borderbut[i], 0, wxALIGN_CENTRE|wxALL, 1 );

      _p_bitm[i]     = new wxBitmap(LPD_PATTERNBOX_W,LPD_B_H,-1);
      _patternbut[i] = new wxBitmapButton( this, ID_PATTERN + i, *_p_bitm[i], wxDefaultPosition, wxSize(40,20),wxSIMPLE_BORDER  );
      alllayerline->Add( _patternbut[i], 0, wxALIGN_CENTRE|wxALL, 1 );

      _gdsinmap[i] = new wxTextCtrl( this, ID_INMAP + i, "", wxDefaultPosition, wxSize(30,-1),wxTE_PROCESS_ENTER );
      alllayerline->Add( _gdsinmap[i], 0, wxALIGN_CENTRE|wxALL, 1 );

      _gdsoutmap[i] = new wxTextCtrl( this, ID_OUTMAP + i, "", wxDefaultPosition, wxSize(30,-1),wxTE_PROCESS_ENTER );
      alllayerline->Add( _gdsoutmap[i], 0, wxALIGN_CENTRE|wxALL, 1 );
   }

   wxStaticBox *item1 = new wxStaticBox( this, -1, "" );
   wxSizer *botline = new wxStaticBoxSizer( item1, wxVERTICAL );  

   wxSizer *botline1 = new wxBoxSizer( wxHORIZONTAL );  
   botline->Add( botline1 );

	_buttonVISIBLE  = new wxButton(this,LAY_BUTTON_VISIBLE,"Hide All",wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT );
    botline1->Add( _buttonVISIBLE, 0, wxALIGN_CENTRE|wxALL, 0 );
	_buttonSELECT	= new wxButton(this,LAY_BUTTON_SELECT,	"None Selectable",wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT );
    botline1->Add( _buttonSELECT, 0, wxALIGN_CENTRE|wxALL, 0 );
	_buttonREAD 	= new wxButton(this,LAY_BUTTON_READ,	"Read Non",wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT );
   botline1->Add( _buttonREAD, 0, wxALIGN_CENTRE|wxALL, 0 );

   botline->Add( new wxStaticLine( this, -1, wxDefaultPosition, wxSize(200,2), wxLI_HORIZONTAL ) , 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT|wxALL, 2);

   wxSizer *botline2 = new wxBoxSizer( wxHORIZONTAL );  
   botline->Add( botline2 );

   _buttonHIDE     = new wxButton(this,LAY_BUTTON_HIDE,"Hide",wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT );
    botline2->Add( _buttonHIDE, 0, wxALIGN_CENTRE|wxALL, 0 );
	_buttonCANCEL 	= new wxButton(this,LAY_BUTTON_CANCEL,	"Cancel",wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
    botline2->Add( _buttonCANCEL, 0, wxALIGN_CENTRE|wxALL, 0 );

    botline2->Add( new wxStaticLine( this, -1, wxDefaultPosition, wxSize(2,20), wxLI_VERTICAL ) , 0, wxALIGN_CENTRE|wxALL, 2);

	_buttonPREV 	= new wxButton(this,LAY_BUTTON_PREV,	"< Previous",wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
    botline2->Add( _buttonPREV, 0, wxALIGN_CENTRE|wxALL, 0 );
	_buttonNEXT		= new wxButton(this,LAY_BUTTON_NEXT,	"Next >",wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
    botline2->Add( _buttonNEXT, 0, wxALIGN_CENTRE|wxALL, 0 );
	_buttonREDRAW	= new wxButton(this,LAY_BUTTON_REDRAW,	"Apply",wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
    botline2->Add( _buttonREDRAW, 0, wxALIGN_CENTRE|wxALL, 0 );
    
   all->Add( botline );

   SetSizer( all );

   all->Fit( this );
   SetAutoLayout( TRUE );
    
   all->SetSizeHints( this );

	_startvalue = 0;

	Centre(wxBOTH);
	Layout();

}

LayerPropertiesDialog::~LayerPropertiesDialog()
{
	delete _brush;
	delete _setup;
	delete _back_setup;

   delete _cdata;
   delete _cd;

	for (int i = 0; i < 10; i++)
	{
      delete _p_bitm[i]; 
   }
}

void LayerPropertiesDialog::SetupWindow(Setup* prev_setup)
{
	// make a backup from the setup for if the user presses cancel
	_prev_setup=prev_setup; //to compare the new with the old setup
	*_setup = *prev_setup;
	*_back_setup = *prev_setup;

	_buttonPREV->Enable();
	_buttonNEXT->Enable();

	if (_startvalue <= 0)
		_buttonPREV->Disable();

	if (_startvalue + 10 >= MAX_LAYERS-1)
		_buttonNEXT->Disable();

	FillControls(_startvalue);

	// center the dialog
	Centre(wxBOTH);
   Show(true);
   SetFocus();
}

void LayerPropertiesDialog::Update(wxUpdateUIEvent&)
{

		if (IsShown())
      {

			if (*_setup != *_prev_setup)
         {
   	      *_setup = *_prev_setup;
         	FillControls(_startvalue);
         }
      }
      return;
}

// redraw the dialog
void LayerPropertiesDialog::Redraw(wxCommandEvent &)
{
	FillControls(_startvalue);
}

// initialize the controls with the right values
void LayerPropertiesDialog::FillControls(int start)
{
    wxColour _kleurtje;
    wxColour _kleurtje2;

	int All_Hided = true;
   int All_Read = true;
	for (int i = start; i < start+10; i++)
	{
		char buf[10];
		int x = i - _startvalue;

		sprintf(buf, "%i", i);
      _numberbox[x]->SetForegroundColour( _setup->Get_Available(i) ? wxColour(255,0,0) : wxColour(0,0,0) );
		_numberbox[x]->SetLabel(buf);

      _namebox[x]->SetForegroundColour( _setup->Get_Available(i) ? wxColour(255,0,0) : wxColour(0,0,0) );
		_namebox[x]->SetValue(_setup->Get_Name(i));

		if (_setup->Get_Visible(i))
			All_Hided = false;
		_visiblebox[x]->SetValue(_setup->Get_Visible(i) ? true : false);

		if ((! _setup->Get_Visible(i)) || (! _setup->Get_Selectable(i)))
      	_selectablebox[x]->SetValue(false);
		else _selectablebox[x]->SetValue(true);

		if (! _setup->Get_Read(i))
      	_readlayerbox[x]->SetValue(false);
		else
      {
			All_Read = false;
      	_readlayerbox[x]->SetValue(true);
      }

      _kleurtje = ltowxc(_setup->Get_FillColor(i));
      _brush->SetColour(_kleurtje);
		_fillbut[x]->SetBackgroundColour(_kleurtje);

      _kleurtje = ltowxc(_setup->Get_BorderColor(i));
   	  _borderbut[x]->SetBackgroundColour(_kleurtje);

      wxMemoryDC dcmemtemp;
      dcmemtemp.SelectObject(*_p_bitm[x]);
      _kleurtje = ltowxc(_setup->Get_FillColor(i));
      _brush->SetColour(_kleurtje);
      dcmemtemp.SetBackground(*_brush);
      dcmemtemp.Clear();

      _patternbut[x]->SetBitmapLabel(*_p_bitm[x]);//*(GLOB->FILLPAT->GetPattern(_setup->Get_Brush(i))));

      _kleurtje2 = ltowxc(_setup->Get_BorderColor(i));
		wxPen _Pen = wxPen(_kleurtje2,1,wxSOLID);

      dcmemtemp.SetPen(_Pen);


		wxColour white("WHITE");
		wxColour black("BLACK");

      dcmemtemp.SetTextBackground(_kleurtje);
      dcmemtemp.SetTextForeground(black);

      wxBrush _brushbitm(*(GLOB->FILLPAT->GetPattern(_setup->Get_Brush(i))));
      dcmemtemp.SetBrush(_brushbitm);

      //convert monogrome to color bitmap
      //in WinNt any monochrome bitmapbrush is drawn to the dest. dc back and fore ground colors
      dcmemtemp.DrawRectangle(0,0,LPD_PATTERNBOX_W,LPD_B_H);

      dcmemtemp.SetPen(wxNullPen);
   	dcmemtemp.SelectObject(wxNullBitmap);

		sprintf(buf, "%i", _setup->Get_GdsInMapping(i));
		_gdsinmap[x]->SetValue(buf);

		sprintf(buf, "%i", _setup->Get_GdsOutMapping(i));
		_gdsoutmap[x]->SetValue(buf);

		_fillbut[x]->Refresh();
		_borderbut[x]->Refresh();
		_patternbut[x]->Refresh();

	}

	if (All_Hided)
	{
		_buttonVISIBLE->SetLabel("Show All");
		_buttonSELECT->SetLabel("All Selectable");
	}

	if (All_Read)
	{
		_buttonREAD->SetLabel("Read All");
	}

   //will update dialog and all its (bitmap)buttons
   Refresh(false);
}

// get the values from the controls and store them
void LayerPropertiesDialog::StoreControls(int start)
{
   int i;
	for ( i = start; i < start+10; i++)
	{
		int x = i - _startvalue;
		char buf[50];

		strcpy(buf,_namebox[x]->GetValue());
		_setup->Set_Name(i, buf);
		_setup->Set_Visible			(i,(bool)_visiblebox[x]->GetValue());
		_setup->Set_Selectable		(i,(bool)(_selectablebox[x]->GetValue()
												&& _visiblebox[x]->GetValue()));
		_setup->Set_Read				(i,(bool)(_readlayerbox[x]->GetValue()));
		_setup->Set_BorderColor		(i,wxctol(_borderbut[x]->GetBackgroundColour()));
		_setup->Set_FillColor		(i,wxctol(_fillbut[x]->GetBackgroundColour()));
		strcpy(buf,_gdsinmap[x]->GetValue());
		_setup->Set_GdsInMapping	(i,atoi(buf));
		strcpy(buf,_gdsoutmap[x]->GetValue());
		_setup->Set_GdsOutMapping	(i,atoi(buf));
	}

   for ( i = start; i < start+10; i++)
   {

     	if ((bool)((_setup->Get_Visible(i) != _prev_setup->Get_Visible(i)) 			||
						 (strcmp(_setup->Get_Name(i),_prev_setup->Get_Name(i)))		||
						 (_setup->Get_Selectable(i) != _prev_setup->Get_Selectable(i))		||
						 (_setup->Get_Read(i) != _prev_setup->Get_Read(i))		||
						 (_setup->Get_BorderColor(i) != _prev_setup->Get_BorderColor(i))	||
						 (_setup->Get_FillColor(i) != _prev_setup->Get_FillColor(i))		||
						 (_setup->Get_Brush(i) != _prev_setup->Get_Brush(i) )            ||
						 (_setup->Get_GdsInMapping(i) != _prev_setup->Get_GdsInMapping(i)) ||
						 (_setup->Get_GdsOutMapping(i) != _prev_setup->Get_GdsOutMapping(i))
      	))
     	{
      	GLOB->CMDH->DoCommand("setlayer %d",i);
         if (strcmp(_setup->Get_Name(i),_prev_setup->Get_Name(i)))
            GLOB->CMDH->DoCommand("name {%s}",_setup->Get_Name(i));
         if (_setup->Get_Visible(i) != _prev_setup->Get_Visible(i))
            GLOB->CMDH->DoCommand("visible %d",_setup->Get_Visible(i));
         if (_setup->Get_Selectable(i) != _prev_setup->Get_Selectable(i))
            GLOB->CMDH->DoCommand("selectable %d",_setup->Get_Selectable(i));
         if (_setup->Get_Read(i) != _prev_setup->Get_Read(i))
            GLOB->CMDH->DoCommand("readlayer %d",_setup->Get_Read(i));
         if (_setup->Get_BorderColor(i) != _prev_setup->Get_BorderColor(i))
         {
            long color = _setup->Get_BorderColor(i);
            int R, G, B;
            R = (int)(color % 256);
            G = (int)((color >> 8) % 256);
            B = (int)((color >> 16) % 256);
            GLOB->CMDH->DoCommand("bordercolor %d %d %d", R, G, B);
         }
         if (_setup->Get_FillColor(i) != _prev_setup->Get_FillColor(i))
         {
            long color = _setup->Get_FillColor(i);
            int R, G, B;
            R = (int)(color % 256);
            G = (int)((color >> 8) % 256);
            B = (int)((color >> 16) % 256);

            GLOB->CMDH->DoCommand("fillcolor %d %d %d", R, G, B);
         }
         if (_setup->Get_Brush(i) != _prev_setup->Get_Brush(i))
            GLOB->CMDH->DoCommand("brush %d", _setup->Get_Brush(i) );
         if (_setup->Get_GdsInMapping(i)!=_prev_setup->Get_GdsInMapping(i))
            GLOB->CMDH->DoCommand("gdsinmap %d",_setup->Get_GdsInMapping(i));
         if (_setup->Get_GdsOutMapping(i)!=_prev_setup->Get_GdsOutMapping(i))
            GLOB->CMDH->DoCommand("gdsoutmap %d",_setup->Get_GdsOutMapping(i));
     	}
   }
}

void LayerPropertiesDialog::CmHide(wxCommandEvent &)
{
	Show(FALSE);

   // to make sure that during recording something will be visible
   GLOB->CMDH->DoCommand("draw");

}

void LayerPropertiesDialog::CmCancel(wxCommandEvent &)
{

	*_setup = *_back_setup;

    wxCommandEvent a = wxCommandEvent();
	CmHide(a);
}

void LayerPropertiesDialog::CmVisible(wxCommandEvent &)
{
	char buf[10];
	strcpy(buf,_buttonVISIBLE->GetLabel());
	if (strcmp(buf, "Hide All") == 0)
	{
		_setup->Set_All_Layers_Invisible();
		_buttonVISIBLE->SetLabel("Show All");
      GLOB->CMDH->DoCommand("visible 0 all");
	}
	else
	{
		_setup->Set_All_Layers_Visible();
		_buttonVISIBLE->SetLabel("Hide All");
      GLOB->CMDH->DoCommand("visible 1 all");
	}
	FillControls(_startvalue);
}

void LayerPropertiesDialog::CmRead(wxCommandEvent &)
{
	char buf[10];
	strcpy(buf,_buttonREAD->GetLabel());
	if (strcmp(buf, "Read Non") == 0)
	{
		_setup->Set_All_Layers_ReadOff();
		_buttonREAD->SetLabel("Read All");
      GLOB->CMDH->DoCommand("readlayer 0 all");
	}
	else
	{
		_setup->Set_All_Layers_ReadOn();
		_buttonREAD->SetLabel("Read Non");
      GLOB->CMDH->DoCommand("readlayer 1 all");
	}
	FillControls(_startvalue);
}

void LayerPropertiesDialog::CmSelect(wxCommandEvent &)
{
	char buf[20];
	strcpy(buf,_buttonSELECT->GetLabel());
	if (strcmp(buf, "All Selectable") == 0)
	{
		_setup->Set_All_Layers_Selectable();
		_buttonSELECT->SetLabel("None Selectable");
      GLOB->CMDH->DoCommand("selectable 1 all");
	}
	else
	{
		_setup->Set_All_Layers_NotSelectable();
		_buttonSELECT->SetLabel("All Selectable");
      GLOB->CMDH->DoCommand("selectable 0 all");
	}
	FillControls(_startvalue);
}

void LayerPropertiesDialog::CmPrev(wxCommandEvent &)
{
	_buttonNEXT->Enable();
	if (_startvalue - 10 >= 0)
	{
		StoreControls(_startvalue);
		_startvalue -= 10;
		FillControls(_startvalue);
	}

	if (_startvalue <= 0)
		_buttonPREV->Disable();
   SetFocus();
   Show(true);
   Refresh();
}

void LayerPropertiesDialog::CmNext(wxCommandEvent &)
{
	_buttonPREV->Enable();
	if (_startvalue + 10 < MAX_LAYERS)
	{
		StoreControls(_startvalue);
		_startvalue += 10;
		FillControls(_startvalue);
	}

	if (_startvalue + 10 >= MAX_LAYERS-1)
		_buttonNEXT->Disable();
   SetFocus();
   Show(true);
   Refresh();
}

void LayerPropertiesDialog::CmRedraw(wxCommandEvent &)
{
		 GLOB->CMDH->DoCommand("draw");
}

wxColour LayerPropertiesDialog::ltowxc(long colour)
{
	unsigned char	red,green,blue;

	blue 	= colour / 65536;
	green = (colour % 65536) / 256;
	red 	= colour % 256;

	return wxColour(red,green,blue);
}

long LayerPropertiesDialog::wxctol(wxColour colour)
{
	long colour_l = 0;
	colour_l += 65536*colour.Blue();
	colour_l += 256*colour.Green();
	colour_l += colour.Red();
	return colour_l;
}

void LayerPropertiesDialog::Cm_B_BitB(wxEvent& event)
{
   if (_cd->ShowModal()==wxID_OK)
   {
		wxColour col;
      *_cdata = _cd->GetColourData();
      col = _cdata->GetColour();
	   _brush->SetColour(col);
      int x=event.GetId();
		_borderbut[x - ID_STROKECOLOR]->SetBackgroundColour(col);
		StoreControls(_startvalue);
		FillControls(_startvalue);
   }
   SetFocus();
   Show(true);
   Refresh(true);
}

void LayerPropertiesDialog::Cm_F_BitB(wxEvent& event)
{
   if (_cd->ShowModal()==wxID_OK)
   {
		wxColour col;
      *_cdata = _cd->GetColourData();
      col = _cdata->GetColour();
	   _brush->SetColour(col);
      int x=event.GetId();
		_fillbut[x-ID_FILLCOLOR]->SetBackgroundColour(col);
		StoreControls(_startvalue);
		FillControls(_startvalue);
   }
   SetFocus();
   Show(true);
   Refresh();
}

void LayerPropertiesDialog::Cm_P_BitB(wxEvent& event)
{
   //the button pressed given indirect the layer
   int x = event.GetId() - ID_PATTERN; //which button


   int choosen=1;
   choosen=_patternDialog->ShowModal();
	_patternbut[x]->SetBitmapLabel(*_p_bitm[x]);//*(GLOB->FILLPAT->GetPattern(choosen)));
   {

		wxColour white("WHITE");
		wxColour black("BLACK");

   	_setup->Set_Brush(x+_startvalue,choosen);
		wxColour col;
      col = ltowxc(_setup->Get_FillColor(x+_startvalue));
      wxMemoryDC dcmemtemp2;

		wxPen _Pen = wxPen(col,1,wxSOLID);
      dcmemtemp2.SelectObject(*_p_bitm[x]);
      _brush->SetColour(black);
      dcmemtemp2.SetBackground(*_brush);
      dcmemtemp2.Clear();
      dcmemtemp2.SetPen(_Pen);

      dcmemtemp2.SetTextBackground(black);
      dcmemtemp2.SetTextForeground(col);

      wxBrush _brushbitm(*(GLOB->FILLPAT->GetPattern(choosen)));
      dcmemtemp2.SetBrush(_brushbitm);

      //convert monogrome to color bitmap
      //in WinNt any monochrome bitmapbrush is drawn to the dest. dc back and fore ground colors
	   dcmemtemp2.SetTextForeground(white);
	   dcmemtemp2.SetTextBackground(black);
      dcmemtemp2.DrawRectangle(0,0,LPD_PATTERNBOX_W,LPD_B_H);

   	dcmemtemp2.SelectObject(wxNullBitmap);

   	StoreControls(_startvalue);
		FillControls(_startvalue);
   }
   _patternDialog->Close();

   SetFocus();
   Show(true);
   Refresh();
}

void LayerPropertiesDialog::Cm_but(wxEvent& event)
{
   //the button pressed given indirect the layer
   int x=event.GetId(); //which button

	StoreControls(_startvalue);
}

void LayerPropertiesDialog::OnChar(wxKeyEvent& event)
{
    if ( event.GetKeyCode() == WXK_RETURN )
    {
		 StoreControls(_startvalue);
		 GLOB->CMDH->DoCommand("draw");
    }
    else if ( event.GetKeyCode() == WXK_NEXT)
    {
        wxCommandEvent a = wxCommandEvent();
        CmNext(a);
    }
    else if ( event.GetKeyCode() == WXK_PRIOR)
    {
        wxCommandEvent a = wxCommandEvent();
        CmPrev(a);
    }
    else
    {
       // key code is within legal range. we call event.Skip() so the
       // event can be processed either in the base wxWindows class
       // or the native control.

       event.Skip();
    }
}

void LayerPropertiesDialog::OnCharHook(wxKeyEvent& event)
{
    if ( event.GetKeyCode() == WXK_RETURN )
    {
		 StoreControls(_startvalue);
		 GLOB->CMDH->DoCommand("draw");
    }
    else if ( event.GetKeyCode() == WXK_NEXT)
    {
        wxCommandEvent a = wxCommandEvent();
        CmNext(a);
    }
    else if ( event.GetKeyCode() == WXK_PRIOR)
    {
        wxCommandEvent a = wxCommandEvent();
        CmPrev(a);
    }
    else
    {
       // key code is within legal range. we call event.Skip() so the
       // event can be processed either in the base wxWindows class
       // or the native control.

       event.Skip();
    }
}


