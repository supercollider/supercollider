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

#pragma once

#include "serialization.hpp" // to forward meta type declarations

#include <QObject>
#include <QKeySequence>
#include <QList>
#include <QAction>
#include <QTextCharFormat>

namespace ScIDE { namespace Settings {

class Theme;

class Manager : public QObject {
    Q_OBJECT

public:
    explicit Manager(const QString& filename, QObject* parent = 0);

    // forwarded
    void beginGroup(const QString& prefix) { mSettings->beginGroup(prefix); }
    void endGroup() { mSettings->endGroup(); }

    QString group() const { return mSettings->group(); }
    QStringList childGroups() const { return mSettings->childGroups(); }
    QStringList childKeys() const { return mSettings->childKeys(); }
    QStringList allKeys() const { return mSettings->allKeys(); }

    void sync() { mSettings->sync(); }
    void remove(const QString& key) { mSettings->remove(key); }

    // extended
    bool contains(const QString& key) const;
    bool isOverridden(const QString& key) const { return mSettings->contains(key); }

    QVariant value(const QString& key) const;
    QVariant defaultValue(const QString& key) const { return mDefaults.value(resolvedKey(key)); }

    void setValue(const QString& key, const QVariant& value);

    QKeySequence shortcut(const QString& key);

    const QList<QAction*>& actions() { return mActions; }
    void addAction(QAction* action, const QString& key, const QString& category = QString());
    QString keyForAction(QAction* action);

    QFont codeFont();

    int defaultCursorFlashTime() const { return mDefaultCursorFlashTime; }

    void setThemeVal(QString key, const QTextCharFormat& val);
    const QTextCharFormat& getThemeVal(QString key);
    void updateTheme();

private:
    void setDefault(const QString& key, const QVariant& value) { mDefaults.insert(resolvedKey(key), value); }

    typedef QSettings::SettingsMap SettingsMap;

    QString resolvedKey(const QString& key) const {
        QString fullKey = mSettings->group();
        if (!fullKey.isEmpty())
            fullKey.append("/");
        fullKey.append(key);
        return fullKey;
    }

    void initDefaults();
    void initHighlightingDefaults();

    QSettings* mSettings;
    SettingsMap mDefaults;
    QList<QAction*> mActions;
    const int mDefaultCursorFlashTime;
    Theme* mTheme;
};

struct ActionData {
    QString category;
    QString key;
};

}} // namespace ScIDE::Settings

Q_DECLARE_METATYPE(ScIDE::Settings::ActionData);
