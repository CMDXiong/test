/********************************************************************************
** Form generated from reading UI file 'multiline.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MULTILINE_H
#define UI_MULTILINE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MultiLine
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *comboBox;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QSpinBox *spinBox;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBox;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_4;
    QComboBox *comboBox_2;
    QWidget *layoutWidget2;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QDoubleSpinBox *doubleSpinBox_2;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label_6;
    QComboBox *comboBox_3;
    QWidget *layoutWidget3;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_7;
    QDoubleSpinBox *doubleSpinBox_3;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label_8;
    QComboBox *comboBox_4;

    void setupUi(QWidget *MultiLine)
    {
        if (MultiLine->objectName().isEmpty())
            MultiLine->setObjectName(QStringLiteral("MultiLine"));
        MultiLine->resize(465, 303);
        layoutWidget = new QWidget(MultiLine);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(40, 40, 411, 24));
        horizontalLayout_3 = new QHBoxLayout(layoutWidget);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        comboBox = new QComboBox(layoutWidget);
        comboBox->setObjectName(QStringLiteral("comboBox"));

        horizontalLayout->addWidget(comboBox);


        horizontalLayout_3->addLayout(horizontalLayout);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        spinBox = new QSpinBox(layoutWidget);
        spinBox->setObjectName(QStringLiteral("spinBox"));

        horizontalLayout_2->addWidget(spinBox);


        horizontalLayout_3->addLayout(horizontalLayout_2);

        layoutWidget1 = new QWidget(MultiLine);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(40, 90, 411, 22));
        horizontalLayout_4 = new QHBoxLayout(layoutWidget1);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(layoutWidget1);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_4->addWidget(label_3);

        doubleSpinBox = new QDoubleSpinBox(layoutWidget1);
        doubleSpinBox->setObjectName(QStringLiteral("doubleSpinBox"));

        horizontalLayout_4->addWidget(doubleSpinBox);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);

        label_4 = new QLabel(layoutWidget1);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_4->addWidget(label_4);

        comboBox_2 = new QComboBox(layoutWidget1);
        comboBox_2->setObjectName(QStringLiteral("comboBox_2"));

        horizontalLayout_4->addWidget(comboBox_2);

        layoutWidget2 = new QWidget(MultiLine);
        layoutWidget2->setObjectName(QStringLiteral("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(40, 140, 411, 22));
        horizontalLayout_5 = new QHBoxLayout(layoutWidget2);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        label_5 = new QLabel(layoutWidget2);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_5->addWidget(label_5);

        doubleSpinBox_2 = new QDoubleSpinBox(layoutWidget2);
        doubleSpinBox_2->setObjectName(QStringLiteral("doubleSpinBox_2"));

        horizontalLayout_5->addWidget(doubleSpinBox_2);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_3);

        label_6 = new QLabel(layoutWidget2);
        label_6->setObjectName(QStringLiteral("label_6"));

        horizontalLayout_5->addWidget(label_6);

        comboBox_3 = new QComboBox(layoutWidget2);
        comboBox_3->setObjectName(QStringLiteral("comboBox_3"));

        horizontalLayout_5->addWidget(comboBox_3);

        layoutWidget3 = new QWidget(MultiLine);
        layoutWidget3->setObjectName(QStringLiteral("layoutWidget3"));
        layoutWidget3->setGeometry(QRect(40, 190, 411, 22));
        horizontalLayout_6 = new QHBoxLayout(layoutWidget3);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        label_7 = new QLabel(layoutWidget3);
        label_7->setObjectName(QStringLiteral("label_7"));

        horizontalLayout_6->addWidget(label_7);

        doubleSpinBox_3 = new QDoubleSpinBox(layoutWidget3);
        doubleSpinBox_3->setObjectName(QStringLiteral("doubleSpinBox_3"));

        horizontalLayout_6->addWidget(doubleSpinBox_3);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_4);

        label_8 = new QLabel(layoutWidget3);
        label_8->setObjectName(QStringLiteral("label_8"));

        horizontalLayout_6->addWidget(label_8);

        comboBox_4 = new QComboBox(layoutWidget3);
        comboBox_4->setObjectName(QStringLiteral("comboBox_4"));

        horizontalLayout_6->addWidget(comboBox_4);


        retranslateUi(MultiLine);

        QMetaObject::connectSlotsByName(MultiLine);
    } // setupUi

    void retranslateUi(QWidget *MultiLine)
    {
        MultiLine->setWindowTitle(QApplication::translate("MultiLine", "MultiLine", 0));
        label->setText(QApplication::translate("MultiLine", "Multi Line Type:", 0));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("MultiLine", "Parallel", 0)
         << QApplication::translate("MultiLine", "Concentric", 0)
        );
        label_2->setText(QApplication::translate("MultiLine", "Number of Lines", 0));
        label_3->setText(QApplication::translate("MultiLine", "Concertric Radius:", 0));
        label_4->setText(QApplication::translate("MultiLine", "Unit", 0));
        comboBox_2->clear();
        comboBox_2->insertItems(0, QStringList()
         << QApplication::translate("MultiLine", "um", 0)
         << QApplication::translate("MultiLine", "mm", 0)
         << QApplication::translate("MultiLine", "nm", 0)
        );
        label_5->setText(QApplication::translate("MultiLine", "Line Width:       ", 0));
        label_6->setText(QApplication::translate("MultiLine", "Unit", 0));
        comboBox_3->clear();
        comboBox_3->insertItems(0, QStringList()
         << QApplication::translate("MultiLine", "um", 0)
         << QApplication::translate("MultiLine", "mm", 0)
         << QApplication::translate("MultiLine", "nm", 0)
        );
        label_7->setText(QApplication::translate("MultiLine", "Line Spacing:     ", 0));
        label_8->setText(QApplication::translate("MultiLine", "Unit", 0));
        comboBox_4->clear();
        comboBox_4->insertItems(0, QStringList()
         << QApplication::translate("MultiLine", "um", 0)
         << QApplication::translate("MultiLine", "mm", 0)
         << QApplication::translate("MultiLine", "nm", 0)
        );
    } // retranslateUi

};

namespace Ui {
    class MultiLine: public Ui_MultiLine {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MULTILINE_H
