//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRID_H__47E3224E_7625_4263_9976_75B578054475__INCLUDED_)
#define AFX_GRID_H__47E3224E_7625_4263_9976_75B578054475__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGrid  
{
	double  m_dbDBUnit;
	double	m_dblSnapX;
	double	m_dblSnapY;
	int		m_intMajorX;
	int		m_intMajorY;
	double	m_dblMinorX;
	double	m_dblMinorY;
	int		m_intGridType;
public:
	void RestoreFromRegistry();
	void StoreToRegistry();
	CGrid(double dbu);

	virtual ~CGrid();

	double	GetSnapGridX(){return m_dblSnapX;}
	void	SetSnapGridX(double x)
	{
		m_dblSnapX = x;
		StoreToRegistry();
	}
	double	GetSnapGridY(){return m_dblSnapY;}
	void	SetSnapGridY(double y)
	{
		m_dblSnapY = y;
		StoreToRegistry();
	}

	int		GetMajorX(){return m_intMajorX;}
	void	SetMajorX(int x)
	{
		m_intMajorX = x;
		StoreToRegistry();
	}
	int		GetMajorY(){return m_intMajorY;}
	void	SetMajorY(int y)
	{
		m_intMajorY = y;
		StoreToRegistry();
	}

	double	GetMinorX(){return m_dblMinorX;}
	void	SetMinorX(double x)
	{
		m_dblMinorX = x;
		StoreToRegistry();
	}
	double		GetMinorY(){return m_dblMinorY;}
	void	SetMinorY(double y)
	{
		m_dblMinorY = y;
		StoreToRegistry();
	}

	int		GetGridType(){return m_intGridType;}
	void	SetGridType(int type)
	{
		m_intGridType = type;
		StoreToRegistry();
	}
};

#endif // !defined(AFX_GRID_H__47E3224E_7625_4263_9976_75B578054475__INCLUDED_)
