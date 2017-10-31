#ifdef __GNUG__
#pragma implementation
#endif

#include "mainwin.h"
#include "gdsapp.h"

#include "wait.h"

const int    WAIT_BUTTON_OK     = wxID_HIGHEST + 5801 ;
const int    WAIT_BUTTON_CANCEL = wxID_HIGHEST + 5802 ;

BEGIN_EVENT_TABLE(Wait, wxDialog)
  EVT_BUTTON    (WAIT_BUTTON_OK,      Wait::CmOk)

END_EVENT_TABLE()



Wait::Wait(GDSMainWindow* parent,char *title,int x,int y,int,int,long style,char *name):
	wxDialog(parent,-1,title,wxPoint(x,y),wxSize(WAIT_HSIZE,WAIT_VSIZE),style,name)
{
	_message = new char[ sizeof("system is busy") ];
	strcpy( _message, "system is busy" );

	_panel = new wxPanel(this,-1,wxPoint(0,0),wxSize(WAIT_HSIZE,WAIT_VSIZE));
	_okButton = new wxButton(_panel,WAIT_BUTTON_OK,"Ok",wxPoint(WAIT_BUTTON1_X,WAIT_BUTTON1_Y),wxSize(WAIT_BUTTON_W,WAIT_BUTTON_H));
	_text1 = new wxStaticText(_panel,-1,_message,wxPoint(WAIT_MESSAGE1_X,WAIT_MESSAGE1_Y),wxSize(200,10));
	Centre(wxBOTH);

   Layout();

}

Wait::~Wait()
{
	delete _message;
}

void Wait::CmOk(wxCommandEvent &)
{
/*TODO
void WaitButtonOK(wxButton&, wxCommandEvent&)
{
	gdsapp->GetMainWindow()->_waitDialog->Show(FALSE);
}
*/
}

void Wait::SetMessage(char *newmess)
{
	if (_message) delete _message;
	_message = strdup(newmess);
	_text1->SetLabel(_message);
}
