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

#ifndef __ENTITYTYPE_HPP 
#define __ENTITYTYPE_HPP 

#include "libecs.hpp"


namespace libecs
{

  /** @addtogroup identifier
   *
   *@{
   */

  /** @file */


  /**

  */

  class EntityType
  {

  public:
    
    enum Type
      {
	ENTITY    = 1,
	VARIABLE = 2,
	PROCESS   = 3,
	SYSTEM    = 4
      };

    EntityType( StringCref typestring );

    EntityType( const Int number );

    EntityType( const Type type )
      :
      theType( type )
    {
      ; // do nothing
    }

    EntityType( EntityTypeCref primitivetype )
      :
      theType( primitivetype.getType() )
    {
      ; // do nothing
    }

    EntityType()
      :
      theType( ENTITY )
    {
    }

      
    StringCref getString() const;

    operator StringCref() const
    {
      return getString();
    }

    const Type& getType() const
    {
      return theType;
    }

    operator const Type&() const
    {
      return getType();
    }

    //    operator const Int&() const
    //    {
    //      return static_cast<const Int&>( getType() ); 
    //    }

    bool operator<( EntityTypeCref rhs ) const
    {
      return theType < rhs.getType();
    }

    bool operator<( const Type rhs ) const
    {
      return theType < rhs;
    }

    bool operator==( EntityTypeCref rhs ) const
    {
      return theType == rhs.getType();
    }

    bool operator==( const Type rhs ) const
    {
      return theType == rhs;
    }


    inline static StringCref  EntityTypeStringOfEntity()
    {
      const static String aString( "Entity" );
      return aString;
    }

    static StringCref  EntityTypeStringOfProcess();
    
    static StringCref  EntityTypeStringOfVariable();
    
    static StringCref  EntityTypeStringOfSystem();

  private:

    Type theType;

  };

  /*@}*/

} // namespace libecs


#endif /* __ENTITYTYPE_HPP */
