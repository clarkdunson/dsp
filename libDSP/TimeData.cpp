#include "TimeData.h"

/**
  * class FrequencyTimeData
  * Copyright 2016, ShopSpotter
  */

// Constructors/Destructors
//

//#define ALLOC_DBG

TimeData::TimeData() 
{
  initAttributes();
}

TimeData::TimeData( const TimeObj &startUTC ) : DataCommon( startUTC )
{
  initAttributes();
}

TimeData::TimeData( const TimeData &src, const bool &skipData ) : DataCommon( src, skipData )
{
  initAttributes();

  sampleRate  = src.sampleRate;

  if( skipData ) return;

  rows = src.rows;
  if( src.data ) {
    size_t malBytes = rows * cols * size;
    data = (char*)malloc( malBytes );
    if( !data )
      std::cerr << "Could malloc() " << malBytes << " bytes!" << &std::endl;
    else
      memcpy( data, src.data, malBytes );
  }
}


TimeData::~TimeData() 
{
 #ifdef ALLOC_DBG
  std::cerr << "TimeData destructor." << &std::endl;
 #endif
}

//  
// Methods
//  


// Accessor methods
//  


// Other methods
//  

void TimeData::initAttributes() 
{
  DataCommon::initAttributes();
  sampleRate = 0.0;
}

void
TimeData::display( std::ostream &outr ) const
{
  DataCommon::display( outr );
  outr << "TimeData:: ________" << &std::endl;
  outr << " Sample rate : " << sampleRate << &std::endl;
}


bool 
TimeData::makeFileName( std::string &fileName ) const
{
  fileName = "";
  std::cerr << "Attempt to create filename for abstract TimeData class shot down!" << &std::endl;
  return false;
}

bool 
TimeData::load()
{
  std::cerr << "Attempt to load data for abstract TimeData class shot down!" << &std::endl;
  return false;
}

bool
TimeData::write( char* fileName ) const {
  FILE* fid = fopen( fileName, "w" );
  if( !fid ) {
    std::cerr << "File: " << fileName << &std::endl;
    return false;
  }

  int* samps = (int*)getData();
  for( size_t s = 0; s < getSampleCount(); s++ )
    fprintf( fid, "%d\n", samps[s] );

  return true;
}



bool
TimeData::setTimeEnd()
{
  if( !rows )
    timeEnd = utc;
  else
    timeEnd = utc + TimeObj((1.0/sampleRate)*(rows-1));

  return true;
}

double
TimeData::getLengthSecs() const 
{
  return ((double)(getSampleCount() - 1)) / getSampleRate();
}

const double epsGrace = 1.1;
const double srGrace = 0.01;

bool
TimeData::append( const DataCommon &apendee, const bool &force )
{

  const TimeData *tDat = (const TimeData *)&apendee;

 /* Will append if times match */
  if( apendee.isEmpty() ) {
    std::cerr << "TimeData::append() specified object to append is empty!" << &std::endl;
    return false;
  }

  if( !force ) 
  {
    if( !pedigreeCheck( apendee ) ) {
      std::cerr << "TimeData::append() mismatch on data pedigree!" << &std::endl;
      return false;
    }

   /* Make sure basis is up to date */
    setTimeEnd();

    double srEps = getSampleRate() * srGrace;
    double srDiff = fabs( sampleRate - tDat->sampleRate );
    if( srDiff > srEps ) {
      std::cerr << "TimeData::append() sample rates differ too much!" << &std::endl;
      return false;
    }

    double samplePeriod = 1.0 / getSampleRate();
    double timeEps = epsGrace * samplePeriod;
    double tDiff = apendee.getUTC().get() - getTimeEnd().get();
    if( tDiff < 0 ) {
      std::cerr << "TimeData::append() apendee starts before end of basis!" << &std::endl;
      return false;
    }
    if( tDiff > timeEps ) {
      std::cerr << "TimeData::append() apendee starts too late after end of basis!" << &std::endl;
      return false;
    }
  } // end of checks

  size_t oldSize = getByteSize();
  size_t addSize = apendee.getByteSize();
  char* newArr = (char*)realloc( getData(), oldSize+addSize );
  if( !newArr ) {
    std::cerr << "TimeData::append() realloc() failed!" << &std::endl;
    return false;
  }

 /* Glory be! */  
  memcpy( newArr+oldSize, apendee.getData(), addSize );

  return true;

}

double
TimeData::sum() const 
{
  return false;
}

double
TimeData::removeDC() const 
{
  return sum() / rows;
}

