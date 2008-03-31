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
// written by Koichi Takahashi <shafi@e-cell.org>,
// E-Cell Project.
//
#ifdef HAVE_CONFIG_H
#include "ecell_config.h"
#endif /* HAVE_CONFIG_H */

#include <assert.h>

#include "PropertiedClassKind.hpp"


namespace libecs {

const PropertiedClassKind* PropertiedClassKind::last( 0 );
const PropertiedClassKind PropertiedClassKind::      NONE( _NONE    , "None" );
const PropertiedClassKind PropertiedClassKind::   STEPPER( _STEPPER , "Stepper" );
const PropertiedClassKind PropertiedClassKind::  VARIABLE( _VARIABLE, "Variable" );
const PropertiedClassKind PropertiedClassKind::   PROCESS( _PROCESS,  "Process" );
const PropertiedClassKind PropertiedClassKind::    SYSTEM( _SYSTEM,   "System" );

const PropertiedClassKind& PropertiedClassKind::get( const String& name )
{
    for ( const PropertiedClassKind* item = last; item; item = item->prev )
    {
        if ( item->name == name )
        {
            return *item;
        }
    }
}

const PropertiedClassKind& PropertiedClassKind::get( enum Code code )
{
    for ( const PropertiedClassKind* item = last; item; item = item->prev )
    {
        if ( item->code == code )
        {
            return *item;
        }
    }
}

const PropertiedClassKind&
PropertiedClassKind::fromEntityType( const EntityType& et )
{
    switch ( et.code )
    {
    case Entity::_PROCESS:
        return PropertiedClassKind::PROCESS;
    case Entity::_VARIABLE:
        return PropertiedClassKind::VARIABLE;
    case Entity::_SYSTEM:
        return PropertiedClassKind::SYSTEM;
    }
    THROW_EXCEPTION( ValueError,
        String( "no PropertiedClassKind counterpart for " )
        + static_cast< const String& >( pck ) );
}

} // namespace libecs
