/*
Program	STRUCTUR.CPP
Purpose	GdsII structure definition
Programmers	R. Spekreijse & N. Noorlander
Last Update	08-12-1995
*/

#ifdef __GNUG__
#pragma implementation
#endif
#include "structur.h"
#include "elemlst.h"
#include "referenc.h"
#include <stdio.h>
#include <stdarg.h>

#include "gdsii.h"
#include "drivout.h"
#include "scrndrv.h"
#include "units.h"


// default constructor
Structure::Structure()
{
	Init("");
}


// constructor
// usage: Structure a_structure("TOP");
Structure::Structure(const wxString& nw_structurename)
{
	Init(nw_structurename);
}

void Structure::Init(const wxString& structurename)
{
	_structurename=structurename;

	_bgnstr      = new ModeTime();
	_nr_of_references = 0;
	_nr_of_explode_references = 0;

}

Structure::~Structure()
{
  	TDLI<Element> freemem=TDLI<Element>(this);
	freemem.totail();
    while (!freemem.hitroot())
      {
          delete freemem.item();
          freemem.remove();
      }
	delete _bgnstr;
}

void Structure::Write(DrawDriver& dr)
{
	// We have to backup clip status, in case this structure falls inside viewport
	// while the parent of this structure intersected with the viewport
	OVERLAP backup_clip_status = dr.GetClipStatus();
	OVERLAP clip_status = dr.GetClipStatus();

   bool ident= dr.GetMatrix()->IsIdentity();
   BoundingBox	absbox;
	absbox = GetBoundingBox();

	// Only check clipping if parent is not completely inside the viewport
	if (clip_status != _OUT)
	{
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

	double Xmin,Ymin,Xmax,Ymax;

	Xmin=dr.LogicalToDeviceX(absbox.GetMin().GetX());
	Ymin=dr.LogicalToDeviceY(absbox.GetMin().GetY());
	Xmax=dr.LogicalToDeviceX(absbox.GetMax().GetX());
	Ymax=dr.LogicalToDeviceY(absbox.GetMax().GetY());

   //careful the YMAX and YMIN are reversed because (0.0) FOR THE window TopLeft
   dr.SetClipIt((bool)(clip_status!=_IN) && (Xmin < MININT || Ymax < MININT || Xmax > MAXINT || Ymin >MAXINT));

   //empty structure, draw something to indicate the (0,0) point
   if (((Xmax - Xmin) == 0) && ((Ymin - Ymax) == 0)) //e.g. no elements
   {
/*
      ->DrawLine(Xmin-3,Ymin-3,Xmin+3,Ymin+3);
      m_dc->DrawLine(Xmin-3,Ymin+3,Xmin+3,Ymin-3);
      m_dc->DrawRectangle(Xmin,Ymin-7,7,7);
*/
      dr.SetClipStatus(backup_clip_status);
      return;
   }

   //if this is smaller then minimum bbox to draw. Just draw the boundingbox itself
   if (((Xmax - Xmin) < GLOB->STRUCTHRES()) && ((Ymin - Ymax) < GLOB->STRUCTHRES()))
   {
      dr.DrawRect((int)Xmin,(int)Ymax,(int)Xmax,(int)Ymin);
      dr.SetClipStatus(backup_clip_status);
      return;
   }

	// write the boundaries, text, paths etc
   ((ElementList*) this)->Write(dr);

   dr.SetClipStatus(backup_clip_status);
}

void Structure::SetStamp(long event)
{
	_stamp= event;
};

long Structure::GetStamp()
{
   return _stamp;
};

Structure& Structure::operator=(Structure& other)
{
    (ElementList &)*this=(ElementList &)other;
	_structurename=other._structurename;
	*_bgnstr=*other._bgnstr;

	// copy other datamembers
	_nr_of_references = other._nr_of_references;
	_nr_of_explode_references = other._nr_of_explode_references;

	return *this;
}

void Structure::Rename(GdsII* _structurelist)
{
	char			name[100];
	char			old_name[100];

	strcpy(old_name,_structurename);
	strcpy(name,_structurename);
	strcat(name,"_C");
	SetStructureName(name);

  	TDLI<Structure> I1=TDLI<Structure>(_structurelist);
   I1.toroot();
	while (I1.iterate())
	{
   	I1.item()->Rename(old_name,name);
   }

	LinkReferences(_structurelist);
}

void Structure::Rename(const wxString& oud,const wxString& nieuw)
{
  	TDLI<Element> elementi=TDLI<Element>(this);
	elementi.toroot();
	while (elementi.iterate())
	{
		if ((elementi.item()->GetType() == AREF) || (elementi.item()->GetType() == SREF))
		{
			Reference*  a_structureReference = (Reference*)elementi.item();
			if (strcmp(oud,a_structureReference->GetStructureReferenceName())==0)
         {
				a_structureReference->SetStructureReferenceName(nieuw);
         }
		}
	}
}

int Structure::GetReferenced()
{
	return _nr_of_references;
}

void Structure::SetReferenced(int nr)
{
	_nr_of_references = nr;
}

void Structure::DecrementReferences(int nr)
{
	_nr_of_references -= nr;
}

void Structure::ResetReferenced()
{
	_nr_of_references=0;
}

void Structure::IncrementReferences()
{
	_nr_of_references++;
}

int Structure::GetExplodeReferenced()
{
	return _nr_of_explode_references;
}

void Structure::SetExplodeReferenced(int nr)
{
	_nr_of_explode_references = nr;
}

void Structure::DecrementExplodeReferences(int nr)
{
	_nr_of_explode_references -= nr;
}

void Structure::ResetExplodeReferenced()
{
	_nr_of_explode_references=0;
}

void Structure::IncrementExplodeReferences()
{
	_nr_of_explode_references++;
}


// link references
void Structure::LinkReferences(GdsII* structurelist)
{
	wxString reference_structure_name;


  	TDLI<Element> elementi=TDLI<Element>(this);

	// for every reference
	elementi.toroot();
	while (elementi.iterate())
	{
		if ((elementi.item()->GetType() == AREF) || (elementi.item()->GetType() == SREF))
		{
			Reference*  a_structureReference = (Reference*) elementi.item();
			reference_structure_name = a_structureReference->GetStructureReferenceName();

			// search the referenced structure
		  	TDLI<Structure> I=TDLI<Structure>(structurelist);

			bool found=false;
			I.toroot();
			while (I.iterate())
			{
				if (!strcmp(I.item()->GetStructureName(), reference_structure_name))
				{
					found=true;
					a_structureReference->SetStructure(I.item());	// LINK the reference
               //the next statement is only for flagging that structure is referenced
               // in order to find the topstructure later on
					I.item()->IncrementReferences();
					_boundingbox.SetValid(false);
					m_sortedOnBbox = false;
					break;
				}
			}
			if (found == false) //this reference is not in the structure list
			{	char errbuf[100];
				sprintf(errbuf, "Reference not found: %s", reference_structure_name.c_str());
				throw GDS_Error(errbuf, "Reference Error");
			}
		}
	}
}

const wxString& Structure::GetStructureName()
{
	return _structurename;
}

void Structure::SetStructureName(const wxString& s)
{
	_structurename=s;
}

//copy all element of this structure into top
//references and indirect references are made absolute in top
//references in top are removed later on
//does not remove lower level references and structures
void Structure::Flatten(Structure* _Top,wxTransformMatrix* _Matrix,bool selectedonly,
                         bool srefSelected,EleFlags refgroup,
                         bool deep)
{

   //IMPORTANT (since the top level where we start already has an iterator to its
   //element list, we need the next to prevent multiple iterators
   DL_List<void*>*	_Flatten=new DL_List<void*>();

   if (_Top!=this)
   	DecrementReferences();

   if ((!deep && _Top==this) || (deep))
   {
      //first assamble all the references at this structure level into a list
      TDLI<Reference>  _FLTI=TDLI<Reference>(_Flatten);
      {
         TDLI<Element> elementi=TDLI<Element>(this);
         elementi.tohead();
         while (!elementi.hitroot())
         {
            switch (elementi.item()->GetType())
            {
               case AREF:
               case SREF:
               {
                     Reference* temp= (Reference*) elementi.item();

                     //if we are in the structure _Top itself then all references that are selected
                     //need to be flattened
                     //if we are in a referenced structure all references need to be flattened
                     if (!selectedonly ||
                         (_Top==this && selectedonly && temp->GetSelect())
                        )
                     {

                        if (_Top==this) //else this structure could be referenced more then once
                           elementi.remove();
                        else if (GetReferenced() == 0)
                        {
                           elementi.remove();
                        }
                        else
                           elementi++;

                        _FLTI.insend(temp);
                     }
                     else
                        elementi++;
                     break;
               }
               default:
                 elementi++;
            }
         }
      }

      //all refernces are collected (not removed except in case of _Top)
      //we now go down into the refrences to collect all elements (copy) them and bring
      //them to the _top of the structure to flatten
      _FLTI.tohead();
      while (!_FLTI.hitroot())
      {
         if (_Top==this) //if the top reference is selected all below is selected also
         {
         	srefSelected=_FLTI.item()->GetSelect();
            if (_FLTI.item()->GetGroupA()) refgroup=ELE_GR_A;
            if (_FLTI.item()->GetGroupB()) refgroup=ELE_GR_B;
            if (_FLTI.item()->GetGroupA() && _FLTI.item()->GetGroupB()) refgroup=ELE_GR_AB;
            //set all copies off elements in referenced structures to this group later on
         }

  			_FLTI.item()->Flatten(_Top,_Matrix,srefSelected,refgroup,deep);

         // if the reference points to a structure that is no longer referenced
         // then the structure should be deleted and removed from the structurelist
         // done in structurelist


         // this reference will be removed so the stucture it points to is one time less referenced
         // delete the reference (it is now flattened)
         if (_Top==this)   //else this structure could be referenced more then once
            delete _FLTI.item();
         else if (GetReferenced() == 0)
            delete _FLTI.item();

         _FLTI.remove();
      }
   }
   delete _Flatten;

   //all elements in underlying structures are copied to the _top level.
   //and now this level also (copy / move to _top)
	if (_Top != this) //all other elements need to be copied to _Top
	{
		// remove the elements if only referenced by the calling Reference
      if (GetReferenced() > 0)
      {
			ElementList::Flatten(_Top,_Matrix,false,srefSelected,refgroup,deep);
      }
      else
      {
			ElementList::Flatten(_Top,_Matrix,true,srefSelected,refgroup,deep);
      }
	}
	CalcBoundingBox();
	m_sortedOnBbox = false;
}


ModeTime* Structure::GetBgnstr()
{
	return _bgnstr;
}

void Structure::SetBgnstr(ModeTime* new_value)
{
	_bgnstr = new_value;
}

void Structure::Explode(Structure* _Top,bool selectedonly,GdsII* _structurelist)
{

   TDLI<Element> elementi=TDLI<Element>(this);
   elementi.tohead();
   while (!elementi.hitroot())
   {
      switch (elementi.item()->GetType())
      {
         case AREF:
         case SREF:
         {
               Reference* temp= (Reference*) elementi.item();
               Structure* refstruct= temp->GetStructure();

               //if we are in the structure _Top itself then only selected references will be treated
               //else all
               if (_Top==this)
               {
                  if ((!selectedonly || (selectedonly && temp->GetSelect())) && (refstruct->GetReferenced()>1 ))
                  {  //make a copy of the referenced structure and redirect the reference to that structure
                     char _newname[100];
                     strcpy(_newname,refstruct->GetStructureName());
                     strcat(_newname,"_E");
	                  temp->SetExplodeReferencesNr();
							Structure* _new = new Structure(_newname); // make a new structure
                     *_new=*refstruct;
                     _new->SetStructureName(_newname);
                     _structurelist->insend(_new);
                     //redirect the reference to this new structure
                     temp->SetStructure(_new);
							temp->SetStructureReferenceName(_newname);
                     //recurse into this reference
                     _new->Explode(_Top,selectedonly,_structurelist);
                  }
               }
               else
               {
	               if (refstruct->GetExplodeReferenced() != refstruct->GetReferenced())
                  {  //make a copy of the referenced structure and redirect ALL references to that structure
                     char _newname[100];
                     strcpy(_newname,refstruct->GetStructureName());
                     strcat(_newname,"_E");
							Structure* _new = new Structure(_newname); // make a new structure
                     *_new=*refstruct;
                     _new->SetStructureName(_newname);
                     _structurelist->insend(_new);
                     //redirect all references to refstruct to this new structure
                     temp->SetStructure(_new);
							temp->SetStructureReferenceName(_newname);
                     Redirect(refstruct,_new);
                     _new->Explode(_Top,selectedonly,_structurelist);
                  }
               }
         }
			default:		 break;

      }
      elementi++;
   }


	CalcBoundingBox();
	m_sortedOnBbox = false;

}

void Structure::Redirect(Structure* _old,Structure* _new)
{
  	TDLI<Element> elementi=TDLI<Element>(this);

	elementi.tohead();
	while (! elementi.hitroot())
   {
		switch(elementi.item()->GetType())
      {
			case AREF:
			case SREF:
         {
				Reference* a_ref = (Reference*) elementi.item();
				if (a_ref->GetStructure()==_old)
            {
					a_ref->SetStructure(_new);
					a_ref->SetStructureReferenceName(_new->GetStructureName());
				} else elementi++;
			} break;
			default: elementi++; break;
		}
	}
}

