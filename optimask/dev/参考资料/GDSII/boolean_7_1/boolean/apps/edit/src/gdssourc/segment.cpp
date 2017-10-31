/*
Program	Line_Seg.CPP
Purpose	Definition of GDSII Line_Seg type structure
Last Update	12-12-1995
*/

#ifdef __GNUG__
#pragma implementation
#endif
#include <stdlib.h>
#include "segment.h"

//static DL_List<void*> _Segment_Pool;
//static DL_StackIter<void*>  RP(&_Segment_Pool);
#define MAXLINE 1000
#define MAXARC  100
void* _Line_Segment_Pool[MAXLINE+3];
void* _Arc_Segment_Pool[MAXARC+3];

int i=-1;
int j=-1;

bool equal(double,double);

Segment::Segment()
{
		m_bin=false;
	   m_holetype=NORMAL_SEG;
}

Segment::~Segment()
{
}

// Constructors
Line_Seg::Line_Seg()
{
	_point.SetX(0.0);
	_point.SetY(0.0);
}


Line_Seg::Line_Seg(double const X, double const Y)
{
	_point.SetX(X);
	_point.SetY(Y);
}

Line_Seg::Line_Seg(Point* const p)
{
	_point.SetX(p->GetX());
	_point.SetY(p->GetY());
}

Line_Seg::Line_Seg(Line_Seg* other)
{
	assert(other->GetType()==LINE);

	_point.SetX(other->_point.GetX());
	_point.SetY(other->_point.GetY());
}

Line_Seg::~Line_Seg()
{
}
/*
void* Line_Seg::operator new(size_t size)
{

   if (i!=-1)
   {
     return _Line_Segment_Pool[i--];
   }

//   if (!RP.empty())
//      return RP.pop();
   return malloc(size);
}

void Line_Seg::operator delete(void* Segmentptr)
{

   if (i < MAXLINE)
   {
     _Line_Segment_Pool[++i]=Segmentptr;
     return;
   }

//   if (RP.count() < 30 )
//   {
//       RP.push(Segmentptr);
//       return;
//   }
	free (Segmentptr);
}
*/

SEGTYPE  Line_Seg::GetType()
{
 return LINE;
};

double Line_Seg::GetX()
{
	return _point.GetX();
}

double Line_Seg::GetY()
{
	return _point.GetY();
}

void Line_Seg::SetX(double a_point_x)
{
	_point.SetX(a_point_x);
}


void Line_Seg::SetY(double a_point_y)
{
	_point.SetY(a_point_y);
}

void Line_Seg::Set(const double X,const double Y)
{
	_point.SetX(X);
	_point.SetY(Y);
}

Line_Seg& Line_Seg::operator=(Line_Seg &other)
{
	assert(other.GetType()==LINE);

   m_holetype=other.m_holetype;
   m_bin=other.m_bin;
	_point.SetX(other._point.GetX());
	_point.SetY(other._point.GetY());

	return *this;
}

// Constructors
Arc_Seg::Arc_Seg()
{
	_point.SetX(0.0);
	_point.SetY(0.0);
	_Apoint.SetX(0.0);
	_Apoint.SetY(0.0);
	_Opoint.SetX(0.0);
	_Opoint.SetY(0.0);
}


Arc_Seg::Arc_Seg(double const X, double const Y,double const AX, double const AY, double const OX, double const OY)
{
	_point.SetX(X);
	_point.SetY(Y);
	_Apoint.SetX(AX);
	_Apoint.SetY(AY);
	_Opoint.SetX(OX);
	_Opoint.SetY(OY);
}


Arc_Seg::Arc_Seg(Point* const p, Point* const ap, Point* const op)
{
	_point.SetX(p->GetX());
	_point.SetY(p->GetY());
	_Apoint.SetX(ap->GetX());
	_Apoint.SetY(ap->GetY());
	_Opoint.SetX(op->GetX());
	_Opoint.SetY(op->GetY());
}

Arc_Seg::Arc_Seg(Arc_Seg* other)
{
	assert(other->GetType()==ARC);

	_point.SetX(other->_point.GetX());
	_point.SetY(other->_point.GetY());
	_Apoint.SetX(other->_Apoint.GetX());
	_Apoint.SetY(other->_Apoint.GetY());
	_Opoint.SetX(other->_Opoint.GetX());
	_Opoint.SetY(other->_Opoint.GetY());
}

Arc_Seg::~Arc_Seg()
{
}

/*
void* Arc_Seg::operator new(size_t size)
{

   if (j!=-1)
   {
     return _Arc_Segment_Pool[j--];
   }

//   if (!RP.empty())
//      return RP.pop();
   return malloc(size);
}

void Arc_Seg::operator delete(void* Segmentptr)
{

   if (j < MAXARC)
   {
     _Arc_Segment_Pool[++j]=Segmentptr;
     return;
   }

//   if (RP.count() < 30 )
//   {
//       RP.push(Segmentptr);
//       return;
//   }
	free (Segmentptr);
}
*/
SEGTYPE  Arc_Seg::GetType()
{
 return ARC;
};

double Arc_Seg::GetX()
{
	return _point.GetX();
}

double Arc_Seg::GetY()
{
	return _point.GetY();
}

double Arc_Seg::GetAX()
{
	return _Apoint.GetX();
}

double Arc_Seg::GetAY()
{
	return _Apoint.GetY();
}

double Arc_Seg::GetOX()
{
	return _Opoint.GetX();
}

double Arc_Seg::GetOY()
{
	return _Opoint.GetY();
}

void Arc_Seg::SetX(double a_point_x)
{
	_point.SetX(a_point_x);
}

void Arc_Seg::SetY(double a_point_y)
{
	_point.SetY(a_point_y);
}

void Arc_Seg::SetAX(double a_point_x)
{
	_Apoint.SetX(a_point_x);
}

void Arc_Seg::SetAY(double a_point_y)
{
	_Apoint.SetY(a_point_y);
}

void Arc_Seg::SetOX(double o_point_x)
{
	_Opoint.SetX(o_point_x);
}

void Arc_Seg::SetOY(double o_point_y)
{
	_Opoint.SetY(o_point_y);
}

void Arc_Seg::Set(const double X,const double Y,double const AX, double const AY, double const OX, double const OY)
{
	_point.SetX(X);
	_point.SetY(Y);
	_Apoint.SetX(AX);
	_Apoint.SetY(AY);
	_Opoint.SetX(OX);
	_Opoint.SetY(OY);
}

Arc_Seg& Arc_Seg::operator=(Arc_Seg &other)
{
	assert(other.GetType()==ARC);

   m_holetype=other.m_holetype;
   m_bin=other.m_bin;
	_point.SetX(other._point.GetX());
	_point.SetY(other._point.GetY());
	_Apoint.SetX(other._Apoint.GetX());
	_Apoint.SetY(other._Apoint.GetY());
	_Opoint.SetX(other._Opoint.GetX());
	_Opoint.SetY(other._Opoint.GetY());

	return *this;
}

