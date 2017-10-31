/*
	name				: orderdlg.cpp
	purpose        : interface for layerdlg.cpp, which is the
   					  implementation of the layerorder dialog
	author         : J.C. Mollien.
	created        : 97-10-09
	last modified	: 98-03-03, by M. Reemst
	notes				: this is a new version of orderdlg. the old OWL version is
     					  renamed as orderdl1.cpp and orderdl1.h. This version will
					     contain only wxWindows functions.
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "mainwin.h"
#include "gdsapp.h"
#include "lsetup.h"
#include "wxwin.h"

#include "insdlg.h"

const int    INS_LIST_SELECT   = wxID_HIGHEST + 5501 ;
const int    INS_UPDATE_DIALOG = wxID_HIGHEST + 5502 ;
const int    B_ID = wxID_HIGHEST + 5503 ;

BEGIN_EVENT_TABLE(InsLayer, wxMiniFrame)
  EVT_UPDATE_UI(INS_UPDATE_DIALOG, InsLayer::Update)
  EVT_GRID_SELECT_CELL( InsLayer::OnSelectCell )
  EVT_GRID_LABEL_LEFT_CLICK( InsLayer::OnLabelLeftClick )
//  EVT_GRID_CELL_CHANGE( InsLayer::OnCellValueChanged )
//  EVT_GRID_CELL_LEFT_CLICK( InsLayer::OnCellLeftClick )

  EVT_CLOSE    (InsLayer::OnCloseWindow)

END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyCheckBox, wxCheckBox)
  EVT_MOUSE_EVENTS(MyCheckBox::OnMouseEvent)
END_EVENT_TABLE()

MyCheckBox::MyCheckBox(wxWindow *parent,
                        wxWindowID id,
                        const wxString& label,
                        const wxPoint& pos,
                        const wxSize& size, long style, MyBoolEdit* edit
                      ):
     wxCheckBox(parent,id,label,pos,size,style)
{
   m_edit=edit;
}

void MyCheckBox::OnMouseEvent(wxMouseEvent& event)
{
   if (event.LeftDown())
   {
      SetValue(!GetValue());
      switch (m_edit->m_col)
      { case 0:
            break;
        case 1:
        {
            if (GetValue())
               GLOB->CMDH->DoCommand("visible 1");
            else
               GLOB->CMDH->DoCommand("visible 0");
            if (GLOB->SETUP->Get_Available(GLOB->CMDH->GetCurrentLayer()))
               GLOB->CMDH->DoCommand("draw");
            break;
        }
        case 2:
            if (GetValue())
               GLOB->CMDH->DoCommand("selectable 1");
            else
               GLOB->CMDH->DoCommand("selectable 0");
            break;
        default:
            break;
      }
   }
}

void MyBoolEdit::BeginEdit(int row, int col, wxGrid* grid)
{
   m_row=row;
   m_col=col;
   wxGridCellBoolEditor::BeginEdit(row,col,grid);
}

bool MyBoolEdit::EndEdit(int row, int col, wxGrid* grid)
{
   m_row=row;
   m_col=col;
   return (wxGridCellBoolEditor::EndEdit(row,col,grid));
}

void MyBoolEdit::StartingClick()
{
   CBox()->SetValue(!CBox()->GetValue());
   switch (m_col)
   { case 0:
         break;
     case 1:
     {
         if (CBox()->GetValue())
            GLOB->CMDH->DoCommand("visible 1");
         else
            GLOB->CMDH->DoCommand("visible 0");
         if (GLOB->SETUP->Get_Available(GLOB->CMDH->GetCurrentLayer()))
            GLOB->CMDH->DoCommand("draw");
         break;
     }
     case 2:
         if (CBox()->GetValue())
            GLOB->CMDH->DoCommand("selectable 1");
         else
            GLOB->CMDH->DoCommand("selectable 0");
         break;
     default:
         break;
   }
}

void MyBoolEdit::Create(wxWindow* parent,
                                  wxWindowID id,
                                  wxEvtHandler* evtHandler)
{
    m_control = new MyCheckBox(parent, -1, wxEmptyString,
                               wxDefaultPosition, wxDefaultSize,
                               wxNO_BORDER,this);

    wxGridCellEditor::Create(parent, id, evtHandler);
}




InsLayer::InsLayer(GDSMainWindow* parent,char* title,long style,char* name):
	wxMiniFrame(parent,-1,title,wxDefaultPosition,wxSize(145,500),style,name)
{
	_parent = parent;

	_panel1 			= new wxPanel(this,INS_UPDATE_DIALOG,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER ,"6");
	m_grid			= new wxGrid(_panel1,-1,wxDefaultPosition,wxDefaultSize);

   m_grid->CreateGrid(200,3);
   m_grid->SetLabelSize(wxVERTICAL,30);
   m_grid->SetLabelBackgroundColour(*wxCYAN);
   m_grid->SetLabelValue(wxHORIZONTAL,"layer",0);
   m_grid->SetLabelValue(wxHORIZONTAL,"visible",1);
   m_grid->SetLabelValue(wxHORIZONTAL,"select",2);
   m_grid->SetColSize(0,50);
   m_grid->SetColSize(1,20);
   m_grid->SetColSize(2,20);

   this->SetAutoLayout(TRUE);
   // Set constraints for panel subwindow
   wxLayoutConstraints *c1 = new wxLayoutConstraints;
   c1->left.SameAs       (this, wxLeft);
   c1->top.SameAs        (this, wxTop);
   c1->right.SameAs      (this, wxRight);
   c1->bottom.SameAs     (this, wxBottom);
   _panel1->SetConstraints(c1);

   //grid
   wxLayoutConstraints *c9 = new wxLayoutConstraints;
   c9->left.SameAs       (_panel1, wxLeft);
   c9->top.SameAs        (_panel1, wxTop);
   c9->right.SameAs      (_panel1, wxRight);
   c9->bottom.SameAs     (_panel1, wxBottom);
   m_grid->SetConstraints(c9);

	Centre(wxBOTH);

   Layout();

}

InsLayer::~InsLayer()
{
}

void InsLayer::SetupWindow()
{

	// set the items in the right order in the listbox
	for (int i = 0; i < MAX_LAYERS; i++)
	{
		for (int x = 0; x < MAX_LAYERS; x++)
		{
			if (GLOB->SETUP->Get_Order(x) == i)
			{
				char _buf[60];
				// insert the layer in the listbox
				sprintf(_buf, "%03d", x);
            m_grid->SetLabelValue(wxVERTICAL,_buf,i);
				sprintf(_buf,"%s", GLOB->SETUP->Get_Name(x));
            if (!GLOB->SETUP->Get_Available(x))
               m_grid->SetCellBackgroundColour(i,0,*wxWHITE);
            else
               m_grid->SetCellBackgroundColour(i,0,*wxRED);
            m_grid->SetCellValue(i,0,_buf);
            if (GLOB->SETUP->Get_Visible(x))
               m_grid->SetCellValue(i,1,"1");
            else
               m_grid->SetCellValue(i,1,"");
            m_grid->SetCellRenderer(i,1, new wxGridCellBoolRenderer);
            m_grid->SetCellEditor(i,1, new MyBoolEdit);
            if (GLOB->SETUP->Get_Selectable(x))
               m_grid->SetCellValue(i,2,"1");
            else
               m_grid->SetCellValue(i,2,"");
            m_grid->SetCellRenderer(i,2, new wxGridCellBoolRenderer);
            m_grid->SetCellEditor(i,2, new MyBoolEdit);
			}
		}
	}
}

void InsLayer::OnCloseWindow(wxCloseEvent& event)
{
	_parent->ShowInsertLayerDlg();
}

void InsLayer::OnSelectCell(wxGridEvent& ev )
{
	char _buf[100];
	strcpy(_buf,m_grid->GetLabelValue(wxVERTICAL,ev.GetRow()));
	_buf[3] = '\0';
   //Set the active layer for this row
   GLOB->CMDH->DoCommand("setlayer %d",atoi(_buf));
   ev.Skip();
}

void InsLayer::OnLabelLeftClick(wxGridEvent& ev )
{
   if (ev.GetRow()< 0)
   {
      switch (ev.GetCol())
      { case 0:
            break;
        case 1:
        {
            GLOB->CMDH->DoCommand("visible 1 all");
            GLOB->CMDH->DoCommand("draw");
            // set the items in the right order in the listbox
            for (int i = 0; i < MAX_LAYERS; i++)
            {
               for (int x = 0; x < MAX_LAYERS; x++)
               {
                  if (GLOB->SETUP->Get_Order(x) == i)
                  {
                     if (GLOB->SETUP->Get_Visible(x))
                        m_grid->SetCellValue(i,1,"1");
                     else
                        m_grid->SetCellValue(i,1,"");
                  }
               }
            }
            break;
        }
        case 2:
        {
            GLOB->CMDH->DoCommand("selectable 1 all");
            // set the items in the right order in the listbox
            for (int i = 0; i < MAX_LAYERS; i++)
            {
               for (int x = 0; x < MAX_LAYERS; x++)
               {
                  if (GLOB->SETUP->Get_Order(x) == i)
                  {
                     if (GLOB->SETUP->Get_Selectable(x))
                        m_grid->SetCellValue(i,2,"1");
                     else
                        m_grid->SetCellValue(i,2,"");
                  }
               }
            }
            break;
        }
        default:
            break;
      }
   }
   else
   {
      char _buf[100];
      strcpy(_buf,m_grid->GetLabelValue(wxVERTICAL,ev.GetRow()));
      _buf[3] = '\0';
      GLOB->CMDH->DoCommand("setlayer %d",atoi(_buf));
   }
   ev.Skip();
}

void InsLayer::Update(wxUpdateUIEvent&)
{
   if (IsShown())
   {
      bool bad=false;
   	for (int i = 0; i < MAX_LAYERS; i++)
      {
         char _buf[100];
         strcpy(_buf,m_grid->GetLabelValue(wxVERTICAL,i));
         _buf[3] = '\0';
         int lay=atoi(_buf);

         if (GLOB->SETUP->Get_Order(lay) != i ||
             GLOB->SETUP->Get_Visible(lay) != (m_grid->GetCellValue(i,1)=="1") ||
             GLOB->SETUP->Get_Selectable(lay) != (m_grid->GetCellValue(i,2)=="1") ||
             GLOB->SETUP->Get_Available(lay) != (m_grid->GetCellBackgroundColour(i,0)==*wxRED) ||
 			    (strcmp(GLOB->SETUP->Get_Name(lay),m_grid->GetCellValue(i,0)))
            )
         { bad=true;
         }
      }
      //if (bad)
         //SetupWindow();
   }
}

/*
void InsLayer::OnCellLeftClick(wxGridEvent& ev )
{
	char* _buf = new char[100];
	strcpy(_buf,m_grid->GetLabelValue(wxVERTICAL,ev.GetRow()));
	_buf[3] = '\0';
   GLOB->CMDH->DoCommand("setlayer %d",atoi(_buf));

   switch (ev.GetCol())
   { case 0:
         break;
     case 1:
     {
         if (m_grid->GetCellValue(ev.GetRow(),ev.GetCol())=="1")
         {
            GLOB->CMDH->DoCommand("visible 0");
            m_grid->SetCellValue(ev.GetRow(),1,"");
         }
         else
         {
            GLOB->CMDH->DoCommand("visible 1");
            m_grid->SetCellValue(ev.GetRow(),1,"1");
         }
         break;
     }
     case 2:
         if (m_grid->GetCellValue(ev.GetRow(),ev.GetCol())=="1")
         {
            GLOB->CMDH->DoCommand("selectable 0");
            m_grid->SetCellValue(ev.GetRow(),1,"");
         }
         else
         {
            GLOB->CMDH->DoCommand("selectable 1");
            m_grid->SetCellValue(ev.GetRow(),1,"1");
         }
         break;
     default:
         break;
   }

   ev.Skip();
}
*/
/*
void InsLayer::OnCellValueChanged(wxGridEvent& ev )
{
	char _buf[100];
	strcpy(_buf,m_grid->GetLabelValue(wxVERTICAL,ev.GetRow()));
	_buf[3] = '\0';
   //Set the active layer for this row
   GLOB->CMDH->DoCommand("setlayer %d",atoi(_buf));
   ev.Skip();
}
*/

