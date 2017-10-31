//! author="klaas"
//! version="1.0"
/*
Program	SETUP.H
Purpose	Holds setup information about each layer
Programmers	R. Spekreijse
Last Update	22-12-1995
*/

#ifndef UNITS_H
#define UNITS_H

#ifdef __GNUG__
#pragma interface
#endif

#include <wx/string.h>

class CommandHandler;
class GDSMainWindow;
class DrawDriver;
class Canvas;
class ToolControl;
class FillPatterns;
class Setup;
class AliasList;

//!global variables are maintained in here.
/*!
Pointers to mainwindow, commandhandler, screendriver toolcontr,
are stored here for quick access. There is only one instance of this class.
That instance is Global to all classes.
\sa Setup
\sa CommandHandler
\sa GDSMainWindow
\sa DrawDriver
\sa WindowsDriver
\sa ToolControl
*/
class Global
{
   public:
	   Global();
		~Global();
   public:
      bool WINDINGRULE;

      //!how accurate is the drawing
      double USERUNITS();
      //!how many meters is 1 unit in the database
      double PHYSUNITS();

      //!how accurate is the drawing
      void USERUNITS(double userunit);
      //!how many meters is 1 unit in the database
      void PHYSUNITS(double physicalunit);

      //!entry of data
      double ENTRYUNITS();

      double SNAPFACTOR();
      double SELECT_SNAPFACTOR();
      double CORRECTIONFACTOR();
      double CORRECTIONABER();
      double ROUNDFACTOR();

      double POLYTOARCRMIN();
      double POLYTOARCRMAX();
      double POLYTOARCABER();

      double ARCTOPOLYABER();

      double ACCUR();
      double SMOOTHABER();
      double MAXLINEMERGE();

      double DISPLAYABER();

      int 	 STRUCTHRES();
      int 	 PRIMTHRES();
      bool   DRAWSMALLPRIM();

      //!event counter each element gets a unique ID by incrementing this value
      long EVENT_COUNTER;

      //!pointer to the mainwindow
      GDSMainWindow* MAINWINDOW;
      //!pointer to the commandhandler
      CommandHandler* CMDH;

      //!is the interface initialized
      bool INITIALIZED;
      //!pointer to the screendriver
		DrawDriver* SCRDRV;
		Canvas* CANVAS;
      //!pointer to the toolcontroller
      ToolControl* TOOLCTRL;

      //!aliaslist for internal variables
		AliasList* 	ALIASLIST;


      //!setup conatining settings for layers
		Setup* SETUP;
      //!fill pattern for primitive filling
      FillPatterns* FILLPAT;
   private:
      double m_userunits;
      double m_physicalunits;
};

extern Global* GLOB;


//!convert units back and forth
class wxUnit
{
	public:
		wxUnit();
		~wxUnit();

      //!constructor on number and multiplier 1.1 and 1e-6
      wxUnit(double number,double multi);
      //!constructor on number in meters and recalculate for multiplier as string
      wxUnit(double number,const wxString& multi);
      //!constructor on number string and multiplier string "1.1" "um"
      wxUnit(const wxString& number,const wxString& multi);
      //!compose a unit based a string "1.1um"
		bool   Eval(const wxString& param);
      //!get value in meters
		double GetValue();
      //!get the number 1.1 um -> 1.1
      double GetNumber();
      //!get the number 1.1 um -> um
      double GetMultiplier();
      //!get the number 1.1 um -> "1.1 um"
      wxString& GetValueString();
      //!get the number 1.1 um -> "um"
      wxString& GetMultiplierString();
      //!get the number 1.1 um -> "1.1"
      wxString& GetNumberString();

      //!copy
		wxUnit&			operator=(const wxUnit&);
      //!as Eval
		wxUnit&			operator=(const char*);
      //!convert to double
      operator double();
      //!double becomes the number
		wxUnit&			operator=(double);
		int				operator==(const wxUnit&) const;
		int				operator!=(const wxUnit&) const;

	protected:
      //!multiplier
		wxString m_multi;
      //!non multiplied number as a string
		wxString m_number;
};


#endif

