#pragma option -Jgd

#include "_dl_itr.h"
#include "structur.h"
#include "referenc.h"
#include "segment.h"
#include "node.h"
#include "element.h"
#include "record.h"
#include "link.h"
#include "alias.h"
#include "_lnk_itr.h"
#include "scanbeam.h"
#include "scrndrv.h"
#include "graph.h"
#include "graphlst.h"
#include "misc.h"
#include "bbox.h"
#include "propedit.h"
#include "proper.h"
#include "toolcont.h"
#include "tool.h"
#include "command.h"
#include "linepars.h"

template class DL_Node<char *>;
template class DL_Iter<char *>;
template class DL_List<char *>;
template class DL_Node<const wxString*>;
template class DL_Iter<const wxString*>;
template class DL_List<const wxString*>;
template class DL_Node<void *>;
template class DL_Iter<void *>;
template class DL_List<void *>;
template class DL_SortIter<void *>;
template class DL_StackIter<void *>;

template class DL_Node<Alias *>;
template class DL_Iter<Alias *>;
template class DL_List<Alias *>;

template class DL_Node<int>;
template class DL_Iter<int>;
template class DL_List<int>;

template class DL_Node<Command*>;
template class DL_Iter<Command*>;
template class DL_List<Command*>;

template class TDLI<Property>;
template class TDLI<Structure>;
template class TDLI<Reference>;
template class TDLI<Node>;
template class TDLI<Segment>;
template class TDLI<Element>;
template class TDLI<Record>;
template class TDLI<Link>;
template class TDLI<Graph>;
template class TDLI<Alias>;
template class TDLIStack<BoundingBox>;
template class TDLIStack<BaseTool>;
template class TDLI<BaseTool>;
template class TDLI<Line_Parser>;
template class TDLIStack<Line_Parser>;
