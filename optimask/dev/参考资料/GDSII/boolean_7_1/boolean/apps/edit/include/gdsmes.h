/*
	GDSMessage
	GDSMES.H
*/

#ifndef _GDSMESS_
#define _GDSMESS_

#ifdef __GNUG__
#pragma interface
#endif


#include <sstream>
#include <fstream>
#include <iostream>

#include <stdio.h>
#include "gdserr.h"

class GDSMessage
{
	public:
		GDSMessage(void);
		virtual ~GDSMessage(void)=0;
      void SetLogFile(char* name);
      char* GetLogFile();
		virtual void 	error		(char *text, char *title) =0;
		virtual void 	info		(char *text, char *title) =0;
		virtual void 	text		(char *text, char *title) =0;
		virtual void 	debug		(char *text, char *title) =0;
		virtual int	question_yesno	(char *text, char *title) =0;
   protected:
      char* m_name;
		std::ofstream	m_Log_file;
}; 

#endif
