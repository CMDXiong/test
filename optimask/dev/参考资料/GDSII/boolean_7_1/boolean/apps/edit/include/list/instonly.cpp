#pragma option -Jgd

#include "_dl_itr.h"
#include "node.h"
#include "record.h"
#include "boolonly.h"
#include "link.h"
#include "_lnk_itr.h"
#include "scanbeam.h"
#include "graph.h"
#include "graphlst.h"
#include "misc.h"

template class DL_Node<void *>;
template class DL_Iter<void *>;
template class DL_List<void *>;

template class DL_Node<int>;
template class DL_Iter<int>;
template class DL_List<int>;

template class TDLI<Node>;
template class TDLI<G_Polygon>;
template class TDLI<Point>;
template class TDLI<LPoint>;
template class TDLI<Record>;
template class TDLI<Link>;
template class TDLI<Graph>;
