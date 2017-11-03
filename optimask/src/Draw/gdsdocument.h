#ifndef GDSDOCUMENT
#define GDSDOCUMENT
#include <QWidget>
#include"../base/Geo.h"             //added by Bamboo 2017/08/05

class CGDS;
class CGDSStructure;
class CCellTreeWidget;
class COptimaskView;
class LayTableView;
class Navigator;

class CGdsDocument
{
public:
     CGdsDocument();                                                 // 构造函数
     ~CGdsDocument();                                                // 析构函数
     void               LoadDocument(QString filename);              // 打开
     void               SaveFile();                                  // 保存
     //返回gds文件中单个构元Cell的相关信息
     void               SetDocumentName(const QString &fileName);    // 设置文件名
     QString            DocumentName() const;                        // 返回文件名
     void               SetViewWid(COptimaskView* gdsview);          // 保存gdsview对象指针
     COptimaskView*     GetViewWid() const;
     void               SetCellTreeWid(CCellTreeWidget* celltree);   // 保存构元窗口对象指针
     CCellTreeWidget*   GetCellTreeWid() const;
     void               SetLayTableViewWid(LayTableView* layerview);
     LayTableView*      GetTableView() const;
     void               SaveCellViewWid(COptimaskView* cellview);    // 保存cellview对象指针
     void               DeleteCellViewWid(COptimaskView* cellview);
     void               SetNavigator(Navigator* navigator);
     Navigator*         GetNavigator() const;
     ref_ptr<CGeoScene> GetSceneData() const;

private:
     QString            m_documentName;                              // 每个文件有一个名字
     CCellTreeWidget*   m_cellTreeWidget;
     COptimaskView*     m_gdsView;
     LayTableView*      m_tableView;
     QList<COptimaskView*> m_listCellView;
     Navigator*         m_navigator;                                 // 保存新建的celltreewidget和view的指针
     ref_ptr<CGeoScene> m_scene;                                     // 保存一个文件的场景数据
};

#endif // GDSDOCUMENT

