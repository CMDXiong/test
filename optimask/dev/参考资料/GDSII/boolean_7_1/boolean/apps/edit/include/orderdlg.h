//! author="J.C. Mollien, last modified by M. Reemst"
//! version="1.0"
/*
 * Definition of class for dialog to specify the layerorder.
*/

#ifndef ORDERDLG_H
#define ORDERDLG_H

#ifdef __GNUG__
#pragma interface
#endif

//! codefiles="orderdlg.cpp"
//! headerfiles="mainwin.h gdsapp.h lsetup.h wxwin.h"

//:defenition
//!c_des: Class LayerOrder.
//!c_des: This class is used to show the layerorder dialog, in which the order of the used layers
//!c_des: can be set.

class LayerOrder: public wxDialog
{
	public:

		// constructor.
		LayerOrder(GDSMainWindow* parent,char* title,long style =0 ,char* name = "main_layer_dlg");

		// destructor.
		~LayerOrder();

      // wxWindows class.
		wxListBox* Get_ListBox();

      // wxWindows class.
		wxButton* Get_ButtonUP();

      // wxWindows class.
		wxButton* Get_ButtonDOWN();

      // wxWindows class.
		wxMessageDialog* Get_Text();

      // wxWindows initialisation.
		void SetupWindow();

      //
      void StoreOrder();

      // Close window if OK-button is pressed.
      void	CmOk(wxCommandEvent &);

      // Close window if CANCEL-button is pressed.
      void	CmCancel(wxCommandEvent &);

      // Move selected layer to a heigher layersetting if UP-button is pressed.
      void	CmUp(wxCommandEvent &);

      // Move selected layer to a lower layersetting if the DOWN-button is pressed.
      void	CmDown(wxCommandEvent &);

      // Show new layerorder if the SHOW-button is pressed.
      void	CmShow(wxCommandEvent &);

      // Make sure the UP- and DOWN-button can't be used if the top or the bottom of the list
      // has been reached.
      void	CmListBox(wxCommandEvent &);

      // Close window if EXIT-button is pressed.
      void 	OnCloseWindow(wxCloseEvent& event);

	protected:

      void Update(wxUpdateUIEvent &);

		// Pointer to mainwindow
		GDSMainWindow*	_parent;

		// wxWindows class.
		wxPanel*		_panel1;

      // wxWindows class.
		wxButton*	_buttonOK;

      // wxWindows class.
		wxButton*	_buttonCANCEL;

      // wxWindows class.
		wxStaticBox* _groupbox;

      // wxWindows class.
		wxListBox*	_listbox;

      // wxWindows class.
		wxStaticText*	_text1;

      // wxWindows class.
		wxButton*	_buttonUP;

      // wxWindows class.
		wxButton*	_buttonDOWN;

      // wxWindows class.
		wxButton*	_buttonSHOW;

      // Declare used events.
      DECLARE_EVENT_TABLE()

};

#endif
