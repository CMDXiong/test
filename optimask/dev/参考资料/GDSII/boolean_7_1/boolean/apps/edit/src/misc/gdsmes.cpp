#ifdef __GNUG__
#pragma implementation
#endif

#include "wxwin.h"

#include "gdsmes.h"

GDSMessage::GDSMessage(void)
{
      m_name=0;
}

void GDSMessage::SetLogFile(char* name)
{
   if (strlen(name) > 0)
   {
		delete [] m_name;
      m_name=new char[strlen(name)+1];
      strcpy(m_name,name);
      m_Log_file.open(name);
      strcpy(m_name,name);
   }
   else
   {
		delete [] m_name;
        m_name=0;
        m_Log_file.close();
   }
}

char* GDSMessage::GetLogFile()
{
   return m_name;
}


GDSMessage::~GDSMessage(void)
{
	delete [] m_name;
	m_Log_file.close();
}

