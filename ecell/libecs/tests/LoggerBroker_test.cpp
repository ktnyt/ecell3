//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//       This file is part of the E-Cell LoggerBroker
//
//       Copyright (C) 1996-2008 Keio University
//       Copyright (C) 2005-2008 The Molecular Sciences Institute
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//
// E-Cell LoggerBroker is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// E-Cell LoggerBroker is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with E-Cell LoggerBroker -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
//END_HEADER
//
// written by Koichi Takahashi
// modified by Moriyoshi Koizumi
//

#define BOOST_TEST_MODULE "LoggerBroker"

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/preprocessor/stringize.hpp>

#include "Model.hpp"
#include "LoggerBroker.hpp"
#include "Variable.hpp"
#include "System.hpp"
#include "Exceptions.hpp"
#include "dmtool/ModuleMaker.hpp"
#include "dmtool/DMObject.hpp"

#include <iostream>


BOOST_AUTO_TEST_CASE(testNonExistent)
{
    using namespace libecs;

    StaticModuleMaker< EcsObject > mmaker;
    Model model( mmaker );

    BOOST_CHECK_THROW(
        model.getLoggerBroker().createLogger(
            FullPN( "Variable:/:test:Value" ),
            Logger::Policy() ),
        NotFound );
}

BOOST_AUTO_TEST_CASE(testValid)
{
    using namespace libecs;

    StaticModuleMaker< EcsObject > mmaker;

    Model model( mmaker );

    model.createEntity( "Variable", FullID( "Variable:/:test" ) );
    model.getLoggerBroker().createLogger(
            FullPN( "Variable:/:test:Value" ),
            Logger::Policy() );
}