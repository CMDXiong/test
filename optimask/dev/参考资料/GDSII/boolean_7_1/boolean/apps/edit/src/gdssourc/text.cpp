/*
Program	TEXT.CPP
Purpose	GdsII text definition
Programmers	R. Spekreijse & N. Noorlander
Last Update	9-4-1998, M. Reemst
*/

#ifdef __GNUG__
#pragma implementation
#endif
#include "text.h"
#include "units.h"
#include "lsetup.h"
#include "proplist.h"
#include "trans.h"
#include <math.h>
#include "drivout.h"
#include "scrndrv.h"

// constructor
Text::Text(int layernr)
{
	Init(layernr);
   _text.Clear();
}


void	Text::GetPoint(Point& offsetpoint)
{
	  offsetpoint.SetX(_relative_matrix.GetValue(2,0));
     offsetpoint.SetY(_relative_matrix.GetValue(2,1));
}



// make all new objects
void Text::Init(int layernr)
{
	m_layer = layernr;
	_texttype = 0;
	_presentationflags.total = 0;
	_pathtype = TEXT_PATH_END_SQAURE;
	m_width = 0;
}

// destructor
Text::~Text()
{
}

void Text::OWrite(Driver_out& dr)
{
   dr.Write(this);
}

void Text::DoWrite(DrawDriver& dr)
{
   dr.SetFilled(GetFilled());
	Point min_point = GetMin();
	Point max_point = GetMax();

	dr.SetSelected(GetSelect());

   dr.GDSDrawText(_text, min_point.GetX(), min_point.GetY(),
										  max_point.GetX(), max_point.GetY(),
								   	  this, &(*dr.GetMatrix() * *GetRelativeMatrix()));

}

wxPropertySheet* Text::GetPropList()
{
   double x;
   double y;
   Shape::GetPropList();
   x=_relative_matrix.GetValue(2,0);
   y=_relative_matrix.GetValue(2,1);
   if (!m_wxpropsheet->HasProperty("x"))
   {
	   wxProperty* theprop=new wxProperty();
      wxPropertyValue aval(x);
	   wxString tmp = wxString("x");
      theprop->SetName( tmp );
      theprop->SetValue(aval);
      tmp = wxString("real");
      theprop->SetRole( tmp );
	   theprop->GetValue().SetModified(false);
   	m_wxpropsheet->AddProperty(theprop);
   }
   if (!m_wxpropsheet->HasProperty("y"))
   {
	   wxProperty* theprop=new wxProperty();
      wxPropertyValue aval(y);
	   wxString tmp = wxString("y");
      theprop->SetName( tmp );
      theprop->SetValue(aval);
      tmp = wxString("real");
      theprop->SetRole( tmp );
	   theprop->GetValue().SetModified(false);
   	m_wxpropsheet->AddProperty(theprop);
   }
   if (!m_wxpropsheet->HasProperty("height"))
   {
	   wxProperty* theprop=new wxProperty();
      wxPropertyValue aval(GetHeight());
	   wxString tmp = wxString("height");
      theprop->SetName( tmp );
      theprop->SetValue(aval);
      tmp = wxString("real");
      theprop->SetRole( tmp );
	   theprop->GetValue().SetModified(false);
   	m_wxpropsheet->AddProperty(theprop);
   }
   if (!m_wxpropsheet->HasProperty("rotation"))
   {
	   wxProperty* theprop=new wxProperty();
      wxPropertyValue aval(GetRotation());
	   wxString tmp = wxString("rotation");
      theprop->SetName( tmp );
      theprop->SetValue(aval);
      tmp = wxString("real");
      theprop->SetRole( tmp );
	   theprop->GetValue().SetModified(false);
   	m_wxpropsheet->AddProperty(theprop);
   }

	return m_wxpropsheet;
}

void Text::UpdatePropList()
{
   if (!m_wxpropsheet)
      return;
   Shape::UpdatePropList();
   if (m_wxpropsheet->HasProperty("x"))
   {
	   if (m_wxpropsheet->GetProperty("x")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
	   _relative_matrix.SetValue(2,0,m_wxpropsheet->GetProperty("x")->GetValue().RealValue());
		m_wxpropsheet->RemoveProperty("x");
   }
   if (m_wxpropsheet->HasProperty("y"))
   {
	   if (m_wxpropsheet->GetProperty("y")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
	   _relative_matrix.SetValue(2,1,m_wxpropsheet->GetProperty("y")->GetValue().RealValue());
		m_wxpropsheet->RemoveProperty("y");
   }
   if (m_wxpropsheet->HasProperty("height"))
   {
      CalcBoundingBox();
	   if (m_wxpropsheet->GetProperty("height")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
      SetHeight(m_wxpropsheet->GetProperty("height")->GetValue().RealValue());
		m_wxpropsheet->RemoveProperty("height");
   }
   if (m_wxpropsheet->HasProperty("rotation"))
   {
      CalcBoundingBox();
	   if (m_wxpropsheet->GetProperty("rotation")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
      SetRotation(m_wxpropsheet->GetProperty("rotation")->GetValue().RealValue());
		m_wxpropsheet->RemoveProperty("rotation");
   }

	return;
}

wxString Text::MyText()
{
	return _text;
}

double Text::GetHeight()
{
   double temp1 = GetRelativeMatrix()->GetValue(0,0); 	// voor het berekenen van de hoek
   double temp2 = GetRelativeMatrix()->GetValue(0,1); 	//

   // Rotation
   double rot_angle = atan2(temp2,temp1)*180/M_PI;

   rot_angle = Check_int(rot_angle);
   if (rot_angle != 90 && rot_angle != -90)
      return GetRelativeMatrix()->GetValue(0,0)/cos((rot_angle/180)*M_PI);
   else
      return GetRelativeMatrix()->GetValue(1,0)/sin((rot_angle/180)*M_PI);
}

void Text::SetHeight(double newheight)
{
	double x=_relative_matrix.GetValue(2,0);
   double y=_relative_matrix.GetValue(2,1);
   _relative_matrix.Scale(1/GetHeight(),1/GetHeight(), x, y); //scale back to 1
   _relative_matrix.Scale(newheight,newheight, x, y);
}

double Text::GetRotation()
{
   return GetRelativeMatrix()->GetRotation();
}

void Text::SetRotation(double rotation)
{
   _relative_matrix.SetRotation(rotation);
}



// sets bounding box
BoundingBox& Text::CalcBoundingBox()
{
//	double scale=1 ;

	m_boundingbox.SetValid(true);

	double dx=0, dy=0;

   switch (GetHorizontal())  //	0-2
   {
      case 0:	// Left
         dx=0;
         break;
      case 1:	//	Centre
         dx=-0.4*strlen(_text);
         break;
      case 2:	// Right
         dx=-0.8*strlen(_text);
         break;
   }
   switch (GetVertical())		//	0-2
   {
      case 0:	//	Top
         dy=-1;
         break;
      case 1:	// Middle
         dy=-0.5;
         break;
      case 2:	// Bottom
         dy=0;
         break;
   }

	m_boundingbox.SetMin( dx, dy);
	m_boundingbox.SetMax(1.1*(0.8*strlen(_text)+dx), (1.0+dy));

   // Ok, we want to keep these values to draw the box around the text, not the boundingbox...
   _min = m_boundingbox.GetMin();
   _max = m_boundingbox.GetMax();

	m_boundingbox.MapBbox(&_relative_matrix);

	return m_boundingbox;
}


void Text::SetRelativeMatrix(wxTransformMatrix* m)
{
	_relative_matrix = *m;
}

// makes a text same as this
// usage : text1 = text2;
Text& Text::operator=(Text& a_text)
{
   (Shape &)*this=(Shape &)a_text;

   _text=a_text._text;

   _relative_matrix = a_text._relative_matrix;

   _presentationflags.total=a_text._presentationflags.total;
	_texttype =a_text._texttype;

	_pathtype =a_text._pathtype;

	return *this;
}

bool Text::Select(Point* P)
{
	if (CalcBoundingBox().PointInBox(*P))
		return true;
	return false;
}

bool Text::Select(BoundingBox* area,wxTransformMatrix* m)
{
	// check if the boundingBox of the polygon is in the selected boundingBox
	BoundingBox	backup =CalcBoundingBox();
	backup.MapBbox(m);

   Point min,max,amin,amax;
	min = backup.GetMin();
	max = backup.GetMax();
	amin = area->GetMin();
	amax = area->GetMax();

	if ((min.GetX() < amin.GetX()) || (min.GetY() < amin.GetY()) ||
		(max.GetX() > amax.GetX()) || (max.GetY() > amax.GetY())) return false;

	return true;
}

TEXT_TYPE Text::GetTexttype()
{
	return _texttype;
}


void Text::SetTexttype(TEXT_TYPE t)
{
	_texttype = t;
}

TEXT_PATHTYPE Text::GetPathtype()
{
	return _pathtype;
}

void Text::SetPathtype(TEXT_PATHTYPE p)
{
	_pathtype = p;
}

void Text::SetText(const wxString& t)
{
   _text=t;
}

void Text::Transform(wxTransformMatrix* _Matrix)
{
   _relative_matrix = *_Matrix * _relative_matrix;
}

Element* Text::Duplicate()
{
		Text* a_text= new Text;
		*a_text=*this;

		return a_text;
}

void  Text::Convert(G_Polygon*, double)
{
}

wxTransformMatrix* Text::GetRelativeMatrix()
{
	return &_relative_matrix;
}


Point Text::GetMin()
{
	return _min;
}

Point Text::GetMax()
{
	return _max;
}

void Text::SetFont(char nw_font)
{
	_presentationflags.bits._font = nw_font;
}

void Text::SetVertical(char nw_vertical)
{
	_presentationflags.bits._vertical = nw_vertical;
}


void Text::SetHorizontal(char nw_horizontal)
{
	_presentationflags.bits._horizontal = nw_horizontal;
}


char Text::GetFont()
{
	return (char) _presentationflags.bits._font;
}


char Text::GetVertical()
{
	return (char) _presentationflags.bits._vertical;
}


char Text::GetHorizontal()
{
	return (char) _presentationflags.bits._horizontal;
}

void		Text::SetPresentationFlags(	short int the_total)
{
	_presentationflags.total=the_total;
}

short int	Text::GetPresentationFlags( )
{
	return _presentationflags.total;
}

