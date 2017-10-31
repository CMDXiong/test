/*
Program		DRIVER.CPP
Purpose		Base class for output drivers (<< support)
Programmer		K. Holwerda
Last Update		24-09-96
*/


#ifdef __GNUG__
#pragma implementation
#endif

#include "wxwin.h"

#include "drivout.h"

#include "arc.h"
#include "boundary.h"
#include "elemlst.h"
#include "gdsii.h"
#include "path.h"
#include "point.h"
#include "segment.h"
#include "polyline.h"
#include "referenc.h"
#include "structur.h"
#include "text.h"
#include "units.h"

void Driver_out::OWrite(Element* _Element)
{
   if  (!_Element->GetStructure())
   {
            Shape* a_shape= (Shape*) _Element;
            if ((_only_visible) && (!GLOB->SETUP->Get_Visible(a_shape->GetLayer())))
               return;
   }

/*
   switch (_Element->GetType())
   {
      case BOUNDARY:
           {
            Boundary* a_boundary= (Boundary*) _Element;
            Write(a_boundary);
            break;
           }
      case PATH:
           {
            Path* a_path= (Path*) _Element;
            Write(a_path);
            break;
           }
      case ARCSEG:
           {
            Arcseg* a_arc= (Arcseg*) _Element;
            Write(a_arc);
            break;
           }
      case TEXT:
           {
            Text* a_text= (Text*) _Element;
            Write(a_text);
            break;
           }
      case AREF:
          {
            ArrayReference* a_arrayreference = (ArrayReference*) _Element;
            Write(a_arrayreference);
            break;
          }
      case SREF:
          {
            StructureReference* a_structurereference = (StructureReference*) _Element;
            Write(a_structurereference);
            break;
          }

      case BOX:
          {
            Box* a_box= (Box*) _Element;
            Write(a_box);
            break;
          }
      case CIRCLE:
          {
            Circle* a_cir= (Circle*) _Element;
            Write(a_cir);
            break;
          }
      default:
         throw GDS_Error("Unknown element in element list","error",9);
   }
*/
   _Element->Write(*this);

   return;
}

// ElementList---------------------------------------------------------------
void Driver_out::OWrite(ElementList* _elementList)
{
  	TDLI<Element> I=TDLI<Element>(_elementList);

	I.toroot();
	while (I.iterate())
		OWrite(I.item());
}


