#ifndef _PROPEDIT
#define _PROPEDIT

#ifdef __GNUG__
#pragma interface
#endif



#include "proplist.h"

#include "wxwin.h"
class	MywxPropertyListView: public wxPropertyListView
{
   public:
		MywxPropertyListView(wxPanel *propPanel, long flags);
      wxProperty* GetSelected();
};

class PropEditDialog: public wxDialog
{

   public:

		PropEditDialog(wxWindow*,const wxString&,long style ,const wxString& name,wxPropertySheet* wxpropsheet);

		~PropEditDialog();

      void 				OnCloseWindow(wxCloseEvent& event);

      void				Add(wxCommandEvent &);
      void				Remove(wxCommandEvent &);

      void				Ok(wxCommandEvent &);

      void				Cancel(wxCommandEvent &);



      void RegisterValidators(void);



	protected:



	   bool Register;

	   wxPropertySheet* m_sheet;

		MywxPropertyListView* view;

		wxWindow*		_parent;

  		wxPanel*			_panel;

		wxButton*		_addButton;
		wxButton*		_okButton;
		wxButton*		_cancelButton;
		wxButton*		_removeButton;

      DECLARE_EVENT_TABLE()


};





class PropAddDialog: public wxDialog

{

   public:

		PropAddDialog(wxWindow*,char*,long style ,char *name);

		~PropAddDialog();

      void 				OnCloseWindow(wxCloseEvent& event);

      wxString			GetName();
      wxString			GetType();
      wxString 		GetValue();


	protected:



      void				Ok(wxCommandEvent &);

      void				Cancel(wxCommandEvent &);



	   bool Register;



		wxWindow*		_parent;

  		wxPanel*			_panel;

		wxStaticText*  _tname;
		wxStaticText*  _ttype;
		wxStaticText*  _tvalue;
		wxTextCtrl*		_name;
		wxChoice*		_type;
		wxTextCtrl*		_value;
		wxButton*		_okButton;
		wxButton*		_cancelButton;

      DECLARE_EVENT_TABLE()

};





#endif
