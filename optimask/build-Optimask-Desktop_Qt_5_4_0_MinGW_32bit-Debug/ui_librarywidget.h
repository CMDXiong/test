/********************************************************************************
** Form generated from reading UI file 'librarywidget.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LIBRARYWIDGET_H
#define UI_LIBRARYWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LibraryWidget
{
public:
    QAction *actionView;
    QWidget *centralwidget;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;
    QGroupBox *groupBox;
    QLineEdit *lineEdit;
    QLabel *label;
    QGroupBox *groupBox_2;
    QLineEdit *lineEdit_2;
    QLabel *label_2;
    QGroupBox *groupBox_3;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuView;
    QMenu *menuDesign_Manager;
    QMenu *menuCategory;
    QMenu *menuCell;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *LibraryWidget)
    {
        if (LibraryWidget->objectName().isEmpty())
            LibraryWidget->setObjectName(QStringLiteral("LibraryWidget"));
        LibraryWidget->resize(470, 416);
        actionView = new QAction(LibraryWidget);
        actionView->setObjectName(QStringLiteral("actionView"));
        centralwidget = new QWidget(LibraryWidget);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        checkBox = new QCheckBox(centralwidget);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setGeometry(QRect(20, 40, 121, 21));
        checkBox_2 = new QCheckBox(centralwidget);
        checkBox_2->setObjectName(QStringLiteral("checkBox_2"));
        checkBox_2->setGeometry(QRect(170, 40, 101, 21));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(20, 90, 261, 171));
        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(20, 30, 231, 21));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 60, 221, 91));
        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(290, 90, 141, 171));
        lineEdit_2 = new QLineEdit(groupBox_2);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(10, 30, 121, 20));
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 60, 111, 91));
        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(20, 270, 401, 101));
        LibraryWidget->setCentralWidget(centralwidget);
        menubar = new QMenuBar(LibraryWidget);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 470, 23));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuDesign_Manager = new QMenu(menubar);
        menuDesign_Manager->setObjectName(QStringLiteral("menuDesign_Manager"));
        menuCategory = new QMenu(menubar);
        menuCategory->setObjectName(QStringLiteral("menuCategory"));
        menuCell = new QMenu(menuCategory);
        menuCell->setObjectName(QStringLiteral("menuCell"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        LibraryWidget->setMenuBar(menubar);
        statusbar = new QStatusBar(LibraryWidget);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        LibraryWidget->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuDesign_Manager->menuAction());
        menubar->addAction(menuCategory->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuCategory->addAction(menuCell->menuAction());
        menuCategory->addSeparator();
        menuCell->addAction(actionView);

        retranslateUi(LibraryWidget);

        QMetaObject::connectSlotsByName(LibraryWidget);
    } // setupUi

    void retranslateUi(QMainWindow *LibraryWidget)
    {
        LibraryWidget->setWindowTitle(QApplication::translate("LibraryWidget", "Library Manager", 0));
        actionView->setText(QApplication::translate("LibraryWidget", "View", 0));
        checkBox->setText(QApplication::translate("LibraryWidget", "Show Categories", 0));
        checkBox_2->setText(QApplication::translate("LibraryWidget", "  Show Files", 0));
        groupBox->setTitle(QApplication::translate("LibraryWidget", "Library", 0));
        label->setText(QApplication::translate("LibraryWidget", "TextLabel", 0));
        groupBox_2->setTitle(QApplication::translate("LibraryWidget", "View", 0));
        label_2->setText(QApplication::translate("LibraryWidget", "TextLabel", 0));
        groupBox_3->setTitle(QApplication::translate("LibraryWidget", "Messages", 0));
        menuFile->setTitle(QApplication::translate("LibraryWidget", "File", 0));
        menuEdit->setTitle(QApplication::translate("LibraryWidget", "Edit", 0));
        menuView->setTitle(QApplication::translate("LibraryWidget", "View", 0));
        menuDesign_Manager->setTitle(QApplication::translate("LibraryWidget", "Design Manager", 0));
        menuCategory->setTitle(QApplication::translate("LibraryWidget", "Category", 0));
        menuCell->setTitle(QApplication::translate("LibraryWidget", "Cell", 0));
        menuHelp->setTitle(QApplication::translate("LibraryWidget", "Help", 0));
    } // retranslateUi

};

namespace Ui {
    class LibraryWidget: public Ui_LibraryWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LIBRARYWIDGET_H
