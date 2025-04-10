#include "LockObj.h"

/**
  * class LockObj
  * Copyright 2016, ShotSpotter
  * Implements the standard pid file locking mechanism
  */

enum LockObjStates {
  UNLOCKED,
  LOCKED,
  FURRENER,
  numLockObjStates
};

std::string LockObj::lockDir = "";
std::string LockObj::hostName = "";


LockObj::~LockObj() { 
  if( locked() ) fprintf( stderr, "LockObj quitting whilst LOCKED!!!\n" ); 
}


#define HOST_NAME_LEN 64

void
LockObj::initClass() 
{
  char *lockPath;
  char cmd[1024];
  char hostNameBuf[HOST_NAME_LEN];

  if( gethostname( hostNameBuf, HOST_NAME_LEN ) )
    fprintf(stderr, "LockObj::initClass() hostname fetch failed!!!!!!\n" ); 
  else
    hostName = hostNameBuf;

  if( ( lockPath = getenv( "LOCK_PATH" ) ) ) {
      /** Remake Lock dir */
      strcpy( cmd, "mkdir -p " );
      strcat( cmd, lockPath );
      //fprintf( stderr, "lockDir = %s\n", cmd );
      if( !system( cmd ) ) {
       /** We made it so ... */
        strcpy( cmd, "chmod 777 " );
        strcat( cmd, lockPath );
        if( !system( cmd ) ) {
          lockDir = lockPath;
          lockDir += "/";
          //fprintf( stderr, "lockDir = %s\n", lockDir.c_str() );
          return;
        }
      }
  }

  /** Default */
  lockDir = "/tmp/";
}


bool
LockObj::testClass() 
{

  LockObj a;
  LockObj b;
  
  if( a.lock( "703" ) != 0 ) goto FUPDUCK;
  fprintf( stderr, "---- The next line is not an error, but a proper result\n" );
  if( b.lock( "703" ) <= 0 ) goto FUPDUCK;
  fprintf( stderr, "---- The prior line is not an error, but a proper result\n" );
  if( b.lock( "705" ) != 0 ) goto FUPDUCK;

  if( a.unlock() ) goto FUPDUCK;
  if( b.unlock() ) goto FUPDUCK;
  
  return false;  // Voila
  
FUPDUCK :
  fprintf( stderr, "FUPDUCK: LockObj regression failed!!!\n" );
  return true;

}


#define PID_BUF_LENGTH 16

pid_t
LockObj::lock( std::string tag )
{
 /* Trap all usage */
  if( !tag.length() ) return -1;
  
  FILE *lockFid;
  buildLockFileName( tag );

 // Check if already locked ...
  if( ( lockFid = fopen( lockFile.c_str(), "r" ) ) ) {
    pid_t preLockedPid;
    char hName[128];
    if( fscanf( lockFid, "%d", &preLockedPid ) < 1 || fscanf( lockFid, "%s", hName ) < 1 ) {
      fprintf(stderr, "LockObj::lock() fscanf pid conversion failed %d for host %s!!!!!!\n", preLockedPid, hName );
      fclose( lockFid );
      return -1;
    } else {
      if( getpid() != preLockedPid )
        fprintf(stderr, "Error pid Lockfile already EXISTS for OTHER PID %d for host %s!!!!!!\n", preLockedPid, hName );
      else
        fprintf(stderr, "Error pid Lockfile already EXISTS for THIS PID %d for host %s!!!!!!\n", preLockedPid, hName );
      lockFile = "";
      fclose( lockFid );
      tag = hName;
      return preLockedPid;
    }
  }

 // Clear to lock now
  if( !(lockFid = fopen( lockFile.c_str(), "w" )) ) {
    fprintf(stderr, "Pid Locking FAILED!!!!!!\n" );
    lockFile = "";
    return -1;
  } else {
    fprintf( lockFid, "%d\n%s\n", getpid(), hostName.c_str() );
    fclose( lockFid );
    return 0;
  }
}


pid_t
LockObj::unlock()
{
  FILE *lockFid;
  
  if( lockFile.length() > 0 ) {
  // Check rigorously ...
    if( ( lockFid = fopen( lockFile.c_str(), "r" ) ) ) {
      pid_t alreadyLockedPid;
      char pidBuf[PID_BUF_LENGTH];
      
      if( !fgets( pidBuf, PID_BUF_LENGTH, lockFid ) ) {
        fclose( lockFid );
        fprintf(stderr, "unlock() fgets() pid file read failed!!!!!!\n"  );
        return -1;
      }
      fclose( lockFid );

      if( sscanf( pidBuf, "%d", &alreadyLockedPid ) != 1 ) {
        fprintf(stderr, "unlock() sscanf() pid conversion failed %d!!!!!!\n", alreadyLockedPid  );
        return -1;
      }
      
      if( getpid() != alreadyLockedPid ) {
        fprintf(stderr, "Pid Lockfile PID does not match this PID %d!!!!!!\n", alreadyLockedPid  );
        return alreadyLockedPid;
      }
    }
    
    if( unlink( lockFile.c_str() ) == -1 ) {
      fprintf( stderr, "Warning lock file could not be deleted!! errno = %d|%s\n", errno, strerror( errno ) );
      return -1;
    } else {
      lockFile = "";
      return 0; // Success
    }
  } else {
    fprintf( stderr, "Warning unlock() called when NOT LOCKED!!!\n" ); 
    return -1;
  }
}


int
LockObj::locked() 
{ 
  FILE *fid = NULL;
  pid_t foundPid; 
  
  char pidBuf[PID_BUF_LENGTH]; 
  if( !lockFile.length() ) return UNLOCKED;
  
  if( !( fid = fopen( lockFile.c_str(), "r" ) ) ) return UNLOCKED;
  
  if( !fgets( pidBuf, PID_BUF_LENGTH, fid ) ) {
    fclose( fid );
    return FURRENER;
  }
  fclose( fid );
  
  if( !(sscanf( pidBuf, "%d", &foundPid ) != 1) ) return FURRENER;
  
  if( foundPid == getpid() ) return LOCKED;
  else return FURRENER;  
}
