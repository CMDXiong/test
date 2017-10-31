/*
	WINMessage
	WINMES.CPP
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include <fstream>

#include "gdsmes.h"
#include "wx/wxprec.h"
#include "wxwin.h"
#include "gdserr.h"
#include "stdio.h"

#include "winmess.h"

WINMessage::WINMessage()
{


};

WINMessage::~WINMessage()
{};

void WINMessage::error(char *text, char *title)
{
	if (m_name)
      m_Log_file << text << " " << title << endl;
   else
		wxMessageBox(text, title, wxICON_HAND);
	throw GDS_Error("fatal error", "Fatal Error", 9, 1);
};

void WINMessage::info(char *text, char *title)
{
	if (m_name)
      m_Log_file << text << " " << title << endl;
   else
		wxMessageBox(text, title, wxICON_INFORMATION);
};

void WINMessage::text(char *text, char *title)
{
	if (m_name)
      m_Log_file << text << " " << title << endl;
   else
		wxMessageBox(text, title, wxICON_INFORMATION);
};

void WINMessage::debug(char *text, char *title)
{
	if (m_name)
      m_Log_file << text << " " << title << endl;
   else
		wxMessageBox(text, title, wxOK);
};

int WINMessage::question_yesno(char *text, char *title)
{
	if (m_name)
      m_Log_file << text << " " << title << endl;
   else
   {
		if (wxMessageBox(text, title, wxYES_NO| wxICON_QUESTION) == wxYES)
			return 1;
   }
	return 0;
};

