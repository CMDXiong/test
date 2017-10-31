/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 * ascii2sf
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


/* This undoes what sf2ascii does. There's no error checking to speak of, it
 * just converts the numbers before the comments back to a binary file. Most
 * of 'em are hex, so there's no problem, the REAL_8 and REAL_4 could be
 * though. If there are more than 9 places to the right of the decimal, a
 * REAL_8 is assumed. If you play around with this and generate the wrong one,
 * you'll get an invalid stream file. Then again, that's a real risk no
 * matter what you do. Read the stream_description.txt before attempting
 * to modify a stream file using these programs. It's real easy to generate
 * trash. BTW, as far as I know you'll never use REAL_4, but it's defined, so
 * the program attempts to handle it. Good luck! */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include "strfcns.h"

int ishex (char *string);
int real_4 (char *string);
unsigned int hex_2_int (char *hex_number);
int count_hex_bytes (char *hex_number);
int find_float_parts (double in_num, double *mantissa);

#define MAX_LINE 1024

int main (void)
    {
    char current_line[MAX_LINE];
    int line_number;
    char *comment_loc;
    char **ascii_numbers;
    int no_fields;
    int field_ktr;
    int mantissa_ktr;
    int int_ktr;
    int sm_ktr;
    double real_number;
    unsigned int integer_number;
    unsigned int part_integer_number;
    int exponent;
    double mantissa;
    unsigned long long int_mantissa;
    unsigned long long part_mantissa;
    unsigned part_mantissa_int;
    int no_bytes;
    int real_4_flag = 0;

    /* get the lines one at a time from stdin */
    line_number = 0;
    while (fgets (current_line, MAX_LINE, stdin))
        {
        /* increment the line number */
        line_number++;

        /* remove the carriage return */
        chop (current_line);

        /* remove comments */
        comment_loc = strchr (current_line, '#');
        if (comment_loc)
            {
            *comment_loc = '\0';
            }

        /* remove leading and trailing spaces, as well as multiple spaces */
        rmlead (current_line, ' ');
        rmtrail (current_line, ' ');
        rmdouble (current_line, ' ');

        /* ignore any lines that are empty */
        if (*current_line)
            {
            /* split the line into fields using ' ' as seperator
             * no_fields is set to the number of fields returned
             * ascii_numbers is a pointer to an array od pointers to char */
            ascii_numbers = split (current_line, &no_fields, ' ', 0);
            /* look at each field (which should be a number in hex or decimal format)
             * one at a time */
            for (field_ktr = 0; field_ktr < no_fields; field_ktr++)
                {
                /* is it a hex number? */
                if (ishex (ascii_numbers[field_ktr]))
                    {
                    /* find the integer value */
                    integer_number = hex_2_int (ascii_numbers[field_ktr]);
                    /* how many bytes in hex number */
                    no_bytes = count_hex_bytes (ascii_numbers[field_ktr]);
                    /* catch any returned errors */
                    if (no_bytes < 0)
                        {
                        fprintf (stderr, "            Line %d\n", line_number);
                        fprintf (stderr, "            That was a fatal error, by the way...\n");
                        exit (1);
                        }
                    /* output the bytes */
                    for (int_ktr = no_bytes - 1; int_ktr >= 0; int_ktr--)
                        {
                        /* make temp copy and rotate to the right app. number of times */
                        part_integer_number = integer_number;
                        for (sm_ktr = 0; sm_ktr < int_ktr; sm_ktr++)
                            {
                            part_integer_number >>= 8;
                            }
                        /* mask off all but right byte */
                        part_integer_number &= 0xff;
                        /* ti tuptuo */
                        putchar (part_integer_number);
                        }
                    }
                else
                    {
                    /* ok, it wasn't a hex number, treat it as a decimal floating point
                     * number, but test to be sure */
                    if (isnum (ascii_numbers[field_ktr]))
                        {
                        /* it passed, so find the value */
                        real_number = atof (ascii_numbers[field_ktr]);
                        /* find the exponent and mantissa */
                        exponent = find_float_parts (real_number, &mantissa);
                        /* GDSII used an "excess 64" notation for the exponent */
                        exponent += 64;
                        /* if it's less than 0, set the high bit */
                        if (real_number < 0)
                            {
                            exponent |= 0x80;
                            }
                        /* check to see whether this should be a real_4 or real_8
                         * if in doubt make it real_8, as real_4 is defined, but never
                         * used */
                        if (real_4 (ascii_numbers[field_ktr]))
                            { /* REAL_4 */
                            if (! real_4_flag)
                                {
                                fprintf (stderr, "***WARNING*** I just output a REAL_4 floating point, since that's\n");
                                fprintf (stderr, "              never used in a stream file, your ASCII file is probably bad.\n");
                                }
                            real_4_flag = 1;
                            /* find the mantissa as the numerator of a binary fraction */
                            mantissa *= pow (2, 24);
                            /* we can consider it an int now, but it has to be long long (64 bit)
                             * for the real_8 type */
                            int_mantissa = mantissa;
                            /* write out the exponent */
                            putchar (exponent);
                            /* write the mantissa one byte at a time */
                            for (mantissa_ktr = 2; mantissa_ktr >= 0; mantissa_ktr--)
                                {
                                /* make a temp copy and shift it the app. number of bytes */
                                part_mantissa = int_mantissa;
                                for (sm_ktr = 0; sm_ktr < mantissa_ktr; sm_ktr++)
                                    {
                                    part_mantissa >>= 8;
                                    }
                                /* mask off all but the right byte */
                                part_mantissa &= 0xff;
                                /* putchar doesn't seem to like long long */
                                part_mantissa_int = part_mantissa;
                                putchar (part_mantissa);
                                }
                            }
                        else
                            { /* REAL_8 */
                            /* find the mantissa as the numerator of a binary fraction */
                            mantissa *= pow (2, 56);
                            /* we can consider it an int now, but it has to be long long (64 bit)
                             * for the real_8 type */
                            int_mantissa = mantissa;
                            /* write out the exponent */
                            putchar (exponent);
                            /* write the mantissa one byte at a time */
                            for (mantissa_ktr = 6; mantissa_ktr >= 0; mantissa_ktr--)
                                {
                                /* make a temp copy and shift it the app. number of bytes */
                                part_mantissa = int_mantissa;
                                for (sm_ktr = 0; sm_ktr < mantissa_ktr; sm_ktr++)
                                    {
                                    part_mantissa >>= 8;
                                    }
                                /* mask off all but the right byte */
                                part_mantissa &= 0xff;
                                part_mantissa_int = part_mantissa;
                                /* putchar doesn't seem to like long long */
                                putchar (part_mantissa_int);
                                }
                            }
                        }
                    else
                        {
                        /* it wasn't a number after all... */
                        fprintf (stderr, "***ERROR*** Not a number: '%s' line %d\n",
                                 ascii_numbers[field_ktr], line_number);
                        fprintf (stderr, "            That was a fatal error, by the way...\n");
                        exit (1);
                        }
                    }
                }

            }

        }

    exit (0);

    }

int ishex (char *string)
    {
    char *temp_string;
    int hex_truth = 1;
    int ktr;

    temp_string = strdup (string);

    strlwr (temp_string);

    if (strncmp (temp_string, "0x", 2))
        {
        hex_truth = 0;
        goto done;
        }

    for (ktr = 2; ktr < strlen (temp_string); ktr++)
        {
        if (!isxdigit (*(temp_string + ktr)))
            {
            hex_truth = 0;
            goto done;
            }
        }

    done:;
    free (temp_string);

    return (hex_truth);

    }

/* not general purpose, assumes some things that have already happened in this program */
int real_4 (char *string)
    {
    char *point_loc;

    /* must have a decimal point */
    point_loc = strchr (string, '.');

    /* and there must be 9 or fewer places to the right of it */
    if (point_loc)
        {
        if (strlen (point_loc) < 11)
            {
            return (1);
            }
        }

    return (0); /* failure */

    }

unsigned int hex_2_int (char *hex_number)
    {
    unsigned int out_num;
    char *local_hex;
    int length_local_hex;
    int ktr;
    int value;

    /* remove any leading "0x" or "0X" */
    if (! strncmp (hex_number, "0x", 2) || ! strncmp (hex_number, "0X", 2))
        {
        local_hex = hex_number + 2;
        }
    else
        {
        local_hex = hex_number;
        }

    /* convert it one char at a time */
    out_num = 0;
    length_local_hex = strlen (local_hex);
    for (ktr = 0; ktr < length_local_hex; ktr++)
        {
        switch (tolower (*(local_hex + length_local_hex - ktr - 1)))
            {
            case '0':
                value = 0;
                break;
            case '1':
                value = 1;
                break;
            case '2':
                value = 2;
                break;
            case '3':
                value = 3;
                break;
            case '4':
                value = 4;
                break;
            case '5':
                value = 5;
                break;
            case '6':
                value = 6;
                break;
            case '7':
                value = 7;
                break;
            case '8':
                value = 8;
                break;
            case '9':
                value = 9;
                break;
            case 'a':
                value = 10;
                break;
            case 'b':
                value = 11;
                break;
            case 'c':
                value = 12;
                break;
            case 'd':
                value = 13;
                break;
            case 'e':
                value = 14;
                break;
            case 'f':
                value = 15;
                break;
            default:
                /* not a valid hex digit */
                return (-1);
                break;
            }

        out_num += value * pow (16, ktr);
        }

    return (out_num);

    }
	
int find_float_parts (double in_num, double *mantissa)
    {
    int exponent;

    in_num = fabs (in_num);

    if (fabs (in_num) < 1e-18)
        {
        exponent = 0;
        *mantissa = 0;
        }
    else
        {
        for (exponent = -64; exponent < 64; exponent++)
            {
            *mantissa = in_num / pow (16, exponent);
            if (*mantissa <= 1)
                {
                break;
                }
            }
        }

    return (exponent);

    }

int count_hex_bytes (char *hex_number)
    {
    char *local_hex;
    int no_char;

    /* remove any leading "0x" or "0X" */
    if (! strncmp (hex_number, "0x", 2) || ! strncmp (hex_number, "0X", 2))
        {
        local_hex = hex_number + 2;
        }
    else
        {
        local_hex = hex_number;
        }

    no_char = strlen (local_hex);

    /* make sure it's an even number */
    if (fabs (no_char / 2 - (double) no_char /2) > .001)
        {
        fprintf (stderr, "***ERROR*** Odd number of bytes!\n");
        return (-1);
        }

    return (no_char / 2);

    }
