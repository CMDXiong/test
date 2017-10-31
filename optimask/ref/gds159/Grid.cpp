//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "gds.h"
#include "Grid.h"
#include "GdsDoc.h"
#include "GdsView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CGrid::CGrid(double dbu)
{
	m_dbDBUnit = dbu;
	RestoreFromRegistry();
}

CGrid::~CGrid()
{

}

void CGrid::StoreToRegistry()
{
	CWinApp* app = AfxGetApp();
	app->WriteProfileInt("Grid", "Grid style", m_intGridType);
	app->WriteProfileInt("Grid", "Grid major X", m_intMajorX);
	app->WriteProfileInt("Grid", "Grid major Y", m_intMajorY);


	int val = (int)(10000.0 * m_dblMinorX);
	if(10000.0 * m_dblMinorX - val > m_dblMinorX * 10000.0/pow(10.0, 15.0))
		val++;
	app->WriteProfileInt("Grid", "Grid minor X", val);

	val = (int)(10000.0 * m_dblMinorY);
	if(10000.0 * m_dblMinorY - val > m_dblMinorY * 10000.0/pow(10.0, 15.0))
		val++;
	app->WriteProfileInt("Grid", "Grid minor Y", val);

	val = (int)(10000.0 * m_dblSnapX);
	if(10000.0 * m_dblSnapX - val > m_dblSnapX * 10000.0/pow(10.0, 15.0))
		val++;
	app->WriteProfileInt("Grid", "Grid snap X",  val);
	val = (int)(10000.0 * m_dblSnapY);
	if(10000.0 * m_dblSnapY - val > m_dblSnapY * 10000.0/pow(10.0, 15.0))
		val++;
	app->WriteProfileInt("Grid", "Grid snap Y",  val);

	CString dbunit;
	dbunit.Format("%e", m_dbDBUnit);
	app->WriteProfileString("Grid", "Database Unit String", dbunit);

	// Delete previously used as integer
	app->WriteProfileString("Grid", "Database Unit", NULL);
}

void CGrid::RestoreFromRegistry()
{
 	CWinApp* app = AfxGetApp();
	m_intGridType = app->GetProfileInt("Grid", "Grid style", GRID_TYPE_DOT);
	m_intMajorX = app->GetProfileInt("Grid", "Grid major X", 10);
	m_intMajorY = app->GetProfileInt("Grid", "Grid major Y", 10);



	m_dblMinorX = app->GetProfileInt("Grid", "Grid minor X", 1);
	if(m_dblMinorX >= 1000)
		m_dblMinorX /= 10000;

	m_dblMinorY = app->GetProfileInt("Grid", "Grid minor Y", 1);
	if(m_dblMinorY >= 1000)
		m_dblMinorY /= 10000;

	m_dblSnapX = app->GetProfileInt("Grid", "Grid snap X",    1);
	if(m_dblSnapX >= 1000)
		m_dblSnapX /= 10000;
	m_dblSnapY = app->GetProfileInt("Grid", "Grid snap Y",    1);
	if(m_dblSnapY >= 1000)
		m_dblSnapY /= 10000;

	CString dbunit = app->GetProfileString("Grid", "Database Unit String", "1.0e-6");
	double dbu = atof(dbunit);

	double diff = fabs(m_dbDBUnit - dbu);
	if(dbu != 1.0 && diff > dbu * 1.0e-9){
		dbu = dbu/m_dbDBUnit;
		m_dblMinorX = m_dblMinorX * dbu;
		m_dblMinorY = m_dblMinorY * dbu;
		m_dblSnapX = m_dblSnapX * dbu;
		m_dblSnapY = m_dblSnapY * dbu;
	}
}
