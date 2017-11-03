/*
Program	POLYLINE.CPP
Purpose	GdsII polyline structure definition
Last Update	07-12-1995
*/

#ifdef __GNUG__
#pragma implementation 
#endif

#include "units.h"
#include "polyline.h"
#include "liner.h"
#define SELECT_MARGE1

//this here is to initialize the static iterator of polyline
//with NOLIST constructor
TDLI<Segment> 	G_Polyline::_PI=TDLI<Segment>();

void CalcR(Point& begin_p,Point& middle_p,Point& end_p,double& Radius,Point& center_p);

G_Polyline::G_Polyline()
{
	_hasarcs=false;
}


G_Polyline::~G_Polyline()
{
	MakeEmpty();
	_hasarcs=false;
}

void G_Polyline::MakeEmpty()
{
	_PI.Attach(this);
	//empty the polyline
	_PI.delete_all();
	_PI.Detach();
	_hasarcs=false;
}

bool G_Polyline::ContainsArcs()
{
   return _hasarcs;
}

bool G_Polyline::CheckForArcs()
{
	_PI.Attach(this);
	_PI.tohead();
	while (!_PI.hitroot())
	{
		if (_PI.item()->GetType()==ARC ) {
		   _PI.Detach();
      	return true;
      }
		_PI++;
	}
   _PI.Detach();
   return false;
}

bool G_Polyline::CheckForPerfectArc(Point& middle, double& radius)
{
	_PI.Attach(this);
	_PI.tohead();
	if (_PI.count()==2 && _PI.item()->GetType()==ARC )
   {
      double x=_PI.item()->GetX();
      double y=_PI.item()->GetY();
		Arc_Seg *aseg=(Arc_Seg*) _PI.item();
      middle.SetX(aseg->GetOX());
      middle.SetY(aseg->GetOY());
		radius = sqrt(pow(x-aseg->GetOX(),2.0) + pow(y-aseg->GetOY(),2.0));
      //now check if the next point is equal
      _PI++;
      if ((x==_PI.item()->GetX()) && y==_PI.item()->GetY())
      {
         _PI.Detach();
         return true;
      }
   }
	_PI.Detach();
   return false;
}

G_Polyline& G_Polyline::operator=(G_Polyline &other)
{
	_PI.Attach(&other);
   assert(count() == 0);
	_PI.tohead();
	while (!_PI.hitroot())
	{
	  if (_PI.item()->GetType()==LINE)
	  {
		  Line_Seg *segn,*seg=(Line_Seg*)_PI.item();
		  segn = new Line_Seg();
        *segn=*seg;
		  insend(segn);
	  }
	  else
	  {
		  Arc_Seg *asegn,*aseg=(Arc_Seg*) _PI.item();
		  asegn = new Arc_Seg();
        *asegn=*aseg;
		  insend(asegn);
	  }
	  _PI++;
	}

	_PI.Detach();
   _hasarcs=other._hasarcs;
	return *this;
}

G_Polygon& G_Polygon::operator=(G_Polygon &other)
{
	G_Polyline::operator=((G_Polyline&)other);
   _hasholes=other._hasholes;
	return *this;
}


bool G_Polyline::RemoveZero()
{
   double x,y;
	_PI.Attach(this);
	_PI.tohead();
	while (!_PI.attail() && _PI.count()>1)
	{
	  _PI.next_wrap();
	  x=_PI.item()->GetX();y=_PI.item()->GetY();
	  _PI.prev_wrap();
     if (x==_PI.item()->GetX() && y==_PI.item()->GetY() && _PI.item()->GetType()==LINE) //skip eqaul points
     {
        delete _PI.item();
        _PI.remove();
     }
     else
	     _PI++;
   }
	_PI.Detach();
   _hasarcs=CheckForArcs();
   return true;
}

bool G_Polyline::PointOnPolyline(Point* P, double width)
{
	bool	result = false;
	double	distance;
	Point		p1,p2;
	wxLine 		line1;

	// alle punten van het polygon aflopen totdat we zeker weten of het
	// punt wel of niet binnen het polygon zit.
	_PI.Attach(this);

	_PI.tohead();
	p2.Set(_PI.item()->GetX(),_PI.item()->GetY());
	while ((! _PI.hitroot())&&(! result)) {
		p1.Set(p2.GetX(),p2.GetY());
		_PI++;
      if (_PI.hitroot()) break;
		p2.Set(_PI.item()->GetX(),_PI.item()->GetY());

      if (width > sqrt(pow(p1.GetX()-P->GetX(),2)+pow(p1.GetY()-P->GetY(),2)))
        	result=true;
   	else if (!((p1.GetX() == p2.GetX()) && (p1.GetY() == p2.GetY())))
      {
			wxLine line1(p1,p2);
			if (line1.PointInLine(*P,distance,width/2.0) == R_IN_AREA)
         	result=true;
      }
	}

	_PI.Detach();
	return result;
}

bool G_Polyline::PointOnArcseg(Point* P,double width)
{
	/*
	 * o = centre of Arcseg, a = a point on the Arcseg,
	 * p1 = start of Arcseg, p2 = end of Arcseg.
	 *
	 * n & m = temporary variables
	 * rP = distance of P to o, rp1 = distance of p1 to o
	 * h?? = angle in rad
	 */

	_PI.Attach(this);

	if (! _PI.item()->GetType()==ARC )
   {
		_PI.Detach();
   	return false;
   }
	Point		o,a,p1,p2;
	double	n,m,rP,rp1, ha,hp1,hp2,hP;
	Arc_Seg* aseg=(Arc_Seg*) _PI.item();

	// retrieve points
	p1.SetX(_PI.item()->GetX());
	p1.SetY(_PI.item()->GetY());
	o.SetX(aseg->GetOX());
	o.SetY(aseg->GetOY());
	a.SetX(aseg->GetAX());
	a.SetY(aseg->GetAY());
	_PI++;
	p2.SetX(_PI.item()->GetX());
	p2.SetY(_PI.item()->GetY());

	// calculate radius
	n = p1.GetX() - o.GetX();
	m = p1.GetY() - o.GetY();
	rp1 = sqrt(n*n + m*m);
	n = P->GetX() - o.GetX();
	m = P->GetY() - o.GetY();
	rP = sqrt(n*n + m*m);

	// check if point lies in circle
	if (width != 0)
   {
		if ((rp1 + width/2.0 < rP) || (rp1 - width/2.0 > rP))
      {
			_PI.Detach();
       	return false;
      }
   }
	else
   {
		if (rp1 + GLOB->SELECT_SNAPFACTOR() < rP)
      {
			_PI.Detach();
       	return false;
      }
   }

	// calculate angles
	hp1 = atan2(p1.GetY() - o.GetY(),p1.GetX() - o.GetX());
	hp2 = atan2(p2.GetY() - o.GetY(),p2.GetX() - o.GetX());
	ha = atan2(a.GetY() - o.GetY(),a.GetX() - o.GetX());
	hP = atan2(P->GetY() - o.GetY(),P->GetX() - o.GetX());

	if (hp2 > hp1) {
		n = hp1;
		hp1 = hp2;
		hp2 = n;
	}

	// check if hP lies on the same Arcseg as ha
	if ((hp2 > ha) && (hp1 < ha)) {
		if ((hp2 > hP) && (hp1 < hP))
      {
			_PI.Detach();
       	return true;
      }
	} else {
		if ((hp2 < hP) && (hp1 > hP))
      {
			_PI.Detach();
       	return true;
      }
	}

	_PI.Detach();

	return false;
}

BoundingBox* G_Polyline::CalcBoundingBox(BoundingBox* a_box)
{
	if (_hasarcs)
	{
		G_Polyline* dummy = new G_Polyline();
		a_box->SetValid(true);
		*dummy=*this;
      dummy->ConvertArcs(GLOB->ARCTOPOLYABER());
      dummy->CalcBoundingBox(a_box);
      // Is the next line ok?
      a_box->EnLarge(GLOB->ARCTOPOLYABER());
		delete dummy;
	}
	else
	{
		_PI.Attach(this);
		Point a;
		a_box->SetValid(false);
		_PI.tohead();
		while (!_PI.hitroot())
		{
			a.SetX(_PI.item()->GetX());
			a.SetY(_PI.item()->GetY());
			// make _boundingbox bigger if the link makes the graph bigger
			if	(!a_box->GetValid())
			{
				a_box->SetBoundingBox(a);
				a_box->SetValid(true);
			}
			else
				a_box->Expand(a);
			_PI++;
		}
		_PI.Detach();
	}

	return a_box;
}

void G_Polyline::Transform(wxTransformMatrix* _Matrix)
{

   if (_Matrix->IsIdentity()) return;

	_PI.Attach(this);

	Point a;
	_PI.tohead();
	while (!_PI.hitroot())
	{
		a.SetX(_PI.item()->GetX());
		a.SetY(_PI.item()->GetY());
		a.Transform(_Matrix);
		_PI.item()->SetX(a.GetX());
		_PI.item()->SetY(a.GetY());
		if (_PI.item()->GetType()==ARC )
		{
			Arc_Seg* aseg=(Arc_Seg*) _PI.item();
			a.SetX(aseg->GetAX());
			a.SetY(aseg->GetAY());
			a.Transform(_Matrix);
			aseg->SetAX(a.GetX());
			aseg->SetAY(a.GetY());

			a.SetX(aseg->GetOX());
			a.SetY(aseg->GetOY());
			a.Transform(_Matrix);
			aseg->SetOX(a.GetX());
			aseg->SetOY(a.GetY());

		}
		_PI++;
	}
	_PI.Detach();
}

void G_Polyline::AddSegment(Segment* a)
{
	if (a->GetType()==ARC )
	   _hasarcs=true;
	insend(a);
}


void G_Polyline::ConvertArcs(double aber)
{
	Point Mid;
	double r;
	double beginrad,endrad,midrad,phit,dphi;
	double Segments;

	_PI.Attach(this);

	_PI.tohead();
	while (!_PI.hitroot())
	{ if (_PI.item()->GetType()==ARC )//convert arc to segments
	  {
			CalcArc(&_PI,Mid,r,beginrad,midrad,endrad,phit);
			Line_Seg* lseg;

			delete _PI.item();
			_PI.remove();

         if (aber >= r)
           dphi=M_PI_4;
         else
			  dphi=2.0*acos((r-aber)/r);

         //smaller the one degree?
         if (dphi < M_PI/180)
            dphi=M_PI/180.0;

         if (dphi > M_PI_4)
            dphi=M_PI_4;

			//set the number of segments
			Segments=ceil(phit/dphi);

			dphi=phit/(Segments);

			if (midrad > endrad)
			  dphi=-dphi; //Clockwise

			for (int i=1; i<=Segments; i++)
			{ double x,y;
			  x=Mid.GetX()+r*cos(beginrad);
			  y=Mid.GetY()+r*sin(beginrad);
			  lseg = new Line_Seg(x,y);
			  _PI.insbefore(lseg);
			  beginrad+=dphi;
			}
	  }
	  else
	    _PI++;
	}
	_PI.Detach();
   _hasarcs=false;
}

// variation on ConvertArcs, this function converts an arc into segments were
// the number of segments is determined by dphi (maximum angle of a piece) and
// minseg (the arc will contain a minimum of minseg segments).
// e.g. if you specify dphi on 10 a circle will be converted into 36 segments,
//	     however if you specify minseg as 40, it will be converted to 40 segments.
void G_Polyline::ConvertArcs2(double dphi,int minseg)
{
	Point Mid;
	double r;
	double beginrad,endrad,midrad,phit;
	double Segments;

   //convert dphi to radians first
   dphi = dphi* M_PI/180.0;

	_PI.Attach(this);
	_PI.tohead();
	while (!_PI.hitroot()) {
		if (_PI.item()->GetType()==ARC )							//convert arc to segments
			{
			CalcArc(&_PI,Mid,r,beginrad,midrad,endrad,phit);
			Line_Seg* lseg;

			delete _PI.item();
			_PI.remove();

			//set the number of segments

			Segments = (ceil(phit/dphi) < minseg?minseg:ceil(phit/dphi));

			dphi=phit/(Segments);
			if (midrad > endrad) dphi=-dphi; 				//Clockwise

			for (int i=1; i<=Segments; i++) {
				double x,y;
				x=Mid.GetX()+r*cos(beginrad);
				y=Mid.GetY()+r*sin(beginrad);
				lseg = new Line_Seg(x,y);
				_PI.insbefore(lseg);
				beginrad+=dphi;
			}
	  }
     else
         _PI++;
	}
	_PI.Detach();
   _hasarcs=false;
}



void G_Polyline::CalcArc(TDLI<Segment>* where,Point& mid,double& Radius,double& beginrad,double& midrad,double& endrad,double& phit)
{

   assert(where->item()->GetType()==ARC );

   Arc_Seg* aseg=(Arc_Seg*) where->item();

   double ax,ay,bx,by,cx,cy,ox,oy;

   ax=aseg->GetX();   ay=aseg->GetY();
   bx=aseg->GetAX();   by=aseg->GetAY();
   ox=aseg->GetOX();   oy=aseg->GetOY();

   where->next_wrap();
   cx=where->item()->GetX(); cy=where->item()->GetY();
   where->prev_wrap();

   beginrad=atan2(ay-oy,ax-ox);
   midrad=atan2(by-oy,bx-ox);
   endrad=atan2(cy-oy,cx-ox);
   midrad=midrad-beginrad;
   endrad=endrad-beginrad;
	if (midrad<=0) midrad+=2.0*M_PI;
	if (endrad<=0) endrad+=2.0*M_PI;

	//middle gives direction
	if (midrad < endrad)
		phit=endrad; //counterclockwise
	else
		phit=2.0*M_PI-endrad; //Clockwise

	if (beginrad<0) beginrad+=2.0*M_PI;
	midrad=midrad+beginrad;
	endrad=endrad+beginrad;

	mid.Set(ox,oy);
	Radius = sqrt(pow(ax-ox,2) + pow(ay-oy,2));
}

void G_Polyline::Copy(G_Polyline* other)
{
	_PI.Attach(this);

	TDLI<Segment> 	PIT=TDLI<Segment>(other);

	PIT.tohead();
	_PI.tohead();

	while (!_PI.hitroot() || !PIT.hitroot())
	{
		*(_PI.item())=*(PIT.item());
		_PI++;
		PIT++;
	}

	//delete the rest
	while (!_PI.hitroot())
	{
		delete _PI.item();
		_PI.remove();
	}

	while (!PIT.hitroot())
	{
	  if (PIT.item()->GetType()==LINE)
	  {
		  Line_Seg *segn,*seg=(Line_Seg*)PIT.item();
		  segn = new Line_Seg(seg->GetX(),seg->GetY());
		  _PI.insend(segn);
	  }
	  else
	  {
		  Arc_Seg *asegn,*aseg=(Arc_Seg*) PIT.item();
		  asegn = new Arc_Seg(aseg->GetX(),aseg->GetY(),aseg->GetAX(),aseg->GetAY(),aseg->GetOX(),aseg->GetOY());
		  _PI.insend(asegn);
	  }
	  PIT++;
	}

	_PI.Detach();
   _hasarcs=other->_hasarcs;
}


G_Polygon::G_Polygon()
{
	_hasarcs=false;
	_hasholes=false;
}


G_Polygon::~G_Polygon()
{
	MakeEmpty();
	_hasarcs=false;
   _hasholes=false;
}

bool G_Polygon::RemoveZero()
{
   double x,y;
	_PI.Attach(this);
	_PI.tohead();
	while (!_PI.hitroot() && _PI.count()>1)
	{
	  _PI.next_wrap();
	  x=_PI.item()->GetX();y=_PI.item()->GetY();
	  _PI.prev_wrap();
     if (x==_PI.item()->GetX() && y==_PI.item()->GetY() && _PI.item()->GetType()==LINE) //skip eqaul points
     {
        delete _PI.item();
        _PI.remove();
     }
     else
	     _PI++;
   }
	_PI.Detach();
   _hasarcs=CheckForArcs();
   return true;
}

//calculate the area of simple polygons (not selfintersecting)
//coordinates may be negatif
double G_Polygon::CalcArea()
{
	_PI.Attach(this);
   int t=_PI.count();
	if (t<3)
   {
		_PI.Detach();
   	return 0.0;
   }
	double area = 0.0;

	_PI.tohead();
   double miny = _PI.item()->GetY();
	while (!_PI.hitroot())
	{
		if (_PI.item()->GetY() < miny )
         miny=_PI.item()->GetY();
		_PI++;
	}

   if (miny > 0 )
       miny=0;

	_PI.tohead();
	for (int i=0;i<t;i++)
	{
	   double x=_PI.item()->GetX();
      double y=_PI.item()->GetY()-miny;
		_PI.next_wrap();
		area = area + (_PI.item()->GetY()-miny) * x;
		area = area - _PI.item()->GetX() * y;
	}
	_PI.Detach();
	return (fabs(area / 2.0));
}


// determines the direction of the polygon, all polygons must be in a
// clockwise order to avoid conflics with certain algortihms. When
// the polygon is in a non-clockwise order the area of the polygon is in
// a clockwise order.
// returns true if the polygon is in clockwise order
bool G_Polygon::DirectionIsClockWise()
{
	return (bool)(CalcArea()< 0.0);
}


/*****************************************************
 * Code by OK                                        *
 *****************************************************/

void  G_Polygon::GetLRO(const Point& P, const Point& p1, const Point& p2, int &LRO1, int &LRO2,const double marge)
{
	if (p1.GetX() > (P.GetX() + marge)) LRO1 = -1;			// beginnode is right of P
	else
		if (p1.GetX() < (P.GetX() - marge)) LRO1 = 1;		// beginnode is left of P
		else LRO1 = 0;													// beginnode is on vertical line through P

	if (p2.GetX() > (P.GetX() + marge)) LRO2 = -1;			// endnode is right of P
	else
		if (p2.GetX() < (P.GetX() - marge)) LRO2 = 1;		// endnode is left of P
		else LRO2 = 0;													// endnode is on vertical line through P
}

int  G_Polygon::PointInPolygon(const Point& P, double width)
{
	int		R_tot = 0, L_tot = 0;
	int		p1_LRO, p2_LRO;
	double	px = P.GetX(), py = P.GetY();
	double	Y_intersect,distance;
	Point		p1,p2,o,a;

	// alle punten van het polygon aflopen totdat we zeker weten of het
	// punt wel of niet binnen het polygon zit.
	_PI.Attach(this);

	_PI.tohead();
	while (! _PI.hitroot())
   {
		p1.Set(_PI.item()->GetX(),_PI.item()->GetY());
		_PI.next_wrap();
		p2.Set(_PI.item()->GetX(),_PI.item()->GetY());
		_PI.prev_wrap();

		if (width)
      {						// polygon has a width
      	if (!((p1.GetX() == p2.GetX()) && (p1.GetY() == p2.GetY())))
         {
            wxLine line1(p1,p2);
				if ((line1.PointInLine(P,distance,width/2.0))==R_IN_AREA)
            {
               _PI.Detach();
               return INSIDE_POLY;
            }

         } else continue;
		} else {
			GetLRO(P,p1,p2,p1_LRO,p2_LRO,GLOB->ACCUR());
			if (p1_LRO != p2_LRO)
         {
				int L = 0, R = 0;
				if (p2_LRO == -1) { R = -p1_LRO; L = 1; }
				if (p2_LRO == 0) if (p1_LRO == 1) R = -1; else L = -1;
				if (p2_LRO == 1) { R = 1; L = p1_LRO; }

				// calculate intersection point with line for px
				if (p1_LRO == 0)
            {
					if ((p1.GetY() < (py + GLOB->SELECT_SNAPFACTOR())) && (p1.GetY() > (py - GLOB->SELECT_SNAPFACTOR())))
               {
                  _PI.Detach();
                  return ON_POLY;
               }
					else
               	Y_intersect = p1.GetY();
            }
				else if (p2_LRO == 0)
            {
               if ((p2.GetY() < (py + GLOB->SELECT_SNAPFACTOR())) && (p2.GetY() > (py - GLOB->SELECT_SNAPFACTOR())))
               {
                  _PI.Detach();
                  return ON_POLY;
               }
               else
               	Y_intersect = p2.GetY();
            }
            else //both p2_LRO and p1_LRO not 0
            {
               if ((p1.GetY() > (py + GLOB->SELECT_SNAPFACTOR())) && (p2.GetY() > (py + GLOB->SELECT_SNAPFACTOR())))
                  Y_intersect = p1.GetY(); //a save value to check later
               else if ((p1.GetY() < (py- GLOB->SELECT_SNAPFACTOR())) && (p2.GetY() < (py - GLOB->SELECT_SNAPFACTOR())))
                  Y_intersect = p1.GetY(); //a save value to check later
               else //need to calculate intersection
               {
                  if (!((p1.GetX() == p2.GetX()) && (p1.GetY() == p2.GetY())))
                  {
                     wxLine line1(p1,p2);
                     line1.CalculateLineParameters();
                     Y_intersect = line1.Calculate_Y(px);
                  }
                  else
                     continue;
               }
            }
				if (Y_intersect > (py + GLOB->SELECT_SNAPFACTOR()))
            {
					R_tot += R;
					L_tot += L;
				}
            else if ((Y_intersect <= (py + GLOB->SELECT_SNAPFACTOR())) && (Y_intersect >= (py - GLOB->SELECT_SNAPFACTOR())))
            {
               _PI.Detach();
               return ON_POLY;
            }
		   }
		}
		_PI++;
	}

	_PI.Detach();
	// geef het juiste resultaat terug
	if (R_tot == 0)
		if (L_tot == 0) return OUTSIDE_POLY;
		else return ON_POLY;
	else
		if (L_tot == 0) return ON_POLY;
		else return INSIDE_POLY;
}


//*******************************************************************
// De punten van de polygon worden getest of ze converteerd kunnen
// worden naar ARC's.
// Conversie vind plaats als; 	aberatie < aber
//				Rmin < radius < Rmax
//*******************************************************************
void G_Polygon::ConvertLine(double aber,double Rmin,double Rmax)
{

	int n,minimum_points;

  	Point center_p;
  	minimum_points = 3; 	// alleen polygonen met meer dan 3 punten kunnen
								// geconverteerd worden/

   Line_Seg* lsegn;
	if ( count())
   {
   	lsegn = new Line_Seg( ((Segment*)headitem())->GetX(), ((Segment*)headitem())->GetY());
   	insend(lsegn);
   }
   else
      return;

	if ( count() > minimum_points)
	{

// Polygon wordt getest op ARC's, wordt er een ARC gevonden dan wordt het beginpunt naar
// het eindpunt van de ARC gezet. Dit voorkomt dat een ARC niet herkent wordt wanneer
// het beginpunt zich ergens op de ARC bevind.
// In het geval dat alle punten voldoen aan een ARC (circel) vind de conversie direct plaats.
// In alle andere gevallen wordt het testen op ARC's en het converteren gedaan door
// G_Polyline::ConvertLine.

		_PI.Attach(this);
      _PI.tohead();
      for (int count=0;count < _PI.count();count++)
      {
         n = TestArc(&_PI,aber,Rmin,Rmax,center_p);

         if (n < _PI.count() && n > minimum_points)
         {
            _PI>>(n-1);	// verschuiving beginpunt naar eindpunt ARC.
            _PI.removetail();
            _PI.reset_head();
            lsegn->SetX(_PI.item()->GetX());
            lsegn->SetY(_PI.item()->GetY());
            _PI.insend(lsegn);

            break;
         }
         else  if (n > minimum_points) // alle punten van de polygon voldoen aan een ARC (circle)
         {
            _PI.tohead();
            insertArc(&_PI,n,center_p);
            _PI.removetail();
            delete lsegn;
            _PI.Detach();
			   _hasarcs=CheckForArcs();
            return;
         }

         _PI++;
         _PI.removetail();
         _PI.reset_head();
         lsegn->SetX(_PI.item()->GetX());
         lsegn->SetY(_PI.item()->GetY());
         _PI.insend(lsegn);
      }
      _PI.Detach();

  		G_Polyline::ConvertLine(aber,Rmin,Rmax); // De punten van de polyline die voldoen aan een circelboog
  	}					  	 // worden geconverteerd naar een ARC.

   removetail();
   delete lsegn;
   _hasarcs=CheckForArcs();
}



void G_Polyline::ConvertLine(double aber,double Rmin,double Rmax)
{
 	int n,minimum_points;

  	Point center_p;   //center punt van een ARC

 	minimum_points = 3;

	if (count() > minimum_points) 	// Alleen polylijnen met meer dan 3 punten kunnen
	{				// worden geconverteerd naar ARC's
		_PI.Attach(this);
   	_PI.tohead();
      _PI++;_PI++;_PI++;
      while (!_PI.hitroot())
      {
	      _PI--;_PI--;_PI--;
         n= TestArc(&_PI,aber,Rmin,Rmax,center_p);

         if (n > minimum_points )
            insertArc(&_PI,n,center_p);
         else _PI++;

         if (!_PI.hitroot()) _PI++; // Als het resterende aantal punten minder is dan 3
         if (!_PI.hitroot()) _PI++; // wordt het testen op ARC's gestopt.
         if (!_PI.hitroot()) _PI++;
      }
		_PI.Detach();
 	}
   _hasarcs=CheckForArcs();
}

//*************************************************************************************
// Een rij punten (minimaal 4) van een polylijn worden getest of deze voldoen aan
// een ARC.
// Er zijn 3 opties:
//		     1) aber > 0; conversie vind plaats als aberatie kleiner is
//				  dan de opgegeven aber.
//		     2) aber = 0; conversie vind plaats als de punten "exact" (binnen een
//				  een bepaalde marge) op een cirkelboog bevinden. De
//				  De aberatie mag heel groot zijn. (in principe oneindig)
//		     3) aber < 0; conversie vind plaats als de punten "exact" (binnen een
//				  een bepaalde marge) op een cirkelboog bevinden en als
//				  aberatie kleiner is dan de opgegeven |aber|.
//
// Definitie van aberatie    : afwijking tussen lijnsegment en circelboog
// Definitie van lijnsegment : lijn tussen twee opeenvolgende punten
//
// input : aber (maximale aberatie)
// 	   Rmin (minimum radius)
// 	   Rmax (maximum radius)
//
// output: center_p_old (centerpunt van circelboog)
//
// return: i (aantal punten die voldoen aan een circelboog)
//*************************************************************************************

int G_Polyline::TestArc(TDLI<Segment>* _PI,double aber, double Rmin, double Rmax ,Point& center_p_old)
{
   Point begin_p,middle_p, test_p, end_p,center_p ;

   TDLI<Segment> 	middle=TDLI<Segment>(this); // first make iterator
   TDLI<Segment> 	test=TDLI<Segment>  (this); // first make iterator
   TDLI<Segment> 	end=TDLI<Segment>   (this); // first make iterator

   double error1, error2, Radius,Radius_old;
   double x_between, y_between, dx, dy, x_prev, y_prev;
   double inp,phi,phi_old,phi_fac,a,b,lseg,lseg_old,seg_fac;
   double t_inp;
   int    i, j,ii,minimum_points;

   middle.toiter(_PI); // Alle iterators wijzen naar het eerste punt
   test.toiter(_PI);
   end.toiter(_PI);

   Radius   = GROOT;
   error1   = GROOT;
   error2   = GROOT;
   phi      = 0.0;
   lseg_old = 0.0;
   phi_old  = 0.0;

   if (aber < KLEIN) 		// aber <= 0 ; test op "perfecte circelbogen"
   {
      phi_fac  = 1+PHI_MARGE;  	// hoek factor   ; geeft aan hoeveel de segmenthoeken onderling mogen afwijken
      seg_fac  = 1+SEG_MARGE;	// segment factor; geeft aan hoeveel de segmentlengtes onderling mogen afwijken

      if (aber < 0.0)		// aber < 0; zie punt 3 in de header
         aber = -aber;
      else			// aber = 0; zie punt 2 in de header
         aber = GROOT;
   }
   else				// aber > 0; zie punt 1 in de header
   {
      phi_fac  = GROOT;
      seg_fac  = GROOT;
   }

   minimum_points = 3;

   begin_p.Set(_PI->item()->GetX(),_PI->item()->GetY()); 	// Beginpunt

   for (ii = 1;ii <= minimum_points;ii++)  	// Eindpunt wordt 3 posities opgeschoven en er wordt tevens
     {// getest of een van de punten niet behoord tot het type ARC
      if (end.item()->GetType()!=ARC)
	   	end++;
      else
      	return (0);			// Verlaat de functie indien een van de punten van het type ARC is.
     }

	middle++;

   i = minimum_points;

   do
   {
      i++;
      if (i % 2 != 0)        // Middelpunt schuift 1 punt op als aantal punten oneven is
         middle++;

      end_p.Set(end.item()->GetX(),end.item()->GetY()); 	 // Eindpunt

      middle_p.Set(middle.item()->GetX(),middle.item()->GetY()); // Middelpunt
      Radius_old   = Radius;
      center_p_old = center_p;

      // Als het laatste punt gelijk is aan beginpunt dan niet opnieuw ARC berekenen

      if (!((fabs(end_p.GetX()-begin_p.GetX()) < GLOB->ACCUR()) && (fabs(end_p.GetY()-begin_p.GetY())< GLOB->ACCUR())))
      	CalcR(begin_p,middle_p,end_p,Radius,center_p);  // Berekening radius en centerpunt

      test.toiter(_PI);

      j=1;

      test_p.Set(test.item()->GetX(),test.item()->GetY()); // Eerste testpunt

      while ( j<i )  // Alle punten liggende tussen begin en eindpunt worden getest
      {		     		// of ze voldoen aan de berekende circelboog
         j++;
         x_prev = test_p.GetX(); // Testpunt (1)
         y_prev = test_p.GetY();

         test++;

         test_p.Set(test.item()->GetX(),test.item()->GetY()); // Testpunt (2)

         dx = center_p.GetX() - test_p.GetX();
         dy = center_p.GetY() - test_p.GetY();

         error1 = fabs(Radius - sqrt(pow(dx,2)+pow(dy,2))); // Afwijking tussen berekende circelboog en testpunt

         x_between = (test_p.GetX() + x_prev)/2.0;  // Berekening punt tussen twee testpunt 1 en testpunt 2,
         y_between = (test_p.GetY() + y_prev)/2.0;  // dit is nodig voor berekening van de aberatie

         dx = center_p.GetX() - x_between;
         dy = center_p.GetY() - y_between;

         error2 = fabs(Radius - sqrt(pow(dx,2)+pow(dy,2))); // Aberatie

         inp = (center_p.GetX() - x_prev)*(test_p.GetY() - center_p.GetY()) - /* a x b (vectoren) */
             (center_p.GetY() - y_prev)*(test_p.GetX() - center_p.GetX());

         a = sqrt(pow((center_p.GetX() - x_prev),2)+pow((center_p.GetY() - y_prev),2));
         b = sqrt(pow((center_p.GetX() - test_p.GetX()),2)+pow((center_p.GetY() - test_p.GetY()),2));

         phi_old = phi;

         t_inp = inp/(a*b);

         if (t_inp > 1.0)     // t_inp mag niet groter worden dan 1 of kleiner dan -1
            t_inp = 1.0;
         else
         if (t_inp < -1.0)
            t_inp = -1.0;

         phi = 180.0*asin(t_inp)/M_PI; 	// Segment hoek

         lseg_old = lseg;

         lseg = sqrt(pow((test_p.GetX() - x_prev),2)+pow((test_p.GetY() - y_prev),2)); // segment lengte

         if (j == 2)
         {
	    		phi_old    = phi;
	    		lseg_old   = lseg;
         }

         if (error1 > aber || 		  	  			   /* Afwijking punt groter dan aber */
             error2 > aber || 			  				/* Aberatie lijn groter dan aber */
             fabs(phi) > fabs(phi_old)*phi_fac ||  /* Segment hoek groter vorige hoek */
             fabs(phi) < fabs(phi_old)/phi_fac ||  /* Segment hoek kleiner vorige hoek */
             phi*phi_old < 0.0 ||		  				/* Segment hoek tegengestelde richting van vorige hoek */
             lseg > lseg_old*seg_fac ||		  		/* Segment lengte groter vorige segment */
             lseg < lseg_old/seg_fac)		  			/* Segment lengte kleiner vorige segment */
         {
            if (Radius_old > Rmin && Radius_old < Rmax)	// Als de nieuw gevonden circelboog niet
               														// voldoet dan wordt getest of de vorige
               														// radius binnen Rmin en Rmax ligt.
               return (i-1);	// Return aantal gevonden punten -1
               					// (laatste punt voldoet niet)
            else
               return (0);		// Gevonden radius voldoet niet
         }
      }
      //een ARC segment is gevonden dat aan alle regels voldoet
      //indien mogelijk proberen we nog een punt verder te komen
      if (end.item()->GetType()==ARC )	// stop het huidige segment is een arc
         break;

      end++; // probeer met het nieuwe eindpunt een nieuwe circelboog te berekenen.

   }while (!end.hitroot());

   if (Radius > Rmin && Radius < Rmax)	// nieuw gevonden circelboog oke?
      {
   	center_p_old = center_p;
      return (i);
      }
   else
      return (0);	// Gevonden radius voldoet niet
}

//*******************************************************************************************
// Berekening radius en centerpunt van een circelboog bestaande uit drie punten;
//
// input  : 	- begin_p (beginpunt)
// 		- middle_p (middenpunt,een punt tussen begin en eind punt, op de circelboog)
// 		- end_p (eindpunt)
//
// output :	- Radius
// 		- center_p (centerpunt)
//*******************************************************************************************
void CalcR(Point& begin_p,Point& middle_p,Point& end_p,double& Radius,Point& center_p)
{
   double ax,ay,bx,by,cx,cy;
   double ax2,ay2,bx2,by2,cx2,cy2;
   double mx,my,d;

   ax=begin_p.GetX();   ay=begin_p.GetY();   ax2=pow(ax,2);ay2=pow(ay,2);
   bx=middle_p.GetX();  by=middle_p.GetY();  bx2=pow(bx,2);by2=pow(by,2);
   cx=end_p.GetX();     cy=end_p.GetY();     cx2=pow(cx,2);cy2=pow(cy,2);

   d=2*(ay*cx + by*ax - by*cx - ay*bx - cy*ax + cy*bx);

   if (fabs(d) < KLEIN) // protectie tegen delen door nul (oneindig grote radius)
   	d = KLEIN;

   mx= (  by * ax2 - cy * ax2 - by2 * ay + cy2 * ay + bx2 * cy + ay2 * by
        + cx2 * ay - cy2 * by - cx2 * by - bx2 * ay + by2 * cy - ay2 * cy
       )/d;

   my= (  ax2 * cx + ay2 * cx + bx2 * ax - bx2 * cx + by2 * ax - by2 * cx
        - ax2 * bx - ay2 * bx - cx2 * ax + cx2 * bx - cy2 * ax + cy2 * bx
       )/d ;

   center_p.Set(mx,my);

   Radius = sqrt(pow(ax-mx,2) + pow(ay-my,2));
}

//******************************************************************************************************************
// De punten van de poyline die voldoen aan een circelboog worden hier vervangen door een ARC
// De ARC wordt beschreven door 4 coordinaten:
//						- beginpunt
//						- eindpunt
//						- middenpunt (een punt tussen begin en eind punt, op de circelboog)
//						- centerpunt
// In principe kan een ARC beschreven worden door de eerste 3 punten maar in het bijzondere geval dat de
// ARC ook een circel is (beginpunt en eindpunt zijn het zelfde) is het nodig om de centerpunt op te geven.
//
// input : n (aantal punten die door een ARC worden vervangen)
//         center_p (centerpunt van de ARC)
//******************************************************************************************************************
void  G_Polyline::insertArc(TDLI<Segment>* PI,int n, Point& center_p)
{
	Arc_Seg* aseg;
	Point begin_p,middle_p;
	int middlepoint,i;

	middlepoint= n/2;

	begin_p.Set(PI->item()->GetX(),PI->item()->GetY()); // beginpunt van ARC

	for (i=1;i<middlepoint;i++) 	// alle punten tot aan het middenpunt worden verwijderd
	{
		delete PI->item();
		PI->remove();
	}

  	middle_p.Set(PI->item()->GetX(),PI->item()->GetY()); 	// middenpunt van ARC

	for (i=middlepoint;i<n;i++)	// alle punten vanaf het middenpunt tot aan het eindpunt worden verwijderd
	{				// (eindpunt zelf wordt niet verwijderd)
		delete PI->item();
		PI->remove();
	}
	aseg = new Arc_Seg(begin_p.GetX(),begin_p.GetY(),middle_p.GetX(),middle_p.GetY(),center_p.GetX(),center_p.GetY());

	PI->insbefore(aseg);


}

bool G_Polygon::ContainsHoles()
{
   return _hasholes;
}

bool G_Polygon::CheckForHoles()
{
	_PI.Attach(this);
	_PI.tohead();
	while (!_PI.hitroot())
	{
		if (_PI.item()->GetHoleType()==HOLE_SEG)
      {
		   _PI.Detach();
      	return true;
      }
		_PI++;
	}
   _PI.Detach();
   return false;
}

bool G_Polygon::CheckForPerfectArc(Point& middle, double& radius)
{
	_PI.Attach(this);
	_PI.tohead();
	if (_PI.count()==1 && _PI.item()->GetType()==ARC )
   {
      double x=_PI.item()->GetX();
      double y=_PI.item()->GetY();
		Arc_Seg *aseg=(Arc_Seg*) _PI.item();
      middle.SetX(aseg->GetOX());
      middle.SetY(aseg->GetOY());
		radius = sqrt(pow(x-aseg->GetOX(),2.0) + pow(y-aseg->GetOY(),2.0));
	   _PI.Detach();
	  	return true;
   }
	_PI.Detach();
   return false;
}

double  G_Polygon::CalcLength()
{
   double len=0;
	_PI.Attach(this);
	_PI.totail();
   double x=_PI.item()->GetX();
   double y=_PI.item()->GetY();
	_PI.tohead();
   len=len + sqrt(pow(x-_PI.item()->GetX(),2.0) + pow(y-_PI.item()->GetY(),2.0));
   x=_PI.item()->GetX();
   y=_PI.item()->GetY();
	while (!_PI.hitroot())
	{
      len=len + sqrt(pow(x-_PI.item()->GetX(),2.0) + pow(y-_PI.item()->GetY(),2.0));
      x=_PI.item()->GetX();
      y=_PI.item()->GetY();
		_PI++;
	}
	_PI.Detach();
   return len;
}

double  G_Polyline::CalcLength()
{
   double len=0;
	_PI.Attach(this);
	_PI.tohead();
   double x=_PI.item()->GetX();
   double y=_PI.item()->GetY();
	while (!_PI.hitroot())
	{
      len=len + sqrt(pow(x-_PI.item()->GetX(),2.0) + pow(y-_PI.item()->GetY(),2.0));
      x=_PI.item()->GetX();
      y=_PI.item()->GetY();
		_PI++;
	}
	_PI.Detach();
   return len;
}

// ---------------------------------------------------------------------------
// spline drawing code
// ---------------------------------------------------------------------------

static void gds_quadratic_spline(TDLI<Segment>&	org,double a1, double b1, double a2, double b2,
                         double a3, double b3, double a4, double b4,double aber);
static void gds_clear_stack();
static int gds_spline_pop(double *x1, double *y1, double *x2, double *y2, double *x3,
                  double *y3, double *x4, double *y4);
static void gds_spline_push(double x1, double y1, double x2, double y2, double x3, double y3,
                    double x4, double y4);

void G_Polygon::ConvertSplined(double Aber)
{
    Line_Seg* 			lseg;
    double           cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
    double           x1, y1, x2, y2;

    TDLI<Segment> 	org=TDLI<Segment>(this);
    if (org.count() <2)
        return;

    org.totail();
    x1 = org.item()->GetX();
    y1 = org.item()->GetY();

    org.tohead();
    x2 = org.item()->GetX();
    y2 = org.item()->GetY();
    lseg=new Line_Seg(x2,y2);
    org.insend(lseg);
    cx1 = (x1 + x2) / 2.0;
    cy1 = (y1 + y2) / 2.0;
    cx2 = (cx1 + x2) / 2.0;
    cy2 = (cy1 + y2) / 2.0;

  	 delete org.item();
    org.remove();
    lseg=new Line_Seg(org.item()->GetX(),org.item()->GetY());
    org.insend(lseg);

    int i=1;
    int count=org.count();
    while (i < count)
    {
        x1 = x2;
        y1 = y2;
        x2 = org.item()->GetX();
        y2 = org.item()->GetY();
        cx4 = (x1 + x2) / 2.0;
        cy4 = (y1 + y2) / 2.0;
        cx3 = (x1 + cx4) / 2.0;
        cy3 = (y1 + cy4) / 2.0;

        gds_quadratic_spline(org,cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4,Aber);

        cx1 = cx4;
        cy1 = cy4;
        cx2 = (cx1 + x2) / 2.0;
        cy2 = (cy1 + y2) / 2.0;
		  delete org.item();
        org.remove();
        i++;
    }
    org.tohead();
  	 delete org.item();
	 org.removehead();
}

void G_Polyline::ConvertSplined(double Aber)
{
    double           cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
    double           x1, y1, x2, y2;


    TDLI<Segment> 	org=TDLI<Segment>(this);
    if (org.count() <2)
        return;

    org.tohead();

    x1 = org.item()->GetX();
    y1 = org.item()->GetY();

  	 delete org.item();
    org.remove();
    x2 = org.item()->GetX();
    y2 = org.item()->GetY();
    cx1 = (x1 + x2) / 2.0;
    cy1 = (y1 + y2) / 2.0;
    cx2 = (cx1 + x2) / 2.0;
    cy2 = (cy1 + y2) / 2.0;

    Line_Seg* lseg=new Line_Seg(x1,y1);
    org.insend(lseg);

  	 delete org.item();
    org.remove();
    int i=1;
    int count=org.count();
    while (i < count)
    {
        x1 = x2;
        y1 = y2;
        x2 = org.item()->GetX();
        y2 = org.item()->GetY();
        cx4 = (x1 + x2) / 2.0;
        cy4 = (y1 + y2) / 2.0;
        cx3 = (x1 + cx4) / 2.0;
        cy3 = (y1 + cy4) / 2.0;

        gds_quadratic_spline(org,cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4,Aber);

        cx1 = cx4;
        cy1 = cy4;
        cx2 = (cx1 + x2) / 2.0;
        cy2 = (cy1 + y2) / 2.0;
	     delete org.item();
        org.remove();
        i++;
    }

    lseg=new Line_Seg(cx1,cy1);
    org.insend(lseg);

    lseg=new Line_Seg(x2,y2);
    org.insend(lseg);
}

/********************* CURVES FOR SPLINES *****************************

  The following spline drawing routine is from

    "An Algorithm for High-Speed Curve Generation"
    by George Merrill Chaikin,
    Computer Graphics and Image Processing, 3, Academic Press,
    1974, 346-349.

      and

        "On Chaikin's Algorithm" by R. F. Riesenfeld,
        Computer Graphics and Image Processing, 4, Academic Press,
        1975, 304-310.

***********************************************************************/

#define     half(z1, z2)    ((z1+z2)/2.0)
#define     THRESHOLD   5

/* iterative version */

void gds_quadratic_spline(TDLI<Segment>&	org,double a1, double b1, double a2, double b2, double a3, double b3, double a4,
                         double b4,double Aber)
{
    register double  xmid, ymid;
    double           x1, y1, x2, y2, x3, y3, x4, y4;
    Line_Seg* lseg;

    gds_clear_stack();
    gds_spline_push(a1, b1, a2, b2, a3, b3, a4, b4);

    while (gds_spline_pop(&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4))
    {
        xmid = half(x2, x3);
        ymid = half(y2, y3);
        if (fabs(x1 - xmid) < Aber && fabs(y1 - ymid) < Aber &&
            fabs(xmid - x4) < Aber && fabs(ymid - y4) < Aber)
        {
            lseg=new Line_Seg(x1,y1);
            org.insend(lseg);
            lseg=new Line_Seg(xmid,ymid);
            org.insend(lseg);
        } else {
            gds_spline_push(xmid, ymid, half(xmid, x3), half(ymid, y3),
                half(x3, x4), half(y3, y4), x4, y4);
            gds_spline_push(x1, y1, half(x1, x2), half(y1, y2),
                half(x2, xmid), half(y2, ymid), xmid, ymid);
        }
    }
}


/* utilities used by spline drawing routines */


typedef struct gds_spline_stack_struct {
    double           x1, y1, x2, y2, x3, y3, x4, y4;
}
Stack;

#define         SPLINE_STACK_DEPTH             20
static Stack    gds_spline_stack[SPLINE_STACK_DEPTH];
static Stack   *gds_stack_top;
static int      gds_stack_count;

static void gds_clear_stack()
{
    gds_stack_top = gds_spline_stack;
    gds_stack_count = 0;
}

static void gds_spline_push(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    gds_stack_top->x1 = x1;
    gds_stack_top->y1 = y1;
    gds_stack_top->x2 = x2;
    gds_stack_top->y2 = y2;
    gds_stack_top->x3 = x3;
    gds_stack_top->y3 = y3;
    gds_stack_top->x4 = x4;
    gds_stack_top->y4 = y4;
    gds_stack_top++;
    gds_stack_count++;
}

int gds_spline_pop(double *x1, double *y1, double *x2, double *y2,
                  double *x3, double *y3, double *x4, double *y4)
{
    if (gds_stack_count == 0)
        return (0);
    gds_stack_top--;
    gds_stack_count--;
    *x1 = gds_stack_top->x1;
    *y1 = gds_stack_top->y1;
    *x2 = gds_stack_top->x2;
    *y2 = gds_stack_top->y2;
    *x3 = gds_stack_top->x3;
    *y3 = gds_stack_top->y3;
    *x4 = gds_stack_top->x4;
    *y4 = gds_stack_top->y4;
    return (1);
}



