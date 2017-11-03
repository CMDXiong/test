
#ifndef TRANS_H
#define TRANS_H

#ifdef __GNUG__
#pragma interface
#endif
//
// defaults for Strans
//
#define	DEFAULT_STRANS_ANGLE			0
#define DEFAULT_STRANS_SCALE			1.0
#define DEFAULT_STRANS_ABS_ANGLE		false
#define DEFAULT_STRANS_ABS_SCALE		false
#define DEFAULT_STRANS_REFLECTION	false

#include "gdsii.h"
#include "gdsdef.h"
class Reference;
class Text;
class Image;

typedef struct gds_strans_bitflags
{
#if wxBYTE_ORDER == wxBIG_ENDIAN
		unsigned reflection:	1;
		unsigned	reserved1 : 7;
		unsigned	reserved2 : 5;
		unsigned	abs_scale :	1;
		unsigned	abs_angle :	1;
		unsigned	reserved  :	1;
#else
		unsigned	reserved  :	1;
		unsigned	abs_angle :	1;
		unsigned	abs_scale :	1;
		unsigned	reserved2 : 5;
		unsigned	reserved1 : 7;
		unsigned	reflection:	1;
#endif
} gds_strans_bitflags ;

class Strans
{
	public:
		// constructor
		Strans(double nw_angle=DEFAULT_STRANS_ANGLE,
				double nw_scale=DEFAULT_STRANS_SCALE,
				bool nw_abs_angle=DEFAULT_STRANS_ABS_ANGLE,
				bool nw_abs_scale=DEFAULT_STRANS_ABS_SCALE,
				bool nw_reflection=DEFAULT_STRANS_REFLECTION);

		Strans& operator=(Strans& a_strans);

		void MakeStrans(Reference* reference);
		void MakeStrans(Text* text);
		void MakeStrans(Image* image);

		// public member functions
		bool					GetStrans();
		void					SetAngle(double);
		void					SetScale(double);
		void					SetAbsAngle(bool);
		void					SetAbsScale(bool);
		void					SetReflection(bool);
		double GetAngle();
		double GetScale();
		bool				GetAbsAngle();
		bool				GetAbsScale();
		bool				GetReflection();

		union  {
					short int total;
					gds_strans_bitflags bits;
				 } _stransflags;

	protected:
		double			_angle;
		double			_scale;
      bool           _strans_need;
};

#endif