
#define COLUMN_LAYERGROUP 0            //构层组群(Layer Group)或类别(Category)或排序(Order) (Ref.: K-Layout)
#define COLUMN_LAYERNAME  1            //构层名称(Layer Name)
#define COL_CHKBOX_LOCK   2            //是否锁定(LOCK=1, UNLOCK=0)
#define COL_CHKBOX_HIDE   3            //是否隐藏(HIDE=1=UNSHOW, UNHIDE=0=SHOW)
#define COL_CHKBOX_PROT   4            //是否保护(PROTect=1=Unselectable, UNPROT=0=Selectable)
#define COL_CHKBOX_FILL   5            //是否填充(FILL=1, UNFILL=0)
#define COLUMN_GDSNUMBER  6            //GDSII构层标号(Layer Number)
#define COLUMN_DATATYPE   7            //GDSII构层数据类型(Layer Data Type)
#define COLUMN_LAYERNOTE  8            //构层注释(Layer Note)

#include "laytablemodel.h"
#include <QPixmap>

const QString layerrsrcPath = "../Optimask/Icon";     //指定图标目录

LayTableModel::LayTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    init();
}

LayTableModel::~LayTableModel()
{

}

//CHECK!! 是否可以用矩阵和 FOR loop 形式？ 如何快速自动更新？
void LayTableModel::init()
{
    LayerRecord layer01;
    layer01.layergroup = QString("A");
    layer01.layername = QString("Chip");
    layer01.layerlock = false;
    layer01.layerhide = false;
    layer01.layerprot = false;
    layer01.layerfill = false;
    layer01.layergdsnum = QString("1");
    layer01.layerdatatype = QString(" ");
    layer01.layernote = QString("Note 1");
    LayersList.append(layer01);

    LayerRecord layer02;
    layer02.layergroup = QString("A");
    layer02.layername = QString("Mark");
    layer02.layerlock = false;
    layer02.layerhide = false;
    layer02.layerprot = false;
    layer02.layerfill = false;
    layer02.layergdsnum=QString("2");
    layer02.layerdatatype = QString(" ");
    layer02.layernote=QString("Note 2");
    LayersList.append(layer02);

    LayerRecord layer03;
    layer03.layergroup = QString("B");
    layer03.layername = QString("tile");
    layer03.layerlock = false;
    layer03.layerhide = false;
    layer03.layerprot = false;
    layer03.layerfill = false;
    layer03.layergdsnum=QString("3");
    layer03.layerdatatype = QString(" ");
    layer03.layernote=QString("Note 3");
    LayersList.append(layer03);

    LayerRecord layer04;
    layer04.layergroup = QString("C");
    layer04.layername = QString("Bar");
    layer04.layerlock = false;
    layer04.layerhide = false;
    layer04.layerprot = false;
    layer04.layerfill = false;
    layer04.layergdsnum=QString("255");
    layer04.layerdatatype = QString(" ");
    layer04.layernote=QString("Note 4");
    LayersList.append(layer04);
}

/* 更新表格数据 */
void LayTableModel::updateData(QList<LayerRecord> recordList)
{
    LayersList = recordList;
    beginResetModel();
    endResetModel();
}

/* 行数 */
int LayTableModel::rowCount(const QModelIndex &parent) const
{
    return LayersList.count();
}

/* 列数 */
int LayTableModel::columnCount(const QModelIndex &parent) const
{
    return 9;
}

//设置表格项数据
//主要是判断对checkbox的操作是选中，还是反选中。
//如果是选中则将该结点的index加入m_checkedList中，并发送dataChanged信号。
//反之则将该节点的index从m_checkedList中删除，也发送dataChanged信号。dataChanged信号会触发相应的槽函数，
//并且会调用到data()函数，这样会重新加载这个结点的状态
bool LayTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    int nColumn = index.column();
    LayerRecord record = LayersList.at(index.row());
    switch (role)
    {
      case Qt::DisplayRole:
      {
        if (nColumn == COLUMN_LAYERGROUP)
        {
            record.layergroup = value.toString();
            LayersList.replace(index.row(), record);
            emit dataChanged(index, index);
            return true;
        }
        if (nColumn == COLUMN_LAYERNAME)
        {
            record.layername = value.toString();
            LayersList.replace(index.row(), record);
            emit dataChanged(index, index);
            return true;
        }
        if (nColumn == COLUMN_GDSNUMBER)
        {
            record.layergdsnum = value.toString();
            LayersList.replace(index.row(), record);
            emit dataChanged(index, index);
            return true;
        }
        if (nColumn == COLUMN_DATATYPE)
        {
            record.layerdatatype = value.toString();
            LayersList.replace(index.row(), record);
            emit dataChanged(index, index);
            return true;
        }
        if (nColumn == COLUMN_LAYERNOTE)
        {
            record.layernote = value.toString();
            LayersList.replace(index.row(), record);
            emit dataChanged(index, index);
            return true;
        }
      }
      case Qt::CheckStateRole:
      {
        if (nColumn == COL_CHKBOX_LOCK)
        {
            record.layerlock = (value.toInt() == Qt::Checked);
            LayersList.replace(index.row(), record);
            emit dataChanged(index, index);
            return true;
        }
        if (nColumn == COL_CHKBOX_HIDE)
        {
            record.layerhide = (value.toInt() == Qt::Checked);
            LayersList.replace(index.row(), record);
            emit dataChanged(index, index);
            return true;
        }
        if (nColumn == COL_CHKBOX_PROT)
        {
            record.layerprot = (value.toInt() == Qt::Checked);
            LayersList.replace(index.row(), record);
            emit dataChanged(index, index);
            return true;
        }
        if (nColumn == COL_CHKBOX_FILL)
        {
            record.layerfill = (value.toInt() == Qt::Checked);
            LayersList.replace(index.row(), record);
            emit dataChanged(index, index);
            return true;
        }
      }
      default:
        return false;
    }
    return false;
}

// 表格项数据
QVariant LayTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int nRow = index.row();
    int nColumn = index.column();
    LayerRecord record = LayersList.at(nRow);

    switch (role)
    {
      case Qt::TextAlignmentRole:
          return QVariant(Qt::AlignHCenter | Qt::AlignVCenter);
      case Qt::DisplayRole: //构层属性
      {
        if (nColumn == COLUMN_LAYERGROUP) return record.layergroup;
        if (nColumn == COLUMN_LAYERNAME)  return record.layername;
        if (nColumn == COLUMN_GDSNUMBER)  return record.layergdsnum;
        if (nColumn == COLUMN_DATATYPE)   return record.layerdatatype;
        if (nColumn == COLUMN_LAYERNOTE)  return record.layernote;
        return "";
      }
      case Qt::CheckStateRole: //判断构层控制checkbox是否选中
      {
        if (nColumn == COL_CHKBOX_LOCK)
            return record.layerlock ? Qt::Checked : Qt::Unchecked;
        if (nColumn == COL_CHKBOX_HIDE)
            return record.layerhide ? Qt::Checked : Qt::Unchecked;
        if (nColumn == COL_CHKBOX_PROT)
            return record.layerprot ? Qt::Checked : Qt::Unchecked;
        if (nColumn == COL_CHKBOX_FILL)
            return record.layerfill ? Qt::Checked : Qt::Unchecked;
        //此处不要有 return "";
      }
      default:
        return QVariant();
    }
    return QVariant();
}

/* 表头显示 */
QVariant LayTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
      case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignHCenter | Qt::AlignVCenter);
      case Qt::DisplayRole:
      {
        if (orientation == Qt::Horizontal)
        {
            if (section == COLUMN_LAYERNAME)
                return QStringLiteral("LAYER");
            if (section == COLUMN_GDSNUMBER)
                return QStringLiteral("#");
            if (section == COLUMN_DATATYPE)
                return QStringLiteral("DT");
            if (section == COLUMN_LAYERNOTE)
                return QStringLiteral("NOTE");
        }
      }
      case Qt::DecorationRole:
      {
        if (orientation == Qt::Horizontal)
        {
            if (section == COLUMN_LAYERGROUP)
                return QPixmap(layerrsrcPath + "/group.ico");     //Group & Ungroup
            if (section == COL_CHKBOX_LOCK)
                return QPixmap(layerrsrcPath + "/lock.ico");      //LOCK(1) & UNLOCK(0)
            if (section == COL_CHKBOX_HIDE)
                return QPixmap(layerrsrcPath + "/glasses.png");   //SHOW(1) & HIDE(0) (in ICED: UNBLANK & BLANK)
            if (section == COL_CHKBOX_PROT)
                return QPixmap(layerrsrcPath + "/select.ico");    //PROT(1) & UNPROT(0) (ICED & L-Edit) (i.e., SELECTABLE(1)&UNSELECTABLE(0))
            if (section == COL_CHKBOX_FILL)
                return QPixmap(layerrsrcPath + "/fill.ico");      //FILL(1) & UNFILL(0) (in ICED, not in L-Edit)
        }
      }
      default:
        return QVariant();
    }
    return QVariant();
}

// 表格可选中、可复选
Qt::ItemFlags LayTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractItemModel::flags(index);

    //ItemIsUserCheckable 项目上是否有复选框
    //ItemIsEnabled 项目上是否没有被禁用（Enabled可用/Disabled禁用）
    //ItemIsSelectable 项目是否可以选中
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    //增加Qt :: ItemIsUserCheckable ，使其具备显示checkbox的能力
    if (index.column() == COL_CHKBOX_LOCK) flags |= Qt::ItemIsUserCheckable;
    if (index.column() == COL_CHKBOX_HIDE) flags |= Qt::ItemIsUserCheckable;
    if (index.column() == COL_CHKBOX_PROT) flags |= Qt::ItemIsUserCheckable;
    if (index.column() == COL_CHKBOX_FILL) flags |= Qt::ItemIsUserCheckable;
    return flags;
}
