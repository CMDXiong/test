#ifndef TABLEMODEL
#define TABLEMODEL

#endif // TABLEMODEL

#include <QAbstractTableModel>
class TableModel: public QAbstractTableModel
{
  public:
    typedef struct LayerRecord{
        QString layeritem;             //Layer Name
        bool layerlock;                //是否锁定(LOCK=1, UNLOCK=0)
        bool layerhide;                //是否隐藏(HIDE=1=UNSHOW, UNHIDE=0=SHOW)
        bool layerprot;                //是否保护(PROTect=1=Unselectable, UNPROT=0=Selectable)
        bool layerfill;                //是否填充(FILL=1, UNFILL=0)
        QString layergdsnum;           //GDSII Layer Number
        QString layerdatatype;         //GDSII Layer Data Type
        QString layergroup;            //GDSII Layers' Group
        QString layernote;             //Note for the Layer

    } fileRecord;
    explicit TableModel(QObject *parent=0);
    ~TableModel();
    void updateData(QList<LayerRecord> recordList);
    void init();
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    private:
    QList<LayerRecord> m_recordList;
};
