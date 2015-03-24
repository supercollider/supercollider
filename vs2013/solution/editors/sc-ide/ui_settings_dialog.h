/********************************************************************************
** Form generated from reading UI file 'settings_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_DIALOG_H
#define UI_SETTINGS_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include "icon_list_widget.hpp"

QT_BEGIN_NAMESPACE

class Ui_ConfigDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    ScIDE::IconListWidget *configPageList;
    QStackedWidget *configPageStack;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ConfigDialog)
    {
        if (ConfigDialog->objectName().isEmpty())
            ConfigDialog->setObjectName(QStringLiteral("ConfigDialog"));
        ConfigDialog->resize(579, 365);
        verticalLayout = new QVBoxLayout(ConfigDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        configPageList = new ScIDE::IconListWidget(ConfigDialog);
        configPageList->setObjectName(QStringLiteral("configPageList"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(configPageList->sizePolicy().hasHeightForWidth());
        configPageList->setSizePolicy(sizePolicy);
        configPageList->setMaximumSize(QSize(100, 16777215));
        configPageList->setDragDropMode(QAbstractItemView::NoDragDrop);
        configPageList->setIconSize(QSize(32, 32));
        configPageList->setProperty("isWrapping", QVariant(false));
        configPageList->setViewMode(QListView::ListMode);

        horizontalLayout->addWidget(configPageList);

        configPageStack = new QStackedWidget(ConfigDialog);
        configPageStack->setObjectName(QStringLiteral("configPageStack"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(configPageStack->sizePolicy().hasHeightForWidth());
        configPageStack->setSizePolicy(sizePolicy1);
        configPageStack->setLayoutDirection(Qt::LeftToRight);

        horizontalLayout->addWidget(configPageStack);


        verticalLayout->addLayout(horizontalLayout);

        buttonBox = new QDialogButtonBox(ConfigDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Ok|QDialogButtonBox::Reset);
        buttonBox->setCenterButtons(false);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(ConfigDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ConfigDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ConfigDialog, SLOT(reject()));
        QObject::connect(configPageList, SIGNAL(currentRowChanged(int)), configPageStack, SLOT(setCurrentIndex(int)));

        configPageStack->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(ConfigDialog);
    } // setupUi

    void retranslateUi(QDialog *ConfigDialog)
    {
        ConfigDialog->setWindowTitle(QApplication::translate("ConfigDialog", "SuperCollider IDE Configuration", 0));
    } // retranslateUi

};

namespace Ui {
    class ConfigDialog: public Ui_ConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_DIALOG_H
