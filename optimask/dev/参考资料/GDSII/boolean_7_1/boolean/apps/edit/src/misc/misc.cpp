//
// misc.cpp
//
//
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "misc.h"
#include <math.h>

#ifdef __UNIX__
int stricmp(const char* a, const char* b)
{
	int i = 0, l = (strlen(a) > strlen(b)? strlen(b) : strlen(a));
	while ( i < l ) {
		if (tolower(a[i]) != tolower(b[i])) {
			if (tolower(a[i]) < tolower(b[i])) return -1;
			else return 1;
		}; i++;
	}
	if (strlen(a) != strlen(b)) {
		if (strlen(a) > strlen(b)) return 1;
		else return -1;
	} else return 0;
}
#endif

double Check_int(double getal)
{
	// check if the number is very close to an integer
	if ( (ceil(getal) - getal) < 0.0001)
		return ceil(getal);

	else if ( (getal - floor(getal)) < 0.0001)

		return floor(getal);

	return getal;

}

wxString ConvertToUnixPath( const wxString& dospath )
{
   wxString condospath;
   condospath = dospath;
#if defined(__UNIX__) || defined(__GNUC__)
#else
   //if unix like path starting with a slash on windows, this means driver leter like "/p" = "p:"
   if ( dospath[0] == '/' ) 
   {
      condospath = dospath.Mid(1,1) + ":"; 
      condospath = condospath + dospath.Mid(2); 
   }
   condospath.Replace("\\","/");
#endif
   return condospath; 
}
