/*
 Copyright ShotSpotter, 2016
*/

#ifndef __FREQUENCY_DATA_H__
#define __FREQUENCY_DATA_H__

#include "DataCommon.h"

class FreqData : public DataCommon
{

public:

    FreqData() : DataCommon( FREQUENCY_DATA ) { makeInst(); }
   ~FreqData() { remakeInst(); }

    void makeInst();
    void remakeInst();

    void initAttributes();

protected:
   // Parameters
    double  freqResolution;
};

inline
void 
FreqData::makeInst() {
    freqResolution = INVALID_VALUE;
}

inline
void 
FreqData::remakeInst() {
}


#endif // __FREQUENCY_DATA_H__
