//#ifndef TABLEMODEL
//  #define TABLEMODEL
//#endif // TABLEMODEL

#include <QAbstractTableModel>
class LayTableModel : public QAbstractTableModel
{
  public:
    typedef struct LayerRecord{
        QString layergroup;            //构层组群(Layer Group)或类别(Category)或排序(Order) (Ref.: K-Layout)
        QString layername;             //构层名称(Layer Name)
        bool layerlock;                //构层锁定(LOCK=1, UNLOCK=0)
        bool layerhide;                //构层隐藏(HIDE=1=UNSHOW, UNHIDE=0=SHOW)
        bool layerprot;                //构层保护(PROTect=1=Unselectable, UNPROT=0=Selectable)
        bool layerfill;                //构层填充(FILL=1, UNFILL=0)
        QString layergdsnum;           //构层GDSII标号(Layer Number)
        QString layerdatatype;         //构层GDSII数据类型(Layer Data Type)
        QString layernote;             //构层注释(Layer Note)
    } laystruct;

    explicit LayTableModel(QObject *parent=0);
    ~LayTableModel();
    void updateData(QList<LayerRecord> recordList);                                    //更新数据
    void init();                                                                       //初始化
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);           //设置构层数据
    QVariant data(const QModelIndex &index, int role) const;                           //构层数据
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;     //构层面板表头
    Qt::ItemFlags flags(const QModelIndex &index) const;

  private:
    QList<LayerRecord> LayersList;     //构层清单
};
