#!/usr/bin/env python

import string


# PropertyAttribute bit masks
SETABLE = 1 << 0   # == 1
GETABLE = 1 << 1   # == 2


# FullID and FullPropertyName field numbers
TYPE       = 0
SYSTEMPATH = 1
ID         = 2
PROPERTY   = 3



def parseFullID( fullid ):
    aList = string.split( fullid, ':' )
    aLength = len( aList )
    if aLength != 3:
        raise ValueError(
            "FullID must have 2 ':'s. ( %d given ) " % aLength - 1 )
    return aList


def parseFullPropertyName( fullpropertyname ):
    aList = string.split( fullpropertyname, ':' )
    aLength = len( aList )
    if aLength != 4:
        raise ValueError(
            "FullPropertyName must have 3 ':'s. ( %d given ) " % aLength - 1 )
    return aList


def constructFullID( words ):
    aLength = len( words )
    if aLength != 3:
        raise ValueError(
            "FullID has 3 fields. ( %d given )" % aLength )
    return string.join( words, ':' )


def constructFullPropertyName( words ):
    aLength = len( words )
    if aLength != 4:
        raise ValueError(
            "FullPropertyName has 4 fields. ( %d given )" % aLength )
    return string.join( words, ':' )



if __name__ == "__main__":
    
    fullid  = parseFullID( 'System:/CELL/CYTOPLASM:MT0' )
    print fullid

    fullproperty =  parseFullPropertyName(
        'System:/CELL/CYTOPLASM:MT0:activity' )
    print fullproperty

    fullidstring = constructFullID( fullid )
    print fullidstring

    fullpropertystring = constructFullPropertyName( fullproperty )
    print fullpropertystring
