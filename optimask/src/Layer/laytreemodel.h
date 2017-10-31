/* 构层面板表格模式 */

#ifndef LAYTREEMODEL
#define LAYTREEMODEL

#include "./base/Geo.h"
#include "laytreenode.h"
#include <QColor>
#include <QIcon>
#include <QAbstractTableModel>

class LayTableModel : public QAbstractItemModel
{
    Q_OBJECT
  public:

    explicit LayTableModel(QObject *parent=0);
    ~LayTableModel();

   //QAbstractItemModel必须实现的5个标准API
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;                           //构层数据
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;     //构层面板表头
    QModelIndex index ( int row, int column, const QModelIndex & parent  = QModelIndex() ) const;
    QModelIndex parent ( const QModelIndex & index ) const;

    //在model中对数据的增删
    bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex());
    //从gds读取底层数据
    void SetSceneData(CGeoScene* pScene);         // 数据来源
    void setLayerList();                          // 每层的信息设置

  private:
    ref_ptr<CGeoScene> m_scene;         //当前文件数据
    LayerRecordNode *RootNode;          //构建底层数据根结点
    LayerRecordNode *nodeFromIndex(const QModelIndex &index) const;//返回索引对应的节点值
    QColor m_background_color/*,m_text_color*/;

};
#endif // LAYTREEMODEL

