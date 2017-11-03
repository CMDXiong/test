/*
Program	GDSII.CPP
Purpose	Definition of GDSII structure
Programmer	R. Spekreijse
Last Update	9-4-1998, M. Reemst
*/


#ifdef __GNUG__
#pragma implementation
#endif

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

#include <stdarg.h>
#include "gdsii.h"


#include "lsetup.h"
#include "elemlst.h"
#include "referenc.h"
#include "element.h"
#include "units.h"
#include "structur.h"
#include "drivout.h"
#include "scrndrv.h"




// these function will be called from the drivers
// set functions when reading GDS-II
// get functions when writing GDS-II


// ModeTime -------------------------------------------------------------------
// all the get functions return an integer (two byte)
// from a date or a time
ModeTime::ModeTime()
{
	time_t timer;
	struct tm * tblock;
	timer = time(NULL);
	tblock = localtime(&timer);

	_lmt_year=tblock->tm_year;
	_lmt_month=tblock->tm_mon+1;
	_lmt_day=tblock->tm_mday;
	_lmt_hour=tblock->tm_hour;
	_lmt_minute=tblock->tm_min;
	_lmt_second=tblock->tm_sec;

	_lat_year=tblock->tm_year;
	_lat_month=tblock->tm_mon+1;
	_lat_day=tblock->tm_mday;
	_lat_hour=tblock->tm_hour;
	_lat_minute=tblock->tm_min;
	_lat_second=tblock->tm_sec;
}

int	ModeTime::GetLmtYear()
{
	return _lmt_year;
}

int	ModeTime::GetLmtMonth()
{
	return _lmt_month;
}

int	ModeTime::GetLmtDay()
{
	return _lmt_day;
}

int	ModeTime::GetLmtHour()
{
	return _lmt_hour;
}

int	ModeTime::GetLmtMinute()
{
	return _lmt_minute;
}

int	ModeTime::GetLmtSecond()
{
	return _lmt_second;
}

int	ModeTime::GetLatYear()
{
	return _lat_year;
}

int	ModeTime::GetLatMonth()
{
	return _lat_month;
}

int	ModeTime::GetLatDay()
{
	return _lat_day;
}

int	ModeTime::GetLatHour()
{
	return _lat_hour;
}

int	ModeTime::GetLatMinute()
{
	return _lat_minute;
}

int	ModeTime::GetLatSecond()
{
	return _lat_second;
}

// all the set functions sets an integer (two byte)
// from a date or a time
void	ModeTime::SetLmtYear(int new_value)
{
	_lmt_year = new_value;
}

void	ModeTime::SetLmtMonth(int new_value)
{
	_lmt_month = new_value;
}

void	ModeTime::SetLmtDay(int new_value)
{
	_lmt_day = new_value;
}

void	ModeTime::SetLmtHour(int new_value)
{
	_lmt_hour = new_value;
}

void	ModeTime::SetLmtMinute(int new_value)
{
	_lmt_minute = new_value;
}

void	ModeTime::SetLmtSecond(int new_value)
{
	_lmt_second = new_value;
}

void	ModeTime::SetLatYear(int new_value)
{
	_lat_year = new_value;
}

void	ModeTime::SetLatMonth(int new_value)
{
	_lat_month = new_value;
}

void	ModeTime::SetLatDay(int new_value)
{
	_lat_day = new_value;
}

void	ModeTime::SetLatHour(int new_value)
{
	_lat_hour = new_value;
}

void	ModeTime::SetLatMinute(int new_value)
{
	_lat_minute = new_value;
}

void	ModeTime::SetLatSecond(int new_value)
{
	_lat_second = new_value;
}

ModeTime& ModeTime::operator=(ModeTime& other)
{
	_lmt_year=other._lmt_year;
	_lmt_month=other._lmt_month;
	_lmt_day=other._lmt_day;
	_lmt_hour=other._lmt_hour;
	_lmt_minute=other._lmt_minute;
	_lmt_second=other._lmt_second;

	_lat_year=other._lat_year;
	_lat_month=other._lat_month;
	_lat_day=other._lat_day;
	_lat_hour=other._lat_hour;
	_lat_minute=other._lat_minute;
	_lat_second=other._lat_second;
	return *this;
}



// Library ------------------------------------------------------------------
Library::Library()
{
   _filename="DUMMY.cal";

	_libraryname="dummy";

}


Library::~Library()
{
}


Library& Library::operator=(Library &other)
{
	_userunits = 0.0;
	_physunits = 0.0;
	_fileopened = other._fileopened;
	_filehandle = other._filehandle;
	_version = other._version;

   _filename=other._filename;
   _fontsname=other._fontsname;
	_libraryname=other._libraryname;

	_userunits = other._userunits;
	_physunits = other._physunits;
	return *this;
}

double Library::GetUserUnits()
{
	return _userunits;
}

double Library::GetPhysicalUnits()
{
	return _physunits;
}

void Library::SetUserUnits(double userunits)
{
	_userunits = userunits;
}

void Library::SetPhysicalUnits(double physunits)
{
	_physunits = physunits;
}

void Library::SetFilename(const wxString& name)
{
	_filename = name;
}


void Library::SetFontsname(const wxString& name)
{
	_fontsname = name;
}

wxString Library::GetFilename()
{
	return _filename;
}

wxString Library::GetFontsname()
{
	return _fontsname;
}

void Library::SetVersion(int nr)
{
	_version = nr;
}

int Library::GetVersion()
{
	return _version;
}


wxString Library::GetLibraryName()
{
	return _libraryname;
}

void Library::SetLibraryName(const wxString& l)
{
	_libraryname=l;
}




// GdsII --------------------------------------------------------------------
GdsII::GdsII()
{
	_bgnlib        = new ModeTime();
	_library		   = new Library();
	_topstructure  = 0;
  	_trans_matrix 	= new wxTransformMatrix();

   m_multipletop  = false;

	if (!_bgnlib || !_library )
		throw GDS_Error("Out of memory!\n\nCannot create object GDSII",
			"Fatal Creation Error", 0, 1);

  	TDLI<Structure> _SI=TDLI<Structure>(this);
	//first empty the element list
	_SI.delete_all();

}


//
// destructor
// free all structures
//
GdsII::~GdsII()
{
  	TDLI<Structure> freemem=TDLI<Structure>(this);
	freemem.tohead();
    while (!freemem.hitroot())
      {
          delete freemem.item();
          freemem.remove();
      }

	delete _bgnlib;
	delete _library;
   delete _trans_matrix;
	_topstructure = 0;
}

void GdsII::Write(DrawDriver& dr)
{
	OVERLAP clip_status;

   double x,y,w,h;
   dr.GetClippingBox(x, y, w, h) ;
   BoundingBox clippingregion(x,y,x+w,y+h);
   clip_status = clippingregion.Intersect(_showstructure->GetBoundingBox());

	if (clip_status == _OUT) return;
   dr.SetClipStatus(clip_status);
   dr.SetBusyDrawing(true);

   wxTransformMatrix ident;
   dr.SetMatrix(ident);

   dr.SetParentSelect(false);

	// begin drawing the first structure if any
	if (_showstructure)
   {
      dr.SetLayer(-1);
      //setup an array with the layer order as index
		int	layerorder[MAX_LAYERS];
		for (int layer=0; layer < MAX_LAYERS; layer++)
      {  int curorder=GLOB->SETUP->Get_Order(layer);
         if (curorder < 0) //wrong order number
         {
            char buf[80];
            sprintf(buf,"Order for Layer %d, must be >=0 and < %d set to 0",layer,MAX_LAYERS);
				_messagehandler->info(buf, "Error");
				layerorder[0] = layer;
         }
         else if (curorder >= MAX_LAYERS) //wrong order number
         {
            char buf[80];
            sprintf(buf,"Order for Layer %d, must be >=0 and < %d set to MAX_LAYERS",layer,MAX_LAYERS);
				_messagehandler->info(buf, "Error");
				layerorder[MAX_LAYERS-1] = layer;
         }
         else
				layerorder[curorder] = layer;
      }

		if (dr.GetDrawStyle() != FILLED)
   		_showstructure->Write(dr);
		else
      {
			for (int order = MAX_LAYERS-1; order >= 0; order--)
         {
				dr.SetActiveLayer(layerorder[order]);
				if (GLOB->SETUP->Get_Available(layerorder[order])&&
                GLOB->SETUP->Get_Visible(layerorder[order])  &&
                !dr.GetStopDrawing()
               )
					_showstructure->Write(dr);
			}
		}
 	} else throw GDS_Error("No Structures in this GDS-II file", "Screendriver warning", 5, 0);

   dr.SetBusyDrawing(false);
}

ModeTime* GdsII::GetBgnlib()
{
	return _bgnlib;
}

int	GdsII::GetVersion()
{
	return _version;
}
void GdsII::SetVersion(int a)
{
	_version=a;
}


BoundingBox& GdsII::CalcBoundingBox()
{
	_boundingbox.Reset();
	_boundingbox.SetValid(true);
	// Calculate through the GdsII structure if at least 1 structure is available
	if (_topstructure)
	{
		_boundingbox = _topstructure->CalcBoundingBox();
	}
	return _boundingbox;
}


BoundingBox& GdsII::GetBoundingBox()
{
	assert (_boundingbox.GetValid());
	return _boundingbox;
}

Library* GdsII::GetLibrary()
{
	return _library;
}

double GdsII::GetUserUnits()
{
	return _library->GetUserUnits();
}

double GdsII::GetPhysicalUnits()
{
	return _library->GetPhysicalUnits();
}

void	 GdsII::SetUserUnits(double a)
{
  _library->SetUserUnits(a);
}

void	 GdsII::SetPhysicalUnits(double a)
{
  _library->SetPhysicalUnits(a);
}

Structure *GdsII::GetTopStructure()
{
	return _topstructure;
}

void GdsII::SetBgnlib(ModeTime *new_value)
{
	_bgnlib = new_value;
}

void GdsII::SetLibrary(Library *new_value)
{
	if (_library)
		delete _library;
	_library = new_value;
}

void GdsII::SetTopStructure(Structure *a_structure)
{
	_topstructure = a_structure;
	_boundingbox.SetValid(false);
}

void GdsII::Merge(GdsII* other_gdsii,double x,double y,double angle)
{
   TDLI<Structure> I2=TDLI<Structure>(other_gdsii);

   I2.toroot();
	while (I2.iterate())
	{
   	if (GetStructure(I2.item()->GetStructureName()))
      { // rename all structures from I2 that exist in I1
         I2.item()->Rename(other_gdsii);
			other_gdsii->LinkReferences();
         other_gdsii->SetTopStructure(other_gdsii->SearchTopStructure());
         other_gdsii->SetShowStructure(other_gdsii->SearchTopStructure());
         I2.toroot();
      }
	}

   I2.toroot();
	while (I2.iterate())
   {
		insend(I2.item()); // put all items from I2 in I1
      I2.remove();
      I2--;
   }

  	StructureReference* _Sref2 = new StructureReference();
	_Sref2->SetStructureReferenceName(other_gdsii->GetTopStructure()->GetStructureName());
	_Sref2->SetStructure(other_gdsii->GetTopStructure());


   wxTransformMatrix*_relative_matrix = new wxTransformMatrix();

   _relative_matrix->Rotate(angle, 0, 0);
   _relative_matrix->Scale(1, 1, 0, 0);
   // Translate2D over XY from the structrereference
   _relative_matrix->Translate(x,y);

	_Sref2->SetRelativeMatrix(_relative_matrix);
   delete _relative_matrix;

	_topstructure->insbegin(_Sref2);

	LinkReferences();
   SetTopStructure(SearchTopStructure());

   SetShowStructure(GetTopStructure());
   SetReferenceNr();
   CalcBoundingBox();
}

void GdsII::Replace(const wxString& to_replace,GdsII* other_gdsii)
{
   TDLI<Structure> I2=TDLI<Structure>(other_gdsii);

   I2.toroot();
	while (I2.iterate())
	{
   	if (GetStructure(I2.item()->GetStructureName()))
      { // rename all structures from I2 that exist in I1
         I2.item()->Rename(other_gdsii);
			other_gdsii->LinkReferences();
         other_gdsii->SetTopStructure(other_gdsii->SearchTopStructure());
         other_gdsii->SetShowStructure(other_gdsii->SearchTopStructure());
         I2.toroot();
      }
	}

   I2.toroot();
	while (I2.iterate())
   {
		insend(I2.item()); // put all items from I2 in I1
      I2.remove();
      I2--;
   }

   TDLI<Structure> I1=TDLI<Structure>(this);

   //get the structure to replace and delete it
	I1.tohead();
	while (!I1.hitroot())
   {
		if (I1.item()->GetStructureName().Cmp(to_replace)==0)
      {
			 delete I1.item();
          I1.remove();
      }
      else
          I1++;
   }

   other_gdsii->GetTopStructure()->SetStructureName(to_replace);

	LinkReferences();
   SetTopStructure(SearchTopStructure());

   SetShowStructure(GetTopStructure());
   SetReferenceNr();
   CalcBoundingBox();
}

Structure *GdsII::GetShowStructure()
{
	return _showstructure;
}

void GdsII::SetShowStructure(Structure *a_structure)
{
	_showstructure = a_structure;
}

void GdsII::ShowInformation()
{
	int count_top_structures = 0;

	if (!empty())
	{
	  	TDLI<Structure> _SI=TDLI<Structure>(this);
		cout << "This GDSfile contains the following structures: " << endl << endl;
		_SI.toroot();
		while (_SI.iterate())
		{
			cout << _SI.item()->GetStructureName();
			if (_SI.item()->GetReferenced() == 0)
			{
				cout << "\t\t(top)";
				count_top_structures++;
			}
			else
				cout << "\t\t (" << _SI.item()->GetReferenced() << ")";
			cout << endl;
		}

		cout << endl;

		if (count_top_structures > 1)
			cout << endl << "More than one topstructure, maybe you forget to link references?" << endl;
	}
	else cout << "This GDSfile has no structures!" << endl;
}

GdsII& GdsII::operator=(GdsII& a_gdsii)
{

   {
      TDLI<Structure> _SI=TDLI<Structure>(this);
      //first empty the element list
      _SI.delete_all();
   }
  	TDLI<Structure> _SI=TDLI<Structure>(&a_gdsii);
	_SI.tohead();
	while (!_SI.hitroot())
   {
      Structure* tmp=new Structure();
   	*tmp = *(_SI.item());
      insbegin(tmp);
		_SI++;
	}

	_boundingbox = a_gdsii._boundingbox;
	LinkReferences();
   SetTopStructure(SearchTopStructure());
	_showstructure=a_gdsii._showstructure;
	_boundingbox.SetValid(false);
	return *this;
}

void	 GdsII::SetGroupFlags(group *a_group,EleFlags mask)
{
	_showstructure->SetGroupFlags(a_group,mask,false);
}

void	 GdsII::SetGroupFlagsRefs(EleFlags mask,bool srefselected)
{
	_showstructure->SetGroupFlagsRefs(mask,srefselected);
}

void	 GdsII::ClearFlags(short int mask)
{
	_showstructure->ClearFlags(mask);
}

void   GdsII::SetFlags(EleFlags mask,bool selectedonly)
{
	_showstructure->SetFlags(mask,selectedonly);
}

void   GdsII::SetAvailable()
{
   int i;
   // Clear all first
   for (i=0; i<MAX_LAYERS; i++)
      GLOB->SETUP->Set_Available(i, false);

   TDLI<Structure> _SI=TDLI<Structure>(this);
   _SI.toroot();
   while (_SI.iterate())
      _SI.item()->SetAvailable();

}


void GdsII::Flatten(bool selected_only,bool deep)
{
   wxTransformMatrix*_ident = new wxTransformMatrix();

	_showstructure->Flatten(_showstructure,_ident,selected_only,false,ELE_GR_A,deep);

   //traverse to set the number of times a structure is referenced
   ResetReferencesNr();
   _topstructure->SetReferenceNr();
   CheckEmpty(_topstructure);

   delete _ident;
   CalcBoundingBox();
}

void GdsII::Transform(bool mirror_x,bool mirror_y,double move_h,double move_v,
                  double scale_h_ratio,double scale_v_ratio,double degr,
                  EleFlags 	 mask)
{
   _trans_matrix->Identity();

   _trans_matrix->Mirror(mirror_x,mirror_y);
   _trans_matrix->Scale(scale_h_ratio, scale_v_ratio, 0, 0);
   _trans_matrix->Translate(move_h, move_v);
   _trans_matrix->Rotate(degr,0,0);

   _showstructure->Transform(_trans_matrix,mask);
   CalcBoundingBox();
}

void GdsII::UndoTransform(EleFlags 	 mask)
{
   _trans_matrix->Invert();
   _showstructure->Transform(_trans_matrix,mask);
   CalcBoundingBox();
}

void GdsII::Explode(bool selectedonly)
{
   _showstructure->Explode(_showstructure,selectedonly,this);
   //traverse to set the number of times a structure is referenced
   ResetReferencesNr();
   _topstructure->SetReferenceNr();
}

bool GdsII::AddProperty(const wxString& name,const wxString& type,const wxString& value,bool selectedonly)
{
   ResetBinHere();
   _showstructure->AddProperty(name,type,value,selectedonly);
   return true;
}

bool GdsII::AddProperty(EleFlags mask,const wxString& name,const wxString& type,const wxString& value)
{
   ResetBinHere();
   _showstructure->AddProperty(mask,name,type,value);
   return true;
}

bool GdsII::Move(int destination,EleFlags mask)
{
   ResetBinHere();
   _showstructure->Move(destination,mask);
   CalcBoundingBox();
   return true;
}

bool GdsII::Copy(int destination,EleFlags mask)
{
   ResetBinHere();
   _showstructure->Copy(destination,mask);
   CalcBoundingBox();
   return true;
}

bool GdsII::Delete(EleFlags mask)
{
   ResetBinHere();
   _showstructure->Delete_A(false,mask);
   DeleteBin();
   CalcBoundingBox();
   return true;
}

bool GdsII::Delete_A(EleFlags mask)
{
   ResetBinHere();
   _showstructure->Delete_A(true,mask);
   CalcBoundingBox();
   return true;
}

bool GdsII::ToArcs(int destination,EleFlags mask)
{
   ResetBinHere();
   _showstructure->ToArcs(destination,mask);
   CalcBoundingBox();
   return true;
}

bool GdsII::ToLines(int destination,EleFlags mask)
{
	ResetBinHere();
   _showstructure->ToLines(destination,mask);
   CalcBoundingBox();
   return true;
}

void 	GdsII::Operation(OPS operation,EleFlags maskA,EleFlags maskB,int destination,bool m_destination_add)
{
   ResetBinHere();
   _showstructure->Operation(operation,maskA,maskB,destination,m_destination_add);
   CalcBoundingBox();
}

/*
bool GdsII::Recurse(wxPropertyValues* args)
{
   bool result=false;

   wxPropertyValues::Node* node = args->GetFirst();
	DOWHAT dothis=node->GetData()->IntegerValue();
   node=node->GetNext();

	switch (dothis)
	{
      case(DO_CORRECTION):
      case(DO_SMOOTHEN)  :
      case(DO_MAKE_RING) :
		case(DO_AND)		 :
		case(DO_EXOR)   	 :
		case(DO_A_SUB_B)	 :
		case(DO_B_SUB_A)	 :
		case(DO_OR)     	 :
      {
         _structurelist->ResetBinHere();
         _showstructure->Recurse(args);
         CalcBoundingBox();
         break;
      }
		default:
			throw GDS_Error("Wrong operation in gdsii object");
	}

   return true;
}
*/

void GdsII::CalcArea(double* area,EleFlags mask)
{
   _showstructure->CalcArea(area,mask);
}


void GdsII::SetReferenceNr()
{
	ResetReferencesNr();
	_topstructure->SetReferenceNr();
}

void GdsII::SetModTime()
{
	time_t timer;
	struct tm * tblock;
	timer = time(NULL);
	tblock = localtime(&timer);

	_bgnlib->SetLmtYear(tblock->tm_year);
	_bgnlib->SetLmtMonth(tblock->tm_mon+1);
	_bgnlib->SetLmtDay(tblock->tm_mday);
	_bgnlib->SetLmtHour(tblock->tm_hour);
	_bgnlib->SetLmtMinute(tblock->tm_min);
	_bgnlib->SetLmtSecond(tblock->tm_sec);

	_bgnlib->SetLatYear(tblock->tm_year);
	_bgnlib->SetLatMonth(tblock->tm_mon+1);
	_bgnlib->SetLatDay(tblock->tm_mday);
	_bgnlib->SetLatHour(tblock->tm_hour);
	_bgnlib->SetLatMinute(tblock->tm_min);
	_bgnlib->SetLatSecond(tblock->tm_sec);
}

void GdsII::ResetUnits(double factor)
{
   _trans_matrix->Identity();
   _trans_matrix->Scale(factor, factor, 0, 0);

   _topstructure->ClearFlags(ELE_GR_C); //clear all
   _topstructure->SetFlags(ELE_GR_C,false);
   _topstructure->SetGroupFlagsRefs(ELE_GR_C,false);
  	_topstructure->Transform(_trans_matrix,ELE_GR_C);
// CalcBoundingBox();
}

void GdsII::MakeStructure(wxString& structurename)
{
      Structure* newst = new Structure(structurename);

      //get the selected elements from the showstructure and move them to the local
      //element list
      bool found=false;

      while (GetStructure((char*) structurename.c_str()))
      {
         structurename+="_C";
      }

      TDLI<Element> I2=TDLI<Element>(_showstructure);
      I2.tohead();
      while (!I2.hitroot())
      {
         if (I2.item()->GetSelect())
         {
            Element* a_ele=I2.item();
            I2.remove();
            a_ele->SetSelect(false);
            newst->insbegin(a_ele);
            found=true;
         }
         else
            I2++;
      }

      if (found)
      {
         newst->CalcBoundingBox();

         StructureReference* _Sref2 = new StructureReference();
         Point help(newst->GetBoundingBox().GetMin().GetX(),newst->GetBoundingBox().GetMin().GetY());
         _Sref2->SetPoint(help);
         _Sref2->SetStructureReferenceName(newst->GetStructureName());
         _Sref2->SetStructure(newst);
         _Sref2->SetSelect(true);
         I2.insbegin(_Sref2);
         insend(newst);
         newst->SetReferenced(1);
         wxTransformMatrix*	_trans_matrix 	= new wxTransformMatrix();
         _trans_matrix->Identity();
         _trans_matrix->Translate(-newst->GetBoundingBox().GetMin().GetX(),-newst->GetBoundingBox().GetMin().GetY());
         newst->Transform(_trans_matrix,ELE_ALL);
         newst->CalcBoundingBox();
         delete _trans_matrix;
         newst->SortOnBoundingBox();
         _showstructure->CalcBoundingBox();
         _showstructure->SortOnBoundingBox();

      }
      else
      	delete newst;


}

Structure* GdsII::GetStructure(const wxString& structurename)
{
  	TDLI<Structure> _SI=TDLI<Structure>(this);
	_SI.toroot();
	while (_SI.iterate())
		if (_SI.item()->GetStructureName().Cmp(structurename)==0)
			return _SI.item();

	return 0;
}

int GdsII::LinkReferences()
{
	ResetReferencesNr();
  	TDLI<Structure> _SI=TDLI<Structure>(this);
	_SI.tohead();
	while (!_SI.hitroot())
	{
		_SI.item()->LinkReferences(this);
		_SI++;
	}
	return 0;
}

//	Searches for the topstructure in the structurelist
// structures should be flagged by linkreferences() as being referenced
Structure* GdsII::SearchTopStructure()
{
	int count_top_structures = 0;
	Structure *topstructure = 0;

  	TDLI<Structure> _SI=TDLI<Structure>(this);
	_SI.toroot();
	while (_SI.iterate())
	{
		if (_SI.item()->GetReferenced()==0)
		{
			count_top_structures++;
			if (!topstructure)
				topstructure = _SI.item();
		}
	}



	if (count_top_structures!=1)
	{  char buf[100];

      char name[80];
      strcpy(name,"_top_");
      bool found;
      do
      {
         found=false;
      	if (GetStructure(name))
         {
             strcat(name,"T");
             found = true;
         }
      }
      while (found);

		sprintf(buf,"More then one topstructure\n created a new structure containing the tops, named: %s",name);
		_messagehandler->info(buf,"fixed");

      m_multipletop=true;
      
   	Structure* _new = new Structure(name); // make a new structure to contain all other tops
		topstructure = _new;
      double x=0;
		_SI.toroot();
		while (_SI.iterate())
		{
			if (_SI.item()->GetReferenced()==0)
			{
            StructureReference* _Sref = new StructureReference();
            _SI.item()->CalcBoundingBox();
            x=x+(_SI.item()->GetBoundingBox().GetMax().GetX()-_SI.item()->GetBoundingBox().GetMin().GetX());
            Point a(x,0);
            _Sref->SetPoint(a);
            _Sref->SetStructureReferenceName(_SI.item()->GetStructureName());
            _Sref->SetStructure(_SI.item());
            _new->insend(_Sref);
			}
		}
      _SI.insend(_new);
   }
	return topstructure;
}

void GdsII::ResetBinHere()
{
  	TDLI<Structure> _SI=TDLI<Structure>(this);
	_SI.toroot();
	while (_SI.iterate())
		_SI.item()->SetBinHere(false);
}

void GdsII::ResetReferencesNr()
{
  	TDLI<Structure> _SI=TDLI<Structure>(this);
	_SI.toroot();
	while (_SI.iterate())
		_SI.item()->ResetReferenced();
}

void GdsII::ResetExplodeReferencesNr()
{
  	TDLI<Structure> _SI=TDLI<Structure>(this);
	_SI.toroot();
	while (_SI.iterate())
		_SI.item()->ResetExplodeReferenced();
}

void GdsII::CheckEmpty(Structure* the_top)
{
  	TDLI<Structure> _SI=TDLI<Structure>(this);
	_SI.tohead();
	while (!_SI.hitroot())
   {
		if ((_SI.item()->GetReferenced() <= 0)&&(_SI.item() != the_top))
      {
			delete _SI.item();
			_SI.remove();
			_SI.tohead();
		} else _SI++;
	}
}

void GdsII::DeleteBin()
{
  	TDLI<Structure> _SI=TDLI<Structure>(this);
	_SI.tohead();
	while (!_SI.hitroot())
   {
		if (_SI.item()->GetBinHere())
      {
			delete _SI.item();
			_SI.remove();
		} else _SI++;
	}
}

void GdsII::SortOnBoundingBox()
{
  	TDLI<Structure> _SI=TDLI<Structure>(this);
	_SI.toroot();
	while (_SI.iterate())
		_SI.item()->SortOnBoundingBox();
}


