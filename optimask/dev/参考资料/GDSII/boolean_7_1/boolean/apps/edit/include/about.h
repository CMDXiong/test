#ifdef __GNUG__
#pragma interface
#endif

#ifndef ABOUT_H
#define ABOUT_H

class About: public wxDialog
{
	public:
		About(GDSMainWindow*,char*,long style = (wxTHICK_FRAME | wxCAPTION),char *name="frame");
		~About();
      void CmOk(wxCommandEvent &);
      void OnCloseWindow(wxCloseEvent& event);

	protected:
		GDSMainWindow*	_parent;

		wxButton*		_okButton;
		wxPanel*			_panel;
		wxStaticText*	_text1;
		wxStaticText*	_text2;
		wxListBox*		_lb;

      DECLARE_EVENT_TABLE()

};

#endif
