/* 构层面板表格模式 */

#define COLUMN_LAYERGROUP 0            //构层组群(Layer Group)或类别(Category)或排序(Order) (Ref.: K-Layout)
#define COLUMN_LAYERNAME  1            //构层名称(Layer Name)
#define COL_CHKBOX_LOCK   2            //是否锁定(LOCK=1, UNLOCK=0)
#define COL_CHKBOX_HIDE   3            //是否隐藏(HIDE=1=UNSHOW, UNHIDE=0=SHOW)
#define COL_CHKBOX_PROT   4            //是否保护(PROTect=1=Unselectable, UNPROT=0=Selectable)
#define COL_CHKBOX_FILL   5            //是否填充(FILL=1, UNFILL=0)
#define COLUMN_GDSNUMBER  6            //GDSII构层标号(Layer Number)
#define COLUMN_DATATYPE   7            //GDSII构层数据类型(Layer Data Type)
#define COLUMN_LAYERNOTE  8            //构层注释(Layer Note)

#include "laytreemodel.h"
#include <QPixmap>

const QString layerrsrcPath = "../src/Icon";     //指定图标目录

//----------------LayTableModel函数的定义-------------------------
LayTableModel::LayTableModel(QObject *parent)
    : QAbstractItemModel(parent){
    RootNode=new LayerRecordNode;
}

LayTableModel::~LayTableModel(){}


/* 行数 */
int LayTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
           return 0;
      LayerRecordNode *parentNode = nodeFromIndex(parent);
      if (!parentNode)
            return 0;
      return parentNode->children.count();
}

/* 列数 */
int LayTableModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 9;
}
//父节点索引
QModelIndex LayTableModel::parent(const QModelIndex &child) const
{
        LayerRecordNode *node = nodeFromIndex(child);
        if (!node)
            return QModelIndex();
        LayerRecordNode *parentNode = node->parent;
        if (!parentNode)
                return QModelIndex();
        LayerRecordNode *grandparentNode = parentNode->parent;
        if (!grandparentNode)
                return QModelIndex();

        int row = grandparentNode->children.indexOf(parentNode);
        return createIndex(row, 0, parentNode);
}
//子节点索引
QModelIndex LayTableModel::index(int row, int column, const QModelIndex &parent) const
{
        if (!RootNode || row < 0 || column < 0)
                return QModelIndex();
        LayerRecordNode *parentNode = nodeFromIndex(parent);
        LayerRecordNode *childNode = parentNode->children.value(row);
        if (!childNode)
                return QModelIndex();
        return createIndex(row, column, childNode);
}
//根据索引获取内部指针
LayerRecordNode *LayTableModel::nodeFromIndex(const QModelIndex &index) const
{
        if (index.isValid()) {
                return static_cast<LayerRecordNode *>(index.internalPointer());
        } else {
                return RootNode;
        }
}
// 表格项数据
QVariant LayTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    LayerRecordNode *record = nodeFromIndex(index);
    int nColumn = index.column();

    switch (role)
    {
      case Qt::TextAlignmentRole:
          return QVariant(Qt::AlignHCenter | Qt::AlignVCenter);

      case Qt::DisplayRole: //构层属性
      {
        switch (nColumn) {
          //case COLUMN_LAYERGROUP : return record.layergroup;
          case COLUMN_LAYERNAME : return record->layername;
          case COLUMN_GDSNUMBER : return record->layergdsnum;
          case COLUMN_DATATYPE : return record->layerdatatype;
          case COLUMN_LAYERNOTE : return record->layernote;
        }
        return ""; //此处要有 return "";
      }

    case Qt::DecorationRole:
   {
    switch(nColumn){
      case COLUMN_LAYERGROUP: {
            unsigned int w = 32;
            unsigned int h = 16;
            QImage image (w, h, QImage::Format_RGB32);
            image.fill (m_background_color.rgb ());
            QPixmap pixmap = QPixmap::fromImage (image); // Qt 4.6.0 workaround
            return QVariant (QIcon (pixmap));
        }

     }
   }
      case Qt::CheckStateRole: //构层控制checkbox是否选中
      {
        switch (nColumn) {
          case COL_CHKBOX_LOCK : return record->layerlock ? Qt::Checked : Qt::Unchecked;
          case COL_CHKBOX_HIDE : return record->layerhide ? Qt::Checked : Qt::Unchecked;
          case COL_CHKBOX_PROT : return record->layerprot ? Qt::Checked : Qt::Unchecked;
          case COL_CHKBOX_FILL : return record->layerfill ? Qt::Checked : Qt::Unchecked;
        }
        //此处不要 return "";
      }

      default: return QVariant();
    }
    return QVariant();
}

/* 表头显示 */
QVariant LayTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
      case Qt::TextAlignmentRole://默认代理的对齐方式
        return QVariant(Qt::AlignHCenter | Qt::AlignVCenter);
      case Qt::DisplayRole://显示文字
      {
        if (orientation == Qt::Horizontal)
        {
          switch (section) {//每一列
            case COLUMN_LAYERNAME : return QStringLiteral("LAYER");
            case COLUMN_GDSNUMBER : return QStringLiteral("#");
            case COLUMN_DATATYPE  : return QStringLiteral("DT");
            case COLUMN_LAYERNOTE : return QStringLiteral("NOTE");
          }
        }
      }
      case Qt::DecorationRole://绘制装饰数据（通常是图标）
      {
        if (orientation == Qt::Horizontal)
        {
          switch (section) {
            //case COLUMN_LAYERGROUP : return QPixmap(layerrsrcPath + "/group.ico");     //Group & Ungroup
            case COL_CHKBOX_LOCK : return QPixmap(layerrsrcPath + "/lock.ico");        //LOCK(1) & UNLOCK(0)
            case COL_CHKBOX_HIDE : return QPixmap(layerrsrcPath + "/glasses.png");     //SHOW(1) & HIDE(0) (in ICED: UNBLANK & BLANK)
            case COL_CHKBOX_PROT : return QPixmap(layerrsrcPath + "/select.ico");      //PROT(1) & UNPROT(0) (ICED & L-Edit) (i.e., SELECTABLE(1)&UNSELECTABLE(0))
            case COL_CHKBOX_FILL : return QPixmap(layerrsrcPath + "/fill.ico");        //FILL(1) & UNFILL(0) (in ICED, not in L-Edit)
          }
        }
      }
      default:
        return QVariant();
    }
    return QVariant();
}

//在model中对数据的增删
bool LayTableModel::insertRows(int position, int count, const QModelIndex &parent )
{
    LayerRecordNode * parentItem = nodeFromIndex(parent);
       if (position > parentItem->children.count())
           return false;
       beginInsertRows(parent,position,position+count-1);
       bool result = true;
       for (;count;--count)
           parentItem->insertChild(position, new LayerRecordNode());
       endInsertRows();
       return result;
}
bool LayTableModel::removeRows(int position, int rows, const QModelIndex &parent )
{
    return false;
}
// 表格可选中、可复选

void LayTableModel::SetSceneData(CGeoScene *pScene)
{
    m_scene = pScene;
    setLayerList();
}
void LayTableModel::setLayerList()
{

    std::vector<ref_ptr<CGeoLayer> > layerList = m_scene->GetLayerList();
    for(std::vector<ref_ptr<CGeoLayer> >::iterator iterLayer = layerList.begin(); iterLayer != layerList.end(); ++iterLayer){
        ref_ptr<CGeoLayer> layer = *iterLayer;

        LayerRecordNode *node=new LayerRecordNode ;
        node->layername = QString::fromLocal8Bit(layer->GetName().c_str());
        node->layerlock = false;
        node->layerhide = false;
        node->layerprot = false;
        node->layerfill = false;
        node->layergdsnum =layer->GetLayerNo();
        node->layerdatatype = QString("TYPE ");
        node->layernote = QString("Note 1");
        node->parent=RootNode;
       // layer01.children
        RootNode->children.append(node);
    }
}

