/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 * strfcns.c, part of the pnews package by Jim Buchanan
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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include "strfcns.h"

char chop (char *in_string)
    {
    int last_loc;
    char last_char;

    last_loc = strlen (in_string) - 1;
    last_char = in_string[last_loc];

    in_string[last_loc] = '\0';

    return (last_char);

    }

void strlwr (char *in_string)
    {
    int ktr;

    ktr = 0;
    while ((in_string[ktr] = tolower(in_string[ktr])))
        ktr++;

    }

/* I changed this for stream utilities */
char *rmlead (char *string, char delim)
    {
    int ktr;
    int offset;

    offset = -1;
    while (string[++offset] == delim);

    ktr = -1;
    while (string[++ktr])
        {
        string[ktr] = string[ktr + offset];
        }

    return (string);

    }

char *rmtrail (char *string, char delim)
    {
    int ptr;

    ptr = strlen (string);

    while (string[--ptr] == delim);

    string[ptr + 1] = '\0';

    return (string);

    }

char *rmdouble (char *string, char delim)
    {
    int ptr;

    if (*string)
        {
        ptr = 0;
        while (string[++ptr])
            {
            if ((string[ptr] == delim) &&
                (string[ptr - 1] == delim))
                {
                rmlead (string + ptr, delim);
                }
            }
        }

    return (string);

    }

int charcount (char *string, char delim)
    {
    int ptr;
    int count;

    ptr = -1;
    count = 0;
    while (string[++ptr])
        if (string[ptr] == delim)
            count++;

    return (count);

    }

/* split will return a NULL pointer on some errors (such as out of memory or
   a null string for input), check and be careful! */
char **split (char *string, int *no_fields, char delim, int rem_mult)
    {
    char *local_string;
    char **local_mat;
    int start;
    int end;
    int field_no;

    local_string = strdup (string);

    if (local_string != NULL)
        {
        if (rem_mult)
            {
            rmlead (local_string, delim);
            rmtrail (local_string, delim);
            rmdouble (local_string, delim);
            }

        if (*local_string != '\0')
            {
            *no_fields = charcount (local_string, delim) + 1;

            local_mat = (char **) malloc (sizeof (char *) * *no_fields);

            if (local_mat)
                {
                start = 0;
                end = -1;
                field_no = -1;
                do
                    {
                    end++;
                    if ((local_string[end] == delim) ||
                        (local_string[end] == '\0'))
                        {
                        field_no++;
                        local_mat[field_no] = (char *) malloc (sizeof (char) * end - start + 1);
                        if (! local_mat[field_no])
                            {
                            return (NULL);
                            }
                        strncpy (local_mat[field_no], local_string + start, end - start);
                        local_mat[field_no][end - start] = '\0';
                        start = end + 1;
                        }
                    }
                while (local_string[end]);
                }
            }
        else
            {
            *no_fields = 0;
            local_mat = NULL;
            }

        free (local_string);

        }
    else
        {
        local_mat = NULL;
        }

    return (local_mat);

    }

/*
   numbers have '0'-'9', '.', 'e', 'E', '+', '-'
   'e' or 'E' cannot be the first char
   there must be numbers before and after 'e' or 'E'
   there must only be one 'e' or 'E'
   '+' and '-' must be first char or after 'e' or 'E'
   only one '.' on either side of the 'e' or 'E'
*/
int isnum (char *in_string)
    {
    char *local_string;
    char *e_string;

    local_string = strdup (in_string);
    if (! local_string)
        {
        fprintf (stderr, "***ERROR*** strfcns: Could not malloc in isnum()\n");
        exit (1);
        }
		
    strlwr (local_string);

    /* can only have one 'e' */
    if (charcount (local_string, 'e') > 1)
        {
        free (local_string);
        return (0);
        }

    /* 'e' can't be the first or last char */
    if ((*local_string == 'e') ||
        (local_string[strlen (local_string) - 1]) == 'e')
        {
        free (local_string);
        return (0);
        }

    /* find a possible 'e' */
    /* seperate the strings if so */
    e_string = strchr (local_string, 'e');
    if (e_string)
        {
        *e_string = '\0';
        e_string++;
        /* check the post e part */
        if (! li_non_sci (e_string))
            {
            free (local_string);
            return (0);
            }
        }

    /* check the pre e part */
    if (! li_non_sci (local_string))
        {
        free (local_string);
        return (0);
        }


    free (local_string);
    return (1);

    }

int li_non_sci (char *local_string)
    {
    char *cp;
    int length_string;
    int string_ktr;

    /* must only use proper chars */
    length_string = strlen (local_string);
    for (string_ktr = 0; string_ktr < length_string; string_ktr++)
        {
        if (! li_good_char (local_string[string_ktr]))
            {
            return (0);
            }
        }

    /* can't have more than one '.', '-'. '+' */
    if ((charcount (local_string, '.') > 1) ||
        (charcount (local_string, '-') > 1) ||
        (charcount (local_string, '+') > 1))
        {
        return (0);
        }

    /* '-' of '+' must be first char */
    cp = strchr (local_string, '-');
    if (cp)
        {
        if (cp != local_string)
            {
            return (0);
            }
        }
    cp = strchr (local_string, '+');
    if (cp)
        {
        if (cp != local_string)
            {
            return (0);
            }
        }

    return (1);

    }

int li_good_char (char in_char)
    {
    char ok[13] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '+', '.'};
    int ktr;

    for (ktr = 0; ktr < 13; ktr++)
        {
        if (ok[ktr] == in_char)
            {
            return (1);
            }
        }

    return (0);

    }
