/*
	WINMessage
	WINMES.H
*/

#ifndef _WINMESS_
#define _WINMESS_

#ifdef __GNUG__
#pragma interface
#endif

#include "gdsmes.h"

using namespace std;

class WINMessage : public GDSMessage
{
	private:
	public:
		WINMessage();
		~WINMessage();
		void error			(char *text, char *title);
		void info			(char *text, char *title);
		void text			(char *text, char *title);
		void debug			(char *text, char *title);
		int	 question_yesno	(char *text, char *title);
};
#endif

