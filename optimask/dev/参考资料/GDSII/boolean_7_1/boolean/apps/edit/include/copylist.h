#ifndef COPYLIST_H
#define COPYLIST_H

#ifdef __GNUG__
#pragma interface
#endif

#include "wxwin.h"
#include <wx/clipbrd.h>

//! codefiles="copylist.cpp"
//! headerfiles="copylist.h"


class CopywxListBox: public wxListBox

{

public:

	CopywxListBox(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size,
                  int n,const wxString choices[],long style);
	~CopywxListBox();
   void OnMouseRightDown(class wxMouseEvent &);
   void CopySel(class wxCommandEvent &event);
   void CopyAll(class wxCommandEvent &event);
   wxMenu* m_mousemenu;						// the floating menu

   // Declare used events.
   DECLARE_EVENT_TABLE()

};
#endif
