#ifndef LAYTREENODE_H
#define LAYTREENODE_H
#include<QList>
#include<QString>
class LayerRecordNode
{
 public:
    LayerRecordNode();
    QString layergroup;            //构层组群(Layer Group)或类别(Category)或排序(Order) (Ref.: K-Layout)
    QString layername;             //构层名称(Layer Name)
    bool layerlock;                //构层锁定(LOCK=1, UNLOCK=0)
    bool layerhide;                //构层隐藏(HIDE=1=UNSHOW, UNHIDE=0=SHOW)
    bool layerprot;                //构层保护(PROTect=1=Unselectable, UNPROT=0=Selectable)
    bool layerfill;                //构层填充(FILL=1, UNFILL=0)
    int layergdsnum;               //构层GDSII标号(Layer Number)
    QString layerdatatype;         //构层GDSII数据类型(Layer Data Type)
    QString layernote;             //构层注释(Layer Note)
    void insertChild(int position,LayerRecordNode*node);

    LayerRecordNode* parent;            //父节点
    QList<LayerRecordNode*> children;//子节点列表
};

#endif // LAYTREENODE_H

