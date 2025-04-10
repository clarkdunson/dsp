
#ifndef __EVENTDATA_H__
#define __EVENTDATA_H__

/**
  * class EventData
  */

// For all EventData classes at qf, col1 = datenum., col 2 = dur in secs.


#include "DataCommon.h"

class EventData : public DataCommon
{
public:

  // Constructors/Destructors
  //  


  /**
   * Empty Constructor
   */
  EventData();

  /**
   * Time Constructor
   */
  EventData( const TimeObj &startUTC );

  /**
   * Regular Constructor
   */
  EventData( const std::string &staNum, const std::string &chan, const std::string &moniker );

  /**
   * Regular Constructor
   */
  EventData( const int &startDay = 0, const int &endDay = 0 );

  /**
   * Regular Constructor
   */
  EventData( const TimeObj &startT = TimeObj(), const TimeObj &endT = TimeObj() );

  /**
   * Copy Constructor
   * @param src Object to be copied.
   * @param skipData (optional) Skip copy of the data, for speed
   */
  EventData( const EventData &src, const bool &skipData = false );

  /**
   * Empty Destructor
   */
  virtual ~EventData ( );


  /**
   * @return char*
   * @param  which
   */
  char* getLabel (int which ) const { return NULL; }


  /**
   * @return bool
   * @param  me
   */
  bool append ( const DataCommon& me, const bool &force = false  ) { return false; }

  /**
   * Provided to enforce use of derived class load()
   * @return bool True if load was successful
   */ 
  bool load();

  /**
   * @return bool
   * @param  fileName
   * @param  format
   * @param  formatStr
   */
  bool write (char* fileName, int format, char* formatStr ) { return false; }

  /**
   * @return bool
   * @param  fileName
   */
  bool write( char* fileName ) const;

  /**
   * @return bool
   * @param  inFid
   * @param  fileName
   * @param  formatStr
   */
  bool readLabelsFile (FILE* inFid, char* fileName, char* formatStr ) { return false; }

  /**
   * Set the value of timeEnd
   * Lack of arguments forces encapsulation.
   * @return true if the object is non-empty, i.e. it spans a finite amount of time.
   */
  bool setTimeEnd();

  /**
   * Is the object empty of data?
   * @return true if there is no data in this object.
   */
  bool isEmpty() const { return !rows; }

  /**
   * @return bool
   * @param  fileName
   */
  bool readBinaryFile (char* fileName ) { return false; }

  /**
   * @return bool
   * @param  fileName
   */
  bool writeBinaryFile (char* fileName ) { return false; }

  /**
   * @return bool
   * @param  she
   */
  bool diff (const EventData& she ) { return false; }

  /**
   * @return bool
   */
  static bool testClass();

protected:

  /** Column labels */
  char* labels;
  
  /** Column types */
  int* colTypes;

public:


 /**
   * Set the value of labels
   * @param new_var the new value of labels
   */
  void setLabels ( char* new_var )   {
      labels = new_var;
  }

  /**
   * Get the value of labels
   * @return the value of labels
   */
  char* getLabels ( )   {
    return labels;
  }

  /**
   * Set the value of colTypes
   * @param new_var the new value of colTypes
   */
  void setColTypes ( int* new_var )   {
      colTypes = new_var;
  }

  /**
   * Get the value of colTypes
   * @return the value of colTypes
   */
  int* getColTypes ( )   {
    return colTypes;
  }

  /**
   * Data trimmer
   * @param begT beginning time of slice window.
   * @param endT ending time of slice window.
   * @param newData Pointer to trimmed pulse array.  NULL if none found.  Note:: This has been malloc()'d.
   * @param numRows number of rows found.
   * @return true if trim produced at least one event 
   */
  bool trim( const TimeObj& begT, const TimeObj& endT, char **newData, size_t *numRows ) const;

  /**
   * Check object for consistency
   * @return bool true if check passes.
   */
  bool check() const { return false; }

  /**
   * Sort object for increasing t.
   * @return bool true if sort succeeds.
   */
  bool sort() { return false; }

  /**
   * Check object for correct sort.
   * @return bool true if check passes.
   */
  bool checkSort() const { return false; }

  /**
   * Check object for overlapping events.  
   * NOTE: For this to work, col 1 must be datenum and col 2 must be seconds duration.
   * @return bool true if check passes.
   */
  bool checkOverlap() const { return false; }

  /**
   * Merges any overlapping events into one.
   * @return bool true if flatten succeeds.
   */
  bool flatten() { return false; }

  /**
   * Check object for overlapping events.
   * @return bool true if check passes.
   */
  bool checkFlatten() const { return false; }


  /**
   * Default initailizer
   */
  void initAttributes ( ) ;

  /**
   * Displayer of object values.
   * @param outr stream to write to.
   */
  void display( std::ostream &outr = std::cout ) const;

};

#endif // __EVENTDATA_H__
