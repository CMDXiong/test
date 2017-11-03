#include "trans.h"
#include "referenc.h"
#include "text.h"
#include "arc.h"
#include "math.h"

// Strans -------------------------------------------------------------------
Strans::Strans(double a_angle, double a_scale,
					bool a_abs_angle, bool a_abs_scale, bool a_reflection)
{
	_stransflags.total = 0;

	SetAbsAngle(a_abs_angle);
	SetAbsScale(a_abs_scale);
	SetReflection(a_reflection);
	SetAngle(a_angle);
	SetScale(a_scale);
}

Strans& Strans::operator=(Strans& a_strans)
{
	_stransflags.total = a_strans._stransflags.total;

	SetAbsAngle( bool(a_strans._stransflags.bits.abs_angle) );
	SetAbsScale( bool(a_strans._stransflags.bits.abs_scale) );
	SetReflection( bool(a_strans._stransflags.bits.reflection) );
	SetAngle(a_strans._angle);
	SetScale(a_strans._scale);

	return *this;
}


bool Strans::GetStrans()
{
	return _strans_need;
}

void Strans::SetAngle(double a_angle)
{
	_angle = a_angle;
}


void Strans::SetScale(double a_scale)
{
	_scale = a_scale;
}


void Strans::SetAbsAngle(bool b)
{
	_stransflags.bits.abs_angle = b;
}


void Strans::SetAbsScale(bool b)
{
	_stransflags.bits.abs_scale = b;
}


void Strans::SetReflection(bool b)
{
	_stransflags.bits.reflection = b;
}



double Strans::GetAngle()
{
	return _angle;
}


double Strans::GetScale()
{
	return _scale;
}


bool Strans::GetAbsAngle()
{
	return (bool) _stransflags.bits.abs_angle;
}


bool Strans::GetAbsScale()
{
	return (bool)_stransflags.bits.abs_scale;
}


bool Strans::GetReflection()
{
	return (bool)_stransflags.bits.reflection;
}


void Strans::MakeStrans(Reference* _reference)
{
	double scale_factor = _reference->GetRelativeMatrix()->GetValue(0,0);
	double rot_angle = 0;

   double temp1 = _reference->GetRelativeMatrix()->GetValue(0,0); 	// voor het berekenen van de hoek

   double temp2 = _reference->GetRelativeMatrix()->GetValue(0,1); 	//
   bool check_1 = (_reference->GetRelativeMatrix()->GetValue(0,0) < 0); 		// om te kijken of er horizontaal gespiegeld moet worden

   bool check_2 = (_reference->GetRelativeMatrix()->GetValue(1,1) < 0);			//                    verticaal

   _strans_need=false;

	if (check_1)

   {
		// Reflection horizontal (in y)

  		SetReflection(true);
  		rot_angle = 180;
		SetAngle(rot_angle);
      _strans_need = true;
	}

	if (check_2)

	{
		// Reflection vertical (in x)

		SetReflection(!GetReflection());
      _strans_need = true;
	}


	if (_reference->GetRelativeMatrix()->GetValue(0,1) != 0)
   {

      	// Rotation

      rot_angle = Check_int(atan2(temp2,temp1)*180/M_PI); 														// kijk of dit een integer is (met bepaalde toegestane afwijking)

		SetAngle(rot_angle);
      if (rot_angle != 90 && rot_angle != -90)
	      scale_factor = _reference->GetRelativeMatrix()->GetValue(0,0)/cos((rot_angle/180)*M_PI);
      else
	      scale_factor = _reference->GetRelativeMatrix()->GetValue(1,0)/sin((rot_angle/180)*M_PI); 	// er kan nl. niet door 0 gedeeld worden !
      _strans_need = true;
	}



	// scale

   scale_factor = Check_int(scale_factor);



	if (scale_factor < 0) // schaling altijd positief

      	scale_factor = -scale_factor;



	if (scale_factor != 1) // niet schalen bij 1

   {
		SetScale(scale_factor);
      _strans_need = true;
	}

}

void Strans::MakeStrans(Image* image)
{
	double scale_factor = image->GetRelativeMatrix()->GetValue(0,0);
	double rot_angle = 0;

   double temp1 = image->GetRelativeMatrix()->GetValue(0,0); 	// voor het berekenen van de hoek

   double temp2 = image->GetRelativeMatrix()->GetValue(0,1); 	//
   bool check_1 = (image->GetRelativeMatrix()->GetValue(0,0) < 0); 		// om te kijken of er horizontaal gespiegeld moet worden

   bool check_2 = (image->GetRelativeMatrix()->GetValue(1,1) < 0);			//                    verticaal

   _strans_need=false;

	if (check_1)

   {
		// Reflection horizontal (in y)

  		SetReflection(true);
  		rot_angle = 180;
		SetAngle(rot_angle);
      _strans_need = true;
	}

	if (check_2)

	{
		// Reflection vertical (in x)

		SetReflection(!GetReflection());
      _strans_need = true;
	}


	if (image->GetRelativeMatrix()->GetValue(0,1) != 0)
   {

      	// Rotation

      rot_angle = Check_int(atan2(temp2,temp1)*180/M_PI); 														// kijk of dit een integer is (met bepaalde toegestane afwijking)

		SetAngle(rot_angle);
      if (rot_angle != 90 && rot_angle != -90)
	      scale_factor = image->GetRelativeMatrix()->GetValue(0,0)/cos((rot_angle/180)*M_PI);
      else
	      scale_factor = image->GetRelativeMatrix()->GetValue(1,0)/sin((rot_angle/180)*M_PI); 	// er kan nl. niet door 0 gedeeld worden !
      _strans_need = true;
	}



	// scale

   scale_factor = Check_int(scale_factor);



	if (scale_factor < 0) // schaling altijd positief

      	scale_factor = -scale_factor;



	if (scale_factor != 1) // niet schalen bij 1

   {
		SetScale(scale_factor);
      _strans_need = true;
	}

}

void Strans::MakeStrans(Text* text)
{
	// onderstaande code is om transformatie weg te schrijven.
	// 18/03/98



	double scale_factor = text->GetRelativeMatrix()->GetValue(0,0); // niet definitief!
	double rot_angle;



   double temp1 = text->GetRelativeMatrix()->GetValue(0,0); 	// voor het berekenen van de hoek

   double temp2 = text->GetRelativeMatrix()->GetValue(0,1); 	//
   bool check_1 = (text->GetRelativeMatrix()->GetValue(0,0) < 0); 		// om te kijken of er horizontaal gespiegeld moet worden

   bool check_2 = (text->GetRelativeMatrix()->GetValue(1,1) < 0);			//                    verticaal
	if (check_1)

   {
		// Reflection horizontal (in y)

  		SetReflection(true);
  		rot_angle = 180;
		SetAngle(rot_angle);

		_strans_need = true;

  	}
	if (check_2)

     {
		// Reflection vertical (in x)

		SetReflection(!GetReflection());
		_strans_need = true;

	}

	if (text->GetRelativeMatrix()->GetValue(0,1) != 0)
  	{

      	// Rotation

      double rot_angle = atan2(temp2,temp1)*180/M_PI;

      rot_angle = Check_int(rot_angle); 														// kijk of dit een integer is (met bepaalde toegestane afwijking)
		SetAngle(rot_angle);
      if (rot_angle != 90 && rot_angle != -90)
	      scale_factor = text->GetRelativeMatrix()->GetValue(0,0)/cos((rot_angle/180)*M_PI);
      else
	      scale_factor = text->GetRelativeMatrix()->GetValue(1,0)/sin((rot_angle/180)*M_PI); 	// er kan nl. niet door 0 gedeeld worden !
		_strans_need = true;

	}



	// scale

   scale_factor = Check_int(scale_factor);



	if (scale_factor < 0) // schaling altijd positief

      	scale_factor = -scale_factor;



	if (scale_factor != 1) // niet schalen bij 1

   {

		SetScale(scale_factor);

		_strans_need = true;

	}
}

