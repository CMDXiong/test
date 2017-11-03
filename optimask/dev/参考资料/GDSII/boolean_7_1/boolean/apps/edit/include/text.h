/*
Program	TEXT.H
Purpose	GdsII text definition
Programmers	R. Spekreijse & N. Noorlander
Last Update	12-12-1995
*/

//! author="klaas"
//! version="1.0"
#ifndef TEXT_H
#define TEXT_H

#ifdef __GNUG__
#pragma interface
#endif

#include <string.h>
#include <stdlib.h>

#include "misc.h"
#include "wxmatrix.h"
#include "point.h"
#include "bbox.h"
#include "gdserr.h"
#include "gdsii.h"
#include "shape.h"

class DrawDriver;
class Driver_out;

class Strans;

// temporary defines, textfont can be different
#define 	_char_width		10
#define 	_char_height	8

enum TEXT_PATHTYPE {TEXT_PATH_END_SQAURE,TEXT_PATH_END_ROUND,TEXT_PATH_END_SQAURE_EXT};
typedef  short int TEXT_TYPE;

//
// enumerates used by the Presentation class
//
enum {FONT0,FONT1,FONT2,FONT3};
enum {TOP, MIDDLE, BOTTOM};
enum {LEFT, CENTER, RIGTH};

typedef struct pres_bitflags
{

#if wxBYTE_ORDER == wxBIG_ENDIAN
		unsigned	reserved  :	 10;
		unsigned	_font:		 2;
		unsigned	_vertical: 	 2;
		unsigned	_horizontal: 2;
#else
		unsigned	_horizontal: 2;
		unsigned	_vertical: 	 2;
		unsigned	_font:		 2;
		unsigned	reserved  :	 10;
#endif
} pres_bitflags;


//
// defaults for Presentation
//
#define DEFAULT_PRESENTATION_FONT			FONT0
#define DEFAULT_PRESENTATION_VERTICAL		TOP
#define DEFAULT_PRESENTATION_HORIZONTAL	LEFT
#define DEFAULT_FONT_HEIGHT					1

//!Element of type Text
/*!
\sa ElementList
*/
class Text : public Shape
{
	public:
		// Constructors & destructor
		Text(int layernr = 0);
		~Text();

      void OWrite(Driver_out& where);
      void DoWrite(DrawDriver& dr);

		TYPE					  	GetType() {return TEXT;};
		void						GetPoint(Point& offsetpoint);

		// public member funtions
		void 					  	Init(int);
		Element*					Duplicate();
		Text&					  	operator=(Text&);
		// get and set functions
		void                 SetRelativeMatrix(wxTransformMatrix* m);
		wxTransformMatrix*      GetRelativeMatrix();
		BoundingBox& 			CalcBoundingBox();
		void			  	SetPresentationFlags(	short int total);
		short int		  	GetPresentationFlags( );
		TEXT_TYPE	GetTexttype();
		void			  	SetTexttype(TEXT_TYPE);
		TEXT_PATHTYPE	GetPathtype();
		void			  	SetPathtype(TEXT_PATHTYPE);
		void			  	SetText(const wxString& text);
		wxString		  	MyText();
		void 					   Transform(wxTransformMatrix* _Matrix);
      void                 Convert(G_Polygon* noneed, double marge);

		bool			Select(Point*);
		bool			Select(BoundingBox*,wxTransformMatrix*);

      double					GetHeight();
      void 						SetHeight(double newheight);
      Point	               GetMin();
      Point 	            GetMax();

		wxPropertySheet* GetPropList();
      void 				UpdatePropList();

		void SetFont(char nw_font = DEFAULT_PRESENTATION_FONT);
		void SetVertical(char nw_vertical = DEFAULT_PRESENTATION_VERTICAL);
		void SetHorizontal(char nw_horizontal = DEFAULT_PRESENTATION_HORIZONTAL);
		char	GetFont();
		char	GetVertical();
		char	GetHorizontal();
		void SetRotation(double rotation);
		double GetRotation();


	protected:
		wxString				  	_text;
		TEXT_TYPE			  	_texttype;
		TEXT_PATHTYPE		  	_pathtype;
      wxTransformMatrix 	_relative_matrix;
      Point	               _min;
      Point	               _max;
		union  {
					short int total;
					pres_bitflags bits;
				 } _presentationflags;
};

#endif
