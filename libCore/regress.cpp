#include "libCore.h"

int
main( int argc, char* argv[] ) 
{
  TimeObj::initClass();
  LogObj::initClass();
  LockObj::initClass();
  
  LogObj logg;
  logg.init( "stderr", std::string("699") );
  
  if( TimeObj::testClass() ) { 
    fprintf( stderr, "XXX Clark Time regression can fail sometimes!\n" );
    goto BLAM;
  }
  if( LockObj::testClass() ) goto BOGUS;
  //if( qfPath::testClass() ) return DRATS;

BLAM :
  printf( "VOILA!\n" );
  return VOILA;

BOGUS :
  printf( "FUPDUCK!\n" );
  return DRATS;
}