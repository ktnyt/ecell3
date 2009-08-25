//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//       This file is part of the E-Cell System
//
//       Copyright (C) 1996-2009 Keio University
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

#include "Util.hpp"
#include "VariableReference.hpp"
#include "Stepper.hpp"
#include "FullID.hpp"
#include "Exceptions.hpp"
#include "Variable.hpp"
#include "Model.hpp"

#include "Process.hpp"

#include <boost/format.hpp>
#include <boost/bind.hpp>

namespace libecs
{

LIBECS_DM_INIT_STATIC( Process, Process );

SET_METHOD_DEF( Polymorph, VariableReferenceList, Process )
{
    if ( value.getType() != PolymorphValue::TUPLE )
    {
        THROW_EXCEPTION_INSIDE( ValueError,
                                asString() + ": argument must be a tuple" );
    }

    typedef boost::range_const_iterator< PolymorphValue::Tuple >::type const_iterator;
    PolymorphValue::Tuple const& aTuple( value.as< PolymorphValue::Tuple const& >() );

    for ( const_iterator i( boost::begin( aTuple ) ); i != boost::end( aTuple );
          ++i )
    {
        if ( (*i).getType() != PolymorphValue::TUPLE )
        {
            THROW_EXCEPTION_INSIDE( ValueError,
                                    asString() + ": every element of the tuple "
                                    "must also be a tuple" );
        }
        PolymorphValue::Tuple const& anElem( (*i).as< PolymorphValue::Tuple const & >() );
        if ( anElem.size() < 2 )
        {
            THROW_EXCEPTION_INSIDE( ValueError,
                                    asString() + ": each element of the tuple "
                                    "must have at least 4 elements" );
        }
        registerVariableReference(
            anElem[ 0 ].as< String >(),
            FullID( anElem[ 1 ].as< String >() ),
            anElem.size() > 2 ? anElem[ 2 ].as< Integer >(): 0l,
            ( anElem.size() > 3 ?
                anElem[ 3 ].as< Integer >(): 1l ) != 0 ? true: false );
    }
}

GET_METHOD_DEF( Polymorph, VariableReferenceList, Process )
{
    PolymorphVector aVector;
    aVector.reserve( theVariableReferenceVector.size() );

    for( VariableReferenceVectorConstIterator i(
            theVariableReferenceVector.begin() );
         i != theVariableReferenceVector.end() ; ++i )
    {
        VariableReferenceCref aVariableReference( *i );
        FullID aFullID( aVariableReference.getVariable()->getFullID() );
        aFullID.setEntityType( EntityType::NONE );

        aVector.push_back( boost::tuple< String, String, Integer, Integer >(
            aVariableReference.getName(),
            aFullID.asString(),
            aVariableReference.getCoefficient(),
            aVariableReference.isAccessor() ) );
    }

    return aVector;
}

SAVE_METHOD_DEF( Polymorph, VariableReferenceList, Process )
{
    PolymorphVector aVector;
    aVector.reserve( theVariableReferenceVector.size() );

    for( VariableReferenceVectorConstIterator i(
            theVariableReferenceVector.begin() );
         i != theVariableReferenceVector.end() ; ++i )
    {
        VariableReferenceCref aVariableReference( *i );

        // (1) Variable reference name

        // convert back all variable reference ellipses to the default '_'.
        String aReferenceName( aVariableReference.getName() );

        if( VariableReference::isEllipsisNameString( aReferenceName ) )
        {
            aReferenceName = VariableReference::DEFAULT_NAME;
        }

        // (2) FullID

        FullID aFullID( aVariableReference.getVariable()->getFullID() );
        aFullID.setEntityType( EntityType::NONE );


        // (3) Coefficient and (4) IsAccessor
        const Integer aCoefficient( aVariableReference.getCoefficient() );
        const bool        anIsAccessorFlag( aVariableReference.isAccessor() );

        // include both if IsAccessor is non-default (not true).
        if( anIsAccessorFlag != true )
        {
            aVector.push_back( boost::tuple< String, String, Integer, Integer >(
                aReferenceName,
                aFullID.asString(),
                aCoefficient,
                static_cast<Integer>( anIsAccessorFlag ) ) );
        }
        else
        {
            // output only the coefficient if IsAccessor has a 
            // default value, and the coefficient is non-default.
            if( aCoefficient != 0 )
            {
                aVector.push_back( boost::tuple< String, String, Integer >(
                    aReferenceName,
                    aFullID.asString(),
                    aCoefficient ) );
            }
            else
            {
                aVector.push_back( boost::tuple< String, String >(
                    aReferenceName,
                    aFullID.asString() ) );
            }
        }
    }

    return aVector;
}


Process::Process() 
    : theZeroVariableReferenceIterator( theVariableReferenceVector.end() ),
      thePositiveVariableReferenceIterator( theVariableReferenceVector.end() ),
      theActivity( 0.0 ),
      thePriority( 0 ),
      theStepper( NULLPTR ),
      theNextSerial( 1 )
{
    ; // do nothing
}


void Process::dispose()
{
    if ( !disposed_ )
    {
        if( getStepper() )
        {
            getStepper()->unregisterProcess( this );
        }
    }

    Entity::dispose();
}


Process::~Process()
{
}


SET_METHOD_DEF( String, StepperID, Process )
{
    StepperPtr aStepperPtr( getModel()->getStepper( value ) );

    setStepper( aStepperPtr );
}

GET_METHOD_DEF( String, StepperID, Process )
{
    Stepper const* aStepper( getStepper() );
    return aStepper ? aStepper->getID(): "";
}


void Process::setStepper( StepperPtr const aStepper )
{
    if( theStepper != aStepper )
    {
        if( aStepper != NULLPTR )
        {
            aStepper->registerProcess( this );
        }
        else
        {
            theStepper->unregisterProcess( this );
        }

        theStepper = aStepper;
    }
}

VariableReferenceCref
Process::getVariableReference( IntegerParam anID ) const
{
    VariableReferenceVectorConstIterator anIterator(
            findVariableReference( anID ) );

    if( anIterator != theVariableReferenceVector.end() )
    {
        return *anIterator;
    }
    else
    {
        THROW_EXCEPTION_INSIDE( NotFound,
                                asString() + ": VariableReference #"
                                + stringCast( anID )
                                + " not found in this Process" );
    }
}

VariableReferenceCref
Process::getVariableReference( StringCref aVariableReferenceName ) const
{
    VariableReferenceVectorConstIterator anIterator(
            findVariableReference( aVariableReferenceName ) );

    if( anIterator != theVariableReferenceVector.end() )
    {
        return *anIterator;
    }
    else
    {
        THROW_EXCEPTION_INSIDE( NotFound,
                                asString() + ": VariableReference ["
                                + aVariableReferenceName
                                + "] not found in this Process" );
    }
}

void Process::removeVariableReference( IntegerParam anID )
{
    VariableReferenceVector::iterator i( findVariableReference( anID ) );
    if ( i == theVariableReferenceVector.end() )
    {
        THROW_EXCEPTION_INSIDE( NotFound,
                                asString() + ": VariableReference #"
                                + stringCast( anID )
                                + " not found in this Process" );
    }
    theVariableReferenceVector.erase( i );
}

void Process::removeVariableReference( StringCref aName )
{
    bool aIsRemoved( false );

    for ( VariableReferenceVector::size_type
            i( 0 ), e ( theVariableReferenceVector.size() );
            i < e; )
    {
        if ( theVariableReferenceVector[ i ].getName() == aName )
        {
            theVariableReferenceVector.erase( theVariableReferenceVector.begin() + i );
            aIsRemoved = true;
        }
        else
        {
            ++i;
        }
    }

    if ( !aIsRemoved )
    {
        THROW_EXCEPTION_INSIDE( NotFound,
                                asString() + ": VariableReference ["
                                + aName
                                + "] not found in this Process" );
    }
}

void Process::removeVariableReference( Variable const* aVariable )
{
    bool aIsRemoved( false );

    for ( VariableReferenceVector::size_type
            i( 0 ), e ( theVariableReferenceVector.size() );
            i < e; )
    {
        if ( theVariableReferenceVector[ i ].getVariable() == aVariable )
        {
            theVariableReferenceVector.erase( theVariableReferenceVector.begin() + i );
            aIsRemoved = true;
        }
        else
        {
            ++i;
        }
    }

    if ( !aIsRemoved )
    {
        THROW_EXCEPTION_INSIDE( NotFound,
                                asString() + ": VariableReference ["
                                + aVariable->asString()
                                + "] not found in this Process" );
    }
}

const Integer Process::registerVariableReference( FullID const& aFullID,
                                                  IntegerParam aCoefficient,
                                                  const bool isAccessor )
{
    theVariableReferenceVector.push_back(
            VariableReference(
                theNextSerial, aFullID, aCoefficient, isAccessor ) );
    return theNextSerial++;
}


const Integer Process::registerVariableReference( StringCref aName,
                                                  FullID const& aFullID,
                                                  IntegerParam aCoefficient,
                                                  const bool isAccessor )
{
    theVariableReferenceVector.push_back(
            VariableReference(
                theNextSerial, aFullID, aCoefficient, isAccessor ) );
    theVariableReferenceVector.back().setName( aName );
    return theNextSerial++;
}

const Integer Process::registerVariableReference( Variable* aVariable,
                                                  IntegerParam aCoefficient,
                                                  const bool isAccessor )
{
    theVariableReferenceVector.push_back(
            VariableReference(
                theNextSerial, aVariable, aCoefficient, isAccessor ) );
    return theNextSerial++;
}


const Integer Process::registerVariableReference( StringCref aName,
                                                  Variable* aVariable,
                                                  IntegerParam aCoefficient,
                                                  const bool isAccessor )
{
    theVariableReferenceVector.push_back(
            VariableReference(
                theNextSerial, aVariable, aCoefficient, isAccessor ) );
    theVariableReferenceVector.back().setName( aName );
    return theNextSerial++;
}


void Process::resolveVariableReferences()
{
    if ( !getSuperSystem() )
    {
        THROW_EXCEPTION_INSIDE( IllegalOperation,
                                asString() + ": process is not associated to "
                                "any system" );
    }

    Integer anEllipsisNumber( 0 );

    for ( VariableReferenceVector::iterator
            i( theVariableReferenceVector.begin() ),
            e( theVariableReferenceVector.end() );
          i != e; ++i )
    {
        VariableReference& aVarRef( *i );

        if( aVarRef.isDefaultName() )
        {
            aVarRef.setName( VariableReference::ELLIPSIS_PREFIX + 
                ( boost::format( "%03d" ) % anEllipsisNumber ).str() );
            anEllipsisNumber++;
        }

        // relative search; allow relative systempath
        FullID const& aFullID( aVarRef.getFullID() );
        SystemPtr aSystem( getSuperSystem()->getSystem( aFullID.getSystemPath() ) );
        if ( !aSystem )
        {
            THROW_EXCEPTION_INSIDE( IllegalOperation,
                                    asString() + ": system path ["
                                    + aFullID.getSystemPath().asString()
                                    + "] could not be resolved" );
        }

        if ( !aVarRef.getVariable() )
        {
            aVarRef.setVariable( aSystem->getVariable( aFullID.getID() ) );
        }
        else
        {
            aVarRef.setFullID( aVarRef.getVariable()->getFullID() );
        }
    }
}

void Process::updateVariableReferenceVector()
{
    // first sort by reference name
    std::sort( theVariableReferenceVector.begin(), 
               theVariableReferenceVector.end(), 
               VariableReference::Less() );

    // find the first VariableReference whose coefficient is 0,
    // and the first VariableReference whose coefficient is positive.
    std::pair <VariableReferenceVectorIterator,
               VariableReferenceVectorIterator> aZeroRange(
        std::equal_range( theVariableReferenceVector.begin(), 
                          theVariableReferenceVector.end(), 
                          0, VariableReference::CoefficientLess() ) );

    theZeroVariableReferenceIterator     = aZeroRange.first;
    thePositiveVariableReferenceIterator = aZeroRange.second;
}



VariableReferenceVectorIterator 
Process::findVariableReference( StringCref aVariableReferenceName )
{
    // well this is a linear search.. but this won't be used during simulation.
    for( VariableReferenceVectorIterator i(
            theVariableReferenceVector.begin() );
         i != theVariableReferenceVector.end(); ++i )
    {
        if( (*i).getName() == aVariableReferenceName )
        {
            return i;
        }
    }

    return theVariableReferenceVector.end();
}


VariableReferenceVectorConstIterator 
Process::findVariableReference( StringCref aVariableReferenceName ) const
{
    // well this is a linear search.. but this won't be used during simulation.
    for( VariableReferenceVector::const_iterator i(
            theVariableReferenceVector.begin() );
         i != theVariableReferenceVector.end(); ++i )
    {
        if( (*i).getName() == aVariableReferenceName )
        {
            return i;
        }
    }

    return theVariableReferenceVector.end();
}

VariableReferenceVectorIterator 
Process::findVariableReference( IntegerParam anID )
{
    // well this is a linear search.. but this won't be used during simulation.
    for( VariableReferenceVectorIterator i(
            theVariableReferenceVector.begin() );
         i != theVariableReferenceVector.end(); ++i )
    {
        if( (*i).getSerial() == anID )
        {
            return i;
        }
    }

    return theVariableReferenceVector.end();
}


VariableReferenceVectorConstIterator 
Process::findVariableReference( IntegerParam anID ) const
{
    // well this is a linear search.. but this won't be used during simulation.
    for( VariableReferenceVector::const_iterator i(
            theVariableReferenceVector.begin() );
         i != theVariableReferenceVector.end(); ++i )
    {
        if( (*i).getSerial() == anID )
        {
            return i;
        }
    }

    return theVariableReferenceVector.end();
}



void Process::declareUnidirectional()
{
    std::for_each( thePositiveVariableReferenceIterator,
                   theVariableReferenceVector.end(),
                   boost::bind2nd( boost::mem_fun_ref( &VariableReference::setIsAccessor ), false ) );
}



const bool Process::isDependentOn( const ProcessCptr aProcessPtr ) const
{
    VariableReferenceVectorCref aVariableReferenceVector(
            aProcessPtr->getVariableReferenceVector() );
    
    for( VariableReferenceVectorConstIterator i(
            theVariableReferenceVector.begin() );
         i != theVariableReferenceVector.end() ; ++i )
    {
        VariableReferenceCref aVariableReference1( *i );

        for( VariableReferenceVectorConstIterator j(
                aVariableReferenceVector.begin() );
             j != aVariableReferenceVector.end(); ++j )
        {
            VariableReferenceCref aVariableReference2( *j );
            
            if( aVariableReference1.getVariable() == 
                    aVariableReference2.getVariable() && 
                aVariableReference1.isAccessor() && 
                aVariableReference2.isMutator() )
            {
                return true;
            }
        }
    }

    return false;
}

void Process::preinitialize()
{
    resolveVariableReferences();
    updateVariableReferenceVector(); 
} 

void Process::initialize()
{
    if( getStepper() == NULLPTR )
    {
        THROW_EXCEPTION_INSIDE( InitializationFailed,
                                "No stepper is connected with [" +
                                asString() + "]." );
    }
}

void Process::addValue( VariableReference const& aVarRef, RealParam value )
{
    aVarRef.getVariable()->addValue( aVarRef.getCoefficient() * value );
}

void Process::addValue( RealParam aValue )
{
    setActivity( aValue );

    // Increase or decrease variables, skipping zero coefficients.
    std::for_each( theVariableReferenceVector.begin(),
                   theZeroVariableReferenceIterator,
                   boost::bind( &VariableReference::addValue, _1, aValue ) );

    std::for_each( thePositiveVariableReferenceIterator,
                   theVariableReferenceVector.end(),
                   boost::bind( &VariableReference::addValue, _1, aValue ) );
}

} // namespace libecs
