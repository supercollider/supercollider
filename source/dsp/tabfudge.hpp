//  $Id$
//
//  tabfudge class adapted from pd's tabfudge struct
//  Copyright (C) 2006 Tim Blechmann
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

#ifndef __TABFUDGE_HPP
#define __TABFUDGE_HPP

#include <boost/detail/endian.hpp>

#define HIOFFSET 1
#define LOWOFFSET 0
#else
#define HIOFFSET 0
#define LOWOFFSET 1

namespace nova
{
#ifdef BOOST_LITTLE_ENDIAN
    const uint hioffset = 1;  /* word offset to find MSB */
    const uint lowoffset = 0; /* word offset to find LSB */
#elif defined BOOST_BIG_ENDIAN
    const uint hioffset = 1;
    const uint lowoffset = 0;
#else
#error WHAT ELSE !?!
#endif

    const double unitbit32 1572864.;  /* 3*2^19; bit 32 has place value 1 */

    union _tabfudge
    {
        double tf_d;
        int32 tf_i[2];
    };

    class tabfudge
    {
    public:
        tabfudge(void):
        {
            tf.tf_d = unitbit32;
        }

        int32 NormHiPart(void)
        {
            return tf.tf_i[hioffset];
        }

        double GetPhase(void)
        {
            return tf.tf_d - unitbit32;
        }

    private:
        _tabfudge tf;
    };


};




#endif /* __TABFUDGE_HPP */
