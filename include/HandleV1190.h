// HandleV1190.h

#ifndef HandleV1190_H
#define HandleV1190_H

#include "SetupHistos.h"

//Extern
extern int gV1190nitems;

class tdc_t
{
 public: 
  Double_t timeRF[512];
 int timeRaw[512];
};

void  HandleV1190(TMidasEvent& event, void* ptr, int wsize, int MYLABEL, tdc_t* timeArray, hist_t* hist);
void  HandleBOR_V1190(int run, int time, tdc_t *timeArray);
void  HandleEOR_V1190(int run, int time);


#endif
// end
