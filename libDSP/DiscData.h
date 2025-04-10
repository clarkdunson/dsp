#ifndef __DISCRETEDATA_H__
#define __DISCRETEDATA_H__

#include "DataCommon.h"

/* Scheme minimizes number of mallocs */
#define DISCRETE_DATA_LABEL_LENGTH 64
#define DISCRETE_DATA_MAX_COLUMNS 32 /* Hardcoded to sscanf */
#define DISCRETE_DATA_MAX_LINE_LENGTH 4096

#define DISCRETE_DATA_ROW_PAGE 1024

class DiscData : public DataCommon
{

public:

    DiscData() : DataCommon( DISCRETE_DATA ) { makeInst(); }
   ~DiscData() { remakeInst(); }

    void makeInst();
    void remakeInst();
    
protected:
    // Parameters
    char *labels;
    int  *colTypes;
    
public:
    char* getLabel( unsigned int which ) const;
    
    bool append( const DiscData &me );

    bool read( char *fileName, int format, char *formatStr = NULL );
    bool write( char *fileName, int format, char *formatStr = NULL ) { return DRATS; }

    bool readLabelsFile( FILE *inFid, char *fileName, char *formatStr = NULL );
    
    bool readBinaryFile( char *fileName );
    bool writeBinaryFile( char *fileName ) const;
    
    bool diff( const DiscData &she ) const;
    
    static bool testClass();
};

inline
void 
DiscData::makeInst() 
{   
    labels = NULL;
    colTypes = NULL;
}

inline
void 
DiscData::remakeInst() 
{
    DataCommon::remakeInst();
    if( labels ) { free( labels ); labels = NULL; }
    if( colTypes ) { free( colTypes ); colTypes = NULL; }
}


inline
char* 
DiscData::getLabel( unsigned int which ) const {
    if( which > cols ) SSTWARN("Label Chosen out of range!!");
    return labels + which * DISCRETE_DATA_LABEL_LENGTH;
}

// XXX Clark needs to include all of Data Common ... hmmmmm.
inline
bool 
DiscData::diff( const DiscData &she ) const {
    
    if( cols != she.cols ) return DRATS;
    if( rows != she.rows ) return DRATS;
    
   /* The space between label strings doesn't count !!! */
    for( unsigned int lth = 0; lth < cols; lth++ ) {
       if( strcmp( getLabel( lth ), she.getLabel( lth ) ) ) {
            return DRATS;
        }
    }
    
    if( memcmp( data, she.data, rows*cols*sizeof(double) ) ) return DRATS;
    
    return VOILA;
}
#endif // __DISCRETEDATA_H__
