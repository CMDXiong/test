#ifdef __GNUG__
#pragma implementation
#endif

#include <fstream>

#include <assert.h>
#include <stdlib.h>
#include "mainwin.h"
#include "gdsapp.h"
#include "wxwin.h"

#include "debugdlg.h"
#include "eventids.h"
#include "chandbas.h"
#include "linepars.h"


const int    DEBUGDIALOG_BUTTON_HIDE      = wxID_HIGHEST + 5021 ;
const int    DEBUGDIALOG_BUTTON_STOP      = wxID_HIGHEST + 5022 ;
const int    DEBUGDIALOG_BUTTON_EXEC      = wxID_HIGHEST + 5023 ;
const int    DEBUGDIALOG_BUTTON_CONTINUE  = wxID_HIGHEST + 5024 ;
const int    DEBUGDIALOG_BUTTON_NEXTPROC  = wxID_HIGHEST + 5025 ;
const int    DEBUGDIALOG_BUTTON_COPY      = wxID_HIGHEST + 5026 ;
const int    DEBUGDIALOG_EDIT_CHANGED 		= wxID_HIGHEST + 5027 ;

BEGIN_EVENT_TABLE(DebugDialog, wxDialog)
  EVT_BUTTON    (DEBUGDIALOG_BUTTON_HIDE, DebugDialog::Hide)
  EVT_BUTTON    (DEBUGDIALOG_BUTTON_STOP, DebugDialog::Stop)
  EVT_BUTTON    (DEBUGDIALOG_BUTTON_EXEC, DebugDialog::Execute)
  EVT_BUTTON    (DEBUGDIALOG_BUTTON_CONTINUE, DebugDialog::Continue)
  EVT_BUTTON    (DEBUGDIALOG_BUTTON_NEXTPROC, DebugDialog::NextProc)
  EVT_BUTTON    (DEBUGDIALOG_BUTTON_COPY, DebugDialog::Copy)

  EVT_TEXT_ENTER(DEBUGDIALOG_EDIT_CHANGED,DebugDialog::Execute)

  EVT_IDLE( DebugDialog::OnIdle)

  EVT_CLOSE     (DebugDialog::OnCloseWindow)

END_EVENT_TABLE()

#define DEBUGDIALOG_HSIZE		600
#define DEBUGDIALOG_VSIZE		220

DebugDialog::DebugDialog(GDSMainWindow* parent,char *title,long style,char *name):
	wxDialog(parent,-1,title,wxDefaultPosition,wxSize(DEBUGDIALOG_HSIZE,DEBUGDIALOG_VSIZE),style,name)
{
	m_parent = parent;

	m_panel 		= new wxPanel	(this,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER ,"panel");
  	m_history  = new wxStaticText(m_panel,-1,"history");
  	m_comline  = new wxStaticText(m_panel,-1,"command line");
	m_lb			= new wxListBox(m_panel,-1,wxDefaultPosition,wxDefaultSize,0,NULL,wxLB_SINGLE);
	m_editField	= new wxTextCtrl(m_panel,DEBUGDIALOG_EDIT_CHANGED,"command",wxDefaultPosition,wxSize(400,50),wxTE_MULTILINE);

	m_hideButton 	 = new wxButton	(m_panel,DEBUGDIALOG_BUTTON_HIDE,"Hide");
	m_stopButton 	 = new wxButton	(m_panel,DEBUGDIALOG_BUTTON_STOP,"Stop");
	m_execButton 	 = new wxButton	(m_panel,DEBUGDIALOG_BUTTON_EXEC,"Execute");
	m_continueButton = new wxButton	(m_panel,DEBUGDIALOG_BUTTON_CONTINUE,"Continue");
	m_nextprocButton = new wxButton	(m_panel,DEBUGDIALOG_BUTTON_NEXTPROC,"Continue EOF");
	m_copyButton     = new wxButton	(m_panel,DEBUGDIALOG_BUTTON_COPY,"Copy from history");

   this->SetAutoLayout(TRUE);

   // Set constraints for panel subwindow
   wxLayoutConstraints *c1 = new wxLayoutConstraints;
   c1->left.SameAs       (this, wxLeft,1);
   c1->top.SameAs        (this, wxTop,1);
   c1->right.SameAs      (this, wxRight, 1);
   c1->bottom.SameAs     (this, wxBottom, 1);
   m_panel->SetConstraints(c1);


   //history text
   wxLayoutConstraints *c2 = new wxLayoutConstraints;
   c2->left.SameAs       (m_panel, wxLeft);
   c2->top.SameAs        (m_panel, wxTop);
   c2->height.AsIs();
   c2->width.AsIs();
   m_history->SetConstraints(c2);


   //listbox
   wxLayoutConstraints *c3 = new wxLayoutConstraints;
   c3->left.SameAs       (m_panel, wxLeft);
   c3->top.SameAs        (m_history, wxBottom);
   c3->right.SameAs      (m_panel, wxRight,2);
   c3->bottom.SameAs     (m_comline,wxTop,2);
   m_lb->SetConstraints(c3);



   //comline text
   wxLayoutConstraints *c4 = new wxLayoutConstraints;
   c4->left.SameAs       (m_panel, wxLeft);
   c4->bottom.SameAs     (m_editField, wxTop);
   c4->height.AsIs();
   c4->width.AsIs();
   m_comline->SetConstraints(c4);


   //editField
   wxLayoutConstraints *c7 = new wxLayoutConstraints;
   c7->left.SameAs       (m_panel, wxLeft);
   c7->bottom.SameAs     (m_hideButton, wxTop);
   c7->right.SameAs      (m_panel, wxRight,2);
   c7->height.AsIs();
   m_editField->SetConstraints(c7);


   //hidebutton
   wxLayoutConstraints *c8 = new wxLayoutConstraints;
   c8->left.SameAs       (m_panel, wxLeft,5);
   c8->bottom.SameAs     (m_panel, wxBottom,5);
   c8->height.AsIs();
   c8->width.AsIs();
   m_hideButton->SetConstraints(c8);


   //stopbutton
   wxLayoutConstraints *c5 = new wxLayoutConstraints;
   c5->left.RightOf      (m_hideButton,10);
   c5->bottom.SameAs     (m_panel, wxBottom,5);
   c5->height.AsIs();
   c5->width.AsIs();
   m_stopButton->SetConstraints(c5);

   //execbutton
   wxLayoutConstraints *c12 = new wxLayoutConstraints;
   c12->left.RightOf      (m_stopButton,10);
   c12->bottom.SameAs     (m_panel, wxBottom,5);
   c12->height.AsIs();
   c12->width.AsIs();
   m_execButton->SetConstraints(c12);

   //continuebutton
   wxLayoutConstraints *c15 = new wxLayoutConstraints;
   c15->left.RightOf      (m_execButton,10);
   c15->bottom.SameAs     (m_panel, wxBottom,5);
   c15->height.AsIs();
   c15->width.AsIs();
   m_continueButton->SetConstraints(c15);


   //nextprocbutton
   wxLayoutConstraints *c9 = new wxLayoutConstraints;
   c9->left.RightOf      (m_continueButton,10);
   c9->bottom.SameAs     (m_panel, wxBottom,5);
   c9->height.AsIs();
   c9->width.AsIs();
   m_nextprocButton->SetConstraints(c9);

   //copybutton
   wxLayoutConstraints *c11 = new wxLayoutConstraints;
   c11->left.RightOf      (m_nextprocButton,10);
   c11->bottom.SameAs     (m_panel, wxBottom,5);
   c11->height.AsIs();
   c11->width.AsIs();
   m_copyButton->SetConstraints(c11);

	m_lineparser=0;
   m_is_executed=true;
	m_endoffile=true;
	m_stop_at_next_file=false;
   _process_file_stack = new DL_List<void*>();


   Layout();

}

DebugDialog::~DebugDialog()
{
   {
		TDLI<Line_Parser> I(_process_file_stack);
		I.delete_all();
   }
   delete _process_file_stack;
   if (m_lineparser)
   	delete m_lineparser;
}

void DebugDialog::Init(char* processfile)
{
   {
		TDLI<Line_Parser> I(_process_file_stack);
		I.delete_all();
   }
   if (m_lineparser)
   	delete m_lineparser;
   m_lineparser = new Line_Parser(GLOB->ALIASLIST,processfile);
 	GLOB->MAINWINDOW->CmStepMode(true);
   Show(true);
   m_is_executed=true;
	m_endoffile=false;
  	m_stop_at_next_file=false;

   GLOB->CMDH->SetContinue(false);
}

void DebugDialog::SetCommand(char* BaseCommand)
{
   m_editField->SetValue(BaseCommand);
}

void DebugDialog::Hide(wxCommandEvent&)
{
   {
		TDLI<Line_Parser> I(_process_file_stack);
		I.delete_all();
   }
   delete m_lineparser;
   m_lineparser=0;
   m_is_executed=true;
	m_endoffile=false;
   Show(false);
}

void DebugDialog::Stop(wxCommandEvent&)
{
   GLOB->CMDH->SetContinue(false);
}

void DebugDialog::Execute(wxCommandEvent&)
{
   if ( (m_editField->GetValue().c_str())[0] != '#' &&
   		strstr(m_editField->GetValue().c_str(),"loadprocess") != 0)
   {//recursive processfile
      DL_Iter<const wxString*>* 	_I; //allows recursive calls
      wxString commandstring = m_editField->GetValue() + '\0';
      wxString dummy;
      Evalstring* _commandstring = new Evalstring( commandstring, GLOB->ALIASLIST );
      _I=_commandstring->GetArgIter();
      _commandstring->Parse(dummy);
      _I->tohead();
      (*_I)++;

      TDLIStack<Line_Parser> I(_process_file_stack);
      I.push(m_lineparser);
      m_lineparser = new Line_Parser(GLOB->ALIASLIST,(char*)_I->item()->c_str());
      m_endoffile=false;
      delete _commandstring;
	   m_is_executed=true; //consider this one now executed
      m_lb->Append(m_editField->GetValue().c_str());
      m_lb->SetSelection(m_lb->GetCount()-1,true);
      if (strcmp(m_editField->GetValue().c_str(),m_commandstring))
         m_editField->SetValue(m_commandstring);
      else
         m_is_executed=true;
      return;
   }

   if (strlen(m_editField->GetValue().c_str()) > 0) {
      try{
			m_parent->set_Cursor("wait");
         if (!GLOB->CMDH->DoCommand("%s",m_editField->GetValue().c_str()))
         {
            char tmp[80];
            sprintf(tmp,"%s \n Could not execute command",m_editField->GetValue().c_str());
            throw GDS_Error(tmp, "Command Error",9,1);
         }
			m_parent->set_Cursor("ready");
      }
      catch (GDS_Error& _error) {
         _messagehandler->info(_error.GetErrorMessage(), _error.GetHeaderMessage());
      }
      catch(...) {
         _messagehandler->info("SOMETHING WENT WRONG leaf program","NON HANDLED ERROR");
      }
   }
	m_lb->Append(m_editField->GetValue().c_str());
	m_lb->SetSelection(m_lb->GetCount()-1,true);

   if (strcmp(m_editField->GetValue().c_str(),m_commandstring))
      m_editField->SetValue(m_commandstring);
   else
   	m_is_executed=true;

}

void DebugDialog::Continue(wxCommandEvent&)
{
   GLOB->CMDH->SetContinue(true);
}

void DebugDialog::NextProc(wxCommandEvent&)
{
   	m_stop_at_next_file=true;
	   GLOB->CMDH->SetContinue(true);
}

void DebugDialog::Copy(wxCommandEvent&)
{
	int i = m_lb->GetSelection();
   if (i != -1) {
      m_editField->SetValue(m_lb->GetString(i));
   }
}

void DebugDialog::OnCloseWindow(wxCloseEvent& event)
{
   Show(false);
}

void DebugDialog::OnIdle(wxIdleEvent& event)
{

if (IsShown())
{

   if (m_lineparser && !m_endoffile && m_is_executed)
   {
      try
      {
         if(m_lineparser->GetNextCommand())
         {
            if (m_lineparser->GetArgList()->count())
            {
               if (m_lineparser->GetArgList()->headitem()->CmpNoCase("record") != 0)
               {  //we are reading from a process file so ignore record commands
                  strcpy(m_commandstring,m_lineparser->GetCommandParsed());
                  m_editField->SetValue(m_commandstring);
					   m_is_executed=false;
               }
            }

            m_lineparser->FlushArg();

         }
         else
         {
            strcpy(m_commandstring,"");
            m_editField->SetValue(m_commandstring);
            m_endoffile=true;
         }
      }
      catch (GDS_Error& _error) // error from the parser: process here
      {
         _messagehandler->info(_error.GetErrorMessage(),
                                "ProcessParser::LoadProcess(): Fatal Error");
         return ;
      }
   }

   //directly execute in case of continue mode
   if (!m_is_executed && !m_endoffile && GLOB->CMDH->GetContinue())
   {
      wxCommandEvent a = wxCommandEvent(); 
      Execute(a);
   } 
   if (m_endoffile && !_process_file_stack->empty())
   {
      TDLIStack<Line_Parser> I(_process_file_stack);
      m_lineparser=I.pop();
		m_is_executed=true; //in order to fetch a new command from the new current file
      if (m_stop_at_next_file)
		   GLOB->CMDH->SetContinue(false);
   	m_stop_at_next_file=false;
      m_endoffile=false;
   }
}
	return;
}

const int    STOPDIALOG_BUTTON_STOP      = wxID_HIGHEST + 5030;


BEGIN_EVENT_TABLE(StopDialog, wxDialog)
  EVT_BUTTON    (STOPDIALOG_BUTTON_STOP, StopDialog::Stop)
  EVT_CLOSE     (StopDialog::OnCloseWindow)
END_EVENT_TABLE()

#define STOPDIALOG_HSIZE		60
#define STOPDIALOG_VSIZE		60

StopDialog::StopDialog(GDSMainWindow* parent,char *title,long style,char *name):
	wxDialog(parent,-1,title,wxPoint(0,0),wxSize(STOPDIALOG_HSIZE,STOPDIALOG_VSIZE),style,name)
{
	m_parent = parent;

	m_panel 		= new wxPanel	(this,-1,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER ,"panel");
	m_stopButton 	 = new wxButton	(m_panel,STOPDIALOG_BUTTON_STOP,"Stop",wxDefaultPosition,wxDefaultSize);


   this->SetAutoLayout(TRUE);


   // Set constraints for panel subwindow
   wxLayoutConstraints *c1 = new wxLayoutConstraints;
   c1->left.SameAs       (this, wxLeft,1);
   c1->top.SameAs        (this, wxTop,1);
   c1->right.SameAs      (this, wxRight, 1);
   c1->bottom.SameAs     (this, wxBottom, 1);
   m_panel->SetConstraints(c1);


   //stopbutton
   wxLayoutConstraints *c5 = new wxLayoutConstraints;
   c5->left.SameAs       (m_panel, wxLeft,1);
   c5->top.SameAs        (m_panel, wxTop,1);
   c5->right.SameAs      (m_panel, wxRight, 1);
   c5->bottom.SameAs     (m_panel, wxBottom, 1);
//   c5->height.AsIs();
//   c5->width.AsIs();
   m_stopButton->SetConstraints(c5);
}

StopDialog::~StopDialog()
{
}

void StopDialog::Stop(wxCommandEvent&)
{
   GLOB->CMDH->SetContinue(false);
}

void StopDialog::OnCloseWindow(wxCloseEvent& event)
{
   Show(false);
}


