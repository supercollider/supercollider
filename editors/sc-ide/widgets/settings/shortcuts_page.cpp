/*
    SuperCollider Qt IDE
    Copyright (c) 2012 Jakob Leben & Tim Blechmann
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "shortcuts_page.hpp"
#include "ui_settings_shortcuts.h"
#include "../../core/settings/manager.hpp"
#include "../../core/main.hpp"

#include <QHeaderView>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QKeyEvent>
#include <QMessageBox>

Q_DECLARE_METATYPE(QAction*)
Q_DECLARE_METATYPE(QKeySequence)

namespace ScIDE { namespace Settings {

ShortcutsPage::ShortcutsPage(QWidget* parent): QWidget(parent), ui(new Ui::ShortcutConfigPage) {
    ui->setupUi(this);

    ui->clearSeq->setIcon(style()->standardIcon(QStyle::SP_DockWidgetCloseButton));

    connect(ui->filter, SIGNAL(textChanged(QString)), this, SLOT(filterBy(QString)));
    connect(ui->actionTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this,
            SLOT(showItem(QTreeWidgetItem*, QTreeWidgetItem*)));

    // automation
    connect(ui->defaultOpt, SIGNAL(clicked()), ui->customSeq, SLOT(reset()));
    connect(ui->customOpt, SIGNAL(clicked()), ui->customSeq, SLOT(setFocus()));
    connect(ui->customSeq, SIGNAL(editingStarted()), ui->customOpt, SLOT(toggle()));
    connect(ui->clearSeq, SIGNAL(clicked()), ui->customSeq, SLOT(reset()));
    connect(ui->clearSeq, SIGNAL(clicked()), ui->customOpt, SLOT(click()));

    // value application
    connect(ui->defaultOpt, SIGNAL(clicked()), this, SLOT(apply()));
    connect(ui->customOpt, SIGNAL(clicked()), this, SLOT(apply()));
    connect(ui->customSeq, SIGNAL(editingFinished()), this, SLOT(apply()));
}

ShortcutsPage::~ShortcutsPage() { delete ui; }

void ShortcutsPage::load(Manager* s) {
    ui->actionTree->clear();

    s->beginGroup("IDE/shortcuts");

    const QList<QAction*>& actions = s->actions();
    foreach (QAction* action, actions)
        addAction(action, s);

    s->endGroup();

    ui->actionTree->sortByColumn(0, Qt::AscendingOrder);
    ui->actionTree->header()->resizeSections(QHeaderView::ResizeToContents);
}

void ShortcutsPage::store(Manager* s) {
    s->beginGroup("IDE/shortcuts");

    s->remove("");

    int c = ui->actionTree->topLevelItemCount();
    for (int i = 0; i < c; ++i) {
        QTreeWidgetItem* item = ui->actionTree->topLevelItem(i);
        QAction* action = item->data(0, ActionRole).value<QAction*>();
        Q_ASSERT(action);

        QVariant var = item->data(0, CustomSequenceRole);
        if (var.isValid()) {
            s->setValue(s->keyForAction(action), var);
        } else {
            var = s->defaultValue(s->keyForAction(action));
        }
        action->setShortcut(var.value<QKeySequence>());
    }

    s->endGroup();
}

void ShortcutsPage::addAction(QAction* a, Manager* s) {
    QString description = a->statusTip();
    if (description.isEmpty())
        description = a->toolTip();

    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setIcon(0, a->icon());
    item->setText(0, a->text().remove('&'));
    item->setText(2, description);

    QString key = s->keyForAction(a);

    item->setData(0, ActionRole, QVariant::fromValue<QAction*>(a));

    item->setData(0, DefaultSequenceRole, s->defaultValue(key));

    if (s->isOverridden(key)) {
        // For reason of performance, ensure the variant holds a QKeySequence
        QKeySequence seq = s->value(key).value<QKeySequence>();
        item->setData(0, CustomSequenceRole, QVariant::fromValue<QKeySequence>(seq));
    }

    updateItem(item);

    ui->actionTree->addTopLevelItem(item);
}

void ShortcutsPage::filterBy(const QString& str) {
    int c = ui->actionTree->topLevelItemCount();
    if (str.isEmpty()) {
        for (int i = 0; i < c; ++i)
            ui->actionTree->topLevelItem(i)->setHidden(false);
    } else {
        for (int i = 0; i < c; ++i) {
            QTreeWidgetItem* item = ui->actionTree->topLevelItem(i);
            bool visible =
                item->text(0).contains(str, Qt::CaseInsensitive) || item->text(2).contains(str, Qt::CaseInsensitive);
            item->setHidden(!visible);
        }
    }
}

void ShortcutsPage::showItem(QTreeWidgetItem* item, QTreeWidgetItem* prev) {
    QVariant defaultSeqVar = item ? item->data(0, DefaultSequenceRole) : QVariant();
    QString defaultSeq = defaultSeqVar.value<QKeySequence>().toString(QKeySequence::NativeText);
    ui->defaultSeq->setText(defaultSeq);

    QVariant customSeqVar = item ? item->data(0, CustomSequenceRole) : QVariant();
    ui->customSeq->setSequence(customSeqVar.value<QKeySequence>());

    if (customSeqVar.isValid())
        ui->customOpt->setChecked(true);
    else
        ui->defaultOpt->setChecked(true);
}

void ShortcutsPage::apply() { applyTo(ui->actionTree->currentItem()); }

void ShortcutsPage::applyTo(QTreeWidgetItem* targetItem) {
    if (!targetItem)
        return;

    bool seqIsCustom = ui->customOpt->isChecked();

    QKeySequence currentSeq =
        seqIsCustom ? ui->customSeq->sequence() : targetItem->data(0, DefaultSequenceRole).value<QKeySequence>();

    // Check for duplicates
    if (!currentSeq.isEmpty()) {
        int itemCount = ui->actionTree->topLevelItemCount();
        for (int itemIdx = 0; itemIdx < itemCount; ++itemIdx) {
            QTreeWidgetItem* item = ui->actionTree->topLevelItem(itemIdx);
            if (item == targetItem)
                continue;
            QKeySequence itemSeq = activeItemSequence(item);
            if (currentSeq == itemSeq) {
                // Got a duplicate sequence
                if (confirmOverride(currentSeq, item)) {
                    item->setData(0, CustomSequenceRole, QVariant::fromValue(QKeySequence()));
                    updateItem(item);
                } else {
                    seqIsCustom = true;
                    currentSeq = QKeySequence();
                    ui->customOpt->setChecked(true);
                    ui->customSeq->clear();
                }
                break;
            }
        }
    }

    if (seqIsCustom) {
        targetItem->setData(0, CustomSequenceRole, QVariant::fromValue(currentSeq));
    } else {
        targetItem->setData(0, CustomSequenceRole, QVariant());
    }

    updateItem(targetItem);
}

bool ShortcutsPage::confirmOverride(const QKeySequence& duplicateSequence, QTreeWidgetItem* duplicateItem) {
    QString warningString = tr("Shortcut '%1' has already been assigned to '%2'.\n\n"
                               "Would you like to override it?");

    warningString = warningString.arg(duplicateSequence.toString(), duplicateItem->text(0));

    QMessageBox::StandardButton result = QMessageBox::warning(
        this, tr("Duplicate Shortcut"), warningString, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);

    return result == QMessageBox::Ok;
}

void ShortcutsPage::updateItem(QTreeWidgetItem* item) {
    QVariant seqData = item->data(0, CustomSequenceRole);
    if (!seqData.isValid())
        seqData = item->data(0, DefaultSequenceRole);

    QKeySequence seq = seqData.value<QKeySequence>();

    item->setText(1, seq.toString(QKeySequence::NativeText));
}

QKeySequence ShortcutsPage::activeItemSequence(QTreeWidgetItem* item) {
    QVariant seqVar = item->data(0, CustomSequenceRole);
    if (!seqVar.isValid())
        seqVar = item->data(0, DefaultSequenceRole);
    return seqVar.value<QKeySequence>();
}

}} // namespace ScIDE::Settings
