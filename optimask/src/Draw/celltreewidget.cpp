#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

#include "celltreewidget.h"
#include "gdsdocument.h"
#include "./File/gds2reader.h"

const QString rsrcPath = "./Icon/";     //指定图标目录

CCellTreeWidget::CCellTreeWidget(QWidget *parent):QTreeWidget(parent)
{
    createActions();
    popMenu = new QMenu(this);
}
CCellTreeWidget::~CCellTreeWidget()
{
}

void CCellTreeWidget::contextMenuEvent ( QContextMenuEvent * event )
{
    QTreeWidgetItem *item = NULL;
    QPoint point = QCursor::pos();//得到鼠标的位置，全局坐标
    point = mapFromGlobal(point);//mapFromGlobal将全局坐标转化成窗口坐标
    item = this->itemAt(point);
//    if(item != NULL)
//    {
//        createMenus();
//    }
    createMenus();
   // createConnection();
    popMenu->exec(QCursor::pos());//菜单出现的位置为当前鼠标的位置
    event->accept();
    connect(dispMethAct, SIGNAL(triggered(QAction*)), this, SLOT(dispMethSlot(QAction*)));
    connect(sortingAct, SIGNAL(triggered(QAction*)), this, SLOT(sortingSlot(QAction*)));
    ConstructTreeWidget();

}

void CCellTreeWidget::createActions()
{
    cellFCL               = new QAction(tr("Flat Cell List"),this);
    cellFCL->setCheckable(true);
    cellFCL->setChecked(false);
    cellSM                 = new QAction(tr("Split Mode"),this);
    cellSM->setCheckable(true);
    cellSM->setChecked(false);
    cellSorting                                 = new QAction(tr("Sorting"),this);
    cellSortByNameBeginToEnd                        = new QAction(tr("By NameBeginToEnd"),this);
    cellSortByNameBeginToEnd->setCheckable(true);
    cellSortByNameBeginToEnd->setChecked(false);
    cellSortByNameEndToBegin                        = new QAction(tr("By NameEndToBegin"),this);
    cellSortByNameEndToBegin->setCheckable(true);
    cellSortByNameEndToBegin->setChecked(false);
    cellSortByTimeEarlyToLate                        = new QAction(tr("By TimeEarlyToLate"),this);
    cellSortByTimeEarlyToLate->setCheckable(true);
    cellSortByTimeEarlyToLate->setChecked(false);
    cellSortByTimeLateToEarly                        = new QAction(tr("By TimeLateToEarly"),this);
    cellSortByTimeLateToEarly->setCheckable(true);
    cellSortByTimeLateToEarly->setChecked(false);
    cellSortByAreaSmaToLarge     = new QAction(tr("By Area - Small To Large"),this);
    cellSortByAreaSmaToLarge->setCheckable(true);
    cellSortByAreaSmaToLarge->setChecked(false);
    cellSortByAreaLarToSmall      = new QAction(tr("By Area - Large To Small"),this);
    cellSortByAreaLarToSmall->setCheckable(true);
    cellSortByAreaLarToSmall->setChecked(false);
    cellNewCell             = new QAction(QIcon(rsrcPath+"/collapse.png"), tr("New Cell"),this);
    cellDeleteCell         = new QAction(tr("Delete Cell"),this);
    cellRenameCell      = new QAction(tr("Rename Cell"),this);
    cellReplaceCell       = new QAction(tr("Replace Cell"),this);
    cellHierarchyCell     = new QAction(tr("Hierarchy Cell"),this);
    cellHierarchyCell->setCheckable(true);
    cellHierarchyCell->setChecked(true);
    cellFlattenCell        = new QAction(tr("Flatten Cell"),this);
    cellFlattenCell->setCheckable(true);
    cellFlattenCell->setChecked(false);
    cellUserProperties = new QAction(tr("User Properties"),this);
    cellCopy                  = new QAction(tr("Copy"),this);
    cellCut                    = new QAction(tr("Cut"),this);
    cellPaste                 = new QAction(tr("Paste"),this);
    cellSANT                 = new QAction(tr("Show As New Top"),this);
    cellHide                  = new QAction(tr("Hide"),this);
    cellShow                 = new QAction(tr("Show"),this);
    cellShowAll            = new QAction(tr("Show All"),this);
    cellWhereAmI       = new QAction(tr("Where Am I"),this);
    cellSSCA                 = new QAction(tr("Save Selected Cells As"),this);

    dispMethAct = new QActionGroup(this);
    dispMethAct->addAction(cellHierarchyCell);
    dispMethAct->addAction(cellFlattenCell);

    sortingAct = new QActionGroup(this);
    sortingAct->addAction(cellSortByNameBeginToEnd);
    sortingAct->addAction(cellSortByNameEndToBegin);
    sortingAct->addAction(cellSortByTimeEarlyToLate);
    sortingAct->addAction(cellSortByTimeLateToEarly);

}

void CCellTreeWidget::populateMenu(QMenu *menu, QList<QAction *> actions)
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

void CCellTreeWidget::createMenus()
{
    QAction *separator = 0;
   // Sorting子菜单
    QMenu *cellSortingMenu = new QMenu(tr("Sorting"), this);
    foreach (QAction *action, QList<QAction*>()
            << cellSortByNameBeginToEnd<<cellSortByNameEndToBegin
           <<cellSortByTimeEarlyToLate<<cellSortByTimeLateToEarly
            << cellSortByAreaSmaToLarge << cellSortByAreaLarToSmall)
        cellSortingMenu->addAction(action);
    cellSorting->setMenu(cellSortingMenu);

   // cell Structure的上下文菜单
    populateMenu(popMenu, QList<QAction*>()
            << cellFCL  << cellSM << cellSorting << separator
            << cellNewCell << cellDeleteCell << cellRenameCell << cellReplaceCell
            <<cellHierarchyCell<< cellFlattenCell << cellUserProperties << separator << cellCopy << cellCut
            << cellPaste << separator << cellSANT << separator << cellHide << cellShow
            << cellShowAll << separator << cellWhereAmI << separator << cellSSCA);
}

void CCellTreeWidget::dispMethSlot(QAction *)
{
    QAction *pAct;
    pAct = new QAction(this);
    pAct->setCheckable(true);
    pAct->setChecked(true);
    if (pAct == cellHierarchyCell)
    {
        cellFlattenCell->setChecked(false);
    }
    else if (pAct == cellFlattenCell)
    {
        cellHierarchyCell->setChecked(false);       
    } 
}

void CCellTreeWidget::sortingSlot(QAction *)
{
    QAction *sAct;
    sAct = new QAction(this);
    sAct->setCheckable(true);
    sAct->setChecked(true);
    if(sAct ==cellSortByNameBeginToEnd)
    {
        cellSortByNameEndToBegin->setChecked(false);
        cellSortByTimeEarlyToLate->setChecked(false);
        cellSortByTimeLateToEarly->setChecked(false);
    }
    else if(sAct == cellSortByNameEndToBegin)
    {
        cellSortByNameBeginToEnd->setChecked(false);
        cellSortByTimeEarlyToLate->setChecked(false);
        cellSortByTimeLateToEarly->setChecked(false);
    }
    else if(sAct == cellSortByTimeEarlyToLate)
    {
        cellSortByNameBeginToEnd->setChecked(false);
        cellSortByNameEndToBegin->setChecked(false);
        cellSortByTimeLateToEarly->setChecked(false);
    }
    else if(sAct == cellSortByTimeLateToEarly)
    {
        cellSortByNameBeginToEnd->setChecked(false);
        cellSortByNameEndToBegin->setChecked(false);
        cellSortByTimeEarlyToLate->setChecked(false);

    }
}

void CCellTreeWidget::SetGdsDocument(CGdsDocument* doc)
{
   gdsdoc=doc;//保存指向gds文件的指针
}
CGdsDocument* CCellTreeWidget::GetGdsDocument()const
{
     return gdsdoc;
}
//QList<QString> CCellTreeWidget::GetCellList(CGdsDocument* doc)
//{
////   QList<QString> m_celllist;
//   m_celllist=doc->getCellInfo();//获取gds文件中构元信息
//   return m_celllist;
//}

void CCellTreeWidget::ConstructTreeWidget()
{
    ref_ptr<CGeoScene> pScene = gdsdoc->GetSceneData();
    const GeoStructList& lstStru = pScene->GetCellList();
    this->setSortingEnabled(false);
    this->setHeaderHidden(true);
    //无层次
    if(cellFlattenCell->isChecked())
    {
        //删除所有item
        while(topLevelItemCount()> 0)
        {delete topLevelItem(0);}
        //重建item
        QList<QTreeWidgetItem *> flatTreeItems; //构建构元树
        QTreeWidgetItem *flatItem;
        QStringList flatNameLists;
        //获取构元名字
        for (int i = 0; i < lstStru.size(); i++)
        {
            const CGeoGDSStruct* pCell = lstStru.at(i).get(); //获取构元
            flatNameLists<<QString::fromStdString(pCell->GetName());

        }
        //按字母排序
        if(cellSortByNameBeginToEnd->isChecked()|cellSortByNameEndToBegin->isChecked())
        {
            flatNameLists.sort(Qt::CaseInsensitive);
            for (int j = 0; j < lstStru.size(); j++)
            {
                flatItem=new QTreeWidgetItem(this);
                //从A到Z排序
                if(cellSortByNameBeginToEnd->isChecked())
                {
                    flatItem->setText(0,flatNameLists[j]);
                }
                //从Z到A排序
                else if(cellSortByNameEndToBegin->isChecked())
                {
                  flatItem->setText(0,flatNameLists[lstStru.size()-j-1]);
                }
                flatItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt:: ItemIsSelectable);
                flatTreeItems.push_back(flatItem);
            }

        }
        //按时间排序
        else if(cellSortByTimeEarlyToLate->isChecked()|cellSortByTimeLateToEarly->isChecked())
        {
           QList<time_t> modifylists;
           for(int i = 0; i < lstStru.size(); i++)
           {
               const CGeoGDSStruct* modifycell=lstStru.at(i).get();
               modifylists.push_back(modifycell->GetModifyTime());
           }

           for(int i = 0; i < lstStru.size(); i++)
           {
               for (int j = i; j <lstStru.size(); j++)
               {
                   if(modifylists[i]>modifylists[j])
                   {
                       time_t temp =modifylists[i];
                       modifylists[i] =modifylists[j];
                       modifylists[j] = temp;
                   }
               }
           }
           for(int i = 0; i < modifylists.length(); i++)
           {
               for(int j = 0; j < lstStru.size(); j++)
               {
                   //从早到晚排序
                   if(cellSortByTimeEarlyToLate->isChecked())
                   {
                       if(lstStru.at(j).get()->GetModifyTime()== modifylists[i])
                       {
                           flatItem=new QTreeWidgetItem(this);
                           flatItem->setText(0,flatNameLists[j]);
                           flatItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt:: ItemIsSelectable);
                           flatTreeItems.push_back(flatItem);
                       }
                   }
                   //从晚到早排序
                   else if(cellSortByTimeLateToEarly->isChecked())
                   {
                       if(lstStru.at(j).get()->GetModifyTime()== modifylists[lstStru.size()-i-1])
                       {
                           flatItem=new QTreeWidgetItem(this);
                           flatItem->setText(0,flatNameLists[j]);
                           flatItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt:: ItemIsSelectable);
                           flatTreeItems.push_back(flatItem);
                       }
                   }

               }

           }
        }
        //无要求时
        else
        {
            for (int i = 0; i < flatNameLists.length(); i++)
            {
                flatItem=new QTreeWidgetItem(this);
                flatItem->setText(0,flatNameLists[i]);
                flatItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt:: ItemIsSelectable);
                flatTreeItems.push_back(flatItem);
            }
        }
        this->addTopLevelItems(flatTreeItems);
    }

    else if(cellHierarchyCell->isChecked())
    {
       while(topLevelItemCount()> 0)
       {delete topLevelItem(0);}

       QList<QTreeWidgetItem *> masterTreeWidgetItems;
       for (int i = 0; i < lstStru.size(); i++)
       {
           const CGeoGDSStruct* pCell = lstStru.at(i).get(); //获取构元         
           if(pCell->GetRefCount()== 0)  //主构元
           {
               QTreeWidgetItem* masterItem = new QTreeWidgetItem(this);
               QString mastername = QString::fromStdString(pCell->GetName());
               masterItem->setText(0,mastername);
               masterItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt:: ItemIsSelectable);
               masterTreeWidgetItems.push_back(masterItem);
               GetChildTreeWidget(pCell,masterItem);
           }
       }

       this->addTopLevelItems(masterTreeWidgetItems);
    }


}

void CCellTreeWidget::GetChildTreeWidget(const CGeoGDSStruct *pCell, QTreeWidgetItem *masterItem)
{
    QTreeWidgetItem* childItem;
    QStringList childNameLists,arrayNameLists;
    QList<int>  timeLists,arrayTimeLists;

    for (int j = 0; j < pCell->GetChildCount();j++)
    {
        const CGeoBase* pChild = pCell->GetChild(j);
        const CGeoTransform* pTran = dynamic_cast<const CGeoTransform*>(pChild);
        if (pTran != 0)
        {
            const CGeoBase*  pChild =pTran->GetChild() ;

            switch(pChild->GetObjType())
            {
            case GEO_TYPE_ARRAY:
            {
                const CGeoArray* parray = dynamic_cast<const CGeoArray*>(pChild);
                for(int i=0; i<parray->GetChildCount();i++)
                {
                    pChild = parray->GetChild(i);
                    const CGeoGDSStruct* pChildStrct = dynamic_cast<const CGeoGDSStruct*>(pChild);
                    arrayNameLists<<QString::fromStdString(pChildStrct->GetName());
                }
//                if(cellSortByNameBeginToEnd->isChecked())
//                {
//                    arrayNameLists.sort(Qt::CaseInsensitive);
//                }
//                else if(cellSortByNameEndToBegin->isChecked())
//                {
//                    arrayNameLists.sort(Qt::CaseInsensitive);
//                    QStringList saveSortLists;
//                    for(int i=0;i<arrayNameLists.length();++i)
//                    {
//                        saveSortLists[i]=arrayNameLists[arrayNameLists.length()-i-1];
//                    }
//                    for(int i=0;i<saveSortLists.length();++i)
//                    {
//                        arrayNameLists[i]=saveSortLists[i];
//                    }
//                }
            }
                break;

            case GEO_TYPE_STRUCT:
            {
                const CGeoGDSStruct* pChildStrct = dynamic_cast<const CGeoGDSStruct*>(pChild);
                childNameLists<<QString::fromStdString(pChildStrct->GetName());

//                if(cellSortByNameBeginT\
//                    arrayNameLists.sort(Qt::CaseInsensitive);
//                    QStringList saveSortLists;
//                    for(int i=0;i<arrayNameLists.length();++i)
//                    {
//                        saveSortLists[i]=arrayNameLists[arrayNameLists.length()-i-1];
//                    }
//                    for(int i=0;i<saveSortLists.length();++i)
//                    {
//                        arrayNameLists[i]=saveSortLists[oEnd->isChecked())
                //                {
                //                    arrayNameLists.sort(Qt::CaseInsensitive);
                //                }
                //                else if(cellSortByNameEndToBegin->isChecked())
                //                {i];
//                    }
//                }

            }
             default:
                break;
            }

        }
    }
    if(childNameLists.length()>1)
    {
        for(int i = 0; i<childNameLists.length();++i)
        {
            int sameTime = 1;
            for(int j = i+1; j<childNameLists.length();++j)
            {
                if(childNameLists[j]==childNameLists[i])
                {
                   ++sameTime;
                   childNameLists.removeAt(j);
                   --j;
                }
            }
            timeLists<<sameTime;
        }

        for (int j = 0; j < pCell->GetChildCount();j++)
        {
            const CGeoBase* pChild = pCell->GetChild(j);
            const CGeoTransform* pTran = dynamic_cast<const CGeoTransform*>(pChild);
            if (pTran != 0)
            {
                const CGeoBase*  pChild = pTran->GetChild();

                switch(pChild->GetObjType())
                {

                case GEO_TYPE_STRUCT:
                {
                    const CGeoGDSStruct* pChildStrct = dynamic_cast<const CGeoGDSStruct*>(pChild);

                    if((childNameLists.length()>0)&&(pCell->GetChildCount()>childNameLists.length()))
                    {
                        int judgeSameTime=0;
                        for(int k=0;k<childNameLists.length();++k)
                        {
                            if(childNameLists[k]==QString::fromStdString(pChildStrct->GetName()))
                            {
                                ++judgeSameTime;
                                if(judgeSameTime<2)
                                {
                                     childItem = new QTreeWidgetItem(masterItem);
                                     QString childname;
                                     if(timeLists[k]==1)
                                     {
                                         childname=childNameLists[k];
                                     }
                                     else
                                     {
                                         childname=childNameLists[k]+" ( "+QString::number(timeLists[k])+" times )";
                                     }

                                     childItem->setText(0,childname);
                                     childItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt:: ItemIsSelectable);
                                     childNameLists.removeAt(k);
                                     timeLists.removeAt(k);
                                     const CGeoGDSStruct* pChildStrctTree = dynamic_cast<const CGeoGDSStruct*>(pChild);
                                     GetChildTreeWidget(pChildStrctTree,childItem);

                               }
                            }
                        }
                    }


                }
                    break;
                 default:
                    break;
                }

            }
        }
    }
    else
    {

        for (int j = 0; j < pCell->GetChildCount();j++)
        {
            const CGeoBase* pChild = pCell->GetChild(j);
            const CGeoTransform* pTran = dynamic_cast<const CGeoTransform*>(pChild);
            if (pTran != 0)
            {
                const CGeoBase*  pChild = pTran->GetChild();

                switch(pChild->GetObjType())
                {
                case GEO_TYPE_STRUCT:
                {
                    const CGeoGDSStruct* pChildStrct = dynamic_cast<const CGeoGDSStruct*>(pChild);
                    childItem = new QTreeWidgetItem(masterItem);
                    QString childname = QString::fromStdString(pChildStrct->GetName());
                    childItem->setText(0,childname);
                    childItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt:: ItemIsSelectable);

                    const CGeoGDSStruct* pChildStrctTree = dynamic_cast<const CGeoGDSStruct*>(pChild);
                    GetChildTreeWidget(pChildStrctTree,childItem);

                }
                    break;
                default:
                    break;
                }

            }
        }

    }

    if(arrayNameLists.length()>1)
    {
        for(int i = 0; i<arrayNameLists.length();++i)
        {
            int sameTime = 1;
            for(int j = i+1; j<arrayNameLists.length();++j)
            {
                if(arrayNameLists[j]==arrayNameLists[i])
                {
                   ++sameTime;
                   arrayNameLists.removeAt(j);
                   --j;
                }
            }
            arrayTimeLists<<sameTime;
        }

        for (int j = 0; j < pCell->GetChildCount();j++)
        {
            const CGeoBase* pChild = pCell->GetChild(j);
            const CGeoTransform* pTran = dynamic_cast<const CGeoTransform*>(pChild);
            if (pTran != 0)
            {
                const CGeoBase*  pChild = pTran->GetChild();

                switch(pChild->GetObjType())
                {
                case GEO_TYPE_ARRAY:
                {
                    const CGeoArray* parray = dynamic_cast<const CGeoArray*>(pChild);
                    for(int i=0; i<parray->GetChildCount();i++)
                    {
                        pChild = parray->GetChild(i);
                        const CGeoGDSStruct* pChildStrct = dynamic_cast<const CGeoGDSStruct*>(pChild);

                        if((childNameLists.length()>0)&&(pCell->GetChildCount()>childNameLists.length()))
                        {
                            int judgeArraySameTime=0;
                            for(int n=0;n<arrayNameLists.length();++n)
                            {
                                if(arrayNameLists[n]==QString::fromStdString(pChildStrct->GetName()))
                                {
                                    ++judgeArraySameTime;
                                    if(judgeArraySameTime<2)
                                    {
                                        childItem = new QTreeWidgetItem(masterItem);
                                        QString childname;
                                        if(timeLists[n]==1)
                                        {
                                            childname=childNameLists[n];
                                        }
                                        else
                                        {
                                            childname=childNameLists[n]+" ( "+QString::number(arrayTimeLists[n])+" times )";
                                        }

                                        childItem->setText(0,childname);
                                        childItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt:: ItemIsSelectable);
                                        arrayNameLists.removeAt(n);
                                        arrayTimeLists.removeAt(n);
                                        const CGeoGDSStruct* pChildStrctTree = dynamic_cast<const CGeoGDSStruct*>(pChild);
                                        GetChildTreeWidget(pChildStrctTree,childItem);

                                    }
                                }
                            }
                        }

                    }

                }
                    break;
                }
            }
        }

    }


    else
    {
        for (int j = 0; j < pCell->GetChildCount();j++)
        {
            const CGeoBase* pChild = pCell->GetChild(j);
            const CGeoTransform* pTran = dynamic_cast<const CGeoTransform*>(pChild);
            if (pTran != 0)
            {
                const CGeoBase*  pChild = pTran->GetChild();

                switch(pChild->GetObjType())
                {
                case GEO_TYPE_ARRAY:
                {
                    const CGeoArray* parray = dynamic_cast<const CGeoArray*>(pChild);
                    for(int i=0; i<parray->GetChildCount();i++)
                    {
                        pChild = parray->GetChild(i);
                        const CGeoGDSStruct* pChildStrct = dynamic_cast<const CGeoGDSStruct*>(pChild);
                        QTreeWidgetItem* childItem = new QTreeWidgetItem(masterItem);
                        QString childname = QString::fromStdString(pChildStrct->GetName());
                        childItem->setText(0,childname);
                        childItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt:: ItemIsSelectable);
                        const CGeoGDSStruct* pChildStrctTree = dynamic_cast<const CGeoGDSStruct*>(pChild);
                        GetChildTreeWidget(pChildStrctTree,childItem);

                    }
                }
                    break;
                 default:
                    break;
                }

            }
        }

    }

}
