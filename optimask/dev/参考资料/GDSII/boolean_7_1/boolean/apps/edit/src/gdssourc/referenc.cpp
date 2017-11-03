/*
Program	REFERENC.CPP
Purpose	defines GDSII refence structure type
Last Update	9-4-1998, M. Reemst
*/


#ifdef __GNUG__
#pragma implementation
#endif
#include <stdarg.h>
#include "referenc.h"
#include "structur.h"
#include "lsetup.h"
#include "units.h"
#include "proplist.h"
#include "drivout.h"
#include "scrndrv.h"


Reference::Reference(Structure *a_structure)
{
	if (!a_structure)
		throw GDS_Error("Unknown Object.\n\nCannot copy Structure from NULL.",
						"Fatal Creation Error.", 0, 1);
	_structure = a_structure;
	_structure->IncrementReferences();
	_relative_matrix = new wxTransformMatrix();

	_structurereferencename.Clear();

	if (!_relative_matrix )
		throw GDS_Error("Out of Memory.\n\nCould not create Reference Object.",
						"Fatal Creation Error", 0, 1);
}

Reference::Reference()
{
	_structure = 0;
	_structurereferencename.Clear();
	_relative_matrix = new wxTransformMatrix();

	if (!_relative_matrix )
		throw GDS_Error("Out of Memory.\n\nCould not create Reference Object.",
						"Fatal Creation Error", 0, 1);
}


Reference::~Reference()
{
	delete _relative_matrix;
}

wxPropertySheet* Reference::GetPropList()
{
   double x;
   double y;
   Element::GetPropList();
   x=_relative_matrix->GetValue(2,0);
   y=_relative_matrix->GetValue(2,1);
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
	return m_wxpropsheet;
}

void Reference::UpdatePropList()
{
   if (!m_wxpropsheet)
      return;
   Element::UpdatePropList();
   if (m_wxpropsheet->HasProperty("x"))
   {
	   if (m_wxpropsheet->GetProperty("x")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
	   (*_relative_matrix).SetValue(2,0,m_wxpropsheet->GetProperty("x")->GetValue().RealValue());
		m_wxpropsheet->RemoveProperty("x");
   }
   if (m_wxpropsheet->HasProperty("y"))
   {
	   if (m_wxpropsheet->GetProperty("y")->GetValue().GetModified())
			GLOB->SETUP->Set_UpdateAvailableLayers(true);
	   (*_relative_matrix).SetValue(2,1,m_wxpropsheet->GetProperty("y")->GetValue().RealValue());
		m_wxpropsheet->RemoveProperty("y");
   }
	return;
}


BoundingBox& Reference::GetBoundingBox()
{
	assert (m_boundingbox.GetValid());
	return m_boundingbox;
}

const wxString& Reference::GetStructureReferenceName()
{
	return _structurereferencename;
}


Structure *Reference::GetStructure()
{
	return _structure;
}


void Reference::SetStructure(Structure *a_structure)
{
	_structure = a_structure;
}

void	Reference::GetPoint(Point& offsetpoint)
{
	  offsetpoint.SetX(_relative_matrix->GetValue(2,0));
     offsetpoint.SetY(_relative_matrix->GetValue(2,1));
}


void	Reference::SetPoint(Point& offsetpoint)
{
	  (*_relative_matrix).SetValue(2,0,offsetpoint.GetX());
	  (*_relative_matrix).SetValue(2,1,offsetpoint.GetY());
}


void Reference::SetStructureReferenceName(const wxString& s)
{
	_structurereferencename=s;
}


wxTransformMatrix* Reference::GetRelativeMatrix()
{
	return _relative_matrix;
}

void Reference::SetRelativeMatrix(wxTransformMatrix* m)
{
	*_relative_matrix = *m;
}

void Reference::SetReferencesNr()
{
  // increment the structure its references NR
  _structure->IncrementReferences();
  _structure->SetReferenceNr();
};

void Reference::SetExplodeReferencesNr()
{
  // increment the structure its explode references NR
  _structure->IncrementExplodeReferences();
  _structure->SetExplodeReferenceNr();
};

ArrayReference::ArrayReference():Reference()
{
   m_rows=0;
   m_columns=0;
	_horpoint  = new Point(0, 0);
	_verpoint = new Point(0, 0);
}

ArrayReference::~ArrayReference()
{
	delete _horpoint;
	delete _verpoint;
}

void ArrayReference::OWrite(Driver_out& dr)
{
   dr.Write(this);
}

void ArrayReference::DoWrite(DrawDriver& dr)
{
	// NOTE: matrix in structurereference wil be copied in
	//       the driver it's matrix!
 	//       When the function is completed it will be copied
	//			back in structurereference. (swapped)

	bool		SRefSelBackup;

   bool ident = dr.GetMatrix()->IsIdentity();

	// first make a copy in the driver it's matrix
	wxTransformMatrix _backup =*dr.GetMatrix();
	wxTransformMatrix _relmat = *GetRelativeMatrix();

	SRefSelBackup = dr.GetParentSelect();
	if(ident && GetSelect())
   	dr.SetParentSelect(true);
	dr.SetSelected(GetSelect());

	wxTransformMatrix tmpmat =*dr.GetMatrix();
	tmpmat *= *GetRelativeMatrix();
  	dr.SetMatrix(tmpmat);

   // get row and column distance
   int columns=GetColumns();
   int rows=GetRows();

   Point offset;
   GetPoint(offset);
   Point hor=*GetHorzPoint();
   Point ver=*GetVertPoint();

   double dx1=(hor.GetX()-offset.GetX())/columns;
   double dy1=(hor.GetY()-offset.GetY())/columns;
   double dx2=(ver.GetX()-offset.GetX())/rows;
   double dy2=(ver.GetY()-offset.GetY())/rows;

   for (int i = 0  ; i < rows ; i++)
   {
	   for (int j = 0  ; j < columns ; j++)
      {
         // Translate2D over XY from the structurereference
			GetStructure()->Write(dr);
         _relmat.Translate(dx1,dy1);
			tmpmat = _backup;
			tmpmat *= _relmat;
      	dr.SetMatrix(tmpmat);
      }
      //translate back in dx1*columns and in dy1
      _relmat.Translate(-dx1*columns + dx2,-dy1*columns +dy2);
      tmpmat = _backup;
      tmpmat *= _relmat;
      dr.SetMatrix(tmpmat);
   }

	// now copy matrix back
	dr.SetMatrix(_backup);
 	dr.SetParentSelect(SRefSelBackup);
}


void ArrayReference::CalcArea(double* area,EleFlags mask)
{
   if (_structure)
   {
      for (int i = 0  ; i < m_rows ; i++)
         for (int j = 0  ; j < m_columns ; j++)
			   _structure->CalcArea(area,mask);
   }
}

void ArrayReference::Transform(wxTransformMatrix* _Matrix)
{
   *_relative_matrix = *_Matrix * *_relative_matrix;
   _horpoint->Transform(_Matrix);
   _verpoint->Transform(_Matrix);
}

BoundingBox& ArrayReference::CalcBoundingBox()
{
	m_boundingbox.Reset();
	m_boundingbox.SetValid(true);

	m_boundingbox = _structure->CalcBoundingBox();

   // get row and column distance
   Point atpoint;
   GetPoint(atpoint);

   double dx1=(_horpoint->GetX()-atpoint.GetX())/m_columns;
   double dy1=(_horpoint->GetY()-atpoint.GetY())/m_columns;
   double dx2=(_verpoint->GetX()-atpoint.GetX())/m_rows;
   double dy2=(_verpoint->GetY()-atpoint.GetY())/m_rows;

   Point offset;

   offset.SetX(dx1*(m_columns-1)+dx2*(m_rows-1));
   offset.SetY(dy1*(m_columns-1)+dy2*(m_rows-1));

	m_boundingbox.MapBbox(_relative_matrix);

   BoundingBox temp;
   BoundingBox onestruct;
   temp=m_boundingbox;
   onestruct=m_boundingbox;

   //translate bbox with offset x and y
	temp.Translate(offset);
	m_boundingbox.Expand(temp);

   offset.SetX(dx1*(m_columns-1));
   offset.SetY(dy1*(m_columns-1));
   temp=onestruct;
	temp.Translate(offset);
	m_boundingbox.Expand(temp);

   offset.SetX(dx2*(m_rows-1));
   offset.SetY(dy2*(m_rows-1));
   temp=onestruct;
	temp.Translate(offset);
	m_boundingbox.Expand(temp);

	return m_boundingbox;
}

void ArrayReference::SetHorzSpace(Point *hor_space)
{
	delete _horpoint;
	_horpoint = hor_space;
}

void ArrayReference::SetVertSpace(Point *vert_space)
{
	delete _verpoint;
	_verpoint = vert_space;
}

Point* ArrayReference::GetHorzPoint()
{
	return _horpoint;
}

Point* ArrayReference::GetVertPoint()
{
	return _verpoint;
}

int ArrayReference::GetRows()
{
	return m_rows;
}

int ArrayReference::GetColumns()
{
	return m_columns;
}

void ArrayReference::SetRows(int i)
{
	m_boundingbox.Reset();
	m_boundingbox.SetValid(false);
	m_rows = i;
}

void ArrayReference::SetColumns(int i)
{
	m_boundingbox.Reset();
	m_boundingbox.SetValid(false);
	m_columns = i;
}

ArrayReference& ArrayReference::operator=(ArrayReference& a_arrayreference)
{
   (Element &)*this=(Element &)a_arrayreference;

	_structure = a_arrayreference.GetStructure();
	m_boundingbox = a_arrayreference.GetBoundingBox();
	m_boundingbox.SetValid(false);
	_structurereferencename=a_arrayreference._structurereferencename;
   *_horpoint=*a_arrayreference._horpoint;
   *_verpoint=*a_arrayreference._verpoint;

	*_relative_matrix=*a_arrayreference._relative_matrix;
   m_columns=a_arrayreference.m_columns;
   m_rows=a_arrayreference.m_rows;

	return *this;
}

Element* ArrayReference::Duplicate()
{
	ArrayReference*	newone = new ArrayReference;
	*newone = *this;
	return newone;
}

bool ArrayReference::Select(Point* P)
{
	if (! GetBoundingBox().PointInBox(* P)) return false;

	wxTransformMatrix* relativematrix= new wxTransformMatrix(*_relative_matrix);
	wxTransformMatrix* _inverse= new wxTransformMatrix(*relativematrix);

	_inverse->Invert();

	Point*	C = new Point(*P);
	bool	result = false;

   //test for each structure in the array if the point is in there
   // get row and column distance
   Point offset;
   GetPoint(offset);
   Point hor=*GetHorzPoint();
   Point ver=*GetVertPoint();

   double dx1=(hor.GetX()-offset.GetX())/m_columns;
   double dy1=(hor.GetY()-offset.GetY())/m_columns;
   double dx2=(ver.GetX()-offset.GetX())/m_rows;
   double dy2=(ver.GetY()-offset.GetY())/m_rows;

	if (_structure)
   {
      for (int i = 1  ; i <= m_rows ; i++)
      {
         for (int j = 1  ; j <= m_columns ; j++)
         {
            C->Set(P->GetX(),P->GetY());
            C->Transform(_inverse);

	         if ((result = NULL != _structure->Select(C,QUERY)))
                return result;

            // Translate2D over XY from the structurereference
            relativematrix->Translate(dx1,dy1);
            *_inverse=*relativematrix;
            _inverse->Invert();
         }
         //translate back in dx*columns and in dy
         relativematrix->Translate(-dx1*m_columns + dx2,-dy1*m_columns +dy2);
         *_inverse=*relativematrix;
         _inverse->Invert();
      }
   }

	delete C;
	delete _inverse;
   delete relativematrix;
	return result;
}

void ArrayReference::Flatten(Structure* _Top,wxTransformMatrix* _Matrix,bool srefSelected,EleFlags refgroup,bool deep)
{
	// first make a copy in the driver it's matrix
	wxTransformMatrix* _backup = new wxTransformMatrix(*_Matrix);
	wxTransformMatrix* _relmat = new wxTransformMatrix(*_relative_matrix);
	assert(_backup != 0);

	*_Matrix *= *_relative_matrix;

   // get row and column distance
   Point offset;
   GetPoint(offset);
   Point hor=*GetHorzPoint();
   Point ver=*GetVertPoint();

   double dx1=(hor.GetX()-offset.GetX())/m_columns;
   double dy1=(hor.GetY()-offset.GetY())/m_columns;
   double dx2=(ver.GetX()-offset.GetX())/m_rows;
   double dy2=(ver.GetY()-offset.GetY())/m_rows;

   //sheet on number of refs to make flatten work correctly
   _structure->SetReferenced( m_rows*m_columns - 1 + _structure->GetReferenced() );

	if (_structure)
   {
      for (int i = 0  ; i < m_rows ; i++)
      {
         for (int j = 0  ; j < m_columns ; j++)
         {
            // Translate2D over XY from the structurereference
			   _structure->Flatten(_Top,_Matrix,false,srefSelected,refgroup,deep);
            _relmat->Translate(dx1,dy1);
            *_Matrix = *_backup;
            *_Matrix *= *_relmat;
         }
         //translate back in dx1*columns and in dy1
         _relmat->Translate(-dx1*m_columns + dx2,-dy1*m_columns +dy2);
			*_Matrix = *_backup;
			*_Matrix *= *_relmat;
      }
   }

	// now copy matrix back
	*_Matrix = *_backup;
	delete _relmat;
	delete _backup;
}

bool ArrayReference::Select(BoundingBox* B,wxTransformMatrix* m)
{
	Point			min,max,amin,amax;

	BoundingBox	backup = CalcBoundingBox();
	backup.MapBbox(m);

	min = backup.GetMin();
	max = backup.GetMax();
	amin = B->GetMin();
	amax = B->GetMax();

   if ((min.GetX() < amin.GetX()) || (min.GetY() < amin.GetY()) ||
       (max.GetX() > amax.GetX()) || (max.GetY() > amax.GetY())) return false;

	return true;
}

StructureReference::StructureReference() : Reference()
{
}

StructureReference::~StructureReference()
{
}

void StructureReference::OWrite(Driver_out& dr)
{
   dr.Write(this);
}

void StructureReference::DoWrite(DrawDriver& dr)
{
	// NOTE: matrix in structurereference wil be copied in
	//       the driver it's matrix!
 	//       When the function is completed it will be copied
	//			back in structurereference. (swapped)

	bool		SRefSelBackup;

   bool ident = dr.GetMatrix()->IsIdentity();

	// first make a copy in the driver it's matrix
	wxTransformMatrix _backup =*dr.GetMatrix();

	wxTransformMatrix mnew = *dr.GetMatrix();
   mnew *= *GetRelativeMatrix();
	dr.SetMatrix(mnew);

	SRefSelBackup = dr.GetParentSelect();
	if(ident && GetSelect())
   	dr.SetParentSelect(true);
	dr.SetSelected(GetSelect());

	GetStructure()->Write(dr);
 	dr.SetParentSelect(SRefSelBackup);

	// now copy matrix back
	dr.SetMatrix(_backup);
}

void StructureReference::CalcArea(double* area,EleFlags mask)
{
   _structure->CalcArea(area,mask);
}

void StructureReference::Flatten(Structure* _Top,wxTransformMatrix* _Matrix,bool srefSelected,EleFlags refgroup,bool deep )
{
   // first make a copy in the driver it's matrix
   wxTransformMatrix*_backup = new wxTransformMatrix(*_Matrix);
   assert(_backup != 0);

   *_Matrix *= (*_relative_matrix);

	_structure->Flatten(_Top,_Matrix,false,srefSelected,refgroup,deep);

   // now copy matrix back
   *_Matrix = *_backup;
   delete _backup;
}


void StructureReference::Transform(wxTransformMatrix* _Matrix)
{
   *_relative_matrix = *_Matrix * *_relative_matrix;
}

BoundingBox& StructureReference::CalcBoundingBox()
{
	m_boundingbox.Reset();
	m_boundingbox.SetValid(true);

	m_boundingbox = _structure->CalcBoundingBox();
	m_boundingbox.MapBbox(_relative_matrix);

	return m_boundingbox;
}


// makes a structref. same as these
// usage : strucref1 = strucref2;
StructureReference& StructureReference::operator=(StructureReference &other)
{
   (Element &)*this=(Element &)other;

	_structure = other.GetStructure();
	m_boundingbox = other.GetBoundingBox();
	_structurereferencename=other._structurereferencename;

	*_relative_matrix=*other._relative_matrix;
	return *this;
}

Element* StructureReference::Duplicate()
{
	StructureReference*	newone = new StructureReference;
	*newone = *this;
	return newone;
}


bool StructureReference::Select(Point* P)
{

	wxTransformMatrix*_inverse = new wxTransformMatrix(*_relative_matrix);

	_inverse->Invert();

	if (! m_boundingbox.PointInBox(*P)) return false;

	Point*	C = new Point(*P);
	bool	result = false;

	C->Transform(_inverse);

	if (_structure)
   {
      //is this point with a polygon of the referenced structure
      result= NULL != _structure->Select(C,QUERY);
	}
	delete C;
	delete _inverse;
	return result;
}

bool StructureReference::Select(BoundingBox* B,wxTransformMatrix* m)
{
	Point			min,max,amin,amax;

	BoundingBox	backup = CalcBoundingBox();
	backup.MapBbox(m);

	min = backup.GetMin();
	max = backup.GetMax();
	amin = B->GetMin();
	amax = B->GetMax();

	if ((min.GetX() < amin.GetX()) || (min.GetY() < amin.GetY()) ||
		(max.GetX() > amax.GetX()) || (max.GetY() > amax.GetY())) return false;

	return true;
}


