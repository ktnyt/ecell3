#include "libecs.hpp"

#include "ContinuousProcess.hpp"

USE_LIBECS;

LIBECS_DM_CLASS( OrderedBiBiFluxProcess, ContinuousProcess )
{

 public:

  LIBECS_DM_OBJECT( OrderedBiBiFluxProcess, Process )
    {
      INHERIT_PROPERTIES( ContinuousProcess );

      PROPERTYSLOT_SET_GET( Real, KcF );
      PROPERTYSLOT_SET_GET( Real, KcR );
      PROPERTYSLOT_SET_GET( Real, Keq );
      PROPERTYSLOT_SET_GET( Real, KmS0 );
      PROPERTYSLOT_SET_GET( Real, KmS1 );
      PROPERTYSLOT_SET_GET( Real, KmP0 );
      PROPERTYSLOT_SET_GET( Real, KmP1 );
      PROPERTYSLOT_SET_GET( Real, KiS0 );
      PROPERTYSLOT_SET_GET( Real, KiS1 );
      PROPERTYSLOT_SET_GET( Real, KiP0 );
      PROPERTYSLOT_SET_GET( Real, KiP1 );
    }
  

  OrderedBiBiFluxProcess()
    :
    KcF( 0.0 ),
    KcR( 0.0 ),
    Keq( 1.0 ),
    KmS0( 1.0 ),
    KmS1( 1.0 ),
    KmP0( 1.0 ),
    KmP1( 1.0 ),
    KiS0( 1.0 ),
    KiS1( 1.0 ),
    KiP0( 1.0 ),
    KiP1( 1.0 )
    {
      ; // do nothing
    }
  
  SIMPLE_SET_GET_METHOD( Real, KcF );
  SIMPLE_SET_GET_METHOD( Real, KcR );
  SIMPLE_SET_GET_METHOD( Real, Keq );
  SIMPLE_SET_GET_METHOD( Real, KmS0 );
  SIMPLE_SET_GET_METHOD( Real, KmS1 );
  SIMPLE_SET_GET_METHOD( Real, KmP0 );
  SIMPLE_SET_GET_METHOD( Real, KmP1 );
  SIMPLE_SET_GET_METHOD( Real, KiS0 );
  SIMPLE_SET_GET_METHOD( Real, KiS1 );
  SIMPLE_SET_GET_METHOD( Real, KiP0 );
  SIMPLE_SET_GET_METHOD( Real, KiP1 );
    
  virtual void initialize()
    {
      Process::initialize();
      S0 = getVariableReference( "S0" );
      S1 = getVariableReference( "S1" );
      P0 = getVariableReference( "P0" );
      P1 = getVariableReference( "P1" );
      C0 = getVariableReference( "C0" );
    }

  virtual void process()
    {
      Real S0Concentration = S0.getMolarConc();
      Real S1Concentration = S1.getMolarConc();
      Real P0Concentration = P0.getMolarConc();
      Real P1Concentration = P1.getMolarConc();
      
      Real Denom( KcR * KiS0 * KmS1
		  + KcR * KmS1 * S0Concentration + KcR * KmS0 
		  * S1Concentration + KcF * KmP1 * P0Concentration / Keq
		  + KcF * KmP0 * P1Concentration / Keq + KcR 
		  * S0Concentration * S1Concentration
		  + KcF * KmP1 * S0Concentration * P0Concentration 
		  / (Keq * KiS0) + KcF * P0Concentration 
		  * P1Concentration / Keq
		  + KcR * KmS0 * S1Concentration * P1Concentration 
		  / KiP1 + KcR * S0Concentration * S1Concentration 
		  * P0Concentration /KiP0
		  + KcF * S1Concentration * P0Concentration 
		  * P1Concentration / (KiS1 * Keq) );
      
      Real velocity( KcF * KcR * C0.getValue()
		     * ( S0Concentration * S1Concentration 
			 - P0Concentration * P1Concentration / Keq ) 
		     / Denom );
      
      setFlux( velocity );
    }

 protected:

  Real KcF;
  Real KcR;
  Real Keq;

  Real KmS0;
  Real KmS1;
  Real KmP0;
  Real KmP1;

  Real KiS0;
  Real KiS1;
  Real KiP0;
  Real KiP1;

  VariableReference S0;
  VariableReference S1;
  VariableReference P0;
  VariableReference P1;
  VariableReference C0;
  
};

LIBECS_DM_INIT( OrderedBiBiFluxProcess, Process );
