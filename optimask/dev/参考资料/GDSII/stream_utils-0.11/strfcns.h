/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 * strfcns.h, part of the pnews package by Jim Buchanan
 * 
 * You can contact me via email at:
 * 
 * jim@buchanan1.net
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 * Copyright (C) 1996-1998 Jim Buchanan
 *
 * This program may be freely distributed under the terms of the GPL
 * General Public License, any version you prefer. The GPL may be found
 * at http://www.gnu.org/
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

char chop (char *in_string);
void strlwr (char *in_string);
char *rmlead (char *string, char delim);
char *rmtrail (char *string, char delim);
char *rmdouble (char *string, char delim);
int charcount (char *string, char delim);
char **split (char *string, int *no_fields, char delim, int rem_mult);
int isnum (char *in_string);
int li_non_sci (char *local_string);
int li_good_char (char in_char);
