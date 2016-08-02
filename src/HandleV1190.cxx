//
// ROOT analyzer
//
// CSI detector handling
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
//Bank:YDT_ Length: 40(I*1)/10(I*4)/10(Type) Type:Unsigned Integer*4
//   1-> 0x4000031f 0x08018a7a 0x18018002 0x09018a7a 0x19018002 0x0a018a7a 0x1a018002 0x0b018a7a 
//       Gbl Head   TDC Head8  TDC Trail8 TDC Head9  TDC Trail9 TDC HeadA  TDC TrailA TDC HeadB
//   9-> 0x1b018002 0x8000015f 
//       TDC TrailB Gbl Trail   

//Bank:YDT_ Length: 72(I*1)/18(I*4)/18(Type) Type:Unsigned Integer*4
//   1-> 0x400037df 0x081be3fe 0x0010504c 0x00704d32 0x00b04142 0x00305071 0x00504fa8 0x00b08b53
//       Gbl Head   TDC Head8  TDC meas   TDC meas   TDC meas   TDC meas   TDC meas   TDC meas 
//   9-> 0x0030a3cc 0x0050a1b8 0x181be00a 0x091be3fe 0x191be002 0x0a1be3fe 0x1a1be002 0x0b1be3fe
//       TDC meas   TDC meas   TDC Trail8 TDC Head9  TDC Trail9 TDC HeadA  TDC TrailA TDC HeadB
//  17-> 0x1b1be002 0x8000025f
//       TDC TrailB Gbl Trail   
//

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
#include "HandleV1190.h"
#include "Globals.h"

int gV1190nitems;
static uint32_t tRef=0, previousSN=0, tRF =0;

const double timeSlope = 0.2; // ns/channel 
const uint32_t Nchannels = 64+128+128+64+64+64;
//const uint32_t choffset[] = {0,64, 320, 384, 448}; //before April 24, 2015
//const int choffset[] = {192,384,320,64,128,0}; 
const int choffset[] = {0,192,384,320,64,128}; 
const int binlimit = 4196;
TH1D * hV1190_T[Nchannels] = {NULL}; // Raw T
TH1D * hTime[Nchannels] = {NULL}; // Time  
TH1D * hTimeRF[Nchannels] = {NULL}; //  Time wrt RF
TH1D * hTall = {NULL}; // T
TH2D * hTall2D = {NULL}; // T

void HandleV1190(TMidasEvent& event, void* ptr, tdc_t* timeArray, int nitems, int MYLABEL)
{
  uint32_t *data;
  int    i, debug = 0, debug1 = 0; 
  //  int eventId = event.GetEventId();
  data = (uint32_t *) ptr;
  //if ( MYLABEL == 0 ) {
  //if (hV1190_T[0] == NULL)
      //  HandleBOR_V1190(gRunNumber, 0);
    // Reset Waveform histos before scanning ALL banks
    //    hV1190_T[channel]->Reset();
    // published var for HandlePHYSICS
    gV1190nitems = nitems;
    uint32_t geo, evtcnt=0, tdc=0, bunchid, evtid, channel;
    uint32_t modchannel, measure, wordcnt, evtidt, errflag, trailer;
    // Loop over all the banks
    if (nitems != 0) {
      if (debug) printf("V1190_ Evt#:%d nitems:%d\n", event.GetSerialNumber(), nitems); 
      for (i=0 ; i<nitems ; i++) {
	if (debug) printf("items:%d - data[%d]: %d / 0x%x\n", Nchannels, i, data[i], data[i]);
	switch (data[i] & 0xF8000000) {
	case 0x40000000: // Global Header
	  geo = data[i] & 0x1F;
	  evtcnt = 0x3FFFFF & (data[i] >> 5);
	  break;
	case 0x08000000:  // TDC Header
	  tdc  = 0x3 & (data[i] >> 24);
	  bunchid = data[i] & 0xFFF;
	  evtid   = 0xFFF & (data[i] >> 12);
	  break;  
	case 0x00000000:  // TDC measurement
	  channel = 0x3F & (data[i] >> 19); 
	  channel = (channel/16)*16+15-channel%16; //Flipping channels 0->15; 1->14;...;15->0; 31->16; 47->32; 63;48.
	  modchannel = (tdc/2 * 64) + channel;  // changed AS May 15,2013
	  //modchannel = (tdc * 32) + channel; //original
	  measure = data[i] & 0x7FFFF;
	  if (modchannel<Nchannels) {
	    hV1190_T[modchannel+choffset[MYLABEL]]->Fill((float)measure, 1.);
	    //	    hTime[modchannel+choffset[MYLABEL]]->Fill((float)measure*timeSlope, 1.);
	  }
	  (data[i] & 0x04000000) ? trailer = 1 : trailer = 0;
	  // Deal with delta time in reference to the STOP (tRef)
	  if (modchannel+choffset[MYLABEL] == 14 && event.GetSerialNumber() != previousSN) {
	    previousSN = event.GetSerialNumber();
	    // Extract tRef (ICT_@0 (1))
	    tRef = measure;
	  	  }  
	  if (modchannel+choffset[MYLABEL] == 12) {
	   	    tRF = measure; //RF signal
	  } 
	   {
	    // Build hTime
 	    hTime[modchannel+choffset[MYLABEL]]->Fill( ((float)measure - (float)tRef)*timeSlope, 1.);
	   	    hTimeRF[modchannel+choffset[MYLABEL]]->Fill( ((float)measure - (float)tRF)*timeSlope, 1.);
	   timeArray->timeRF[modchannel+choffset[MYLABEL]] = ( ((float)measure - (float)tRF)*timeSlope);
	    // Build the overall timing histo
	    hTall->Fill( ((float)measure - (float)tRef)*timeSlope, 1.);
	    hTall2D->Fill(modchannel+choffset[MYLABEL],((float)measure - (float)tRef)*timeSlope);
	  }
	  if (debug1) {
	    printf("evt#:%d trailer:%d bk:%d tdc:%d channel:%d modch:%d H#:%d measure:%d/0x%x tRef:%d/0x%x dT:%d\n"
		   ,evtcnt, trailer, MYLABEL, tdc, channel, modchannel, modchannel+choffset[MYLABEL], measure, measure, tRef, tRef, measure-tRef);
	  }
	  break;
	case 0x18000000: // TDC Trailer
	  wordcnt = data[i] & 0xFFF;
	  evtidt  = 0xFFF & (data[i] >> 12);
	  break;
	case 0x20000000: // TDC Error
	  errflag = data[i] & 0x7FFFF;
	  break;
	} // switch
      } // for loop
    } // nitems != 0
    //  } // label
}
//---------------------------------------------------------------------------------
void HandleBOR_V1190(int run, int time, tdc_t *timeArray)
{
  char label[32], sig[32];

  for (int i=0;i<512;i++){
    timeArray->timeRF[i]=0.;
}
  // Booking 
  printf(" in V1190 BOR... Trying to book\n");
  gOutputFile->cd();

  hV1190_T[0] = (TH1D*)gDirectory->Get("Time00");
  if (hV1190_T[0] == 0) {
    if (gOutputFile) {

    // Make a Time directory and cd to it.
     TDirectory* TimeRaw_dir = gOutputFile->mkdir("TimeRaw");
     TimeRaw_dir->cd();

      printf(" in V1190 BOR... Booking histos\n");
      for (unsigned int channel=0;channel<Nchannels;channel++) {
	sprintf(label, "TimeRaw%02d", channel);
	sprintf(sig, "TimeRaw%03d", channel);
	hV1190_T[channel] = new TH1D(label, sig, binlimit, 0, 8*binlimit);
	//printf("Booking TH1D %s \n", label);
      }

      gOutputFile->cd();

      TDirectory* Time_dir = gOutputFile->mkdir("Time");
      Time_dir->cd();

      for (unsigned int channel=0;channel<Nchannels;channel++) {
        sprintf(label, "Time%02d", channel);
        sprintf(sig, "Time%03d", channel);
        hTime[channel] = new TH1D(label, sig, binlimit, -1000, 1000);
        //printf("Booking TH1D %s \n", label);
      }

      for (unsigned int channel=0;channel<Nchannels;channel++) {
        sprintf(label, "TimeRF%02d", channel);
        sprintf(sig, "TimeRF%03d", channel);
        hTimeRF[channel] = new TH1D(label, sig, binlimit, -1000, 1000);
        //printf("Booking TH1D %s \n", label);
      }
      
      hTall = new TH1D("dTall", "dTall", binlimit, -1000, 1000);
      hTall2D = new TH2D("dTall2D", "dTall2D", Nchannels,0,Nchannels,binlimit, -1000, 1000);
    
  printf(" in V1190 BOR... Booking histos Done ....\n");
    }
  }  
}
//---------------------------------------------------------------------------------
void HandleEOR_V1190(int run, int time)
{
  printf(" in V1190 EOR\n");
}
