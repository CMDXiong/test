/*
Program			CHANDBAS.CPP
Purpose			BaseCommandHandler between userinterface and commands (header).
					This file is the buffer between every command generator
					 and the internal structures. It's platform independed.
					All calls to CHandler are done to
					 CommandHandler (or Dos_CommandHandler)
					CHandler is used by the ProcessParser.
*/

#ifdef __GNUG__
#pragma implementation
#endif
#include <iostream>
#include <fstream>
#include <wx/string.h>

#include "chandbas.h"
#include "command.h"
#include "units.h"

#include "bbox.h"
#include "gdsii.h"
#include "elemlst.h"
#include "gdsdrin.h"
#include "gdsdrout.h"
#include "keydrin.h"
#include "keydrout.h"
#include "toolcont.h"
#include "gdsapp.h"
#include "ddehand.h"
#include "scrndrv.h"
#include "distdlg.h"
#include "exlout.h"
#include "cnchier.h"
#include "gbrhier.h"
#include "flashin.h"
#include "flashout.h"
#include "emaskin.h"
#include "emaskout.h"
#include "group.h"
#include "prcspars.h"
#include "gdsapp.h"
#include	"mainwin.h"
#include "execdlg.h"
#include "dcclbuf.h"
#include "proper.h"
#include "wxsvg.h"
#include "structur.h"

using namespace std;

CommandHandler::CommandHandler()
{
	m_spline=0;
	_pathtype=0;

	m_viewer = false;

	// make group arrays
	_group_A = new group;
	_group_B = new group;
	_destination = 0;
   m_destination_add=false;
	_filename.Clear();

	// assign gdsii and the mainwindow
	_gdsii = 0;

	_processing_file = 0;

	_file_history = new DL_List<Alias*>();

	// boolean variables for checked menu's
	_view_coord = true;
	_view_outline = false;
	_view_debug = false;
   _selected_only = true;
   _visible_only = false;
   _savesetuponexit=false;
   _width="0um";
   _mirror_x=false;
   _mirror_y=false;
   _move_hor="0";
   _move_ver="0";
   _scale=1;
   _rotate=0;
   _direction=true;
	_maxlinemerge="1";
	_selectmarge=0.01;
	_snapfactor="1";
	_correctionfactor="1";
	_correctionaber="1";
	_roundfactor=1;
	_roundtype="1";
	_poly2arcrmin="1";
	_poly2arcrmax="1";
	_poly2arcaber="1";
	_arc2polyaber="1";
	_smoothaber="1";
	_displayaber="1";
	_structhres=10;
	_primthres=1;
	_drawsmallprim=true;
   _default_width="1000 um";
   _default_height="1500 um";
   _default_acc=0.01;
   _default_unit="1um";

   strcpy(_str_grid_colour,"0 0 0");
   strcpy(_str_back_colour,"0 0 0");

   m_grid_space="10.0 mm";

	// make a new setup with default settings
	_setup = new Setup();
   //announce to GLOB
   GLOB->SETUP=_setup;
	GLOB->SETUP->Set_UpdateAvailableLayers(true);
	_CurrentLayer=0;

   _grid_colour = 0;
   _back_colour = 0;
   m_stepmode=false;
   m_continue=true;

   _commandhistory = new DL_List<Command*>();
	// make the zoomlist
	_zoom_history = new DL_List<void*>();
   _start_point_x = 0;
   _start_point_y = 0;
	_ddehandler 	= new DDEHandler(this);

	int i = 0;

	// Draw commands
//   _handler[i]   	= new Handler("none",&CommandHandler::Select);	//not used
	_handler[i++]  = new Handler("select",&CommandHandler::Select);
	_handler[i++]  = new Handler("endcommand",&CommandHandler::EndCommand);
   _handler[i++]  = new Handler("undo",&CommandHandler::UndoCommand);
   _handler[i++]  = new Handler("tool",&CommandHandler::Tool);
   _handler[i++]  = new Handler("deselect",&CommandHandler::DeSelect);
   _handler[i++]  = new Handler("begin",&CommandHandler::Begin);
   _handler[i++]  = new Handler("end",&CommandHandler::End);
   _handler[i++]  = new Handler("setcursor",&CommandHandler::SetCursor);
   _handler[i++]  = new Handler("addpoint",&CommandHandler::AddPoint);

	// File commands
   _handler[i++]  = new Handler("newfile",&CommandHandler::NewFile);
   _handler[i++]  = new Handler("loadfile",&CommandHandler::LoadFile);
   _handler[i++]  = new Handler("savefile",&CommandHandler::SaveFile);
   _handler[i++]  = new Handler("closefile",&CommandHandler::CloseFile);
   _handler[i++]  = new Handler("mergefile",&CommandHandler::MergeFile);

	// Display commands
   _handler[i++]  = new Handler("zoom",&CommandHandler::Zoom);
   _handler[i++]  = new Handler("zoomback",&CommandHandler::ZoomBack);
   _handler[i++]  = new Handler("zoomout",&CommandHandler::ZoomOut);
   _handler[i++]  = new Handler("zoomout2",&CommandHandler::ZoomOut2);
   _handler[i++]  = new Handler("scrollscreen",&CommandHandler::ScrollScreen);
   _handler[i++]  = new Handler("clearscreen",&CommandHandler::ClearScreen);
   _handler[i++]  = new Handler("draw",&CommandHandler::Draw);
   _handler[i++]  = new Handler("print",&CommandHandler::PrintFile);
   _handler[i++]  = new Handler("viewstructure",&CommandHandler::ViewStructure);
   _handler[i++]  = new Handler("drawoutline",&CommandHandler::DrawOutline);
   _handler[i++]  = new Handler("startpoint",&CommandHandler::SetStartPoint);

	// Operations
   _handler[i++]  = new Handler("makestructure",&CommandHandler::MakeStructure);
   _handler[i++]  = new Handler("delete",&CommandHandler::Delete);
   _handler[i++]  = new Handler("move",&CommandHandler::Move);
   _handler[i++]  = new Handler("resettrans",&CommandHandler::ResetTrans);
   _handler[i++]  = new Handler("flatten",&CommandHandler::Flatten);
   _handler[i++]  = new Handler("keystring",&CommandHandler::KeyString);

   // Settings
   _handler[i++]  = new Handler("selected",&CommandHandler::SetSelectedOnly);
   _handler[i++]  = new Handler("savevisible",&CommandHandler::SetVisibleOnly);
   _handler[i++]  = new Handler("savesetuponexit",&CommandHandler::SetSaveSetUpOnExit);
   _handler[i++]  = new Handler("showgrid",&CommandHandler::ShowGrid);
   _handler[i++]  = new Handler("relativepoint",&CommandHandler::SetRelPoint);
   _handler[i++]  = new Handler("help",&CommandHandler::ShowBrowser);

	// Main commands
   _handler[i++]  = new Handler("exit",&CommandHandler::Exit);
   _handler[i++]  = new Handler("quit",&CommandHandler::Exit);
   _handler[i++]  = new Handler("record",&CommandHandler::Record);

   // Additional commands
   _handler[i++]  = new Handler("pause",&CommandHandler::Pause);

   // File commands
   _handler[i++]  = new Handler("loadsetup",&CommandHandler::LoadSetup);
   _handler[i++]  = new Handler("savesetup",&CommandHandler::SaveSetup);
   _handler[i++]  = new Handler("loadprocess",&CommandHandler::Load_Process_File);
   _handler[i++]  = new Handler("stepmode",&CommandHandler::StepMode);
   _handler[i++]  = new Handler("askfile",&CommandHandler::AskFile);
   _handler[i++]  = new Handler("setlogfile",&CommandHandler::SetLogFile);
   _handler[i++]  = new Handler("ask",&CommandHandler::Ask);
   _handler[i++]  = new Handler("viewer",&CommandHandler::SetViewer);

   // Define Groups
   _handler[i++]  = new Handler("group_a",&CommandHandler::Define_A);
   _handler[i++]  = new Handler("group_b",&CommandHandler::Define_B);
   _handler[i++]  = new Handler("destination",&CommandHandler::DefineDestination);

   // Operations
   _handler[i++]  = new Handler("addproperty",&CommandHandler::AddProperty);
   _handler[i++]  = new Handler("addproperty_a",&CommandHandler::Operation_addproperty_a);
   _handler[i++]  = new Handler("transform",&CommandHandler::Operation_transform);
   _handler[i++]  = new Handler("undotrans",&CommandHandler::Operation_undotrans);
   _handler[i++]  = new Handler("move_a",&CommandHandler::Operation_move_a);
   _handler[i++]  = new Handler("copy_a",&CommandHandler::Operation_copy_a);
   _handler[i++]  = new Handler("delete_a",&CommandHandler::Operation_delete_a);
   _handler[i++]  = new Handler("transform_a",&CommandHandler::Operation_transform_a);
   _handler[i++]  = new Handler("undotrans_a",&CommandHandler::Operation_undotrans_a);
   _handler[i++]  = new Handler("calcarea_a",&CommandHandler::Operation_calcarea_a);
   _handler[i++]  = new Handler("makering",&CommandHandler::Operation_makering);
   _handler[i++]  = new Handler("correction",&CommandHandler::Operation_correction);
   _handler[i++]  = new Handler("toarcs",&CommandHandler::Operation_toarcs);
   _handler[i++]  = new Handler("tolines",&CommandHandler::Operation_tolines);
   _handler[i++]  = new Handler("smoothen",&CommandHandler::Operation_smoothen);
   _handler[i++]  = new Handler("intersect",&CommandHandler::Operation_intersect);
   _handler[i++]  = new Handler("merge",&CommandHandler::Operation_merge);
   _handler[i++]  = new Handler("a_subtract_b",&CommandHandler::Operation_a_subtract_b);
   _handler[i++]  = new Handler("b_subtract_a",&CommandHandler::Operation_b_subtract_a);
   _handler[i++]  = new Handler("exor",&CommandHandler::Operation_exor);

   // Transformation settings
	_handler[i++]  = new Handler("mirrorx",&CommandHandler::SetMirrorX);
   _handler[i++]  = new Handler("mirrory",&CommandHandler::SetMirrorY);
   _handler[i++]  = new Handler("movehor",&CommandHandler::SetMoveHor);
   _handler[i++]  = new Handler("movever",&CommandHandler::SetMoveVer);
   _handler[i++]  = new Handler("scale",&CommandHandler::SetScale);
   _handler[i++]  = new Handler("rotate",&CommandHandler::SetRotate);
   _handler[i++]  = new Handler("direction",&CommandHandler::SetDirection);

   //	Factors' settings
   _handler[i++]  = new Handler("selectmarge",&CommandHandler::SetSelectmarge);
   _handler[i++]  = new Handler("snapfactor",&CommandHandler::SetSnapfactor);
   _handler[i++]  = new Handler("correctionfactor",&CommandHandler::SetCorrectionfactor);
	_handler[i++]  = new Handler("roundfactor",&CommandHandler::SetRoundfactor);
   _handler[i++]  = new Handler("correctionaber",&CommandHandler::SetCorrectionaber);
   _handler[i++]  = new Handler("roundtype",&CommandHandler::SetRoundtype);
   _handler[i++]  = new Handler("toarcrmin",&CommandHandler::SetPoly2ArcRmin);
   _handler[i++]  = new Handler("toarcrmax",&CommandHandler::SetPoly2ArcRmax);
   _handler[i++]  = new Handler("arcaccur",&CommandHandler::SetPoly2ArcAber);
   _handler[i++]  = new Handler("polyaccur",&CommandHandler::SetArc2PolyAber);
   _handler[i++]  = new Handler("smoothaccur",&CommandHandler::SetSmoothAber);
   _handler[i++]  = new Handler("displayaber",&CommandHandler::SetDisplayaber);
	_handler[i++]  = new Handler("structhres",&CommandHandler::SetStrucThres);
   _handler[i++]  = new Handler("primthres",&CommandHandler::SetPrimThres);
   _handler[i++]  = new Handler("maxlinemerge",&CommandHandler::SetMaxlinemerge);
   _handler[i++]  = new Handler("defaultaccur",&CommandHandler::SetDefaultAcc);
   _handler[i++]  = new Handler("defaultunit",&CommandHandler::SetDefaultUnit);
   _handler[i++]  = new Handler("setwinding",&CommandHandler::SetWindingRule);

   // next commands are for changing the setup for certain layers
   _handler[i++]  = new Handler("setlayer",&CommandHandler::SetCurrentLayer);
   _handler[i++]  = new Handler("setlayerbyname",&CommandHandler::SetCurrentLayerByName);
   _handler[i++]  = new Handler("name",&CommandHandler::SetLayerName);
   _handler[i++]  = new Handler("gdsinmap",&CommandHandler::SetGdsInMap);
   _handler[i++]  = new Handler("gdsoutmap",&CommandHandler::SetGdsOutMap);
   _handler[i++]  = new Handler("visible",&CommandHandler::SetLayerVisible);
   _handler[i++]  = new Handler("selectable",&CommandHandler::SetLayerSelectable);
   _handler[i++]  = new Handler("bordercolor",&CommandHandler::SetLayerBorderColor);
   _handler[i++]  = new Handler("fillcolor",&CommandHandler::SetLayerFillColor);
   _handler[i++]  = new Handler("brush",&CommandHandler::SetLayerBrush);
   _handler[i++]  = new Handler("order",&CommandHandler::SetLayerOrder);
   _handler[i++]  = new Handler("readlayer",&CommandHandler::SetLayerRead);

   // Display settings
   _handler[i++]  = new Handler("showcoords",&CommandHandler::ShowCoords);
   _handler[i++]  = new Handler("addfile",&CommandHandler::AddMenuFile);

   // Other settings
   _handler[i++]  = new Handler("gridspace",&CommandHandler::SetGridSpace);
   _handler[i++]  = new Handler("gridcolour",&CommandHandler::SetGridColour);
   _handler[i++]  = new Handler("backgroundcolour",&CommandHandler::SetBackColour);
   _handler[i++]  = new Handler("gridplacement",&CommandHandler::SetGridPlacement);

   // Help / Show commands
   _handler[i++]  = new Handler("showdistdlg",&CommandHandler::ShowDistDlg);
   _handler[i++]  = new Handler("showidentdlg",&CommandHandler::ShowIdentDlg);
   _handler[i++]  = new Handler("showcursordlg",&CommandHandler::ShowCursorDlg);
   _handler[i++]  = new Handler("showfactorsdlg",&CommandHandler::ShowFactors);
   _handler[i++]  = new Handler("showorderdlg",&CommandHandler::ShowOrder);
   _handler[i++]  = new Handler("pathtypedlg",&CommandHandler::ShowPathtypeDlg);
   _handler[i++]  = new Handler("inslayerdlg",&CommandHandler::ShowInsertLayerDlg);
   _handler[i++]  = new Handler("showgroupsdlg",&CommandHandler::ShowGroups);
   _handler[i++]  = new Handler("showalias",&CommandHandler::ShowAlias);
   _handler[i++]  = new Handler("showenv",&CommandHandler::ShowEnv);
	_handler[i++]  = new Handler("showlayersdlg",&CommandHandler::ShowLayers);
   _handler[i++]  = new Handler("showfilename",&CommandHandler::ShowFilename);
   _handler[i++]  = new Handler("showtopstructure",&CommandHandler::ShowTopStructure);
   _handler[i++]  = new Handler("showkeybnf",&CommandHandler::ShowKeyBNF);
   _handler[i++]  = new Handler("showprocessbnf",&CommandHandler::ShowProcessBNF);
   _handler[i++]  = new Handler("showcommandbnf",&CommandHandler::ShowProcessBNF);
   _handler[i++]  = new Handler("showexecdlg",&CommandHandler::ShowExecDlg);
   _handler[i++]  = new Handler("setcontinue",&CommandHandler::SetContinueFlag);
   _handler[i++]  = new Handler("setalias",&CommandHandler::SetAlias);
   _handler[i++]  = new Handler("setenv",&CommandHandler::SetEnv);
   _handler[i++]  = new Handler("replacestruc",&CommandHandler::ReplaceStruct);
   _handler[i++]  = new Handler("setpathtype",&CommandHandler::SetPathtype);
   _handler[i++]  = new Handler("setwidth",&CommandHandler::SetWidth);
   _handler[i++]  = new Handler("setspline",&CommandHandler::SetSpline);

   maxpointer = i;
   _keyword=NULL; //Solve memory leak
}

// frees all the dynamic data
CommandHandler::~CommandHandler()
{
	if (_gdsii)
		delete _gdsii;

	// delete the groups
	delete _group_A;
	delete _group_B;
   {
   	DL_Iter<Command*> _hi(_commandhistory);
      _hi.tohead();
      while (!_hi.hitroot())
      {
          delete _hi.item();
          _hi.remove();
      }
   }
   delete _commandhistory;

	// close the processfile if there was a command recording
	if (Get_Record())
	{
		_processing_file->flush();
		delete _processing_file;
		_processing_file = 0;
	}

	// delete the setup
	delete _setup;
   _setup=0;
   GLOB->SETUP=0;

  	// delete the zoomlist
	EmptyZoomHistory();
	delete _zoom_history;
	delete _ddehandler;
   for (int i=0;i<maxpointer;i++)
       delete _handler[i];

	{
		DL_Iter<Alias*> I(_file_history);
      I.tohead();
      while (!I.hitroot())
      {
          delete I.item();
          I.remove();
      }
	}
	delete _file_history;
	delete _keyword; //Solve memory leak;

}

bool CommandHandler::ToHistory(char* Format,...)
{
   Command* _current_command;

	va_list ap;

   wxString buffer;
	va_start(ap,Format);
	buffer.PrintfV(Format,ap);
	va_end(ap);

   // write commands to processing file if record is ON
   if ( Get_Record() )
   {
      *_processing_file << buffer << endl;
   }

   _current_command = new Command();
   _current_command->Set_Command((char*)buffer.c_str());

   wxString keyword;
   if (buffer.Find(' ')!=-1)     //get the keyword only
   	keyword=buffer.SubString(0,buffer.Find(' '));

   _current_command->Set_Keyword(keyword);

	if (GLOB->INITIALIZED) {
   	GLOB->MAINWINDOW->_execDialog->addCommand(_current_command->Get_Command());
   }
   _commandhistory->insend(_current_command);
   return true;
}

bool CommandHandler::ToHistory(Command* a_command)           
{
   // write commands to processing file if record is ON
   if ( Get_Record() )
   {
      *_processing_file << a_command->Get_Command() << endl;
   }

	if (GLOB->INITIALIZED) {
   	GLOB->MAINWINDOW->_execDialog->addCommand(a_command->Get_Command());
   }

   _commandhistory->insend(a_command);
   return true;
}


bool CommandHandler::Replace_Setup(Setup* new_setup)
{
  //just copy the contents of the new one
  *_setup=*new_setup;
  return true;
}

bool CommandHandler::DoCommand(char* Format,...)
{
   bool retval=false;

   //to evaluate the commandstring
   // because recursive call must be possible it nust be on the stack

   Command* _current_command;

   // _the_command is used global to exess all items for the current command
   // be aware of recursive calls
   _current_command= new Command();

	va_list ap;

   wxString buffer;
	va_start(ap,Format);
	buffer.PrintfV(Format,ap);
	va_end(ap);

   _current_command->Set_Command((char*)buffer.c_str());

   //now evaluate the commands
   wxString commandstring = buffer + '\0';

   Evalstring* _commandstring = new Evalstring(commandstring, GLOB->ALIASLIST);
 	DL_Iter<const wxString*>* 	_Ibackup=_I; //allows recursive calls
   wxString tmp;
   _commandstring->Parse(tmp);
   _I=_commandstring->GetArgIter();

	if (!_I->count())
      return true;  //no commands but that is oke

   _I->tohead();
   if (_keyword)
	   delete _keyword;
   _keyword=_I->item();
   _I->removehead();
   _current_command->Set_Keyword(*_keyword);

   try
   {
      int ret_val=ExecuteCommand();

      if (ret_val==1)
      {
         // write commands to processing file if record is ON
         if ( Get_Record() )
         {
            *_processing_file << _current_command->Get_Command().c_str() << endl;
         }

         if (GLOB->INITIALIZED) {
            GLOB->MAINWINDOW->_execDialog->addCommand(_current_command->Get_Command());
         }
         _commandhistory->insend(_current_command);
         retval = true;   //oke
      }
      else if (ret_val==2)
      {
         char errbuf[LINELENGTH];
         sprintf(errbuf,"Could not Parse command: \n %s \n",_current_command->Get_Command().c_str());
         _messagehandler->info(errbuf, "Command error");
         retval = false;  //bad command
      }
      else // unknown command
      {
         char buf[LINELENGTH];
         sprintf(buf," Unknown Command: %s",_keyword->c_str());
         _messagehandler->info(buf, "Command error");
         retval = false;  //bad command
      }
   }
	catch (GDS_Error& _error)
   {
      char errbuf[LINELENGTH];
      sprintf(errbuf,"error executing command \n %s \n",_current_command->Get_Command().c_str());
      _messagehandler->info(errbuf, "Command error");
      retval = false;  //bad command
   }

   delete _commandstring;
 	_I=_Ibackup; //allows recursive calls
	return retval;
}


int CommandHandler::ExecuteCommand()
{
   if (_keyword->GetChar(0)!='#')
   {
      for(int i=0; i < maxpointer; i++)
      {
         if (!_handler[i]->keyword.CmpNoCase(*_keyword))
         {
            if ((this->*(_handler[i]->fpointer))())
               return 1;   //good command
            return 2; //command is bad
         }
      }
      return 0;  //command does not exist
   }
   else
      return 1;   //good command but comment
}

// Exit the application
// calls the Window Managers Exit function
bool CommandHandler::Exit()
{
	_statusbar->SetProcess("Quitting...");
   GLOB->MAINWINDOW->Exit();
	return true;
}


// Only available in Mainwindow:
// All action by the user are stored in a file (recorded)
// This is an easy way to make processfiles
bool CommandHandler::Record()
{
	// if a filename is given -> start processing. Else stop it
	if (_I->count() && _I->item()->Len() > 0)
	{
		// open a file and set the statusbar on recording
		_statusbar->SetRecording(true);
		_processing_file = new ofstream(_I->item()->c_str());

		*_processing_file << "# Process file for GDS-II postprocessing tool" << endl << endl;
	}
	else
	{
		// recording is done. Restore the statusbar
		// and flush the file so everything is saved!
		_statusbar->SetRecording(false);
		_processing_file->flush();
		delete _processing_file;
		_processing_file = 0;
	}
   return true;
}


// delete the gdsii structure, the zoomhistory and the groups
bool CommandHandler::CloseFile()
{
   if (_I->count()) return false;
	_statusbar->SetProcess("Closing file...");

	if (_gdsii)
	{
		// redraw the screen to make it empty
		ClearScreen();
		delete _gdsii;
	}
	_gdsii = 0;

	_filename.Clear();

	GLOB->MAINWINDOW->GDS_loaded(false);
	// delete the items in the zoom history
	EmptyZoomHistory();
	return true;
}

bool CommandHandler::NewFile()
{
   double accur;
   double width;
   double height;

   if (_I->count() == 0)
   {
		accur=_default_acc;
      width=_default_width.GetValue()/GLOB->PHYSUNITS();
      height=_default_height.GetValue()/GLOB->PHYSUNITS();

   }
   else if (_I->count() == 4)
   {
      _I->item()->ToDouble(&width);
      (*_I)++;
      _I->item()->ToDouble(&height);
      (*_I)++;
      _I->item()->ToDouble(&accur);
      //use this when writing the file out
		_default_acc=accur;
      (*_I)++;
   }
   else
   	return false;


   // pass command to the command handler
   try
   {
      _statusbar->SetProcess("Create New File");

      if (!_gdsii)
         delete _gdsii;
      _gdsii = new GdsII();
      if (!_gdsii)
         return false;

      Structure* _structure = new Structure("top");
      _gdsii->insend(_structure);
      _gdsii->SetTopStructure(_structure);
      _gdsii->SetShowStructure(_structure);
      _gdsii->SetUserUnits(accur);
      _gdsii->SetPhysicalUnits(GLOB->ENTRYUNITS());
      GLOB->USERUNITS(_gdsii->GetUserUnits());
      GLOB->PHYSUNITS(_gdsii->GetPhysicalUnits());
      _gdsii->CalcBoundingBox(); //it is simply zero so
      _gdsii->GetBoundingBox().SetMax(width,height);
     	_filename="newfile.cal";
      _statusbar->SetFile(_filename);
      _statusbar->SetProcess();
		GLOB->SETUP->Set_UpdateAvailableLayers(true);
   }
   catch (GDS_Error& _error) // error occured
   {
      _statusbar->SetProcess();
      // catch the error from the parser and throw it
      _messagehandler->info(_error.GetErrorMessage(), "Error Creating newfile");
      return false;
   }

   GLOB->MAINWINDOW->GDS_loaded(true);

   // get the boundingbox from the gdsii file
   BoundingBox bbox;
   bbox = _gdsii->GetBoundingBox();

   GLOB->SCRDRV->SetMappingScroll(  bbox.GetMin().GetX(),
                              bbox.GetMin().GetY(),
                              bbox.GetMax().GetX(),
                              bbox.GetMax().GetY(),
                              true);

   // get the scalingfactor from the whole GDS-II structure
   _scale = GLOB->SCRDRV->GetScaleFactorX();

	Draw();
   return true;
}

// load the file "filename"
bool CommandHandler::LoadFile()
{
	wxString type;
	wxString file;

   if (_I->count() != 2) return false;
	if (_I->item()->Len()==0) return false;
   file=*_I->item();
   (*_I)++;
	if (_I->item()->Len()==0) return false;
   type=*_I->item();

   bool succeeded=false;

	if (!strcmp(type,"gds-ii") || !strcmp(type,"key") ||
   	 !strcmp(type,"flash")  || !strcmp(type,"emask")
      )
   {  SetFileType(type);
      // if a file already opened: close it first
      if (_gdsii)
      {
         CloseFile();
         GLOB->MAINWINDOW->GDS_loaded(false);
      }
      // allocate memory for GDSII file
      GdsII* h_gdsii = new GdsII();
      if (!h_gdsii)
         throw GDS_Error("Can't allocate memory for the file",
            "CommandHandler(), LoadFile(), Memory Allocation Error", 9,0);

      // make all the layers not available by default
      for (int i = 0; i < MAX_LAYERS; i++)
         _setup->Set_Available(i, false);

      _name_backup=file;
      if (!GetFileType(file,h_gdsii,type))
      {
         delete h_gdsii;
         _gdsii = 0;
			succeeded = false;
      }
      else
      {
         _gdsii=h_gdsii;
         GLOB->USERUNITS(_gdsii->GetUserUnits());
         GLOB->PHYSUNITS(_gdsii->GetPhysicalUnits());

         _statusbar->SetFile(file);
         _statusbar->SetProcess();
         GLOB->MAINWINDOW->GDS_loaded(true);
	 	 succeeded = true;
      }
    }
	else
	{
		char errbuf[LINELENGTH];
		sprintf(errbuf," Loadfile( %s ): could not determine type. (gds_ii or key emask or flash)\n"
					"  Syntax: loadfile <path><filename> <filetype>", file.c_str());
		_messagehandler->info( errbuf, "Loadfile error" );
      SetFileType("");
	}
   if (succeeded)
   {
		GLOB->SETUP->Set_UpdateAvailableLayers(true);
		// get the boundingbox from the gdsii file
		BoundingBox bbox;
		bbox = _gdsii->GetBoundingBox();

      GLOB->SCRDRV->SetMappingScroll(  bbox.GetMin().GetX(),
                                 bbox.GetMin().GetY(),
                                 bbox.GetMax().GetX(),
                                 bbox.GetMax().GetY(),
                                 true);

      GLOB->SCRDRV->SetScroll(bbox.GetMin().GetX(),
                              bbox.GetMin().GetY(),
										bbox.GetMax().GetX(),
										bbox.GetMax().GetY()
                             );


		// get the scalingfactor from the whole GDS-II structure
		_scale = GLOB->SCRDRV->GetScaleFactorX();

		return true;
	}
	else
   {
		Draw();
   }
   return succeeded;
}

// merge the file "filename"
bool CommandHandler::MergeFile()
{
	wxString type;
	wxString file;

   wxUnit xx;
   wxUnit yy;
   double x=0;
   double y=0;
   double angle=0;


   if (_I->count() == 2)
   {
      if (_I->item()->Len()==0) return false;
      file=*_I->item();
      (*_I)++;
      if (_I->item()->Len()==0) return false;
      type=*_I->item();
   }
   else if (_I->count() == 5)
   {
      if (_I->item()->Len()==0) return false;
      file=*_I->item();
      (*_I)++;
      if (_I->item()->Len()==0) return false;
      type=*_I->item();
      (*_I)++;
      xx.Eval(*_I->item());
 	   x =xx.GetValue();
      (*_I)++;
      yy.Eval(*_I->item());
 	   y =yy.GetValue();
      (*_I)++;
      _I->item()->ToDouble(&angle);
   }

   //check if right files
	if (strcmp(type,"gds-ii") && strcmp(type,"key")&& strcmp(type,"flash")&& strcmp(type,"emask"))
	{
		char errbuf[LINELENGTH];
		sprintf(errbuf," Mergefile( %s ): could not determine type. (GDS-II or KEY or flash or emask)\n"
					"  Syntax: mergefile <path><filename>,<filetype>", file.c_str());
		_messagehandler->info( errbuf, "Mergefile error" );
		return false;
	}

   // allocate memory for GDSII file
   GdsII* _gdsii_merge = new GdsII();
   if (!_gdsii_merge)
      throw GDS_Error("Can't allocate memory for the file",
         "CommandHandler(), MergeFile(), Memory Allocation Error", 9,0);

   // first backup the units from the current gdsii file
   double _old_userunits = GLOB->USERUNITS();
   double _old_physunits = GLOB->PHYSUNITS();


   if (!GetFileType(file,_gdsii_merge,type))
   {
      delete _gdsii_merge;
      _gdsii_merge = 0;
      SetFileEnv(_name_backup); // The current file is still the old file...
      return false;
   }

   SetFileEnv(_name_backup); // The current file is still the old file...

   // Ok, now we have a gdsii file, but the units can be different from the one
   // already in memory. So we'll have to multiply the units from one of the
   // gdsii objects.

   if (_old_physunits < _gdsii_merge->GetPhysicalUnits())
   {
      double factor = _gdsii_merge->GetPhysicalUnits()/_old_physunits;

      _gdsii->ClearFlags(ELE_ALL  ^ ELE_FILLED);
      _gdsii_merge->ResetUnits(factor);

      _gdsii->SetPhysicalUnits(_old_physunits);
   }
   else if (_gdsii_merge->GetPhysicalUnits() < _old_physunits)
   {
      double factor = _old_physunits/_gdsii_merge->GetPhysicalUnits();
      _gdsii->ClearFlags(ELE_ALL  ^ ELE_FILLED);
      _gdsii->ResetUnits(factor);
      _gdsii->SetPhysicalUnits(_gdsii_merge->GetPhysicalUnits());
   }

   _statusbar->SetProcess("Merging file...");
	x = x / _gdsii->GetPhysicalUnits();
	y = y / _gdsii->GetPhysicalUnits();
   GLOB->USERUNITS(_gdsii->GetUserUnits());
   GLOB->PHYSUNITS(_gdsii->GetPhysicalUnits());
   _gdsii->Merge(_gdsii_merge,x,y,angle);
   delete _gdsii_merge;
   _statusbar->SetProcess();

   //the userunits will be kept according to the old userunits.
   _gdsii->SetUserUnits(_old_userunits);
	GLOB->SETUP->Set_UpdateAvailableLayers(true);

   // ... then (, if true,) handle screen stuff

   // get the boundingbox from the gdsii file
   BoundingBox bbox;
   bbox = _gdsii->GetBoundingBox();

   // screendriver must contain this function!
   GLOB->SCRDRV->SetMappingScroll(
                              bbox.GetMin().GetX(),
                              bbox.GetMin().GetY(),
                              bbox.GetMax().GetX(),
                              bbox.GetMax().GetY(),
                              true);

   // get the scalingfactor from the whole GDS-II structure
   _scale = GLOB->SCRDRV->GetScaleFactorX();

//	Draw();
   return true;
}

// merge the file "filename"
bool CommandHandler::ReplaceStruct()
{
	char type[LINELENGTH];
	char file[LINELENGTH];
	char strucname[LINELENGTH];


   if (_I->count() != 3) return false;
	if (_I->item()->Len()==0) return false;
   strcpy(file,_I->item()->c_str());
   (*_I)++;
	if (_I->item()->Len()==0) return false;
   strcpy(type,_I->item()->c_str());
   (*_I)++;
	if (_I->item()->Len()==0) return false;
   strcpy(strucname,_I->item()->c_str());

   //check if right files
	if (strcmp(type,"gds-ii") && strcmp(type,"key"))
	{
		char errbuf[LINELENGTH];
		sprintf(errbuf," Mergefile( %s ): could not determine type. (GDS-II or KEY)\n"
					"  Syntax: mergefile <path><filename>,<filetype>", file);
		_messagehandler->info( errbuf, "Mergefile error" );
		return false;
	}

   // allocate memory for GDSII file
   GdsII* _gdsii_merge = new GdsII();
   if (!_gdsii_merge)
      throw GDS_Error("Can't allocate memory for the GDS-II file",
         "Base CommandHandler(), MergeFileGDS(), Memory Allocation Error", 9,0);

   // fist backup the units from the current gdsii file
   double _old_userunits = GLOB->USERUNITS();
   double _old_physunits = GLOB->PHYSUNITS();


   if (!GetFileType(file,_gdsii_merge,type))
   {
      delete _gdsii_merge;
      _gdsii_merge = 0;
      SetFileEnv(_name_backup); // The current file is still the old file...
      return false;
   }

   SetFileEnv(_name_backup); // The current file is still the old file...

   // Ok, now we have a gdsii file, but the units can be different from the one
   // already in memory. So we'll have to multiply the units from one of the
   // gdsii objects.

   if (_old_physunits < _gdsii_merge->GetPhysicalUnits())
   {
      double factor = _gdsii_merge->GetPhysicalUnits()/_old_physunits;

      _gdsii->ClearFlags(ELE_ALL  ^ ELE_FILLED);
      _gdsii_merge->ResetUnits(factor);

      _gdsii->SetPhysicalUnits(_old_physunits);
      _gdsii->SetUserUnits(_old_userunits);
   }
   else if (_gdsii_merge->GetPhysicalUnits() < _old_physunits)
   {
      double factor = _old_physunits/_gdsii_merge->GetPhysicalUnits();
      _gdsii->ClearFlags(ELE_ALL ^ ELE_FILLED);
      _gdsii->ResetUnits(factor);
      _gdsii->SetPhysicalUnits(_gdsii_merge->GetPhysicalUnits());
      _gdsii->SetUserUnits(_gdsii_merge->GetUserUnits());
   }
/*
   if (USERUNITS < _old_userunits)
   {
      _gdsii->SetUserUnits(_old_userunits);
   }
*/
   _statusbar->SetProcess("Merging file...");
   GLOB->USERUNITS(_gdsii->GetUserUnits());
   GLOB->PHYSUNITS(_gdsii->GetPhysicalUnits());
   _gdsii->Replace(strucname,_gdsii_merge);
   delete _gdsii_merge;
   _statusbar->SetProcess();

	GLOB->SETUP->Set_UpdateAvailableLayers(true);
   return true;
}


bool CommandHandler::GetFileType(const wxString& filename, GdsII* gdsii,const wxString& type)
{
	if (filename.IsEmpty())
		ParameterError("CommandHandler(), GetFileType");

	if (wxFileExists(filename))
	{
		SetFileEnv(filename);

		// clear the statusbar
		_statusbar->Reset();
		_statusbar->SetProcess("Loading file of given type");

      if (!strcmp(type,"gds-ii"))
      {
         // make a GDSII read driver
         GDS_driver_in *_driver_in = new GDS_driver_in(filename);
         if (!_driver_in)
            throw GDS_Error("Can't allocate memory for the File driver",
               "CommandHandler(), GetFileType(), Memory Allocation Error", 9,0);
         try // to load the GDS-II file
         {
            _driver_in->Read(gdsii);
         }
         catch (GDS_Error& error)
         {
            _statusbar->SetFile();
            delete _driver_in;

            _messagehandler->info(error.GetErrorMessage(),"Could not read the file" );
            return false;
         }
         delete _driver_in;
      }
      else if (!strcmp(type,"key"))
      {
         // make a KEY read driver
         KEY_driver_in *_driver_in = new KEY_driver_in(filename);
         if (!_driver_in)
            throw GDS_Error("Can't allocate memory for the File driver",
               "CommandHandler(), GetFileType(), Memory Allocation Error", 9,0);
         try // to load the GDS-II file
         {
            _driver_in->Read(gdsii);
         }
         catch (GDS_Error& error)
         {
            _statusbar->SetFile();
            delete _driver_in;

            _messagehandler->info(error.GetErrorMessage(),"Could not read the file" );
            return false;
         }
         delete _driver_in;
      }
      else if (!strcmp(type,"flash"))
      {
         // make a KEY read driver
         FLASH_driver_in *_driver_in = new FLASH_driver_in(filename, false, _CurrentLayer);
         if (!_driver_in)
            throw GDS_Error("Can't allocate memory for the File driver",
               "CommandHandler(), GetFileType(), Memory Allocation Error", 9,0);
         try // to load the GDS-II file
         {
            _driver_in->Read(gdsii);
         }
         catch (GDS_Error& error)
         {
            _statusbar->SetFile();
            delete _driver_in;

            _messagehandler->info(error.GetErrorMessage(),"Could not read the file" );
            return false;
         }
         delete _driver_in;
      }
      else if (!strcmp(type,"emask"))
      {
         // make a KEY read driver
         EMASK_driver_in *_driver_in = new EMASK_driver_in(filename, false, _CurrentLayer);
         if (!_driver_in)
            throw GDS_Error("Can't allocate memory for the File driver",
               "CommandHandler(), GetFileType(), Memory Allocation Error", 9,0);
         try // to load the GDS-II file
         {
            _driver_in->Read(gdsii);
         }
         catch (GDS_Error& error)
         {
            _statusbar->SetFile();
            delete _driver_in;

            _messagehandler->info(error.GetErrorMessage(),"Could not read the file" );
            return false;
         }
         delete _driver_in;
      }
      else
         return false;

    	return true;
	}
	else
	{
		char errbuf[LINELENGTH];
		sprintf(errbuf," File does not exist!: %s", filename.c_str());
		_messagehandler->info( errbuf,"Base CommandHandler(), GetFileType(), File Error");
		return false;
	}
}

bool CommandHandler::StepMode()
{
   if (_I->count() != 1) return false;
	if (_I->item()->Len()==0) return false;

   if (!_I->item()->CmpNoCase("true"))
      m_stepmode=true;
   else
      m_stepmode=false;
   return true;
}

bool CommandHandler::Load_Process_File()
{
   if (_I->count() != 1) return false;
	if (_I->item()->Len() == 0)
		ParameterError("Base CommandHandler(), LoadProcessFile");

   char file[LINELENGTH];
   strcpy(file,_I->item()->c_str());
	if (wxFileExists(file))
		{
			ProcessParser*	_processparser;
         _statusbar->SetProcess("Loading process file...");
         SetFileType("prs");
         SetProcEnv(file);
         // make a parser for the process file and process it
         _processparser = new ProcessParser(file,GLOB->ALIASLIST);
         if (_processparser->LoadProcess()==false)
         {
            _statusbar->SetProcess();
            delete _processparser;
            return false;
         }
         delete _processparser;
			_statusbar->SetProcess();

			GLOB->SETUP->Set_UpdateAvailableLayers(true);
			return true;
		}
	else
	{
		char errbuf[LINELENGTH];
		sprintf(errbuf,"Process File %s doesn't exist! ", file);
		_messagehandler->info(errbuf, "Load_Process_File(): Error");
		return false;
	}
}


// save the file "filename"
bool CommandHandler::SaveFile()
{
   if (m_viewer)
   {
		_messagehandler->info( "only view mode can not save", "Savefile error" );
		return false;
   }

	const char* type;
	const char* file;

   if (_I->count() != 2) return false;
	if (_I->item()->Len()==0) return false;
   file=_I->item()->c_str();
   (*_I)++;
	if (_I->item()->Len()==0) return false;
   type=_I->item()->c_str();

   _name_backup=_filename;

   bool succeeded=false;
	if (!strcmp(type,"gds-ii"))
		succeeded = SaveFileGDS(file);
	else if (!strcmp(type,"key"))
		succeeded = SaveFileKEY(file);
	else if (!strcmp(type,"svg"))
		succeeded = SaveFileSVG(file);
	else if (!strcmp(type,"flash"))
		succeeded = SaveFileFLASH(file);
	else if (!strcmp(type,"emask"))
		succeeded = SaveFileEMASK(file);
	else if (!strcmp(type,"exelon"))
		succeeded = SaveFileExelon(file);
	else if (!strcmp(type,"gbr"))
		succeeded = SaveFileGerber(file);
	else if (!strcmp(type,"cnc"))
		succeeded = SaveFileCnc(file);
	else if (!strcmp(type,"png"))
		succeeded = SaveFilePNG(file);
	else if (!strcmp(type,"bmp"))
		succeeded = SaveFileBMP(file);
	else
	{
		char errbuf[LINELENGTH];
		sprintf(errbuf," Savefile( %s ): could not determine type. (gds-ii or key or flash gerber exelon)\n"
					"  Syntax: savefile <path><filename>,<filetype>", file );
		_messagehandler->info( errbuf, "Savefile error" );
	}
   if (	succeeded && strcmp(type,"gbr") && strcmp(type,"exelon")
    		&& strcmp(type,"cnc") && strcmp(type,"dmp") && strcmp(type,"dmp")
    		&& strcmp(type,"png") && strcmp(type,"svg") && strcmp(type,"bmp")
    	)
   	AddFileToHistory(file,type);

   return succeeded;
}

// write a GDS-II structure as ascii key file format
bool CommandHandler::SaveFileKEY(const wxString& filename)
{
	if (filename.IsEmpty())
		ParameterError("Base CommandHandler(), KEYfile");

	_statusbar->SetProcess("Writing KEY file...");
	if (!_gdsii)
   {
		_messagehandler->error("No GDSII structure opened yet.","error" );
      return false;
   }

	// Open a file for writing GDSII keyfile
	KEY_driver_out *_driver = new KEY_driver_out(filename, _visible_only);
	if (!_driver)
		throw GDS_Error("Can't allocate memory for the GDS-II keydriver",
			"Base CommandHandler(), SaveFileKEY(), Memory Allocation Error", 9, 0);

	// Write the structure to the file
	_driver->Write(_gdsii);

	// close the driver
	delete _driver;

   _filename=_name_backup;
	_statusbar->SetFile(_filename);
	_statusbar->SetProcess();
	return true;
}

// write a GDS-II structure as ascii key file format
bool CommandHandler::SaveFileSVG(const wxString& filename)
{
	if (filename.IsEmpty())
		ParameterError("Base CommandHandler(), KEYfile");

	_statusbar->SetProcess("Writing KEY file...");
	if (!_gdsii)
   {
		_messagehandler->error("No GDSII structure opened yet.","error" );
      return false;
   }

	DrawDriver* _draw_driver = new DrawDriver();
	_draw_driver->Init();

   // get the boundingbox from the gdsii file
   BoundingBox bbox;
   //*bbox = *_gdsii->GetBoundingBox();
   bbox =GLOB->SCRDRV->GetVirtualSizeScreen();

   _draw_driver->SetMappingScroll(
                              bbox.GetMin().GetX(),
                              bbox.GetMin().GetY(),
                              bbox.GetMax().GetX(),
                              bbox.GetMax().GetY(),
                              true);

    int dx,dy;
    GLOB->CANVAS->GetClientSize(&dx,&dy);

    wxSVGFileDC svgDC (filename, dx, dy, (float) 81.28) ;

    if (_gdsii)
    {
      _draw_driver->SetDC(&svgDC);
      if (_draw_driver->GetDrawStyle()!=SELECTED)
         _draw_driver->ClearClippingRegion();
      if (GetGridVisible() && (GetGridPlacement() == 0))
         _draw_driver->DrawGrid(GetGridSpace().GetValue(), GetGridColour(), 1);
      // write the GDS-II file to the screen
   	_draw_driver->Write(_gdsii);
      if (GetGridVisible() && (GetGridPlacement() == 1))
         _draw_driver->DrawGrid(GetGridSpace().GetValue(),GetGridColour(), 1);

      _draw_driver->DrawOrigin();
      _draw_driver->SetDC(0);
    }
    delete _draw_driver;

    _filename=_name_backup;
    _statusbar->SetFile(_filename);
	_statusbar->SetProcess();
	return svgDC.Ok();
}


// write a GDS-II structure as ascii exelon file format
bool CommandHandler::SaveFileExelon(const wxString& filename)
{
	if (filename.IsEmpty())
		ParameterError("Base CommandHandler(), Exelonfile");

	_statusbar->SetProcess("Writing EXELON file...");
	if (!_gdsii)
   {
		_messagehandler->error("No GDSII structure opened yet.","error" );
      return false;
   }

	// Open a file for writing exelon file
	EXL_driver_out *_driver = new EXL_driver_out(filename, _visible_only);
	if (!_driver)
		throw GDS_Error("Can't allocate memory for the exelon driver",
			"Base CommandHandler(), SaveFileEXL(), Memory Allocation Error", 9, 0);

	// Write the structure to the file
  	_driver->Write(_gdsii);

	// close the driver
	delete _driver;

   _filename=_name_backup;
	_statusbar->SetFile(_filename);
	_statusbar->SetProcess();
	return true;
}

// write a GDS-II structure as ascii Gerber file format
bool CommandHandler::SaveFileGerber(const wxString& filename)
{
	if (filename.IsEmpty())
		ParameterError("Base CommandHandler(), Gerberfile");

	_statusbar->SetProcess("Writing Gerber file...");
	if (!_gdsii)
   {
		_messagehandler->error("No GDSII structure opened yet.","error" );
      return false;
   }

	// Open a file for writing gerber file
	GBRDriver *_driver = new GBRDriver(filename, _visible_only);
	if (!_driver)
		throw GDS_Error("Can't allocate memory for the gerber driver",
			"Base CommandHandler(), SaveFileGerber(), Memory Allocation Error", 9, 0);

	// Write the structure to the file
  	_driver->Write(_gdsii);

	// close the driver
	delete _driver;

   _filename=_name_backup;
	_statusbar->SetFile(_filename);
	_statusbar->SetProcess();
	return true;
}

// write a GDS-II structure as ascii CNC file format
bool CommandHandler::SaveFileCnc(const wxString& filename)
{
	if (filename.IsEmpty())
		ParameterError("Base CommandHandler(), Cncfile");

	_statusbar->SetProcess("Writing Cnc file...");
	if (!_gdsii)
   {
		_messagehandler->error("No GDSII structure opened yet.","error" );
      return false;
   }

	// Open a file for writing CNC file
	HierDriver *_driver = new HierDriver(filename, _visible_only);
	if (!_driver)
		throw GDS_Error("Can't allocate memory for the CNC driver",
			"Base CommandHandler(), SaveFileCnc(), Memory Allocation Error", 9, 0);

	// Write the structure to the file
  	_driver->Write(_gdsii);

	// close the driver
	delete _driver;

   _filename=_name_backup;
	_statusbar->SetFile(_filename);
	_statusbar->SetProcess();
	return true;
}

// write a GDS-II structure as bitmap PNG file format
bool CommandHandler::SaveFilePNG(const wxString& filename)
{
	if (filename.IsEmpty())
		ParameterError("Base CommandHandler(), PNGfile");

	_statusbar->SetProcess("Writing PNG file...");
	if (!_gdsii)
   {
		_messagehandler->error("No GDSII structure opened yet.","error" );
      return false;
   }

   GLOB->MAINWINDOW->SavePNG(filename);

   _filename=_name_backup;
	_statusbar->SetFile(_filename);
	_statusbar->SetProcess();
	return true;
}

// write a GDS-II structure as bitmap PNG file format
bool CommandHandler::SaveFileBMP(const wxString& filename)
{
	if (filename.IsEmpty())
		ParameterError("Base CommandHandler(), BMPfile");

	_statusbar->SetProcess("Writing BMP file...");
	if (!_gdsii)
   {
		_messagehandler->error("No GDSII structure opened yet.","error" );
      return false;
   }

   GLOB->MAINWINDOW->SaveBMP(filename);

   _filename=_name_backup;
	_statusbar->SetFile(_filename);
	_statusbar->SetProcess();
	return true;
}


// save the GDS-II structure to a file
bool CommandHandler::SaveFileGDS(const wxString& filename)
{
	if (filename.IsEmpty())
		ParameterError("Base CommandHandler(), SaveFile");

	_statusbar->SetProcess("Writing GDS-II file...");
	// can we save something?
	if (!_gdsii)
   {
		_messagehandler->error("No GDSII structure opened yet.","error" );
      return false;
   }

	// Open a file for writing GDSII
	GDS_driver_out *_driver = new GDS_driver_out(filename, _visible_only);
	if (!_driver)
		throw GDS_Error("Can't allocate memory for the GDS-II driver",
			"Base CommandHandler(), SaveFile(), Memory Allocation Error", 9, 0);
	if (_driver->bad())
	{
		delete _driver;
		throw GDS_Error("Could not open file for output",
			"Base CommandHandler(), SaveFile(), File Error");
	}
	// Write the structure to the file
  	_driver->Write(_gdsii);

	// close the file
	delete _driver;

   if (_name_backup!=_filename)
	   _filename=_name_backup;
	_statusbar->SetFile(_filename);
	_statusbar->SetProcess();
	return true;
}

// write a GDS-II structure as Flash file format
bool CommandHandler::SaveFileFLASH(const wxString& filename)
{
	if (filename.IsEmpty())
		ParameterError("Base CommandHandler(), FLASHfile");

	_statusbar->SetProcess("Writing FLASH file...");
	// can we save something?
	if (!_gdsii)
   {
		_messagehandler->error("No GDSII structure opened yet.","error" );
      return false;
   }

	FLASH_driver_out *_driver = new FLASH_driver_out(filename, _visible_only);
	if (!_driver)
		throw GDS_Error("Can't allocate memory for the GDS-II ",
			"Base CommandHandler(), SaveFileFLASH(), Memory Allocation Error", 9, 0);

	// Write the structure to the file
  	_driver->Write(_gdsii);

	// close the driver
	delete _driver;

   _filename=_name_backup;
	_statusbar->SetFile(_filename);
	_statusbar->SetProcess();
	return true;
}

// write a GDS-II structure as EMASK file format
bool CommandHandler::SaveFileEMASK(const wxString& filename)
{
	if (filename.IsEmpty())
		ParameterError("Base CommandHandler(), EMASKfile");

	_statusbar->SetProcess("Writing EMASK file...");
	// can we save something?
	if (!_gdsii)
   {
		_messagehandler->error("No GDSII structure opened yet.","error" );
      return false;
   }

	EMASK_driver_out *_driver = new EMASK_driver_out(filename, _visible_only);
	if (!_driver)
		throw GDS_Error("Can't allocate memory for the GDS-II EMASKdriver",
			"Base CommandHandler(), SaveFileEMASK(), Memory Allocation Error", 9, 0);

	// Write the structure to the file
  	_driver->Write(_gdsii);

	// close the driver
	delete _driver;

   _filename=_name_backup;
	_statusbar->SetFile(_filename);
	_statusbar->SetProcess();
	return true;
}

bool CommandHandler::LoadSetup()
{
   if (_I->count() != 1) return false;
	if (_I->item()->Len()==0)
		ParameterError("Base CommandHandler(), LoadSetup");

	if (wxFileExists(_I->item()->c_str()))
   // pass command to the command handler
   {
      ProcessParser*	_processparser;
      if (_statusbar)
	      _statusbar->SetProcess("Loading setup file...");
      SetProcEnv(_I->item()->c_str());
      // make a parser for the process file and process it
      _processparser = new ProcessParser((char*)_I->item()->c_str(),GLOB->ALIASLIST);
      if (_statusbar)
	      _statusbar->SetProcess();
      if (_processparser->LoadProcess()==false)
      {
         delete _processparser;
         return false;
      }
      delete _processparser;
   }
	else
	{
		char errbuf[LINELENGTH];
		sprintf(errbuf,"Setup File doesn't exist! "
							 "LoadSetup(): %s\n",_I->item());
		_messagehandler->info(errbuf, "Error loading setup");
		return false;
	}

   _CurrentLayer=0;
   if (_statusbar)
	   _statusbar->SetInsertLayer(_setup->Get_Name(_CurrentLayer));
   return true;
}

bool CommandHandler::SaveSetup()
{
   if (m_viewer)
   {
		_messagehandler->info( "only view mode can not save", "SaveSetUp error" );
		return false;
   }

   if (_I->count() != 1) return false;
	if(_I->item()->Len() > 0 )
	{
	   char buf[LINELENGTH];
      strcpy(buf,_I->item()->c_str());
		DoCommand("record  %s",buf);

		DoCommand("# Startup");
		DoCommand("showcoords        %s", _view_coord   ? "1" : "0" );
		DoCommand("drawoutline       %s", _view_outline ? "1" : "0" );
		DoCommand("structhres        %d", GetStrucThres() );
		DoCommand("primthres         %d %s", GetPrimThres(),GetDrawSmallPrim()? "1" : "0"  );

		DoCommand("selected           %s", GetSelectedOnly()? "1" : "0" );
		DoCommand("# Transformation");
		DoCommand("mirrorx           %s", GetMirrorX()? "1" : "0" );
		DoCommand("mirrory           %s", GetMirrorY()? "1" : "0" );
		DoCommand("movehor           {%s}", GetMoveHor().GetValueString().c_str() );
		DoCommand("movever           {%s}", GetMoveVer().GetValueString().c_str() );
		DoCommand("scale             %f", GetScale() );
		DoCommand("rotate            {%f}", GetRotate() );
		DoCommand("direction         {%s}", GetDirection() ? "clockwise" : "counterclockwise"  );

		DoCommand("# Factors");
		DoCommand("selectmarge       {%f}", GetSelectmarge());
		DoCommand("snapfactor        {%s}", GetSnapfactor().GetValueString().c_str() );
		DoCommand("correctionfactor  {%s}", GetCorrectionfactor().GetValueString().c_str() );
		DoCommand("roundfactor       {%f}", GetRoundfactor());
		DoCommand("correctionaber    {%s}", GetCorrectionaber().GetValueString().c_str() );
		DoCommand("roundtype         {%s}", GetRoundtype().c_str() );
		DoCommand("toarcrmin         {%s}", GetPoly2ArcRmin().GetValueString().c_str() );
		DoCommand("toarcrmax         {%s}", GetPoly2ArcRmax().GetValueString().c_str() );
		DoCommand("arcaccur          {%s}", GetPoly2ArcAber().GetValueString().c_str() );
		DoCommand("polyaccur         {%s}", GetArc2PolyAber().GetValueString().c_str() );
		DoCommand("smoothaccur       {%s}", GetSmoothAber().GetValueString().c_str() );
		DoCommand("maxlinemerge      {%s}", GetMaxlinemerge().GetValueString().c_str() );
		DoCommand("displayaber       {%s}", GetDisplayaber().GetValueString().c_str() );
      DoCommand("savesetuponexit   {%s}",  _savesetuponexit ? "1" : "0" );
      DoCommand("savevisible       {%s}",  _visible_only ? "1" : "0" );

		DoCommand("# Grid settings");
      DoCommand("showgrid          %s", GetGridVisible() ? "1" : "0" );
      DoCommand("gridspace         {%s}", GetGridSpace().GetValueString().c_str() );
      DoCommand("gridcolour        %s", _str_grid_colour );
      DoCommand("gridplacement     %i", GetGridPlacement() );
      DoCommand("backgroundcolour  %s", _str_back_colour );

		// For all layers send the setting to this file
		DoCommand("# Layer settings");
		for (int i = 0; i < MAX_LAYERS; i++)
		{
			DoCommand("setlayer       %d", i);

			DoCommand("name           {%s}", _setup->Get_Name(i));
			DoCommand("visible        %d", _setup->Get_Visible(i));
			DoCommand("readlayer      %d", _setup->Get_Read(i));

			long color;
			int R, G, B;
			color = _setup->Get_BorderColor(i);
			R = (int)(color % 256);
			G = (int)((color >> 8) % 256);
			B = (int)((color >> 16) % 256);
			DoCommand("bordercolor    %d %d %d", R, G, B);

			color = _setup->Get_FillColor(i);
			R = (int)(color % 256);
			G = (int)((color >> 8) % 256);
			B = (int)((color >> 16) % 256);
			DoCommand("fillcolor      %d %d %d", R, G, B);

			DoCommand("brush          %d",_setup->Get_Brush(i));
			DoCommand("order          %d",_setup->Get_Order(i));
			DoCommand("gdsinmap       %d",_setup->Get_GdsInMapping(i));
			DoCommand("gdsoutmap      %d",_setup->Get_GdsOutMapping(i));
		}

		DoCommand("record");

		return true;
	}
	else
		return false;
}




// Operations Section -------------------------------------------------------

// define processgroup A
// if buffer contains an empty string or "empty"
// it will clear the group
// the format for layers is as folows :
// 	layer layer ...layer boolean (true or false)
//
// all layer are in the group and the boolean at the end specifies if only the
// selected object should be processed.

bool CommandHandler::Define_A()
{
   if ((_I->count() ==0 ) || (_I->item()->Len()==0))
      return false;
   if (_I->count() ==1 )
   {
   	if (_I->item()->CmpNoCase("empty")==0)
   	{
         _operationmethod=GROUP_LAYER;
			_group_A->Clear();
      	return true;
   	}
      else if (_I->item()->CmpNoCase("selected")==0)
      {
         _operationmethod=GROUP_SELECTED;
	      _gdsii->ClearFlags(ELE_GR_A);
         _gdsii->SetFlags(ELE_GR_A,true); //set selected to GR_A flags
      	return true;
      }
   }

   _operationmethod=GROUP_LAYER;
	_group_A->Clear();
   if (_I->count() < 2)
   {
   	_messagehandler->info("format : \n layer layer ... layer Boolean (true or false)","group error");
		return false;
   }
   _I->totail();
   if (_I->item()->CmpNoCase("true")==0) _group_A->SetOnlySelected(true);
   else if (_I->item()->CmpNoCase("false")==0) _group_A->SetOnlySelected(false);
   else
   {
   	_messagehandler->info("format : \n layer layer ... layer Boolean (true or false)","group error");
		return false;
   }

   _I->tohead();
   for (int i=0;i < _I->count()-1;i++)
   {
	  	_group_A->SetLayer(atoi(_I->item()->c_str()),true);
      (*_I)++;
   }

	return(true);
}

// define processgroup B
// if buffer contains an empty string or "empty"
// it will clear the group
bool CommandHandler::Define_B()
{
   if ((_I->count() ==0 ) || (_I->item()->Len()==0))
      return false;
   if (_I->count() ==1 )
   {
   	if (_I->item()->CmpNoCase("empty")==0)
   	{
         _operationmethod=GROUP_LAYER;
			_group_B->Clear();
      	return true;
   	}
      else if (_I->item()->CmpNoCase("selected")==0)
      {
         _operationmethod=GROUP_SELECTED;
         _gdsii->ClearFlags(ELE_GR_B); //clear all GR_B flags
         _gdsii->SetFlags(ELE_GR_B,true); //set selected to GR_B flags
      	return true;
      }
   }

   _operationmethod=GROUP_LAYER;
	_group_B->Clear();
   if (_I->count() < 2)
   {
   	_messagehandler->info("format : \n layer layer ... layer Boolean (true or false)","group error");
		return false;
   }
   _I->totail();
   if (_I->item()->CmpNoCase("true")==0) _group_B->SetOnlySelected(true);
   else if (_I->item()->CmpNoCase("false")==0) _group_B->SetOnlySelected(false);
   else
   {
   	_messagehandler->info("format : \n layer layer ... layer Boolean (true or false)","group error");
		return false;
   }

   _I->tohead();
   for (int i=0;i < _I->count()-1;i++)
   {
	  	_group_B->SetLayer(atoi(_I->item()->c_str()),true);
      (*_I)++;
   }

	return(true);
}

// define the destination layer
bool CommandHandler::DefineDestination()
{
   if (_I->count() ==1 )
   {
      if (_I->item()->Len()==0)
         ParameterError("Base CommandHandler(), DefineDestination");

      if (_I->item()->CmpNoCase("empty") != 0)
         _destination=atoi(_I->item()->c_str());
      else
         _destination=-1;

      m_destination_add=false;
   }
   else if (_I->count() ==2 )
   {
      if (_I->item()->Len()==0)
         ParameterError("Base CommandHandler(), DefineDestination");

      if (_I->item()->CmpNoCase("empty") != 0)
         _destination=atoi(_I->item()->c_str());
      else
         _destination=-1;

      (*_I)++;

      if (_I->item()->CmpNoCase("true") == 0)
         m_destination_add=true;
      else
         m_destination_add=false;

   }
   else
   {
		if (_I->item()->Len()==0)
			ParameterError("Base CommandHandler(), DefineDestination");
      return false;
   }
	return(true);
}

bool CommandHandler::Operation_transform()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_operationmethod=SELECTED_ELEMENTS;
	_statusbar->SetProcess("Transform selected...");
   SetOperationFlags(false);
   double degr = _rotate;
   if (_direction)
      degr = -degr;
   _gdsii->Transform(_mirror_x,_mirror_y,
   						_move_hor.GetValue()/GLOB->PHYSUNITS(),_move_ver.GetValue()/GLOB->PHYSUNITS(),
                     _scaling.GetValue(),_scaling.GetValue(),degr,ELE_GR_C);
	_statusbar->SetProcess();

	return true;
}

bool CommandHandler::Operation_undotrans()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_operationmethod=SELECTED_ELEMENTS;
   _statusbar->SetProcess("Undo transform...");
   SetOperationFlags(false);

  	_gdsii->UndoTransform(ELE_GR_C);

	_statusbar->SetProcess();

	return true;
}

bool CommandHandler::Operation_move_a()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_statusbar->SetProcess("move_a -> d...");
   SetOperationFlags(true);

   _gdsii->Move(_destination,ELE_GR_A);

	_statusbar->SetProcess();

	// This checks the memory structures for available layers (with content)
	GLOB->SETUP->Set_UpdateAvailableLayers(true);

	return true;
}

bool CommandHandler::Operation_copy_a()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_statusbar->SetProcess("copy_a -> d...");
   SetOperationFlags(true);
   _gdsii->Copy(_destination,ELE_GR_A);

	_statusbar->SetProcess();

	// This checks the memory structures for available layers (with content)
	GLOB->SETUP->Set_UpdateAvailableLayers(true);

	return true;
}

bool CommandHandler::Operation_delete_a()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_statusbar->SetProcess("delete_a...");

   SetOperationFlags(true);

   _gdsii->Delete_A(ELE_GR_A);

	_statusbar->SetProcess();

	// This checks the memory structures for available layers (with content)
	GLOB->SETUP->Set_UpdateAvailableLayers(true);

	return true;
}

bool CommandHandler::Operation_addproperty_a()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

   if (_I->count() != 3)  return false;
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.",
			"Base CommandHandler(), Flatten(), Structure Error", 9, 0);

	_statusbar->SetProcess("AddProperty...");
   wxString name=*_I->item(); (*_I)++;
   wxString type=*_I->item(); (*_I)++;
   wxString value=*_I->item();

	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_statusbar->SetProcess("addproperty  to group_a...");

   SetOperationFlags(true);

   _gdsii->AddProperty(ELE_GR_A,name,type,value);

	_statusbar->SetProcess();

	// This checks the memory structures for available layers (with content)
	GLOB->SETUP->Set_UpdateAvailableLayers(true);
	_statusbar->SetProcess();
	return true;
}

bool CommandHandler::Operation_transform_a()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_statusbar->SetProcess("tranform_a ...");

   SetOperationFlags(false);

   double degr = _rotate;
   if (_direction)
      degr = -degr;
   _gdsii->Transform(_mirror_x,_mirror_y,
   						_move_hor.GetValue()/GLOB->PHYSUNITS(),_move_ver.GetValue()/GLOB->PHYSUNITS(),
                     _scaling.GetValue(),_scaling.GetValue(),degr,ELE_GR_A);

	_statusbar->SetProcess();

	return true;
}

bool CommandHandler::Operation_undotrans_a()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_statusbar->SetProcess("undo transform...");

   SetOperationFlags(false);

  	_gdsii->UndoTransform(ELE_GR_A);

	_statusbar->SetProcess();

	return true;
}

bool CommandHandler::Operation_calcarea_a()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_statusbar->SetProcess("calcarea_a ...");
   SetOperationFlags(true);
   double area=0;
   _gdsii->CalcArea(&area,ELE_GR_A);
   _statusbar->SetProcess("Setting Alias...");

   area = area * pow(_gdsii->GetPhysicalUnits(),2);  //meter^2
   char areastr[80];
   sprintf(areastr,"{%30.10f square meter}",area);

   if (!GLOB->ALIASLIST->SetAlias((char*)_I->item()->c_str(),areastr))
   {
      char errbuf[LINELENGTH];
      sprintf(errbuf," SetAlias() Error: Could not set Alias %s = %s\n",
                        _I->item()->c_str(),areastr);
      _messagehandler->info(errbuf,"BaseCommandHandler Command Execution Error.");
      return false;
   }

	_statusbar->SetProcess();

	// This checks the memory structures for available layers (with content)
	GLOB->SETUP->Set_UpdateAvailableLayers(true);

	return true;
}

bool CommandHandler::Operation_makering()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_statusbar->SetProcess("makering a -> d...");
   SetOperationFlags(true);
	_gdsii->Operation(MAKE_RING,ELE_GR_A,ELE_NON,_destination,m_destination_add);

	_statusbar->SetProcess();

	// This checks the memory structures for available layers (with content)
	GLOB->SETUP->Set_UpdateAvailableLayers(true);

	return true;
}

bool CommandHandler::Operation_correction()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_statusbar->SetProcess("correction a -> d...");
   SetOperationFlags(true);

	_gdsii->Operation(CORRECTION,ELE_GR_A,ELE_NON,_destination,m_destination_add);
	_statusbar->SetProcess();

	// This checks the memory structures for available layers (with content)
	GLOB->SETUP->Set_UpdateAvailableLayers(true);

	return true;
}

bool CommandHandler::Operation_toarcs()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_statusbar->SetProcess("toarcs a -> d...");

   SetOperationFlags(true);

   _gdsii->ToArcs(_destination,ELE_GR_A);

	_statusbar->SetProcess();

	// This checks the memory structures for available layers (with content)
	GLOB->SETUP->Set_UpdateAvailableLayers(true);

	return true;
}

bool CommandHandler::Operation_tolines()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_statusbar->SetProcess("tolines a -> d...");

   SetOperationFlags(true);

   _gdsii->ToLines(_destination,ELE_GR_A);

	_statusbar->SetProcess();

	// This checks the memory structures for available layers (with content)
	GLOB->SETUP->Set_UpdateAvailableLayers(true);

	return true;
}

bool CommandHandler::Operation_smoothen()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_statusbar->SetProcess("smoothen a -> d...");

   SetOperationFlags(true);

	_gdsii->Operation(SMOOTHEN,ELE_GR_A,ELE_NON,_destination,m_destination_add);

	_statusbar->SetProcess();

	// This checks the memory structures for available layers (with content)
	GLOB->SETUP->Set_UpdateAvailableLayers(true);

	return true;
}

bool CommandHandler::Operation_intersect()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_statusbar->SetProcess("intersect a _ b -> d...");
   SetOperationFlags(true);

	_gdsii->Operation(AND,ELE_GR_A,ELE_GR_B,_destination,m_destination_add);

	_statusbar->SetProcess();

	// This checks the memory structures for available layers (with content)
	GLOB->SETUP->Set_UpdateAvailableLayers(true);

	return true;
}

bool CommandHandler::Operation_merge()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_statusbar->SetProcess("merge a _ b -> d...");
   SetOperationFlags(true);

	_gdsii->Operation(OR,ELE_GR_A,ELE_GR_B,_destination,m_destination_add);

	_statusbar->SetProcess();

	// This checks the memory structures for available layers (with content)
	GLOB->SETUP->Set_UpdateAvailableLayers(true);

	return true;
}

bool CommandHandler::Operation_a_subtract_b()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_statusbar->SetProcess("subtract a _ b -> d...");
   SetOperationFlags(true);
	_gdsii->Operation(A_SUB_B,ELE_GR_A,ELE_GR_B,_destination,m_destination_add);

	_statusbar->SetProcess();

	// This checks the memory structures for available layers (with content)
	GLOB->SETUP->Set_UpdateAvailableLayers(true);

	return true;
}

bool CommandHandler::Operation_b_subtract_a()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_statusbar->SetProcess("subtract b _ a -> d...");
   SetOperationFlags(true);

	_gdsii->Operation(B_SUB_A,ELE_GR_A,ELE_GR_B,_destination,m_destination_add);

	_statusbar->SetProcess();

	// This checks the memory structures for available layers (with content)
	GLOB->SETUP->Set_UpdateAvailableLayers(true);

	return true;
}

bool CommandHandler::Operation_exor()
{
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "Operation Error", 9, 0);

	_statusbar->SetProcess("exor a _ b -> d...");
   SetOperationFlags(true);

	_gdsii->Operation(EXOR,ELE_GR_A,ELE_GR_B,_destination,m_destination_add);

	_statusbar->SetProcess();

	// This checks the memory structures for available layers (with content)
	GLOB->SETUP->Set_UpdateAvailableLayers(true);

	return true;
}

bool CommandHandler::SetSelectedOnly()
{
	bool i= atoi(_I->item()->c_str()) != 0;
	_selected_only = i;
   return true;
}

bool CommandHandler::SetVisibleOnly()
{
	bool i= atoi(_I->item()->c_str()) != 0;
   _visible_only = i;
   return true;
}

bool CommandHandler::SetSaveSetUpOnExit()
{
	bool i= atoi(_I->item()->c_str()) != 0;
   _savesetuponexit = i;
   return true;
}

bool CommandHandler::SetGridColour()
{
   if (_I->count() != 3)  return false;

	strcpy(_str_grid_colour,"");
	unsigned char color[3];

	for (int j = 0; j < 3; j++)
	{
		color[j] = (unsigned char) atoi(_I->item()->c_str());
		strcat(_str_grid_colour,_I->item()->c_str());
		strcat(_str_grid_colour," ");
		(*_I)++;
	}
	_grid_colour  = (long) color[0];
	_grid_colour |= ((long)color[1] << 8);
	_grid_colour |= ((long)color[2] << 16);

   return true;
}

bool CommandHandler::SetBackColour()
{
   if (_I->count() != 3)  return false;

	strcpy(_str_back_colour,"");
	unsigned char color[3];

	for (int j = 0; j < 3; j++)
	{
		color[j] = (unsigned char) atoi(_I->item()->c_str());
		strcat(_str_back_colour,_I->item()->c_str());
		strcat(_str_back_colour," ");
		(*_I)++;
	}
	_back_colour  = (long) color[0];
	_back_colour |= ((long)color[1] << 8);
	_back_colour |= ((long)color[2] << 16);

	GLOB->MAINWINDOW->SetBackGroundColor(_back_colour);
   return true;
}

long CommandHandler::GetGridColour()
{
 	return _grid_colour;
}

long CommandHandler::GetBackColour()
{
 	return _back_colour;
}

bool CommandHandler::SetGridSpace()
{
   if (_I->count() != 1)  return false;
	m_grid_space.Eval(*_I->item());
   return true;
}

bool CommandHandler::GetMirrorX()
{
	return _mirror_x;
}
bool CommandHandler::GetMirrorY()
{
	return _mirror_y;
}

wxUnit& CommandHandler::GetMoveHor()
{
	return _move_hor;
}

wxUnit& CommandHandler::GetMoveVer()
{
	return _move_ver;
}

wxUnit& CommandHandler::GetSnapfactor()
{
	return _snapfactor;
}

wxUnit& CommandHandler::GetCorrectionfactor()
{
	return _correctionfactor;
}

wxUnit& CommandHandler::GetCorrectionaber()
{
	return _correctionaber;
}

double CommandHandler::GetRoundfactor()
{
	return _roundfactor;
}

double  CommandHandler::GetScale()
{
	return _scaling;
}

double  CommandHandler::GetRotate()
{
	return _rotate;
}

bool	  CommandHandler::GetDirection()
{
	return _direction;
}


wxUnit& CommandHandler::GetGridSpace()
{
 	return m_grid_space;
}

bool CommandHandler::SetGridPlacement()
{
   if (_I->count() != 1)  return false;
	_grid_placement = atoi(_I->item()->c_str());
   return true;
}

bool CommandHandler::SetStartPoint()
{
   if (_I->count() != 2)
	{
		char errbuf[LINELENGTH];
		sprintf(errbuf, "Startpoint command does not have 2 parameters \n"
							 "Syntax: startpoint x y");
		_messagehandler->info(errbuf, "Parser Error");
      return false;
   }

   if (_gdsii)
   {
      _start_point_x = atof(_I->item()->c_str())*GLOB->PHYSUNITS()/GLOB->ENTRYUNITS();
      (*_I)++;
      _start_point_y = atof(_I->item()->c_str())*GLOB->PHYSUNITS()/GLOB->ENTRYUNITS();
   }
   else
   {
		_start_point_x=0;
		_start_point_y=0;
   }
  	GLOB->MAINWINDOW->_distDialog->setStartPoint(_start_point_x,_start_point_y);
	return true;
}

bool CommandHandler::SetRelPoint()
{
   if (_I->count() != 2)
	{
		char errbuf[LINELENGTH];
		sprintf(errbuf, "Relativepoint command does not have 2 parameters \n"
							 "Syntax: relativepoint x y");
		_messagehandler->info(errbuf, "Parser Error");
      return false;
   }
   if (_gdsii)
   {
      _rel_point_x = atof(_I->item()->c_str())*GLOB->PHYSUNITS()/GLOB->ENTRYUNITS();
      (*_I)++;
      _rel_point_y = atof(_I->item()->c_str())*GLOB->PHYSUNITS()/GLOB->ENTRYUNITS();
   }
   else
   {
		_rel_point_x=0;
		_rel_point_y=0;
   }
  	GLOB->MAINWINDOW->_distDialog->setRelPoint(_rel_point_x,_rel_point_y);
	return true;
}

int CommandHandler::GetGridPlacement()
{
 	return _grid_placement;
}

// flatten selected structures
bool CommandHandler::Flatten()
{
   if (_I->count() > 1)  return false;

   bool deep=true;

   if (_I->count() == 1)
      deep=false;

	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.",
			"Base CommandHandler(), Flatten(), Structure Error", 9, 0);

	_statusbar->SetProcess("Flatten...");
   _gdsii->Flatten(_selected_only,deep);
	_statusbar->SetProcess();

	BoundingBox bbox = _gdsii->GetShowStructure()->GetBoundingBox();
   GLOB->SCRDRV->SetMappingScroll(
										bbox.GetMin().GetX(),
										bbox.GetMin().GetY(),
										bbox.GetMax().GetX(),
										bbox.GetMax().GetY(),
										true);

	return true;
}

// add property to elements (selected)
bool CommandHandler::AddProperty()
{
   if (_I->count() != 3)  return false;
	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.",
			"Base CommandHandler(), AddProperty(), Structure Error", 9, 0);

	_statusbar->SetProcess("AddProperty...");
   wxString name=*_I->item(); (*_I)++;
   wxString type=*_I->item(); (*_I)++;
   wxString value=*_I->item();

   SetOperationFlags(false);

	_gdsii->AddProperty(name,type,value,_selected_only);
	_statusbar->SetProcess();

	return true;
}

//	Setting Functions --------------------------------------------------------

// Transformation

bool CommandHandler::SetMirrorX()
{
   if (_I->count() != 1)  return false;
   if ((*_I->item()=="true") || (*_I->item()=="1"))
		_mirror_x=true;
   else
		_mirror_x=false;
	return true;
}

bool CommandHandler::SetMirrorY()
{
   if (_I->count() != 1)  return false;
   if ((*_I->item()=="true") || (*_I->item()=="1"))
		_mirror_y=true;
   else
		_mirror_y=false;
	return true;
}

bool CommandHandler::SetMoveHor()
{
   if (_I->count() != 1)  return false;
	_move_hor=*_I->item();
	return true;
}

bool CommandHandler::SetMoveVer()
{
   if (_I->count() != 1)  return false;
	_move_ver=*_I->item();
	return true;
}

bool CommandHandler::SetScale()
{
   if (_I->count() != 1)  return false;
	_scaling=*_I->item();
	return true;
}

bool CommandHandler::SetRotate()
{
   if (_I->count() != 1)  return false;
   _I->item()->ToDouble(&_rotate);
	return true;
}

bool CommandHandler::SetDirection()
{
   if (_I->count() != 1)  return false;
   if (_I->item()->CmpNoCase("Clockwise"))
		_direction=false;
   else
		_direction=true;
	return true;
}

bool CommandHandler::SetWindingRule()
{
   if (_I->count() != 1)  return false;

   if (_I->item()->CmpNoCase("True"))
		GLOB->WINDINGRULE=false;
   else
		GLOB->WINDINGRULE=true;
	return true;
}

// set the marge and map to database units if there is a GDS-II file loaded
// else this must be done later (in loading)
bool CommandHandler::SetSelectmarge()
{
   if (_I->count() != 1)  return false;
	_I->item()->ToDouble(&_selectmarge);
	return true;
}

bool CommandHandler::SetSnapfactor()
{
   if (_I->count() != 1)  return false;
	_snapfactor=*_I->item();
	return true;
}

bool CommandHandler::SetCorrectionfactor()
{
   if (_I->count() != 1)  return false;
	_correctionfactor=*_I->item();
	return true;
}

bool CommandHandler::SetCorrectionaber()
{
   if (_I->count() != 1)  return false;
	_correctionaber=*_I->item();
	return true;
}

bool CommandHandler::SetRoundfactor()
{
   if (_I->count() != 1)  return false;
	_I->item()->ToDouble(&_roundfactor);
	return true;
}

bool CommandHandler::SetRoundtype()
{
   if (_I->count() != 1)  return false;
	_roundtype=*_I->item();
	return true;
}


bool CommandHandler::SetPoly2ArcRmin()
{
   if (_I->count() != 1)  return false;
	_poly2arcrmin=*_I->item();
	return true;
}


bool CommandHandler::SetPoly2ArcRmax()
{
   if (_I->count() != 1)  return false;
	_poly2arcrmax=*_I->item();
	return true;
}

bool CommandHandler::SetPoly2ArcAber()
{
   if (_I->count() != 1)  return false;
	_poly2arcaber=*_I->item();
	return true;
}

bool CommandHandler::SetArc2PolyAber()
{
   if (_I->count() != 1)  return false;
	_arc2polyaber=*_I->item();
	return true;
}

bool CommandHandler::SetSmoothAber()
{
   if (_I->count() != 1)  return false;
	_smoothaber=*_I->item();
	return true;
}


bool CommandHandler::SetDisplayaber()
{
   if (_I->count() != 1)  return false;
	_displayaber=*_I->item();
	return true;
}

bool CommandHandler::SetStrucThres()
{
   if (_I->count() != 1)  return false;
	_I->item()->ToLong((long*)&_structhres);

	return true;
}

bool CommandHandler::SetWidth()
{
   if (_I->count() != 1)  return false;

	_width=*_I->item();
	return true;
}

wxUnit& CommandHandler::GetWidth()
{
	return _width;
}

bool CommandHandler::SetPathtype()
{
   if (_I->count() != 1)  return false;

   _pathtype=atoi(_I->item()->c_str());
	return true;
}

int CommandHandler::GetPathtype()
{
	return _pathtype;
}

bool CommandHandler::SetPrimThres()
{
   if (_I->count() != 2)  return false;
	_I->item()->ToLong((long*)&_primthres);
   (*_I)++;
	if (*_I->item()=="true")
   	_drawsmallprim=true;
   else
   	_drawsmallprim=false;
	return true;
}

bool CommandHandler::SetMaxlinemerge()
{
   if (_I->count() != 1)  return false;
	_maxlinemerge=*_I->item();
	return true;
}

wxString& CommandHandler::GetRoundtype()
{
	return _roundtype;
}

wxUnit& CommandHandler::GetPoly2ArcRmin()
{
	return _poly2arcrmin;
}

wxUnit& CommandHandler::GetPoly2ArcRmax()
{
	return _poly2arcrmax;
}

wxUnit& CommandHandler::GetPoly2ArcAber()
{
	return _poly2arcaber;
}

wxUnit& CommandHandler::GetArc2PolyAber()
{
	return _arc2polyaber;
}

wxUnit& CommandHandler::GetSmoothAber()
{
	return _smoothaber;
}

wxUnit& CommandHandler::GetDisplayaber()
{
	return _displayaber;
}

int CommandHandler::GetStrucThres()
{
	return _structhres;
}

int CommandHandler::GetPrimThres()
{
	return _primthres;
}

bool CommandHandler::GetDrawSmallPrim()
{
	return _drawsmallprim;
}

wxUnit& CommandHandler::GetMaxlinemerge()
{
	return _maxlinemerge;
}

// ---=== functions used to pass data between objects ===---
group* CommandHandler::Get_A()
{
	return _group_A;
}

group* CommandHandler::Get_B()
{
	return _group_B;
}

int CommandHandler::Get_Destination()
{
	return _destination;
}

GdsII* CommandHandler::Get_GDSII()
{
	return _gdsii;
}

void CommandHandler::Set_GDSII(GdsII* a_gdsii)
{
	_gdsii = a_gdsii;
   GLOB->USERUNITS(_gdsii->GetUserUnits());
   GLOB->PHYSUNITS(_gdsii->GetPhysicalUnits());
}

Structure* CommandHandler::Get_TopStructure()
{
	return	_gdsii->GetTopStructure();
}

wxString CommandHandler::Get_FileName()
{
	return _filename;
}

void CommandHandler::Set_FileName(const wxString& name)
{
	_filename=name;
}

bool CommandHandler::SetCurrentLayer()
{
	if (_I->count() != 1)  return false;
	_CurrentLayer=atoi(_I->item()->c_str());
   _statusbar->SetInsertLayer(_setup->Get_Name(_CurrentLayer));

   return true;
}

bool CommandHandler::SetCurrentLayerByName()
{
	if (_I->count() != 1)  return false;
	if (_setup->Get_Number(_I->item()->c_str()) != -1)
   {
		_CurrentLayer=_setup->Get_Number(_I->item()->c_str());
	   _statusbar->SetInsertLayer(_I->item()->c_str());
	   return true;
   }
   return false;
}

int CommandHandler::GetCurrentLayer()
{
	return _CurrentLayer;
}

bool CommandHandler::SetLayerName()
{
	if (_I->count() != 1)  return false;
	_setup->Set_Name(_CurrentLayer,_I->item()->c_str());
   return true;
}

bool CommandHandler::SetGdsInMap()
{
	if (_I->count() != 1)  return false;
	_setup->Set_GdsInMapping(_CurrentLayer,atoi(_I->item()->c_str()));
   return true;
}

bool CommandHandler::SetGdsOutMap()
{
	if (_I->count() != 1)  return false;
	_setup->Set_GdsOutMapping(_CurrentLayer,atoi(_I->item()->c_str()));
   return true;
}

bool CommandHandler::SetLayerVisible()
{
	 if (_I->count() == 1)
		 _setup->Set_Visible(_CurrentLayer, 0 != atoi(_I->item()->c_str()));
    else if (_I->count() == 2)
    {
    	_I->tohead();
      (*_I)++;
      if (_I->item()->CmpNoCase("all")==0)
      {
      	(*_I)--;
         for (int j = 0; j < MAX_LAYERS; j++)
         {
            _setup->Set_Visible(j, 0 != atoi(_I->item()->c_str()));
         }
      }
      else
      	return false;
    }
    else
	    return false;
   return true;
}

bool CommandHandler::SetLayerSelectable()
{
	 if (_I->count() == 1)
		 _setup->Set_Selectable(_CurrentLayer, 0 != atoi(_I->item()->c_str()));
    else if (_I->count() == 2)
    {
    	_I->tohead();
      (*_I)++;
      if (_I->item()->CmpNoCase("all")==0)
      {
      	(*_I)--;
         for (int j = 0; j < MAX_LAYERS; j++)
         {
            _setup->Set_Selectable(j,0 != atoi(_I->item()->c_str()));
         }
      }
      else
      	return false;
    }
    else
	    return false;
   return true;
}

bool CommandHandler::SetLayerBorderColor()
{
	// find de three colors defined in value R,G,B
   if (_I->count() != 3)  return false;

	unsigned char color[3];

	for (int j = 0; j < 3; j++)
	{
		color[j] = (unsigned char) atoi(_I->item()->c_str());
		(*_I)++;
	}

	//set the colors for this layer
	_setup->Set_BorderColor(_CurrentLayer, color[0], color[1], color[2]);
   return true;
}

bool CommandHandler::SetLayerFillColor()
{
	// find de three colors defined in value R,G,B
   if (_I->count() != 3)  return false;

	unsigned char color[3];

	for (int j = 0; j < 3; j++)
	{
		color[j] = (unsigned char) atoi(_I->item()->c_str());
		(*_I)++;
	}

	//set the colors for this layer
	_setup->Set_FillColor(_CurrentLayer, color[0], color[1], color[2]);
   return true;
}

bool CommandHandler::SetLayerBrush()
{
   if (_I->count() != 1)  return false;
	_setup->Set_Brush(_CurrentLayer,atoi(_I->item()->c_str()));
   return true;
}

bool CommandHandler::SetLayerOrder()
{
   if (_I->count() != 1)  return false;
	_setup->Set_Order(_CurrentLayer,atoi(_I->item()->c_str()));
   return true;
}

bool CommandHandler::SetLayerRead()
{
	 if (_I->count() == 1)
		 _setup->Set_Read(_CurrentLayer,0 != atoi(_I->item()->c_str()));
    else if (_I->count() == 2)
    {
    	_I->tohead();
      (*_I)++;
      if (_I->item()->CmpNoCase("all")==0)
      {
	      (*_I)--;
         for (int j = 0; j < MAX_LAYERS; j++)
         {
            _setup->Set_Read(j,0 != atoi(_I->item()->c_str()));
         }
      }
      else
      	return false;
    }
    else
	    return false;
   return true;
}

bool CommandHandler::GetViewCoord()
{
	return _view_coord;
}

bool CommandHandler::SetViewCoord(bool value)
{
	 _view_coord=value;
	return true;
}

bool CommandHandler::GetViewOutline()
{
	return _view_outline;
}

bool CommandHandler::GetSelectedOnly()
{
	return _selected_only;
}

bool CommandHandler::GetVisibleOnly()
{
	return _visible_only;
}

bool CommandHandler::GetSaveSetUpOnExit()
{
	return _savesetuponexit;
}

bool CommandHandler::GetGridVisible()
{
	return _show_grid;
}

// will be called when there are insufficient parameters after the command
// throws an error class
void CommandHandler::ParameterError(const wxString& from )
{
   wxString buf;
	buf.Printf("Parameter needed\nFunction: %s()\n", from.c_str() );
	throw GDS_Error((char*)buf.c_str(), "Parameter Error", 9, 0);
}

bool CommandHandler::Get_Record()
{
	if (_processing_file)
		return true;
	return false;
}

// Set of Get/Set functions for handling Environment and Alias Variables
bool CommandHandler::GetEnv(const wxString& envname,wxString& envstring)
{
   char* help;

	if (envname.Len()==0)
		return false;	// I don't like NULL-pointers and A name is needed

   help=getenv(envname.c_str());
	if( help == 0 )
   {
		envstring.Empty();
		return false;	// Environment name not found
   }

	envstring=help;

	return true;
}


bool CommandHandler::SetEnv()
{
   if (_I->count() != 2)  return false;

	_statusbar->SetProcess("Setting Environment variable...");

	char* _env_ptr;
	_env_ptr = new char[LINELENGTH];
	strcpy(_env_ptr,_I->item()->c_str()); //name of variable
   (*_I)++;
	strcat(_env_ptr,"=");
	strcat(_env_ptr,_I->item()->c_str()); // value for it

	_statusbar->SetProcess();

	if( putenv(_env_ptr)!=0 )
   {
      char errbuf[LINELENGTH];
      sprintf(errbuf," SetEnv() Error: Could not set Environment variable %s = %s\n",
                        _env_ptr, _I->item()->c_str());
   	_messagehandler->info(errbuf,"BaseCommandHandler Command Execution Error.");
      return false;
   }
	return true;	// Success
}


bool CommandHandler::GetAlias(const wxString& aliasname,wxString& aliasstring)
{
	bool done;

	_statusbar->SetProcess("Getting Alias...");
	done=GLOB->ALIASLIST->GetAlias(aliasname,aliasstring);
	_statusbar->SetProcess();
	return done;
}

int CommandHandler::GetNrAlias()
{
	return GLOB->ALIASLIST->count();
}

bool CommandHandler::GetAliasNr(int i,wxString& aliasname,wxString& aliasstring)
{
	return GLOB->ALIASLIST->GetAliasNr(i, aliasname, aliasstring);
}


bool CommandHandler::SetAlias()
{
   if (_I->count() != 2)  return false;

	wxString aliasname;
	_statusbar->SetProcess("Setting Alias...");

   if (_I->count() != 2)
      return false;

	aliasname=*_I->item(); //name of alias
   (*_I)++;

	if (!GLOB->ALIASLIST->SetAlias(aliasname,*_I->item()))
   {
      char errbuf[LINELENGTH];
      sprintf(errbuf," SetAlias() Error: Could not set Alias %s = %s\n",
                        aliasname.c_str(), _I->item()->c_str());
   	_messagehandler->info(errbuf,"BaseCommandHandler Command Execution Error.");
      return false;
   }

	_statusbar->SetProcess();
	return true;	// Success
}


//	Ask file dialogs


bool CommandHandler::AskFile()
{
	wxString title;
	wxString type;
	wxString filter;
	wxString dir;
	wxString ext;
	wxString aliasname;
	wxString aliasvalue;

   if (_I->count() != 6)
      return false;

	aliasvalue.Clear();

   title=*_I->item();
   (*_I)++;
   type=*_I->item();
   (*_I)++;
   filter=*_I->item();
   (*_I)++;
   dir=*_I->item();
   (*_I)++;
   ext=*_I->item();
   (*_I)++;
   aliasname=*_I->item();
   (*_I)++;

   if (dir.Right(1)=="/")
   	dir.RemoveLast();
	if(GLOB->MAINWINDOW->AskFilename(title,type,filter,dir,aliasvalue,ext))
	{    
		wxString command = "\0";
      Evalstring astring( command, GLOB->ALIASLIST) ;
      astring.Parse(aliasvalue);
      GLOB->ALIASLIST->SetAlias( aliasname, aliasvalue);
   }
   else // Cancel was pressed...
   {
		wxString command = "\0";
      Evalstring astring( command, GLOB->ALIASLIST);
      astring.Parse(aliasvalue);
      GLOB->ALIASLIST->SetAlias(aliasname, aliasvalue);
   }
	return true;
}


// this is a command that can be called from within the processfile
// Keep it platform independed remember?
bool CommandHandler::Pause()
{
   if (_I->count() == 1)
	{
		// Your mainwindow (GDSMainWindow) must declare a Pause function!
		GLOB->MAINWINDOW->Pause(_I->item()->c_str());
	}
   else if (_I->count() == 0)
		GLOB->MAINWINDOW->Pause("press OK");
	else return false;
   return true;
}

bool CommandHandler::SetLogFile()
{
   if (_I->count() == 1)
		_messagehandler->SetLogFile((char*)_I->item()->c_str());
   else
	   _messagehandler->SetLogFile("");
   return true;
}

bool CommandHandler::SetViewer()
{
	if (_I->count() != 1)  return false;

   if (!_I->item()->CmpNoCase("true"))
      m_viewer=true;
   else
      m_viewer=false;
   return true;
}

bool CommandHandler::Ask()
{
	wxString message;
	wxString aliasname;
	wxString valueparsed;
	wxString aliasvalue;

   if (_I->count() != 2)
      return false;

   message=*_I->item();
   (*_I)++;
   aliasname=*_I->item();
   (*_I)++;

	if (GLOB->MAINWINDOW->Ask(message,aliasvalue))
   {
		wxString command = aliasvalue.c_str() + '\0';
      Evalstring astring( command, GLOB->ALIASLIST);
      astring.Parse(valueparsed);
      GLOB->ALIASLIST->SetAlias( aliasname, valueparsed);
	}
   return true;
}


// Show functions

bool CommandHandler::ShowCursorDlg()
{
   if (_I->count())  return false;
   wxCommandEvent a = wxCommandEvent();
	GLOB->MAINWINDOW->ShowCursorDlg(a);
   return true;
}

bool CommandHandler::ShowFactors()
{
   if (_I->count())  return false;
   wxCommandEvent a = wxCommandEvent();
	GLOB->MAINWINDOW->ShowFactors(a);
   return true;
}

bool CommandHandler::ShowGroups()
{
   if (_I->count())  return false;
   wxCommandEvent a = wxCommandEvent();
	GLOB->MAINWINDOW->ShowGroups(a);
   return true;
}

bool CommandHandler::ShowAlias()
{
   if (_I->count() != 1)  return false;
	GLOB->MAINWINDOW->ShowAlias(_I->item()->c_str());
   return true;
}

bool CommandHandler::ShowEnv()
{
   if (_I->count() != 1)  return false;
	GLOB->MAINWINDOW->ShowEnv(_I->item()->c_str());
   return true;
}

bool CommandHandler::ShowLayers()
{
   if (_I->count())  return false;
   wxCommandEvent a = wxCommandEvent();
	GLOB->MAINWINDOW->ShowLayers(a);
   return true;
}


bool CommandHandler::ShowOrder()
{
	if (_I->count() != 0)  return false;

   wxCommandEvent a = wxCommandEvent();
	GLOB->MAINWINDOW->ShowLayerOrder(a);
   return true;
}

bool CommandHandler::ShowInsertLayerDlg()
{
	if (_I->count() != 0)  return false;

	GLOB->MAINWINDOW->ShowInsertLayerDlg();
   return true;
}

bool CommandHandler::ShowTextDlg()
{
	if (_I->count() != 0)  return false;

   wxCommandEvent a = wxCommandEvent();
	GLOB->MAINWINDOW->ShowTextDlg(a);
   return true;
}

bool CommandHandler::ShowPathtypeDlg()
{
	if (_I->count() != 0)  return false;

	GLOB->MAINWINDOW->ShowPathtypeDlg();
   return true;
}

bool CommandHandler::ShowExecDlg()
{
	if (_I->count() != 0)  return false;

	GLOB->MAINWINDOW->ShowExecDlg();
   return true;
}

bool CommandHandler::SetContinueFlag()
{
	if (_I->count() != 1)  return false;

   if (!_I->item()->CmpNoCase("true"))
      m_continue=true;
   else
      m_continue=false;
   return true;
}

bool CommandHandler::ShowDistDlg()
{
	if (_I->count() != 0)  return false;

	GLOB->MAINWINDOW->ShowDistDlg();
   return true;
}

bool CommandHandler::ShowIdentDlg()
{
	if (_I->count() != 0)  return false;

	GLOB->MAINWINDOW->ShowIdentDlg();
   return true;
}

bool CommandHandler::ShowFilename()
{
   if (_I->count())  return false;
	char messbuf[100];
	if (_gdsii)
	{
		sprintf(messbuf, " file = %s", _filename.c_str());
	}
	else
		strcpy(messbuf," file = no_file_in_memory");
  	GLOB->MAINWINDOW->ShowString(messbuf);
   return true;
}

bool CommandHandler::ShowTopStructure()
{
   if (_I->count())  return false;
   if (_gdsii)
   {
      char buf[LINELENGTH];
      sprintf(buf,"topstructure = %s",_gdsii->GetTopStructure()->GetStructureName().c_str());
     	GLOB->MAINWINDOW->ShowString(buf);
   }
   return true;
}

bool CommandHandler::ShowHelp()
{
   if (_I->count() != 1)  return false;
	GLOB->MAINWINDOW->ShowHelp(_I->item()->c_str());
   return true;
}

bool CommandHandler::ShowKeyBNF()
{
   if (_I->count())  return false;
	GLOB->MAINWINDOW->ShowKeyBNF();
   return true;
}

bool CommandHandler::ShowProcessBNF()
{
   if (_I->count())  return false;
	GLOB->MAINWINDOW->ShowProcessBNF();
   return true;
}


// Add filename to history
bool CommandHandler::AddMenuFile()
{
	wxString type;
	wxString file;

   if (_I->count() != 2) return false;
	if (_I->item()->Len()==0) return false;
   file=*_I->item();
   (*_I)++;
	if (_I->item()->Len()==0) return false;
   type=*_I->item();

	if (!strcmp(type,"gds-ii"))
		return AddFileToHistory(file,type);
	else if (!strcmp(type,"key"))
		return AddFileToHistory(file,type);
	else if (!strcmp(type,"flash"))
		return AddFileToHistory(file,type);
	else if (!strcmp(type,"emask"))
		return AddFileToHistory(file,type);
	else if (!strcmp(type,"prs"))
		return AddFileToHistory(file,type);
	else if (!strcmp(type,"exelon"))
		return true;
	else if (!strcmp(type,"gbr"))
		return true;
	else if (!strcmp(type,"png"))
		return true;
	else if (!strcmp(type,"bmp"))
		return true;

   char errbuf[LINELENGTH];
   sprintf(errbuf," addfile(%s %s): could not determine type. (gds-ii or key or flash)\n"
            "  Syntax: addfile <filename> <type>", file.c_str(), type.c_str());
   _messagehandler->info( errbuf, "addfile error" );
   return false;
}


// Show Coordinates switch
bool CommandHandler::ShowCoords()
{
   if (_I->count() != 1)  return false;
	bool i = 0 != atoi(_I->item()->c_str());
	if ( i==true )
		GLOB->MAINWINDOW->ViewCoordinates(true);
	else
		GLOB->MAINWINDOW->ViewCoordinates(false);
	return true;
}

bool CommandHandler::SetSpline()
{
   if (_I->count() != 1)  return false;
	bool i = 0 != atoi(_I->item()->c_str());
	if ( i==true )
		m_spline=true;
	else
		m_spline=false;
	return true;
}

// gives the screendriver a message that only outlines must be drawn
// also available in processfiles!
bool CommandHandler::DrawOutline()
{
	bool i = 0 != atoi(_I->item()->c_str());
	_view_outline=i;
   GLOB->SCRDRV->SetDrawStyle(GetViewOutline()?WIREFRAME:FILLED);
	return true;
}

void CommandHandler::AddSlash(wxString& filename)
{
	if (filename.Last() != '/' &&
		 filename.Last() != '\\' )
		filename+="/";
}


void CommandHandler::StripSlash(wxString& filename)
{
	if (filename.Last() == '/' ||
		 filename.Last() == '\\' )
		filename.RemoveLast();
}


wxString CommandHandler::GetFileType()
{
	return _file_type;
}

bool CommandHandler::SetFileType(const wxString& file_type)
{
	   _file_type=file_type;
		_file_type.MakeLower();
		if (_file_type.Contains("gds-ii"))
			_file_type="gds-ii";
		else if (_file_type.Contains("key"))
			_file_type="key";
		else if (_file_type.Contains("flash"))
			_file_type="flash";
		else if (_file_type.Contains("emask"))
			_file_type="emask";
		else if (_file_type.Contains("prs"))
			_file_type="prs";
		else
      {
			_file_type="";
			GLOB->ALIASLIST->SetAlias("file_type",_file_type);
			return false;
      }
   	GLOB->ALIASLIST->SetAlias("file_type",_file_type);
		return true;
}


bool CommandHandler::SetFileEnv(const wxString& _filetoload)
{
   wxString path;
   wxString name;
   wxString ext;

	if (_filetoload.Len())
   {
	   wxSplitPath(_filetoload,&path,&name,&ext);
	   _filename=_filetoload;
		if (path.Len())
      	GLOB->ALIASLIST->SetAlias("file_path",path);
#ifndef __UNIX__
		else
      	GLOB->ALIASLIST->SetAlias("file_path","."); // wxWindows changes c:\boolean\5.3 into c:\boolean\5.3\5.3 with ./
#else
		else
      	GLOB->ALIASLIST->SetAlias("file_path","./");
#endif
		GLOB->ALIASLIST->SetAlias("filename",name + '.' + ext);
		GLOB->ALIASLIST->SetAlias("file_name",name);
		GLOB->ALIASLIST->SetAlias("file_ext",ext);

	}
	return true;

}


bool CommandHandler::SetProcEnv(const wxString& _processfile)
{
   wxString path;
   wxString name;
   wxString ext;

	if (_processfile.Len())
   {
	   wxSplitPath(_processfile,&path,&name,&ext);

		GLOB->ALIASLIST->SetAlias("proc_path",path);
		GLOB->ALIASLIST->SetAlias("procfile",name + '.' + ext);
		GLOB->ALIASLIST->SetAlias("proc_name",name);
		GLOB->ALIASLIST->SetAlias("proc_ext",ext);
		return true;
	}
	else
	{
		GLOB->ALIASLIST->SetAlias("proc_ext","prs");
		return false;
	}
}


void CommandHandler::SetupEnv()
{
	wxString namebuf;

	// Set ROOT DIR and Load default configuration
	GetEnv("GDSROOT",namebuf);

   namebuf = ConvertToUnixPath( namebuf );

	if( namebuf.Len()==0 )
	{
		namebuf=" $GDSROOT does not exist";
		_messagehandler->error((char*) namebuf.c_str(), "Initialization error: bad environment");
	}

	AddSlash(namebuf);
	GLOB->ALIASLIST->SetAlias("gdsroot",namebuf);
	namebuf+="default/config/gdsapp.cfg";
	GLOB->ALIASLIST->SetAlias("gdscfg",namebuf);

	// Load Default settings
	DoCommand("loadsetup %s",namebuf.c_str());

	// Set USER DIR and Load User configuration
	GetEnv("HOME",namebuf);
   namebuf = ConvertToUnixPath( namebuf );
	if( namebuf)
	{
		AddSlash(namebuf);
		namebuf+="gdsuser/";
		GLOB->ALIASLIST->SetAlias("gdsuser",namebuf);
	}
	else
		GetAlias("gdsuser",namebuf);
	namebuf+="config/gdsapp.cfg";

	if(wxFileExists(namebuf))
	{
		GLOB->ALIASLIST->SetAlias("gdscfg",namebuf);
		DoCommand("loadsetup %s",namebuf.c_str());
	}
	else
	{
		char errbuf[100];
		sprintf(errbuf,"Configuration file %s does not exist",namebuf.c_str());
		_messagehandler->info(errbuf,"Error");
	}

	// Load Settings
	GetAlias("setupfile",namebuf);

	if(wxFileExists(namebuf))
		DoCommand("loadsetup %s",namebuf.c_str());
	else
	{
		char errbuf[100];
		sprintf(errbuf,"Setup  file %s does not exist",namebuf.c_str());
		_messagehandler->info(errbuf,"Error");
	}

	// Load History
	GetAlias("historyfile",namebuf);
	if(wxFileExists(namebuf))
		DoCommand("loadsetup %s",namebuf.c_str());
	else
	{
		char errbuf[100];
		sprintf(errbuf,"Setup  file %s does not exist",namebuf.c_str());
		_messagehandler->info(errbuf,"Info");
	}

   _statusbar->SetInsertLayer(_setup->Get_Name(_CurrentLayer));

	// Set current-dir Alias
//	GetAlias("datadir",namebuf);
//	SetAlias("file_path",namebuf);
}

bool CommandHandler::ResetTrans()
{
   // transformation settings
   _mirror_x=false;
   // transformation settings
   _mirror_y=false;
   // transformation settings
   _move_hor=1;
   // transformation settings
   _move_ver=1;
   // transformation settings
   _scaling=1;
   // transformation settings
   _rotate=0;
   //rotation direction
   _direction=false;

   return true;
}

bool CommandHandler::KeyString()
{
   double scaling=GLOB->ENTRYUNITS();
   if (_I->count() > 2)  return false;

   // pass command to the command handler
   try
   {
      _statusbar->SetProcess("Placing Key String...");

      if (!_gdsii)
      {
      	_gdsii = new GdsII();
	      if (!_gdsii)
            return false;
			Structure* _structure = new Structure("keytop");
         _gdsii->insend(_structure);
         _gdsii->SetTopStructure(_structure);
         _gdsii->SetShowStructure(_structure);
			_gdsii->SetUserUnits(_default_acc);
			_gdsii->SetPhysicalUnits(_default_unit);
         GLOB->USERUNITS(_gdsii->GetUserUnits());
         GLOB->PHYSUNITS(_gdsii->GetPhysicalUnits());

         _filename="DUMMY.cal";
         _statusbar->SetFile(_filename);
         _statusbar->SetProcess();
      }
      // make a parser for the process file and process it
      KEY_string_in*	_keystring = new KEY_string_in(GLOB->ALIASLIST,scaling/GLOB->PHYSUNITS());
      _keystring->Insert(_gdsii,(char*)_I->item()->c_str());
		GLOB->SETUP->Set_UpdateAvailableLayers(true);
      delete _keystring;
   }
   catch (GDS_Error& _error) // error occured
   {
      _statusbar->SetProcess();
      // catch the error from the parser and throw it
      _messagehandler->info(_error.GetErrorMessage(), "Error Placing Key String");
      return false;
   }

	GLOB->MAINWINDOW->GDS_loaded(true);

   // get the boundingbox from the gdsii file
   BoundingBox bbox ;
   bbox = _gdsii->GetBoundingBox();

   // screendriver must contain this function!
   GLOB->SCRDRV->SetMappingScroll(  
                              bbox.GetMin().GetX(),
                              bbox.GetMin().GetY(),
                              bbox.GetMax().GetX(),
                              bbox.GetMax().GetY(),
                              true);
   // get the scalingfactor from the whole GDS-II structure
   _scale = GLOB->SCRDRV->GetScaleFactorX();

   return true;
}

bool CommandHandler::SetDefaultAcc()
{
   if (_I->count() != 1)  return false;
	_I->item()->ToDouble(&_default_acc);
   return true;
}

bool CommandHandler::SetDefaultUnit()
{
   if (_I->count() != 1)  return false;
	_default_unit=*_I->item();
   return true;
}



// select (point or area) (parameters x1, y1, x2, y2, type)
bool CommandHandler::Select()
{
   if (_I->count() == 5)
   {
      double x1,y1,x2,y2;
      int mode;
      _I->item()->ToDouble(&x1);
      (*_I)++;
      _I->item()->ToDouble(&y1);
      (*_I)++;
      _I->item()->ToDouble(&x2);
      (*_I)++;
      _I->item()->ToDouble(&y2);
      (*_I)++;

      if (!_I->item()->CmpNoCase("true")) mode =1;
      else if (!_I->item()->CmpNoCase("false")) mode =3;
      else
      {
         ParameterError("Select");
         return false;
      }

      Select(x1, y1, x2, y2, mode);
      return true;
   }
   else if (_I->count() == 3)
   {
      double x1,y1,x2,y2;
      int mode;

      _I->item()->ToDouble(&x1);
      x2 = x1;
      (*_I)++;
      _I->item()->ToDouble(&y1);
      y2 = y1;
      (*_I)++;
      if (!_I->item()->CmpNoCase("true")) mode =0;
      else if (!_I->item()->CmpNoCase("false")) mode =2;
      else
      {
         ParameterError("Select");
         return false;
      }


      Select(x1, y1, x2, y2, mode);
      return true;
   }
   else if (_I->count() == 2 && _I->item()->CmpNoCase("all")==0)
   {
      int mode;

      (*_I)++;
      if (!_I->item()->CmpNoCase("true")) mode =4;
      else if (!_I->item()->CmpNoCase("false")) mode =5;
      else
      {
         ParameterError("Select");
         return false;
      }
      Select(0,0,0,0, mode);
      return true;
   }
   else
   {
      ParameterError("Select");
      return false;
   }
}

// selecttype can be:
// 0 : select point
// 1 : select Box
// 2 : deselect point
// 3 : deselect Box
bool CommandHandler::Select(double vx1, double vy1, double vx2,double vy2,
													int selecttype)
{
	bool			result = false;
	Point*			p1 = new Point;
	BoundingBox* 	bbox = new BoundingBox(vx1,vy1,vx2,vy2);

	_statusbar->SetProcess("selecting...");

	// check if there is a gdsii structure on the screen
	if (!_gdsii) return false;

	p1->SetX(vx1);
	p1->SetY(vy1);

	wxTransformMatrix*	_ident = new wxTransformMatrix();

	switch (selecttype) {
		case	0:	result = NULL != _gdsii->GetShowStructure()->Select(p1,SELECT); break;
		case	1: result = NULL != _gdsii->GetShowStructure()->Select(bbox,SELECT,_ident); break;
		case	2:	result = NULL != _gdsii->GetShowStructure()->Select(p1,DESELECT); break;
		case	3: result = NULL != _gdsii->GetShowStructure()->Select(bbox,DESELECT,_ident); break;
		case	4: result = NULL != _gdsii->GetShowStructure()->Select(bbox,SELECTALL,_ident); break;
		case	5: result = NULL != _gdsii->GetShowStructure()->Select(bbox,DESELECTALL,_ident); break;
		default: throw GDS_Error("wrong selectType in Select x1 y1 x2 y2 Type ", "Parser Error", 0, 0);
	};

	_statusbar->SetProcess();

	delete p1;
	delete bbox;
	delete _ident;

	return result;
}

ElementList* CommandHandler::GetTopElementList()
{
	 return _gdsii->GetShowStructure();
}

bool CommandHandler::Tool()
{
	if (_I->count() != 1)
	   return false;
   else
   {
   	if (!_I->item()->CmpNoCase("move"))
	      GLOB->TOOLCTRL->StartTool(MOVE_TOOL,false);
		else if (!_I->item()->CmpNoCase("editproperty"))
	      GLOB->TOOLCTRL->StartTool(EDITPROPERTY_TOOL,false);
		else if (!_I->item()->CmpNoCase("insertreference"))
	      GLOB->TOOLCTRL->StartTool(INSERTREF_TOOL,false);
		else if (!_I->item()->CmpNoCase("insertstructure"))
	      GLOB->TOOLCTRL->StartTool(INSERTSTRUC_TOOL,false);
		else if (!_I->item()->CmpNoCase("copy"))
	      GLOB->TOOLCTRL->StartTool(COPY_TOOL,false);
		else if (!_I->item()->CmpNoCase("select"))
	      GLOB->TOOLCTRL->StartTool(SELECT_TOOL,false);
		else if (!_I->item()->CmpNoCase("deselect"))
	      GLOB->TOOLCTRL->StartTool(DESELECT_TOOL,false);
		else if (!_I->item()->CmpNoCase("text"))
	      GLOB->TOOLCTRL->StartTool(TEXT_TOOL,false);
		else if (!_I->item()->CmpNoCase("delete"))
	      GLOB->TOOLCTRL->StartTool(DELETE_TOOL,false);
		else if (!_I->item()->CmpNoCase("polygon"))
	      GLOB->TOOLCTRL->StartTool(POLYGON_TOOL,false);
		else if (!_I->item()->CmpNoCase("polyline"))
	      GLOB->TOOLCTRL->StartTool(POLYLINE_TOOL,false);
		else if (!_I->item()->CmpNoCase("circle"))
	      GLOB->TOOLCTRL->StartTool(CIRCLE_TOOL,false);
		else if (!_I->item()->CmpNoCase("circle2"))
	      GLOB->TOOLCTRL->StartTool(CIRCLE2_TOOL,false);
		else if (!_I->item()->CmpNoCase("rectangle"))
	      GLOB->TOOLCTRL->StartTool(RECTANGLE_TOOL,false);
		else if (!_I->item()->CmpNoCase("zoom"))
	      GLOB->TOOLCTRL->StartTool(ZOOMIN_TOOL,false);
		else if (!_I->item()->CmpNoCase("edit"))
	      GLOB->TOOLCTRL->StartTool(EDIT_TOOL,false);
		else if (!_I->item()->CmpNoCase("end"))
	      GLOB->TOOLCTRL->EndTool(true);
		else if (!_I->item()->CmpNoCase("stop"))
	      GLOB->TOOLCTRL->StopTool();
      else
         return false;
   }
   return true;
}

bool CommandHandler::EndCommand()
{
	if (_I->count() == 0 )
      GLOB->TOOLCTRL->EndTool(true);
   else
      return false;
   return true;
}

bool CommandHandler::UndoCommand()
{
	static ToolSet undo_for_tool;

	if (_I->count() != 0)
      return false;

   if (GLOB->TOOLCTRL->Undo())
      return true;
   if (_commandhistory->empty())
      return true;

   Command* undo_command=_commandhistory->tailitem();
   _commandhistory->removetail();


      if (!strcmp(undo_command->Get_Keyword(), "stop"))
      {  //this was the last stopped tool
         undo_for_tool=undo_command->Get_Tool();
      }
      else if (!strcmp(undo_command->Get_Keyword(),"copy"))
      {
         TDLI<Element> olditer=TDLI<Element>(GetTopElementList());
         olditer.tohead();
         while (!olditer.hitroot())
         {
            if (olditer.item()->GetStamp()==undo_command->Get_EventStamp()) //_eventstamp
            {
               delete olditer.item();
               olditer.remove();
            }
            else
               olditer++;
         }
         GLOB->SCRDRV->DestroyClippingRegion();
         DoCommand("draw");
         delete undo_command;
      }
      else if (!strcmp(undo_command->Get_Keyword(),"move"))
      {
         {
            TDLI<Element> olditer=TDLI<Element>(GetTopElementList());
            olditer.tohead();
            while (!olditer.hitroot())
            {
               if (olditer.item()->GetStamp()==undo_command->Get_EventStamp()) //_eventstamp
               {
                  delete olditer.item();
                  olditer.remove();
               }
               else
                  olditer++;
            }
         }
         GetTopElementList()->takeover(undo_command->Get_ElementList());
         delete undo_command->Get_ElementList();
         GetTopElementList()->CalcBoundingBox();
         _gdsii->CalcBoundingBox();
         GLOB->SCRDRV->DestroyClippingRegion();
         DoCommand("draw");
         delete undo_command;
      }
      else if (!strcmp(undo_command->Get_Keyword(),"delete"))
      {
         GetTopElementList()->takeover(undo_command->Get_ElementList());
         delete undo_command->Get_ElementList();
         GetTopElementList()->CalcBoundingBox();
         _gdsii->CalcBoundingBox();
         GLOB->SCRDRV->DestroyClippingRegion();
         DoCommand("draw");
         delete undo_command;
      }
      else if (!strcmp(undo_command->Get_Keyword(),"addreference") ||
      			!strcmp(undo_command->Get_Keyword(),"addstructure") ||
      			!strcmp(undo_command->Get_Keyword(),"addtext")
              )
      {
            TDLI<Element> olditer=TDLI<Element>(GetTopElementList());
            olditer.tohead();
            while (!olditer.hitroot())
            {
               if (olditer.item()->GetStamp()==undo_command->Get_EventStamp()) //_eventstamp
               {
                  delete olditer.item();
                  olditer.remove();
               }
               else
                  olditer++;
            }
            GetTopElementList()->CalcBoundingBox();
            _gdsii->CalcBoundingBox();
	         GLOB->SCRDRV->DestroyClippingRegion();
            DoCommand("draw");
            delete undo_command;
      }
      else if (!strcmp(undo_command->Get_Keyword(),"end"))
      {
         if (
         		(undo_command->Get_Tool()==EDIT_TOOL) ||
         		(undo_command->Get_Tool()==EDITPOLYGON_TOOL) ||
         		(undo_command->Get_Tool()==EDITPOLYLINE_TOOL) ||
         		(undo_command->Get_Tool()==EDITREC_TOOL) ||
         		(undo_command->Get_Tool()==EDITCIRCLE_TOOL)
         	)
	      {
            {
               TDLI<Element> olditer=TDLI<Element>(GetTopElementList());
               olditer.tohead();
               while (!olditer.hitroot())
               {
                  if (olditer.item()->GetStamp()==undo_command->Get_EventStamp()) //_eventstamp
                  {
                     delete olditer.item();
                     olditer.remove();
                  }
                  else
                     olditer++;
               }
            }
            GetTopElementList()->insend(undo_command->Get_Element());
            GetTopElementList()->CalcBoundingBox();
            _gdsii->CalcBoundingBox();
	         GLOB->SCRDRV->DestroyClippingRegion();
            DoCommand("draw");
            delete undo_command;
   		}
      	else
         {
               TDLI<Element> olditer=TDLI<Element>(GetTopElementList());
               olditer.tohead();
               while (!olditer.hitroot())
               {
                  if (olditer.item()->GetStamp()==undo_command->Get_EventStamp()) //_eventstamp
                  {
                     delete olditer.item();
                     olditer.remove();
                  }
                  else
                     olditer++;
               }
		         GLOB->SCRDRV->DestroyClippingRegion();
               DoCommand("draw");
               delete undo_command;
         }
      }
      else
         UndoCommand();
   return true;
}

bool CommandHandler::Move()
{
   return true;
}

bool CommandHandler::Delete()
{
   _operationmethod=SELECTED_ELEMENTS;
	SetOperationFlags(true);

   _gdsii->Delete(ELE_GR_C);

	_statusbar->SetProcess();

	// This checks the memory structures for available layers (with content)
	GLOB->SETUP->Set_UpdateAvailableLayers(true);

	return true;
}

bool CommandHandler::MakeStructure()
{
	if (_I->count() != 1)
	   return false;

   wxString tmp = *_I->item();
   _gdsii->MakeStructure( tmp );

	_statusbar->SetProcess();
	return true;
}

bool CommandHandler::DeSelect()
{
	return Select();
}


bool CommandHandler::Begin()
{
	if (_I->count()== 0)
      GLOB->TOOLCTRL->Begin();
	else
      return false;
   return true;
}

bool CommandHandler::End()
{
	if (_I->count()== 0)
      GLOB->TOOLCTRL->End();
	else
      return false;
   return true;
}

bool CommandHandler::SetCursor()
{
	if (_I->count() == 2)
   {
      wxUnit x,y;
   	x=*_I->item();
      (*_I)++;
   	y=*_I->item();
      if (GLOB->TOOLCTRL->GetActiveTool())
	      GLOB->TOOLCTRL->GetActiveTool()->SetCursor(x.GetValue()/GLOB->PHYSUNITS(),y.GetValue()/GLOB->PHYSUNITS());
   }
	else
      return false;
   return true;
}

bool CommandHandler::AddPoint()
{
	if (_I->count() == 2)
   {
      double x,y;
      _I->item()->ToDouble(&x);
      (*_I)++;
      _I->item()->ToDouble(&y);
      if (GLOB->TOOLCTRL->GetActiveTool())
	      GLOB->TOOLCTRL->GetActiveTool()->AddPoint(x,y);
   }
	else
      return false;
   return true;
}

// Draw the structures on the screen
// pass message to the screendriver
bool CommandHandler::Draw()
{
	// mainwindow must contain a Statusbar object!
	_statusbar->SetProcess("Drawing structures...");

	if (_gdsii)
	{
      if (GLOB->SETUP->Get_UpdateAvailableLayers())
      {
         _gdsii->SetAvailable();
      	GLOB->SETUP->Set_UpdateAvailableLayers(false);
      }

      GLOB->CANVAS->Update();
      GLOB->TOOLCTRL->Draw();
	}
	else
	{
      if (GLOB->SETUP->Get_UpdateAvailableLayers())
      {
      	GLOB->SETUP->Set_UpdateAvailableLayers(false);
      }
		ClearScreen();
		// there is no GDS-II structure loaded!  (or not yet)
	}
	_statusbar->SetProcess();
	return true;
}


// just clears the screen -> passed to the screendriver
// a color can be given in this format: 00RRGGBB hex
// R = Red, G = Green and B = blue
// R, G and B can vary between 00 and FF
// 0x00000000 = black and 0x00FFFFFF is white
bool CommandHandler::ClearScreen()
{
	GLOB->CANVAS->SetBackGroundColor(_back_colour);
/*
   wxClientDCbuf temp(GLOB->CANVAS);
   GLOB->SCRDRV->SetDC(&temp);
 	GLOB->SCRDRV->Clear();
   GLOB->SCRDRV->SetDC(0);
*/
	return true;
}

wxColour	CommandHandler::ltowxc(long l)
{
	unsigned char	red,green,blue;

	red 	= (unsigned char)(l / 65536);
	green = (unsigned char)((l % 65536) / 256);
	blue 	= (unsigned char)(l % 256);

   return wxColour(red,green,blue);
}

// when zooming a stack with old zooms is created. Make this empty
// so the user can't zoom back anymore
void CommandHandler::EmptyZoomHistory()
{
	TDLIStack<BoundingBox> I(_zoom_history);
	while (!I.empty())
	{
		delete I.pop();
	}
}


// zoom in (parameters x1, y1, x2, y2)
bool CommandHandler::Zoom()
{
	if (_I->count() != 4)
		ParameterError("Zoom");

   double x1,y1,x2,y2;

   _I->item()->ToDouble(&x1);
   (*_I)++;
   _I->item()->ToDouble(&y1);
   (*_I)++;
   _I->item()->ToDouble(&x2);
   (*_I)++;
   _I->item()->ToDouble(&y2);

	Zoom(x1, y1, x2, y2, 1);

  	return true;
}

// zoomout
bool CommandHandler::ZoomOut()
{
	Zoom(0, 0, 0, 0, 0);
   Draw();
	return true;
}

// zoomback
bool CommandHandler::ZoomBack()
{
	Zoom(0, 0, 0, 0, 2);
   Draw(); 
	return true;
}

// zoomback
bool CommandHandler::ZoomOut2()
{
	Zoom(0, 0, 0, 0, 3);
   Draw();
	return true;
}

// status can be:
// 0 : Zoom out
// 1 : Zoom in
// 2 : Zoom back one level
double CommandHandler::Zoom(double vx1,
													double vy1,
													double vx2,
													double vy2,
													int zoomin)
{
	_statusbar->SetProcess("Zooming...");

	float scale;

	// check if there is a gdsii structure on the screen
	if (!_gdsii)
		return 1;

	// border is used for a total view.
	// it prevents that the outlines are drawn on the edge of the screen
	bool border = false;
	BoundingBox bbox;
	TDLIStack<BoundingBox> I(_zoom_history);

	if (zoomin == 0) // zoom out
	{
      bbox = _gdsii->GetShowStructure()->GetBoundingBox();
      border = true;
      vx1 = bbox.GetMin().GetX();
      vx2 = bbox.GetMax().GetX();
      vy1 = bbox.GetMin().GetY();
      vy2 = bbox.GetMax().GetY();

      // empty the zoom history
      while (!I.empty())
      {
         delete I.pop();

      }
	}
	else if (zoomin == 3) // zoom out 2*
	{
         bbox = GLOB->SCRDRV->GetVirtualSizeScreen();
         border = true;
         double dx=bbox.GetMax().GetX()-bbox.GetMin().GetX();
         double dy=bbox.GetMax().GetY()-bbox.GetMin().GetY();
         vx1 = bbox.GetMin().GetX()-dx;
         vx2 = bbox.GetMax().GetX()+dx;
         vy1 = bbox.GetMin().GetY()-dy;
         vy2 = bbox.GetMax().GetY()+dy;
         if (!I.empty())
            I.push(new BoundingBox((GLOB->SCRDRV->GetVirtualSizeScreen())));
         else
            I.push(new BoundingBox((_gdsii->GetShowStructure()->GetBoundingBox())));
	}
	else if (zoomin == 1) // zoom in
	{
		scale = GLOB->SCRDRV->GetScaleFactorX() / _scale;
		if( scale >= 1.0E+10) return scale;
		// fill the zoomhistory here
		if (!I.empty())
			I.push(new BoundingBox((GLOB->SCRDRV->GetVirtualSizeScreen())));
		else
			I.push(new BoundingBox((_gdsii->GetShowStructure()->GetBoundingBox())));
	}
	else // zoom back one level
	{
		if (!I.empty())
		{
			// with or without border?
			if (I.count() == 1)
				// top zoom: show with border
				border = true;
			else
				border = false;
			// get the last zoom boundingbox from the list
         BoundingBox* bbox;
			bbox = I.pop();
			vx1 = bbox->GetMin().GetX();
			vx2 = bbox->GetMax().GetX();
			vy1 = bbox->GetMin().GetY();
			vy2 = bbox->GetMax().GetY();

			// now remove thiszoom from the history
			delete bbox;
		}
      else
      {
         BoundingBox bbox;
         bbox = _gdsii->GetShowStructure()->GetBoundingBox();
         border = true;
         vx1 = bbox.GetMin().GetX();
         vx2 = bbox.GetMax().GetX();
         vy1 = bbox.GetMin().GetY();
         vy2 = bbox.GetMax().GetY();
      }
	}

	// Set the new mapping
   GLOB->SCRDRV->SetMappingScroll(vx1, vy1, vx2, vy2, border);

	// set the new scale on the Statusbar
	scale = GLOB->SCRDRV->GetScaleFactorX() / _scale;
	_statusbar->SetZoom(scale);

	// redraw after a zoom is done in mainwin.cpp

	// return the scalefactor
	return scale;
}

// let the Window Manager handle this
// this is platform depended -> so let the mainwindow handle this
bool CommandHandler::PrintFile()
{
	// set the statusbar
	_statusbar->SetProcess("Printing...");
	GLOB->MAINWINDOW->CmFilePrint();
	_statusbar->SetProcess();
	return true;
}

int CommandHandler::Get_ZoomHistoryEmpty()
{
	TDLIStack<BoundingBox> I(_zoom_history);
	return I.empty();
}

double CommandHandler::Get_ScaleFactor()
{
	return GLOB->SCRDRV->GetScaleFactorX() / _scale;
}

bool CommandHandler::IsZoomed()
{
	TDLIStack<BoundingBox> I(_zoom_history);
	if (I.empty())
		return true;
	return false;
}

// view from a given structure
bool CommandHandler::ViewStructure()
{
	if (_I->item()->Len()==0)
		ParameterError("ViewStructure");

	if (!_gdsii)
		throw GDS_Error("No GDSII structure opened yet.", "ViewStructure Error", 9, 0);

	// if all specified on the commandline >> draw whole GDS-II file
	if (!_I->item()->CmpNoCase("all"))
	{
		View_Structure(0);
		return true;
	}
	else
	{
		TDLI<Structure> I(Get_GDSII());
		I.toroot();
		while (I.iterate())
		{
			if (!_I->item()->CmpNoCase(I.item()->GetStructureName()))
			{
				View_Structure(I.item());
				return true;
			}
		}
	}
	return true;
}


int CommandHandler::View_Structure(Structure* structure)
{
	// is there something loaded?
	if (!_gdsii)
		return -1;

	// make zoomhistory empty
	EmptyZoomHistory();

   //clear all flags since they are not valid anymore
   _gdsii->ClearFlags(ELE_ALL  ^ ELE_FILLED); //clear all flags

	if (structure)
		_gdsii->SetShowStructure(structure);
	else
		_gdsii->SetShowStructure(_gdsii->GetTopStructure());

	BoundingBox bbox = _gdsii->GetShowStructure()->GetBoundingBox();
   GLOB->SCRDRV->SetMappingScroll(
										bbox.GetMin().GetX(),
										bbox.GetMin().GetY(),
										bbox.GetMax().GetX(),
										bbox.GetMax().GetY(),
										true);
	Draw();
	return 0;
}

bool CommandHandler::ShowGrid()
{
	_show_grid = 0 != atoi(_I->item()->c_str());
   return true;
}

bool CommandHandler::ScrollScreen()
{
	_statusbar->SetProcess("Scrolling...");

	// get the boundingbox from the gdsii file
	BoundingBox bbox;
	bbox = GLOB->SCRDRV->GetVirtualSizeScreen();
	double vx1, vy1, vx2, vy2;
	vx1 = bbox.GetMin().GetX();
	vx2 = bbox.GetMax().GetX();
	vy1 = bbox.GetMin().GetY();
	vy2 = bbox.GetMax().GetY();

	// calculate the change in the coordinates
	double dx, dy;

   if (*_I->item()=="ctrl")
   {
      dx  = (vx2-vx1)*25/100 ;
      dy  = (vy2-vy1)*25/100 ;
      (*_I)++;
   }
   else
   {
      dx  = (vx2-vx1)*5/100 ;
      dy  = (vy2-vy1)*5/100 ;
   }

	if (!_I->item()->CmpNoCase("right"))
	{
	   GLOB->CANVAS->ScrollIt(dx,false);
   }
	else if (!_I->item()->CmpNoCase("left"))
	{
	   GLOB->CANVAS->ScrollIt(-dx,false);
   }
	else if (!_I->item()->CmpNoCase("up"))
	{
	   GLOB->CANVAS->ScrollIt(dy,true);
   }
	else if (!_I->item()->CmpNoCase("down"))
	{
	   GLOB->CANVAS->ScrollIt(-dy,true);
   }
	else if (!_I->item()->CmpNoCase("pageup"))
	{
	   GLOB->CANVAS->ScrollIt((vy2-vy1),true);
   }
	else if (!_I->item()->CmpNoCase("pagedown"))
	{
	   GLOB->CANVAS->ScrollIt(-(vy2-vy1),true);
   }
	else return true;

//	_statusbar->SetProcess();
//   Draw();
   return true;
}


bool CommandHandler::ShowBrowser()
{
	wxString namebuf;
	GetAlias("gdsroot",namebuf);

   if (_I->count() == 0)
   {
#ifndef __UNIX__
	namebuf+="boolean_html\\topframe.html";
   _ddehandler->startLoadingWin(namebuf);
#else
	namebuf+="boolean_html/topframe.html";
   _ddehandler->startLoadingUnix(namebuf);
#endif
	}
   else if (_I->count() == 1)
   {
#ifndef __UNIX__
	namebuf+="boolean_html\\commands.html#T_";
   namebuf+=*_I->item();
   _ddehandler->startLoadingWin(namebuf);
#else
	namebuf+="boolean_html/commands.html#T_";
   namebuf+=*_I->item();
   _ddehandler->startLoadingUnix(namebuf);
#endif
	}
 	return true;
}

bool CommandHandler::AddFileToHistory(const wxString& file,const wxString& type)
{
   Alias* a = new Alias(file,type);

   //if the files is not yet in the history insert it.
   DL_Iter<Alias*> I(_file_history);
   I.toroot();
   while (I.iterate() && strcmp(I.item()->GetAlias(),file) );

   if (I.hitroot())
   { //it is not yet in the list
      I.insbegin(a);
   }
   else
	   delete a; //Solve memory leak

   while (I.count() > MAXHISTORY)
   {
         I.totail();
         delete I.item();
         I.remove();
   }
   GLOB->MAINWINDOW->UpdateHistoryMenu();
   return true;
}

bool CommandHandler::SaveHistory()
{
		// holds the loaded files in the filemenu
      //	IB.item()->GetAlias() holds the filename
      //	IB.item()->GetValue() holds the filetype (i.e. "key", "cal" or "gds")
		DL_List<Alias*>* _file_history_bak = new DL_List<Alias*>();
      {
         DL_Iter<Alias*> I(_file_history_bak);
         {
            {  //duplicate the history
               DL_Iter<Alias*> IB(_file_history);
               while (IB.iterate())
                  I.insbegin(new Alias(IB.item()->GetAlias(),IB.item()->GetValue()));
            }

            wxString buf;
            GetAlias("historyfile",buf);
            DoCommand("record  %s",buf.c_str());
            I.toroot();
            while (I.iterate())
               DoCommand("addfile  {%s} %s",I.item()->GetAlias().c_str(),I.item()->GetValue().c_str());
            DoCommand("record");

            I.tohead();
            while (!I.hitroot())
            {
                delete I.item();
                I.remove();
            }
         }
      }
      delete _file_history_bak;
      return true;
}

void CommandHandler::SetOperationFlags(bool explode)
{
   //explode is not always needed, for instance if transforming it
   //will only be for the topelement list and there is no need to explode
   if (explode)
   {
      //first explode selected references to make them unique
      if (_operationmethod == GROUP_LAYER)
      {
         if (_group_A->GetOnlySelected() || _group_B->GetOnlySelected())
         {
            //explode selected references to make them unique
            _gdsii->Explode(_selected_only);
         }
      }
      else
      {  //operation is on selected elements,
         //explode selected references to make them unique
         _gdsii->Explode(_selected_only);
      }

   }

   //if the groupmethod is GROUP_LAYER the group flags for the elements need to be set now
   if (_operationmethod == GROUP_LAYER)
   {  //set the group flags of all elements according to the groups of layers
      _gdsii->ClearFlags(ELE_GR_AB); //clear all GR_AB flags
      _gdsii->SetGroupFlags(_group_A,ELE_GR_A);
      _gdsii->SetGroupFlags(_group_B,ELE_GR_B);
   }
   else if (_operationmethod == GROUP_SELECTED)
   {  //set the group flags of all reference elements according to its group settings
      // in the top element list the element all ready have their group flag set (A or B group)
      //now takeover those settings to the elements in the referenced structures
      //because at this moment those references are unique
      _gdsii->SetGroupFlagsRefs(ELE_GR_A,false);
      _gdsii->SetGroupFlagsRefs(ELE_GR_B,false);
   }
   else
   {
      _gdsii->ClearFlags(ELE_GR_C); //clear all GR_C flags
      _gdsii->SetFlags(ELE_GR_C,_selected_only); //set (selected) to group C
      //set the group flags of all reference elements according to its group settings
      // in the top element list the element all ready have their group flag set (C group)
      //now takeover those settings to the elements in the referenced strcutures
      //because at this moment those references are unique
      _gdsii->SetGroupFlagsRefs(ELE_GR_C,false);
   }
}

Handler::Handler(const wxString& functie,lfp fpoint)
{
	keyword=functie;
   fpointer = fpoint;
}

Handler::~Handler()
{
}


