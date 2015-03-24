/********************************************************************************
** Form generated from reading UI file 'settings_general.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_GENERAL_H
#define UI_SETTINGS_GENERAL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GeneralConfigPage
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QRadioButton *startNewSessionOption;
    QRadioButton *startLastSessionOption;
    QHBoxLayout *horizontalLayout;
    QRadioButton *startNamedSessionOption;
    QLineEdit *startSessionName;
    QSpacerItem *verticalSpacer;
    QButtonGroup *startSessionOptions;

    void setupUi(QWidget *GeneralConfigPage)
    {
        if (GeneralConfigPage->objectName().isEmpty())
            GeneralConfigPage->setObjectName(QStringLiteral("GeneralConfigPage"));
        GeneralConfigPage->resize(500, 356);
        verticalLayout = new QVBoxLayout(GeneralConfigPage);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(GeneralConfigPage);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        startNewSessionOption = new QRadioButton(GeneralConfigPage);
        startSessionOptions = new QButtonGroup(GeneralConfigPage);
        startSessionOptions->setObjectName(QStringLiteral("startSessionOptions"));
        startSessionOptions->addButton(startNewSessionOption);
        startNewSessionOption->setObjectName(QStringLiteral("startNewSessionOption"));

        verticalLayout->addWidget(startNewSessionOption);

        startLastSessionOption = new QRadioButton(GeneralConfigPage);
        startSessionOptions->addButton(startLastSessionOption);
        startLastSessionOption->setObjectName(QStringLiteral("startLastSessionOption"));

        verticalLayout->addWidget(startLastSessionOption);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        startNamedSessionOption = new QRadioButton(GeneralConfigPage);
        startSessionOptions->addButton(startNamedSessionOption);
        startNamedSessionOption->setObjectName(QStringLiteral("startNamedSessionOption"));

        horizontalLayout->addWidget(startNamedSessionOption);

        startSessionName = new QLineEdit(GeneralConfigPage);
        startSessionName->setObjectName(QStringLiteral("startSessionName"));

        horizontalLayout->addWidget(startSessionName);


        verticalLayout->addLayout(horizontalLayout);

        verticalSpacer = new QSpacerItem(20, 250, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(GeneralConfigPage);

        QMetaObject::connectSlotsByName(GeneralConfigPage);
    } // setupUi

    void retranslateUi(QWidget *GeneralConfigPage)
    {
        GeneralConfigPage->setWindowTitle(QApplication::translate("GeneralConfigPage", "Form", 0));
        label->setText(QApplication::translate("GeneralConfigPage", "On application start:", 0));
        startNewSessionOption->setText(QApplication::translate("GeneralConfigPage", "Start a new session", 0));
        startLastSessionOption->setText(QApplication::translate("GeneralConfigPage", "Load last session", 0));
        startNamedSessionOption->setText(QApplication::translate("GeneralConfigPage", "Load session:", 0));
    } // retranslateUi

};

namespace Ui {
    class GeneralConfigPage: public Ui_GeneralConfigPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_GENERAL_H
