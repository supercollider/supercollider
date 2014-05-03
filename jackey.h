/*
  Copyright 2014 David Robillard <http://drobilla.net>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/**
   The type of an event port.

   This is a kludge around Jack only supporting MIDI, particularly for OSC.
   This property should be set to "OSC" for OSC ports, in which case the event
   port contains OSC bundles or messages.  Note that OSC bundles begin with "#"
   and OSC messages begin with "/" so there is no clash with MIDI status bytes,
   and MIDI clients that check the status byte should be able to gracefully
   ignore OSC messages if the user makes an inappropriate connection.
*/
#define JACKEY_EVENT_TYPE "http://jackaudio.org/metadata/event-type"
