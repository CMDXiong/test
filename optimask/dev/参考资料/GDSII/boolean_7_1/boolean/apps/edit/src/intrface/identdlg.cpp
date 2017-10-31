#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include <stdlib.h>
#include "mainwin.h"
#include "wxwin.h"
#include "lsetup.h"
#include "identdlg.h"
#include "eventids.h"
#include "chandbas.h"
#include "gdsii.h"
#include "shape.h"
#include "referenc.h"
#include "elemlst.h"
#include "proper.h"
#include "propedit.h"
#include "structur.h"

#define IDENTDLG_HSIZE		400
#define IDENTDLG_VSIZE		200

const int    IDENT_BUTTON_HIDE   = wxID_HIGHEST + 5302 ;
const int    IDENT_BUTTON_UPDATE = wxID_HIGHEST + 5303 ;
const int    IDENT_BUTTON_EDIT 	= wxID_HIGHEST + 5304 ;


BEGIN_EVENT_TABLE(IdentDlg, wxDialog)
  EVT_BUTTON    (IDENT_BUTTON_HIDE,  IdentDlg::hide)

  EVT_BUTTON    (IDENT_BUTTON_UPDATE,IdentDlg::Update)

  EVT_BUTTON    (IDENT_BUTTON_EDIT,IdentDlg::Edit)

  EVT_CLOSE     (IdentDlg::OnCloseWindow)

END_EVENT_TABLE()


IdentDlg::IdentDlg(GDSMainWindow* parent,char *title,long style,char *name):
	wxDialog(parent,-1,title,wxDefaultPosition,wxSize(IDENTDLG_HSIZE,IDENTDLG_VSIZE),style,name)
{
	_parent = parent;

	_panel 			= new wxPanel	(this,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER,"indentpanel");

	_lb				= new wxListBox(_panel,-1,wxDefaultPosition,wxDefaultSize,0,NULL,wxLB_SINGLE);
	_updateButton 	= new wxButton	(_panel,IDENT_BUTTON_UPDATE,"Update");
	_hideButton 	= new wxButton	(_panel,IDENT_BUTTON_HIDE,"Hide Dialog");
	_editButton 	= new wxButton	(_panel,IDENT_BUTTON_EDIT,"Edit");
   this->SetAutoLayout(TRUE);

   // Set constraints for panel subwindow
   wxLayoutConstraints *c1 = new wxLayoutConstraints;

   c1->left.SameAs       (this, wxLeft,1);

   c1->top.SameAs        (this, wxTop,1);

   c1->right.SameAs      (this, wxRight, 1);

   c1->bottom.SameAs     (this, wxBottom, 1);

   _panel->SetConstraints(c1);



   //listbox

   wxLayoutConstraints *c3 = new wxLayoutConstraints;

   c3->left.SameAs       (_panel, wxLeft);

   c3->top.SameAs        (_panel, wxTop);

   c3->right.SameAs      (_panel, wxRight,2);

   c3->bottom.SameAs     (_hideButton,wxTop,2);

   _lb->SetConstraints(c3);



   //hidebutton

   wxLayoutConstraints *c8 = new wxLayoutConstraints;

   c8->left.SameAs       (_panel, wxLeft,5);

   c8->bottom.SameAs     (_panel, wxBottom,5);

   c8->height.AsIs();

   c8->width.AsIs();

   _hideButton->SetConstraints(c8);



   //updatebutton

   wxLayoutConstraints *c5 = new wxLayoutConstraints;

   c5->left.RightOf      (_hideButton,10);

   c5->bottom.SameAs     (_panel, wxBottom,5);

   c5->height.AsIs();

   c5->width.AsIs();

   _updateButton->SetConstraints(c5);



   //editbutton

   wxLayoutConstraints *c6 = new wxLayoutConstraints;

   c6->left.RightOf      (_updateButton,10);

   c6->bottom.SameAs     (_panel, wxBottom,5);

   c6->height.AsIs();

   c6->width.AsIs();

   _editButton->SetConstraints(c6);

   Layout();


}

IdentDlg::~IdentDlg()
{
}

void IdentDlg::hide(wxCommandEvent &)
{
	_parent->ShowIdentDlg();
}

void IdentDlg::OnCloseWindow(wxCloseEvent& event)
{
	_parent->ShowIdentDlg();
}

void IdentDlg::Update(wxCommandEvent &)
{
	Shape*		s;
	Reference*	r;
   char			lyr[1000],msg[1000];

	_lb->Clear();

	GdsII*			tmp 	= GLOB->CMDH->Get_GDSII();
   if (! tmp) return;

	{
		TDLI<Structure> I=TDLI<Structure>(tmp);
     	I.toroot();
		while (I.iterate()) {
		  	TDLI<Element> I2=TDLI<Element>(I.item());
         I2.toroot();
         while (I2.iterate()) {
	      	if (I2.item()->GetSelect()) {
   	      	strcpy(msg,"");
					if ((I2.item()->GetType() != SREF) && (I2.item()->GetType() != AREF)) {
         	   	switch(I2.item()->GetType()) {
							case CIRCLE		: strcat(msg,"Circle on layer "); break;
							case ARCSEG		: strcat(msg,"ArcSegment on layer "); break;
							case BOX			: strcat(msg,"Box on layer "); break;
							case BOUNDARY	: strcat(msg,"Boundary on layer "); break;
							case PATH		: strcat(msg,"Path on layer "); break;
							case TEXT		: strcat(msg,"Text on layer "); break;
							default: break;
         	      }
            	   s = (Shape*)I2.item();
						sprintf(lyr,"%i, %s",s->GetLayer(),GLOB->SETUP->Get_Name(s->GetLayer()));
	               strcat(msg,lyr);
   	         } else {
               	r = (Reference*)I2.item();
						sprintf(msg,"Structureref %s",r->GetStructure()->GetStructureName().c_str());
         	   }
               char hulp[200];
               if (I2.item()->HasPropList())
               {
                   wxList& props=I2.item()->GetPropList()->GetProperties();

                   for (wxNode *node = props.GetFirst(); node; node = node->GetNext() )
                   {
                     wxProperty*  current = (wxProperty*)node->GetData();

                     char type[80];
                     wxPropertyValueType A_VALUE= current->GetValue().Type();
                     switch (A_VALUE)
                     {
                        case wxPropertyValueInteger:
                              strcpy(type,"integer");
                              break;
                        case wxPropertyValueReal:
                              strcpy(type,"real");
                              break;
                        case wxPropertyValuebool:
                              strcpy(type,"bool");
                              break;
                        case wxPropertyValueString:
                              strcpy(type,"string");
                              break;
                        default:
                              strcpy(type,"member");
                        		break;
                     }

                     sprintf(hulp,"  Property %s: type %s: value %s",current->GetName().c_str(),type,current->GetValue().GetStringRepresentation().c_str());
		               strcat(msg,hulp);
                   }
               }
					_lb->Append(msg);
            }
         }
      }
   }
}

void IdentDlg::Edit(wxCommandEvent &)
{
	GdsII*			tmp 	= GLOB->CMDH->Get_GDSII();
   if (! tmp) return;

	int selection = _lb->GetSelection();
	if (selection == -1)
		_messagehandler->info("You must select an element first.","No element Selected");

	{
      int i=0;
      bool found=false;

		TDLI<Structure> I=TDLI<Structure>(tmp);
     	I.toroot();

		while (I.iterate()&& !found) {
		  	TDLI<Element> I2=TDLI<Element>(I.item());
         I2.toroot();
         while (I2.iterate()&& !found) {
	      	if (I2.item()->GetSelect())
            {
               if (selection==i)
               {
                 	long style = (wxRESIZE_BORDER |wxTHICK_FRAME | wxSYSTEM_MENU | wxCAPTION);

                  wxString title;
                  switch (I2.item()->GetType())
                  {
                     case BOUNDARY:
                           title="Boundary";
                           break;
                     case PATH:
                           title="Path";
                           break;
                     case ARCSEG:
                           title="Arcsegment";
                           break;
                     case TEXT:
                           title="Text";
                           break;
                     case BOX:
                           title="Rectangle";
                           break;
                     case CIRCLE:
                           title="Circle";
                           break;
                     case AREF:
                           title="Structure Array Reference";
                           break;
                     case SREF:
                           title="Structure Reference";
                           break;

                  }
                  PropEditDialog* a_prop= new	PropEditDialog(this,title,style,"propedit",I2.item()->GetPropList());
						I2.item()->UpdatePropList();
                  delete a_prop;
                  found=true;
               }
            	i++;
            }
         }
      }
   }
}

