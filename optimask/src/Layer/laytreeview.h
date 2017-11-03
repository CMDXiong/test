#ifndef LAYTABLEVIEW
#define LAYTABLEVIEW

#include <QAction>
#include <QWidget>
#include <QMenu>
#include <QTreeView>
#include <QHeaderView>
#include"laytreemodel.h"
#include"./Draw/gdsdocument.h"
class CGdsDocument;

class LayTableView:public QTreeView
{
    Q_OBJECT
public:
  explicit LayTableView();
  void SetGdsDocument(CGdsDocument* doc);
  CGdsDocument* GetGdsDocument()const;
  //void ​sortByColumn(int column);//按列排序
  void SetModeData();//设置对应model所需要的数据
  void populateMenu(QMenu *menu, QList<QAction *> actions);
  void createMenu();
  void createActions();

 // std::vector<LayTableModel::LayerRecord> selected_layers () const;

public slots:
  void context_menu (const QPoint &pt);//显示上下文菜单
  void cm_group();//group
private:
  LayTableModel *tableModel;//声明一个model对象
  CGdsDocument *gdsdoc;
  QMenu* m_menu;//上下文菜单

  QAction* select_all;

  QAction*tab_menu;
  QAction* new_tab;
  QAction* remove_tab;
  QAction* rename_tab;

  QAction* hide;
  QAction* hide_all;
  QAction* show;
  QAction* show_all;
  QAction* show_only;
  QAction* valid;
  QAction* invvalid;
  QAction* rename;

  QAction* hide_empty_layers;
  QAction* test_shapes_in_view;

  QAction* select_source;

  QAction* sort_menu;
  QAction* sort_ild;
  QAction* sort_idl;
  QAction* sort_ldi;
  QAction* sort_dli;
  QAction* sort_name;

  QAction* del;
  QAction* insert;
  QAction* add_others;
  QAction* clean_up;

  QAction* group;
  QAction* ungroup;

  QAction* regroup_menu;
  QAction* grp_i;
  QAction* grp_d;
  QAction* grp_l;
  QAction* flatten;

  QAction* copy;
  QAction* cut;
  QAction* paste;


};


#endif // LAYTABLEVIEW

