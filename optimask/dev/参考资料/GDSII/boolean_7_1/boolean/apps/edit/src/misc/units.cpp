#ifdef __GNUG__
#pragma implementation
#endif
#include "units.h"
#include "misc.h"
#include "chandbas.h"
#include "gdsii.h"
#include "windrv.h"
#include "bbox.h"
#include "alias.h"

Global::Global()
{
   WINDINGRULE		= true;

   EVENT_COUNTER    = 0; //session of boolean event counter used for undo actions
   INITIALIZED=false;
/*
   MAINWINDOW=0;
	SCRDRV=0;
   CMDH= 0;

   TOOLCTRL=0;
*/
	FILLPAT = new       FillPatterns();
   m_userunits=0.01;
   m_physicalunits=1e-6;

	ALIASLIST = new AliasList();

}

Global::~Global()
{
   delete FILLPAT;
   delete ALIASLIST;
}

//how accurate is the drawing
double Global::USERUNITS()
{
//   if (CMDH && CMDH->Get_GDSII())
//		return CMDH->Get_GDSII()->GetUnits()->GetUserUnits();
//   else
      return m_userunits;
}

//how many meters is 1 unit in the database
double Global::PHYSUNITS()
{
//   if (CMDH && CMDH->Get_GDSII())
//		return CMDH->Get_GDSII()->GetUnits()->GetPhysicalUnits();
//   else
      return m_physicalunits;
}

//how accurate is the drawing
void Global::USERUNITS(double userunit)
{
   m_userunits=userunit;
}
//how many meters is 1 unit in the database
void Global::PHYSUNITS(double physicalunit)
{
	m_physicalunits=physicalunit;
}

//entry of data
double Global::ENTRYUNITS()
{
	return CMDH->GetDefaultUnit().GetValue();
}

double Global::SNAPFACTOR()
{
	return GLOB->CMDH->GetSnapfactor()/PHYSUNITS();
}

double Global::SELECT_SNAPFACTOR()
{
   BoundingBox bbox = GLOB->SCRDRV->GetVirtualSizeScreen();
   double dx=bbox.GetMax().GetX()-bbox.GetMin().GetX();
	return GLOB->CMDH->GetSelectmarge()*dx;
}

double Global::CORRECTIONFACTOR()
{
	return GLOB->CMDH->GetCorrectionfactor().GetValue()/PHYSUNITS();
}

double Global::CORRECTIONABER()
{
	return GLOB->CMDH->GetCorrectionaber().GetValue()/PHYSUNITS();
}

double Global::ROUNDFACTOR()
{
	return GLOB->CMDH->GetRoundfactor();
}

double Global::POLYTOARCRMIN()
{
	return GLOB->CMDH->GetPoly2ArcRmin().GetValue()/PHYSUNITS();
}

double Global::POLYTOARCRMAX()
{
	return GLOB->CMDH->GetPoly2ArcRmax().GetValue()/PHYSUNITS();
}

double Global::POLYTOARCABER()
{
	return GLOB->CMDH->GetPoly2ArcAber().GetValue()/PHYSUNITS();
}

double Global::ARCTOPOLYABER()
{
	return GLOB->CMDH->GetArc2PolyAber().GetValue()/PHYSUNITS();
}

double Global::ACCUR()
{
	return GLOB->CMDH->GetDefaultAcc()/10000;
}

double Global::SMOOTHABER()
{
	return GLOB->CMDH->GetSmoothAber().GetValue()/PHYSUNITS();
}

double Global::MAXLINEMERGE()
{
	return GLOB->CMDH->GetMaxlinemerge().GetValue()/PHYSUNITS();
}

double Global::DISPLAYABER()
{
	return GLOB->CMDH->GetDisplayaber().GetValue()/PHYSUNITS();
}

int 	 Global::STRUCTHRES()
{
	return GLOB->CMDH->GetStrucThres();
}

int 	 Global::PRIMTHRES()
{
	return GLOB->CMDH->GetPrimThres();
}

bool Global::DRAWSMALLPRIM()
{
	return GLOB->CMDH->GetDrawSmallPrim();
}

wxUnit::wxUnit()
{
	m_multi="1";
	m_number="1";
}

wxUnit::wxUnit(double number,double multi)
{
	m_number.Printf("%.*f",3,number);
	m_multi.Printf("%.*f",3,multi);
}

wxUnit::wxUnit(double number,const wxString& multi)
{
	m_multi=multi;
	m_number.Printf("%.*f",3,number/GetMultiplier());
}


wxUnit::wxUnit(const wxString& number,const wxString& multi)
{
	m_multi=multi;
	m_number=number;
}

wxUnit::~wxUnit()
{
}

wxString& wxUnit::GetValueString()
{
   static wxString tot;
   tot=m_number+m_multi;
	return tot;
}

double wxUnit::GetNumber()
{

	static double dd;
   m_number.ToDouble(&dd);
   return dd;
}

double wxUnit::GetValue()
{
	static double dd;
   m_number.ToDouble(&dd);
	dd=dd*GetMultiplier();
   return dd;
}

double wxUnit::GetMultiplier()
{
	static double multi;
	if ( m_multi.Len()) //is there a valid ascii multiplier
	{
		if (m_multi =="pm")
      {
			multi=1e-12;
      }
      else if (m_multi =="nm")
      {
			multi=1e-9;
      }
      else if (m_multi =="um")
      {
			multi=1e-6;
      }
      else if (m_multi =="mm")
      {
			multi=1e-3;
      }
      else if (m_multi =="cm")
      {
			multi=1e-2;
      }
      else if (m_multi =="dm")
      {
			multi=1e-1;
      }
      else if (m_multi =="m")
      {
			multi=1e-0;
      }
      else if (m_multi =="e")
      {
			multi=0.00254;
      }
      else if (m_multi =="mil")
      {
			multi=0.00254;
      }
      else if (m_multi =="inch")
      {
			multi=0.30;
      }
   }
   else
      multi=1;
   return multi;
}

wxString& wxUnit::GetMultiplierString()
{
	return m_multi;
}

wxString& wxUnit::GetNumberString()
{
	return m_number;
}

// copy the contents of this class in another
wxUnit& wxUnit::operator=(const wxUnit &other)
{
   m_multi=other.m_multi;
   m_number=other.m_number;

	return *this;
}

// equal or not
int wxUnit::operator==(const wxUnit &other) const
{
   if ((m_multi==other.m_multi) && ( m_number == other.m_number))
			return 0;
	return 1;
}

int wxUnit::operator!=(const wxUnit &other) const
{
	return ! (*this == other);
}

wxUnit& wxUnit::operator=(const char* str)
{
   Eval(str);
	return *this;
}



wxUnit& wxUnit::operator=(double dd)
{
	m_number.Printf("%f", dd);
	return *this;
}


wxUnit::operator double()
{
   return GetValue();
}

bool wxUnit::Eval(const wxString& param)
{

	char *endptr=0;

	if (param.Len() == 0)
   {
      return false;
   }

	strtod(param.c_str(),&endptr);
   if (endptr==param) //no number found
   {
      m_number="1";
      m_multi="";
   }
   else
   {
		m_number=param.Left(endptr-param.c_str());
   	m_multi=endptr;
   }

   m_multi.Trim(true);
   m_multi.Trim(false);

	if ( m_multi.Len()) //is there a valid ascii multiplier
	{
		if (
		      (m_multi =="pm") ||
		      (m_multi =="nm") ||
		      (m_multi =="um") ||
		      (m_multi =="mm") ||
		      (m_multi =="cm") ||
		      (m_multi =="dm") ||
		      (m_multi =="m") ||
		      (m_multi =="e") ||
		      (m_multi =="mil") ||
		      (m_multi =="inch")
         )
         return true;
		else
			return false;
	}
	return true;
}


