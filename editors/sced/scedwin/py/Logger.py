# -*- coding: utf-8 -*-
# sced (SuperCollider mode for gedit)
#
# Copyright 2012 Jakob Leben
# Copyright 2009 Artem Popov and other contributors (see AUTHORS)
#
#    sced is free software: 
#    you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
# 
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
# 
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.


import gobject
import pango
import gtk
import pipe_util

class Logger:
    def __init__(self, pipe, log_view):
        self.__pipe = pipe
        self.__log_view = log_view

        tag_table = log_view.buffer.get_tag_table()
        self.__tag = gtk.TextTag()

        self.__good_tag = gobject.new(gtk.TextTag,
            weight                  = pango.WEIGHT_BOLD,
            foreground              = "darkgreen",
            paragraph_background    = "lightgreen")

        self.__bad_tag = gobject.new(gtk.TextTag,
            weight                  = pango.WEIGHT_BOLD,
            foreground              = "darkred",
            paragraph_background    = "pink")

        # for warnings, etc.
        self.__ugly_tag = gobject.new(gtk.TextTag,
            #weight                  = pango.WEIGHT_BOLD,
            foreground              = "red")

        tag_table.add(self.__tag)
        tag_table.add(self.__good_tag)
        tag_table.add(self.__bad_tag)
        tag_table.add(self.__ugly_tag)

        pipe_util.start_reader(pipe)
        self.__reader_task = gobject.timeout_add(20, self.__reader_tick)

    def __reader_tick(self):
        res = pipe_util.get_buffer()
        if(res is False):
            self.stop()
            return False
        if(res is not None):
            self.__append_to_buffer(unicode(res, 'mac_latin2'))
        return True

    def __append_to_buffer(self, text):
        # FIXME: coloring the text is broken, so don't try to do it

        buffer = self.__log_view.buffer

        buffer.insert(buffer.get_end_iter(), text)

        buffer.place_cursor(buffer.get_end_iter())
        self.__log_view.view.scroll_mark_onscreen(buffer.get_insert())

        return

        if text.startswith("ERROR"):
            tags = self.__bad_tag

        elif text.startswith("WARNING") or text.startswith("FAILURE"):
            tags = self.__ugly_tag

        elif text.startswith("StartUp done."):
            tags = self.__good_tag
        else:
            tags = self.__tag

        buffer.insert_with_tags(buffer.get_end_iter(), text.rstrip(), tags)
        buffer.insert(buffer.get_end_iter(), "\n")

        buffer.place_cursor(buffer.get_end_iter())
        self.__log_view.view.scroll_mark_onscreen(buffer.get_insert())

        # only required for thread-based implementation
        # return False

    def stop(self):
        if not self.__pipe.closed:
            pipe_util.stop_reader()
            self.__pipe.close()
            gobject.source_remove(self.__reader_task)

