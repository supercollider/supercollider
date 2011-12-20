/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2011 Tim Blechmann. All rights reserved.

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

#ifndef FUNCTION_ATTRIBUTES_H
#define FUNCTION_ATTRIBUTES_H

#ifdef __GNUC__

#define CONST __attribute__((const))
#define PURE __attribute__((pure))
#define MALLOC __attribute__((malloc))
#define HOT __attribute__((hot))
#define COLD __attribute__((cold))
#define FLATTEN __attribute__((flatten))

#endif

#ifdef __clang__
#undef HOT
#undef FLATTEN

#define HOT /*HOT*/
#define FLATTEN /*FLATTEN*/
#endif

#ifdef __PATHCC__
#undef HOT
#undef FLATTEN

#define HOT /*HOT*/
#define FLATTEN /*FLATTEN*/
#endif


#ifndef PURE
#define PURE /*PURE*/
#endif

#ifndef CONST
#define CONST /*CONST*/
#endif

#ifndef MALLOC
#define MALLOC /*MALLOC*/
#endif

#ifndef HOT
#define HOT /*HOT*/
#endif

#ifndef COLD
#define COLD /*COLD*/
#endif

#endif /* FUNCTION_ATTRIBUTES_H */
