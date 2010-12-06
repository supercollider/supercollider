# -*- coding: utf-8 -*-
# $Id$
#
# SCons builder to combine c++ source files
# Copyright (C) 2007 Tim Blechmann
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.

# $Revision$
# $LastChangedRevision$
# $LastChangedDate$
# $LastChangedBy$

import SCons.Action
import SCons.Builder

def build_it(target, source, env):
    """generate a file, that's including all sources"""

    out = ""
    for src in source:
        out += "#include \"%s\"\n" % src.abspath

    open(str(target[0]), 'w').write(out)
    return 0

def generate(env):
    env.Append(BUILDERS = {
        'Combine': env.Builder(
        action = SCons.Action.Action(build_it, "generating '$TARGET'"),
        target_factory = env.fs.File,
        )
        })

def exists(env):
    return True
