#include "LogObj.h"
#include "TimeObj.h"

/**
  * class LogObj
  * Copyright 2016, ShotSpotter
  * Provide uniform interface to log functionality.  Every process using a log
  * instantiates a LogObj object prior to use, and deletes it when finished.  Most
  * usage is to instantiate at launch.  Delete at exit.  The log is 
  * written to /tmp during use to minimize network use. This /tmp file is then
  * closed and moved to logDir if logDir != tmp
  */

const std::string LogObj::scratchDir = "/tmp/";


std::string LogObj::logDir = "";

LogObj::~LogObj () 
{ 
  std::string cmd;
  if( fid != stderr ) { 
    fclose(fid);
    std::string src, targ;
    getTmpName( src );
    getFullName( targ );
#if 0
    Path tmpr( src );
    if( tmpr.bytes() > 0 ) {
      //fprintf( stderr, "src = %s, targ = %s\n", src.c_str(), targ.c_str() );
      if( src != targ ) {
        if( persistent ) {
          cmd = "cat ";
          cmd += src;
          cmd += " >> ";
          cmd += targ;
        } else {
          cmd = "cp ";
          cmd += src;
          cmd += " ";
          cmd += targ;
        }          
      //fprintf( stderr, "cmd = %s\n", cmd.c_str() );
        if( system( cmd.c_str() ) )
          fprintf( stderr, "LogObj::~LogObj() Problem copying log to dir!", src.c_str(), targ.c_str() );
        else {
          cmd = "rm ";
          cmd += src;
          if( system( cmd.c_str() ) )
            fprintf( stderr, "LogObj::~LogObj() Problem deleting local scratch log!" );
        }
      }
    }
#endif
  }
}

void
LogObj::initClass() 
{
  char *logPath;
  struct stat flunky;
  
  if( ( logPath = getenv( "LOG_PATH" ) ) ) {
    
    if( stat( logPath, &flunky ) ) {
      fprintf( stderr, "LogObj::initClass(): LOG_PATH defined but does not exist!!\n" );
      goto DEFAULT;
    }

    logDir = logPath;
    logDir += "/";
//fprintf( stderr, "logDir = %s\n", logDir.c_str() );
    return;
  }
  
DEFAULT :
  logDir = scratchDir;  // Must end in '/'
}

void
LogObj::buildFileName( pid_t daPid ) 
{
  char fName[1024];
  int len;
  TimeObj nowT;
  std::string pupp;
  
 // Name root allows persistent logging to be easy.
  nameRoot = prgName;
  nameRoot += ".";
  if( staNum.length() == 4 ) {
    nameRoot += staNum;
    nameRoot += ".";
  }

  strcpy( fName, nameRoot.c_str() );

  nowT.setToTimeOfDay();
  if( nowT.getDayMoniker( pupp ) ) {
    strcat( fName, pupp.c_str() );
    strcat( fName, "." );
  }
  if( nowT.getTimeMoniker( pupp ) ) {
    strcat( fName, pupp.c_str() );
    strcat( fName, "." );
  }
  len = strlen( fName );
  if( daPid == -1 )
    sprintf( fName+len, "%d", getpid() );
  else
    sprintf( fName+len, "%d", daPid );
    
  strcat( fName, ".log" );
  
  fileName = fName;
}

void
LogObj::getFullName( std::string &fullPath ) const 
{ 
  if( persistent ) {
fprintf( stderr, "logDir = %s, nameRoot = %s\n", logDir.c_str(),  nameRoot.c_str() );

    fullPath = logDir; fullPath += nameRoot; fullPath += "log";
  } else {
    fullPath = logDir; fullPath += fileName;
  }
}


bool 
LogObj::init( const char* prg, const std::string _staNum, const std::string _lockFile )
{
  char *lastSlash;
  if( fileName.length() ) return false;
  if( !strcmp( prg, "stderr" ) ) { fileName = prg; fid = stderr; staNum = _staNum; return true; }
  if( !(lastSlash = const_cast<char*>(strrchr( prg, '/' ))) ) 
    prgName = prg;
  else {
    char prgNameTmp[256];
    strcpy( prgNameTmp, lastSlash+1 );
    prgName = prgNameTmp;
  }
  
 /* Enforce three digit sid, but accept 4 */
  if ( _staNum.length() == 4 )
    staNum = _staNum;
  else if ( _staNum.length() == 3 ) {
    staNum = "0";
    staNum += _staNum;
  } else {
   // It's not site specific
    staNum = "";
  }
  
  fid = NULL;
  
  // Now construct franken-log name
  buildFileName();

  std::string working;
  getTmpName( working );

  
  if( !(fid = fopen( working.c_str(), "w" ) ) ) {
    fid = stderr;
    fprintf( fid, "Warning: Could not open log file!  Re-directing to stderr.\n" );
    return false;
  }

  TimeObj nowl = TimeObj::getTimeOfDay();
  std::string timee;
  nowl.getSQLDatetimeFull( timee );
  char hhost[64];
  gethostname( hhost, 62 );
  msg( "INFO", "Log started at: %s, on: %s", timee.c_str(), hhost );
  
#ifdef USE_LOCK
  loglock.lock( _lockFile );
#endif // USE_LOCK
  return true;
}


bool
LogObj::initPersistent( const char* prg, const std::string _staNum, const std::string _lockFile )
{
  persistent = true;
  return init( prg, _staNum, _lockFile );
}



bool 
LogObj::init( const char* fName )
{
  char *lastSlash;
  if( fileName.length() ) return false;
  if( !strcmp( fName, "stderr" ) ) { fileName = fName; fid = stderr; staNum = "none"; return true; }
  if( !(lastSlash = const_cast<char*>(strrchr( fName, '/' ))) ) {
    prgName = fName;
    logDir = ""; // For this usage blaze over
  } else {
    char nameTmp[1024];
    size_t pathLen = lastSlash - fName;
    strncpy( nameTmp, fName, pathLen );
    nameTmp[pathLen] = 0x00;
    //fprintf( stderr, "nameTmp = %s\n", nameTmp );
    prgName = nameTmp;
  }
  fileName = prgName;
  staNum = "none";
  fid = NULL;
  
  
  std::string working;
  getTmpName( working );
  if( !(fid = fopen( working.c_str(), "w" ) ) ) {
    fid = stderr;
    fprintf( fid, "Warning: Could not open log file!  Re-directing to stderr.\n" );
    return false;
  }
  
  TimeObj nowl = TimeObj::getTimeOfDay();
  std::string timee;
  nowl.getSQLDatetimeFull( timee );
  char hhost[64];
  gethostname( hhost, 62 );
  msg( "INFO", "Log started at: %s, on: %s", timee.c_str(), hhost );
  
#ifdef USE_LOCK
  std::string dud = "";
  loglock.lock( dud );
#endif // USE_LOCK
  return true;
}


void
LogObj::die( const char *fmt, ... )
{
  va_list   ap;
  int       ofr;
  char      logStr[4096];

  va_start( ap, fmt );
  ofr = sprintf( logStr, "FATAL: " );
  ofr += vsprintf ( logStr+ofr, fmt, ap );
/*ofr +=*/sprintf( logStr+ofr, ": errno=%d: '%s'", errno, strerror(errno) );
  va_end( ap );

  TimeObj nowl = TimeObj::getTimeOfDay();
  std::string timee;
  nowl.getSQLDatetimeFull( timee );
  
  fprintf( fid, "%s: CMN%s: %s: %s\n", prgName.c_str(), staNum.c_str(), timee.c_str(), logStr );

  #ifdef CONSOLE_OUTPUT
  fprintf( stderr, "%s: CMN%s: %s\n", prgName.c_str(), staNum.c_str(), logStr );
  #endif // CONSOLE_OUTPUT

  
#ifdef USE_LOCK
  /* Clear PID lock file */
  if( loglock.locked() ) loglock.unlock();
#endif // USE_LOCK

  /* Close lock file */
  if( fid != stderr ) fclose(fid);
  
  /// XXX Clark.  Be careful with your destructors, dynamically allocated objects are missed!
  exit(1);
}


void
LogObj::msg( const char* typer, const char *fmt, ... ) const
{
  va_list   ap;
  int       ofr;
  char      logStr[1024];

  va_start( ap, fmt );
  ofr = sprintf( logStr, "%s: ", typer );
  vsprintf ( logStr+ofr, fmt, ap );
  va_end( ap );

  TimeObj nowl = TimeObj::getTimeOfDay();
  std::string timee;
  nowl.getSQLDatetimeFull( timee );
  
  fprintf( fid, "%s: CMN%s: %s: %s\n", prgName.c_str(), staNum.c_str(), timee.c_str(), logStr );

  #ifdef CONSOLE_OUTPUT
  fprintf( stderr, "%s: CMN%s: %s\n", prgName.c_str(), staNum.c_str(), logStr );
  #endif // CONSOLE_OUTPUT
  /* Note all in the same file system, mv uses rename, ie. it is effectively atomic */
}

/*
void
LogObj::statMsg( int statr, const char* typer, const char *fmt, ... ) const
{
  va_list   ap;
  int       ofr;
  char      logStr[2048];
  char      cmd[4096];
  std::string    logMsg;

  va_start( ap, fmt );
  ofr = sprintf( logStr, "%s: ", typer );
  vsprintf ( logStr+ofr, fmt, ap );
  va_end( ap );

  TimeObj nowl = TimeObj::getTimeOfDay();
  std::string timee;
  nowl.getSQLDatetimeFull( timee );


  logMsg = logStr;
  logMsg += "System error std::string: ";
  if( strerror_r( statr, cmd, 4096 ) < 0 )
    logMsg += "Unknown";
  else
    logMsg += statr;
  
  fprintf( fid, "%s: CMN%s: %s: %s\n", prgName.c_str(), staNum.c_str(), timee.c_str(), logMsg.c_str() );

  #ifdef CONSOLE_OUTPUT
  fprintf( stderr, "%s: CMN%s: %s\n", prgName.c_str(), staNum.c_str(), logMsg.c_str() );
  #endif // CONSOLE_OUTPUT
  // Note all in the same file system, mv uses rename, ie. it is effectively atomic
}
*/



void
LogObj::bail( const char *fmt, ... )
{
  va_list   ap;
  int       ofr;
  char      logStr[4096];

  va_start( ap, fmt );
  ofr = sprintf( logStr, "FATAL: " );
  ofr += vsprintf ( logStr+ofr, fmt, ap );
/*ofr +=*/sprintf( logStr+ofr, ": errno=%d: '%s'", errno, strerror(errno) );
  va_end( ap );

  fprintf( stderr, "LogObj::bail() %s\n", logStr );

  exit(1);
}


void
LogObj::warn( const char *fmt, ... )
{
  va_list   ap;
  int       ofr;
  char      logStr[4096];

  va_start( ap, fmt );
  ofr = sprintf( logStr, "WARN: " );
  ofr += vsprintf ( logStr+ofr, fmt, ap );
/*ofr +=*/sprintf( logStr+ofr, ": errno=%d: '%s'", errno, strerror(errno) );
  va_end( ap );

  fprintf( stderr, "LogObj::warn() %s\n", logStr );
}

#define APP_BUF_SIZE 4096
bool
LogObj::append( const std::string& appendix )
{
  char cmd[APP_BUF_SIZE];
  FILE *outr;
  if( appendix.length() == 0 ) return false;
  
  FILE* appFid = fopen( appendix.c_str(), "r" );
  if( !appFid ) {
    fprintf( fid, "LogObj::append() Open of apendee |%s| failed!", appendix.c_str() );
    return false;
  }

  if( fileName == "stderr" ) // 'Append' the file to stderr
    outr = stderr;
  else 
    outr = fid;
  
  while( fgets( cmd, APP_BUF_SIZE, appFid ) ) {
    fprintf( outr, "%s", cmd );
  }
  
  fclose( appFid );
  return true;
}



