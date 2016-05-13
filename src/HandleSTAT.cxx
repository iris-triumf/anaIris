//
// ROOT analyzer
//
// DAQ Stat handling
//
// $Id$
//

/// \mainpage
///
/// \section intro_sec Introduction
///
///
/// \section features_sec Features
///  
///   state = gOdb->odbReadInt("/runinfo/State");

#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <assert.h>
#include <signal.h>

//#include "midasServer.h"
#include "TMidasEvent.h"
#include <TApplication.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TH2.h>
#include <TNtuple.h>
#include <TFolder.h>
#include <TFile.h>
#include <TMath.h>

#include "Globals.h"

// published var for HandlePHYSICS
int gSTATnitems;

TNtuple* ntp;
const int Nchannels = 8;
TH1D * hStat[Nchannels] = {NULL};

void HandleBOR_STAT(int run, int time);
void HandleEOR_STAT(int run, int time);

void HandleSTAT(TMidasEvent& event, void* ptr, int nitems, int MYLABEL)
{
  uint32_t *data;
  int    i, debug = 1, debug1 = 0; 
  int eventId = event.GetEventId();

  data = (uint32_t *) ptr;
  if ( MYLABEL == 0 ) {
    if (hStat[0] == NULL)
      HandleBOR_STAT(gRunNumber, 0);
    
    // publishig var for HandlePHYSICS
    gSTATnitems = nitems;

    // Loop over all the banks

    if (nitems != 0) {
      if (debug) printf(" STAT Evt#:%d nitems:%d\n", event.GetSerialNumber(), nitems); 
      if (nitems > Nchannels) return;
      for (i=0 ; i<nitems ; i++) {
	if (debug1) printf("items:%d - data[%d]: %d / 0x%x\n", Nchannels, i, data[i], data[i]);
	if (i<5) {
	  hStat[i]->Fill((float)data[i] , 1.); 
	} else {
	  hStat[i]->Fill((float)data[i]*50/1000 , 1.); 
	}
      } // nitems != 0
    } 
  }
}

typedef struct {
  char label[16];
  uint32_t nbin;
  uint32_t lowbin;
  uint32_t highbin;
} Hdef;

Hdef hdef[Nchannels] = { {"version", 1000, 0, 0xffffffff}
		  ,{"TrigNo", 1000, 0,  0xffffffff}, {"TrigTS", 1000, 0,  0xffffffff}
		  ,{"Rd StartT", 1000, 0,  0xffffffff}, {"Rd EndT", 1000, 0,  0xffffffff}
		  ,{"Trig Latency", 300, 0,299}
		  ,{"Rd eTime", 3000, 0, 2999}, {"Busy eTime", 3000, 0, 2999}
};

//---------------------------------------------------------------------------------
void HandleBOR_STAT(int run, int time)
{
  char label[32], sig[32];

  // Booking 
  printf(" in Stat BOR... Trying to book\n");
  gOutputFile->cd();

  // Make a ADCStat directory and cd to it.
  TDirectory* astat_dir = gOutputFile->mkdir("Astat");
  astat_dir->cd();
  
  hStat[0] = (TH1D*)gDirectory->Get("version");
  if (hStat[0] == 0) {
    if (gOutputFile) {
      printf(" in Stat BOR... Booking histos\n");
      for (int channel=0;channel<Nchannels;channel++) {
	sprintf(label, "%s", hdef[channel].label);
	sprintf(sig, "%s", hdef[channel].label);
	hStat[channel] = new TH1D(label, sig, hdef[channel].nbin, hdef[channel].lowbin, hdef[channel].highbin);
	printf("Booking TH1D %s \n", hdef[channel].label);
      }
      printf(" in Stat BOR... Booking histos Done ....\n");
    }
  }
}

void HandleEOR_STAT(int run, int time)
{
  printf(" in Stat EOR\n");
}
