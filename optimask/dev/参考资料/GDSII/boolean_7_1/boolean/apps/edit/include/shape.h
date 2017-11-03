//!! author="klaas"
//!! version="1.0"
/*
Program	SHAPE.H
Purpose	base class for shapes
Last Update	24-09-96
*/

#ifndef SHAPE_H
#define SHAPE_H

#ifdef __GNUG__
#pragma interface
#endif

#include "bbox.h"
#include "element.h"
class G_Polygon;

//!virtual Base class for all primitive types.
/*!
A shape is a primitive that can be drawn directly,
without refrencing other shapes, also it has a boundingbox.
Therefore a reference to a structure is not a shape.
That kind of elements are derived from the class Element directly
\sa Path
\sa Text
\sa Boundary
\sa Box
\sa Circle
\sa Arcseg
*/
class Shape : public Element
{
	public:
		//! Constructors
		Shape();
      void Write(DrawDriver& dr);
      //!calculate the boundingbox for the shape
		BoundingBox&				GetBoundingBox();

      //!get the layer of the shape
		virtual int 			 					GetLayer();
      //!set the layer of the shape
		virtual void			 					SetLayer(int);
      //!get the width of the shape
		virtual double								GetWidth();
      //!set the width of the shape
		virtual void			 					SetWidth(double width);

      //!convert primitive to polygon
		virtual void                        Convert(G_Polygon*, double)=0;

      //!get property list
      virtual wxPropertySheet*		GetPropList();
      //!update property list (remove members of object)
		void UpdatePropList();

		Shape& 				operator=(Shape &other);

	protected:
      //!layer off primitive
		int	m_layer;
      //!width of primitive
		float	m_width;
};

#endif
