#include "libSVG.h"

int
main( int argc, char* argv[] ) 
{
  Plot ts;
   
  if( !ts.write("TestSVG") )
  	goto BOGUS;
  else
  	goto BLAM;


BLAM :
  printf( "VOILA!\n" );
  return VOILA;

BOGUS :
  printf( "FUPDUCK!\n" );
  return DRATS;
}