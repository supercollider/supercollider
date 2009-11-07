# -*- coding: utf-8 -*-
#
# code generation system for simd framework
# Copyright (C) 2008, 2009 Tim Blechmann <tim@klingt.org>
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

from string import Template

def generate_header(header_string):
    header = Template("""//  $header
//  Copyright (C) 2008, 2009 Tim Blechmann <tim@klingt.org>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.
//
//  implemented as part of nova
""")
    return header.substitute(header = header_string)


def generate_prolog(guard, headers, namespace = 'nova'):
    includes = ""
    for header in headers:
        includes = includes + ("#include %s\n" % header)

    prolog = Template("""
#ifndef $guard
#define $guard

$includes
namespace $namespace {
""")
    return prolog.substitute(guard = guard, includes = includes, namespace = namespace)

def generate_epilog(guard, namespace = 'nova'):
    epilog = Template("""
} /* namespace $namespace */

#endif /* $guard */
""")
    return epilog.substitute(guard = guard, namespace = namespace)


def generate_file(body, header_string, guard, headers, namespace = 'nova'):
    header = generate_header(header_string)
    prolog = generate_prolog(guard, headers, namespace)
    epilog = generate_epilog(guard, namespace)

    return header + prolog + body + epilog

