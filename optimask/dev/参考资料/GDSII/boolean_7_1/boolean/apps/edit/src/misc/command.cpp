#ifdef __GNUG__
#pragma implementation
#endif

#include "wxwin.h"

#include "gdserr.h"

#include "command.h"
#include "elemlst.h"
#include "structur.h"
#include "element.h"
#include	<stdarg.h>

Command::Command()
{
		_undo = false;
      _commandstring.Clear();
      _keyword.Clear();
}

Command::~Command()
{
}

const char*	Command::Get_Keyword()
{
	return _keyword.c_str();
}

void Command::Set_Keyword(const wxString& a_keyword)
{
	_keyword=a_keyword;
}

wxString Command::Get_Command()
{
	return _commandstring;
}

bool Command::Set_Command(char* Format,...)
{
	va_list ap;

   wxString buffer;
	va_start(ap,Format);
	_commandstring.PrintfV(Format,ap);
	va_end(ap);

   return true;
}

bool Command::Get_Undo()
{
	return _undo;
}

void Command::Set_Undo(bool possible)
{
   _undo = possible;
}

ElementList*	Command::Get_ElementList()
{
  return undo_data._elementlist;
}

void Command::Set_ElementList(ElementList* a_elementlist)
{
  undo_data._elementlist=a_elementlist;
}

Element*	Command::Get_Element()
{
  return undo_data._element;
}

void Command::Set_Element(Element* a_element)
{
  undo_data._element=a_element;
}

Structure* Command::Get_Structure()
{
  return undo_data._structure;
}

void Command::Set_Structure(Structure* a_structure)
{
  undo_data._structure=a_structure;
}

long Command::Get_Long()
{
  return undo_data._long;
}

void Command::Set_Long(long a_long)
{
  undo_data._long=a_long;
}

long Command::Get_EventStamp()
{
  return _eventstamp;
}

void Command::Set_EventStamp(long a_long)
{
  _eventstamp=a_long;
}

wxTransformMatrix* Command::Get_Matrix()
{
  return  undo_data._matrix;

}

void    Command::Set_Matrix(wxTransformMatrix* _undo_trans_matrix)
{
	undo_data._matrix=_undo_trans_matrix;
}

void 	Command::Set_Tool(ToolSet a_tool)
{
  _tool=a_tool;
}


ToolSet Command::Get_Tool()
{
  return _tool;
}

