//! author="klaas"
//! version="1.0"
/*
 *	Program		FlashPrs.h
 *	Purpose		Functions to read David Mann flash files and to convert
 *					it into a GdsII data structure (Header)
 *	Author		J.C. Mollien
 *		"Sunday morning I'm waking up, can't even focus on my coffee cup
 *     I don't know whose bed I'm in, where do I start, where do I begin?"
 * Modified		29-08-1997
 *	Created     19-08-1997
 */

#ifndef EMASKPRS_H
#define EMASKPRS_H

#ifdef __GNUG__
#pragma interface
#endif

#define EMASK_BUFF_SIZE 2047 // must at the very least be 600.

#include "misc.h"

// inludes used for all io and standard functions (libs)
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

//! headerfiles="misc.h gdserr.h lsetup.h"
//! codefiles="flashin.cpp"


#include "lsetup.h"
#include "gdserr.h"


class Arcseg;
class ArrayReference;
class Boundary;
class Box;
class Circle;
class Element;
class ElementList;
class ColumnsRows;
class GdsII;
class Library;
class Path;
class Point;
class Polygon;
class Segment;
class Shape;
class G_Polygon;
class G_Polyline;
class Reference;
class StructureReference;
class Structure;
class Text;


//!Input driver for EMASK files.
/*!
 EMASK contains rectanguler DATA called flashes
 Flash data is read/mapped into the right internal structure (Box element)
 The FLASH file is parsed according to the BNF given here.

 Below you can see the Bachus Naur representation of the format for EMASK
  representation :
\verbatim

 <command> 		::= [<blanks>] {<keyword> <value>}+ <endcommand> [<blanks>]

 <keyword>		::= {'X'|'Y'|'W'|'V'|'U'}

 <value>			::= {<digit>}+

 <digit>        ::= {0-9}

 <EOF>          ::= 'Z'

 <BOF>          ::= '<'

 <comment>      ::= [<blanks>] <quote> <anycharacter but quote | EOF> <quote>

 <blanks>			::= { TAB | SPACE }*

 <endcommand>   ::= 'S'

 <file>         ::= <BOF> {<comment> <comment> <comment>}{<command>}+ {<comment>}<EOF>

 note:
 	[ ]	entity CAN occur zero or more times
 	( )	entity MUST occur
 	{ }	pick one of the entities within the braces
 	{ }*	entities within the braces CAN occur zero or more times
 	{ }+	entities within the braces MUST occur one or more times
 	 |		OR
\endverbatim
*/
class EMASK_driver_in
{
   public:
      //! constructor
      /*!
      \param filename: name of GDSII input file
      \param add: add data to existing drawing
      \param add: add data on this layernr
      */
      EMASK_driver_in(const wxString& filename, bool add, int layernr = 0);

      //!destructor
      ~EMASK_driver_in();

      void Read(GdsII*);
      void SetDavidMannAdd(bool);
   protected:

      //!read one command from flash file
      bool   Read_Command_From_EMASK_File ();
      //! remove blanks
      void  Strip_Blanks();
      void  Strip_Item(int);

      //!read flash command and store the last_Xpos, last_Ypos etc.
      void     Read_Items(int &teller);
      bool   is_Keyword(char);
      char*    ReadValue(int &teller);

      //!read commands from command buffer
      int Read_XY();
      //!read commands from command buffer
      int Read_WH();
      //!read commands from command buffer
      int  Read_A();

      void Read_GDSII(GdsII*);
      void Read_Bgnlib(GdsII*);
      void Read_Library(GdsII*);
      void Read_Structure(GdsII*);
      void Read_Bgnstr(Structure*);
      void Read_StructureName(Structure*);
      void Read_Element(Structure* _structure);
      void Read_Box(Structure* _structure);
      void ReadElflags(Element* _element);
      void ReadLayer(Shape* _shape);
      void ReadBoxType(Box* _box);
      void ReadWidth(Shape* _shape);

      void   Read_Flash(Structure*);

      // conversion functions
      double Deg2Rad(int);
      double Inch2Meter(int value);

		int		GetAddAtLayer();
		bool								GetDavidMannAdd();

		int    Getlast_Xpos();
		int    Getlast_Ypos();
		int    Getlast_Height();
		int    Getlast_Width();
		int     Getlast_Angle();

		void		SetAddAtLayer(int);

		void     Setlast_Xpos(int);
		void     Setlast_Ypos(int);
		void     Setlast_Height(int);
		void     Setlast_Width(int);
		void     Setlast_Angle(int);


	private:

      //!filename of flash file
		wxString	EMASK_filename;

      //!file handle
		std::ifstream		   EMASK_file;

		//! om het commando in op te slaan
		char           command_buffer[EMASK_BUFF_SIZE];

      //!layer to add the data
		int	AddAtLayer;
      //!add to existing drawing
		bool							DavidMannAdd;


		//!position data off last flash
		int          last_Xpos;
		//!position data off last flash
		int          last_Ypos;
		//!position data off last flash
		int          last_Height;
		//!position data off last flash
		int          last_Width;
		//!position data off last flash
		int           last_Angle;
};

#endif
