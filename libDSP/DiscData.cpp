/*
 Copyright ShotSpotter, 2016
*/

#include "DiscData.h"

bool
DiscData::read( char *newFileName, int newFormat, char *formatStr ) 
{
    int status = VOILA;

   /* Reset object */
    remakeInst();


   /* Ready read */
    bool compressed = false;
    int format = newFormat;
    FILE *inFid = DataCommon::readStart( newFileName, &format, &compressed );
    if( !inFid ) { SSTWARN( "File Open Failed!!!" ) return DRATS; }


   /* Read file */    
    switch( format ) {
        case FORMAT_LABELS :
            if( readLabelsFile( inFid, newFileName, formatStr ) ) {
                sprintf( errNote, "Problem, reading labels file: %s\n", newFileName );
                SSTWARN(errNote);
                status = DRATS;
            }
            break;
        default :
            SSTWARN("Chosen read format not supported!! Yet... :)");
            return DRATS;
    }


   /* Close file */    
    if( !DataCommon::readFinish( inFid, compressed ) ) SSTERR("File Close Failed!!!")

    return status;
}


bool
DiscData::readLabelsFile( FILE *inFid, char *fileName, char *formatStr ) 
{
    char line[DISCRETE_DATA_MAX_LINE_LENGTH];
    char fmtStr[512];
    
    char* dividers[DISCRETE_DATA_MAX_COLUMNS];
    int charCounter = 9;
    unsigned int labelCounter = 1;

    
   /* Keep us honest */
    if( labels || rows || cols || colTypes != 0 ) {
        SSTERR( "USAGE!!! Must remake object first or leak memory!!!!" );
    }
    
    
   /* Read in and parse label line */
    if( fgets( line, DISCRETE_DATA_MAX_LINE_LENGTH, inFid ) != line ) {
        sprintf( errNote, "Problem reading label line of file |%s|!!!!", fileName );
        SSTWARN(errNote);
        return DRATS;
    }
    
    if( strncmp( line, "LABELS = ", charCounter ) ) {
        sprintf( errNote, "Problem parsing label line of file |%s|!!!!", fileName );
        SSTWARN(errNote);
        return DRATS;
    }

    dividers[0] = line + charCounter - 1;
    //fprintf( stderr, "%s\n", dividers[0] );  
    while( line[charCounter] != '\n' ) {
        if( line[charCounter] == '\t' ) {
            dividers[labelCounter] = line + charCounter;
            labelCounter++;
        }
        charCounter++;
    }
    line[charCounter] = '\t';
    

   /* Discover labels, take all */
    unsigned int ith, labelLength=0;
    char *labelsTmp = (char*)malloc( DISCRETE_DATA_LABEL_LENGTH * labelCounter );
    if( !labelsTmp ) SSTERR( "Malloc for labelTmp FAILED!!!!" );
    for( ith = 0; ith < labelCounter-1; ith++ ) {
        labelLength = dividers[ith+1] - dividers[ith] - 1;
        strncpy( labelsTmp+ith*DISCRETE_DATA_LABEL_LENGTH, dividers[ith]+1, labelLength );
        labelsTmp[ith*DISCRETE_DATA_LABEL_LENGTH+labelLength] = '\0';
        //fprintf( stderr, "label# = %d, labelLength = %d, label = |%s|\n", ith+1, labelLength, labelsTmp+ith*DISCRETE_DATA_LABEL_LENGTH );
    }
    strncpy( labelsTmp+ith*DISCRETE_DATA_LABEL_LENGTH, dividers[ith]+1, (line+charCounter)-dividers[labelCounter-1]-1 );
    labelsTmp[ith*DISCRETE_DATA_LABEL_LENGTH+labelLength] = '\0';
    //fprintf( stderr, "label# = %d, labelLength = %d, label = |%s|\n", ith+1, labelLength, labelsTmp+ith*DISCRETE_DATA_LABEL_LENGTH );


   /* Parse and store labels, set up for read */
    if( !formatStr ) {
    
        fmtStr[0] = '\0';
        for( ith = 0; ith < labelCounter; ith++ ) strcat( fmtStr, "%lf" );
        labels = labelsTmp;
        cols = labelCounter;

    } else {
    
       /* sscanf string supplied, select labels */
        int formatCharCounter = 0;
        unsigned int formatCounter = 0;
        int labelIndices[DISCRETE_DATA_MAX_COLUMNS];
        labelCounter = 0;
        while( formatStr[formatCharCounter] != '\0' ) {
            if( formatStr[formatCharCounter] == '%' ) {
                if( formatStr[formatCharCounter+1] != '*' ) {
                    labelIndices[labelCounter] = formatCounter;
                    labelCounter++;
                }
                formatCounter++;
            }
            formatCharCounter++;
        }
        
        if( formatCounter < labelCounter ) SSTERR("FUPDUCK format");
        //fprintf( stderr, "labelCounter = %d\n", labelCounter );
        
        labels = (char*)malloc( DISCRETE_DATA_LABEL_LENGTH * labelCounter );
        if( !labels ) SSTERR( "Malloc for labels FAILED!!!!" );
        cols = labelCounter;

        for( ith = 0; ith < labelCounter; ith++ ) {
            strcpy( labels+ith*DISCRETE_DATA_LABEL_LENGTH, labelsTmp+labelIndices[ith]*DISCRETE_DATA_LABEL_LENGTH );
            //fprintf( stderr, "label# %d = %s = %s\n", ith, labels+ith*DISCRETE_DATA_LABEL_LENGTH, getLabel( ith ) );
        }
        
        strcpy( fmtStr, formatStr );
        free( labelsTmp );
    } 
    
    
   /* Now it's time to read !!!! */
    {
#if 0
        double *a, *dataTmp;
       // double b[DISCRETE_DATA_MAX_COLUMNS];
        
        dataTmp = (double*)malloc( labelCounter*sizeof(double)*DISCRETE_DATA_ROW_PAGE );
        if( !dataTmp ) {
            SSTERR("Malloc for data FAILED!!!!" );
        }
        
        int linesRead = 0;
        while( fgets( line, DISCRETE_DATA_MAX_LINE_LENGTH, inFid ) == line ) {
            
           // int numItemsRead = 0;
            
            a = dataTmp + labelCounter * linesRead;
            
            linesRead++;

           /* Crude, but faster than stdarg! */
            if( numItemsRead = sscanf( line, fmtStr, a, a+1, a+2, a+3, a+3, a+4, a+5, a+6, a+7, a+8, a+9, a+10, a+11, a+12, a+13, a+14, a+15, a+16, a+17, a+18, a+19, a+20, a+21, a+22, a+23, a+24, a+25, a+26, a+27, a+28, a+29, a+30, a+31 ) != labelCounter ) {
                sprintf( errNote, "Choked on scan of line# %d.  Only %d out of %d items read!!\n", linesRead, numItemsRead, labelCounter );
                SSTWARN( errNote );
                return DRATS;
            }
             //fprintf( stderr, "time = %lf ma4 = %lf ma5 = %lf\n", a[0], a[1], a[2] ); 
        }
        
       /* Huge very important assignments */
        data = (char*)realloc( dataTmp, linesRead * cols * sizeof(double) );
        rows = linesRead;
        
        for( ith = 0; ith < rows; ith++ ) {
            a = ((double*)data + cols * ith);
            //fprintf( stderr, "%s = %lf %s = %lf %s = %lf\n", getLabel(0), a[0], getLabel(1), a[1], getLabel(2), a[2] );
        }
#endif
    }
    
 
    return VOILA;
}

bool 
DiscData::append( const DiscData &me ) 
{
    char *dataTmp;
    
    if( !data ) { /* Start from scratch */
    
        if( cols || rows || labels ) SSTERR("USAGE!!!");
        
        cols = me.cols;
    
        labels = (char*)malloc( cols*DISCRETE_DATA_LABEL_LENGTH );
        if( !labels ) SSTERR("DiscData::append() malloc() FAILED!!!");
        memcpy( labels, me.labels, cols*DISCRETE_DATA_LABEL_LENGTH);

        dataTmp = (char*)malloc( cols*(rows+me.rows)*sizeof(double) );
        if( !dataTmp ) SSTERR("DiscData::append() malloc() FAILED!!!");
        
    } else { /* Add to existing */

        if( !me.cols || !me.rows ) {
            SSTWARN( "DiscData::append() nothing to do." );
            return DRATS;
        }
        if( me.cols != cols ) {
            SSTWARN( "DiscData::append() column mismatch" );
            return DRATS;
        }

        for( unsigned int lth = 0; lth < cols; lth++ ) {
           if( strcmp( getLabel( lth ), me.getLabel( lth ) ) ) {
                sprintf( errNote, "Column Labels differ: %s vice %s", getLabel( lth ), me.getLabel( lth ) );
                SSTWARN( errNote );
            }
        }

        dataTmp = (char*)realloc( data, cols*(rows+me.rows)*sizeof(double) );
        if( !dataTmp ) SSTERR("DiscData::append() realloc() FAILED!!!");

    }

    data = dataTmp;
    
    memcpy( data+cols*(rows)*sizeof(double), me.data, cols*(me.rows)*sizeof(double));
    
    rows += me.rows;
    
    return VOILA;
}


// XXX Clark  Fast for now, needs abstraction with DataCommon yet

bool
DiscData::readBinaryFile( char *fileName ) 
{
    remakeInst();
    
   /* Open file */    
    FILE *inFid = fopen( fileName, "r" );
    if( !inFid ) {
        sprintf( errNote, "File \"%s\" could not be opened!!!\n", fileName );
        SSTWARN( errNote );
        return DRATS;
    }

    if( fread( &cols, sizeof(unsigned int), 1, inFid ) != 1 ) SSTERR("cols read failed!!!");
    if( fread( &rows, sizeof(unsigned long long), 1, inFid ) != 1 ) SSTERR("rows read failed!!!");
    
    labels = (char*)malloc( DISCRETE_DATA_LABEL_LENGTH * cols );
    if( !labels ) SSTERR( "Malloc for labels FAILED!!!!" );
    if( fread( labels, DISCRETE_DATA_LABEL_LENGTH, cols, inFid ) != cols ) SSTERR("labels read failed!!!");
    
    data = (char*)malloc( cols*rows*sizeof(double) );
    if( !data ) SSTERR("DiscData::append() malloc() FAILED!!!");
    if( fread( data, sizeof(double), cols*rows, inFid ) != cols*rows ) SSTERR("data read failed!!!");
    
    fclose( inFid );
    
    return VOILA;
}

bool
DiscData::writeBinaryFile( char *fileName ) const
{
   /* Open file */    
    FILE *outFid = fopen( fileName, "w" );
    if( !outFid ) {
        sprintf( errNote, "File \"%s\" could not be opened!!!\n", fileName );
        SSTWARN( errNote );
        return DRATS;
    }

    if( fwrite( &cols, sizeof(unsigned int), 1, outFid ) != 1 ) SSTERR("cols write failed!!!");
    if( fwrite( &rows, sizeof(unsigned long long), 1, outFid ) != 1 ) SSTERR("rows write failed!!!");
    
    if( fwrite( labels, DISCRETE_DATA_LABEL_LENGTH, cols, outFid ) != cols ) SSTERR("labels write failed!!!");
    
    if( fwrite( data, sizeof(double), size_t(cols*rows), outFid ) != cols*rows ) SSTERR("data write failed!!!");
    
    fclose( outFid );
    
    return VOILA;
}



bool
DiscData::testClass()
{
   // Declaration Tests
    DiscData          dd1, dd2, c, d;
    
    //dd1.read( "20060716.CMN.605.01.fb", FORMAT_LABELS );
    //dd2.read( "20060717.CMN.605.01.fb", FORMAT_LABELS );
 #if 0   
    
    dd1.read( "20060716.CMN.605.01.fb", FORMAT_LABELS, "%lf%*lf%*lf%*lf%lf%lf" );
    dd2.read( "20060717.CMN.605.01.fb", FORMAT_LABELS, "%lf%*lf%*lf%*lf%lf%lf" );

    fprintf( stderr, "dd1 Before append: cols = %u, rows = %llu\n", dd1.getCols(), dd1.getRows() );
    fprintf( stderr, "dd2 Before append: cols = %u, rows = %llu\n", dd2.getCols(), dd2.getRows() );

    dd1.append( dd2 );

    fprintf( stderr, "dd1 After append: cols = %u, rows = %llu\n", dd1.getCols(), dd1.getRows() );

    fprintf( stderr, "c Before append: cols = %u, rows = %llu\n", c.getCols(), c.getRows() );
    c.append( dd1 );
    fprintf( stderr, "c After append: cols = %u, rows = %llu\n", c.getCols(), c.getRows() );
    
    c.writeBinaryFile( "test.out" );
    d.readBinaryFile( "test.out" );

    if( c.diff( d ) ) { fprintf( stderr, "READ WRITE Broke\n" ); return DRATS; }
#endif
    return VOILA;
}



