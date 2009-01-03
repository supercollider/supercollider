# -*- coding: utf-8 -*-

import gobject
import pango
import gtk

class Logger:
    def __init__(self, pipe, log_view):
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

        self.__watch_id = gobject.io_add_watch(pipe,
            gobject.IO_IN |
            gobject.IO_PRI |
            gobject.IO_ERR |
            gobject.IO_HUP,
            self.__on_output)

    def __on_output(self, source, condition):
        s = source.readline()
        if s == '':
            self.__append_to_buffer("EOF")
            return False

        # FIXME: A workaround for a mac character
        self.__append_to_buffer(unicode(s, 'mac_latin2'))

        if condition & gobject.IO_ERR:
            s = source.read() # can safely read until EOF here
            self.__append_to_buffer(unicode(s, 'mac_latin2'))
            return False
        elif condition & gobject.IO_HUP:
            s = source.read() # can safely read until EOF here
            self.__append_to_buffer(unicode(s, 'mac_latin2'))
            return False
        elif condition != 1:
            return False
        return True

    def __append_to_buffer(self, text):
        buffer = self.__log_view.buffer

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
        gobject.source_remove(self.__watch_id)
