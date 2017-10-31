#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include <stdlib.h>
#include "mainwin.h"
#include "gdsapp.h"
#include "wxwin.h"

#include "execdlg.h"
#include "eventids.h"
#include "chandbas.h"
#include "copylist.h"

#define EXECDLG_HSIZE		400
#define EXECDLG_VSIZE		200

const int    EXEC_BUTTON_EXEC       = wxID_HIGHEST + 5001 ;
const int    EXEC_BUTTON_COPY       = wxID_HIGHEST + 5002 ;
const int    EXEC_BUTTON_HIDE       = wxID_HIGHEST + 5003 ;
const int    EXEC_EDIT_CHANGED 		= wxID_HIGHEST + 5004 ;

BEGIN_EVENT_TABLE(ExecDlg, wxDialog)
  EVT_BUTTON    (EXEC_BUTTON_EXEC, ExecDlg::Execute)

  EVT_BUTTON    (EXEC_BUTTON_COPY, ExecDlg::Copy)

  EVT_BUTTON    (EXEC_BUTTON_HIDE, ExecDlg::hide)

  EVT_TEXT_ENTER(EXEC_EDIT_CHANGED,ExecDlg::Execute)

  EVT_CLOSE     (ExecDlg::OnCloseWindow)
  EVT_CHAR			(ExecDlg::OnChar)
  EVT_CHAR_HOOK	(ExecDlg::OnCharHook)

END_EVENT_TABLE()


ExecDlg::ExecDlg(GDSMainWindow* parent,char *title,long style,char *name):
	wxDialog(parent,-1,title,wxDefaultPosition,wxSize(EXECDLG_HSIZE,EXECDLG_VSIZE),style,name)
{
	_parent = parent;

	_panel 		= new wxPanel	(this,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER ,"panel");
  	_m_history  = new wxStaticText(_panel,-1,"history");
  	_m_comline  = new wxStaticText(_panel,-1,"command line");
	_lb			= new CopywxListBox(_panel,-1,wxDefaultPosition,wxDefaultSize,0,NULL,wxLB_SINGLE);

	_execButton = new wxButton	(_panel,EXEC_BUTTON_EXEC,"Execute");
	_copyButton = new wxButton	(_panel,EXEC_BUTTON_COPY,"Copy");
	_hideButton = new wxButton	(_panel,EXEC_BUTTON_HIDE,"Hide Dialog");
	_editField	= new wxTextCtrl(_panel,EXEC_EDIT_CHANGED,"command",wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);

   this->SetAutoLayout(TRUE);

   // Set constraints for panel subwindow
   wxLayoutConstraints *c1 = new wxLayoutConstraints;

   c1->left.SameAs       (this, wxLeft,1);

   c1->top.SameAs        (this, wxTop,1);

   c1->right.SameAs      (this, wxRight, 1);

   c1->bottom.SameAs     (this, wxBottom, 1);

   _panel->SetConstraints(c1);



   //history text

   wxLayoutConstraints *c2 = new wxLayoutConstraints;

   c2->left.SameAs       (_panel, wxLeft);

   c2->top.SameAs        (_panel, wxTop);

   c2->height.AsIs();

   c2->width.AsIs();

   _m_history->SetConstraints(c2);



   //listbox

   wxLayoutConstraints *c3 = new wxLayoutConstraints;

   c3->left.SameAs       (_panel, wxLeft);

   c3->top.SameAs        (_m_history, wxBottom);

   c3->right.SameAs      (_panel, wxRight,2);

   c3->bottom.SameAs     (_m_comline,wxTop,2);

   _lb->SetConstraints(c3);



   //comline text

   wxLayoutConstraints *c4 = new wxLayoutConstraints;

   c4->left.SameAs       (_panel, wxLeft);

   c4->bottom.SameAs     (_editField, wxTop);

   c4->height.AsIs();

   c4->width.AsIs();

   _m_comline->SetConstraints(c4);



   //editField

   wxLayoutConstraints *c7 = new wxLayoutConstraints;

   c7->left.SameAs       (_panel, wxLeft);

   c7->bottom.SameAs     (_hideButton, wxTop);

   c7->right.SameAs      (_panel, wxRight,2);

   c7->height.AsIs();

   _editField->SetConstraints(c7);



   //hidebutton

   wxLayoutConstraints *c8 = new wxLayoutConstraints;

   c8->left.SameAs       (_panel, wxLeft,5);

   c8->bottom.SameAs     (_panel, wxBottom,5);

   c8->height.AsIs();

   c8->width.AsIs();

   _hideButton->SetConstraints(c8);



   //copybutton

   wxLayoutConstraints *c5 = new wxLayoutConstraints;

   c5->left.RightOf      (_hideButton,10);

   c5->bottom.SameAs     (_panel, wxBottom,5);

   c5->height.AsIs();

   c5->width.AsIs();

   _copyButton->SetConstraints(c5);



   //execbutton
   wxLayoutConstraints *c9 = new wxLayoutConstraints;

   c9->left.RightOf      (_copyButton,10);

   c9->bottom.SameAs     (_panel, wxBottom,5);

   c9->height.AsIs();

   c9->width.AsIs();

   _execButton->SetConstraints(c9);

   Layout();

   _editField->Clear();
}

ExecDlg::~ExecDlg()
{
}

void ExecDlg::addCommand(const wxString& BaseCommand)
{
	if (!GLOB->MAINWINDOW->_exit) //only do this if we are nor exiting the application, else problems in wxGTK
	{
		_lb->Append(BaseCommand);
	//	_lb->SetFirstItem(_lb->GetCount()-1);
		_lb->SetSelection(_lb->GetCount()-1,true);
	}	
}

void ExecDlg::SetCommand(char* BaseCommand)
{
   _editField->SetValue(BaseCommand);
}

void ExecDlg::hide(wxCommandEvent&)
{
	_parent->ShowExecDlg();
}

void ExecDlg::OnCloseWindow(wxCloseEvent& event)
{
	_parent->ShowExecDlg();
}


void ExecDlg::Copy(wxCommandEvent&)
{
	int i = _lb->GetSelection();
   if (i != -1) {
      _editField->SetValue(_lb->GetString(i));
   }
}

void ExecDlg::Execute(wxCommandEvent&)
{
	DoExecute();
}

void ExecDlg::DoExecute()
{
	char tmp[256];
   strcpy(tmp,_editField->GetValue());
   if (strlen(tmp) > 0) {
      try{
			_parent->set_Cursor("wait");
	   	GLOB->CMDH->DoCommand(tmp);
			_parent->set_Cursor("ready");
      }
      catch (GDS_Error& _error) {
         _messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
      }
      catch(...) {
         _messagehandler->info("SOMETHING WENT WRONG leaf program","NON HANDLED ERROR");
      }
   }
   _editField->Clear();

}

void ExecDlg::OnChar(wxKeyEvent& event)
{
    if ( event.GetKeyCode() == WXK_RETURN )
    {
      wxCommandEvent a = wxCommandEvent();
      Copy(a);
    }
    else
    {
       // key code is within legal range. we call event.Skip() so the
       // event can be processed either in the base wxWindows class
       // or the native control.

       event.Skip();
    }
}

void ExecDlg::OnCharHook(wxKeyEvent& event)
{
/*
    if ( event.GetKeyCode() == WXK_RETURN )
    {
      Execute();
    }
    else*/ if ( event.GetKeyCode() == WXK_ESCAPE )
		Hide();
    else
    {
       // key code is within legal range. we call event.Skip() so the
       // event can be processed either in the base wxWindows class
       // or the native control.

       event.Skip();
    }
}
