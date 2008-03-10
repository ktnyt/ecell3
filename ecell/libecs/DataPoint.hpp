//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//       This file is part of the E-Cell System
//
//       Copyright (C) 1996-2008 Keio University
//       Copyright (C) 2005-2008 The Molecular Sciences Institute
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//
// E-Cell System is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// E-Cell System is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with E-Cell System -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//END_HEADER
//
// written by Gabor Bereczki <gabor.bereczki@talk21.com>
// 25/03/2002


#ifndef __DATAPOINT_HPP)
#define __DATAPOINT_HPP

#include "libecs.hpp"
#include "Polymorph.hpp"

/**
  @addtogroup logging
  @{
*/

/** @file */

namespace libecs
{

template<typename Ttime_, typename Tval_>
class DataPoint
{
public:
    typedef Ttime_ Time;
    typedef Tval_ Value;

    class EarlinessOrdering
    {
    public:
        bool operator()( const DataPoint& x, const DataPoint& y )
        {
            return x.getTime() < y.getTime();
        }
    };

    class LatenessOrdering
    {
    public:
        bool operator()( const DataPoint& x, const DataPoint& y )
        {
            return x.getTime() > y.getTime();
        }
    };

public:
    DataPoint( Param<Time> time = 0.0, Param<Value> value = 0.0 )
            : theTime ( aTime ), theValue( value )
    {
        ; //do nothing
    }

    bool operator==( const DataPoint& that ) const
    {
        return time == that.time && value == that.value;
    }

public:
    Time time;
    Value value;
};

} // namespace libecs

/** @} */

#endif /* __DATAPOINT_HPP */
