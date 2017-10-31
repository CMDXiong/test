/*
Program	SHAPE.CPP
Purpose	Contains a shape
Last Update	29-9-96
*/

#ifdef __GNUG__
#pragma implementation
#endif
#include "shape.h"
#include "units.h"
#include "lsetup.h"
#include "proplist.h"
#include "drivout.h"
#include "scrndrv.h"

Shape::Shape()
{
  m_layer=0;
};

Shape& Shape::operator=(Shape &other)
{
	(Element&) *this=(Element &)other;
	m_layer=other.m_layer;
	m_boundingbox = other.m_boundingbox;
	m_width=other.m_width;
   return *this;
}

void Shape::Write(DrawDriver& dr)
{
   dr.SetFilled(GetFilled());
   bool ident = dr.GetMatrix()->IsIdentity();
   BoundingBox	absbox=GetBoundingBox();

   // check if this layer must be written
   if (! dr.NeedToDraw(m_layer)) return;
   if (dr.GetDrawStyle() != INVERT_WIREFRAME)
   {
      if ((!GLOB->SETUP->Get_Visible(m_layer)))
         return;
   }

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

   // set a layer and layercolor
   dr.SetLayer(m_layer);

   //if this is smaller then minimum bbox to draw. Just draw the boundingbox itself
   if (((Xmax - Xmin) < GLOB->PRIMTHRES()) && ((Ymin - Ymax) < GLOB->PRIMTHRES()))
   {
      if (GLOB->DRAWSMALLPRIM())
      {
         dr.DrawRect((int)Xmin,(int)Ymax,(int)Xmax,(int)Ymin);
      }
      dr.UnSetLayer(m_layer);
      return;
   }

   DoWrite(dr);

   dr.SetClipStatus(backup_clip_status);
   dr.UnSetLayer(m_layer);
	return;
}

BoundingBox& Shape::GetBoundingBox()
{
	assert (m_boundingbox.GetValid());
	return m_boundingbox;
}

int Shape::GetLayer()
{
	return m_layer;
}

void Shape::SetLayer(int new_value)
{
	m_layer = new_value;
}


double Shape::GetWidth()
{
	return m_width;
}

void Shape::SetWidth(double new_value)
{
	m_width = new_value;
}

wxPropertySheet* Shape::GetPropList()
{
	Element::GetPropList();
   if (!m_wxpropsheet->HasProperty("layer"))
   {
      long lay=m_layer;
	   wxProperty* theprop=new wxProperty();
      wxPropertyValue aval(lay);
	   wxString tmp = wxString("layer");
      theprop->SetName( tmp );
      theprop->SetValue(aval);
      tmp = wxString("integer");
      theprop->SetRole( tmp );
	   theprop->GetValue().SetModified(false);
   	m_wxpropsheet->AddProperty(theprop);
   }
   if (!m_wxpropsheet->HasProperty("width"))
   {
	   wxProperty* theprop=new wxProperty();
      wxPropertyValue aval(&m_width);
	   wxString tmp = wxString("width");
      theprop->SetName( tmp );
      theprop->SetValue(aval);
      tmp = wxString("real");
      theprop->SetRole( tmp );
	   theprop->GetValue().SetModified(false);
   	m_wxpropsheet->AddProperty(theprop);
   }
   if (!m_wxpropsheet->HasProperty("filled"))
   {
	   wxProperty* theprop=new wxProperty();
      wxPropertyValue aval(GetFilled());
	   wxString tmp = wxString("filled");
      theprop->SetName( tmp );
      theprop->SetValue(aval);
      tmp = wxString("bool");
      theprop->SetRole( tmp );
	   theprop->GetValue().SetModified(false);
   	m_wxpropsheet->AddProperty(theprop);
   }
	return m_wxpropsheet;
}

void Shape::UpdatePropList()
{
   if (!m_wxpropsheet)
		return;
   Element::UpdatePropList();
   //remove the predefined properties
   if (m_wxpropsheet->HasProperty("width"))
   {
      CalcBoundingBox();
	   if (m_wxpropsheet->GetProperty("width")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
		m_wxpropsheet->RemoveProperty("width");
   }
   if (m_wxpropsheet->HasProperty("layer"))
   {
	   if (m_wxpropsheet->GetProperty("layer")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
      m_layer=m_wxpropsheet->GetProperty("layer")->GetValue().RealValue();
		m_wxpropsheet->RemoveProperty("layer");
   }
   if (m_wxpropsheet->HasProperty("filled"))
   {
	   if (m_wxpropsheet->GetProperty("filled")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
      SetFilled(m_wxpropsheet->GetProperty("filled")->GetValue().BoolValue());
		m_wxpropsheet->RemoveProperty("filled");
   }
};


