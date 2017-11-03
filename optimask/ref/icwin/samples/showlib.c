#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "showlib.h"

/* The following routines form the first draft of a simple library that 
is useful in processing .SHO files.  Notice that they make use of the 
following external variables: */

extern FILE *fp_sho, *fp_out;                     
extern char *sho_file_name, *output_file_name; 

/* which must be declared and initialized in your program.  The external
variable: */

extern int errno;

/* is declared and set by the operating system.

Required PROTOTYPES are declared in sholib.h */

/*#**********************************************************************/
/*                                                                      */
/*              char *skip_passed(char *ptr0, char *string)             */
/*                                                                      */
/**************************-******************-**************************/

/* Purpose:

   Skip_passed(ptr0, string) searches a show line starting at ptr0
   until it finds a match for string.  If a match is found it returns
   a pointer to the character following the matching section of the show
   line.  If no match is found it returns NULL.  Skip_passed() uses a 
   case dependent compare.  A blank in string matches 1 or more blanks in 
   the show line.  Thus, if string ends in a blank, the returned pointer 
   will not point to a blank character. */

char *skip_passed(char *ptr0, char *string)
{
   char *ptr = ptr0;
   char *str = string;

   while(*ptr0!='\0'){
      ptr = ptr0;  str = string;
      while(*str==*ptr){
         if(*str==' '){
            while(*ptr==' ') ptr++;
            while(*str==' ') str++;
            }
         else if(*str=='\0') return ptr; /* MATCH */
         else{
            ptr++;
            str++;
            }
         }
      if(*str=='\0') return ptr; /* MATCH */
      ptr0++;
      }
   return NULL;               /* NO MATCH */
   }
/*#**********************************************************************/
/*                                                                      */
/*                     char *find_line(char *string)                    */
/*                                                                      */
/**************************-******************-**************************/

/* Purpose:

   Reads stream *fp_sho until it finds a line that starts with "string".  
   It returns a pointer to to the first non-blank character of the line if
   a match is found.  It returns NULL for no match.  This module uses a case
   dependent compare.  A blank in string matches 1 or more blanks in the 
   show file line.

   Side effects:

   Find_line() uses read_show_line() to read *fp_sho.  Read_show_line()
   reads data into a static buffer.  The pointer it returns points to
   data in this buffer.  Any call to read_show_line() overwrites the
   data that was in the buffer as a result of previous calls.

   The file position for *fp_sho is advanced to the character following
   the end of the matching line.  If no match is found the file pos will
   point to the end of file. */

char *find_line(char *string)
{
   char *ptr, *ptr0, *str;

   while(1){
      ptr0 = read_show_line();
      if(ptr0==NULL) return(NULL); /* RETURN END OF FILE FLAG */
      str = string;
      ptr = ptr0;
      while(*str==*ptr){
         if(*str==' '){
            while(*ptr==' ') ptr++;
            while(*str==' ') str++;
            }
         else if(*str=='\0'){
            return ptr0; /* RETURN POINTER TO MATCHING LINE */
            }
         else{
            ptr++;
            str++;
            }
         }
         if(*str=='\0') return ptr0; /* RETURN POINTER TO MATCHING LINE */
      }
   }
/*#**********************************************************************/
/*                                                                      */
/*                      char *read_show_line(void)                      */
/*                                                                      */
/**************************-******************-**************************/

/* Purpose:

   Read_show_line() reads the next "complete" line from stream *fp_sho.  
   This routine reads whole lines, i.e. if a line ends in an '&' 
   (continuation mark) the '&' is erased and the next line is read and 
   appended to it.  (There are likely to be five or more blanks seperating 
   the two parts  of the merged line.)  Any '\t', '\n', or '\r' characters 
   that appear in the line are replaced by blanks.  Trailing blanks are 
   removed from the end of the line.

   Read_show_line() returns a pointer to to the first non-blank character 
   of the line.  It returns NULL if it reads the end of file before reading
   a line. 

   Side effects:

   Read_show_line() reads data into a static buffer.  The pointer it 
   returns points to data in this buffer.  Any call to read_show_line() 
   overwrites the data that was in the buffer as a result of previous 
   calls.

   The file position for *fp_sho is advanced to the character following
   the end of the line.  If an end of file (eof) is read, it repositions 
   the file pointer to the eof so that the next file read will also return 
   an eof.
   
   */
char *read_show_line(void)
{
   static char buffer[MAX_SHOW_LINE_LEN+1];
   int len = 0;
   int chr = 0;
   char *ptr;
   int n;

   while(1){
      n = read_show_line0(buffer+len, MAX_SHOW_LINE_LEN-len+1);
      if(n==-1){ /* End of file flag */
         if(chr=='&'){
            crash("Unexpected end of file reading \"%s\"\n"
               "The last line ends with an '&' (continuation mark)",
               sho_file_name);
            }
         fseek(fp_sho, 0, SEEK_END);
         return(NULL); /****** RETURN END OF FILE *******/
         }
      len += n;

      /* Check final non-blank character for '&' (continuation mark) */

      if(len==0) continue; /* skip empty line */
      ptr = buffer + len - 1;
      chr = *ptr;

      if(chr!='&'){ /* Skip leading white space */
         ptr = buffer;
         while(*ptr==' ') ptr++;
         if(*ptr=='\0') continue; /* skip empty line */
         return(ptr); /******* RETURN COMPLETE LINE *******/
         }
      /* chr=='&' */

      len--;
      };
   }
/*#**********************************************************************/
/*                                                                      */
/*              read_show_line0(char *buffer, int buffer_len)           */
/*                                                                      */
/**************************-******************-**************************/

/* Purpose: Read_show_line0() is used by read_show_line0() */

int read_show_line0(char *buffer, int buffer_len)
{
   char *ptr;

   /* Read line */

   errno = 0;
   ptr = fgets(buffer, buffer_len, fp_sho);
   if(ptr==NULL){
      if(feof(fp_sho)){
         return(-1);
         }
      sys_crash("Error reading \"%s\"", sho_file_name);
      }

   /* Replace tab, return, and newline characters with blanks */

   for(ptr=buffer; *ptr!='\0'; ptr++){
      if(*ptr=='\t' || *ptr=='\n' || *ptr=='\r') *ptr = ' ';
      }

   /* Remove trailing blanks */

   while(ptr>buffer && *(ptr-1)==' ') ptr--;
   *ptr = '\0';
   return(ptr-buffer);
   }
/*#**********************************************************************/
/*                                                                      */
/*                        crash(char *fmt, ...)                         */
/*                                                                      */
/**************************-******************-**************************/

/* Purpose: Print message, close and remove output file, exit */

void crash(char *fmt, ...)
{
   va_list ap;

   va_start(ap, fmt);
   vprintf(fmt, ap);
   va_end(ap);
   printf("\n");

   if(fp_out!=NULL){
      fclose(fp_out);
      remove(output_file_name);
      }
   exit(10);
   }
/*#**********************************************************************/
/*                                                                      */
/*                      sys_crash(char *fmt, ...)                       */
/*                                                                      */
/**************************-******************-**************************/

/* Purpose: Print message, print system error message, close and remove 
            output file, exit */

void sys_crash(char *fmt, ...)
{
   va_list ap;
   
   va_start(ap, fmt);
   vprintf(fmt, ap);
   va_end(ap);
   printf("\n");

   if(errno!=0) printf("System reports: %s", strerror(errno));

   if(fp_out!=NULL){
      fclose(fp_out);
      remove(output_file_name);
      }
   exit(10);
   }
