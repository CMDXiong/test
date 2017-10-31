//! author="J.C. Mollien, last modified by M. Reemst"
//! version="1.0"
/*
 * Definition of class for dialog to insert a new layer.
*/

#ifndef INSDLG_H
#define INSDLG_H

#ifdef __GNUG__
#pragma interface
#endif

#include "chandbas.h"
#include "wx/minifram.h"
#include "wx/grid.h"

//! codefiles="insdlg.cpp"
//! headerfiles="mainwin.h gdsapp.h lsetup.h wxwin.h chandbas.h wx/minifram.h"

//:defenition
//!c_des: Class InsLayer.
//!_cdes: This class is used to show the layerorder dialog, in which the order of the used layers
//!_cdes: can be set.

class InsLayer: public wxMiniFrame
{
	public:

		// Constructor
		InsLayer(GDSMainWindow* parent,char* title,long style = 0 ,char* name = "insert_layer_dlg");

		// Destructor
		~InsLayer();

      // Windows initialization.
		void SetupWindow();

      // Gets selection and gives command "setlayer" to the commandhandler to set the current layer.
      void	CmListBox();

      // Close window if EXIT-button is pressed.
      void 	OnCloseWindow(wxCloseEvent& event);

      void OnLabelLeftClick(wxGridEvent& ev );
      void OnSelectCell(wxGridEvent& ev );
//      void OnCellValueChanged(wxGridEvent& ev );
//      void OnCellLeftClick(wxGridEvent& ev );

	protected:
		void 			 Update(wxUpdateUIEvent&);

		// Pointer to mainwindow.
		GDSMainWindow*	_parent;

		// wxWindows class.
		wxPanel*		_panel1;

      //wxWindows class.
		wxGrid*	   m_grid;

      // Declare used events.
      DECLARE_EVENT_TABLE()
};

class MyBoolEdit : public wxGridCellBoolEditor
{
public:
    void BeginEdit(int row, int col, wxGrid* grid);
    bool EndEdit(int row, int col, wxGrid* grid);
    void StartingClick();
    void Create(wxWindow* parent,wxWindowID id,wxEvtHandler* evtHandler);

    int m_row;
    int m_col;
};

class MyCheckBox : public wxCheckBox
{
public:
      MyCheckBox(wxWindow *parent,
                        wxWindowID id,
                        const wxString& label,
                        const wxPoint& pos,
                        const wxSize& size, long style , MyBoolEdit* edit
                 );
      void OnMouseEvent(wxMouseEvent& event);
      DECLARE_EVENT_TABLE()
      MyBoolEdit* m_edit;
};

#endif
