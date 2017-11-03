#include "ccelldockwidget.h"
#include <QAction>
#include <QToolBar>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include "./Script/fuzzyquery.h"

#ifdef RELEASE
  const QString rsrcPath = "./Icon/";     //指定图标目录
#else
  const QString rsrcPath = "../src/Icon";     //指定图标目录
#endif

CCellDockWidget::CCellDockWidget(QWidget *parent) : QWidget(parent)
{
    init();
}

CCellDockWidget::~CCellDockWidget()
{

}

void CCellDockWidget::init()
{
    QAction *addSeparator = 0;
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    vboxLayout->setSpacing(0);
    vboxLayout->setContentsMargins(0, 0, 0, 0);
    vboxLayout->setObjectName(QStringLiteral("vboxLayout"));

    cellTreeTabs = new QTabWidget(this); //将QTabWidget变成中心部件
    cellTreeTabs->setObjectName(QStringLiteral("cellTreeTabs"));

    //第一行
    QHBoxLayout *topHoritalLayout = new QHBoxLayout();
    QComboBox *comboBox = new QComboBox();
    comboBox->addItems(QStringList()<<"Top dowm - all cells"<<"Bottom up - all cells"<<"Top dowm - non -instanced"
                       <<"By date modified"<<"DRC status");

    QToolBar *toolBar = new QToolBar();
    QAction *cellCollapseAll = new QAction(QIcon(rsrcPath+"/collapse.png"), tr("Collapse All"), this);
    cellCollapseAll->setToolTip(tr("Collapse All"));
    QAction *cellExpandAll = new QAction(QIcon(rsrcPath+"/Expand.png"), tr("Expand All"), this);
    cellExpandAll->setToolTip(tr("Expand All"));

    QList<QAction*> actions;
    actions << addSeparator << cellCollapseAll << cellExpandAll ;
    foreach (QAction *action, actions) {
        if (!action) {
            toolBar->addSeparator();
        }
        else {
            toolBar->addAction(action);
        }
    }

    topHoritalLayout->addWidget(comboBox);
    topHoritalLayout->addWidget(toolBar);

    QHBoxLayout *buttonHLayout = new QHBoxLayout();
    QToolBar *toolBar1 = new QToolBar();
    QAction *cellNewCell = new QAction(QIcon(rsrcPath+"/NewCell.png"), tr("New Cell"), this);
    cellNewCell->setToolTip(tr("New Cell"));
    QAction *cellDeleteCell = new QAction(QIcon(rsrcPath+"/DeleteCell.png"), tr("Delete Cell"), this);
    cellDeleteCell->setToolTip(tr("Delete Cell"));
    QAction *cellCopyToTextViewl = new QAction(QIcon(rsrcPath+"/CopyToText.png"), tr("Copy"), this);
    cellCopyToTextViewl->setToolTip(tr("Copy To Text View"));
    QAction *cellShowAllCell = new QAction(QIcon(rsrcPath+"/ShowAll.png"), tr("Show"), this);
    cellShowAllCell->setToolTip(tr(" Show All Cell"));
    QList<QAction*> actions1;
    actions1 <<addSeparator << cellNewCell << cellDeleteCell << addSeparator <<  cellCopyToTextViewl << addSeparator <<  cellShowAllCell;
    foreach (QAction *action, actions1) {
        if (!action) {
            toolBar1->addSeparator();
        }
        else {
            toolBar1->addAction(action);
        }
    }
    buttonHLayout->addWidget(toolBar1);

    //底部
    QHBoxLayout *horitalLayout=new QHBoxLayout();
    QLabel *label1=new QLabel("Levels");
    QSpinBox *spinbox1=new QSpinBox();
    spinbox1->setRange(-100,100);
    QLabel *label2=new QLabel("...");
    QSpinBox *spinbox2=new QSpinBox();
    spinbox2->setRange(-100,100);
    horitalLayout->addWidget(label1);
    horitalLayout->addWidget(spinbox1);
    horitalLayout->addWidget(label2);
    horitalLayout->addWidget(spinbox2);

    QHBoxLayout *horitalLayout2=new QHBoxLayout();
    QToolBar *toolBar2 = new QToolBar();
    QStringList queryStrList=QStringList() << "box" << "Aref" <<"Sref"<<"410";
    FuzzyQuery *queryEdit= new FuzzyQuery(queryStrList);
    queryEdit->setMaximumHeight(25);
    queryEdit->setMinimumHeight(25);
    QAction *findAct = new QAction(this);
    findAct->setIcon(QIcon(rsrcPath+"/find.png"));
    findAct->setToolTip(tr("Find"));
    toolBar2->addAction(findAct);
    horitalLayout2->addWidget(queryEdit);
    horitalLayout2->addWidget(toolBar2);

    vboxLayout->addLayout(topHoritalLayout);
    vboxLayout->addLayout(buttonHLayout);
    vboxLayout->addLayout(horitalLayout2);
    vboxLayout->addWidget(cellTreeTabs);
    vboxLayout->addLayout(horitalLayout);

}

