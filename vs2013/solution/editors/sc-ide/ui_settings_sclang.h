/********************************************************************************
** Form generated from reading UI file 'settings_sclang.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_SCLANG_H
#define UI_SETTINGS_SCLANG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "path_chooser_widget.hpp"

QT_BEGIN_NAMESPACE

class Ui_SclangConfigPage
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QFormLayout *formLayout;
    QCheckBox *autoStart;
    QLabel *label_3;
    ScIDE::PathChooserWidget *runtimeDir;
    QLabel *activeConfigFileLabel;
    QHBoxLayout *horizontalLayout;
    QComboBox *activeConfigFileComboBox;
    QToolButton *sclang_add_configfile;
    QToolButton *sclang_remove_configfile;
    QGroupBox *ide_sclang_override_configuration_file;
    QFormLayout *formLayout_2;
    QLabel *label;
    QWidget *widget_3;
    QGridLayout *gridLayout;
    QListWidget *sclang_include_directories;
    QVBoxLayout *verticalLayout_3;
    QToolButton *sclang_add_include;
    QToolButton *sclang_remove_include;
    QLabel *excludeLabel;
    QWidget *widget_2;
    QGridLayout *gridLayout_2;
    QListWidget *sclang_exclude_directories;
    QVBoxLayout *verticalLayout_2;
    QToolButton *sclang_add_exclude;
    QToolButton *sclang_remove_exclude;
    QCheckBox *sclang_post_inline_warnings;

    void setupUi(QWidget *SclangConfigPage)
    {
        if (SclangConfigPage->objectName().isEmpty())
            SclangConfigPage->setObjectName(QStringLiteral("SclangConfigPage"));
        SclangConfigPage->resize(613, 421);
        verticalLayout = new QVBoxLayout(SclangConfigPage);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        widget = new QWidget(SclangConfigPage);
        widget->setObjectName(QStringLiteral("widget"));
        formLayout = new QFormLayout(widget);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        autoStart = new QCheckBox(widget);
        autoStart->setObjectName(QStringLiteral("autoStart"));

        formLayout->setWidget(0, QFormLayout::FieldRole, autoStart);

        label_3 = new QLabel(widget);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);

        runtimeDir = new ScIDE::PathChooserWidget(widget);
        runtimeDir->setObjectName(QStringLiteral("runtimeDir"));

        formLayout->setWidget(1, QFormLayout::FieldRole, runtimeDir);

        activeConfigFileLabel = new QLabel(widget);
        activeConfigFileLabel->setObjectName(QStringLiteral("activeConfigFileLabel"));

        formLayout->setWidget(2, QFormLayout::LabelRole, activeConfigFileLabel);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        activeConfigFileComboBox = new QComboBox(widget);
        activeConfigFileComboBox->setObjectName(QStringLiteral("activeConfigFileComboBox"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(activeConfigFileComboBox->sizePolicy().hasHeightForWidth());
        activeConfigFileComboBox->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(activeConfigFileComboBox);

        sclang_add_configfile = new QToolButton(widget);
        sclang_add_configfile->setObjectName(QStringLiteral("sclang_add_configfile"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(sclang_add_configfile->sizePolicy().hasHeightForWidth());
        sclang_add_configfile->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(sclang_add_configfile);

        sclang_remove_configfile = new QToolButton(widget);
        sclang_remove_configfile->setObjectName(QStringLiteral("sclang_remove_configfile"));
        sizePolicy1.setHeightForWidth(sclang_remove_configfile->sizePolicy().hasHeightForWidth());
        sclang_remove_configfile->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(sclang_remove_configfile);


        formLayout->setLayout(2, QFormLayout::FieldRole, horizontalLayout);


        verticalLayout->addWidget(widget);

        ide_sclang_override_configuration_file = new QGroupBox(SclangConfigPage);
        ide_sclang_override_configuration_file->setObjectName(QStringLiteral("ide_sclang_override_configuration_file"));
        ide_sclang_override_configuration_file->setEnabled(true);
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(ide_sclang_override_configuration_file->sizePolicy().hasHeightForWidth());
        ide_sclang_override_configuration_file->setSizePolicy(sizePolicy2);
        ide_sclang_override_configuration_file->setFlat(false);
        ide_sclang_override_configuration_file->setCheckable(false);
        ide_sclang_override_configuration_file->setChecked(false);
        formLayout_2 = new QFormLayout(ide_sclang_override_configuration_file);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        label = new QLabel(ide_sclang_override_configuration_file);
        label->setObjectName(QStringLiteral("label"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label);

        widget_3 = new QWidget(ide_sclang_override_configuration_file);
        widget_3->setObjectName(QStringLiteral("widget_3"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(widget_3->sizePolicy().hasHeightForWidth());
        widget_3->setSizePolicy(sizePolicy3);
        gridLayout = new QGridLayout(widget_3);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        sclang_include_directories = new QListWidget(widget_3);
        sclang_include_directories->setObjectName(QStringLiteral("sclang_include_directories"));

        gridLayout->addWidget(sclang_include_directories, 0, 0, 2, 1);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        sclang_add_include = new QToolButton(widget_3);
        sclang_add_include->setObjectName(QStringLiteral("sclang_add_include"));
        sclang_add_include->setEnabled(true);

        verticalLayout_3->addWidget(sclang_add_include);

        sclang_remove_include = new QToolButton(widget_3);
        sclang_remove_include->setObjectName(QStringLiteral("sclang_remove_include"));
        sclang_remove_include->setEnabled(true);

        verticalLayout_3->addWidget(sclang_remove_include);


        gridLayout->addLayout(verticalLayout_3, 0, 2, 1, 1);


        formLayout_2->setWidget(2, QFormLayout::FieldRole, widget_3);

        excludeLabel = new QLabel(ide_sclang_override_configuration_file);
        excludeLabel->setObjectName(QStringLiteral("excludeLabel"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, excludeLabel);

        widget_2 = new QWidget(ide_sclang_override_configuration_file);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        sizePolicy3.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy3);
        gridLayout_2 = new QGridLayout(widget_2);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        sclang_exclude_directories = new QListWidget(widget_2);
        sclang_exclude_directories->setObjectName(QStringLiteral("sclang_exclude_directories"));

        gridLayout_2->addWidget(sclang_exclude_directories, 0, 1, 3, 1);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        sclang_add_exclude = new QToolButton(widget_2);
        sclang_add_exclude->setObjectName(QStringLiteral("sclang_add_exclude"));
        sclang_add_exclude->setEnabled(true);

        verticalLayout_2->addWidget(sclang_add_exclude);

        sclang_remove_exclude = new QToolButton(widget_2);
        sclang_remove_exclude->setObjectName(QStringLiteral("sclang_remove_exclude"));
        sclang_remove_exclude->setEnabled(true);

        verticalLayout_2->addWidget(sclang_remove_exclude);


        gridLayout_2->addLayout(verticalLayout_2, 0, 3, 1, 1);


        formLayout_2->setWidget(3, QFormLayout::FieldRole, widget_2);

        sclang_post_inline_warnings = new QCheckBox(ide_sclang_override_configuration_file);
        sclang_post_inline_warnings->setObjectName(QStringLiteral("sclang_post_inline_warnings"));

        formLayout_2->setWidget(4, QFormLayout::FieldRole, sclang_post_inline_warnings);


        verticalLayout->addWidget(ide_sclang_override_configuration_file);

        ide_sclang_override_configuration_file->raise();
        widget->raise();

        retranslateUi(SclangConfigPage);

        QMetaObject::connectSlotsByName(SclangConfigPage);
    } // setupUi

    void retranslateUi(QWidget *SclangConfigPage)
    {
        SclangConfigPage->setWindowTitle(QApplication::translate("SclangConfigPage", "Form", 0));
        autoStart->setText(QApplication::translate("SclangConfigPage", "Start Interpreter With IDE", 0));
        label_3->setText(QApplication::translate("SclangConfigPage", "Runtime Directory:", 0));
        activeConfigFileLabel->setText(QApplication::translate("SclangConfigPage", "Active config file:", 0));
#ifndef QT_NO_TOOLTIP
        sclang_add_configfile->setToolTip(QApplication::translate("SclangConfigPage", "Save a new config file", 0));
#endif // QT_NO_TOOLTIP
        sclang_add_configfile->setText(QApplication::translate("SclangConfigPage", "+", 0));
        sclang_remove_configfile->setText(QApplication::translate("SclangConfigPage", "-", 0));
        ide_sclang_override_configuration_file->setTitle(QApplication::translate("SclangConfigPage", "Interpreter Options (stored in current active config file)", 0));
        label->setText(QApplication::translate("SclangConfigPage", "Include:", 0));
        sclang_add_include->setText(QApplication::translate("SclangConfigPage", "+", 0));
        sclang_remove_include->setText(QApplication::translate("SclangConfigPage", "-", 0));
        excludeLabel->setText(QApplication::translate("SclangConfigPage", "Exclude:", 0));
        sclang_add_exclude->setText(QApplication::translate("SclangConfigPage", "+", 0));
        sclang_remove_exclude->setText(QApplication::translate("SclangConfigPage", "-", 0));
        sclang_post_inline_warnings->setText(QApplication::translate("SclangConfigPage", "Post Inline Warnings", 0));
    } // retranslateUi

};

namespace Ui {
    class SclangConfigPage: public Ui_SclangConfigPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_SCLANG_H
