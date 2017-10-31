/*
Program	ELEMENT.CPP
Purpose	Contains a boundary
Last Update	11-12-1995
*/


#ifdef __GNUG__
#pragma implementation
#endif
#include "element.h"
#include "units.h"
#include "proplist.h"
#include "drivout.h"
#include "scrndrv.h"

Element::~Element()
{
   if (m_wxpropsheet)
	   delete m_wxpropsheet;
}

Element::Element()
{
   m_stamp= ++GLOB->EVENT_COUNTER;
   bit_flags._all=0;
	bit_flags.bits.m_filled=true;
   m_wxpropsheet=0;
}

void Element::Write(Driver_out& dr)
{
   this->OWrite(dr);
}

void Element::Write(DrawDriver& dr)
{
   dr.SetFilled(GetFilled());
   bool ident = dr.GetMatrix()->IsIdentity();
   BoundingBox	absbox=GetBoundingBox();

	OVERLAP backup_clip_status = dr.GetClipStatus();
	OVERLAP clip_status = dr.GetClipStatus();

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
   {
      dr.SetClipStatus(backup_clip_status);
		return;
	}

	double Xmin,Ymin,Xmax,Ymax;

	Xmin=dr.LogicalToDeviceX(absbox.GetMin().GetX());
	Ymin=dr.LogicalToDeviceY(absbox.GetMin().GetY());
	Xmax=dr.LogicalToDeviceX(absbox.GetMax().GetX());
	Ymax=dr.LogicalToDeviceY(absbox.GetMax().GetY());

   //careful the YMAX and YMIN are reversed because (0.0) FOR THE window TopLeft
   dr.SetClipIt((bool)(clip_status!=_IN) && (Xmin < MININT || Ymax < MININT || Xmax > MAXINT || Ymin >MAXINT));

   DoWrite(dr);

   dr.SetClipStatus(backup_clip_status);
	return;
}

Element& Element::operator=(Element &other)
{
	m_stamp=other.m_stamp;
	bit_flags._all=other.bit_flags._all;

   if (other.m_wxpropsheet)
   {
      if (m_wxpropsheet)
         delete m_wxpropsheet;
	   m_wxpropsheet = new wxPropertySheet();

      wxNode *node = other.m_wxpropsheet->GetProperties().First();
      while (node)
      {
         wxProperty *prop = (wxProperty *)node->Data();
         wxProperty *propcopy = new wxProperty(*prop);
         m_wxpropsheet->AddProperty( propcopy);
         node = node->Next();
      }
   }
   return *this;
}

void Element::SetStamp(long event)
{
	m_stamp= event;
};

long Element::GetStamp()
{
   return m_stamp;
};

wxPropertySheet* Element::HasPropList()
{
   return m_wxpropsheet;
}

wxPropertySheet* Element::GetPropList()
{
   if (!m_wxpropsheet)
	   m_wxpropsheet=new wxPropertySheet();
   return m_wxpropsheet;
}

void Element::UpdatePropList()
{
};

void Element::AddProperty(const wxString& name,const wxString& type,const wxString& value)
{
   if (!m_wxpropsheet)
	   m_wxpropsheet=new wxPropertySheet();

   if (m_wxpropsheet->HasProperty(name))
		m_wxpropsheet->RemoveProperty(name);

   wxString cname = name;
   wxString ctype = type;
   wxProperty* theprop=new wxProperty();
   if (!strcmp(type,"bool"))
   {
      if (!stricmp(value,"true"))
      {
         wxPropertyValue aval((bool) true);
         theprop->SetName( cname );
         theprop->SetValue(aval);
         theprop->SetRole(ctype);
      }
      else
      {
         wxPropertyValue aval((bool) false);
         theprop->SetName(  cname );
         theprop->SetValue(aval);
         theprop->SetRole( ctype );
      }
   }
   else if (!strcmp(type,"real"))
   {
      double convert=atof(value);
      wxPropertyValue aval(convert);
      theprop->SetName( cname );
      theprop->SetValue(aval);
      theprop->SetRole( ctype );
   }
   else if (!strcmp(type,"integer"))
   {
      long convert=atof(value);
      wxPropertyValue aval(convert);
      theprop->SetName( cname );
      theprop->SetValue(aval);
      theprop->SetRole( ctype );
   }
   else// if (!strcmp(a_prop.GetType(),"string"))
   {
      wxPropertyValue aval(value);
      theprop->SetName( cname );
      theprop->SetValue(aval);
      theprop->SetRole( ctype );
   }
   m_wxpropsheet->AddProperty(theprop);
   UpdatePropList();

}

bool Element::GetGroupA()
{
	return bit_flags.bits.m_group_a;
}

bool Element::GetGroupB()
{
	return bit_flags.bits.m_group_b;
}

bool Element::GetGroupC()
{
	return bit_flags.bits.m_group_c;
}

bool Element::GetFilled()
{
	return bit_flags.bits.m_filled;
}

void	Element::SetSelect(bool	value)
{
	bit_flags.bits.m_select=value;
}

void	Element::SetGroupA(bool	value)
{
	bit_flags.bits.m_group_a=value;
}

void	Element::SetGroupB(bool	value)
{
	bit_flags.bits.m_group_b=value;
}

void	Element::SetGroupC(bool	value)
{
	bit_flags.bits.m_group_c=value;
}

void	Element::SetFilled(bool	value)
{
	bit_flags.bits.m_filled=value;
}

bool Element::GetSelect()
{
	return bit_flags.bits.m_select;
}

void Element::SetFlags(short int which)
{
	bit_flags._all = bit_flags._all | which;
}

void Element::ClearFlags(short int which)
{
	bit_flags._all = bit_flags._all & (which ^ (short int)0xffff);
}

short int Element::GetFlags()
{
	return bit_flags._all;
}

void Element::SetTemplate(bool b)
{
	bit_flags.bits.m_template = b;
}


void Element::SetExternal(bool b)
{
	bit_flags.bits.m_external = b;
}


void Element::SetUsed(bool b)
{
	bit_flags.bits.m_used = b;
}


bool Element::GetTemplate()
{
	return bit_flags.bits.m_template;
}


bool Element::GetExternal()
{
	return bit_flags.bits.m_external;
}


bool Element::GetUsed()
{
	return bit_flags.bits.m_used;
}


