/*
 *	Program			FLASHPRS.cpp
 *	Purpose			Driver for FLASH files
 * Author         Mollien J.C.
 *		"Sunday morning I'm waking up, can't even focus on my coffee cup
 *     I don't know whose bed I'm in, where do I start, where do I begin?"
 * Modified			29-08-1997
 * Created			19-08-1997
 * Comment        The Flash files I have seen suggest that the first and last
                  part of the file is always the same. I have written this
                  driver accordingly. If the sequence for such files is not :

                  #"00" a lot of spaces "some code" "some text"X1234Y....A14
                  "some text and number of Flashes"$

                  then some of the driver must be rewritten. Especially the
						Read_Command_From_FLASH_File function. Also bear in mind
                  that the save routine must save in blocks of 512 bytes with
                  the last 31 or so bytes free. This was probably used for tape
						mechanisms that read and wrote in blocks. The maximum size of
						a Flash is 31 bytes. (X7 Y7 H6 W6 A3 and a semicolumn)
						I don't know if it is necessary, but safe is better than sorry.

						layout of file is : whenever a constructor destructor main(Read GdsII)
						then whenever a function is used in main or below that function is
						written immediately below. Exceptions are the functions to access
						private data; These can be found at the bottom of the file.

						Known difficulties :
							- The routine for conversion from flash format squares to
								four linesegment squares does not always work properly.
								When a square is rotated it messes up things.
 */


#ifdef __GNUG__
#pragma implementation
#endif

#include "flashin.h"

#define  VERSION_NUMBER  3
#define  LIBRARY_NAME    "myLibrary"
#define  STRUCTURE_NAME  "myStructure\0"
#define  U_UNIT_VALUE    0.1         // 1E-1
#define  P_UNIT_VALUE    0.000001   // 1E-6
#define  ELFLAGS_TEMPLATE false        // bool
#define  ELFLAGS_EXTERNAL false        // bool

#define inch            2.54

// hier de #includes van de classes!!!
#include "arc.h"
#include "boundary.h"
#include "elemlst.h"
#include "gdsii.h"
#include "path.h"
#include "point.h"
#include "segment.h"
#include "polyline.h"
#include "referenc.h"
#include "shape.h"
#include "structur.h"
#include "text.h"
#include "units.h"


//CONSTRUCTOR-------------------------------------------------------------------
FLASH_driver_in::FLASH_driver_in(const wxString& filename, bool add, int layernr)
{

	FLASH_filename=filename;
	SetDavidMannAdd(add);
	SetAddAtLayer(layernr);

	if (!GetDavidMannAdd())
	{
		// make all the layers not available by default
		for (int i = 0; i < MAX_LAYERS; i++)
			GLOB->SETUP->Set_Available(i, false);
	}

	FLASH_file.open(FLASH_filename, ios::in);
	if (FLASH_file.bad())
	{
		char errbuf[100];
		sprintf(errbuf, "Cannot open file %s!",FLASH_filename.c_str() );
		throw GDS_Error(errbuf, "flashprs: File Error", 9, 0);
	}

	Setlast_Xpos(0);
	Setlast_Ypos(0);
	Setlast_Width(0);
	Setlast_Height(0);
	Setlast_Angle(0);
}

//DESTRUCTOR--------------------------------------------------------------------
FLASH_driver_in::~FLASH_driver_in()
{
	FLASH_file.close();
}

//Read GdsII (Main routine)-----------------------------------------------------
void FLASH_driver_in::Read(GdsII* _GdsII)
{
	Read_GDSII(_GdsII); // initialisation of the GdsII instance.
// end read
}

//READ GDSII--------------------------------------------------------------------
// om het gdsii object te initialiseren.
void FLASH_driver_in::Read_GDSII(GdsII* _GdsII)
{
	if (!GetDavidMannAdd())
	{
		_GdsII->SetVersion(VERSION_NUMBER);
		Read_Bgnlib(_GdsII);
		Read_Library(_GdsII);
	}

	Read_Structure(_GdsII);

//	Process new data for ...

	//the topstructure is the structure that is not referenced
	_GdsII->SetTopStructure(_GdsII->SearchTopStructure());

	_GdsII->SetShowStructure(_GdsII->GetTopStructure());

	_GdsII->SetReferenceNr();
	_GdsII->CalcBoundingBox();

// end read GdsII
}

// READ BGNLIB------------------------------------------------------------------
void FLASH_driver_in::Read_Bgnlib(GdsII* _GdsII)
{
// set begin library inhoud
	ModeTime* _bgnlib = _GdsII->GetBgnlib();

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

// end read bgnlib
}

// READ LIBRARY-----------------------------------------------------------------
void FLASH_driver_in::Read_Library(GdsII* _GdsII)
{
// set librarysetting
	Library* _library = _GdsII->GetLibrary();
	_library->SetLibraryName(LIBRARY_NAME);
	_GdsII->SetUserUnits(U_UNIT_VALUE);
	_GdsII->SetPhysicalUnits(P_UNIT_VALUE);
// end read library
}


// READ STRUCTURE
void FLASH_driver_in::Read_Structure(GdsII* _structurelist)
{
	Structure* _structure;
	if (GetDavidMannAdd())
	{
		TDLI<Structure> I=TDLI<Structure>(_structurelist);
		I.tohead();
		_structure = I.item(); // bovenste structure nemen !!enige structure!!

		while (Read_Command_From_FLASH_File())
		{// since RCFFF has been called. the buffer is filled with a command.
			int teller = 0; // to remove warning in read_items.
			Strip_Nonsense();
			Read_Items(teller);  // to fill internal stores  with data from flash file.
			Read_Element(_structure);
		}
	}
	else
	{
		_structure = new Structure;
		// van nieuw structure de settings invullen.

		Read_Bgnstr(_structure);
		Read_StructureName(_structure);

		while (Read_Command_From_FLASH_File())
		{// since RCFFF has been called. the buffer is filled with a command.
			int teller = 0; // to remove warning in read_items.
			Strip_Nonsense();
			Read_Items(teller);  // to fill internal stores  with data from flash file.
			Read_Element(_structure);
		}
		// in de lijst zetten van het nieuwe structure
	  	if (_structure->empty())
		{
			char errbuf[100];
			sprintf(errbuf, "File %s contains no flashes!\nFile is EMPTY!!",FLASH_filename.c_str() );
			throw GDS_Error(errbuf, "flashprs: File Error", 9, 0);
		}
		else _structurelist->insend(_structure);
	}
// end read structure
}

// READ BGNSTR------------------------------------------------------------------
void FLASH_driver_in::Read_Bgnstr(Structure* _structure)
{
// nieuwe beginstructure inhoud.
	ModeTime* _bgnstr = _structure->GetBgnstr();

	time_t timer;
	struct tm * tblock;
	timer = time(NULL);
	tblock = localtime(&timer);

	_bgnstr->SetLmtYear(tblock->tm_year);
	_bgnstr->SetLmtMonth(tblock->tm_mon+1);
	_bgnstr->SetLmtDay(tblock->tm_mday);
	_bgnstr->SetLmtHour(tblock->tm_hour);
	_bgnstr->SetLmtMinute(tblock->tm_min);
	_bgnstr->SetLmtSecond(tblock->tm_sec);

	_bgnstr->SetLatYear(tblock->tm_year);
	_bgnstr->SetLatMonth(tblock->tm_mon+1);
	_bgnstr->SetLatDay(tblock->tm_mday);
	_bgnstr->SetLatHour(tblock->tm_hour);
	_bgnstr->SetLatMinute(tblock->tm_min);
	_bgnstr->SetLatSecond(tblock->tm_sec);

// end read bgnstr
}

// READ STRUCTURENAME-----------------------------------------------------------
// nieuwe structure naam geven.
void FLASH_driver_in::Read_StructureName(Structure* _structure)
{
	_structure->SetStructureName(STRUCTURE_NAME);
// end read structurename
}

// member functie om file te lezen
//READ COMMAND FROM FLASH FILE--------------------------------------------------
bool FLASH_driver_in::Read_Command_From_FLASH_File ()
{
	int teller =0;
	char ch = '\0';

	while((FLASH_file.get(ch)) &&			// een char ophalen
		  !((ch == ';') || (ch =='$')) &&	// zolang er geen einde commando of einde file is
		  (FLASH_file.eof()==0 ))			// test voor eof

// read the file and fill the buffer with the command found.
// file is read until a semi-collumn is found.
	{
      if (ch != '\n')
      {
         command_buffer[teller] = ch;
         teller++;
         if (teller>=FLASH_BUFF_SIZE-1)// to test if the buffer exceeds the limits
         {
            char errbuf[100];
            sprintf(errbuf, "File %s!\nIs NOT a valid David Mann File",FLASH_filename.c_str());
            throw GDS_Error(errbuf, "flashprs: File Error", 9, 0);
         }
      }
	}
	switch (ch)
	{
		case ';' : {
						command_buffer[teller] = ';';
						return true;
					  }
		case '$' : {
						command_buffer[teller] = '$';
						return false;
					  }
		default :
		{
			char errbuf[100];
			sprintf(errbuf, "'%c! in file indicates that File %s! is not a Flash file ", ch, FLASH_filename.c_str());
			throw GDS_Error(errbuf, "flashprs: File Error", 9, 0);
		}
	}
}

//STRIP NONSENSE----------------------------------------------------------------
// om de command buffer vrij te maken van spaties en commentaar.
void FLASH_driver_in::Strip_Nonsense()
{
	Strip_Blanks();   // remove blanks
	Strip_BOF();      // remove start of FLASH file symbol '#'
	Strip_Comment();  // remove comment.
//end strip nonsense
}

// STRIP BLANKS-----------------------------------------------------------------
void FLASH_driver_in::Strip_Blanks()
{
	int teller =0;
	while (command_buffer[teller] != ';')
	{
		if (command_buffer[teller] == ' ') //als er een spatie is
			Strip_Item(teller);
		else teller++;
	}
// end strip blanks
}

// STRIP BOF--------------------------------------------------------------------
void FLASH_driver_in::Strip_BOF()
{
	int teller = 0;
	if (command_buffer[teller] == '#') Strip_Item(teller);
// end strip bof
}

// STRIP COMMENT----------------------------------------------------------------
void FLASH_driver_in::Strip_Comment()
{
	int teller = 0;
	while (command_buffer[teller] != ';')
	{
		if (command_buffer[teller] == '"') //als er een aanhalings-teken is
		{
			// commentaar weghalen.
			int nieuwe_teller = teller;
			while (command_buffer[nieuwe_teller+1] != '"')
			{
				Strip_Item(nieuwe_teller); // teken weghalen.
			}
			//laatste aanhalings-teken weg halen.
			Strip_Item(nieuwe_teller);// laatste commentaar teken.
			Strip_Item(nieuwe_teller);// aanhalingsteken.
		}
		else teller++;
	}
	command_buffer [teller+1] = '\0';
// end strip comment
}


//STRIP ITEM--------------------------------------------------------------------
// hulpfunctie voor STRIP NONSENSE
void FLASH_driver_in::Strip_Item(int a)
{
	while (command_buffer[a+1] != ';')
	{
		command_buffer[a] = command_buffer[a+1];
		a++;
	}
	command_buffer[a] = ';';
//end strip item
}

//READ ITEMS--------------------------------------------------------------------
// om een FLASH te lezen uit de command buffer.
void FLASH_driver_in::Read_Items(int &teller) // misschien met een bool??
{
// check for each command if it is there
	if (is_Keyword(command_buffer[teller]))
	{
		char* valuestring;
		if ((command_buffer[teller]== 'X')||(command_buffer[teller]== 'x'))
		{
			// waarde van x inlezen tot het volgende commando
			teller++; //keyword zelf weghalen.
			valuestring = ReadValue(teller);
			Setlast_Xpos(atol(valuestring));
			delete valuestring;
			Read_Items(teller);
			return;
		}
		else if ((command_buffer[teller]== 'Y')||(command_buffer[teller]== 'y'))
		{
			// waarde van y inlezen tot het volgende commando
			teller++; //keyword zelf weghalen.
			valuestring = ReadValue(teller);
			Setlast_Ypos(atol(valuestring));
			delete valuestring;
			Read_Items(teller);
			return;
		}
		else if ((command_buffer[teller]== 'W')||(command_buffer[teller]== 'w'))
		{
			// waarde van h inlezen tot het volgende commando
			teller++; //keyword zelf weghalen.
			valuestring = ReadValue(teller);
			Setlast_Width(atol(valuestring));
			delete valuestring;
			Read_Items(teller);
			return;
		}
		else if ((command_buffer[teller]== 'H')||(command_buffer[teller]== 'h'))
		{
			// waarde van w inlezen tot het volgende commando
			teller++; //keyword zelf weghalen.
			valuestring = ReadValue(teller);
			Setlast_Height(atol(valuestring));
			delete valuestring;
			Read_Items(teller);
			return;
		}
		else if ((command_buffer[teller]== 'A')||(command_buffer[teller]== 'a'))
		{
			// waarde van w inlezen tot het volgende commando
			teller++; //keyword zelf weghalen.
			valuestring = ReadValue(teller);
			Setlast_Angle(atoi(valuestring));
			delete valuestring;
			Read_Items(teller);
			return;
		}
		else if (command_buffer[teller]== ';')
		{
			delete valuestring; //Added to stop possible memory leaks
			return; // Good way out.
		}
		else
			delete valuestring; //Added to stop possible memory leaks
	}
	else
	{
		char errbuf[100];
		sprintf(errbuf ,"\n wrong command in file %s!\n",FLASH_filename.c_str());
		throw GDS_Error(errbuf, "flashprs: File Error", 9, 0);
	}
// end read items
}

// IS KEYWORD-------------------------------------------------------------------
// Kijken of de operator wel echt een operator is.
bool FLASH_driver_in::is_Keyword(char ch)
{
	switch (ch)
	{
		case 'x' : case 'X' : // small and LARGE commands
		case 'y' : case 'Y' :
		case 'h' : case 'H' :
		case 'w' : case 'W' :
		case 'a' : case 'A' :
		case ';' : // and the terminator, not really a command but necessary
		{
			return true;
		}
		default :
		{
			return false;
		}
	}
// end is keyword
}

// READ VALUE-------------------------------------------------------------------
// om de waarden uit de buffer te lezen.
char* FLASH_driver_in::ReadValue(int &teller)
{
	int vsteller =0;
	char* valuestring = new char [7];
	while (!is_Keyword(command_buffer[teller]))
	{
		valuestring[vsteller] = command_buffer[teller];
		teller++;
		vsteller++;
	}
	valuestring[vsteller] = '\0';
	return valuestring;
// end readvalue
}


// READ ELEMENT-----------------------------------------------------------------
// initialisation of the elements
void FLASH_driver_in::Read_Element(Structure* _structure)
{
//	<element>			::=	{<boundary> | <path> | <SREF> | <AREF> | <text> | <node> | <box>}
//									/* {<property>}* */
//									ENDEL
// for flashes an element is always a box.

	Read_Box(_structure);
	//	ReadProperty();
	// ReadENDEL
}

//READ BOX-------------------------------------------------------------------
// om een box te vullen met de waarden van de laatst gelezen FLASH,
// en deze box toe te voegen aan de lijst met elements in de Structure _structure.
void FLASH_driver_in::Read_Box(Structure* _structure)
{
// <box>	::=	BOX [ELFLAGS] [PLEX] LAYER BOXTYPE [WIDTH] XY

	Box* _box = new Box();

	// Optional
	// Must be there
	ReadLayer(_box);
	ReadBoxType(_box);
	ReadWidth(_box);

	double a = Deg2Rad(Getlast_Angle());
   //in 10 micro Inch in File, so times 1e-6 for meters * 1e-1 for the 10 extra
	double w = Inch2Meter(Getlast_Width())*1e-7/P_UNIT_VALUE;
	double h = Inch2Meter(Getlast_Height())*1e-7/P_UNIT_VALUE;
	double x = Inch2Meter(Getlast_Xpos())*1e-7/P_UNIT_VALUE;
	double y = Inch2Meter(Getlast_Ypos())*1e-7/P_UNIT_VALUE;
   _box->Set(x,y,a,w,h);
  	_structure->insend(_box);
// end read box
}

// READ LAYER-------------------------------------------------------------------
void FLASH_driver_in::ReadLayer(Shape* _shape)
{
	_shape->SetLayer(GetAddAtLayer());
	GLOB->SETUP->Set_Available(GetAddAtLayer(), true);
}

// READ BOXTYPE-----------------------------------------------------------------
void FLASH_driver_in::ReadBoxType(Box* _box)
{
	_box->SetBoxtype(0);
}

// READ WIDTH-------------------------------------------------------------------
void FLASH_driver_in::ReadWidth(Shape* _shape)
{
	_shape->SetWidth(0);
}

//DEG2RAD-----------------------------------------------------------------------
double  FLASH_driver_in::Deg2Rad(int value)
{
	return value/10.0*M_PI/180.0;
// eind deg2rad
}

//Inch2Meter----------------------------------------------------------------------
// om inches om te zetten in meters
double FLASH_driver_in::Inch2Meter(int value)
{
	return value*inch;
//end inch2meter
}

//------------------------------------------------------------------------------
// member functies om private data aan te spreken.
// GetAddAtLayer
int FLASH_driver_in::GetAddAtLayer()
{
	return AddAtLayer;
}

//	GET DAVID MANN ADD-----------------------------------------------------------
bool	FLASH_driver_in::GetDavidMannAdd()
{
	return DavidMannAdd;
}

//GET LAST X-POSITION-----------------------------------------------------------
int FLASH_driver_in::Getlast_Xpos()
{
	return last_Xpos;
}

//GET LAST Y-POSITION-----------------------------------------------------------
int FLASH_driver_in::Getlast_Ypos()
{
	return last_Ypos;
}

//GET LAST HEIGHT---------------------------------------------------------------
int FLASH_driver_in::Getlast_Height()
{
	return last_Height;
}

//GET LAST WIDTH----------------------------------------------------------------
int FLASH_driver_in::Getlast_Width()
{
	return last_Width;
}

//GET LAST ANGLE----------------------------------------------------------------
int  FLASH_driver_in::Getlast_Angle()
{
	return last_Angle;
}

//------------------------------------------------------------------------------
// member functies om private data mee te setten.

void	FLASH_driver_in::SetAddAtLayer(int value)
{
	AddAtLayer = value;
}

//	SETDAVIDMANNADD--------------------------------------------------------------
void	FLASH_driver_in::SetDavidMannAdd(bool value)
{
	DavidMannAdd = value;
}

//SET LAST X-POSITION-----------------------------------------------------------
void FLASH_driver_in::Setlast_Xpos(int value)
{
 last_Xpos = value;
}

//SET LAST Y-POSITION-----------------------------------------------------------
void FLASH_driver_in::Setlast_Ypos(int value)
{
 last_Ypos = value;
}

//SET LAST HEIGHT---------------------------------------------------------------
void FLASH_driver_in::Setlast_Height(int value)
{
 last_Height = value;
}

//SET LAST WIDTH----------------------------------------------------------------
void FLASH_driver_in::Setlast_Width(int value)
{
 last_Width = value;
}

//SET LAST ANGLE----------------------------------------------------------------
void FLASH_driver_in::Setlast_Angle(int value)
{
 last_Angle = value;
}
//------------------------------------------------------------------------------
//---------------------------END OF FILE FLASHPRS.CPP---------------------------
//------------------------------------------------------------------------------
