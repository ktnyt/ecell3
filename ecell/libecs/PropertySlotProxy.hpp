//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//        This file is part of E-CELL Simulation Environment package
//
//                Copyright (C) 1996-2002 Keio University
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//
// E-CELL is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// E-CELL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with E-CELL -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
//END_HEADER
//
// written by Kouichi Takahashi <shafi@e-cell.org> at
// E-CELL Project, Lab. for Bioinformatics, Keio University.
//

#ifndef __PROPERTYSLOTPROXY_HPP
#define __PROPERTYSLOTPROXY_HPP

#include <iostream>
#include <functional>

#include "libecs.hpp"
#include "Util.hpp"
#include "PropertySlot.hpp"
#include "convertTo.hpp"

#include "Polymorph.hpp"

namespace libecs
{


  /** @addtogroup property
      
  @ingroup libecs
  @{
  */

  /** @file */


  class PropertySlotProxy
  {

  public:

    PropertySlotProxy()
    {
      ; // do nothing
    }
    
    virtual ~PropertySlotProxy();

    virtual void setPolymorph( PolymorphCref ) = 0;
    virtual const Polymorph getPolymorph() const = 0;
    
    virtual void setReal( RealCref real ) = 0;
    virtual const Real getReal() const = 0;

    virtual void setInteger( IntegerCref real ) = 0;
    virtual const Integer getInteger() const = 0;

    virtual void setString( StringCref string ) = 0;
    virtual const String getString() const = 0;

    virtual const bool isSetable() const = 0;
    virtual const bool isGetable() const = 0;

    template < typename Type >
    inline void set( const Type& aValue )
    {
      DefaultSpecializationInhibited();
    }

    template < typename Type >
    inline const Type get() const
    {
      DefaultSpecializationInhibited();
    }


  protected:

  };



  template <>
  inline void PropertySlotProxy::set( PolymorphCref aValue )
  {
    setPolymorph( aValue );
  }

  template <>
  inline void PropertySlotProxy::set( RealCref aValue )
  {
    setReal( aValue );
  }

  template <>
  inline void PropertySlotProxy::set( IntegerCref aValue )
  {
    setInteger( aValue );
  }

  template <>
  inline void PropertySlotProxy::set( StringCref aValue )
  {
    setString( aValue );
  }

  template <>
  inline const Polymorph PropertySlotProxy::get() const
  {
    return getPolymorph();
  }

  template <>
  inline const String PropertySlotProxy::get() const
  {
    return getString();
  }

  template <>
  inline const Real PropertySlotProxy::get() const
  {
    return getReal();
  }


  template <>
  inline const Integer PropertySlotProxy::get() const
  {
    return getInteger();
  }



  template
  < 
    class T
  >
  class ConcretePropertySlotProxy
    :
    public PropertySlotProxy
  {

  public:

    typedef PropertySlot<T> PropertySlot_;
    DECLARE_TYPE( PropertySlot_, PropertySlot );

    ConcretePropertySlotProxy( T& anObject, 
			       PropertySlotRef aPropertySlot )
      :
      theObject( anObject ),
      thePropertySlot( aPropertySlot )
    {
      ; // do nothing
    }

    virtual ~ConcretePropertySlotProxy()
    {
      ; // do nothing
    }


#define _PROPERTYSLOT_SETMETHOD( TYPE )\
    virtual void set ## TYPE( TYPE ## Cref aValue )\
    {\
      thePropertySlot.set ## TYPE( theObject, aValue );\
    }

#define _PROPERTYSLOT_GETMETHOD( TYPE )\
    virtual const TYPE get ## TYPE() const\
    {\
      return thePropertySlot.get ## TYPE( theObject );\
    }

    _PROPERTYSLOT_SETMETHOD( Polymorph );
    _PROPERTYSLOT_GETMETHOD( Polymorph );

    _PROPERTYSLOT_SETMETHOD( Real );
    _PROPERTYSLOT_GETMETHOD( Real );

    _PROPERTYSLOT_SETMETHOD( Integer );
    _PROPERTYSLOT_GETMETHOD( Integer );

    _PROPERTYSLOT_SETMETHOD( String );
    _PROPERTYSLOT_GETMETHOD( String );

#undef _PROPERTYSLOT_SETMETHOD
#undef _PROPERTYSLOT_GETMETHOD


    virtual const bool isSetable() const
    {
      return thePropertySlot.isSetable();
    }

    virtual const bool isGetable() const
    {
      return thePropertySlot.isGetable();
    }

  private:

    ConcretePropertySlotProxy();

    T&               theObject;
    PropertySlotRef  thePropertySlot;

  };



  class PropertySlotProxyLoggerAdapter
    :
    public LoggerAdapter
  {

  public:

    PropertySlotProxyLoggerAdapter( PropertySlotProxyPtr aPropertySlotProxy )
      :
      thePropertySlotProxy( aPropertySlotProxy )
    {
      ; // do nothing
    }

    virtual ~PropertySlotProxyLoggerAdapter()
    {
      delete thePropertySlotProxy;
    }

    virtual const Real getValue() const
    {
      return thePropertySlotProxy->getReal();
    }

  private:

    PropertySlotProxyPtr thePropertySlotProxy;

  };

  /*@}*/

}


#endif /* __PROPERTYSLOT_HPP */
