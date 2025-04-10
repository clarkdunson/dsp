#include "TimeObj.h"

/*
 Copyright ShotSpotter, 2016
*/

unsigned int TimeObj::tzOff = 0;

void
TimeObj::initClass() 
{
  char* timeZone=getenv("TZ");
  if( !timeZone ) {
    fprintf( stderr, "Time zone not set, must be UTC!!\n" );
    exit(1); // The law, dammit.
  }
  if( strcmp( timeZone, "UTC" ) ){
    fprintf( stderr, "Time zone set incorrectly, must be UTC!!\n" );
    exit(1); // The law, dammit.
  }

 /* Only for any children are ever spawned.
    Has no effect on this process */
  putenv( const_cast<char*>("TZ=UTC") );
  tzset(); 
}

TimeObj::TimeObj( double sec )
{ 
  if( sec >= 0 ) {
    t.tv_sec = trunc((double)sec);
    t.tv_usec = (sec - t.tv_sec) * 1000000.0;
  } else
      *this = -TimeObj(-sec); 
}

void
TimeObj::set( double sec ) 
{ 
  t.tv_sec = time_t(trunc(sec));  
  t.tv_usec = long((sec - t.tv_sec) * 1000000.0);
}

void
TimeObj::setDatenum( double datenum )
{
  // 719529 = 1970/01/01
  double dnSecs = (datenum-719529) * 86400;
  t.tv_sec = floor( dnSecs );
  t.tv_usec = long( ( dnSecs - t.tv_sec ) * 1000000 );
  //std::cout << "secs: " << t.tv_sec << ", and usecs: " << t.tv_usec << &endl;
}




const 
char*  
TimeObj::format( char* buf, const char *fmt ) const
{
    bool        negative;
    struct timeval  tv;
    char   defFmt[] = "%S.%i";
    
    if( !fmt ) fmt = defFmt;

    // turn into sign-magnitude form
    if (t.tv_sec >= 0) {
    negative = 0;
    tv = t;
    }
    else {
    negative = 1;
    tv = (-(*this)).t;
    }

    // first calculate total durations
    const int tday = tv.tv_sec / (SECS_PER_DAY);
    const int thour = tv.tv_sec / (SECS_PER_HR);
    const int tmin = tv.tv_sec / (MINS_PER_HR);
    const int tsec = tv.tv_sec;
    const long long tmilli = ((long long)1000) * ((long long)tv.tv_sec) + ((long long)tv.tv_usec) / 1000;
    const long long tmicro = ((long long)1000000) * ((long long)tv.tv_sec) + ((long long)tv.tv_usec);

    // then calculate remaining durations
    const int rhour = thour - 24 * tday;
    const int rmin  = tmin -  60 * thour;
    const int rsec  = tsec -  60 * tmin;
    const long long rmilli = tmilli - ((long long)1000) * tsec;
    const long long rmicro = tmicro - ((long long)1000000) * tsec;
    
    //char buf[200];
    char *s = buf;

    for (; *fmt; fmt++) {
    if (*fmt != '%')
        *s++ = *fmt;
    else
        switch(*++fmt) {
          case 0:
        fmt--;  // trailing '%' in format string
        break;

          case '%':
        *s++ = '%'; // "%%" in format string
        break;

          case 'D':
        if (negative) *s++ = '-';
        s += sprintf(s, "%d", tday);
        break;

          case 'H':
        if (negative) *s++ = '-';
        s += sprintf(s, "%d", thour);
        break;

          case 'M':
        if (negative) *s++ = '-';
        s += sprintf(s, "%d", tmin);
        break;

          case 'S':
        if (negative) *s++ = '-';
        s += sprintf(s, "%d", tsec);
        break;

          case 'I':
        if (negative) *s++ = '-';
        s += sprintf(s, "%lld", tmilli);
        break;

          case 'U':
        if (negative) *s++ = '-';
        s += sprintf(s, "%lld", tmicro);
        break;

          case 'h':
        s += sprintf(s, "%.2d", rhour);
        break;

          case 'm':
        s += sprintf(s, "%.2d", rmin);
        break;

          case 's':
        s += sprintf(s, "%.2d", rsec);
        break;

          case 'i':
        s += sprintf(s, "%.3lld", rmilli);
        break;

          case 'u':
        s += sprintf(s, "%.6lld", rmicro);
        break;

          default:
        *s++ = '%'; // echo any bad '%?'
        *s++ = *fmt;    // specifier
        }
    if (size_t(s-buf) >= sizeof(buf)-7) // don't overshoot the buffer
        break;
    }
    *s = 0;

    return buf;
}


const 
char*  
TimeObj::formatDate( const char* buf, const char *fmt = "%A, %D %r" ) const 
{
   // strftime( buf, sizeof(buf), fmt, localtime(&(time_t)t.tv_sec) );
   // return buf;
   return 0x00;
}

time_t
TimeObj::convertHexDateToTimeT( const char *hexDate ) 
{
  char  *nameStart, *zone;
  
  if( ( zone = getenv( "TZ" ) ) && !strcmp( zone, "UTC" ) ) 
  {
    nameStart = strrchr( const_cast<char*>(hexDate), '/' ); // Tolerate filenames
    if( nameStart )
      nameStart = nameStart + 1;
    else
    nameStart = const_cast<char*>(hexDate);

    if( strlen( nameStart ) >= 8 ) {  // Tolerate longer strings (nameStart[>7] don't care)
      unsigned int daTime;
      char namel[9];
      strncpy( namel, nameStart, 8 );  /* There's always room */
      namel[8] = 0x0;  /* There's always room */
      if( sscanf( namel, "%X", &daTime ) == 1 ) return (time_t)daTime;
    }
  }
  
  return -1; /* problem */
}


bool
TimeObj::convertTimeTToHexDate( time_t daT, char *hexDate )
{
  char  flunky[9], *zone;
  
  if( ( zone = getenv( "TZ" ) ) && !strcmp( zone, "UTC" ) ) 
  {
    if( sprintf( flunky, "%08X", ((unsigned int)daT) ) == 8 ) {
      strcpy( hexDate, flunky );
      return true;
    } 
  }
  
  return false; /* problem */
}


bool
TimeObj::convertHexDateToAscii( const char *hexDate, char *asciiDate ) 
{
  char *zone;
  char hexter[9];
  strncpy( hexter, hexDate, 8 );
  hexter[8] = 0x00;
  if( ( zone = getenv( "TZ" ) ) && !strcmp( zone, "UTC" ) ) 
  {
    time_t daTime;
    if( ( daTime = TimeObj::convertHexDateToTimeT( hexter ) ) != -1 ) {
      struct tm daTimeStruct;
      if( gmtime_r( &daTime, &daTimeStruct ) ) {
        char daAsciiTime[20];
        if( strftime( daAsciiTime, 20, "%Y%m%d_%H%M%S", &daTimeStruct ) ) {
          strcpy( asciiDate, daAsciiTime );
          strcat( asciiDate, hexDate+8 );
          return true;
        }
      }
    }
  }
  return false; /* problem */
}

bool 
TimeObj::convertAsciiToHexDate( const char *asciiDate, char *hexDate ) 
{
  char *marker, *zone;
  if( ( zone = getenv( "TZ" ) ) && !strcmp( zone, "UTC" ) ) 
  {
    marker = strrchr( const_cast<char*>(asciiDate), '/' ); // Tolerate filenames
    if( marker )
      marker = marker + 1;
    else
      marker = const_cast<char*>(asciiDate);
    
    if( strlen( marker ) >= 15 ) {  // Tolerate longer strings (nameStart[>14] don't care)
      struct tm timeStruct;
      char timeStr[16];
      strncpy( timeStr, marker, 15 ); // Keep extra chars away from strptime
      timeStr[15] = '\0';
      if( strptime( timeStr, "%Y%m%d_%H%M%S", &timeStruct ) == NULL ) return false;
      if( !convertTimeTToHexDate( ::mktime( &timeStruct ), timeStr ) ) return false; // Re-use timeStr.
      if( strlen( timeStr ) != 8 ) return false;
      strcpy( hexDate, timeStr );
      return true;
    }
  }
  return false;
}


bool 
TimeObj::backDater( const char* moniker, int offset, char *bDated ) 
{
  char *zone;
  if( ( zone = getenv( "TZ" ) ) && !strcmp( zone, "UTC" ) ) 
  {

    int daMonth, daYear, daDay;
    struct tm dater;
    time_t daT;
    char fodder[5];

    strncpy( fodder, moniker, 4 );  fodder[4] = '\0';
    sscanf( fodder, "%d", &daYear );
    strncpy( fodder, moniker+4, 2 );  fodder[2] = '\0';
    sscanf( fodder, "%d", &daMonth );
    strncpy( fodder, moniker+6, 2 );  fodder[2] = '\0';
    sscanf( fodder, "%d", &daDay );
    /*fprintf( stderr, "Year = %d, Month = %d, Day = %d\n", daYear, daMonth, daDay );*/

    dater.tm_sec = 0;         /* seconds */
    dater.tm_min = 0;         /* minutes */
    dater.tm_hour = 0;        /* hours */
    dater.tm_mday = daDay;        /* day of the month */
    dater.tm_mon =daMonth - 1;         /* month */
    dater.tm_year = daYear-1900;        /* year */
    
#if 0   
    dater.tm_wday;        /* day of the week */
    dater.tm_yday;        /* day in the year */
    dater.m_isdst;       /* daylight saving time */
#endif

    daT = ::mktime( &dater );
    if( daT < 0 ) { fprintf( stderr, "mktime Error!!!" ); return false; }
    
    daT = daT + offset * 86400;
    //fprintf( stderr, "b4 = %ld aft = %ld, offset = %d\n", daT, daT + offset * 86400, offset );    
    if( !gmtime_r( &daT,  &dater  ) ) { fprintf( stderr, "gmtime_r Error!!!" ); return false; }
    
    sprintf( bDated, "%04d%02d%02d", dater.tm_year+1900, dater.tm_mon+1, dater.tm_mday );
    return true;
  }
  
 /* Bye bye ... */
  return false;
}

int
TimeObj::backDater( int moniker, int offset ) {
  char monStr[9], setStr[9];
  sprintf( monStr, "%08d", moniker );
  backDater( monStr, offset, setStr );
  return atoi( setStr );
}


bool 
TimeObj::testClass() {
   
    TimeObj a, b, c;
    char timeStr[1025];
    timeStr[1024] = 0x00;
    struct tm fodder, fodder2;
    
    TimeObj zeroid(0,0);
    
    fprintf( stderr, "aTime = %s\n", a.format( timeStr ) );

    if( a != zeroid ) return DRATS;
    
    b.setToTimeOfDay();
    
    c = b;
    if( b != c ) return DRATS;
    if( a == c ) return DRATS;
    
    fprintf( stderr, "aTime = %s\n", a.format( timeStr ) );
    fprintf( stderr, "bTime = %s\n", b.format( timeStr ) );
    fprintf( stderr, "cTime = %s\n", c.format( timeStr ) );

    fprintf( stderr, "Time = %s\n", b.format( timeStr, "%S.%u" ) );
    fprintf( stderr, "Time = %s\n", b.format( timeStr, "%D %h:%m:%s" ) );

    fprintf( stderr, "Time = %s\n", b.formatDate( timeStr ) );

    if( !b.localtime( &fodder ) ) return DRATS;
    
   #if 0
    struct tm {
                  int     tm_sec;         /* seconds */
                  int     tm_min;         /* minutes */
                  int     tm_hour;        /* hours */
                  int     tm_mday;        /* day of the month */
                  int     tm_mon;         /* month */
                  int     tm_year;        /* year */
                  int     tm_wday;        /* day of the week */
                  int     tm_yday;        /* day in the year */
                  int     tm_isdst;       /* daylight saving time */
              };

   #endif
    
    fprintf( stderr, "b time = %04d/%02d/%02d %02d:%02d:%02d\n", fodder.tm_year+1900, fodder.tm_mon+1, fodder.tm_mday, fodder.tm_hour, fodder.tm_min, fodder.tm_sec );
    c.set( 0, 0 );
    if( !c.localtime( &fodder2 ) ) return DRATS;
    fprintf( stderr, "c time = %04d/%02d/%02d %02d:%02d:%02d\n", fodder2.tm_year+1900, fodder2.tm_mon+1, fodder2.tm_mday, fodder2.tm_hour, fodder2.tm_min, fodder2.tm_sec );
    if( a != c ) return DRATS;
    if( !c.mktime( &fodder ) ) return DRATS;
    if( !c.localtime( &fodder2 ) ) return DRATS;
    fprintf( stderr, "c time = %04d/%02d/%02d %02d:%02d:%02d\n", fodder2.tm_year+1900, fodder2.tm_mon+1, fodder2.tm_mday, fodder2.tm_hour, fodder2.tm_min, fodder2.tm_sec );

    fprintf( stderr, "bTime = %s\n", b.format( timeStr ) );
    fprintf( stderr, "cTime = %s\n", c.format( timeStr ) );

    b.makeInt();
    c.makeInt();
    if( b != c ) return DRATS;
    
    {
      std::string tr("2011-01-23 04:05:06"), tl;
      b.setSQLDatetime( tr );
      b.getSQLDatetime( tl );
      fprintf( stderr, "1 before = %s, after = %s\n", tr.c_str(), tl.c_str() ); 
      if( tl != tr ) return DRATS;
      
      tr = "2011-01-23 04:05:06.123456"; tl = "";
      b.setSQLDatetime( tr );
      b.getSQLDatetimeFull( tl );
      fprintf( stderr, "2 before = %s, after = %s\n", tr.c_str(), tl.c_str() ); 
      if( tl != tr ) return DRATS;
      
    }
    
    b.setToTimeOfDay();
    c = b + TimeObj(1,0);
    fprintf( stderr, "milliDiff = %d\n", TimeObj::millisecondsDiff( b, c ) );
    
    {
      const char fName[] = "4E04FF6E.824.24b.bz2";
      const char fPath[] = "/mnt/CalMagNet/archive/CMN824_Peachtree2/0824/4E04FF6E.824.24b.bz2";
      const char hexStr[] = "4E04FF6E";
      const char canonicalMoniker[] = "20110624_211942";
      time_t a, b, c;
      
      char work[256], turk[256];
      
      a = convertHexDateToTimeT( hexStr );
      b = convertHexDateToTimeT( fName );
      c = convertHexDateToTimeT( fPath );
      fprintf( stderr, "a = %ld, b = %ld, c = %ld \n", a, b, c );
      
      if( a == -1 || b == -1 || c == -1 ) return DRATS;
      if( a != b || a != c ) return DRATS;
      
      if( !convertTimeTToHexDate( a, work ) ) return DRATS;
      if( strcmp( hexStr, work ) ) return DRATS;
      fprintf( stderr, "a = %ld work = %s\n", a, work );
      
      if( !convertTimeTToHexDate( b, work ) ) return DRATS;
      if( strcmp( hexStr, work ) ) return DRATS;
      fprintf( stderr, "b = %ld work = %s\n", a, work );
      
      if( !convertTimeTToHexDate( c, work ) ) return DRATS;
      if( strcmp( hexStr, work ) ) return DRATS;
      fprintf( stderr, "c = %ld work = %s\n", a, work );
      
      if( !convertHexDateToAscii( hexStr, work ) ) return DRATS;
      if( strcmp( work, canonicalMoniker ) ) return DRATS;
      if( !convertAsciiToHexDate( work, turk ) ) return DRATS;
      if( strcmp( turk, hexStr ) ) return DRATS;
      
      if( !convertHexDateToAscii( fName, work ) ) return DRATS;
      if( strcmp( work, canonicalMoniker ) ) return DRATS;
      if( !convertAsciiToHexDate( work, turk ) ) return DRATS;
      if( strcmp( turk, hexStr ) ) return DRATS;
       
      if( !convertHexDateToAscii( fPath, work ) ) return DRATS;
      if( strcmp( work, canonicalMoniker ) ) return DRATS;
      if( !convertAsciiToHexDate( work, turk ) ) return DRATS;
      if( strcmp( turk, hexStr ) ) return DRATS;
      
    }
    
    return VOILA;
}


