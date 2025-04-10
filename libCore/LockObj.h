
#ifndef __LOCKOBJ_H__
#define __LOCKOBJ_H__

#include "DSPCommon.h"

/**
  * class LockObj
  * Copyright 2016, ShotSpotter
  * Implements the standard pid file locking mechanism
  */
  
class LockObj
{
  
  // Private attributes
private:
  std::string lockFile;

  /** Place where locks are kept. Points at either /tmp
   * or $LOCK_PATH.  (With trailing '/' character) */
  static std::string lockDir;
  
  /** Place where hostname is kept. */
  static std::string hostName;
  

public: // Initialize & Test
      
  /** Initialize this class for use.  Must call before use!!! */
    static void initClass();
    
  /** Run the regression test for this class.  Return 0 if good.
   * @return bool */
    static bool testClass();

  /** Run the regression test for this class.  Return 0 if good.
   * @return bool */
    static const std::string& getLockDir() { return lockDir; }

public: // Constructors/Destructors
    
  /**
   * Empty Constructor
   */
    LockObj() { lockFile = ""; } 

  /**
   * Empty Destructor
   */
    virtual ~LockObj();


public: // Working methods

  /**
   * Creates a lock file name $LOCK_PATH/lock.tag
   * @param  tag
   */
    void buildLockFileName( std::string tag ) 
    {
      lockFile = lockDir;
      lockFile += "lock.";
      lockFile += tag;
      lockFile += ".pid";
    }
    
  /**
   * Creates the file "lockFile" with the pid for this process inside it.
   * @param  tag
   * @return pid_t Returns zero (0) if successful, -1 if failure, or a positive
   * pid number for the process that already had it locked.
   */
    pid_t lock( std::string tag );
    
  /**
   * Alternate for char, see above.
   * @param  lockName
   * @return pid_t Returns zero (0) if successful.
   */
    pid_t lock( const char* lockName ) { return lock(std::string(lockName)); }


  /**
   * Deletes the file "lockFile" with the pid for this process.
   * Returns zero (0) if successful, -1 if failure, or a positive
   * pid number for an outside process that owns the lock.
   * @return pid_t
   */
    pid_t unlock();

  /**
   * Tests lock for this process.
   * @return int
   */
    int locked();


  /**
   * Return the name of the lock file as a C++ std::string
   * @return std::string
   */
    std::string getLockFileName() { return lockFile; }

  /**
   * Return the name of the lock file as a C char*
   * @return const char*
   */
    const char* lockName() { return lockFile.c_str(); }

};

#endif // __LOCKOBJ_H__
