#ifndef __TIMEOBJ_H__
#define __TIMEOBJ_H__

#include "DSPCommon.h"

/**
  * class TimeObj
  * Copyright 2016, ShotSpotter
  * TimeObj is a time object that keeps time down to a one microsecond
  * accuracy.  It's range is approximately +/- 68 years.  Because it uses
  * struct timeval for its currency, it can be used for both relative and
  * absolute time calculations. Must call TimeObj::initClass() to set the 
  * time zone before use!!!
  */
class TimeObj
{
private: // Value
  
  /** The time value */
  struct timeval t;

  /** The time value */
  static unsigned int tzOff;

public: // Initialize & Test
  
  /**
   * Initialize this class for use.  Must call before use!!!
   */
  static void initClass();
    
  /**
   * Run the regression test for this class.  Return 0 if good.
   * @return bool
   */
  static bool testClass();

  /** The time value offset in hours for time zone
   * @return unsigned int
   */
  static unsigned int getTimeZone() { return tzOff; }

public: // Construct/destruct

  /**
   * Default Constructor zeros time to time_t = 0
   */
    TimeObj() { set( 0, 0 ); }

  /**
   * Destructor
   */
   ~TimeObj() {}

    
  /**
   * Constructor taking a double (seconds)
   * @param  sec
   */
    TimeObj( double sec );

  /**
   * Copy constructor
   * @param src obect to be copied.
   */
    TimeObj(const TimeObj &src) { t.tv_sec = src.t.tv_sec; t.tv_usec = src.t.tv_usec; }

  /**
   * Constructor taking seconds + microseconds
   * @param  sec
   * @param  usec
   */
    TimeObj(time_t sec, long usec) { t.tv_sec = sec; t.tv_usec = usec; }

  /**
   * Constructor taking struct timeval
   * @param  tv
   */
    TimeObj(const struct timeval *tv) { t.tv_sec = tv->tv_sec; t.tv_usec = tv->tv_usec; }

  /**
   * Constructor taking mysql string
   * @param  mysqlStr
   */
    TimeObj( const std::string &mysqlStr ) { setSQLDatetime( mysqlStr ); }


public: // Access

  /**
   * Set this TimeObj from a double (in seconds)
   * @param  sec
   */
    void set( double sec );

  /**
   * Set this TimeObj from another
   * @param  setee
   */
    void set( const TimeObj &setee ) { *this = setee; }

  /**
   * Set this TimeObj from seconds + microseconds
   * @param  sec
   * @param  usec
   */
    void set( time_t sec, long usec ) { t.tv_sec = sec; t.tv_usec = usec; }

  /**
   * Set this TimeObj from a struct timeval
   * @param  tv
   */
    void set( const struct timeval *tv ) { t.tv_sec = tv->tv_sec; t.tv_usec = tv->tv_usec; }

  /**
   * Set this TimeObj from milliseconds
   * @param  msec
   */
    void setMsec( unsigned long msec ) { t.tv_sec = time_t(msec/1000); t.tv_usec = long(1000 * (msec % 1000)); }

  /**
   * Get time in seconds as a double
   * @return  double
   */
    double get() const { return ((double)t.tv_sec) + ((double)t.tv_usec) / 1000000.0; }

  /**
   * Get time in seconds & microseconds
   * @param  sec
   * @param  usec
   */
    void get( time_t &sec, long &usec ) const { sec = t.tv_sec; usec = t.tv_usec; }

  /**
   * Get time in a struct timeval
   * @param  tv
   */
    void get( struct timeval *tv ) const { tv->tv_sec = t.tv_sec; tv->tv_usec = t.tv_usec; }

  /**
   * Get time in milliseconds (for Xt)
   * @return  unsigned long
   */
    unsigned long getMsec() const { return t.tv_sec * 1000 + t.tv_usec / 1000; }

  /**
   * Convert to a string.  The default format is seconds with 3 digits of fraction precision.
   * Other formats that may be selected:
   *
   *   %%                      the '%' character
   *   %D                      total number of days
   *   %H                      total number of hours 
   *   %M                      total number of minutes
   *   %S                      total number of seconds
   *   %I                      total number of milliseconds
   *   %U                      total number of microseconds
   *	%h      00-23           hours remaining after the days
   *   %m      00-59           minutes remaining after the hours
   *   %s      00-59           seconds remaining after the minutes
   *   %i      000-999         milliseconds remaining after the seconds
   *   %u      000000-999999   microseconds remaining after the seconds
   *
   * uppercase descriptors are formatted with a leading '-' for negative times
   * lowercase descriptors are formatted fixed width with leading zeros
   * @return const char*
   * @param  buf
   * @param  format
   */
    const char* format( char* buf, const char *format = 0 ) const;

  /**
   * Convert to a date string, interpreting the time as seconds since
   * Jan 1, 1970.  The default format gives "Tuesday, 01/26/93 11:23:41 AM".
   * See the 'cftime()' man page for explanation of the format string.
   * @return const char*
   * @param  buf
   * @param  fmtr
   */
    const char* formatDate( const char* buf, const char *fmtr ) const;

  /**
   * Convert this time to a tm struct, rounding first.  Returns true if successful.
   * @return bool
   * @param  result
   */
    bool localtime( struct tm *result ) const
    {
      TimeObj tt = *this;
      struct tm tmp;
      tt.round();
      struct tm *tmPtr = localtime_r( &(tt.t.tv_sec), &tmp );
      if( tmPtr ) {
          *result = tmp;
          return true;
      } else
          return false;
    }


  /**
   * Convert this time to a UTC tm struct, rounding first.  Returns true if successful.
   * @return bool
   * @param  result
   */
    bool gmtime( struct tm *result ) const
    {
      TimeObj tt = *this;
      struct tm tmp;
      tt.round();
      struct tm *tmPtr = gmtime_r( &(tt.t.tv_sec), &tmp );
      if( tmPtr ) {
          *result = tmp;
          return true;
      } else
          return false;
    }


  /**
   * Set this time from a tm struct. Returns true if successful.
   * @return bool
   * @param  newTime
   */
    bool mktime( struct tm *newTime )
    { 
        time_t tt = ::mktime( newTime );
        if( tt != -1 ) {
            t.tv_sec = tt;
            t.tv_usec = 0; 
            return true;
        } else 
            return false;
    }

  /**
   * Set this TimeObj from a Matlab datenum
   * @return bool
   * @param  datenum
   */
    void setDatenum( double datenum );


  /**
   * Get this TimeObj as a Matlab datenum
   * @return double
   */
    double getDatenum() const { return( double(t.tv_sec)/86400.0 + double(t.tv_usec)/( 1000000.0 * 86400.0 ) + 719529.0 ); }


  /** Get closest time_t
   * @return time_t */
    time_t getTime_t() const { 
      time_t sec; long usec;
      get( sec, usec );
      if( usec >= 500000 ) sec++;
      return sec;
    }


  /** Set this TimeObj from a SQL string with optional microseconds (YYYY-MM-DD HH:MM:SS[.XXXXXX])
   * If anything goes wrong, this TimeObj remains unchanged.
   * @param  inStr Length of supplied string
   * @param  rpt The default is to write to stderr, but you can change that!
   * @return bool true if successful */
    bool setSQLDatetime( const std::string &inStr, FILE *rpt = stderr )
    {
      struct tm tStruct;
      size_t strLen = inStr.length();
      TimeObj keptCopy( *this );
      if( !strncmp( inStr.c_str(), "0000-00-00 00:00:00", 19 ) ) { zero(); return true;}
      if( strLen < 19 || strLen > 26 ) { fprintf( rpt, "TimeObj::setSQLDatetime: Malformed SQL datetime! s/b: YYYY-MM-DD HH:MM:SS[.XXXXXX], is: %s\n", inStr.c_str() ); return false; }
      if( !strptime( inStr.c_str(), "%Y-%m-%d %H:%M:%S", &tStruct ) ) { fprintf( rpt, "TimeObj::setSQLDatetime: The strptime() conversion failed for: %s\n", inStr.c_str() ); return false; }
      if( !mktime( &tStruct ) ) { fprintf( rpt, "TimeObj::setSQLDatetime: mktime() conversion failed for: %s\n", inStr.c_str() ); goto RESET; }
      if( strLen > 20 ) { // There is a fractional part
        if( inStr[19] == '.' ) {
         /* Crude trick */
          char microseconds[7]; // Six plus nul.
          int numMicroSecs;
          strcpy( microseconds, "000000" );
          std::string fract = inStr.substr( 20, strLen );
          size_t fractLen = fract.length();
          strncpy( microseconds, fract.c_str(), fractLen ); // For once, strncpy's habit of not writing nul at end is useful!!
          if( sscanf( microseconds, "%d", &numMicroSecs ) != 1 ) { fprintf( stderr, "TimeObj::setSQLDatetime: mktime() conversion failed for: %s\n", inStr.c_str() ); goto RESET; }
          t.tv_usec = numMicroSecs;
        } else { fprintf( rpt, "TimeObj::setSQLDatetime: mktime() conversion failed for: %s\n", inStr.c_str() ); goto RESET; } 
      }
      return true;
RESET:
      *this = keptCopy; 
      return false;
    }

#define TIME_BUF_LEN 32
  /** Get this TimeObj as a SQL string.  Rounded to the nearest one second: (YYYY-MM-DD HH:MM:SS)
   * @param outStr The result will be copied to this string, 19 characters long exactly.
   * @param rpt The default is to write to stderr.
   * @return bool true if successful */
    bool getSQLDatetime( std::string &outStr, FILE *rpt = stderr ) const 
    {
      TimeObj work( *this );
      struct tm tim;
      char out[TIME_BUF_LEN];
      size_t nChar;
      
      if( work.t.tv_usec >= 500000 ) work.t.tv_sec++;
      if( !gmtime_r( &(t.tv_sec), &tim ) ) { fprintf( rpt, "TimeObj::getSQLDatetime: gmtime_r() conversion failed!!\n" ); return false; }
      nChar = strftime( out, TIME_BUF_LEN, "%Y-%m-%d %H:%M:%S", &tim );
      if( !nChar ) { fprintf( rpt, "TimeObj::getSQLDatetime: strftime string conversion failed = numChars=|%d|!!\n", (int)nChar ); return false; }
      //fprintf( rpt, "strftime string conversion yielded numChars=|%d| and |%s|!!\n", nChar, out );
      outStr = out;
      return true;
    }

  /** Get this TimeObj as a SQL string.  Rounded to the nearest one second: (YYYY-MM-DD HH:MM:SS)
   * @return string true if successful */
    std::string getSQLDatetime() const 
    {
      std::string sqlT;
      if( getSQLDatetime( sqlT ) )
        return sqlT;
      else
        return std::string( "0000-00-00 00:00:00" );
    }


  /**
   * Get this TimeObj as a SQL string with six fractional values: (YYYY-MM-DD HH:MM:SS.XXXXXX)
   * @param outStr The result will be copied to this string, 27 characters long exactly.
   * @param rpt The default is to write to stderr.
   * @return bool */
    bool getSQLDatetimeFull( std::string &outStr, FILE *rpt = stderr ) const 
    {
      std::string johhny;
      char cut[80];
      
      johhny.resize(TIME_BUF_LEN);
      if( !getSQLDatetime( johhny, rpt ) ) return false;
      
      //sprintf( cut, "%06ld", t.tv_usec );
      sprintf( cut, "%06d", t.tv_usec );
      johhny += ".";
      johhny += cut;
      outStr = johhny;
      return true;
    }

  /** Get this TimeObj as a SQL string.  Rounded to the nearest one second: (YYYY-MM-DD HH:MM:SS)
   * @return string true if successful */
    std::string getSQLDatetimeFull() const 
    {
      std::string sqlT;
      if( getSQLDatetimeFull( sqlT ) )
        return sqlT;
      else
        return std::string( "0000-00-00 00:00:00.000000" );
    }

  /**
   * Set from date monkier, ie. '20110206' yields '2011-02-06 08:00:00' 
   * @param newMon The result will be copied to this string, Must be 8 characters long exactly.
   * @param rpt The default is to write to stderr.
   * @return bool */
    bool setMoniker( const int &newMon, FILE *rpt = stderr ) 
    {
      char buf[9];
      sprintf( buf, "%08d", newMon );
      std::string start = buf;
      std::string stMySql;
      stMySql += start.substr(0,4);
      stMySql += "-";
      stMySql += start.substr(4,2);
      stMySql += "-";
      stMySql += start.substr(6,2);
      stMySql += " 08:00:00"; 
      return setSQLDatetime( stMySql );
    }

  /**
   * Set from date monkier, ie. '20110206' yields '2011-02-06 08:00:00'
   * @param newMon The result will be copied to this string, Must be 8 characters long exactly.
   * @param rpt The default is to write to stderr.
   * @return bool */
    bool setMoniker( const std::string &newMon, FILE *rpt = stderr ) 
    {
      if( newMon.length() != 8 ) {
        std::cerr << "Improper moniker: " << newMon << " s/b 8 chars long!" << &std::endl;
        return false;
      }
      std::string stMySql;
      stMySql += newMon.substr(0,4);
      stMySql += "-";
      stMySql += newMon.substr(4,2);
      stMySql += "-";
      stMySql += newMon.substr(6,2);
      stMySql += " 08:00:00"; 
      return setSQLDatetime( stMySql );
    }


  /**
   * Set from date monkier, ie. '20110206' yields '2011-02-06 00:00:00'
   * @param newMon The result will be copied to this string, Must be 8 characters long exactly.
   * @param rpt The default is to write to stderr.
   * @return bool */
    bool setDayMoniker( const std::string &newMon, FILE *rpt = stderr )
    {
      std::string moniker = newMon;
      char tHr[3];
      if( moniker.length() != 8 ) return false;
      moniker.insert(4,"-");
      moniker.insert(7,"-");
      sprintf( tHr, "%02u", TimeObj::getTimeZone() );
      moniker.append(" ");
      moniker.append( tHr );
      moniker.append(":00:00");
      //fprintf( stderr, "moniker = %s\n", moniker.c_str() );
      setSQLDatetime( moniker, rpt );
      return true;
    }

  /**
   * Get the full date monkier, ie. '2011/02/06 08:09:12' yields '20110206'
   * @param dateStr The result will be copied to this string, 8 characters long exactly.
   * @return bool */
    bool getDayMoniker( std::string &dateStr ) const 
    {
      struct tm tmm;
      char scratch[64];
      if( !localtime( &tmm ) ) return false;
      if( sprintf( scratch, "%04d%02d%02d", tmm.tm_year+1900, tmm.tm_mon+1, tmm.tm_mday ) < 0 ) return false;
      if( strlen( scratch ) != 8 ) return false;
      dateStr = scratch;
      return true;
    }

  /**
   * Integer version of full date monkier.
   * @return the integer value */
    int getDayMoniker() const 
    {
      std::string mon;
      getDayMoniker( mon );
      return atoi( mon.c_str() );
    }

  /**
   * Get the full time monkier, ie. '2011/02/06 08:09:12' yields '080912'
   * @param timeStr The result will be copied to this string, 6 characters long exactly.
   * @return bool */
    bool getTimeMoniker( std::string &timeStr ) const 
    {
      struct tm tmm;
      char scratch[64];
      if( !localtime( &tmm ) ) return false;
      if( sprintf( scratch, "%02d%02d%02d", tmm.tm_hour, tmm.tm_min, tmm.tm_sec ) < 0 ) return false;
      if( strlen( scratch ) != 6 ) return false;
      timeStr = scratch;
      return true;
    }

  /**
   * Get the complete monkier, ie. '2011/02/06 08:09:12' yields '20110206_080912'
   * @param monikerStr The result will be copied to this string, 15 characters long exactly.
   * @return bool */
    bool getMoniker( std::string &monikerStr ) const 
    {
      std::string bud, bro;
      if( !getDayMoniker( bud ) ) return false;
      if( !getTimeMoniker( bro ) ) return false;
      bud += "_";
      bud += bro;
      if( bud.length() != 15 ) return false;
      monikerStr = bud;
      return true;
    }

  /**
   * Get the complete monkier, ie. '2011/02/06 08:09:12' yields '20110206_080912'
   * @param monikerStr The result will be copied to this string, 15 characters long exactly.
   * @return bool */
    std::string getMoniker() const 
    {
      std::string bud;
      if( !getMoniker( bud ) ) 
        return std::string( "00000000" );
      else
        return bud;
    }


public: // Time Operations

  /** Get a time far, far into the future
   * @return TimeObj */
    static TimeObj max() { return TimeObj(INT_MAX, 999999); }

  /** Compute the milliseconds difference.  If greater than ((2^31-1)/1000)/86400, returns -1
   * @return int */
    static int millisecondsDiff( TimeObj prior, TimeObj latter )
    { 
      if( latter < prior || ( latter - prior >= TimeObj( 2147483, 646978 ) ) ) return -1;
      double l = latter.t.tv_sec + latter.t.tv_usec / 1000000.0;
      double p = prior.t.tv_sec + prior.t.tv_usec / 1000000.0;
      return( ( l - p ) * 1000.0 );
    }


  /**
   * Get the current time (seconds since Jan 1, 1970)
   * @return TimeObj
   */
    static TimeObj getTimeOfDay()
    {
      TimeObj  tim;

      if (-1 == gettimeofday(&tim.t, NULL))
      perror("gettimeofday");

      return tim;
    }


  /** Convert hexadecimal time_t to numerical time_t.  Refuses if TZ != UTC.  
   *  Tolerant of file paths, if one or more slashes is found in 'hexDate'
   *  then it is treated as a path and the filname is plucked off the end.
   *  Also, strncmp is used to extract ONLY the first eight chars off of 
   *  'hexDate' slashes or not so if strlen(hexDate) > 8, no problem.  
   * @param  hexDate Hexadecimal time_t, eg: 4E04FF6E = 2011-06-24 21:19:42
   * @return time_t -1 if error */
    static time_t convertHexDateToTimeT( const char *hexDate );

  /** Convert numerical time_t to hexadecimal time_t.  Refuses if TZ != UTC.  
   * @param  daT
   * @param  hexDate Hexadecimal time_t, eg: 4E04FF6E = 2011-06-24 21:19:42
   * @return bool True if success. If false, hexDate was untouched. */
    static bool convertTimeTToHexDate( time_t daT, char *hexDate );

  /** Convert hexadecimal time_t to standard moniker.
    * Refuses if TZ != UTC.  
   * @param  hexDate Hexadecimal time_t, eg: 4E04FF6E = 2011-06-24 21:19:42
   * @param  asciiDate standard moniker: YYYYMMDD_HHMMSS
   * @return bool true if successful. */
    static bool convertHexDateToAscii( const char *hexDate, char *asciiDate );
    
  /** Convert standard moniker to hexadecimal time_t.
    * Refuses if TZ != UTC.  
   * @param  asciiDate standard moniker: YYYYMMDD_HHMMSS
   * @param  hexDate Hexadecimal time_t, eg: 4E04FF6E = 2011-06-24 21:19:42
   * @return bool true if successful. */
    static bool convertAsciiToHexDate( const char *asciiDate, char *hexDate );
    
  /** Compute offset to standard date moniker.
    * Refuses if TZ != UTC.  
   * @param  moniker standard date moniker: YYYYMMDD
   * @param  offset number of days +/-
   * @param  bDated The offset moniker
   * @return bool true if successful. */
    static bool backDater( const char* moniker, int offset, char *bDated );
    
  /** Compute offset to standard date moniker as ints.
   * @param  moniker standard date moniker: YYYYMMDD
   * @param  offset number of days +/-
   * @return bool offset moniker. */
    static int backDater( int moniker, int offset );
    
  /**
   * Set to the current time (seconds since Jan 1, 1970).
   */
    void setToTimeOfDay()
    {
      if (-1 == gettimeofday(&t, NULL))
        perror("gettimeofday");
    }

 
  /**
   * Set from the supplied value of time_t
   * @param  t
   * @return bool
   */
    void setTime_t ( time_t tt ) { t.tv_sec = tt; t.tv_usec = 0; }
  
  /**
   * Set zero time. (1970/01/01 00:00:00 UTC)
   */
    void zero() { t.tv_sec = 0; t.tv_usec = 0; }

  /** True if zero time.
   * @return bool */
    bool isZero() const { return( t.tv_sec == 0 && t.tv_usec == 0 ? true : false ); }

  /** Truncate fractional part of day
   * @return int */
    int getDayLength() const { return( t.tv_sec / 86400 ); }

  /** Return -1 unless exactly on day boundary
   * @return int */
    int getDayLengthStrict() const { 
      if( t.tv_sec % 86400 ) return -1;
      return( t.tv_sec / 86400 );
    }

  /**
   * Truncate fractional part.
   */
    void makeInt() { t.tv_usec = 0; }

  /**
   * Round up to next integer second.
   */
    void ceil() { t.tv_sec++; t.tv_usec = 0; }

  /**
   * Round to closest second.
   */
    void round()
    { 
      if( t.tv_usec > 499999 ) t.tv_sec++;
      t.tv_usec = 0;
    }


public: // Operators
  
  /**
   * Assignment Operation
   * @return TimeObj
   * @param  right
   */
    const TimeObj operator=( const TimeObj &right ) { t.tv_sec = right.t.tv_sec; t.tv_usec = right.t.tv_usec; return *this; }

  /**
   * Unary negation
   * @return TimeObj
   */
    TimeObj operator -() const { return (t.tv_usec == 0) ? TimeObj(- t.tv_sec, 0) : TimeObj(- t.tv_sec - 1, 1000000 - t.tv_usec); }

  /**
   * Destructive multiplication by scalar
   * @return TimeObj&
   * @param  s
   */
    TimeObj& operator *=(double s) { *this = *this * s; return *this; }

  /**
   * Division by scalar
   * @return TimeObj
   * @param  tml
   * @param  s
   */
    friend TimeObj operator /(const TimeObj &tml, double s) { return tml * (1.0 / s); }

  /**
   * Destructive division by scalar
   * @return TimeObj
   * @param  s
   */
    const TimeObj operator /=(double s) { return (*this = *this / s); }

  /**
   * Increment one day
   * @return TimeObj&
   */
    TimeObj& operator ++()
    {   
      setDatenum( getDatenum() + 1.0 );
      return *this;
    }

  /**
   * Increment one day
   * @return TimeObj
   */
    TimeObj operator ++( int )
    {   
      TimeObj ret(*this);
      ++(*this);
      return ret;
    }

  /**
   * Decrement one day
   * @return TimeObj
   * @param  t0
   */
    TimeObj& operator --()
    {   
      setDatenum( getDatenum() - 1.0 );
      return *this;
    }

  /**
   * Decrement one day
   * @return TimeObj
   */
    TimeObj operator --( int )
    {   
      TimeObj ret(*this);
      --(*this);
      return ret;
    }

  /**
   * Plus equals
   * @return TimeObj
   * @param  t0
   */
    TimeObj operator +=( const TimeObj &t0 )
    {   
      int sec; 
      long    usec;                   // System long
      sec =  t.tv_sec + t0.t.tv_sec;
      usec = t.tv_usec + t0.t.tv_usec;
      while (usec < 0 && sec > 0) {
          usec += 1000000;
          sec -= 1;
      }
      t.tv_sec = sec; t.tv_usec = usec;
      return *this;
    }

  /**
   * Minus equals
   * @return TimeObj
   * @param  t0
   */
    TimeObj operator -=( const TimeObj &t0 )
    {  
      int     sec; 
      long    usec; // System long
      sec =  t.tv_sec - t0.t.tv_sec;
      usec = t.tv_usec - t0.t.tv_usec;
      while (usec < 0 && sec > 0) {
        usec += 1000000;
        sec -= 1;
      }
      t.tv_sec = sec; t.tv_usec = usec;
      return *this;
    }

  /**
   * Times equals
   * @return TimeObj
   * @param  t0
   */
    TimeObj operator *=( const TimeObj &t0 ) { return *this; }

  /**
   * Divide equals
   * @return TimeObj
   * @param  t0
   */
    TimeObj operator /=( const TimeObj &t0 ) { return *this; }

  /**
   * Modulo equals
   * @return TimeObj
   * @param  t0
   */
    TimeObj operator %=( const TimeObj &t0 ) { return *this; }

  /**
   * Multiplication by scalar
   * @return TimeObj
   * @param  tml
   * @param  s
   */
    friend TimeObj operator *(const TimeObj &tml, double s) { return TimeObj(tml.get() * s); }

  /**
   * Multiplication by scalar
   * @return TimeObj
   * @param  s
   * @param  tml
   */
    friend TimeObj operator *(double s, const TimeObj &tml) { return tml * s; }

  /**
   * Plus binary operator
   * @return TimeObj
   * @param  t0
   * @param  t1
   */
    friend TimeObj operator +( const TimeObj &t0, const TimeObj &t1 )
    {  
      TimeObj tm(t0.t.tv_sec + t1.t.tv_sec,
      t0.t.tv_usec + t1.t.tv_usec);
      if (tm.t.tv_usec >= 1000000) {
          tm.t.tv_sec  += 1;
          tm.t.tv_usec -= 1000000;
      }
      return tm;
    }

  /**
   * Plus minus operator
   * @return TimeObj
   * @param  t0
   * @param  t1
   */
    friend TimeObj operator -( const TimeObj &t0, const TimeObj &t1 )
    {  
        int sec; 
        long    usec; // System long
        sec =  t0.t.tv_sec - t1.t.tv_sec;
        usec = t0.t.tv_usec - t1.t.tv_usec;
        while (usec < 0 && sec > 0) {
            usec += 1000000;
            sec -= 1;
        }
        return TimeObj(sec, usec);
    }

  /**
   * Plus multiplication operator
   * @return TimeObj
   * @param  t0
   * @param  t1
   */
    friend TimeObj operator *( const TimeObj &t0, const TimeObj &t1 ) { return TimeObj(); }

  /**
   * Plus division operator
   * @return TimeObj
   * @param  t0
   * @param  t1
   */
    friend TimeObj operator /( const TimeObj &t0, const TimeObj &t1 ) { return TimeObj(); }

  /**
   * Plus modulo operator
   * @return TimeObj
   * @param  t0
   * @param  t1
   */
    friend TimeObj operator %( const TimeObj &t0, const TimeObj &t1 ) { return TimeObj(); }
    
   // Comparison Operators

  /**
   * Equality operator
   * @return bool
   * @param  tml
   * @param  tmr
   */
    friend bool operator ==( const TimeObj &tml, const TimeObj &tmr )
    { 
      return (tml.t.tv_sec == tmr.t.tv_sec) && (tml.t.tv_usec == tmr.t.tv_usec); 
    }


  /**
   * Inequality operator
   * @return bool
   * @param  tml
   * @param  tmr
   */
    friend bool operator !=( const TimeObj &tml, const TimeObj &tmr )
    { 
      return (tml.t.tv_sec != tmr.t.tv_sec) || (tml.t.tv_usec != tmr.t.tv_usec); 
    }

  /**
   * Less than operator
   * @return bool
   * @param  tml
   * @param  tmr
   */
    friend bool operator <( const TimeObj &tml, const TimeObj &tmr )
    {
      unsigned int tl, tr, tul, tur;
      tl = tml.t.tv_sec;
      tr = tmr.t.tv_sec;
      tul = tml.t.tv_usec;
      tur = tmr.t.tv_usec;
      if( tl < tr )
        return true;
      else if( tl > tr )
        return false;
      else if( tul < tur ) // tml.t.tv_sec == tmr.t.tv_sec
        return true;
      else
        return false;
    }

  /**
   * Greater than operator
   * @return bool
   * @param  tml
   * @param  tmr
   */
    friend bool operator >( const TimeObj &tml, const TimeObj &tmr )
    {
      if( tml.t.tv_sec > tmr.t.tv_sec )
        return true;
      else if( tml.t.tv_sec < tmr.t.tv_sec )
        return false;
      else if( tml.t.tv_usec > tmr.t.tv_usec ) // tml.t.tv_sec == tmr.t.tv_sec
        return true;
      else
        return false;
    }

  /**
   * Less than or equal operator
   * @return bool
   * @param  tml
   * @param  tmr
   */
    friend bool operator <=( const TimeObj &tml, const TimeObj &tmr )
    {
      if ((tml.t.tv_sec < tmr.t.tv_sec) || (tml.t.tv_sec == tmr.t.tv_sec && tml.t.tv_usec <= tmr.t.tv_usec))
        return true;
      else
        return false;
    }

  /**
   * Greater than or equal operator
   * @return bool
   * @param  tml
   * @param  tmr
   */
    friend bool operator >=( const TimeObj &tml, const TimeObj &tmr )
    {
      if ((tml.t.tv_sec > tmr.t.tv_sec) || (tml.t.tv_sec == tmr.t.tv_sec && tml.t.tv_usec >= tmr.t.tv_usec))
        return true;
      else
        return false;
    }

};

#endif // __TIMEOBJ_H__ 


