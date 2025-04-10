/*
 Copyright ShotSpotter, 2016
*/

#ifndef __FREQUENCYTIMEDATA_H__
#define __FREQUENCYTIMEDATA_H__

#include "DataCommon.h"

class SpecData : public DataCommon
{

public:

    SpecData() : DataCommon( FREQUENCY_TIME_DATA ) { makeInst(); }
   ~SpecData() { remakeInst(); }

    void makeInst();
    void remakeInst();

    void initAttributes();

protected:
   // Parameters
    double     freqResolution;
    double     sampleRate;
};

inline
void 
SpecData::makeInst() {
    freqResolution = INVALID_VALUE;
    sampleRate = INVALID_VALUE;
}

inline
void 
SpecData::remakeInst() {
}

#endif // __FREQUENCYTIMEDATA_H__
