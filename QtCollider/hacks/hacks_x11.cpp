/************************************************************************
 *
 * adapted from:
 * wmctrl
 * A command line tool to interact with an EWMH/NetWM compatible X Window Manager.
 *
 * Copyright 2003 Tomas Styblo <tripie@cpan.org>
 * Copyright 2011 Tim Blechmann (tim@klingt.org)
 * Copyright 2011 Jakob Leben (jakob.leben@gmail.com)
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

#include "hacks_x11.hpp"

#ifdef Q_WS_X11
#    include <stdlib.h>
#    include <stdio.h>

#    include <X11/Xlib.h>
#    include <X11/Xatom.h>

#    define MAX_PROPERTY_VALUE_LEN 4096

static inline void p_verbose(...) {}

#    define gchar char
#    define gboolean bool
#    define g_malloc (char*)malloc
#    define g_free free

static gchar* get_property(Display* disp, Window win, Atom xa_prop_type, const gchar* prop_name, unsigned long* size) {
    Atom xa_prop_name;
    Atom xa_ret_type;
    int ret_format;
    unsigned long ret_nitems;
    unsigned long ret_bytes_after;
    unsigned long tmp_size;
    unsigned char* ret_prop;
    gchar* ret;

    xa_prop_name = XInternAtom(disp, prop_name, False);

    /* MAX_PROPERTY_VALUE_LEN / 4 explanation (XGetWindowProperty manpage):
     *
     * long_length = Specifies the length in 32-bit multiples of the
     *               data to be retrieved.
     */
    if (XGetWindowProperty(disp, win, xa_prop_name, 0, MAX_PROPERTY_VALUE_LEN / 4, False, xa_prop_type, &xa_ret_type,
                           &ret_format, &ret_nitems, &ret_bytes_after, &ret_prop)
        != Success) {
        p_verbose("Cannot get %s property.\n", prop_name);
        return NULL;
    }

    if (xa_ret_type != xa_prop_type) {
        p_verbose("Invalid type of %s property.\n", prop_name);
        XFree(ret_prop);
        return NULL;
    }

    /* null terminate the result to make string handling easier */
    tmp_size = (ret_format / 8) * ret_nitems;
    ret = g_malloc(tmp_size + 1);
    memcpy(ret, ret_prop, tmp_size);
    ret[tmp_size] = '\0';

    if (size) {
        *size = tmp_size;
    }

    XFree(ret_prop);
    return ret;
} /*}}}*/

static int client_msg(Display* disp, Window win, const char* msg, /* {{{ */
                      unsigned long data0, unsigned long data1, unsigned long data2, unsigned long data3,
                      unsigned long data4) {
    XEvent event;
    long mask = SubstructureRedirectMask | SubstructureNotifyMask;

    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.message_type = XInternAtom(disp, msg, False);
    event.xclient.window = win;
    event.xclient.format = 32;
    event.xclient.data.l[0] = data0;
    event.xclient.data.l[1] = data1;
    event.xclient.data.l[2] = data2;
    event.xclient.data.l[3] = data3;
    event.xclient.data.l[4] = data4;

    if (XSendEvent(disp, DefaultRootWindow(disp), False, mask, &event)) {
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "Cannot send %s event.\n", msg);
        return EXIT_FAILURE;
    }
} /*}}}*/

static int activate_window(Display* disp, Window win, /* {{{ */
                           gboolean switch_desktop) {
    unsigned long* desktop;

    /* desktop ID */
    if ((desktop = (unsigned long*)get_property(disp, win, XA_CARDINAL, "_NET_WM_DESKTOP", NULL)) == NULL) {
        if ((desktop = (unsigned long*)get_property(disp, win, XA_CARDINAL, "_WIN_WORKSPACE", NULL)) == NULL) {
            p_verbose("Cannot find desktop ID of the window.\n");
        }
    }

    if (switch_desktop && desktop) {
        if (client_msg(disp, DefaultRootWindow(disp), "_NET_CURRENT_DESKTOP", *desktop, 0, 0, 0, 0) != EXIT_SUCCESS) {
            p_verbose("Cannot switch desktop.\n");
        }
    }
    if (desktop)
        g_free(desktop);

    client_msg(disp, win, "_NET_ACTIVE_WINDOW", 0, 0, 0, 0, 0);
    XMapRaised(disp, win);

    return EXIT_SUCCESS;
} /*}}}*/

#    undef p_verbose
#    undef gchar
#    undef gboolean
#    undef g_malloc
#    undef g_free

bool raise_window(Display* display, QWidget* win) {
    Window window = win->winId();
    int raised_via_xlib_error = XRaiseWindow(display, window);
    if (raised_via_xlib_error == 0)
        return true;

    int raised_via_ewmh_error = activate_window(display, window, true);
    return raised_via_ewmh_error == 0;
}


#endif
