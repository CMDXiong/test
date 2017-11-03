//! author="klaas"
//! version="1.0"
/*
Program	ARC.H
*/


#ifndef ARCSEG_H
#define ARCSEG_H

#ifdef __GNUG__
#pragma interface
#endif

//! headerfiles="arc.h matrix.h misc.h shape.h gdserr.h polyline.h"
//! codefiles="arc.cpp"
#include "misc.h"
#include "wxmatrix.h"
#include "polyline.h"
#include "gdserr.h"
#include "shape.h"
#include "path.h"
#include <wx/image.h>

class DrawDriver;
class Driver_out;




//:defenition
//!c_des: Element of type Arcseg (arcsegment with a width parameter)
//!c_des: See also class Shape
//!rel_type: class: ElementList
//!rel_type: class: G_Polyline
class Arcseg : public Shape
{
	public:
		// Constructors and destructors
		Arcseg(int layernr = 0);
		~Arcseg();

      void OWrite(Driver_out& where);
      void DoWrite(DrawDriver& dr);

		BoundingBox&			CalcBoundingBox();
		int			 			GetDatatype();
		G_Polyline*       	GetPolyline();
		void			 			SetDatatype(int);
		void			 	      SetPolyline(G_Polyline*);
		TYPE					  	GetType() {return ARCSEG;};
		void 						Transform(wxTransformMatrix* _Matrix);
		Element*					Duplicate();

		Arcseg& 					operator=(Arcseg &other);
		void              	Convert(G_Polygon* _converted, double);

		bool			      Select(Point*);
		bool			      Select(BoundingBox*,wxTransformMatrix*);
      wxPropertySheet*		GetPropList();
		void 						UpdatePropList();

	protected:

		G_Polyline*				_polyline;
		int						_datatype;
};

//:defenition
//!c_des: Element of type Box (rectangle with a width parameter)
//!c_des: See also class Shape
//!rel_type: class: ElementList
class Box : public Shape
{
	public:
		// Constructors and destructors
		Box(int layernr = 0);
		~Box();

      void OWrite(Driver_out& where);
      void DoWrite(DrawDriver& dr);

		BoundingBox&			CalcBoundingBox();
		int			 			GetBoxtype();
		void			 			SetBoxtype(int);
		void 						Set(G_Polygon* a_rec_poly);
		void 						Set(double x_centre,double y_centre,double ang, double w, double h);
      double               GetW();
      double               GetH();
		void			 			SetW(double w);
		void			 			SetH(double h);
		TYPE					  	GetType() {return BOX;};
		void 						Transform(wxTransformMatrix* _Matrix);
		void 						Convert(G_Polygon* _polygon, double marge);

		Element*					Duplicate();
		Box& 						operator=(Box &other);

		bool			         Select(Point*);
		bool			         Select(BoundingBox*,wxTransformMatrix*);
		void                 SetRelativeMatrix(wxTransformMatrix* m);
		wxTransformMatrix*   GetRelativeMatrix();
      wxPropertySheet*		GetPropList();
		void 						UpdatePropList();

	protected:

      double 					_w;
      double 					_h;
      wxTransformMatrix 	_relative_matrix;
		int						_boxtype;
};

//:defenition
//!c_des: Element of type Circle (circle with a width parameter)
//!c_des: See also class Shape
//!rel_type: class: ElementList
class Circle: public Shape
{
	public:
		// Constructors and destructors
		Circle(int layernr = 0);
		~Circle();

      void OWrite(Driver_out& where);
      void DoWrite(DrawDriver& dr);

		BoundingBox&			CalcBoundingBox();
		int			 			GetDatatype();
		void			 			SetDatatype(int);
		TYPE					  	GetType() {return CIRCLE;};
		void 						Transform(wxTransformMatrix* _Matrix);
		Point& 					GetMid();
		double	 				GetRadius();
		void 						SetRadius(double radius);
		void 						SetMid(const Point& a_point);
		void 						SetMid(double X,double Y);
		void 						Convert(G_Polygon* _polygon, double marge);

		Element*					Duplicate();
		Circle& 					operator=(Circle &other);

		bool			      Select(Point*);
		bool			      Select(BoundingBox*,wxTransformMatrix*);
      wxPropertySheet*		GetPropList();
		void 						UpdatePropList();

	protected:

		Point                _mid;
		double	            _radius;
		int						_datatype;
};

//:defenition
//!c_des: Element of type Image
//!c_des: See also class Shape
//!rel_type: class: ElementList
class Image: public Shape
{
	public:
		// Constructors and destructors
		Image(int layernr,const wxString& path);
		Image();
		~Image();

      void OWrite(Driver_out& where);
      void DoWrite(DrawDriver& dr);

		BoundingBox&			CalcBoundingBox();
		TYPE					  	GetType() {return IMAGE;};
		void 						Transform(wxTransformMatrix* _Matrix);
		Point 					GetPosition();
		void 						SetPosition(Point *a_point);
		void 						SetPosition(double X,double Y);

		Element*					Duplicate();
		Image& 					operator=(Image &other);

		bool			      	Select(Point*);
		bool			      	Select(BoundingBox*,wxTransformMatrix*);
      wxPropertySheet*		GetPropList();
		void 						UpdatePropList();
		void 						Convert(G_Polygon* _polygon, double aber);
      wxImage              GetImage(){return m_image;}

      double					GetH();
      void 						SetH(double newheight);
		void 						SetRotation(double rotation);
		double 					GetRotation();
      double					GetW();
      void                 SetW(double width);
      void                 SetPictureType(const wxString& type);
      wxString             GetPictureType();
      void                 SetPath(const wxString& path);
      wxString             GetPath();
  		void                 SetRelativeMatrix(wxTransformMatrix* m);
		wxTransformMatrix*   GetRelativeMatrix();


	protected:
      double 					m_w;
      double 					m_h;
      wxImage              m_image;
      wxString					m_path;
      wxTransformMatrix		_relative_matrix;
      wxString					m_type;
};

//:defenition
//!c_des: Element of type Box (rectangle with a width parameter)
//!c_des: See also class Shape
//!rel_type: class: ElementList
class SLine : public Shape
{
	public:
		// Constructors and destructors
		SLine(int layernr = 0);
		~SLine();

      void OWrite(Driver_out& where);
      void DoWrite(DrawDriver& dr);

		BoundingBox&			CalcBoundingBox();
		void 						Set(double x1,double y1,double x2, double y2);
      double               GetX1(){return m_x1;};
      double               GetY1(){return m_y1;};
      double               GetX2(){return m_x2;};
      double               GetY2(){return m_y2;};
      void                 SetX1(double v){m_x1=v;};
      void                 SetY1(double v){m_y1=v;};
      void                 SetX2(double v){m_x2=v;};
      void                 SetY2(double v){m_y2=v;};
		TYPE					  	GetType() {return SLINE;};
		void 						Transform(wxTransformMatrix* _Matrix);
		void 						Convert(G_Polygon* _polygon, double marge);

		Element*					Duplicate();
		SLine& 					operator=(SLine &other);

		bool			         Select(Point*);
		bool			         Select(BoundingBox*,wxTransformMatrix*);
      wxPropertySheet*		GetPropList();
		void 						UpdatePropList();

  		void			  	      SetPathtype(PATHTYPE);
		PATHTYPE	  	         GetPathtype();

	protected:

		PATHTYPE					m_pathtype;
      Structure*           m_arrow;
      double 					m_x1;
      double 					m_y1;
      double 					m_x2;
      double 					m_y2;

};
#endif

