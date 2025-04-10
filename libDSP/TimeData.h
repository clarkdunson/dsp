#ifndef __TIMEDATA_H__
#define __TIMEDATA_H__

/**
  * class TimeData
  * Copyright 2016, ShotSpotter
  */

#include "DataCommon.h"

/**
  * class TimeData
  * The TimeData object is designed to exist in two states:  1) loaded; and 2)
  * unloaded.  When unloaded, the object serves as a proxy for all manner of useful
  * operations such as copying a file.  Note also that the use of the TimeData
  * object is not restricted to QuakeFinder data only.  If the object represents or
  * contains QuakeFinder data, then the getNetwork() function will return a valid
  * result.  If the data is not associated with canonical QuakeFinder data, then
  * getNetwork() will return an empty std::string
  */

class TimeData : public DataCommon
{
public:

  // Constructors/Destructors

  /**
   * Empty Constructor
   */
  TimeData();

  /**
   * Time Constructor
   */
  TimeData( const TimeObj &startUTC );

  /**
   * Copy Constructor
   * @param src Object to be copied.
   * @param skipData (optional) Skip copy of the data, for speed
   */
  TimeData( const TimeData &src, const bool &skipData = false );

  /**
   * Empty Destructor
   */
  ~TimeData();

  // Methods
  
  /**
   * Compute file name
   * @return bool True is filename is valid
   */
  bool makeFileName( std::string &fileName ) const;

  /**
   * Provided to enforce use of derived class load()
   * @return bool True if load was successful
   */ 
  bool load();

  /**
   * @return bool True if write was successful
   * @param fileName Name or Path of file to write
   */
  bool write( char* fileName ) const;

  /**
   * @return bool True if write was successful
   * @param fileName Name or Path of file to write
   */
  bool setTimeEnd();

  /**
   * Is the object empty of data?
   * @return true if there is no data in this object.
   */
  bool isEmpty() const { return !rows; }

  /**
   * Concatenate two time series
   * @param  apendee DataCommon object to be appended.
   * @param  force Append regardless of any validation checks.
   */
  bool append( const DataCommon &apendee, const bool &force = false );

protected:

  /** Sample rate in samples per second */ 
  double sampleRate;

  /**
   * Set the value of sampleRate
   * @param new_var the new value of sampleRate
   */
  void setSampleRate ( const double &new_var ) { sampleRate = new_var; }

public:

  // Protected attribute accessor methods

  /**
   * Get the value of sampleRate
   * @return the value of sampleRate
   */
  double getSampleRate() const { return sampleRate; }

  /**
   * Get the number of samples
   * @return the sample count.
   */
  unsigned long long getSampleCount() const { return getRows(); }

  /**
   * Get the length of the time series in seconds.
   * @return double the length of this time series in seconds (0 if not loaded)
   */
  virtual double getLengthSecs() const;

  /**
   * Add up the algebraic sum of all the samples.
   * @return double the value of the sum.
   */
  virtual double sum() const;

  /**
   * Subtract out the mean of the time series.
   * @return double the value of the mean.
   */
  virtual double removeDC() const;

  /**
   * Dump the contents of this object.
   * @param ostream the stream to write to
   */
  void display( std::ostream &outr = std::cout ) const;

private:

  void initAttributes ( ) ;

};

typedef std::vector<TimeData> TimeDataVector;

#endif // __TIMEDATA_H__
