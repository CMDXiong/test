#ifndef WAIT_H
#define WAIT_H

#ifdef __GNUG__
#pragma interface
#endif

// layout defines

#define WAIT_HSIZE			300
#define WAIT_VSIZE			100

#ifdef __UNIX__
#define WAIT_BUTTON_H		25
#define WAIT_BUTTON_W		70
#define WAIT_BUTTON1_X  	120
#define WAIT_BUTTON1_Y		70
#define WAIT_MESSAGE1_X    50
#define WAIT_MESSAGE1_Y    40
#else
#define WAIT_BUTTON_H		17
#define WAIT_BUTTON_W		70
#define WAIT_BUTTON1_X  	120
#define WAIT_BUTTON1_Y		50
#define WAIT_MESSAGE1_X    50
#define WAIT_MESSAGE1_Y    25
#endif

class Wait: public wxDialog
{
	public:
		Wait(GDSMainWindow*,char*,int,int,int,int,long style = (wxTHICK_FRAME | wxCAPTION),char *name="waitFrame"); 
		~Wait();

		void		SetMessage(char*);
		void		CmOk(wxCommandEvent &);

	protected:

		char*			_message;		// holds the message
		wxPanel*		_panel;
		wxButton*	_okButton;
		wxStaticText*	_text1;

      DECLARE_EVENT_TABLE()

};

#endif
