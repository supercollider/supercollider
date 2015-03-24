/********************************************************************************
** Form generated from reading UI file 'settings_shortcuts.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_SHORTCUTS_H
#define UI_SETTINGS_SHORTCUTS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "key_sequence_edit.hpp"

QT_BEGIN_NAMESPACE

class Ui_ShortcutConfigPage
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label_3;
    QLineEdit *filter;
    QTreeWidget *actionTree;
    QFormLayout *formLayout;
    QRadioButton *defaultOpt;
    QLabel *defaultSeq;
    QRadioButton *customOpt;
    QHBoxLayout *horizontalLayout_2;
    ScIDE::KeySequenceEdit *customSeq;
    QToolButton *clearSeq;

    void setupUi(QWidget *ShortcutConfigPage)
    {
        if (ShortcutConfigPage->objectName().isEmpty())
            ShortcutConfigPage->setObjectName(QStringLiteral("ShortcutConfigPage"));
        ShortcutConfigPage->resize(507, 378);
        verticalLayout = new QVBoxLayout(ShortcutConfigPage);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_3 = new QLabel(ShortcutConfigPage);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout->addWidget(label_3);

        filter = new QLineEdit(ShortcutConfigPage);
        filter->setObjectName(QStringLiteral("filter"));

        horizontalLayout->addWidget(filter);


        verticalLayout->addLayout(horizontalLayout);

        actionTree = new QTreeWidget(ShortcutConfigPage);
        actionTree->setObjectName(QStringLiteral("actionTree"));

        verticalLayout->addWidget(actionTree);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setLabelAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        defaultOpt = new QRadioButton(ShortcutConfigPage);
        defaultOpt->setObjectName(QStringLiteral("defaultOpt"));

        formLayout->setWidget(0, QFormLayout::LabelRole, defaultOpt);

        defaultSeq = new QLabel(ShortcutConfigPage);
        defaultSeq->setObjectName(QStringLiteral("defaultSeq"));

        formLayout->setWidget(0, QFormLayout::FieldRole, defaultSeq);

        customOpt = new QRadioButton(ShortcutConfigPage);
        customOpt->setObjectName(QStringLiteral("customOpt"));

        formLayout->setWidget(1, QFormLayout::LabelRole, customOpt);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        customSeq = new ScIDE::KeySequenceEdit(ShortcutConfigPage);
        customSeq->setObjectName(QStringLiteral("customSeq"));
        customSeq->setReadOnly(true);

        horizontalLayout_2->addWidget(customSeq);

        clearSeq = new QToolButton(ShortcutConfigPage);
        clearSeq->setObjectName(QStringLiteral("clearSeq"));
        clearSeq->setToolButtonStyle(Qt::ToolButtonIconOnly);
        clearSeq->setAutoRaise(false);

        horizontalLayout_2->addWidget(clearSeq);


        formLayout->setLayout(1, QFormLayout::FieldRole, horizontalLayout_2);


        verticalLayout->addLayout(formLayout);


        retranslateUi(ShortcutConfigPage);

        QMetaObject::connectSlotsByName(ShortcutConfigPage);
    } // setupUi

    void retranslateUi(QWidget *ShortcutConfigPage)
    {
        ShortcutConfigPage->setWindowTitle(QApplication::translate("ShortcutConfigPage", "Form", 0));
        label_3->setText(QApplication::translate("ShortcutConfigPage", "Filter:", 0));
        QTreeWidgetItem *___qtreewidgetitem = actionTree->headerItem();
        ___qtreewidgetitem->setText(2, QApplication::translate("ShortcutConfigPage", "Description", 0));
        ___qtreewidgetitem->setText(1, QApplication::translate("ShortcutConfigPage", "Shortcut", 0));
        ___qtreewidgetitem->setText(0, QApplication::translate("ShortcutConfigPage", "Action", 0));
        defaultOpt->setText(QApplication::translate("ShortcutConfigPage", "Default:", 0));
        customOpt->setText(QApplication::translate("ShortcutConfigPage", "Custom:", 0));
        clearSeq->setText(QApplication::translate("ShortcutConfigPage", "Clear", 0));
    } // retranslateUi

};

namespace Ui {
    class ShortcutConfigPage: public Ui_ShortcutConfigPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_SHORTCUTS_H
