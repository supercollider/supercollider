# sced (SuperCollider mode for gedit)
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

def is_block_beginning(s):
    s = "".join(s.split())
    # FIXME: clarify this later
    if s == "(" or s.startswith("(//") or s.startswith("(/*"):
        return True
    else:
        return False

def find_block(doc, where=None):
    if where is None:
        i1 = doc.get_iter_at_mark(doc.get_insert())
    else:
        i1 = where.copy()

    # move backward until a block beginning is found
    while True:
        i1.set_line_offset(0)

        i2 = i1.copy()
        i2.forward_to_line_end()

        if is_block_beginning(doc.get_text(i1, i2)):
            break

        if not i1.backward_line():
            raise RuntimeError("Couldn't find where code block starts!")

    i2 = i1.copy()
    count = 1

    line_comment = False
    block_comment = 0

    # move forward to the end of the block
    while True:
        if not i2.forward_char():
            raise RuntimeError("Couldn't find where code block ends!")

        char = i2.get_char()

        i3 = i2.copy()
        i3.forward_chars(2)
        ct = i2.get_text(i3)

        if ct == "*/":
            block_comment -= 1
        elif ct == "/*":
            block_comment += 1
        elif ct == "//":
            line_comment = True
        elif char == "\n" and line_comment:
            line_comment = False

        if not block_comment and not line_comment:
            if char == "(":
                count += 1
            elif char == ")":
                count -= 1

        if count == 0:
            break

    # XXX: include 2 more characters just in case "where" is near the end
    i2.forward_chars(2)

    if where.in_range(i1, i2):
        return i1, i2
    else:
        raise RuntimeError("Couldn't find code block!")



import re

def scpred(c, *args):
    if re.match("[A-Za-z0-9_]", c):
        return False
    return True

def find_word(doc, where=None):
    if where is None:
        i1 = doc.get_iter_at_mark(doc.get_insert())
    else:
        i1 = where.copy()

    i1.backward_find_char(scpred)
    i1.forward_char() # <-- FIXME: forward should not normally be required

    i2 = i1.copy()
    i2.forward_find_char(scpred)

    return i1, i2
