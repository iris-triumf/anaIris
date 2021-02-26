// HandleV1740.h

#ifndef HandleV1740_H
#define HandleV1740_H

#include "SetupHistos.h"

//Extern
extern int gV1740nitems;


void  HandleV1740(TMidasEvent& event, void* ptr, int wsize,  hist_t* hist);
void  HandleBOR_V1740(int run, int time);
void  HandleEOR_V1740(int run, int time);


#endif
// end
