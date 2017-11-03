#include"laytreenode.h"
LayerRecordNode::LayerRecordNode()
{

    layerlock = false;
    layerhide = false;
    layerprot = false;
    layerfill = false;
    layerdatatype = QString("TYPE ");
    layernote = QString("Note 1");
    parent=NULL;
}

void LayerRecordNode::insertChild(int position,LayerRecordNode*node)
{
    this->children.insert(position,node);
}


