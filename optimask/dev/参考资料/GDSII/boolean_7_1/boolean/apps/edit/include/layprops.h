//! author="Mollien J.C."
//! version="1.0"
/*
 * Definition of class for dialog to specify the layerproperties.
*/

#ifndef LAYERPROPS_H
#define LAYERPROPS_H

#ifdef __GNUG__
#pragma interface
#endif

#include <wx/colordlg.h>

#define wxTEST_GENERIC_DIALOGS_IN_MSW 0

#if defined(__WXMSW__) && wxTEST_GENERIC_DIALOGS_IN_MSW

#include <wx/generic/colrdlgg.h>

#endif

//! codefiles="layprops.cpp"
//! headerfiles="wxwin.h mainwin.h lsetup.h misc.h chandbas.h lpdsizes.h windrv.h patdlg.h""

//:defenition
//!c_des: Class LayerProperties.
//!c_des: This class is used to show a layerproperties dialog, in which the layernames, layernumber, visibility, select/read,
//!c_des: fillcolour, border, pattern and mapping can be set.

class LayerPropertiesDialog : public wxDialog
{
	public:

   	// Constructor.
		LayerPropertiesDialog(GDSMainWindow* parent,char* title,long style =0 ,char* name = "Layer_Properties_Dialog");

      // Destructor.
		~LayerPropertiesDialog();

      //
		Setup* Get_New_Setup();

      // Redraw layout.
      void			Redraw(wxCommandEvent &);

      // Set control properties.
		void			FillControls(int start);

      // Store control settings.
		void			StoreControls(int start);

      // Windows initialisarion.
		void			SetupWindow(Setup* prev_setup);

      //close/hide window if HIDE-button is pressed.
		void 			CmHide(wxCommandEvent &);

      // redraw canvas to see the changes
		void 			CmRedraw(wxCommandEvent &);

      // Close window if CANCEL-button is pressed.
      void 			CmCancel(wxCommandEvent &);

      void 			CmVisible(wxCommandEvent &);

      // Set read properties.
      void 			CmRead(wxCommandEvent &);

      // Go to previous set of layers if PREVIOUS-button is pressed.
      void       	CmPrev(wxCommandEvent &);

      // Go to next set of layers if NEXT-button is pressed.
      void        CmNext(wxCommandEvent &);

      // Select layer.
      void			CmSelect(wxCommandEvent &);

      // pointer to the setup as was.
		Setup*				_prev_setup;

      // pointer to the backup setup.
		Setup*				_back_setup;

      // pointer to the setup new.
		Setup*				_setup;

      // will increase or decrease with 10 (ten layers on the screen).
		int					_startvalue;

      // wxWindows class.
		wxButton*	_buttonHIDE;

      // wxWindows class.
		wxButton*	_buttonCANCEL;

      // wxWindows class.
		wxButton*	_buttonVISIBLE;

      // wxWindows class.
		wxButton*	_buttonREAD;

      // wxWindows class.
		wxButton*	_buttonSELECT;

      // wxWindows class.
		wxButton*	_buttonPREV;

      // wxWindows class.
		wxButton*	_buttonNEXT;

      // wxWindows class.
		wxButton*	_buttonREDRAW;

	protected:

		void 				Update(wxUpdateUIEvent &);

		void 				OnChar(wxKeyEvent& event);

		void 				OnCharHook(wxKeyEvent& event);

		void				Cm_but(wxEvent& event);

		void 				Cm_B_BitB(wxEvent& event);

      //
		void 				Cm_F_BitB(wxEvent& event);

      //
		void 				Cm_P_BitB(wxEvent& event);

      //
		wxColour 		ltowxc(long colour);

      //
		long 				wxctol(wxColour colour);

      // Pointer to mainwindow.
		GDSMainWindow*	_parent;

      // wxWindows class, layer number edit control.
		wxStaticText*	_numberbox[10];

      // wxWindows class, layer name edit control.
		wxTextCtrl*	 	_namebox[10];

      // wxWindows class, layer visible checkbox control.
		wxCheckBox*		_visiblebox[10];

      // wxWindows class, layer selectable checkbox control.
		wxCheckBox*		_selectablebox[10];

      // wxWindows class, layer readlayer checkbox control.
		wxCheckBox*		_readlayerbox[10];

      // wxWindows class, layer fill color buttons.
		wxButton* 	_fillbut[10];

      // wxWindows class, layer border color buttons.
		wxButton* 	_borderbut[10];

      // wxWindows class, layer pattern color buttons.
		wxBitmapButton* 	_patternbut[10];

      // wxWindows class, mapping for gds in.
		wxTextCtrl*	 	_gdsinmap[10];

      // wxWindows class, mapping for gds out.
		wxTextCtrl*	 	_gdsoutmap[10];

      // wxWindows class.
		wxStaticText*	_numberlabel;

      // wxWindows class.
		wxStaticText*	_namelabel;

      // wxWindows class.
		wxStaticText*	_availablelabel;

      // wxWindows class.
		wxStaticText*	_visiblelabel;

      // wxWindows class.
		wxStaticText*	_selectablelabel;

      // wxWindows class.
		wxStaticText*	_readlayerlabel;

      // wxWindows class.
		wxStaticText*	_filllabel;

      // wxWindows class.
		wxStaticText*	_borderlabel;

      // wxWindows class.
      wxStaticText*	_patternlabel;

      // wxWindows class.
      wxStaticText*	_mappinglabel;

      // wxWindows class.
      wxBitmap*      _p_bitm[10];

      // wxWindows class.
      PatternDialog* _patternDialog;

      // wxWindows class, the brush.
		wxBrush*				_brush;

      // wxWindows class, to help construct the colour dialog.
		wxColourData* 		_cdata;

      // wxWindows class, the colour dialog.
		wxColourDialog* 	_cd;

      // Declare used events.
      DECLARE_EVENT_TABLE()


};

#endif
