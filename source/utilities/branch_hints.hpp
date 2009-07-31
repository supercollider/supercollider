//  $Id$
//
//  branch hints
//  Copyright (C) 2007 Tim Blechmann
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

//  $Revision$
//  $LastChangedRevision$
//  $LastChangedDate$
//  $LastChangedBy$

#ifndef BRANCH_HINTS_HPP
#define BRANCH_HINTS_HPP


namespace nova
{
    /** \brief hint for the branch prediction */
    inline bool likely(bool expr)
    {
#ifdef __GNUC__
        return __builtin_expect(expr, true);
#else
        return expr;
#endif
    }

    /** \brief hint for the branch prediction */
    inline bool unlikely(bool expr)
    {
#ifdef __GNUC__
        return __builtin_expect(expr, false);
#else
        return expr;
#endif
    }
}

#endif /* BRANCH_HINTS_HPP */
