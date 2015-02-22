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
   The supported event types of an event port.

   This is a kludge around Jack only supporting MIDI, particularly for OSC.
   This property is a comma-separated list of event types, currently "MIDI" or
   "OSC".  If this contains "OSC", the port may carry OSC bundles (first byte
   '#') or OSC messages (first byte '/').  Note that the "status byte" of both
   OSC events is not a valid MIDI status byte, so MIDI clients that check the
   status byte will gracefully ignore OSC messages if the user makes an
   inappropriate connection.
*/
#define JACKEY_EVENT_TYPES "http://jackaudio.org/metadata/event-types"

/**
   The type of an audio signal.

   This property allows audio ports to be tagged with a "meaning".  The value
   is a simple string.  Currently, the only type is "CV", for "control voltage"
   ports.  Hosts SHOULD be take care to not treat CV ports as audibile and send
   their output directly to speakers.  In particular, CV ports are not
   necessarily periodic at all and may have very high DC.
*/
#define JACKEY_SIGNAL_TYPE "http://jackaudio.org/metadata/signal-type"

/**
   The name of the icon for the subject (typically client).

   This is used for looking up icons on the system, possibly with many sizes or
   themes.  Icons should be searched for according to the freedesktop Icon
   Theme Specification:

   http://standards.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html
*/
#define JACKEY_ICON_NAME "http://jackaudio.org/metadata/icon-name"

/**
   Channel designation for a port.

   This allows ports to be tagged with a meaningful designation like "left",
   "right", "lfe", etc.

   The value MUST be a URI.  An extensive set of URIs for designating audio
   channels can be found at http://lv2plug.in/ns/ext/port-groups
*/
#define JACKEY_DESIGNATION "http://lv2plug.in/ns/lv2core#designation"

/**
   Order for a port.

   This is used to specify the best order to show ports in user interfaces.
   The value MUST be an integer.  There are no other requirements, so there may
   be gaps in the orders for several ports.  Applications should compare the
   orders of ports to determine their relative order, but must not assign any
   other relevance to order values.
*/
#define JACKEY_ORDER "http://jackaudio.org/metadata/order"
