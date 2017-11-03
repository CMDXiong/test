
#define COLUMN_LAYERNAME  0
#define COL_CHKBOX_LOCK 1
#define COL_CHKBOX_HIDE 2
#define COL_CHKBOX_PROT 3
#define COL_CHKBOX_FILL 4
#define COLUMN_GDSNUMBER  5
#define COLUMN_LAYERNOTE  6

#include "tablemodel.h"
#include <QPixmap>

const QString layerrsrcPath = "../Optimask/Icon";     //指定图标目录

TableModel::TableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    init();
}

TableModel::~TableModel()
{

}

void TableModel::init()
{

        LayerRecord record1;
        record1.layeritem = QString("GCS PCM");
        record1.layerlock = false;
        record1.layerhide = false;
        record1.layerprot = false;
        record1.layerfill = false;
        record1.layergdsnum=QString("1");
        record1.layernote=QString("Layer 1");
        m_recordList.append(record1);

        LayerRecord record2;
        record2.layerlock = false;
        record2.layerhide = false;
        record2.layerprot = false;
        record2.layerfill = false;
        record2.layeritem = QString("SA");
        record2.layergdsnum=QString("2");
        record2.layernote=QString("Layer 2");
        m_recordList.append(record2);

        LayerRecord record3;
        record3.layerlock = false;
        record3.layerhide = false;
        record3.layerprot = false;
        record3.layerfill = false;
        record3.layeritem = QString("tile");
        record3.layernote=QString("Layer 3");
        record3.layergdsnum=QString("3");
        m_recordList.append(record3);

        LayerRecord record4;
        record4.layerlock = false;
        record4.layerhide = false;
        record4.layerprot = false;
        record4.layerfill = false;
        record4.layeritem = QString("Bar");
        record4.layergdsnum=QString("4");
        record4.layernote=QString("Layer 4");
        m_recordList.append(record4);

        LayerRecord record5;
        record5.layerlock = false;
        record5.layerhide = false;
        record5.layerprot = false;
        record5.layerfill = false;
        record5.layergdsnum=QString("5");
        record5.layernote=QString("Layer 5");
        record5.layeritem = QString("100tile");
        m_recordList.append(record5);
}

// 更新表格数据
void TableModel::updateData(QList<LayerRecord> recordList)
{
    m_recordList = recordList;
    beginResetModel();
    endResetModel();
}

// 行数
int TableModel::rowCount(const QModelIndex &parent) const
{
    return m_recordList.count();
}

// 列数
int TableModel::columnCount(const QModelIndex &parent) const
{
    return 7;
}

// 设置表格项数据
//主要是判断对checkbox的操作是选中，还是反选中。
//如果是选中则将该结点的index加入m_checkedList中，并发送dataChanged信号。
//反之则将该节点的index从m_checkedList中删除，也发送dataChanged信号。dataChanged信号会触发相应的槽函数，
//并且会调用到data()函数，这样会重新加载这个结点的状态
bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    int nColumn = index.column();
    LayerRecord record = m_recordList.at(index.row());
    switch (role)
    {


    case Qt::DisplayRole:
    {
        if (nColumn == COLUMN_LAYERNAME)
        {
            record.layeritem = value.toString();
            m_recordList.replace(index.row(), record);
            emit dataChanged(index, index);
            return true;
        }
        if (nColumn == COLUMN_GDSNUMBER)
        {
            record.layergdsnum = value.toString();
            m_recordList.replace(index.row(), record);
            emit dataChanged(index, index);
            return true;
        }
    }
    case Qt::CheckStateRole:
    {
        if (nColumn == COL_CHKBOX_LOCK)
        {
            record.layerlock = (value.toInt() == Qt::Checked);
            m_recordList.replace(index.row(), record);
           // emit dataChanged(index, index);
           // return true;
        }
        if (nColumn == COL_CHKBOX_HIDE)
        {
            record.layerhide = (value.toInt() == Qt::Checked);
            m_recordList.replace(index.row(), record);
           // emit dataChanged(index, index);
           // return true;
        }
        if (nColumn == COL_CHKBOX_PROT)
        {
            record.layerprot = (value.toInt() == Qt::Checked);
            m_recordList.replace(index.row(), record);
            emit dataChanged(index, index);
            return true;
        }
        if (nColumn == COL_CHKBOX_FILL)
        {
            record.layerfill = (value.toInt() == Qt::Checked);
            m_recordList.replace(index.row(), record);
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
QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int nRow = index.row();
    int nColumn = index.column();
    LayerRecord record = m_recordList.at(nRow);

    switch (role)
    {
    case Qt::DisplayRole:
    {
        if (nColumn == COLUMN_LAYERNAME)
            return record.layeritem;
        if (nColumn == COLUMN_GDSNUMBER)
            return record.layergdsnum;
        if (nColumn == COLUMN_LAYERNOTE)
            return record.layernote;

        return "";
    }
    case Qt::CheckStateRole://判断显示的对象是checkbox，并且位于第一列
    {
        if (nColumn == COL_CHKBOX_LOCK)
            return record.layerlock ? Qt::Checked : Qt::Unchecked;
        if (nColumn == COL_CHKBOX_HIDE)
            return record.layerhide ? Qt::Checked : Qt::Unchecked;
        if (nColumn == COL_CHKBOX_PROT)
            return record.layerprot ? Qt::Checked : Qt::Unchecked;
        if (nColumn == COL_CHKBOX_FILL)
            return record.layerfill ? Qt::Checked : Qt::Unchecked;
           // return "";
        //在record(m_checkedList)中查找，如果有，显示checkbox被选中
    }
    default:
        return QVariant();
    }

    return QVariant();
}

// 表头数据
QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    case Qt::DisplayRole:
    {
        if (orientation == Qt::Horizontal)
        {
            if (section == COLUMN_LAYERNAME)
                 return QStringLiteral("LAYER");   //Layer Name
                 //return QIcon(rsrcPath+"/layers.png"); //Layer Name
//            if (section == COL_CHKBOX_LOCK)
//                return QStringLiteral("LOCK");      //LOCK(1) & UNLOCK(0)
//            if (section == COL_CHKBOX_HIDE)
//               return QStringLiteral("SHOW");      //SHOW(1) & HIDE(0) (in ICED: UNBLANK & BLANK)
//            if (section == COL_CHKBOX_PROT)
//                return QStringLiteral("PROT");      //PROT(1) & UNPROT(0) (ICED & L-Edit) (i.e., SELECTABLE(1)&UNSELECTABLE(0))
//            if (section == COL_CHKBOX_FILL)
//               return QStringLiteral("FILL");      //FILL(1) & UNFILL(0) (in ICED, not in L-Edit)
            if (section == COLUMN_GDSNUMBER)
               return QStringLiteral("GDSII#");      //GDSII Layer Number
            if (section == COLUMN_LAYERNOTE)
               return QStringLiteral("NOTE");      //NOTE(COMMENT) for the Layer
        }

    }
    case Qt::DecorationRole:
    {
        if (orientation == Qt::Horizontal)
         {
             if (section == COL_CHKBOX_LOCK)
                 return QPixmap(layerrsrcPath + "/Lock.ico");      //LOCK(1) & UNLOCK(0)
             if (section == COL_CHKBOX_HIDE)
                 return QPixmap(layerrsrcPath + "/glasses.png");     //SHOW(1) & HIDE(0) (in ICED: UNBLANK & BLANK)
             if (section == COL_CHKBOX_PROT)
                 return QPixmap(layerrsrcPath + "/Select.ico");      //PROT(1) & UNPROT(0) (ICED & L-Edit) (i.e., SELECTABLE(1)&UNSELECTABLE(0))
             if (section == COL_CHKBOX_FILL)
                 return QPixmap(layerrsrcPath + "/fill.ico");     //FILL(1) & UNFILL(0) (in ICED, not in L-Edit)
         }


    }

    default:
        return QVariant();
    }

    return QVariant();
}

// 表格可选中、可复选
Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractItemModel::flags(index);
    //ItemIsUserCheckable 项目上是否有复选框
    //ItemIsEnabled 项目上是否没有被禁用（Enabled可用/Disabled禁用）
    //ItemIsSelectable 项目是否可以选中
        Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == COL_CHKBOX_LOCK)//如果是第一列的结点，则使其有显示checkbox的能力
        flags |= Qt::ItemIsUserCheckable;//则增加Qt :: ItemIsUserCheckable ，使其具备显示checkbox的能力
    if (index.column() == COL_CHKBOX_HIDE)//如果是第一列的结点，则使其有显示checkbox的能力
        flags |= Qt::ItemIsUserCheckable;//则增加Qt :: ItemIsUserCheckable ，使其具备显示checkbox的能力
    if (index.column() == COL_CHKBOX_PROT)//如果是第一列的结点，则使其有显示checkbox的能力
        flags |= Qt::ItemIsUserCheckable;//则增加Qt :: ItemIsUserCheckable ，使其具备显示checkbox的能力
    if (index.column() == COL_CHKBOX_FILL)//如果是第一列的结点，则使其有显示checkbox的能力
        flags |= Qt::ItemIsUserCheckable;//则增加Qt :: ItemIsUserCheckable ，使其具备显示checkbox的能力
        return flags;

}
