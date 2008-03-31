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

#ifndef __CONTINUOUSPROCESS_HPP
#define __CONTINUOUSPROCESS_HPP

#include "libecs.hpp"
#include "Process.hpp"

namespace libecs
{

LIBECS_DM_CLASS( ContinuousProcess, Process )
{

public:

    LIBECS_DM_OBJECT_ABSTRACT( ContinuousProcess )
    {
        INHERIT_PROPERTIES( Process );
    }


    virtual ~ContinuousProcess()
    {
        ;
    }

    virtual const bool isContinuous() const
    {
        return true;
    }

protected:

};

} // libecs

#endif /* __ CONTINUOUSPROCESS_HPP */
