/************************************************************************
 *
 * Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
 *
 * This file is part of SuperCollider Qt GUI.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ************************************************************************/

#include "primitives.h"

#include "../Common.h"
#include "../type_codec.hpp"
#include "../QcApplication.h"
#include "../QObjectProxy.h"
#include "../widgets/QcWebView.h"
#include "../style/style.hpp"
#include "QtCollider.h"

#ifdef Q_OS_MAC
#    include "../hacks/hacks_mac.hpp"
#endif

#include <PyrKernel.h>

#include <QDesktopServices>
#include <QFontDatabase>
#include <QFontInfo>
#include <QFontMetrics>
#include <QDesktopWidget>
#include <QStyleFactory>
#include <QWebEngineSettings>
#include <QCursor>

namespace QtCollider {

QC_LANG_PRIMITIVE(QtGUI_SetDebugLevel, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QtCollider::setDebugLevel(QtCollider::get(a));
    return errNone;
}

QC_LANG_PRIMITIVE(QtGUI_DebugLevel, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    SetInt(r, QtCollider::debugLevel());
    return errNone;
}

QC_LANG_PRIMITIVE(QWindow_ScreenBounds, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    QtCollider::set(r, screenGeometry);
    return errNone;
}

QC_LANG_PRIMITIVE(QWindow_AvailableGeometry, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    QRect rect = QApplication::desktop()->availableGeometry();
    QtCollider::set(r, rect);
    return errNone;
}

QC_LANG_PRIMITIVE(Qt_StringBounds, 2, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QString str = QtCollider::get(a);

    QFont f = QtCollider::get(a + 1);

    QFontMetrics fm(f);
    QRect bounds = fm.boundingRect(str);

    // we keep the font height even on empty string;
    if (str.isEmpty())
        bounds.setHeight(fm.height());

    QtCollider::set(r, bounds);
    return errNone;
}

QC_LANG_PRIMITIVE(Qt_AvailableFonts, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QFontDatabase database;
    QVariantList list;
    Q_FOREACH (QString family, database.families())
        list << family;
    QtCollider::set(r, list);
    return errNone;
}

QC_LANG_PRIMITIVE(QFont_SetDefaultFont, 2, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    if (!isKindOfSlot(a + 0, SC_CLASS(Font)))
        return errWrongType;

    QFont font(QtCollider::read<QFont>(a + 0));
    const char* className = IsSym(a + 1) ? slotRawSymbol(a + 1)->name : 0;

    QApplication::setFont(font, className);

    return errNone;
}

QC_LANG_PRIMITIVE(QFont_DefaultFamilyForStyle, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    // NOTE:
    // On X11 systems we rely on default fontconfig mappings of font familiy names,
    // as style hints are not necessarily supported.
    // On other systems, style hints should work.

    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    if (!IsInt(a))
        return errWrongType;

    QFont::StyleHint styleHint;
    QString family;
    switch (slotRawInt(a)) {
    case 0:
        styleHint = QFont::SansSerif;
        family = "sans-serif";
        break;
    case 1:
        styleHint = QFont::Serif;
        family = "serif";
        break;
    case 2:
        styleHint = QFont::Monospace;
        family = "monospace";
        break;
    default:
        styleHint = QFont::AnyStyle;
    }

    QFont font(family);
    font.setStyleHint(styleHint, QFont::PreferMatch);

    QtCollider::set(r, font.defaultFamily());

    return errNone;
}

QC_LANG_PRIMITIVE(Qt_GlobalPalette, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    QPalette p(QApplication::palette());
    QtCollider::set(r, p);
    return errNone;
}

QC_LANG_PRIMITIVE(Qt_SetGlobalPalette, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

// The line below is a workaround. The non-win term causes Error C2440 in VS
// https://msdn.microsoft.com/en-us/library/sy5tsf8z.aspx
#if defined(_MSC_VER)
    QPalette p = (QPalette &&) QtCollider::get(a);
#else
    QPalette p = QtCollider::get(a);
#endif

    QApplication::setPalette(p);

    return errNone;
}

QC_LANG_PRIMITIVE(Qt_FocusWidget, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    QWidget* w = QApplication::focusWidget();

#ifdef Q_OS_MAC
    // On Mac we need to make additional checks, as Qt does not monitor
    // focus changes to native Cocoa windows in the same application.
    if (w && !QtCollider::Mac::isKeyWindow(w))
        w = 0;
#endif

    if (w) {
        QObjectProxy* proxy = QObjectProxy::fromObject(w);
        if (proxy && proxy->scObject()) {
            SetObject(r, proxy->scObject());
            return errNone;
        }
    }

    SetNil(r);
    return errNone;
}

QC_LANG_PRIMITIVE(Qt_SetStyle, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    QString str = QtCollider::get(a);
    if (str.isEmpty())
        return errFailed;

    QStyle* style = QStyleFactory::create(str);
    if (!style)
        return errFailed;

    QApplication::setStyle(new QtCollider::Style::StyleImpl(style));
    return errNone;
}

QC_LANG_PRIMITIVE(Qt_AvailableStyles, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    QVariantList list;
    Q_FOREACH (QString key, QStyleFactory::keys())
        list << key;

    QtCollider::set(r, list);
    return errNone;
}

QC_LANG_PRIMITIVE(QWebView_ClearMemoryCaches, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    // @TODO WebEngine: New cache method?
    // QWebEngineSettings::clearMemoryCaches();

    return errNone;
}

QC_LANG_PRIMITIVE(Qt_IsMethodOverridden, 2, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (NotObj(a) || NotSym(a + 1))
        return errWrongType;

    PyrObject* self = slotRawObject(r);
    PyrObjectHdr* superclass = slotRawObject(a);
    PyrSymbol* method = slotRawSymbol(a + 1);

    for (PyrClass* klass = self->classptr; klass != superclass && klass != class_object;
         klass = slotRawSymbol(&klass->superclass)->u.classobj) {
        PyrSlot* methodSlot = &klass->methods;
        if (!IsObj(methodSlot))
            continue;
        PyrObject* methodArray = slotRawObject(methodSlot);
        PyrSlot* methods = methodArray->slots;
        for (int i = 0; i < methodArray->size; ++i) {
            PyrMethod* m = slotRawMethod(methods + i);
            if (slotRawSymbol(&m->name) == method) {
                SetTrue(r);
                return errNone;
            }
        }
    }

    SetFalse(r);
    return errNone;
}

QC_LANG_PRIMITIVE(Qt_CursorPosition, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    QtCollider::set(r, QCursor::pos());

    return errNone;
}

QC_LANG_PRIMITIVE(Qt_SetUrlHandler, 2, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    QString str = QtCollider::get(a);

    if (IsNil(a + 1)) {
        QDesktopServices::unsetUrlHandler(str);
    } else {
        QcCallback* cb = QtCollider::get(a + 1);
        QDesktopServices::setUrlHandler(str, cb, "onCalled");
    }

    return errNone;
}

QC_LANG_PRIMITIVE(Qt_SetAppMenus, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    QList<QMenu*> menuList;


    if (isKindOfSlot(a, class_array)) {
        QMenuBar* menuBar = QcApplication::getMainMenu();
        if (menuBar) {
            menuBar->clear();

            PyrObject* obj = slotRawObject(a);
            PyrSlot* slots = obj->slots;
            int size = obj->size;

            for (int i = 0; i < size; ++i, ++slots) {
                QMenu* menu = QtCollider::get<QMenu*>(slots);
                if (menu) {
                    menuBar->addMenu(menu);
                } else {
                    menuBar->addSeparator();
                }
            }
        }
    }

    return errNone;
}

QC_LANG_PRIMITIVE(QView_AddActionToView, 3, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    QObjectProxy* widgetObj = TypeCodec<QObjectProxy*>::safeRead(a);
    QObjectProxy* actionObj = TypeCodec<QObjectProxy*>::safeRead(a + 1);
    QObjectProxy* beforeObj = TypeCodec<QObjectProxy*>::safeRead(a + 2);

    if (widgetObj && actionObj) {
        QWidget* widget = qobject_cast<QWidget*>(widgetObj->object());
        QAction* action = qobject_cast<QAction*>(actionObj->object());
        QAction* beforeAction = beforeObj ? qobject_cast<QAction*>(beforeObj->object()) : 0;

        if (!beforeAction) {
            PyrSlot* indexArg = a + 2;

            if (NotNil(indexArg)) {
                if (IsInt(indexArg)) {
                    int index = std::max(slotRawInt(indexArg), 0);

                    auto actions = widget->actions();

                    if (index < actions.size()) {
                        beforeAction = actions[index];
                    }
                } else {
                    return errFailed;
                }
            }
        }

        if (widget && action) {
            if (beforeAction) {
                widget->insertAction(beforeAction, action);
            } else {
                widget->addAction(action);
            }
            return errNone;
        }
    }

    return errFailed;
}

QC_LANG_PRIMITIVE(QView_RemoveActionFromView, 2, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    QObjectProxy* widgetObj = TypeCodec<QObjectProxy*>::safeRead(a);
    QObjectProxy* actionObj = TypeCodec<QObjectProxy*>::safeRead(a + 1);

    if (widgetObj && actionObj) {
        QWidget* widget = qobject_cast<QWidget*>(widgetObj->object());
        QAction* action = qobject_cast<QAction*>(actionObj->object());

        if (widget && action) {
            widget->removeAction(action);
            return errNone;
        }
    }

    return errFailed;
}

QC_LANG_PRIMITIVE(QView_RemoveAllActionsFromView, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    if (!QcApplication::compareThread())
        return QtCollider::wrongThreadError();

    QObjectProxy* widgetObj = TypeCodec<QObjectProxy*>::safeRead(a);

    if (widgetObj) {
        QWidget* widget = qobject_cast<QWidget*>(widgetObj->object());

        if (widget) {
            auto actions = widget->actions();
            for (auto action : actions) {
                widget->removeAction(action);
            }

            return errNone;
        }
    }

    return errFailed;
}


void defineMiscPrimitives() {
    LangPrimitiveDefiner definer;
    definer.define<QtGUI_SetDebugLevel>();
    definer.define<QtGUI_DebugLevel>();
    definer.define<QWindow_ScreenBounds>();
    definer.define<QWindow_AvailableGeometry>();
    definer.define<Qt_StringBounds>();
    definer.define<Qt_AvailableFonts>();
    definer.define<QFont_SetDefaultFont>();
    definer.define<QFont_DefaultFamilyForStyle>();
    definer.define<Qt_GlobalPalette>();
    definer.define<Qt_SetGlobalPalette>();
    definer.define<Qt_FocusWidget>();
    definer.define<Qt_SetStyle>();
    definer.define<Qt_AvailableStyles>();
    definer.define<Qt_IsMethodOverridden>();
    definer.define<QWebView_ClearMemoryCaches>();
    definer.define<Qt_CursorPosition>();
    definer.define<Qt_SetUrlHandler>();
    definer.define<Qt_SetAppMenus>();
    definer.define<QView_AddActionToView>();
    definer.define<QView_RemoveActionFromView>();
    definer.define<QView_RemoveAllActionsFromView>();
}

} // namespace QtCollider
