#ifdef __GNUG__
#pragma implementation
#endif
#include "groupdlg.h"
#include "misc.h"
#include "gdsapp.h"
#include "lsetup.h"
#include "misc.h"
#include "chandbas.h"

#include "group.h"
#include "wx/dynarray.h"

#ifdef __UNIX__
#define GROUP_HSIZE			625
#define GROUP_VSIZE			500

#define GROUP_GB1_X			10
#define GROUP_GB1_Y			0
#define GROUP_GB1_W			200
#define GROUP_GB1_H			485
#define GROUP_GB2_X			300
#define GROUP_GB2_Y			0
#define GROUP_GB2_W			200
#define GROUP_GB2_H			205
#define GROUP_GB3_X			300
#define GROUP_GB3_Y			210
#define GROUP_GB3_W			200
#define GROUP_GB3_H			205
#define GROUP_GB4_X			300
#define GROUP_GB4_Y			430
#define GROUP_GB4_W			200
#define GROUP_GB4_H			55

#define GROUP_LB1_X			14
#define GROUP_LB1_Y			20
#define GROUP_LB1_H			460
#define GROUP_LB1_W			190
#define GROUP_LB2_X			304
#define GROUP_LB2_Y			20
#define GROUP_LB2_H			180
#define GROUP_LB2_W			190
#define GROUP_LB3_X			304
#define GROUP_LB3_Y			230
#define GROUP_LB3_H			180
#define GROUP_LB3_W			190
#define GROUP_BUTTON_H		25
#define GROUP_BUTTON_W		70
#define GROUP_OK_X			510
#define GROUP_OK_Y			20
#define GROUP_CANCEL_X		510
#define GROUP_CANCEL_Y		50
#define GROUP_ALL_X			220
#define GROUP_ADDA_Y			20
#define GROUP_REMA_Y			50
#define GROUP_CLRA_Y			80
#define GROUP_ADDB_Y			230
#define GROUP_REMB_Y			260
#define GROUP_CLRB_Y			290
#define GROUP_ADDD_Y			430
#define GROUP_REMD_Y			460
#define GROUP_CB1_X			510
#define GROUP_CB1_Y			190
#define GROUP_CB2_X			510
#define GROUP_CB2_Y			400
#define GROUP_TEXT_X			303
#define GROUP_TEXT_Y			450
#define GROUP_TEXT_H			30
#define GROUP_TEXT_W			190

#define ADDTO_X			510
#define ADDTO_Y			450


#else
// windows defines
	//panel size
   #define GROUP_HSIZE			615
   #define GROUP_VSIZE			520

   #define GROUP_GB1_X			10
   #define GROUP_GB1_Y			0
   #define GROUP_GB1_W			200
   #define GROUP_GB1_H			485

   #define GROUP_GB2_X			300
   #define GROUP_GB2_Y			0
   #define GROUP_GB2_W			GROUP_GB1_W
   #define GROUP_GB2_H			205

   #define GROUP_GB3_X			GROUP_GB2_X
   #define GROUP_GB3_Y			210
   #define GROUP_GB3_W			GROUP_GB1_W
   #define GROUP_GB3_H			205

   #define GROUP_GB4_X			GROUP_GB2_X
   #define GROUP_GB4_Y			430
   #define GROUP_GB4_W			GROUP_GB1_W
   #define GROUP_GB4_H			55

   #define GROUP_BUTTON_H		17
   #define GROUP_BUTTON_W		70

   #define GROUP_LB1_X			20
   #define GROUP_LB1_Y			15
   #define GROUP_LB1_H			465
   #define GROUP_LB1_W			GROUP_GB1_W - 20

   #define GROUP_LB2_X			310
   #define GROUP_LB2_Y			GROUP_LB1_Y
   #define GROUP_LB2_H			185
   #define GROUP_LB2_W        GROUP_LB1_W

   #define GROUP_LB3_X			310
   #define GROUP_LB3_Y			225
   #define GROUP_LB3_H			GROUP_LB2_H
   #define GROUP_LB3_W        GROUP_LB1_W
// button OK
   #define GROUP_OK_X			510
   #define GROUP_OK_Y			10
// button Cancel
   #define GROUP_CANCEL_X		GROUP_OK_X
   #define GROUP_CANCEL_Y		30

// x-coordinate for all buttons, except ok & cancel
   #define GROUP_ALL_X			220
// y-coordinates for all buttons, except ok & cancel
   #define GROUP_ADDA_Y			10
   #define GROUP_REMA_Y			30
   #define GROUP_CLRA_Y			50

   #define GROUP_ADDB_Y			220
   #define GROUP_REMB_Y			240
   #define GROUP_CLRB_Y			260

   #define GROUP_ADDD_Y			440
   #define GROUP_REMD_Y			460
// checkbox 1
   #define GROUP_CB1_X			GROUP_OK_X-5
   #define GROUP_CB1_Y			180
// checkbox 2
   #define GROUP_CB2_X			GROUP_OK_X-5
   #define GROUP_CB2_Y			390
// Destination text
   #define GROUP_TEXT_X			310
   #define GROUP_TEXT_Y			445
   #define GROUP_TEXT_H			35
   #define GROUP_TEXT_W			GROUP_LB1_W

   #define ADDTO_X			GROUP_OK_X-5
   #define ADDTO_Y			445

#endif

const int    GROUP_BUTTON_OK       = wxID_HIGHEST + 4901 ;

const int    GROUP_BUTTON_CANCEL   = wxID_HIGHEST + 4902 ;

const int    GROUP_BUTTON_ADD_A    = wxID_HIGHEST + 4903 ;

const int    GROUP_BUTTON_REMOVE_A = wxID_HIGHEST + 4904 ;

const int    GROUP_BUTTON_CLR_A    = wxID_HIGHEST + 4905 ;

const int    GROUP_BUTTON_ADD_B    = wxID_HIGHEST + 4906 ;

const int    GROUP_BUTTON_REMOVE_B = wxID_HIGHEST + 4907 ;

const int    GROUP_BUTTON_CLR_B    = wxID_HIGHEST + 4908 ;

const int    GROUP_BUTTON_ADD_D    = wxID_HIGHEST + 4909 ;

const int    GROUP_BUTTON_REMOVE_D = wxID_HIGHEST + 4910 ;


BEGIN_EVENT_TABLE(GroupDialog, wxDialog)
  EVT_BUTTON    (GROUP_BUTTON_OK, GroupDialog::CmOk)

  EVT_BUTTON    (GROUP_BUTTON_CANCEL, GroupDialog::CmCancel)

  EVT_BUTTON    (GROUP_BUTTON_ADD_A, GroupDialog::CmAdd_A)

  EVT_BUTTON    (GROUP_BUTTON_REMOVE_A, GroupDialog::CmRemove_A)

  EVT_BUTTON    (GROUP_BUTTON_CLR_A, GroupDialog::CmClear_A)

  EVT_BUTTON    (GROUP_BUTTON_ADD_B, GroupDialog::CmAdd_B)

  EVT_BUTTON    (GROUP_BUTTON_REMOVE_B, GroupDialog::CmRemove_B)

  EVT_BUTTON    (GROUP_BUTTON_CLR_B, GroupDialog::CmClear_B)

  EVT_BUTTON    (GROUP_BUTTON_ADD_D, GroupDialog::CmAdd_D)

  EVT_BUTTON    (GROUP_BUTTON_REMOVE_D, GroupDialog::CmRemove_D)

  EVT_CLOSE     (GroupDialog::OnCloseWindow)

//  EVT_CHECKBOX(id, func)

END_EVENT_TABLE()


GroupDialog::GroupDialog(GDSMainWindow* parent, long style):
		wxDialog(parent,-1,"modify groups",wxPoint(0,0),wxSize(GROUP_HSIZE,GROUP_VSIZE),style,"Group dialog")
{
	_parent = parent;

	// make the controls
	_panel = new wxPanel(this,-1,wxPoint(0,0),wxSize(GROUP_HSIZE,GROUP_VSIZE),wxTAB_TRAVERSAL|wxSTATIC_BORDER ,"panel");

	_gb1 = new wxStaticBox(_panel,-1,"Layers",wxPoint(GROUP_GB1_X,GROUP_GB1_Y),wxSize(GROUP_GB1_W,GROUP_GB1_H),0,"groupdialog gb1");
	_gb2 = new wxStaticBox(_panel,-1,"Group A",wxPoint(GROUP_GB2_X,GROUP_GB2_Y),wxSize(GROUP_GB2_W,GROUP_GB2_H),0,"groupdialog gb2");
	_gb3 = new wxStaticBox(_panel,-1,"Group B",wxPoint(GROUP_GB3_X,GROUP_GB3_Y),wxSize(GROUP_GB3_W,GROUP_GB3_H),0,"groupdialog gb3");
	_gb4 = new wxStaticBox(_panel,-1,"Destination",wxPoint(GROUP_GB4_X,GROUP_GB4_Y),wxSize(GROUP_GB4_W,GROUP_GB4_H),0,"groupdialog gb4");

	_ok = new wxButton		(_panel,GROUP_BUTTON_OK,		"Ok",			wxPoint(GROUP_OK_X,		GROUP_OK_Y),wxSize(GROUP_BUTTON_W,GROUP_BUTTON_H));
	_cancel = new wxButton	(_panel,GROUP_BUTTON_CANCEL,	"Cancel",	wxPoint(GROUP_CANCEL_X,GROUP_CANCEL_Y),wxSize(GROUP_BUTTON_W,GROUP_BUTTON_H));
	_adda = new wxButton		(_panel,GROUP_BUTTON_ADD_A,	"add >>",	wxPoint(GROUP_ALL_X,	GROUP_ADDA_Y),wxSize(GROUP_BUTTON_W,GROUP_BUTTON_H));
	_rema = new wxButton		(_panel,GROUP_BUTTON_REMOVE_A,"<< remove",wxPoint(GROUP_ALL_X,	GROUP_REMA_Y),wxSize(GROUP_BUTTON_W,GROUP_BUTTON_H));
	_clra = new wxButton		(_panel,GROUP_BUTTON_CLR_A,	"clear",		wxPoint(GROUP_ALL_X,	GROUP_CLRA_Y),wxSize(GROUP_BUTTON_W,GROUP_BUTTON_H));
	_addb = new wxButton		(_panel,GROUP_BUTTON_ADD_B,	"add >>",	wxPoint(GROUP_ALL_X,	GROUP_ADDB_Y),wxSize(GROUP_BUTTON_W,GROUP_BUTTON_H));
	_remb = new wxButton		(_panel,GROUP_BUTTON_REMOVE_B,"<< remove",wxPoint(GROUP_ALL_X,	GROUP_REMB_Y),wxSize(GROUP_BUTTON_W,GROUP_BUTTON_H));
	_clrb = new wxButton		(_panel,GROUP_BUTTON_CLR_B,	"clear",		wxPoint(GROUP_ALL_X,	GROUP_CLRB_Y),wxSize(GROUP_BUTTON_W,GROUP_BUTTON_H));
	_addd = new wxButton		(_panel,GROUP_BUTTON_ADD_D,	"add >>",	wxPoint(GROUP_ALL_X,	GROUP_ADDD_Y),wxSize(GROUP_BUTTON_W,GROUP_BUTTON_H));
	_remd = new wxButton		(_panel,GROUP_BUTTON_REMOVE_D,"<< remove",wxPoint(GROUP_ALL_X,	GROUP_REMD_Y),wxSize(GROUP_BUTTON_W,GROUP_BUTTON_H));

	_listbox_main = new wxListBox(_panel,-1,wxPoint(GROUP_LB1_X,GROUP_LB1_Y),wxSize(GROUP_LB1_W,GROUP_LB1_H),0,NULL,wxLB_NEEDED_SB |wxLB_MULTIPLE);
	_listbox_a = new wxListBox(_panel,-1,wxPoint(GROUP_LB2_X,GROUP_LB2_Y),wxSize(GROUP_LB2_W,GROUP_LB2_H),0,NULL,wxLB_NEEDED_SB | wxLB_MULTIPLE);
	_listbox_b = new wxListBox(_panel,-1,wxPoint(GROUP_LB3_X,GROUP_LB3_Y),wxSize(GROUP_LB3_W,GROUP_LB3_H),0,NULL,wxLB_NEEDED_SB | wxLB_MULTIPLE);

	_onlyselect_A = new wxCheckBox(_panel,-1,"Only selected",wxPoint(GROUP_CB1_X,GROUP_CB1_Y),wxSize(100,20));
	_onlyselect_B = new wxCheckBox(_panel,-1,"Only selected",wxPoint(GROUP_CB2_X,GROUP_CB2_Y),wxSize(100,20));
	m_add_to_destination = new wxCheckBox(_panel,-1,"Add to",wxPoint(ADDTO_X,ADDTO_Y),wxSize(100,20));
	_destination = new wxTextCtrl(_panel,-1,"",wxPoint(GROUP_TEXT_X,GROUP_TEXT_Y),wxSize(GROUP_TEXT_W,GROUP_TEXT_H));
	group_A=NULL;
	group_B=NULL;
}

GroupDialog::~GroupDialog()
{
	if (group_A) delete group_A;
	if (group_B) delete group_B;
}

void GroupDialog::SetupWindow(Setup* setup, group* old_A, group* old_B, int old_D,bool old_add)
{
	_setup = setup;

	int i;
	group_A = new group;
	for (i = 0; i < MAX_LAYERS; i++) group_A->SetLayer(i,old_A->GetLayer(i));
	group_A->SetOnlySelected(old_A->GetOnlySelected());

	group_B = new group;
	for (i = 0; i < MAX_LAYERS; i++) group_B->SetLayer(i,old_B->GetLayer(i));
	group_B->SetOnlySelected(old_B->GetOnlySelected());

	_destinationnr = old_D;
	_listbox_main->Clear();
	_listbox_a->Clear();
	_listbox_b->Clear();

	// fill the listbox
	char buf[60];
	char available[3];
	for (i = 0; i < MAX_LAYERS; i++) {
		if (_setup->Get_Available(i))
			strcpy(available,"->");
		else
			strcpy(available,"  ");

		sprintf(buf, "%02d   %s  %s", i, available, _setup->Get_Name(i));

		_listbox_main->Append(buf);

		// are there selected?
		if (In_A(i))
			_listbox_a->Append(buf);
		if (In_B(i))
			_listbox_b->Append(buf);

		if (_destinationnr == i)
		{
			if (_setup->Get_Available(i))
				sprintf(buf, "%02d   %s  %s", i, "->", _setup->Get_Name(i));
			else
				sprintf(buf, "%02d       %s", i, _setup->Get_Name(i));
			_destination->SetValue(buf);
		}
	}
	_onlyselect_A->SetValue(group_A->GetOnlySelected());
	_onlyselect_B->SetValue(group_B->GetOnlySelected());
	m_add_to_destination->SetValue(old_add);
	// set first item in the mainlistbox selected
	_listbox_main->SetSelection(0,TRUE);

	Centre(wxBOTH);
}

void GroupDialog::CmCancel(wxCommandEvent&)
{
   EndModal(GROUP_BUTTON_CANCEL);
}

// the user pressed OK. This function is called then
void GroupDialog::CmOk(wxCommandEvent&)
{
	int i, count;
	char buf[60];

	// get the number of selected item in listbox a
	count = _listbox_a->GetCount();

	// fill the group with the new settings
	group_A->Clear();
	for (i = 0; i < count; i++)
	{
		strcpy(buf,_listbox_a->GetString(i));
		buf[2] = 0;
		group_A->SetLayer(atoi(buf),true);
	}

	count = _listbox_b->GetCount();
	// fill the group with the new settings
	group_B->Clear();
	for (i = 0; i < count; i++)
	{
		strcpy(buf,_listbox_b->GetString(i));
		buf[2] = 0;
		group_B->SetLayer(atoi(buf),true);
	}

	// get destinationlayer
	strcpy(buf,_destination->GetValue());
	if (strlen(buf) > 1)
	{
		buf[2] = 0;
		_destinationnr = atoi(buf);
	}
	else
		_destinationnr = -1;

	group_A->SetOnlySelected((bool)_onlyselect_A->GetValue());
	group_B->SetOnlySelected((bool)_onlyselect_B->GetValue());
	char	group[1024];
	Get_Group_A(group);
	GLOB->CMDH->DoCommand("group_a %s",group);
	Get_Group_B(group);
	GLOB->CMDH->DoCommand("group_b %s",group);
	Get_Destination(group);
   if (m_add_to_destination->GetValue() == true)
		GLOB->CMDH->DoCommand("destination %s true",group);
   else
		GLOB->CMDH->DoCommand("destination %s false",group);
   EndModal(GROUP_BUTTON_OK);
}

// is the layer in group a?
bool GroupDialog::In_A(int layer)
{
	return group_A->GetLayer(layer);

}

// is the layer in group b?
bool GroupDialog::In_B(int layer)
{
	return group_B->GetLayer(layer);
}

// the user presses the add to a button
void GroupDialog::CmAdd_A(wxCommandEvent&)
{
	int i, not_available = 0;
	char buf[60];
	wxArrayInt selections;
	int 	selcount;

	selcount = _listbox_main->GetSelections(selections);

	if (selcount == 0) return;

	// first check if the layers are in the drawing
	for (i = 0; i < selcount; i++)
	{
		strcpy(buf,_listbox_main->GetString(selections[i]));
		buf[3] = 0;
		if (!_setup->Get_Available(atoi(buf)))
		{
			not_available = 1;
			break;
		}
	}

	// is this layer available?
	if (not_available)
	{
		_messagehandler->info("You can't select layers for a group that are not available.", "Selection error");
		return;
	}

	// copy them in the group
	for (i = 0; i < selcount; i++)
	{
		strcpy(buf,_listbox_main->GetString(selections[i]));
		int result = _listbox_a->FindString(buf);
		if (result == -1) _listbox_a->Append(buf);
//		_listbox_main->Deselect(selections[i]);
	}
}

// the user presses add to b
void GroupDialog::CmAdd_B(wxCommandEvent&)
{
	int i, not_available = 0;
	char buf[60];
	wxArrayInt selections;
	int 	selcount;

	selcount = _listbox_main->GetSelections(selections);

	if (selcount == 0) return;

	// first check if the layers are in the drawing
	for (i = 0; i < selcount; i++)
	{
		strcpy(buf,_listbox_main->GetString(selections[i]));
		buf[3] = 0;
		if (!_setup->Get_Available(atoi(buf)))
		{
			not_available = 1;
			break;
		}
	}

	// is this layer available?
	if (not_available)
	{
		_messagehandler->info("You can't select layers for a group that are not available.", "Selection error");
		return;
	}

	// copy them in the group
	for (i = 0; i < selcount; i++)
	{
		strcpy(buf,_listbox_main->GetString(selections[i]));
		int result = _listbox_b->FindString(buf);
		if (result == -1) _listbox_b->Append(buf);
		_listbox_main->Deselect(selections[i]);
	}
}

// the user selected add to destination
void GroupDialog::CmAdd_D(wxCommandEvent&)
{
	wxArrayInt tmp;

	if (_listbox_main->GetSelections(tmp) > 1)
	{
		_messagehandler->info("You can select only one\nlayer for the destination!", "Note");
		return;
	}

	if (_listbox_main->GetSelections(tmp) == 0) return;

	char buf[60], buf2[60], number[3];

	strcpy(buf,_listbox_main->GetString(tmp[0]));
	strncpy(number, buf, 2);
	number[2] = 0;

	if (_setup->Get_Available(atoi(number)))
		sprintf(buf2, "%s   %s  %s", number, "->", buf+8);
	else
		sprintf(buf2, "%s       %s", number, buf+8);

	_destination->SetValue(buf2);
	_listbox_main->Deselect(tmp[0]);
}

void GroupDialog::CmRemove_A(wxCommandEvent&)
{
	wxArrayInt selections;
	int 	selcount, i;

	selcount = _listbox_a->GetSelections(selections);

	for (i = selcount-1; i >= 0; i--)
		_listbox_a->Delete(selections[i]);
}

void GroupDialog::CmRemove_B(wxCommandEvent&)
{
	wxArrayInt selections;
	int 	selcount, i;

	selcount = _listbox_b->GetSelections(selections);

	for (i = selcount-1; i >= 0; i--)
		_listbox_b->Delete(selections[i]);
}

void GroupDialog::CmRemove_D(wxCommandEvent&)
{
	_destination->SetValue("");
}

void GroupDialog::CmClear_A(wxCommandEvent&)
{
	_listbox_a->Clear();
}

void GroupDialog::CmClear_B(wxCommandEvent&)
{
	_listbox_b->Clear();
}

char* GroupDialog::Get_Group_A(char* group_str)
{
	char int_str[20];

	strcpy(group_str,"");

	if (group_A->GetNumberOfLayers() != 0) {
		int i;
		for (i = 0; i < MAX_LAYERS; i++) {
			if (group_A->GetLayer(i)) {
				sprintf(int_str,"%02d ",i);
				strcat(group_str,int_str);
			}
		}
		if (group_A->GetOnlySelected()) strcat(group_str,"true");
      else strcat(group_str,"false");
	} else strcpy(group_str,"empty");

	return group_str;
}

char* GroupDialog::Get_Group_B(char* group_str)
{
	char int_str[20];

	strcpy(group_str,"");

	if (group_B->GetNumberOfLayers() != 0) {
		int i;
		for (i = 0; i < MAX_LAYERS; i++) {
			if (group_B->GetLayer(i)) {
				sprintf(int_str,"%02d ",i);
				strcat(group_str,int_str);
			}
		}
		if (group_B->GetOnlySelected()) strcat(group_str,"true");
      else strcat(group_str,"false");
	} else strcpy(group_str,"empty");

	return group_str;
}

char* GroupDialog::Get_Destination(char* group_str)
{
 if (_destinationnr != -1)
	 sprintf(group_str,"%02d",_destinationnr);
 else
	 strcpy(group_str,"empty");
 return group_str;
}

void GroupDialog::OnCloseWindow(wxCloseEvent& event)
{
   EndModal(GROUP_BUTTON_CANCEL);
}





