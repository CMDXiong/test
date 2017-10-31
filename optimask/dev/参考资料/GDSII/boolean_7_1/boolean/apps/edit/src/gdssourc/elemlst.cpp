/*
   Program	ELEMLST.CPP
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include <math.h>
#include <stdarg.h>

#include "kbool/booleng.h"
#include "units.h"
#include "elemlst.h"
#include "structur.h"
#include "boundary.h"
#include "path.h"
#include "arc.h"
#include "text.h"
#include "referenc.h"
#include "lsetup.h"
#include "polyline.h"
#include "drivout.h"
#include "scrndrv.h"


class ElementBool_Engine: public  Bool_Engine 
{

  public:

    ElementBool_Engine();

    ~ElementBool_Engine();

    virtual void SetState( char* state = 0 );

	 virtual void error(char *text, char *title);

    virtual void info(char *text, char *title);

};


// Prototypes for graph conversion
static void AddPolygon(Bool_Engine* booleng, GroupType A_or_B,G_Polygon* _polygon);
static G_Polygon* GetAndRemovePolygon(Bool_Engine* booleng);
static bool	ConvertToGraphList(ElementList *source, Bool_Engine* booleng,EleFlags maskA,EleFlags maskB);
static bool	ConvertToBoundaryList(int destination, Bool_Engine* booleng, ElementList* target);
static bool CheckLayer(int layer, group *g);

int bboxSorter( Element *, Element * );

//=========================== List =============================================

ElementList::ElementList()
{
   m_binhere			= false;
  	_boundingbox.SetValid(false);
}

ElementList::~ElementList()
{
   {
      TDLI<Element> _SI=TDLI<Element>(this);
      //first empty the element list
      _SI.delete_all();
   }
}

void ElementList::Write(Driver_out& dr)
{
//   dr.Write(this);
   Write((DrawDriver&)dr);
}

void ElementList::Write(DrawDriver& dr)
{
	OVERLAP backup_clip_status = dr.GetClipStatus();
	OVERLAP clip_status = dr.GetClipStatus();

  	TDLI<Element> I=TDLI<Element>(this);
   bool ident= dr.GetMatrix()->IsIdentity();

	if (I.empty()) return;

	// Only check clipping if parent is not completely inside the viewport
	if (clip_status != _OUT)
	{
      BoundingBox	absbox;
		absbox = GetBoundingBox();
		if (!ident) absbox.MapBbox(dr.GetMatrix());
      double x,y,w,h;
      dr.GetClippingBox(x, y, w, h) ;
      BoundingBox clippingregion(x,y,x+w,y+h);
  		clip_status = clippingregion.Intersect(absbox);
	}

	if (clip_status == _OUT)
	{	// This Elementlist is outside the viewport
      dr.SetClipStatus(backup_clip_status);
		return;
	}

   dr.SetClipStatus(clip_status);

	I.toroot();
	while (I.iterate())
   {
      I.item()->Write(dr);
	}
   dr.SetClipStatus(backup_clip_status);
}

void ElementList::SetTimeStamp(long event)
{
   TDLI<Element> elementi=TDLI<Element>(this);
   elementi.toroot();
   while (elementi.iterate())
          elementi.item()->SetStamp(event);
}

ElementList& ElementList::operator=(ElementList& other)
{
	_boundingbox.SetValid(false);
   m_binhere			= false;

   {
      TDLI<Element> _SI=TDLI<Element>(this);
      //first empty the element list
      _SI.delete_all();
   }
  	TDLI<Element> olditer=TDLI<Element>(&other);

	olditer.toroot();
	while (olditer.iterate())
	{
				insend(olditer.item()->Duplicate());
	}

	return *this;
}

void ElementList::SetGroupFlags(group* a_group,EleFlags mask,bool srefselected)
{
	if (! empty())
   {
	  	TDLI<Element> elementi=TDLI<Element>(this);
   	elementi.toroot();
		while (elementi.iterate())
      {
         if  (elementi.item()->GetStructure())
         {
               elementi.item()->SetFlags(mask);
               if (a_group!= 0)
               {
                  if ((a_group->GetOnlySelected() && (elementi.item()->GetSelect() || srefselected)) || (! a_group->GetOnlySelected()))
                    elementi.item()->GetStructure()->SetGroupFlags(a_group,mask,true);
               }
               else
                  elementi.item()->GetStructure()->SetGroupFlags(a_group,mask,true);
         }
         else
         {
					Shape* a_shape = (Shape*) elementi.item();
               if (a_group!= 0)
               {
                  if (CheckLayer(a_shape->GetLayer(), a_group))
                     if ((a_group->GetOnlySelected() && (elementi.item()->GetSelect() || srefselected)) || (! a_group->GetOnlySelected()))
			               a_shape->SetFlags(mask);
               }
               else
			         a_shape->SetFlags(mask);
			}
		}
	}
}

void ElementList::SetGroupFlagsRefs(EleFlags mask,bool srefselected)
{
	if (! empty())
   {
	  	TDLI<Element> elementi=TDLI<Element>(this);
   	elementi.toroot();
		while (elementi.iterate())
      {
         if  (elementi.item()->GetStructure())
         {
               if (!srefselected)
               {
                  if (elementi.item()->GetFlags() & mask ) //has it one of the the mask flags set?
                  {
                     elementi.item()->SetFlags(mask);
                     elementi.item()->GetStructure()->SetGroupFlagsRefs(mask,true);
                  }
               }
               else
               {
                     elementi.item()->SetFlags(mask);
                     elementi.item()->GetStructure()->SetGroupFlagsRefs(mask,true);
               }
			}
         else
         {
               if (srefselected)
                   elementi.item()->SetFlags(mask);
			}
		}
	}
}

void ElementList::SetFlags(EleFlags mask,bool selectedonly)
{
	if (! empty())
   {
	  	TDLI<Element> elementi=TDLI<Element>(this);
   	elementi.toroot();
		while (elementi.iterate())
      {
         if (!selectedonly || (selectedonly && elementi.item()->GetSelect()))
             elementi.item()->SetFlags(mask);
		}
	}
}

void ElementList::ClearFlags(short int mask)
{
	if (! empty())
   {
	  	TDLI<Element> elementi=TDLI<Element>(this);
   	elementi.toroot();
		while (elementi.iterate())
      {
  		   elementi.item()->ClearFlags(mask);
         if  (elementi.item()->GetStructure())
         {
               elementi.item()->GetStructure()->ClearFlags(mask);
         }
		}
	}
}

void ElementList::SetAvailable()
{
	// Get layers
  	TDLI<Element> _eliter=TDLI<Element>(this);
	_eliter.toroot();
	while (_eliter.iterate())
	{
      if  (!_eliter.item()->GetStructure())
			GLOB->SETUP->Set_Available(((Shape*)_eliter.item())->GetLayer(), true);
      else
			_eliter.item()->GetStructure()->SetAvailable();
	}
}

bool ElementList::Delete(int whichlayer)
{
	// Get layers
   bool result=false;
  	TDLI<Element> _SI=TDLI<Element>(this);
	_SI.tohead();
	while(!_SI.hitroot())
	{
      if  (!_SI.item()->GetStructure())
		{
         Shape* a_shape= (Shape*) _SI.item();
         if (a_shape->GetLayer() == whichlayer)
         {
            result=true;
            delete _SI.item();
            _SI.remove();
         }
         else
          _SI++;
      }
      else
	      _SI++;
	}
   return result;
}

void ElementList::SetReferenceNr()
{
   if (empty())
       return;

  	TDLI<Element> _SI=TDLI<Element>(this);

	_SI.toroot();
	while (_SI.iterate())
   {
      if  (_SI.item()->GetStructure())
		{
				Reference*	aref = (Reference*) _SI.item();
				aref->SetReferencesNr();
		}
	}
}

void ElementList::SetExplodeReferenceNr()
{
   if (empty())
       return;

  	TDLI<Element> _SI=TDLI<Element>(this);
	_SI.toroot();
	while (_SI.iterate()) {
      if  (_SI.item()->GetStructure())
		{
				Reference*	aref = (Reference*) _SI.item();
				aref->SetExplodeReferencesNr();
		}
	}
}


bool ElementList::AddProperty(const wxString& name,const wxString& type,const wxString& value,bool selectedonly)
{
	bool result = false;
	if (!empty())
   {
      TDLI<Element> I2=TDLI<Element>(this);
      I2.tohead();
      while (!I2.hitroot())
      {
         if (!selectedonly || (selectedonly && I2.item()->GetSelect()))
         {
            result=true;
				I2.item()->AddProperty(name,type,value);
         }
         I2++;
      }
	}
   return result;
}

bool ElementList::AddProperty(EleFlags mask,const wxString& name,const wxString& type,const wxString& value)
{
	bool result = false;
	Element* _Element;


  	TDLI<Element> _SI=TDLI<Element>(this);

	_SI.tohead();
	while(!_SI.hitroot())
	{
		_Element=_SI.item();

      if  (!_SI.item()->GetStructure())
		{
            if (_Element->GetFlags() & mask)
            {
               result=true;
					_SI.item()->AddProperty(name,type,value);
            }
            _SI++;
      }
      else
      {
            result=_SI.item()->GetStructure()->AddProperty(mask,name,type,value);
            _SI.item()->GetStructure()->SetBinHere(true);
            _SI++;
	   }
	}
	return result;
}


bool ElementList::SelectedAtPoint(Point* P)
{
	bool		found = false;

	if (!empty())
   {
      TDLI<Element> I2=TDLI<Element>(this);
      I2.tohead();
      while (!I2.hitroot() && !found)
      {
         if (I2.item()->GetSelect())
         {
            if (I2.item()->Select(P))
               found=true;
         }
         I2++;
      }
	}
	return found;
}

Element* ElementList::Select(Point* P,SelectType st)
{
	bool		result = false;
	Shape*		s;
   Element*    lastselected=0;

   SortOnBoundingBox();
	if (!empty())
   {
	  	TDLI<Element> _SI=TDLI<Element>(this);
		_SI.totail();
		while ((!_SI.hitroot())&& (!result || st==SELECTALL || st==DESELECTALL))
      {
			if (!_SI.item()->GetStructure())
         {
				s = (Shape*)_SI.item();
				if (GLOB->SETUP->Get_Visible(s->GetLayer()) && GLOB->SETUP->Get_Selectable(s->GetLayer()))
            {
               switch (st)
               {
                  case SELECTALL:
                  case DESELECTALL:
                  {
                        s->SetSelect(st==SELECTALL?true:false);
                     break;
                  }
                  case DESELECT:
                  {
                     if (s->GetSelect())
                     {
                        if (result=s->Select(P))
                        {
                           s->SetSelect(false);
                        	lastselected=s;
                        }
                     }
                     break;
                  }
                  case SELECT:
                  {
                     if (!s->GetSelect())
                     {
                        if (result=s->Select(P))
                        {
                           s->SetSelect(true);
                        	lastselected=s;
                        }
                     }
                     break;
                  }
                  default:   //query
                     if (s->Select(P))
                     {  result = true;
                        lastselected=s;
                     }
               }
            }
			} else {
           	Reference* a_reference= (Reference*) _SI.item();
            switch (st)
            {
               case SELECTALL:
               case DESELECTALL:
               {
                     a_reference->SetSelect(st==SELECTALL?true:false);
                  break;
               }
               case DESELECT:
               {
                  //is this point within an element of the referenced structure
                  if (a_reference->GetSelect())
                  {
                  	if ((result=a_reference->Select(P)))
                     {
	                     a_reference->SetSelect(false);
                        lastselected=a_reference;
                     }
                  }
                  break;
               }
               case SELECT:
               {
                  //is this point with a polygon of the referenced structure
                  if (!a_reference->GetSelect())
                  {
                  	if ((result=a_reference->Select(P)))
                     {
	                     a_reference->SetSelect(true);
                        lastselected=a_reference;
                     }
                  }
                  break;
               }
               default:     //query
						if (a_reference->Select(P))
                  {  result = true;
                     lastselected=a_reference;
                  }
            }
			}
			_SI--;
		}
	}
	return lastselected;
}

bool ElementList::Select(BoundingBox* B,SelectType st,wxTransformMatrix* m)
{
	bool		result = false;
	Shape*		s;

   SortOnBoundingBox();

	if (! empty())
   {
	  	TDLI<Element> _SI=TDLI<Element>(this);
		_SI.totail();
		while (!_SI.hitroot()) {
			if (!_SI.item()->GetStructure())
         {
            s = (Shape*)_SI.item();
				if (GLOB->SETUP->Get_Visible(s->GetLayer()) && GLOB->SETUP->Get_Selectable(s->GetLayer()))
            {
               switch (st)
               {
                  case SELECTALL:
                  case DESELECTALL:
                  {
                        s->SetSelect(st==SELECTALL?true:false);
                     break;
                  }
                  case DESELECT:
                  {
                     if (s->Select(B,m))
                        s->SetSelect(false);
                     break;
                  }
                  case SELECT:
                  {
                     if (s->Select(B,m))
                        s->SetSelect(true);
                     break;
                  }
                  default:   //query
                     if (s->Select(B,m)) result = true;
               }
            }
			} else {
           	Reference* a_reference= (Reference*) _SI.item();
            switch (st)
            {
               case SELECTALL:
               case DESELECTALL:
               {
                     a_reference->SetSelect(st==SELECTALL?true:false);
                  break;
               }
               case DESELECT:
               {
                  //is this point within an element of the referenced structure
						if (a_reference->Select(B,m))
                     a_reference->SetSelect(false);
                  break;
               }
               case SELECT:
               {
                  //is this point with a polygon of the referenced structure
						if (a_reference->Select(B,m))
                     a_reference->SetSelect(true);
                  break;
               }
               default:     //query
						if (a_reference->Select(B,m)) result = true;
            }
			}
			_SI--;
		}
	}
	return result;
}

BoundingBox& ElementList::CalcBoundingBox()
{
  	TDLI<Element> _SI=TDLI<Element>(this);

	_boundingbox.SetValid(false);
	_SI.toroot();
	while (_SI.iterate())
	{
      //Shape* a_shape= (Shape*) _SI.item();
      if (!_boundingbox.GetValid())
      {
         _boundingbox=_SI.item()->CalcBoundingBox();
         _boundingbox.SetValid(true);
      }
      else
         _boundingbox.Expand(_SI.item()->CalcBoundingBox());
	}

	_boundingbox.SetValid(true);
	return _boundingbox;
}


// copy all elements to the structure _Top and make the copies absolute  relative to _Top
//
void ElementList::Flatten(Structure* _Top,wxTransformMatrix* _Matrix,bool remove,bool refselect,EleFlags refgroup,bool deep)
{
  	TDLI<Element> _SI=TDLI<Element>(this);

	_SI.tohead();
	while (!_SI.hitroot())
	{
      if  (!_SI.item()->GetStructure())
		{
            if (remove)
            {
               Element* a_shape=_SI.item();
               a_shape->Transform(_Matrix);
               a_shape->SetSelect(refselect);
               a_shape->SetFlags(refgroup);

               _Top->insend(a_shape);
               _SI.remove();
            }
            else
            {
               Element* a_shape=_SI.item()->Duplicate();
               a_shape->Transform(_Matrix);
               // Add this Element to the end of the list
               a_shape->SetSelect(refselect);
               a_shape->SetFlags(refgroup);

               _Top->insend(a_shape);
               _SI++;
            }
      }
      else
      {
            if (remove)
            {
               _SI.item()->Transform(_Matrix);
               _SI.item()->SetSelect(refselect);
               _SI.item()->SetFlags(refgroup);
               _Top->insend(_SI.item());
               _SI.remove();
            }
            else if (!deep)
            {

               Element* a_ref=_SI.item()->Duplicate();
               a_ref->Transform(_Matrix);
               // Add this Element to the end of the list
               a_ref->SetSelect(refselect);
               a_ref->SetFlags(refgroup);

               _Top->insend(a_ref);
               _SI++;
            }
            else
               _SI++;
      }
	}
}

void ElementList::Transform(wxTransformMatrix* _Matrix,EleFlags mask)
{
   if (count() == 0)
   	return;

  	TDLI<Element> _SI=TDLI<Element>(this);

	_SI.tohead();

	while (!_SI.hitroot())
	{
		if (_SI.item()->GetFlags() & mask)
        	_SI.item()->Transform(_Matrix);
   	_SI++;
	}
   _boundingbox.SetValid(false);
   CalcBoundingBox();
   m_sortedOnBbox = false;
}

bool ElementList::Delete(TYPE a_type,EleFlags mask)
{
	bool result = false;
	Element* _Element;


  	TDLI<Element> _SI=TDLI<Element>(this);

	_SI.tohead();
	while(!_SI.hitroot())
	{
		_Element=_SI.item();

      if (_Element->GetFlags() & mask)
      {
         result=true;
         delete _SI.item();
         _SI.remove();
      }
      else
       _SI++;
	}
	return result;
}

bool ElementList::Delete_A(bool groupmode,EleFlags mask)
{
	bool result = false;
	Element* _Element;

  	TDLI<Element> _SI=TDLI<Element>(this);

	_SI.tohead();
	while(!_SI.hitroot())
	{
		_Element=_SI.item();

      if  (!_SI.item()->GetStructure())
		{
         if (_Element->GetFlags() & mask)
         {
            result=true;
            delete _SI.item();
            _SI.remove();
         }
         else
          _SI++;
      }
      else
      {
            if (_Element->GetFlags() & mask)
            {
               result=true;
               _SI.item()->GetStructure()->Delete_A(groupmode,mask);
               _SI.item()->GetStructure()->SetSortedOnBox(false);
               _SI.item()->GetStructure()->SetBinHere(true);
               if (groupmode)
               {
                  _SI++;
               }
               else
               {
                  delete _SI.item();
                  _SI.remove();
               }
            }
            else
             _SI++;
      }
	}
	return result;
}

bool ElementList::Delete(TYPE a_type,bool OnlySelected)
{
	bool result = false;
	Element* _Element;


  	TDLI<Element> _SI=TDLI<Element>(this);

	_SI.tohead();
	while(!_SI.hitroot())
	{
		_Element=_SI.item();

		if (_Element->GetType()==a_type)
		{
         if  (!_SI.item()->GetStructure())
         {
            if (OnlySelected)
            {
               if (_SI.item()->GetSelect())
               {
                  result=true;
                  delete _SI.item();
                  _SI.remove();
               }
               else
                  _SI++;
            }
            else
            {
               result=true;
               delete _SI.item();
               _SI.remove();
            }
         }
      }
		else
		  _SI++;
	}
	return result;
}


bool ElementList::Move(int destination,EleFlags mask)
{
	bool result = false;
	Element* _Element;

  	TDLI<Element> _SI=TDLI<Element>(this);

	_SI.tohead();
	while(!_SI.hitroot())
   {
		_Element=_SI.item();

      if  (!_SI.item()->GetStructure())
      {
					Shape* a_shape= (Shape*) _Element;
					if (a_shape->GetFlags() & mask)
               {
							a_shape->SetLayer(destination);
							result=true;
					}
      }
      else
      {
               if (_Element->GetStructure()->GetBinHere() == false)
               {
                  _Element->GetStructure()->Move(destination,mask);
                  _Element->GetStructure()->SetBinHere(true);
                  _Element->GetStructure()->SetSortedOnBox(false);
               }
		}
		_SI++;
	}
	return result;
}


bool ElementList::Copy(int destination,EleFlags mask)
{
	Element* _Element;

  	TDLI<Element> _SI=TDLI<Element>(this);

	_SI.tohead();
	while(!_SI.hitroot()) {
		_Element=_SI.item();

      if  (!_SI.item()->GetStructure())
      {
					Shape* a_shape= (Shape*) _Element;
					Shape* new_shape;
					if (a_shape->GetFlags() & mask)
               {
							new_shape=(Shape*)a_shape->Duplicate();
							new_shape->SetLayer(destination);
							_SI.insbegin(new_shape);
					}
		}
      else
      {
               if (_Element->GetStructure()->GetBinHere() == false)
               {
                  _Element->GetStructure()->Copy(destination,mask);
                  _Element->GetStructure()->SetBinHere(true);
                  _Element->GetStructure()->SetSortedOnBox(false);
               }
		}
		_SI++;
	}
	return true;
}

bool ElementList::CalcArea(double* area,EleFlags mask)
{
   double accur = GLOB->ARCTOPOLYABER();
	Element* _Element;

   m_binhere=true;

  	TDLI<Element> _SI=TDLI<Element>(this);

	_SI.tohead();
	while(!_SI.hitroot()) {
		_Element=_SI.item();
		switch (_Element->GetType()) {
			case ARCSEG:
			case PATH: {
				Shape* a_shape= (Shape*) _Element;
				if (a_shape->GetFlags() & mask)
            {
					if (a_shape->GetWidth()!=0)
               {
						G_Polygon* _polygon = new G_Polygon();
						a_shape->Convert(_polygon,accur);
						_polygon->ConvertArcs(accur);
                  *area=_polygon->CalcArea()+*area;
                  delete _polygon;
					}
				}
            _SI++;
				break;
			}
			case BOX:
			case BOUNDARY:
			case CIRCLE:
         {
				Shape* a_shape= (Shape*) _Element;
				if (a_shape->GetFlags() & mask)
            {
					G_Polygon* _polygon = new G_Polygon();
					a_shape->Convert(_polygon,accur);
					// This is the routine
					_polygon->ConvertArcs(accur);
               *area=_polygon->CalcArea()+*area;
               delete _polygon;
				}
            _SI++;
				break;
			}
			case TEXT:
            _SI++;
            break;
			case AREF:
			case SREF:
              ((Reference*)_SI.item())->CalcArea(area,mask);

				  _SI++;
				  break;

			default:
				throw GDS_Error("Unknown element in element list");
		}
	}
	return true;
}


bool ElementList::ToLines( int destination,EleFlags mask)
{
	Element* _Element;

  	TDLI<Element> _SI=TDLI<Element>(this);

	_SI.tohead();
	while(!_SI.hitroot()) {
		_Element=_SI.item();

      if  (!_SI.item()->GetStructure())
      {
					Shape* a_shape= (Shape*) _Element;
					Shape* new_shape;
					if (a_shape->GetFlags() & mask)
               {
							if (a_shape->GetLayer()!=destination)
                     {
								new_shape=(Shape*) a_shape->Duplicate();
								new_shape->SetLayer(destination);
								_SI.insbegin(new_shape);
							}
					}
               else
               {
					  if (a_shape->GetLayer()==destination)
                 {
							delete _SI.item();
							_SI.remove();
							_SI--;
					  }
					}
       }
       else
       {
               if (_Element->GetStructure()->GetBinHere() == false)
               {
                  _Element->GetStructure()->ToLines(destination,mask);
                  _Element->GetStructure()->SetBinHere(true);
               }
       }
		_SI++;
	}

	_SI.tohead();
	while(!_SI.hitroot()) {
		_Element=_SI.item();
		switch (_Element->GetType()) {
			case ARCSEG:
			case PATH: {
				Shape* a_shape= (Shape*) _Element;
				if (a_shape->GetLayer()==destination) {
					if (a_shape->GetWidth()==0) {
						G_Polygon* _polygon = new G_Polygon();
						a_shape->Convert(_polygon,GLOB->ARCTOPOLYABER());
						// This is the routine
						_polygon->ConvertArcs(GLOB->ARCTOPOLYABER());
						Path *tmp = new Path((G_Polyline*) _polygon, destination);
						tmp->CalcBoundingBox();
						tmp->SetLayer(destination);
						_SI.insbegin(tmp);
					} else {
						G_Polygon* _polygon = new G_Polygon();
						a_shape->Convert(_polygon,GLOB->ARCTOPOLYABER());
						// This is the routine
						_polygon->ConvertArcs(GLOB->ARCTOPOLYABER());
						Boundary *tmp = new Boundary(_polygon, destination);
						tmp->CalcBoundingBox();
						tmp->SetLayer(destination);
						_SI.insbegin(tmp);
					}
					delete _SI.item();
					_SI.remove();
				} else _SI++;
				break;
			}
			case BOUNDARY:
			case CIRCLE: {
				Shape* a_shape= (Shape*) _Element;
				if (a_shape->GetLayer()==destination) {
					G_Polygon* _polygon = new G_Polygon();
					a_shape->Convert(_polygon,GLOB->ARCTOPOLYABER());
					// This is the routine
					_polygon->ConvertArcs(GLOB->ARCTOPOLYABER());
					Boundary *tmp = new Boundary(_polygon, destination);
					tmp->CalcBoundingBox();
					tmp->SetLayer(destination);
					_SI.insbegin(tmp);
					delete _SI.item();
					_SI.remove();
				} else _SI++;
				break;
			}
			case BOX:
			case TEXT:
			case AREF:
			case SREF:
				  _SI++;
				  break;

			default:
				throw GDS_Error("Unknown element in element list");
		}
	}
	return true;
}


bool ElementList::ToArcs(int destination,EleFlags mask)
{
	Element* _Element;

  	TDLI<Element> _SI=TDLI<Element>(this);

	_SI.tohead();
	while(!_SI.hitroot())
	{
		_Element=_SI.item();

      if  (!_SI.item()->GetStructure())
      {
					Shape* a_shape= (Shape*) _Element;
					Shape* new_shape;
					if (a_shape->GetFlags() & mask)
               {
							if (a_shape->GetLayer()!=destination)
                     {
								new_shape=(Shape*)a_shape->Duplicate();
								new_shape->SetLayer(destination);
								_SI.insbegin(new_shape);
							}
					}
               else
               {
					  if (a_shape->GetLayer()==destination)
                 {
							delete _SI.item();
							_SI.remove();
							_SI--;
					  }
					}
       }
       else
       {
               if (_Element->GetStructure()->GetBinHere() == false)
               {
                  _Element->GetStructure()->ToArcs(destination,mask);
                  _Element->GetStructure()->SetBinHere(true);
               }
		}
		_SI++;
	}

	_SI.tohead();
	while(!_SI.hitroot()) {
		_Element=_SI.item();
		switch (_Element->GetType()) {
			case PATH:
         {
				Shape* a_shape= (Shape*) _Element;
				if (a_shape->GetLayer()==destination)
            {
               Path* a_path= (Path*) a_shape;
               G_Polyline* a_polyline = a_path->GetPolyline();
               // This is the routine
               a_polyline->ConvertLine(GLOB->POLYTOARCABER(), GLOB->POLYTOARCRMIN(), GLOB->POLYTOARCRMAX());
               a_path->CalcBoundingBox();
               a_path->SetLayer(destination);

					if (a_shape->GetWidth()==0)
               { //in case the is only one arc left make it a perfect circle
                  //  filled flag
                  Point a;
                  double  radius;
                  if (!a_shape->GetFilled() && a_polyline->CheckForPerfectArc(a, radius))
                  {  //just a circle with no width
							Circle* circle = new Circle();
							circle->SetRadius(radius);
							circle->SetFilled(false);
							circle->SetMid(a.GetX(),a.GetY());
                     _SI.insbegin(circle);
                     delete _SI.item();
                     _SI.remove();
							circle->SetLayer(destination);
                  }
                  else
                  {  //ready like this
                   	_SI++;
                  }
               }
					else
               { //in case the is only one arc left make it a perfect circle
                  Point a;
                  double  radius;
                  if (a_polyline->CheckForPerfectArc(a, radius))
                  {  //just a circle with width
							Circle* circle = new Circle();
							circle->SetRadius(radius);
							circle->SetFilled(a_shape->GetFilled());
							circle->SetWidth(a_shape->GetWidth());
							circle->SetMid(a.GetX(),a.GetY());
                     _SI.insbegin(circle);
                     delete _SI.item();
                     _SI.remove();
							circle->SetLayer(destination);
                  }
                  else if (a_polyline->count()==1 && a_path->GetPathtype()==PATH_END_ROUND)
                  {  //just a circle with no width
	                  Point a(((Segment*)headitem())->GetX(), ((Segment*)headitem())->GetY());
							Circle* circle = new Circle();
							circle->SetFilled(a_shape->GetFilled());
							circle->SetRadius(radius);
							circle->SetMid(a.GetX(),a.GetY());
                     _SI.insbegin(circle);
                     delete _SI.item();
                     _SI.remove();
							circle->SetLayer(destination);
                  }
                  else
                  {
                     G_Polygon* _polygon = new G_Polygon();
                     a_shape->Convert(_polygon,GLOB->ARCTOPOLYABER());
                     // This is the routine
                     _polygon->ConvertLine(GLOB->POLYTOARCABER(), GLOB->POLYTOARCRMIN(), GLOB->POLYTOARCRMAX());
                     Point a;
                     double  radius;
                     if (_polygon->CheckForPerfectArc(a, radius))
                     {
                        Circle* circle = new Circle();
								circle->SetFilled(a_shape->GetFilled());
                        circle->SetRadius(radius);
                        circle->SetMid(a.GetX(),a.GetY());
                        _SI.insbegin(circle);
                        delete _SI.item();
                        _SI.remove();
                        circle->SetLayer(destination);
                        delete _polygon;
                     }
                     else
                     {
                        Boundary *tmp = new Boundary(_polygon, destination);
								tmp->SetFilled(a_shape->GetFilled());
                        tmp->CalcBoundingBox();
                        tmp->SetLayer(destination);
                        _SI.insbegin(tmp);
                        delete _SI.item();
                        _SI.remove();
                     }

                  }
               }
				}
            else
            	_SI++;
				break;
			}

			case BOUNDARY:
         {
				Shape* a_shape= (Shape*) _Element;
				if (a_shape->GetLayer()==destination)
            {
					if( a_shape->GetWidth()!=0 )
               {
						G_Polygon* _polygon = new G_Polygon();
						a_shape->Convert(_polygon,GLOB->ARCTOPOLYABER());
						// This is the routine
						_polygon->ConvertLine(GLOB->POLYTOARCABER(), GLOB->POLYTOARCRMIN(), GLOB->POLYTOARCRMAX());
						Boundary *tmp = new Boundary(_polygon, destination);
						tmp->SetFilled(a_shape->GetFilled());
						tmp->CalcBoundingBox();
						tmp->SetLayer(destination);
						_SI.insbegin(tmp);
						delete _SI.item();
						_SI.remove();
					}
               else
               {
						Boundary* a_bound= (Boundary*) a_shape;
						G_Polygon* a_polygon = a_bound->GetPolygon();
						// This is the routine
						a_polygon->ConvertLine(GLOB->POLYTOARCABER(), GLOB->POLYTOARCRMIN(), GLOB->POLYTOARCRMAX());
                  Point a;
                  double  radius;
                  if (a_polygon->CheckForPerfectArc(a, radius))
                  {
							Circle* circle = new Circle();
							circle->SetRadius(radius);
							circle->SetFilled(a_shape->GetFilled());
							circle->SetMid(a.GetX(),a.GetY());
                     _SI.insbegin(circle);
                     delete _SI.item();
                     _SI.remove();
							circle->SetLayer(destination);
                  }
                  else
                  {
							a_bound->SetLayer(destination);
                   	_SI++;
                  }
					 }
				}
            else
            	_SI++;
				break;
			}
			case CIRCLE:
			case ARCSEG:
			case TEXT:
			case AREF:
			case SREF:
			case BOX:
				  _SI++;
				  break;

			default:
				throw GDS_Error("Unknown element in element list");
		}
	}
	return true;
}

void ElementList::Operation(OPS operation,EleFlags maskA,EleFlags maskB,int destination,bool m_destination_add)
{
   //first recursive go into referenced structures
	if (! empty())
   {
	  	TDLI<Element> elementi=TDLI<Element>(this);
   	elementi.toroot();
		while (elementi.iterate())
      {
            if  (elementi.item()->GetStructure())
            {
               if (elementi.item()->GetStructure()->GetBinHere() == false)
               {
						elementi.item()->GetStructure()->
                  				Operation(operation,maskA,maskB,destination,m_destination_add);
                  elementi.item()->GetStructure()->SetBinHere(true);
                  elementi.item()->GetStructure()->SetSortedOnBox(false);
               }
            }
		}
	}

   //in boolean data is stored as double, and the userunits is the fraction part.
   //e.g. 123.44 means userunits = 0.01
   //so to make this integer data we must multiply by 1/userunits 
   double dgrid = 1/GLOB->USERUNITS();


   ElementBool_Engine* myBooleng = new ElementBool_Engine();

   //how accurate will the boolean algorithm calculate intersections
   double GRID =10000;
/*TODO (works!)
   double scale = 1;
   if (_boundingbox.GetWidth() > _boundingbox.GetHeight())
       scale = MAXB_INT/128/dgrid/GRID/_boundingbox.GetWidth();
   else
       scale = MAXB_INT/128/dgrid/GRID/_boundingbox.GetHeight();

   dgrid=dgrid*scale;
*/

   myBooleng->SetGrid(GRID);
   myBooleng->SetDGrid(dgrid);

   //snapfactor
   double snap = GLOB->SNAPFACTOR();
   if ( snap < GLOB->USERUNITS())
   {
   	_messagehandler->info("MARGE smaller than GRID", "ElementList::Operation");
	   myBooleng->SetMarge( GLOB->SNAPFACTOR() );
   }
   else
   {
   	myBooleng->SetMarge(GLOB->SNAPFACTOR());
   }

   myBooleng->SetCorrectionFactor(GLOB->CORRECTIONFACTOR());
   myBooleng->SetCorrectionAber(GLOB->CORRECTIONABER());
   myBooleng->SetSmoothAber(GLOB->SMOOTHABER());
   myBooleng->SetMaxlinemerge(GLOB->MAXLINEMERGE());
   myBooleng->SetRoundfactor(GLOB->ROUNDFACTOR());
   myBooleng->SetWindingRule(GLOB->WINDINGRULE);
   myBooleng->SetWindingRule( TRUE );
   //myBooleng->SetWindingRule( FALSE );

   ConvertToGraphList(this, myBooleng,maskA,maskB);

   //a bad hack to preserve text elements since i can not convert them to polygons
   //the same for polylines with width is zero

   ElementList* _preserve_elements= new ElementList();
   {
      TDLI<Element> elementi=TDLI<Element>(this);
      elementi.toroot();
      while (elementi.iterate())
      {
         Shape* a_shape=(Shape*) elementi.item();
         switch (elementi.item()->GetType())
         {
            case	PATH:
				if ( a_shape->GetWidth()!=0) //path must have width to be able to convert to area
               break;
            case	TEXT:
            {
               if (a_shape->GetFlags() & maskA)
               {
                  Shape* a_shapenew=(Shape*)a_shape->Duplicate();
                  _preserve_elements->insend(a_shapenew);
                  a_shapenew->SetLayer(destination);
               }

               if (a_shape->GetFlags() & maskB)
               {
                  Shape* a_shapenew=(Shape*)a_shape->Duplicate();
                  _preserve_elements->insend(a_shapenew);
                  a_shapenew->SetLayer(destination);
               }

               break;
            }
            default:
               break;
         }
      }
   }

   switch (operation)
   {
       case(OR)		:
//								_statusbar->SetProcess("Boolean...");
            myBooleng->Do_Operation(BOOL_OR);break;
       case(AND)		:
            myBooleng->Do_Operation(BOOL_AND);break;
       case(EXOR)		:
            myBooleng->Do_Operation(BOOL_EXOR);break;
       case(A_SUB_B)	:
            myBooleng->Do_Operation(BOOL_A_SUB_B);break;
       case(B_SUB_A)	:
            myBooleng->Do_Operation(BOOL_B_SUB_A);break;
       case(CORRECTION):
            _statusbar->SetProcess("Correction...");
            myBooleng->Do_Operation(BOOL_CORRECTION);break;
       case(SMOOTHEN)  :
//								_statusbar->SetProcess("Smoothen...");
            myBooleng->Do_Operation(BOOL_SMOOTHEN);break;
       case(MAKE_RING) :
//								_statusbar->SetProcess("MakeRing...");
            myBooleng->Do_Operation(BOOL_MAKERING);break;
		 default:
		 {
			throw GDS_Error("Wrong operation", "Command Error");
		 }
   }

   if (!m_destination_add)
		Delete(destination);

   ConvertToBoundaryList(destination, myBooleng, this);

   takeover(_preserve_elements);
   delete _preserve_elements;
   delete myBooleng;
	CalcBoundingBox();
}

void ElementList::SortOnBoundingBox()
{
   if (!m_sortedOnBbox)
   {
  		TDLI<Element> _SI=TDLI<Element>(this);
		_SI.mergesort(bboxSorter);
   }
   m_sortedOnBbox = true;
}

int bboxSorter(Element* a, Element* b)
{
	Shape *tmp = (Shape*)a;
	BoundingBox temp1 = tmp->GetBoundingBox();
	tmp = (Shape*)b;
	BoundingBox temp2 = tmp->GetBoundingBox();

	double	oa,ob;

	oa = fabs((temp1.GetMin().GetX() - temp1.GetMax().GetX()) *
				(temp1.GetMin().GetY() - temp1.GetMax().GetY()));
	ob = fabs((temp2.GetMin().GetX() - temp2.GetMax().GetX()) *
				(temp2.GetMin().GetY() - temp2.GetMax().GetY()));

	if(oa >  ob) return(1);
	if(oa == ob) return(0);

	return(-1);
}


static int xySorter(Element* a, Element* b)
{
	Shape *tmp = (Shape*)a;
	BoundingBox temp1 = tmp->GetBoundingBox();
	tmp = (Shape*)b;
	BoundingBox temp2 = tmp->GetBoundingBox();

	if ( temp1.GetMin().GetX() < temp2.GetMin().GetX())
		return(-1);
	if ( temp1.GetMin().GetX() > temp2.GetMin().GetX())
		return(1);
   //they are eqaul in x
	if ( temp1.GetMin().GetY() < temp2.GetMin().GetY())
		return(-1);
	if ( temp1.GetMin().GetY() > temp2.GetMin().GetY())
		return(1);
   //they are eqaul in y
	return(0);
}

void ElementList::SortOnXYmin()
{
   m_sortedOnBbox = false;
	TDLI<Element> _SI=TDLI<Element>(this);
   _SI.mergesort(xySorter);
}


/************************************************************************/
static bool ConvertToGraphList(ElementList *source, Bool_Engine* booleng,EleFlags maskA,EleFlags maskB)
{
	bool result = false;

  	TDLI<Element> S=TDLI<Element>(source);

//	_statusbar->SetProcess("convert polygons to graphs");
	S.toroot();
	while(S.iterate())
	{
		Shape* a_shape=(Shape*) S.item();
		switch (S.item()->GetType())
		{
			case PATH:
				if ( a_shape->GetWidth()==0) //path must have width to be able to convert to area
               break;
			case BOX:
			case BOUNDARY:
			case ARCSEG:
			case CIRCLE:
			{
				// Convert shape to polygon
				if (a_shape->GetFlags() & maskA)
            {
					G_Polygon* _polygon = new G_Polygon();
					a_shape->Convert(_polygon,GLOB->ARCTOPOLYABER());
               if (a_shape->GetType()==CIRCLE || a_shape->GetType()==ARCSEG)
                     _polygon->ConvertArcs(GLOB->ARCTOPOLYABER());
               if (_polygon->count()>2) //in case nothing was left of it (one point etc.)
               {
                  result = true;

                  AddPolygon( booleng, GROUP_A,_polygon);
               }
					delete _polygon;
				}

				if (a_shape->GetFlags() & maskB)
            {
					G_Polygon* _polygon = new G_Polygon();
					a_shape->Convert(_polygon,GLOB->ARCTOPOLYABER());
               if (a_shape->GetType()==CIRCLE || a_shape->GetType()==ARCSEG)
                     _polygon->ConvertArcs(GLOB->ARCTOPOLYABER());
               if (_polygon->count()>2) //in case noting was left of it (one point etc.)
               {
                  result = true;

                  AddPolygon( booleng, GROUP_B,_polygon);
               }
					delete _polygon;
            }

			}
			break;
			default: break;
		 }
	}

	// there are boundaries that are converted
	return result;
}

static bool ConvertToBoundaryList(int destination, Bool_Engine* booleng, ElementList* Target)
{
	 _statusbar->SetProcess("convert graphs to polygons");

	G_Polygon* polygon;
	// now build a new list from the graphlist
	while ( polygon = GetAndRemovePolygon( booleng ))
	{
		Boundary * tmp = new Boundary(polygon, destination);
		tmp->CalcBoundingBox();
		Target->insbegin(tmp);
	}
	return true;
}


void AddPolygon(Bool_Engine* booleng, GroupType A_or_B,G_Polygon* _polygon)
{
   TDLI<Segment> 	Iter=TDLI<Segment>(_polygon);

   booleng->StartPolygonAdd( A_or_B );

	// make from each point in the polygon a node
	Iter.tohead();
	while (!Iter.hitroot())
	{
      booleng->AddPoint( Iter.item()->GetX(), Iter.item()->GetY() );
      Iter++;
	}
   booleng->EndPolygonAdd(); 
}

G_Polygon* GetAndRemovePolygon(Bool_Engine* booleng)
{
   // foreach resultant polygon in the booleng ...
   while ( booleng->StartPolygonGet() )
   {
		// make a new polygon
		G_Polygon *polygon = new G_Polygon();

      // foreach point in the polygon
      while ( booleng->PolygonHasMorePoints() )
      {

         Segment* seg;
			// extract the point from the link
			seg = new Line_Seg( booleng->GetPolygonXPoint(), booleng->GetPolygonYPoint() );

         if ( booleng->GetHoleConnectionSegment())
         {
         	seg->SetHoleType(LINK_SEG);
            polygon->SetHoles(true);
         }   
         else if (booleng->GetHoleSegment())
         {
         	seg->SetHoleType(HOLE_SEG);
            polygon->SetHoles(true);
         }

         polygon->insend(seg);
		}
      booleng->EndPolygonGet();
      return polygon;
   }
   return NULL;
}

static bool CheckLayer(int layer, group *g)
{
	if (g->GetLayer(layer)) return true;
	return false;
}


ElementBool_Engine::ElementBool_Engine()
{
}

ElementBool_Engine::~ElementBool_Engine()
{
}

void ElementBool_Engine::SetState( char* state )
{
	_statusbar->SetProcess( state );
}

void ElementBool_Engine::error(char *text, char *title)
{
   _messagehandler->error(text,"title");
}

void ElementBool_Engine::info(char *text, char *title)
{
   _messagehandler->info(text,"title");
}
