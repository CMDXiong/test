#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

#include "celltreewidget.h"
#include "./Draw/gdsdocument.h"
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
    cellSM                = new QAction(tr("Split Mode"),this);
    cellSM->setCheckable(true);
    cellSM->setChecked(false);
    cellMode              = new QAction(tr("Mode"),this);
    cellMode->setCheckable(false);
//    cellMode->setChecked(true);
    cellHierarchy     = new QAction(tr("Hierarchy"),this);
    cellHierarchy->setCheckable(true);
    cellHierarchy->setChecked(true);
    cellFlatten        = new QAction(tr("Flatten"),this);
    cellFlatten->setCheckable(true);
    cellFlatten->setChecked(false);
    cellSorting                                 = new QAction(tr("Sorting"),this);
    cellSortByNameBeginToEnd                        = new QAction(tr("By Name AtoZ"),this);
    cellSortByNameBeginToEnd->setCheckable(true);
    cellSortByNameBeginToEnd->setChecked(false);
    cellSortByNameEndToBegin                        = new QAction(tr("By Name ZtoA"),this);
    cellSortByNameEndToBegin->setCheckable(true);
    cellSortByNameEndToBegin->setChecked(false);
    cellSortByTimeEarlyToLate                        = new QAction(tr("By Time EtoL"),this);
    cellSortByTimeEarlyToLate->setCheckable(true);
    cellSortByTimeEarlyToLate->setChecked(false);
    cellSortByTimeLateToEarly                        = new QAction(tr("By Time LtoE"),this);
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
    dispMethAct->addAction(cellHierarchy);
    dispMethAct->addAction(cellFlatten);

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
    // Mode子菜单
     QMenu *cellModeMenu = new QMenu(tr("Mode"), this);
     foreach (QAction *action, QList<QAction*>()
             << cellHierarchy<<cellFlatten)
         cellModeMenu->addAction(action);
     cellMode->setMenu(cellModeMenu);
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
            << cellFCL  << cellSM <<cellMode<< cellSorting << separator
            << cellNewCell << cellDeleteCell << cellRenameCell << cellReplaceCell
            << cellFlattenCell << cellUserProperties << separator << cellCopy << cellCut
            << cellPaste << separator << cellSANT << separator << cellHide << cellShow
            << cellShowAll << separator << cellWhereAmI << separator << cellSSCA);
}

void CCellTreeWidget::dispMethSlot(QAction *)
{
    QAction *pAct;
    pAct = new QAction(this);
    pAct->setCheckable(true);
    pAct->setChecked(true);
    if (pAct == cellHierarchy)
    {
        cellFlatten->setChecked(false);
    }
    else if (pAct == cellFlatten)
    {
        cellHierarchy->setChecked(false);
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
    if(cellFlatten->isChecked())
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

    else if(cellHierarchy->isChecked())
    {
        //删除所有item
        while(topLevelItemCount()> 0)
        {delete topLevelItem(0);}
        //重建item
        QList<QTreeWidgetItem *> masterTreeWidgetItems;//构建构元树
        QTreeWidgetItem* masterItem;
        QStringList masterNameLists;
        //获取主构元名字
        for (int i = 0; i < lstStru.size(); i++)
        {
            const CGeoGDSStruct* pCell = lstStru.at(i).get(); //获取构元
            if(pCell->GetRefCount()== 0)  //主构元
            {
                masterNameLists<<QString::fromStdString(pCell->GetName());
            }
        }
        //按字母排序
        if(cellSortByNameBeginToEnd->isChecked()|cellSortByNameEndToBegin->isChecked())
        {
            masterNameLists.sort(Qt::CaseInsensitive);
            for(int j = 0; j < masterNameLists.length(); j++)
            {
                for (int i = 0; i < lstStru.size(); i++)
                {

                    const CGeoGDSStruct* pCell = lstStru.at(i).get(); //获取构元
                    if(pCell->GetRefCount()== 0)  //主构元
                    {
                        if(cellSortByNameBeginToEnd->isChecked())
                        {
                            if(QString::fromStdString(pCell->GetName())== masterNameLists[j])
                            {
                                masterItem=new QTreeWidgetItem(this);
                                masterItem->setText(0,masterNameLists[j]);
                                masterItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt:: ItemIsSelectable);
                                masterTreeWidgetItems.push_back(masterItem);
                                GetChildTreeWidget(pCell,masterItem);
                            }
                        }
                        //从晚到早排序
                        else if(cellSortByNameEndToBegin->isChecked())
                        {
                            if(QString::fromStdString(pCell->GetName())== masterNameLists[masterNameLists.length()-j-1])
                            {
                                masterItem=new QTreeWidgetItem(this);
                                masterItem->setText(0,masterNameLists[masterNameLists.length()-j-1]);
                                masterItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt:: ItemIsSelectable);
                                masterTreeWidgetItems.push_back(masterItem);
                                GetChildTreeWidget(pCell,masterItem);
                            }
                        }
                    }

                }
            }

        }
        else if(cellSortByTimeEarlyToLate->isChecked()|cellSortByTimeLateToEarly->isChecked())
        {
           QList<time_t> modifylists;
           for (int i = 0; i < lstStru.size(); i++)
           {
               const CGeoGDSStruct* pCell = lstStru.at(i).get(); //获取构元
               if(pCell->GetRefCount()== 0)  //主构元
               {
                   modifylists.push_back(pCell->GetModifyTime());
               }
           }

           for(int i = 0; i <  modifylists.length(); i++)
           {
               for (int j = i; j <modifylists.length(); j++)
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
               for (int m = 0; m < lstStru.size(); m++)
               {
                   const CGeoGDSStruct* pCell = lstStru.at(m).get(); //获取构元
                   if(pCell->GetRefCount()== 0)  //主构元
                   {
                       if(cellSortByTimeEarlyToLate->isChecked())
                       {
                           if(pCell->GetModifyTime()== modifylists[i])
                           {
                               masterItem=new QTreeWidgetItem(this);
                               masterItem->setText(0,QString::fromStdString(pCell->GetName()));
                               masterItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt:: ItemIsSelectable);
                               masterTreeWidgetItems.push_back(masterItem);
                               GetChildTreeWidget(pCell,masterItem);
                               m=lstStru.size()-1;
                           }
                       }
                       else if(cellSortByTimeLateToEarly->isChecked())
                       {
                         if(pCell->GetModifyTime()== modifylists[modifylists.length()-i-1])
                         {
                             masterItem=new QTreeWidgetItem(this);
                             masterItem->setText(0,QString::fromStdString(pCell->GetName()));
                             masterItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt:: ItemIsSelectable);
                             masterTreeWidgetItems.push_back(masterItem);
                             GetChildTreeWidget(pCell,masterItem);
                             m=lstStru.size()-1;
                         }
                       }

                   }
               }
           }
        }
        else
        {
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
         }
         this->addTopLevelItems(masterTreeWidgetItems);
        }
}

void CCellTreeWidget::GetChildTreeWidget(const CGeoGDSStruct *pCell, QTreeWidgetItem *masterItem)
{
    QTreeWidgetItem* childItem;
    QStringList childNameLists,arrayNameLists;
    QList<int>  timeLists,arrayTimeLists;

    QList<time_t> atimelists,stimelists;

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
                    atimelists.push_back(pChildStrct->GetModifyTime());
                }
            }
                break;

            case GEO_TYPE_STRUCT:
            {
                const CGeoGDSStruct* pChildStrct = dynamic_cast<const CGeoGDSStruct*>(pChild);
                childNameLists<<QString::fromStdString(pChildStrct->GetName());
                stimelists.push_back(pChildStrct->GetModifyTime());
            }
             default:
                break;
            }

        }
    }

    if(cellSortByTimeEarlyToLate->isChecked())
    {
        for(int i = 0; i <  atimelists.length(); i++)
        {
            for (int j = i; j <atimelists.length(); j++)
            {
                if(atimelists[i]>atimelists[j])
                {
                    time_t temp =atimelists[i];
                    atimelists[i] =atimelists[j];
                    atimelists[j] = temp;
                    QString tempStr =arrayNameLists[i];
                    arrayNameLists[i] =arrayNameLists[j];
                    arrayNameLists[j] = tempStr;

                }
            }
        }
        for(int i = 0; i <  stimelists.length(); i++)
        {
            for (int j = i; j <stimelists.length(); j++)
            {
                if(stimelists[i]>stimelists[j])
                {
                    time_t temp =stimelists[i];
                    stimelists[i] =stimelists[j];
                    stimelists[j] = temp;
                    QString tempStr =childNameLists[i];
                    childNameLists[i] =childNameLists[j];
                    childNameLists[j] = tempStr;
                }
            }
        }
    }
    else if(cellSortByTimeLateToEarly->isChecked())
    {
        for(int i = 0; i <  atimelists.length(); i++)
        {
            for (int j = i; j <atimelists.length(); j++)
            {
                if(atimelists[i]<atimelists[j])
                {
                    time_t temp =atimelists[i];
                    atimelists[i] =atimelists[j];
                    atimelists[j] = temp;
                    QString tempStr =arrayNameLists[i];
                    arrayNameLists[i] =arrayNameLists[j];
                    arrayNameLists[j] = tempStr;

                }
            }
        }
        for(int i = 0; i <  stimelists.length(); i++)
        {
            for (int j = i; j <stimelists.length(); j++)
            {
                if(stimelists[i]<stimelists[j])
                {
                    time_t temp =stimelists[i];
                    stimelists[i] =stimelists[j];
                    stimelists[j] = temp;
                    QString tempStr =childNameLists[i];
                    childNameLists[i] =childNameLists[j];
                    childNameLists[j] = tempStr;
                }
            }
        }
    }
    else if(cellSortByNameBeginToEnd->isChecked())
    {
        childNameLists.sort(Qt::CaseInsensitive);
        arrayNameLists.sort(Qt::CaseInsensitive);
    }
    else if(cellSortByNameEndToBegin->isChecked())
    {
        childNameLists.sort(Qt::CaseInsensitive);
        arrayNameLists.sort(Qt::CaseInsensitive);
        for(int n=0;n<childNameLists.length();++n)
        {
            for(int m=0;m<childNameLists.length()-n-1;++m)
            {
                QString saveStr=childNameLists[m+1];
                childNameLists[m+1]=childNameLists[m];
                childNameLists[m]=saveStr;
            }
        }
        for(int n=0;n<arrayNameLists.length();++n)
        {
            for(int m=0;m<arrayNameLists.length()-n-1;++m)
            {
                QString saveStr=arrayNameLists[m+1];
                arrayNameLists[m+1]=arrayNameLists[m];
                arrayNameLists[m]=saveStr;
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
        for(int i = 0; i<childNameLists.length();++i)
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
                        if(childNameLists[i]==QString::fromStdString(pChildStrct->GetName()))
                        {
                            childItem = new QTreeWidgetItem(masterItem);
                            QString childname;
                            if(timeLists[i]==1)
                            {
                                childname=childNameLists[i];
                            }
                            else
                            {
                                childname=childNameLists[i]+" ( "+QString::number(timeLists[i])+" times )";
                            }

                            childItem->setText(0,childname);
                            childItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt:: ItemIsSelectable);
                            const CGeoGDSStruct* pChildStrctTree = dynamic_cast<const CGeoGDSStruct*>(pChild);
                            GetChildTreeWidget(pChildStrctTree,childItem);
                            j=pCell->GetChildCount()-1;

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
        for(int i =0;i<arrayNameLists.length();++i)
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
                            if(arrayNameLists[i]==QString::fromStdString(pChildStrct->GetName()))
                            {
                                childItem = new QTreeWidgetItem(masterItem);
                                QString childname;
                                if(arrayTimeLists[i]==1)
                                {
                                    childname=arrayNameLists[i];
                                }
                                else
                                {
                                    childname=arrayNameLists[i]+" ( "+QString::number(arrayTimeLists[i])+" times )";
                                }

                                childItem->setText(0,childname);
                                childItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt:: ItemIsSelectable);
                                const CGeoGDSStruct* pChildStrctTree = dynamic_cast<const CGeoGDSStruct*>(pChild);
                                GetChildTreeWidget(pChildStrctTree,childItem);
                                j=pCell->GetChildCount()-1;
                            }
                        }

                    }
                        break;
                    }
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
