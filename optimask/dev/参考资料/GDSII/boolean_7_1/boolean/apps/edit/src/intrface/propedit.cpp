#ifdef __GNUG__
#pragma implementation
#endif

#include "wxwin.h"

#include "math.h"
#include "propedit.h"



const int    PROP_BUTTON_ADD = wxID_HIGHEST + 3401;
const int    PROP_BUTTON_OK  = wxID_HIGHEST + 3402;
const int    PROP_BUTTON_CANCEL = wxID_HIGHEST + 3403;
const int    PROP_BUTTON_REMOVE = wxID_HIGHEST + 3404;

wxPropertyValidatorRegistry myListValidatorRegistry;

BEGIN_EVENT_TABLE(PropEditDialog, wxDialog)
  EVT_BUTTON    (PROP_BUTTON_ADD,		PropEditDialog::Add)
  EVT_BUTTON    (PROP_BUTTON_OK,       PropEditDialog::Ok)
  EVT_BUTTON    (PROP_BUTTON_CANCEL,   PropEditDialog::Cancel)
  EVT_BUTTON    (PROP_BUTTON_REMOVE,   PropEditDialog::Remove)
  EVT_CLOSE     (PropEditDialog::OnCloseWindow)
END_EVENT_TABLE()

#define PROP_HSIZE		400
#define PROP_VSIZE		200

PropEditDialog::PropEditDialog(wxWindow* parent,const wxString& title,long style ,const wxString& name,wxPropertySheet* wxpropsheet):
		wxDialog(parent,-1,title,wxPoint(0,0),wxSize(PROP_HSIZE,PROP_VSIZE),style,name)
{

	_parent = parent;
	_panel 	  	= new wxPanel	(this,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER,"proppanel");

   Register=false;
	RegisterValidators();

   m_sheet = wxpropsheet;
	view = new MywxPropertyListView(NULL,
   		  wxPROP_BUTTON_CHECK_CROSS|wxPROP_DYNAMIC_VALUE_FIELD|wxPROP_PULLDOWN|wxPROP_SHOWVALUES);

   wxPropertyListPanel*  proppanel = new wxPropertyListPanel(view, _panel,
   wxPoint(-1, -1), wxSize(400, 500), wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODELESS);
   view->AddRegistry(&myListValidatorRegistry);
   view->ShowView(m_sheet,proppanel);



	_addButton 	= new wxButton	(_panel,PROP_BUTTON_ADD,"Add");
	_okButton 	= new wxButton	(_panel,PROP_BUTTON_OK,"Ok");
	_cancelButton 	= new wxButton	(_panel,PROP_BUTTON_CANCEL,"Cancel");
	_removeButton 	= new wxButton	(_panel,PROP_BUTTON_REMOVE,"Remove");



   this->SetAutoLayout(TRUE);

   // Set constraints for panel subwindow
   wxLayoutConstraints *c2 = new wxLayoutConstraints;
   c2->left.SameAs       (this, wxLeft,1);
   c2->top.SameAs        (this, wxTop,1);
   c2->right.SameAs      (this, wxRight, 1);
   c2->bottom.SameAs     (this, wxBottom, 1);
   _panel->SetConstraints(c2);


   // Set constraints for panel subwindow
   wxLayoutConstraints *c1 = new wxLayoutConstraints;
   c1->left.SameAs       (_panel, wxLeft,1);
   c1->top.SameAs        (_panel, wxTop,1);
   c1->right.SameAs      (_panel, wxRight, 1);
   c1->bottom.SameAs     (_okButton, wxTop, 1);
   proppanel->SetConstraints(c1);


   //okbutton
   wxLayoutConstraints *c8 = new wxLayoutConstraints;
   c8->left.SameAs       (_panel, wxLeft,5);
   c8->bottom.SameAs     (_panel, wxBottom,5);
   c8->height.AsIs();
   c8->width.AsIs();
   _okButton->SetConstraints(c8);



   //cancelbutton
   wxLayoutConstraints *c5 = new wxLayoutConstraints;
   c5->left.RightOf      (_okButton,10);
   c5->bottom.SameAs     (_panel, wxBottom,5);
   c5->height.AsIs();
   c5->width.AsIs();
   _cancelButton->SetConstraints(c5);



   //addbutton
   wxLayoutConstraints *c6 = new wxLayoutConstraints;
   c6->left.RightOf      (_cancelButton,10);
   c6->bottom.SameAs     (_panel, wxBottom,5);
   c6->height.AsIs();
   c6->width.AsIs();
   _addButton->SetConstraints(c6);



   //removebutton
   wxLayoutConstraints *c7 = new wxLayoutConstraints;
   c7->left.RightOf      (_addButton,10);
   c7->bottom.SameAs     (_panel, wxBottom,5);
   c7->height.AsIs();
   c7->width.AsIs();
   _removeButton->SetConstraints(c7);

   Centre(wxBOTH);

   Layout();

   ShowModal();


}



PropEditDialog::~PropEditDialog()
{

}



void PropEditDialog::Ok(wxCommandEvent &)
{
   MakeModal(FALSE);
	Show(FALSE);
}



void PropEditDialog::RegisterValidators(void)

{

  if (!Register)
  {  Register=true;
     myListValidatorRegistry.RegisterValidator((wxString)"real", new wxRealListValidator);
     myListValidatorRegistry.RegisterValidator((wxString)"string", new wxStringListValidator);
     myListValidatorRegistry.RegisterValidator((wxString)"integer", new wxIntegerListValidator);
     myListValidatorRegistry.RegisterValidator((wxString)"bool", new wxBoolListValidator);
     myListValidatorRegistry.RegisterValidator((wxString)"stringlist", new wxListOfStringsListValidator);
     wxStringList *strings2 = new wxStringList("lay1", "lay2", "lay3",NULL);
     myListValidatorRegistry.RegisterValidator((wxString)"layers",new wxStringListValidator(strings2));
  }

}



void PropEditDialog::OnCloseWindow(wxCloseEvent& event)

{
   MakeModal(FALSE);
	Show(FALSE);

}

void PropEditDialog::Add(wxCommandEvent &)
{
	long style = (wxTHICK_FRAME | wxSYSTEM_MENU | wxCAPTION);
   PropAddDialog a_prop(this,"add a property",style ,"propadd");
   if (a_prop.ShowModal())
   {
   	wxProperty* theprop=new wxProperty();
      if (a_prop.GetType().IsSameAs("bool"))
      {
         if (a_prop.GetValue().IsSameAs("true"))
         {
            wxPropertyValue aval((bool) true);
    	    wxString tmp = a_prop.GetName();
            theprop->SetName( tmp );
            theprop->SetValue(aval);
            tmp = a_prop.GetType();
            theprop->SetRole( tmp );
         }
         else
         {
            wxPropertyValue aval((bool) true);
	    wxString tmp = a_prop.GetName();
            theprop->SetName( tmp );
            theprop->SetValue(aval);
            tmp = a_prop.GetType();
            theprop->SetRole( tmp );
         }
      }
      else if (a_prop.GetType().IsSameAs("real"))
      {
         double convert=atof(a_prop.GetValue());
         wxPropertyValue aval(convert);
	 wxString tmp = a_prop.GetName();
         theprop->SetName( tmp );
         theprop->SetValue(aval);
         tmp = a_prop.GetType();
         theprop->SetRole( tmp );
      }
      else if (a_prop.GetType().IsSameAs("integer"))
      {
         long convert=atoi(a_prop.GetValue());
         wxPropertyValue aval(convert);
	     wxString tmp = a_prop.GetName();
         theprop->SetName( tmp );
         theprop->SetValue(aval);
         tmp = a_prop.GetType();
         theprop->SetRole( tmp );
      }
      else //if (!strcmp(a_prop.GetType(),"string"))
      {
         wxPropertyValue aval( a_prop.GetValue());
	     wxString tmp = a_prop.GetName();
         theprop->SetName( tmp );
         theprop->SetValue(aval);
         tmp = a_prop.GetType();
         theprop->SetRole( tmp );
      }
   	m_sheet->AddProperty(theprop);
		view->UpdatePropertyList(TRUE);

   }

}

void PropEditDialog::Remove(wxCommandEvent &)
{
   wxProperty* item=view->GetSelected();

   if (item)
   {
	   m_sheet->RemoveProperty(item->GetName());
      view->UpdatePropertyList(TRUE);
   }

}

void PropEditDialog::Cancel(wxCommandEvent&)
{
   MakeModal(FALSE);

	Show(FALSE);

}




/*

  m_sheet = new wxPropertySheet;



   _LI.Attach(a_proplist);

   _LI.tohead();
   while(!_LI.hitroot())
   {
	   m_sheet->AddProperty(new wxProperty((wxString)_LI.item()->GetName(),(wxString) _LI.item()->GetValue(), "string"));
//      _LI.item()->GetName(),_LI.item()->GetType(),_LI.item()->GetValue());
      _LI++;
   }
*/
/*
  sheet->AddProperty(new wxProperty("fred", 1.0, "real"));

  sheet->AddProperty(new wxProperty("tough choice", (bool)TRUE, "bool"));

  sheet->AddProperty(new wxProperty("ian", (long)45, "integer", new wxIntegerListValidator(-50, 50)));

  sheet->AddProperty(new wxProperty("bill", 25.0, "real", new wxRealListValidator(0.0, 100.0)));

  sheet->AddProperty(new wxProperty("julian", "just a string", "string"));

  sheet->AddProperty(new wxProperty("bitmap", "none", "string", new wxFilenameListValidator("Select a bitmap file", "*.bmp")));

  wxStringList *strings = new wxStringList("one", "two", "three","vier", NULL);

  sheet->AddProperty(new wxProperty("constrained", "vier", "string", new wxStringListValidator(strings)));



  wxStringList *strings2 = new wxStringList("earth", "fire", "wind", "water", NULL);

  sheet->AddProperty(new wxProperty("string list", strings2, "stringlist"));

*/

BEGIN_EVENT_TABLE(PropAddDialog, wxDialog)
  EVT_BUTTON    (PROP_BUTTON_OK,       PropAddDialog::Ok)
  EVT_BUTTON    (PROP_BUTTON_CANCEL,   PropAddDialog::Cancel)
  EVT_CLOSE     (PropEditDialog::OnCloseWindow)
END_EVENT_TABLE()

#define PROPADD_HSIZE		300
#define PROPADD_VSIZE		130

PropAddDialog::PropAddDialog(wxWindow* parent,char* title,long style ,char *name):
		wxDialog(parent,-1,title,wxDefaultPosition,wxSize(PROPADD_HSIZE,PROPADD_VSIZE),style,name)
{
	_parent = parent;

    wxString choices[] =
    {
        "bool",
        "integer",
        "real",
        "string"
    };

	_panel 	  		= new wxPanel	(this,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER,"proppanel");

	_okButton 		= new wxButton	(_panel,PROP_BUTTON_OK,"Ok");
	_cancelButton 	= new wxButton	(_panel,PROP_BUTTON_CANCEL,"Cancel");
	_tname			= new wxStaticText(_panel,-1,"Name      ");
	_ttype			= new wxStaticText(_panel,-1,"Type      ");
	_tvalue			= new wxStaticText(_panel,-1,"Value     ");
	_name				= new wxTextCtrl(_panel,-1,"name");
   _type				= new wxChoice( _panel, -1,wxDefaultPosition,wxDefaultSize, 4, choices );
   _type->SetSelection(3);

	_value			= new wxTextCtrl(_panel,-1,"value");



   this->SetAutoLayout(TRUE);

   // Set constraints for panel subwindow
   wxLayoutConstraints *c2 = new wxLayoutConstraints;
   c2->left.SameAs       (this, wxLeft,1);
   c2->top.SameAs        (this, wxTop,1);
   c2->right.SameAs      (this, wxRight, 1);
   c2->bottom.SameAs     (this, wxBottom, 1);
   _panel->SetConstraints(c2);


   //tname
   wxLayoutConstraints *c13 = new wxLayoutConstraints;
   c13->left.SameAs       (_panel, wxLeft,5);
   c13->top.SameAs     	 (_panel, wxTop,5);
   c13->height.AsIs();
   c13->width.AsIs();
   _tname->SetConstraints(c13);


   //name
   wxLayoutConstraints *c3 = new wxLayoutConstraints;
   c3->left.RightOf      (_tname,5);
   c3->top.SameAs     	 (_tname, wxTop);
   c3->height.AsIs();
   c3->width.AsIs();
   _name->SetConstraints(c3);


   //ttype
   wxLayoutConstraints *c14 = new wxLayoutConstraints;
   c14->left.SameAs      (_panel,wxLeft, 5);
   c14->top.SameAs     	 (_tname, wxBottom,10);
   c14->height.AsIs();
   c14->width.AsIs();
   _ttype->SetConstraints(c14);


   //type
   wxLayoutConstraints *c4 = new wxLayoutConstraints;
   c4->left.RightOf      (_ttype,5);
   c4->top.SameAs     	 (_ttype, wxTop);
   c4->height.AsIs();
   c4->width.AsIs();
   _type->SetConstraints(c4);


   //tvalue
   wxLayoutConstraints *c15 = new wxLayoutConstraints;
   c15->left.SameAs      (_panel,wxLeft, 5);
   c15->top.SameAs     	 (_ttype, wxBottom,10);
   c15->height.AsIs();
   c15->width.AsIs();
   _tvalue->SetConstraints(c15);


   //value
   wxLayoutConstraints *c5 = new wxLayoutConstraints;
   c5->left.RightOf      (_tvalue,5);
   c5->top.SameAs     	 (_tvalue, wxTop);
   c5->height.AsIs();
   c5->width.AsIs();
   _value->SetConstraints(c5);


   //okbutton
   wxLayoutConstraints *c6 = new wxLayoutConstraints;
   c6->left.SameAs       (_panel, wxLeft,5);
   c6->top .SameAs       (_tvalue, wxBottom,10);
   c6->height.AsIs();
   c6->width.AsIs();
   _okButton->SetConstraints(c6);



   //cancelbutton
   wxLayoutConstraints *c7 = new wxLayoutConstraints;
   c7->left.RightOf      (_okButton,10);
   c7->top .SameAs       (_tvalue, wxBottom,10);
   c7->height.AsIs();
   c7->width.AsIs();
   _cancelButton->SetConstraints(c7);

   Centre(wxBOTH);
}



PropAddDialog::~PropAddDialog()
{
}





void PropAddDialog::OnCloseWindow(wxCloseEvent& event)
{
   EndModal(0);
}

void PropAddDialog::Cancel(wxCommandEvent &)
{
   EndModal(0);
}



void PropAddDialog::Ok(wxCommandEvent &)
{
   EndModal(1);
}



wxString PropAddDialog::GetName()
{
    return _name->GetValue();
}



wxString PropAddDialog::GetType()
{
    return _type->GetStringSelection();
}



wxString PropAddDialog::GetValue()
{
    return _value->GetValue();
}

MywxPropertyListView::MywxPropertyListView(wxPanel *propPanel, long flags):wxPropertyListView(propPanel,flags)
{
}

wxProperty* MywxPropertyListView::GetSelected()
{
  int sel=m_propertyScrollingList->GetSelection();
  if (sel > -1)
  {
    wxProperty *newSel = (wxProperty *)m_propertyScrollingList->wxListBox::GetClientData(sel);
    return newSel;
  }
  return 0;
}


