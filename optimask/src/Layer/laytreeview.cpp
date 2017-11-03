#include"laytreeview.h"
LayTableView::LayTableView()
{
    tableModel=new LayTableModel;
    this->setModel(tableModel);
    this->setSortingEnabled(true);

    this->setSelectionBehavior(QAbstractItemView::SelectRows);  //选择行
    this->setSelectionMode(QAbstractItemView::SingleSelection); //单选

    //pTableView->resizeColumnToContents(0);//根据内容自动调整第0列的列宽
    //pTableView->resizeColumnToContents(1);//根据内容自动调整第1列的列宽
    //this->resizeColumnsToContents(); //根据内容自动调整所有列的列宽
   // this->horizontalHeader()->setHighlightSections(false); //被选部分是否高亮显示（突出），默认是false
   // this->verticalHeader()->setVisible(false);//纵向表头隐藏
   // this->horizontalHeader()->setStretchLastSection(false);
    //this->setShowGrid(false);//不显示网格
    this->setFrameShape(QFrame::Panel);//panel样式
    this->showNormal();

    this->setColumnWidth(0,40);  //设置第0列的宽度
    this->setColumnWidth(1,60);  //设置第1列的宽度
    this->setColumnWidth(2,20);  //设置第2列的宽度
    this->setColumnWidth(3,20);  //设置第3列的宽度
    this->setColumnWidth(4,20);  //设置第4列的宽度
    this->setColumnWidth(5,20);  //设置第5列的宽度
    this->setColumnWidth(6,40);  //设置第6列的宽度
    this->setColumnWidth(7,40);  //设置第7列的宽度
    this->setColumnWidth(8,40);  //设置第8列的宽度

    //自定义菜单,响应customContextMenuRequested()这个signal，在响应的槽中显示菜单(QMenu的exec()方法)
     this->setContextMenuPolicy (Qt::CustomContextMenu);
     connect (this, SIGNAL(customContextMenuRequested (const QPoint &)), this, SLOT (context_menu (const QPoint &)));
    // this->header()->hide ();//隐藏头部
     this->setSelectionMode (QTreeView::ExtendedSelection);//实现多行选择
     this->setRootIsDecorated (false);//隐藏根节点前面的小箭头
     this->setIconSize (QSize (32, 16));//设置图标

     m_menu=new QMenu(this);
     createActions();
     createMenu();
     connect(group,SIGNAL(triggered()),this,SLOT(cm_group()));

}
void LayTableView::SetGdsDocument(CGdsDocument* doc)
{
   gdsdoc=doc;//保存指向gds文件的指针
}
CGdsDocument* LayTableView::GetGdsDocument()const
{
    return gdsdoc;
}

void LayTableView::SetModeData()
{
    tableModel->SetSceneData(gdsdoc->GetSceneData().get());
}

void LayTableView::createActions(){
    select_all=new QAction(tr("Select All"),this);

    tab_menu=new QAction(tr("Tabs"),this);
    new_tab=new QAction(tr("New Tab"),this);
    remove_tab=new QAction(tr("Remove Tab"),this);
    rename_tab=new QAction(tr("Rename Tab"),this);

    hide=new QAction(tr("Hide"),this);
    hide_all=new QAction(tr("Hide All"),this);
    show=new QAction(tr("Show"),this);
    show_all=new QAction(tr("Show All"),this);
    show_only=new QAction(tr("Show Only Selected"),this);
    valid=new QAction(tr("Make Valid"),this);
    invvalid=new QAction(tr("Make Invalid"),this);
    rename=new QAction(tr("Rename"),this);

    hide_empty_layers=new QAction(tr("Hide Empty Layers"),this);
    test_shapes_in_view=new QAction(tr("Test For Shapes In View"),this);

    select_source=new QAction(tr("Select Source"),this);

    sort_menu=new QAction(tr("Sort By"),this);
    sort_ild=new QAction(tr("Layout Index, Layer And Datatype"),this);
    sort_idl=new QAction(tr("Layout Index, Datatype And Layer"),this);
    sort_ldi=new QAction(tr("Layer, Datatype And Layout Index"),this);
    sort_dli=new QAction(tr("Datatype, Layer And Layout Index"),this);
    sort_name=new QAction(tr("Name"),this);

    del=new QAction(tr("Delete Layer Entry"),this);
    insert=new QAction(tr("Insert Layer Entry"),this);
    add_others=new QAction(tr("Add Other Layer Entries"),this);
    clean_up=new QAction(tr("Clean Up Layer Entries"),this);

    group=new QAction(tr("Group"),this);
    ungroup=new QAction(tr("Ungroup"),this);

    regroup_menu=new QAction(tr("Regroup Layer Entries"),this);
    grp_i=new QAction(tr("By Layout Index"),this);
    grp_d=new QAction(tr("By Datatype"),this);
    grp_l=new QAction(tr("By Layer"),this);
    flatten=new QAction(tr("Flatten"),this);

    copy=new QAction(tr("Copy"),this);
    cut=new QAction(tr("Cut"),this);
    paste=new QAction(tr("Paste"),this);
}
void LayTableView::populateMenu(QMenu *menu, QList<QAction *> actions)
{
    foreach (QAction *action, actions) {
        if (!action) {
            menu->addSeparator();
        }
        else {
            menu->addAction(action);
        }
    }
}
void LayTableView::createMenu(){
    QAction *separator = 0;


    // Tabs子菜单
     QMenu *tabMenu = new QMenu(tr("Tabs"), this);
     foreach (QAction *action, QList<QAction*>()<< new_tab<<remove_tab<<rename_tab)
         tabMenu->addAction(action);
     tab_menu->setMenu(tabMenu);
     // sort_menu子菜单
      QMenu *sortMenu = new QMenu(tr("Tabs"), this);
      foreach (QAction *action, QList<QAction*>()<< sort_ild<<sort_idl<<sort_ldi<<sort_dli<<sort_name)
          sortMenu->addAction(action);
      sort_menu->setMenu(sortMenu);
      // regroup_menu子菜单
       QMenu *regroupMenu = new QMenu(tr("Tabs"), this);
       foreach (QAction *action, QList<QAction*>()<< grp_i<<grp_d<<grp_l<<flatten)
           regroupMenu->addAction(action);
       regroup_menu->setMenu(regroupMenu);

   // cell Structure的上下文菜单
    populateMenu(m_menu, QList<QAction*>()
            << select_all  << separator<<tab_menu <<separator<< hide << hide_all<<show<<show_all<<show_only
            << valid << invvalid << rename << separator<<hide_empty_layers<<test_shapes_in_view<<separator
            <<select_source<< separator << sort_menu << separator << del << insert<<add_others<<clean_up
            << separator << group << ungroup << separator << regroup_menu << separator
            << copy << cut << paste);


}
void LayTableView::context_menu(const QPoint &pt){
    m_menu->exec(this->mapToGlobal(pt));
}

//获取选择的层的信息
// std::vector<LayerRecordNode*>
// LayTableView::selected_layers () const
// {
//     QModelIndexList selected = this->selectionModel ()->selectedIndexes ();
//     //某一个视图的选择模型可以通过selectionModel()函数获取,获取选区selectedIndexes
//     std::vector <LayerRecordNode*> llist;
//     llist.reserve (selected.size ());// reserve只是预留空间，新元素还没有构造, 此时不能用[]访问元素

//     for (QModelIndexList::const_iterator i = selected.begin (); i != selected.end (); ++i) {

//           LayerRecordNode *node=tableModel->nodeFromIndex(*i);
//           llist.push_back (node);
//     }
//     return llist;

// }


 //分组group功能
void LayTableView::cm_group(){
  // std::vector<LayerRecordNode*> sel = selected_layers ();//将selected_layers存入容器
   QModelIndexList selected = this->selectionModel ()->selectedIndexes ();
   QModelIndex parent1=selected[0].parent();
   int positon=selected[0].row();
   int rows=selected.count();
  tableModel->insertRows(positon,rows,parent1);


}
