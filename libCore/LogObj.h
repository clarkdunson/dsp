
#ifndef __LOGOBJ_H__
#define __LOGOBJ_H__

#include "DSPCommon.h"

//#define USE_LOCK

/**
  * class LogObj
  * Copyright 2016, ShotSpotter
  * Provide uniform interface to log functionality.  Every process using a log
  * instantiates a LogObj object prior to use, and deletes it when finished.  Most
  * usage is to instantiate at launch.  Delete at exit.  The log is 
  * written to /tmp during use to minimize network use. This /tmp file is then
  * closed and moved to logDir if logDir != tmp
  */

class LogObj
{
  
protected: // Protected attributes
  
  /** Name of program using log facility */
    std::string prgName;
  
  /** Name root of this log */
    std::string nameRoot;
  
  /** Name of log file */
    std::string fileName;

#ifdef USE_LOCK
  /** Lock to encapsulate fileName */
    LockObj loglock;
#endif // USE_LOCK
  
  /** Number of station affected */ 
  std::string staNum;
  
  /** Open file pointer */
    FILE   *fid;

  /** Write a persistent log */
    bool   persistent;

  /** Place where logs are kept. Points at either /tmp
   * or $LOG_PATH.  (With trailing '/' character) */
    static std::string logDir;

  /** Place where logs are kept. Points at /tmp nominally. */
    static const std::string scratchDir;

public: // Initialize & Test
  
  /** Place where logs are kept. Points at either /tmp
   * or $LOG_PATH.  (With trailing '/' character) */
    static const std::string& getLogDir() { return logDir; }
    
  /** Initialize this class for use.  Must call before use!!! 
    * If $LOG_PATH does not exist, all file activity for
    * this program will take place in /tmp. */
    static void initClass();
    
  /** Run the regression test for this class.  Return 0 if good.
   * @return bool */
    static bool testClass();

public: // Constructors/Destructors
  
  /** Empty Constructor */
    LogObj()
    { 
      prgName = "default";
      staNum = "none";
      fid = stderr;
      fileName = "";
      persistent = false;
    }
    
  /** Destructor closes log file and moves it to logDir  */
    virtual ~LogObj ();

  /** Log file name is built out of four elements. */
    void buildFileName( pid_t daPid = -1 );
  
  /** Get log file name as built, or empty.
   * @return const std::string& */
    const std::string& getFileName() const { return fileName; }
  
  /** Get log file as it will be placed in data center.
   * @return const std::string& */
    void getFullName( std::string &fullPath ) const;
    
  /** Get log file working name (valid until last act of destructor.
   * @return const std::string& */
    void getTmpName( std::string &tmpPath ) const { 
      tmpPath = scratchDir; 
      tmpPath += fileName; 
    }
  
  /** Defer Initialization until all values to "safe".  We are careful
   * about prg, as argv[0] has the full path in it
   * @param  prg Pass 'stderr' if you wanto to write only to io.  If you pass a std::string that ends in ".log" this name overrides the built name.
   * @param  _staNum The station number
   * @param  _lockFile Optional lock file name 
   * @return bool false if already initialized */
    bool init( const char* prg, const std::string _staNum, const std::string _lockFile = "" );

  /** Same as init, but makes a persistent log, and appends the current log file to it.
   * @param  prg Pass 'stderr' if you wanto to write only to io.  If you pass a std::string that ends in ".log" this name overrides the built name.
   * @param  _staNum The station number
   * @param  _lockFile Optional lock file name 
   * @return bool false if already initialized */
    bool initPersistent( const char* prg, const std::string _staNum, const std::string _lockFile = "" );

  /** Defer Initialization until all values to "safe".  We are careful
   * about prg, as argv[0] has the full path in it
   * @param  fName Pass any filename
   * @return bool false if already initialized */
    bool init( const char* fName );

  /** Generalized message with process killer at end.
   * @param  fmt standard printf formats.  Cannot be const
   * as it also unlocks.  Be careful with die().  It calls
   * exit(), and as such can miss calling destructors for
   * some objects.
   * @param  ... varargs */
    void die( const char *fmt, ... );

  /** printf style message.
   * @param  typer WARNING, INFO, Debug, etc. 
   * @param  fmt standard printf formats 
   * @param  ... varargs */
    void msg( const char* typer, const char *fmt, ... ) const;

  /** printf style message.  translate stat with errorstr
   * @stat system error code. 
   * @param  typer WARNING, INFO, Debug, etc. 
   * @param  fmt standard printf formats 
   * @param  ... varargs */
/*
    void statMsg( int stat, const char* typer, const char *fmt, ... ) const;
*/

  /** printf style killer-eunuch, that can be called prior to LogObj::initClass().
   * @param  fmt standard printf formats.  bail() also calls exit.
   * @param  ... varargs */
    static void bail( const char *fmt, ... );
    
  /** printf style killer-eunuch, that can be called prior to LogObj::initClass().
   * @param  fmt standard printf formats 
   * @param  ... varargs */
    static void warn( const char *fmt, ... );
    
  /** Get the FILE* pointer. Unfortunately, for likely a very good reason,
   * The fprintf protoype type does not use 'const' for the FILE* parameter. 
   * From man page:
   *    int fprintf(FILE *stream, const char *format, ...);
   * A proper design would demand this function return a 'const FILE*' but
   * then we would forever be using const_cast.  So yes, the log design is
   * vulnerable to you doing this: fclose(xx.getFid());  PLEASE DON'T!!!
   * See TimeObj::setSQLDatetime() for an example of this type of usage.
   * @return FILE* */
    FILE* getFid() const { return fid; }
    
  /** Close the file, leave the name intact for reopen()
   * @return int The return code from fclose, unless fid==stderr */
    int close() { 
      if( fid != stderr ) { 
        int stat = fclose(fid); 
        fid = stderr;
        return stat; 
      } else
        return 0;
    }
    
  /** Re-open the file,
   * @return bool Returns true if the file was re-opened. */
    bool reopen() {
      if( fid == stderr && prgName != "stderr" ) { 
        std::string working;
        getTmpName( working );
        fid = fopen( working.c_str(), "a" );
        if( fid ) return true;
        else {
          fprintf( stderr, "Could not open %s, reverting to stderr\n!!", fileName.c_str() );
          fid = stderr;
        }
      }
      return false; 
    }

  /** Append a file to the log.  Close file, use cat, re-open file.
   * @return bool Returns true if successful  If prgName==stderr, then
   * open the file and print it stderr, and then close. */
    bool append( const std::string& appendix );
    
};

#endif // __LOGOBJ_H__
