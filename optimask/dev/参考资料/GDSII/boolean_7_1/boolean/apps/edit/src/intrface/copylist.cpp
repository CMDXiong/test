#ifdef __GNUG__
#pragma implementation
#endif

#include "wxwin.h"
#include <wx/clipbrd.h>


#include "copylist.h"

const int    CP_SEL = wxID_HIGHEST + 8801 ;
const int    CP_ALL = wxID_HIGHEST + 8802 ;

BEGIN_EVENT_TABLE(CopywxListBox, wxListBox)
   EVT_RIGHT_DOWN(CopywxListBox::OnMouseRightDown)
   EVT_MENU( CP_SEL,CopywxListBox::CopySel)
   EVT_MENU( CP_ALL,CopywxListBox::CopyAll)
END_EVENT_TABLE()

CopywxListBox::CopywxListBox(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size,
                  int n,const wxString choices[],long style)
   :wxListBox	(parent,id,pos,size,n,choices,style)
{
   // make a popupmenu for the right mousebutton
   m_mousemenu = new wxMenu("Copy",(long)0);
	m_mousemenu->Append(CP_SEL, "selected");
	m_mousemenu->Append(CP_ALL, "all");
}

CopywxListBox::~CopywxListBox()
{
	delete m_mousemenu;
}

void CopywxListBox::CopySel(class wxCommandEvent &event)
{
   if (wxTheClipboard->Open())
   {
      wxString data;
      data=data+GetStringSelection()+"\n";
      wxTheClipboard->SetData( new wxTextDataObject(data));
      wxTheClipboard->Close();
   }
}

void CopywxListBox::CopyAll(class wxCommandEvent &event)
{
   if (wxTheClipboard->Open())
   {
      wxString data;
      int i;
      for (i = 0; i < GetCount(); i++)
      {
         data=data+GetString(i)+"\n";
      }
      wxTheClipboard->SetData( new wxTextDataObject(data));
      wxTheClipboard->Close();
   }
}

void CopywxListBox::OnMouseRightDown(class wxMouseEvent &event)
{
   PopupMenu(m_mousemenu,0,0);
}
