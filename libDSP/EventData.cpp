#include "EventData.h"

/**
  * class EventData
  * Copyright 2016, ShotSpotter
 */


// Constructors/Destructors
//  

EventData::EventData ( ) {
  initAttributes();
}

//EventData::EventData( const std::string &staNum, const std::string &chan, const std::string &moniker ) : DataCommon( staNum, chan, moniker )

EventData::EventData( const TimeObj &startUTC ) : DataCommon( startUTC )
{
  initAttributes();
}

EventData::EventData( const int &startDay, const int &endDay )
{
  initAttributes();

  if( startDay ) {
    if( startDay < 19800000 || startDay > 20500000 ) {
     std::cerr << "startDay out of range: " << startDay << "!!" << &std::endl;
      return;
    }
    utc.setMoniker( startDay );
  }
  if( endDay ) { // A specific endDay
    if( endDay < 19800000 || endDay > 20500000 ) {
     std::cerr << "endDay out of range: " << startDay << "!!" << &std::endl;
      return;
    }
    if( endDay < startDay ) {
     std::cerr << "endDay: " << endDay << " is before startDay: " << startDay << "!!" << &std::endl;
      return;
    }
    timeEnd.setMoniker( endDay );
  }
  //std::cout << "Start day set to: " << utc.getSQLDatetime() << ", end day set to: " << timeEnd.getSQLDatetime() << &std::endl;
}

EventData::EventData( const TimeObj &startT, const TimeObj &endT )
{
  initAttributes();

  if( startT > endT ) {
   std::cerr << "PulseData::PulseData() endT ahead of startT !!" << &std::endl;
    return;
  }

  if( startT == endT ) {
   std::cerr << "PulseData::PulseData() endT matches startT !!" << &std::endl;
  }

  if( startT.get() ) 
    utc.set( startT );
 
  if( endT.get() ) { // A specific endDay
    timeEnd.set( endT );
  }
}


EventData::EventData( const EventData &src, const bool &skipData ) : DataCommon( src, skipData )
{
  initAttributes();
}


EventData::~EventData() { }

void EventData::initAttributes ( ) 
{
  labels = NULL;
  colTypes = NULL;
}

bool 
EventData::setTimeEnd()
{
  if( !rows ) {
  	std::cerr << "EventData::setTimeEnd(), no rows, doing nothing!" << &std::endl;
  	return false;
  }
  
  std::cerr << "EventData::setTimeEnd(), not implemented!" << &std::endl;
  return false;
}

bool 
EventData::load()
{
  std::cerr << "Attempt to load data for abstract EventData class shot down!" << &std::endl;
  return false;
}

bool 
EventData::write( char* fileName ) const
{
  std::cerr << "EventData::write( char* fileName ) const, not implemented!" << &std::endl;
  return false;
}

void
EventData::display( std::ostream &outr ) const
{
  DataCommon::display( outr );
  outr << "EventData:: _______" << &std::endl;
  if( labels )
    outr << " Labels : " << labels << &std::endl;
  else
    outr << " Labels : NA" << &std::endl;
  if( colTypes )
    outr << " ColumnTypes : " << colTypes << &std::endl;
  else
    outr << " ColumnTypes : NA" << &std::endl;
}

bool
EventData::trim( const TimeObj& begT, const TimeObj& endT, char **newDataHolder, size_t *numRows ) const
{
  int begIdx = 0, endIdx = 0;
  int rowStep = size * cols;
  //std::cout << "Row size=" << rowStep << " size=" << size << " cols=" << cols << &std::endl;

  *numRows = 0;
  *newDataHolder = NULL;

  if( !rows ) {
  	std::cerr << "Empty object !!!" << &std::endl;
    return false;
  }

  double begDn = begT.getDatenum();
  double endDn = endT.getDatenum();

  if( !checkSort() ) {
   std::cerr << "Table sort check failed !!!" << &std::endl;
    return false;
  }

  for( unsigned long long  ith = 0; ith < rows; ith++ ) 
  {
  	double curT = ((double*)data)[ith*cols];
    if( !begIdx ) {
      //std::cout << "begDn=" << begDn << " curT=" << curT << &std::endl;
      if( begDn <= curT )
      	begIdx = ith;
    } else {
      if( endDn < curT ) {
        endIdx = ith - 1;
        break;
      }    
    }
  }

  int numRowsFound = endIdx - begIdx + 1;
  //std::cout << "Found segment from: " << begIdx << " -to- " << endIdx << " numRowsFound: " << numRowsFound << &std::endl;
  TimeObj beg, fin;
  std::string begStr, finStr;
  beg.setDatenum(((double*)data)[begIdx*cols]);
  fin.setDatenum(((double*)data)[endIdx*cols]);
  //fprintf( stdout, "   covering datenum range: %lf -to- %lf\n", beg.getDatenum(), fin.getDatenum() );
  beg.getSQLDatetimeFull( begStr );
  fin.getSQLDatetimeFull( finStr );
  //std::cout << "   covering time range: " << begStr << " -to- " << finStr << &std::endl;

  char* mcer = (char*)malloc(numRowsFound*rowStep);
  if( !mcer ) {
   std::cerr << "malloc() for trimmed table failed!!!" << &std::endl;
    return false;
  }

  memcpy( mcer, data+begIdx*rowStep, numRowsFound*rowStep );

  //fprintf( stdout, "   beginning time: %lf\n", ((double*)data+begIdx*cols)[0] );
  //fprintf( stdout, "   after pulse time: %lf\n", ((double*)data+endIdx*cols)[0] );

  *newDataHolder = mcer;
  *numRows = numRowsFound;

  return true;
}


bool 
EventData::testClass ( )  { return false; }
