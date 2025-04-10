/*
 Copyright ShotSpotter, 2016
*/

#ifndef __SST_COMMON__
#define __SST_COMMON__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>  /* Contains MAX_CANON */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>

//using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#define SST_DEBUG

#define VOILA 0
#define DRATS 1

#define HRS_PER_DAY  24
#define MINS_PER_HR  60
#define SECS_PER_MIN 60
#define SECS_PER_HR  3600
#define SECS_PER_DAY 86400


#define PI 3.1415926535897932384
#define DTR (PI/ 180.0)

#define F_EPSILON 1.0e-10

#define INVALID_ID -1
#define INVALID_VALUE DBL_MIN
#define INVALID_VALUE_F FLT_MIN

#define MAX_NAME_LENGTH 32
#define MAX_DATA_COLS   UINT_MAX
#define MAX_DATA_ROWS   ULLONG_MAX

#define MAX_PATH 1024

enum FileFormatOptions 
{  /* Pass negative value for compression & decompression */
    FORMAT_NATIVE,
    FORMAT_BINARY,
    FORMAT_ASCII,
    FORMAT_WAV,
    FORMAT_LABELS,
    numFileFormatOptions
};

enum NumberFormats 
{
    NUM_INT,
    NUM_FLT,
    NUM_DBL,
    NUM_TIME,
    numNumberFormats
};


#define SST_STATUS_OUTPUT stderr

// XXX Clark NOTE:  awk requires an escape backslash, i.e: 'awk -F\|' to specify the '|' as a separator

// XXX Clark Date time format does not include the three letter time zone put out by date command

// XXX Clark Don't pass carriage returns! 


#define SST_ERR_INIT \
    tzset(); \
    prgName = argv[0]; /* argv[0] persists until exit of main(). */ \
    errNote[1024] = 0x00; \
    inFile[1024] = 0x00; \
    outFile[1024] = 0x00; 

#define SSTERR_INIT( _PRG_, _IN_, _OUT_ ) \
    SST_ERR_INIT \
    prgName=_PRG_;\
    strncpy( inFile,  _IN_, 1024 );\
    strncpy( outFile, _OUT_, 1024 ); /* Has "global" scope */

#define MAKE_TIME \
    char *timerStrung, trSt[65]; \
    trSt[64] = 0x00; \
    time( &daTime ); \
    timerStrung = ctime( &daTime ); \
    strncpy( trSt, timerStrung, 20 ); trSt[20] = 0x00; \
    strcat( trSt, "PDT " ); trSt[28] = 0x00; \
    strncpy( trSt+24, timerStrung+20, 4 ); trSt[28] = 0x00; 

#define SSTERR( __A__ ) { \
    MAKE_TIME \
    fprintf( SST_STATUS_OUTPUT, "   %s|%s|ERROR|%s|%s|%s|%s|%d\n", trSt, prgName, __A__, inFile, outFile, __FILE__, __LINE__ ); }

#define SSTINFO( __A__ ) { \
    MAKE_TIME \
    fprintf( SST_STATUS_OUTPUT, "   %s|%s|INFO|%s|%s|%s|%s|%d\n", trSt, prgName, __A__, inFile, outFile, __FILE__, __LINE__ ); }
  
#define SSTWARN( __A__ ) { \
    MAKE_TIME \
    fprintf( SST_STATUS_OUTPUT, "   %s|%s|WARN|%s|%s|%s|%s|%d\n", trSt, prgName, __A__, inFile, outFile, __FILE__, __LINE__ ); }
  

extern char *prgName;
extern char errNote[1025];
extern char inFile[1025];
extern char outFile[1025];
extern time_t daTime;

typedef std::vector<std::string> StringVec;

#endif /* __SST_COMMON__ */
