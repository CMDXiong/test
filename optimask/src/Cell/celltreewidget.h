#ifndef CELLTREEWIDGET_H
#define CELLTREEWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QDebug>
#include <QActionGroup>

class CGdsDocument;//类的声明
class CGeoBase;
class QTreeWidgetItem;
class CGeoGDSStruct;

class CCellTreeWidget:public QTreeWidget
{
    Q_OBJECT
protected:
    void contextMenuEvent(QContextMenuEvent *event);
public:
    CCellTreeWidget(QWidget *parent = 0);
    ~CCellTreeWidget();
    void           SetGdsDocument(CGdsDocument* doc);
    CGdsDocument*  GetGdsDocument()const;
    QList<QString> GetCellList(CGdsDocument* doc);                // 获取CGdsDocument引用的构元信息
    void           ConstructTreeWidget();// 构建cell树结构(主构元)

QList<QString> m_celllist;
private:
    void createActions();
    void createConnection();
    void populateMenu(QMenu *menu,  QList<QAction *> actions);
    void createMenus();
    void GetChildTreeWidget(const CGeoGDSStruct* , QTreeWidgetItem*);//构建子构元树结构

    CGdsDocument * gdsdoc;
    QAction *sendInfoHintAction;
    QAction *pathReplayAction;
    QAction *canncelPathReplayAction;

    QAction *cellFCL;//Flat Cell List
    QAction *cellSM; //Split Mode
    QAction *cellMode;
    QAction *cellHierarchy;
    QAction *cellFlatten;
    QAction *cellSorting;
    QAction *cellSortByNameBeginToEnd;
    QAction *cellSortByNameEndToBegin;
    QAction *cellSortByTimeEarlyToLate;
    QAction *cellSortByTimeLateToEarly;
    QAction *cellSortByAreaSmaToLarge;
    QAction *cellSortByAreaLarToSmall;

    QAction *cellNewCell;
    QAction *cellDeleteCell;
    QAction *cellRenameCell;
    QAction *cellReplaceCell;
    QAction *cellFlattenCell;
    QAction *cellUserProperties;

    QAction *cellCopy;
    QAction *cellCut;
    QAction *cellPaste;

    QAction *cellSANT;//Show As New Top

    QAction *cellHide;
    QAction *cellShow;
    QAction *cellShowAll;
    QAction *cellWhereAmI;
    QAction *cellSSCA;//Save Selected Cells As

    QActionGroup *dispMethAct;
    QActionGroup *sortingAct;

    QMenu *popMenu;
private slots:
     void dispMethSlot(QAction*);
     void sortingSlot(QAction*);
};

#endif // CELLTREEWIDGET_H

