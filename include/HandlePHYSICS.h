// HandlePHYSICS.h

#ifndef HandlePHYSICS_H
#define HandlePHYSICS_H

#include "SetupHistos.h"

void HandlePHYSICS(det_t *det, tdc_t *timeArray, hist_t *hist);
void  HandleBOR_PHYSICS(int run, int time);
void  HandleEOR_PHYSICS(int run, int time);

#endif
// end

