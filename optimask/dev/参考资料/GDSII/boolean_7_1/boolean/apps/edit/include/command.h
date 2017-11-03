//! author="klaas"
//! version="1.0"

#ifndef COMMAND_H
#define COMMAND_H

#ifdef __GNUG__
#pragma interface
#endif

#include <stdlib.h>
#include <stdio.h>
#include <fstream>

#include "wxmatrix.h"
#include "tool.h"

class Structure;
class ElementList;
class Element;

//!commands are normally based to CommandHandler by ASCII strings,
/*!
the command string is stored as Command in the commandhistory.
If needed information is added to be able to undo the command.
If possible the command undo is based on the eventstamp, given to the command
when it was executed. E.g for a copy of primitives, the eventstamp is used to
to find the elements that resulted from the copy. Those elements will have the same
eventstamp as the command. Those elements will be removed to undo the command.
\sa BaseTool
\sa CommandHandler
*/
class Command
{
	public:
		Command();
		~Command();

      //!get the keyword of the command string
		const char*	Get_Keyword();
      //!set the keyword of the command string
		void 	Set_Keyword(const wxString& a_keyword);
      //!get the command string
		wxString Get_Command();
      //!set the command string
		bool			Set_Command(char* Format,...);
      //!is undo possible
		bool	Get_Undo();
      //!set if  undo possible
		void 	Set_Undo(bool possible);
      //!get elements list for undo using it
		ElementList* Get_ElementList();
      //!set elements list for undo using it
		void 	Set_ElementList(ElementList* a_elementlist);
      //!get element for undo using it
		Element* Get_Element();
      //!set element for undo using it
		void 	Set_Element(Element* a_element);
      //!get structure for undo using it
		Structure* Get_Structure();
      //!set structure for undo using it
		void 	Set_Structure(Structure* a_structure);
      //!get long number for undo
		long 	Get_Long();
      //!set long number for undo
		void 	Set_Long(long a_long);
      //!get tool for undo using it
		void 	Set_Tool(ToolSet a_tool);
      //!set tool for undo using it
		ToolSet	Get_Tool();
      //!get eventstamp for undo using it
		long 	Get_EventStamp();
      //!set eventstamp for undo using it
		void 	Set_EventStamp(long a_long);
      //!get matrix for undo using it
      wxTransformMatrix* Get_Matrix();
      //!set matrix for undo using it
      void    Set_Matrix(wxTransformMatrix* _undo_trans_matrix);

	private:
      //!command maintains undo information
      bool   _undo;
      //!the complete command string
      wxString _commandstring;
      //!only the keyword of the command
		wxString _keyword;
      //!eventstamp of this command
      long     _eventstamp;
      //!tool in use
      ToolSet  _tool;

      union undo_data
      {
         ElementList* _elementlist;
         Element* 	 _element;
         Structure* 	 _structure;
         long 			 _long;
         char* 		 _string;
         wxTransformMatrix* _matrix;
      } undo_data;
};


#endif
