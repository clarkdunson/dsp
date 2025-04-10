#include "DataCommon.h"

/**
  * class DataCommon
  * Copyright 2016, ShotSpotter
  */

// Constructors/Destructors
//  

DataCommon::DataCommon()
{
  //std::cout << "DataCommon::DataCommon" << &endl;
  initAttributes();
}


DataCommon::DataCommon( const TimeObj &startUTC )
{
  initAttributes();
  utc = startUTC;
}

DataCommon::DataCommon( const DataCommon &src, const bool &skipData )
{
  utc         = src.utc;
  timeOffset  = src.timeOffset;
  timeEnd     = src.timeEnd;
  cols        = src.cols;
  size        = src.size;
  meta        = src.meta;
}

DataCommon::~DataCommon() 
{
  if( data != NULL ) {
   	//std::cout << "Deleting non-NULL DataCommon object!" << &endl;
   	free( data );
  } 
}


bool
DataCommon::pedigreeCheck( const DataCommon& she ) const
{
    if( type != she.type ||
      utc  != she.utc  ||
      size != she.size ||
      cols != she.cols ||
      rows != she.rows )
      return false;
    else
      return true; 
}


bool
DataCommon::dataDiff( const DataCommon& she ) const 
{  
  size_t bytes = size * rows * cols;
  int result = memcmp( data, she.data, bytes );
  if( result )
    return true;
  else
    return false;
}
 


bool
DataCommon::diff( const DataCommon& she ) const 
{  

  // Nyah, not so  important
  // string valueType;
  // string valueUnit;
  // string axisLabel;
 #ifdef SST_DEBUG
  // These are dependent on the above
  // TimeObj timeOffset;
  // TimeObj timeEnd;
 #endif

  if( pedigreeCheck( she ) == false )
    return true;

  return dataDiff( she );
}



void
DataCommon::initAttributes() 
{
  type = DATA_COMMON;
  // utc, timeOffset, timeEnd all set to 0
  // network, staNum, chNum all empty.
  //fprintf(stderr,"DC const\n");
  data = NULL;
  size = 4; // Default
  cols = 1;
  rows = 0;
  interleaved = true;
  // valueType, valueUnit, axisLabel all empty.
}

void
DataCommon::display( std::ostream &outr ) const
{
  std::string pawn;
  outr << &std::endl;
  outr << "DataCommon:: ________" << &std::endl;
  utc.getSQLDatetimeFull(pawn);
  outr << " Start time (UTC) : " << pawn << &std::endl;
  outr << " Time Offset (secs) : " << timeOffset.get() << &std::endl;
  pawn.clear();
  timeEnd.getSQLDatetimeFull(pawn);
  outr << " End time (UTC) : " << pawn << &std::endl;
  outr << " Element size (bytes) : " << size << &std::endl;
  outr << " Column count : " << cols << &std::endl;
  outr << " Row count : " << rows << &std::endl;
  for( size_t i = 0; i < meta.size(); i++ ) {
    outr << meta[i] << &std::endl;
  }
}


void
DataCommon::reset() 
{
  clear();

  initAttributes();

  utc = TimeObj(0.0);
  timeOffset = TimeObj(0.0);
  timeEnd = TimeObj(0.0);

  meta.resize(0);

 #ifdef KEEP_HISTORY
  history.resize(0);
 #endif // KEEP_HISTORY
}


// Could be switched to hash table
void 
DataCommon::setMetaItem( const size_t idx, const std::string &meta_item ) 
{ 
  if( idx > 1000 ) 
    std::cerr << idx << " seems like a LOT of meta data items, are you sure?" << &std::endl;

  size_t numItems = meta.size();
  if( idx > numItems ) {
    std::cerr << "Re-sizing meta data to add you string." << &std::endl;
    meta.resize(idx);
  }
  meta[idx] = meta_item;
}

bool
DataCommon::getMetaItem( const size_t idx, std::string &meta_item ) const 
{ 
  size_t numItems = meta.size();
  if( idx > numItems ) {
    std::cerr << idx << ": your specified index is out of range." << std::endl;
    return false;
  }
  meta_item = meta[idx];
  return true;
}



bool 
DataCommon::write( FILE* fid ) const { return false; }


void 
DataCommon::clear()
{
  if( data )
    free( data );
  data = NULL;
  rows = 0;
}


void 
DataCommon::zero()
{
  if( !data ) return;
  
  size_t numBytes = getByteSize();
  memset( data, 0, numBytes );
}

bool
DataCommon::createDataBuffer() 
{
  if( data != NULL ) {
    clear();
  }
  size_t numBytes = cols * size * rows;
  data = (char*)malloc( numBytes );
  if( !data ) {
    std::cerr << "Could not create data buffer DataCommon::createDataBuffer() is aborting!!" << &std::endl;
    return false;
  } else 
    return true;
}

size_t 
DataCommon::load( FILE* fid )
{
  if( data == NULL ) {
    size_t numBytes = cols * size * DEFAULT_NUMBER_OF_ROWS;
    data = (char*)malloc( numBytes );
    if( !data ) {
      std::cerr << "Could not allocate memory to load " << numBytes << " of data!" << &std::endl;
      return 0;
    } else
      rows = DEFAULT_NUMBER_OF_ROWS;
  }

  // Try to read whole rows
  size_t rowBytes = getRowSize();
  size_t numRowsRead = fread( data, rowBytes, DEFAULT_NUMBER_OF_ROWS, fid );
  if( !numRowsRead ) {
    std::cerr << "Could not load any rows from file!" << &std::endl;
    free(data);
    return 0;
  }
  //std::cout << "Loaded sample 1: " << ((int*)data)[0] << &endl;
  //std::cout << "Loaded sample 2: " << ((int*)data)[1] << &endl;
  //std::cout << "Loaded " << numRowsRead << " rows of data." << &endl;

  int errCode = ferror(fid);
  if( errCode ) {
    std::cerr << "Could not load from file due to error!" << &std::endl;
    free(data);
    return 0;
  }

  if( numRowsRead < rows ) 
  {

    // Resize needed.
    if( !feof(fid) ) {
      std::cerr << "Less rows loaded than requested, " << numRowsRead << " .vs. " << rows << ", but end of file not reached!" << &std::endl;
      free(data);
      return 0;
    }

    size_t numBytesRead = rowBytes * numRowsRead;
    char* tmpr = (char*)realloc( data, numBytesRead );
    if( tmpr == NULL ) {
      std::cerr << "Resize of array failed after load!!" << &std::endl;
      free(data);
      return 0;
    }

    //std::cout << "Realloc'ed from: " << (void*)data << " to " << (void*)tmpr << &endl;
    data = tmpr;

  } // else done.

  rows = numRowsRead;

  return numRowsRead;
}

size_t
DataCommon::load( const char* fileName, unsigned long long max_rows )
{
  unsigned long long numRowsRead;
  if( data ) {
    std::cerr << "Attempt to load data without clear()ing first was shot down!" << &std::endl;
    return 0;
  }

  FILE *fid = fopen( fileName, "r" );
  if( fid == NULL ) {
    std::cerr << "File: " << fileName << " was not opened!" << &std::endl;
    return 0;
  }

  //rows = max_rows;  // Zero if not supplied
  if( !(numRowsRead = load( fid )) ) {
    std::cerr << "Load of: " << fileName << " failed!" << &std::endl;
    fclose( fid );
    return 0;
  }

  fclose( fid );

  return numRowsRead;
}

