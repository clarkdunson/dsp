#ifndef __DATACOMMON_H__
#define __DATACOMMON_H__

/**
  * class DataCommon
  * Copyright 2016, ShotSpotter
  */

#include "libCore/libCore.h"

#define DEFAULT_NUMBER_OF_ROWS 10000000

enum ObjTypes {
  DATA_COMMON,
  TIME_DATA,
  FREQUENCY_DATA,
  FREQUENCY_TIME_DATA,
  EVENT_DATA,
  numObjTypes
};

// These are suggestions only
enum metaData {
  VALUE_TYPE, /** Amplitude, energy, etc. */
  VALUE_UNIT, /** Volts, Teslas, etc. */
  AXIS_LABEL, /** Auxilliary label to use when plotting */
  NUM_DC_META
};


/**
  * class DataCommon
  * Core processing base class.  Contains data identifiers, labels, and times.
  * Note that the default number of columns is one.  The design is such that the
  * number of columns affects how loading of the data is done, but the number of
  * rows is wide open.
  */

class DataCommon
{
protected:

  /** These are needed by Matlab's form of RTTI */
  ObjTypes type;
  
  /** Start time of this data */
  TimeObj utc;
  
  /** Used by filters for introducing group delay in an overt way */
  TimeObj timeOffset;
  
  /** Calculated by private methods and cached here */
  TimeObj timeEnd;
  
  /** Data Pointer */
  char* data;

  /** col major if true */
  bool interleaved;
    
  /** Element size in bytes */
  unsigned int size;
  
  /** Number of columns */
  unsigned int cols;
  
  /** Number of rows in this array (utc + (numRows-1)/(sampleRate*86400 = endTime) */
  unsigned long long rows;
  
private: 
  std::vector<std::string> meta;

 #ifdef KEEP_HISTORY
  std::vector<std::string> history;
 #endif // KEEP_HISTORY

public:

  /**
   * Empty Constructor
   */
  DataCommon();

  /**
   * Full Constructor
   */
  DataCommon( const TimeObj &startUTC );

  /**
   * Copy Constructor
   * @param src Object to be copied.
   * @param skipData (optional) Skip copy of the data, for speed
   */
  DataCommon( const DataCommon &src, const bool &skipData = false );

  /**
   * Destructor
   */
  virtual ~DataCommon();

  /**
   * Compute file name
   * @return bool True is filename is valid
   */
  virtual bool makeFileName( std::string &fileName ) const { return false; };

  /**
   * The call to load.  Many heirs simply use the two routines below. 
   * @return bool
   */
  virtual bool createDataBuffer();

  /**
   * The call to load.  Many heirs simply use the two routines below. 
   * @return bool
   */
  virtual bool load() = 0;

  /**
   * A file is read and values are inserted into this object.  If max_rows
   * is supplied, the buffer is malloc'd, then shrunk to fit.  If the file
   * contains more data than requested, the buffer is filled and a warning
   * of incomplete load is issued.
   * @return bool True is load was successful
   * @param  fileName Name of file to read
   * @param  max_rows Pre-alloc this any rows
   */
  virtual size_t load( const char* fileName, unsigned long long max_rows = 0 );

  /**
   * The workhorse that actually does the load.  Subclasses can override.
   * @return bool True is load was successful
   * @param fid  The open file
   */
  virtual size_t load( FILE* fid );

  /**
   * @return bool
   * @param  fileName
   */
  virtual bool write ( char* fileName ) const = 0;

  /**
   * @return bool True is write was successful
   * @param fid The open file ID
   */
  bool write (FILE* fid ) const;

  /**
   * Free all data associated with this object.
   */
  virtual void clear();

  /**
   * Free all data associated with this object.
   */
  virtual void zero();

  /**
   * Clear Data object, deleting all samples, std::strings, and
   * other settings.
   */
  void reset();

  /**
   * @return unsigned int The number of columns
   */
  unsigned int getNumCols() const { return cols; }


  /**
   * @return undef
   */
  unsigned long long getNumRows() const { return rows; }

  /**
   * @return bool
   * @param  she
   */
  bool dataDiff( const DataCommon& she ) const;

  /**
   * @return bool
   * @param  she
   * Checks row, cols, size, type, & utc
   */
  bool pedigreeCheck( const DataCommon& she ) const;

  /**
   * @return bool
   * @param  she
   * Returns true if they are different.  valueType, valueUnit axisLabel are treated
   * don't cares.  timeOffset and timeEnd are supposed to be maintained by the code,
   * and therefore are only examined when SST_DEBUG is defined.
   */
  bool diff( const DataCommon& she ) const;


  /**
   * Extract reduced portion of data from the first sample at or after begT and before finT
   * @return true id any data exists between begT & finT
   * @param  begT
   * @param  finT
   */
  virtual bool trim( const TimeObj &begT, const TimeObj &finT ) const { return false; }

  /**
   * Data trimmer, fully expounded.  Using a pair of functions allows base classes more flexibilty.
   * @param begT beginning time of slice window.
   * @param endT ending time of slice window.
   * @param newData Pointer to trimmed pulse array.  NULL if none found.  Note:: This has been malloc()'d.
   * @param numRows number of rows found.
   * @return true if trim produced at least one event 
   */
  virtual bool trim( const TimeObj& begT, const TimeObj& endT, char **newData, size_t *numRows ) const { return false; }


public:

  /**
   * Set the value of type
   * These are needed by matlabs form of RTTI
   * @param new_var the new value of type
   */
  void setType( const ObjTypes &new_val ) { type = new_val; }

  /**
   * Get the value of type
   * These are needed by matlabs form of RTTI
   * @return the value of type
   */
  ObjTypes getType() const { return type; }

  /**
   * Set the value of utc
   * Start time of this data
   * @param new_var the new value of utc
   */
  void setUTC ( const TimeObj &new_var ) { utc = new_var; }

  /**
   * Get the value of utc
   * Start time of this data
   * @param tt the value of utc
   */
  void getUTC( TimeObj &tt ) const { tt = utc; }

  /**
   * Get the value of utc
   * Start time of this data
   * @return the value of utc
   */
  const TimeObj& getUTC() const { return utc; }

  /**
   * Set the value of timeOffset
   * Used by filters for introducing group delay in an overt way
   * @param new_var the new value of timeOffset
   */
  void setTimeOffset( const TimeObj &new_var ) { timeOffset = new_var; }

  /**
   * Add to the value of timeOffset
   * Used by filters for introducing group delay in an overt way
   * @param time_incr the new value of timeOffset
   */
  void addToTimeOffset( const TimeObj &time_incr ) { timeOffset += time_incr; }

  /**
   * Add to the value of timeOffset
   * Used by filters for introducing group delay in an overt way
   * @param seconds the number of seconds to add to timeOffset
   */
  void addToTimeOffset( const double seconds ) {
    TimeObj offt( seconds );
    timeOffset += offt;
  }

  /**
   * Get the value of timeOffset
   * Used by filters for introducing group delay in an overt way
   * @return the value of timeOffset
   */
  void getTimeOffset( TimeObj &tOff ) const { tOff = timeOffset; }

  /**
   * Get the value of timeOffset
   * Used by filters for introducing group delay in an overt way
   * @return the value of timeOffset
   */
  double getTimeOffset() const { return timeOffset.get(); }

  /**
   * Set the value of timeEnd
   * Lack of arguments forces encapsulation.
   * @return true if the object is non-empty, i.e. it spans a finite amount of time.
   */
  virtual bool setTimeEnd() = 0;

  /**
   * Get the value of timeEnd
   * Calculated by private methods and cached here
   * @param tEnd value of timeEnd.
   * @return true if the object is non-empty, i.e. it spans a finite amount of time.
   */
  void getTimeEnd( TimeObj &tEnd ) const { tEnd = timeEnd; }

  /**
   * Get the value of timeEnd
   * Calculated by private methods and cached here
   * @param tEnd value of timeEnd.
   * @return true if the object is non-empty, i.e. it spans a finite amount of time.
   */
  const TimeObj& getTimeEnd() const { return timeEnd; }

  /**
   * Set the value of data
   * @param new_var the new value of data
   */
  void setData( char* new_var ) { data = new_var; }

  /**
   * Get the value of data
   * @return the value of data
   */
  char* getData() const { return data; }

  /**
   * Get the value of data
   * @param vals the pointer to allocate and write into.
   * @return true if successful.
   */
  virtual bool getVals( double **vals ) const { 
    std::cerr << "Bogus call to DataCommon base getVals() method" << &std::endl;
    return false; 
  }

  /**
   * Get the value of data
   * @param vals the buffer to write from.
   * @param numVals the number of samples in val.
   * @return true if successful.
   */
  virtual bool setVals( double *vals, const size_t &numVals ) { 
    std::cerr << "Bogus call to DataCommon base setVals() method" << &std::endl;
    return false; 
  }

  /**
   * Set the value of cols
   * @param new_var the new value of cols
   */
  void setEltSize( const unsigned int new_size ) { size = new_size; }

  /**
   * Get the value of cols
   * @return the value of cols
   */
  inline unsigned int getEltSize() const { return size; }

  /**
   * Set the value of cols
   * @param new_var the new value of cols
   */
  void setCols( const unsigned int new_val ) { cols = new_val; }

  /**
   * Get the value of cols
   * @return the value of cols
   */
  inline unsigned int getCols() const { return cols; }

  /**
   * Set the value of rows
   * Number of rows in this array (utc + (numRows-1)/(sampleRate*86400 = endTime)
   * @param new_var the new value of rows
   */
  void setRows( const unsigned long long new_val ) { rows = new_val; }

  /**
   * Get the value of rows
   * Number of rows in this array (utc + (numRows-1)/(sampleRate*86400 = endTime)
   * @return the value of rows
   */
  inline unsigned long long getRows() const { return rows; }

  /**
   * Get the number of bytes per row
   * @return bytes per row
   */
  inline size_t getRowSize() const { return size * cols; }

  /**
   * Get the number of bytes of data
   * @return bytes of data stored in this object.
   */
  inline size_t getByteSize() const { return size * cols * rows; }

  /**
   * Sets all meta to new_meta
   * @param new_meta a vector of strings holding meta data.
   */
   void setMeta( const std::vector<std::string> &new_meta ) { meta = new_meta; }

   /**
    * Get a copy of all meta string from this instance
    * @param meta_copy a copy of all meta strings
    */
   void getMeta( std::vector<std::string> &meta_copy ) const { meta_copy = meta; }
 
  /**
   * Sets meta[idx] to meta_item
   * @param new_meta a vector of strings holding meta data.
   */
   void setMetaItem( const size_t idx, const std::string &meta_item );

   /**
    * Get a copy meta[idx] into meta_item
    * @param meta_copy a copy of all meta strings
    * @return true if idx was within range
    */
   bool getMetaItem( const size_t idx, std::string &meta_item ) const;

 #ifdef KEEP_HISTORY
  /**
   * Get the value of axisLabel
   * Auxilliary label to use when plotting
   * @return the value of axisLabel
   */
  void getHistory( std::vector<std::string> &hist ) { hist = history; }

  /**
   * Get the value of axisLabel
   * Auxilliary label to use when plotting
   * @return the value of axisLabel
   */
  void addToHistory( const std::string &nextStr ) { history.push_back(nextStr); }
 #endif // KEEP_HISTORY

protected:

  /**
   * @return bool
   * @param  inFid
   * @param  compressed
   */
  bool readFinish (FILE* inFid, bool compressed ) { return false; }

  /**
   * Default initailizer
   */
  void initAttributes();

public:

  /**
   * Default displayer of object values.
   * @param outr stream to write to.
   */
  virtual void display( std::ostream &outr = std::cout ) const;

  /**
   * Is the object empty of data?
   * @return true if there is no data in this object.
   */
  virtual bool isEmpty() const = 0;

  /**
   * Default adder
   * @param apendee DataCommon object to be appended.
   * @param force Append regardless of any validation checks.
   * @return true if the append was successful.
   */
  virtual bool append( const DataCommon &apendee, const bool &force = false ) { return false; }

  /**
   * Check object for consistency
   * @return bool true if check passes.  (guilty 'til proven innocent)
   */
  virtual bool check() const { return false; }

  /**
   * Sort object for increasing t
   * @return bool true if sort succeeds.
   */
  bool sort() { return false; }

  /**
   * Check object for correct sort
   * @return bool true if check passes.
   */
  virtual bool checkSort() const { return false; }


};

#endif // __DATACOMMON_H__
