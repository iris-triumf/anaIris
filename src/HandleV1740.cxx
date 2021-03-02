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
#include <TH1F.h>
#include <TH2.h>
#include <TNtuple.h>
#include <TFolder.h>
#include <TFile.h>
#include <TMath.h>
#include "HandleV1740.h"
#include "Globals.h"
#include "SetupHistos.h"

//Pulse processing tools from the iris-daqtools library
#include <cmath>
#include "TrapezoidShaper.h"
#include "LeadingEdgeDiscriminator.h"
#include "AmplitudeAnalyser.h"
#include "ConstantFractionAnalyser.h"

int gV1740nitems;

const double timeSlope = 0.2; // ns/channel 
const uint32_t Nchannels = 64+128+128+64+64+64;
const int choffset[] = {0,192,384,320,64,128}; 
const int binlimit = 4196;

// histogram for first V1740 channel
TH1D* V1730Channel0 = 0;

// Shaped signal from first V1740 channel
TH1D *V1730Shaped0 = 0;
// Energy and time spectra derived from the shaped pulse.
TH1D *V1730Energy0 = 0;
TH1D *V1730Time0 = 0;

void HandleV1740(TMidasEvent& event, void* ptr, int nitems, hist_t* hist)
{
  uint32_t *data;
  data = (uint32_t *) ptr;
  gV1740nitems = nitems;
  
  if (nitems == 0) { printf("V1740 decode; warning empty bank\n"); return;}

  // This decoding is assuming that group mask == 1 and we are just reading out 8 channels.

  // Structure to store waveforms for 8 channels.
  std::vector<std::vector<int> > samples;
  for(int i = 0; i < 8; i++){
  
    samples.push_back(std::vector<int>());
  }
  
  // Check that size of data bank looks correct;
  int nblocks = (nitems - 4) / 9;
  int remainder = (nitems - 4) % 9;
  if(remainder != 0) printf("V1740 decoder; bank size is unexpected; size of %i does not satisfy (size-4) % 9 == 0\n",nitems);
  
  // save some information from header.
  int eventID = data[2] & 0xfff;
  uint32_t timeTag = data[3];
  
  printf("V1740_ Evt#:%d timeTag:%x nwords:%d\n", eventID, timeTag, nitems);


  // Loop over all the 9-word blocks
  for(int i = 0; i < nblocks; i++){
    
    int offset = 4 + i*9;
    // write out the decoding for blocks of 9 words explicitly, otherwise hard to understand this bank decoding
    samples[0].push_back(((data[offset] & 0xfff))); 
    samples[0].push_back(((data[offset] & 0xfff000) >> 12 )); 
    samples[0].push_back(((data[offset] & 0xff000000) >> 24) + ((data[offset+1] & 0xf) <<8) ); 

    samples[1].push_back(((data[offset+1] & 0xfff0) >> 4)); 
    samples[1].push_back(((data[offset+1] & 0xfff0000) >> 16 )); 
    samples[1].push_back(((data[offset+1] & 0xf0000000) >> 28) + ((data[offset+2] & 0xff) <<4) ); 

    samples[2].push_back(((data[offset+2] & 0xfff00)>>8)); 
    samples[2].push_back(((data[offset+2] & 0xfff00000) >> 20 )); 

    offset += 3;

    samples[2].push_back(((data[offset] & 0xfff))); 

    samples[3].push_back(((data[offset] & 0xfff000) >> 12 )); 
    samples[3].push_back(((data[offset] & 0xff000000) >> 24) + ((data[offset+1] & 0xf) <<8) ); 
    samples[3].push_back(((data[offset+1] & 0xfff0) >> 4)); 

    samples[4].push_back(((data[offset+1] & 0xfff0000) >> 16 )); 
    samples[4].push_back(((data[offset+1] & 0xf0000000) >> 28) + ((data[offset+2] & 0xff) <<4) ); 
    samples[4].push_back(((data[offset+2] & 0xfff00)>>8)); 

    samples[5].push_back(((data[offset+2] & 0xfff00000) >> 20 )); 

    offset += 3;

    samples[5].push_back(((data[offset] & 0xfff))); 
    samples[5].push_back(((data[offset] & 0xfff000) >> 12 )); 

    samples[6].push_back(((data[offset] & 0xff000000) >> 24) + ((data[offset+1] & 0xf) <<8) ); 
    samples[6].push_back(((data[offset+1] & 0xfff0) >> 4)); 
    samples[6].push_back(((data[offset+1] & 0xfff0000) >> 16 )); 

    samples[7].push_back(((data[offset+1] & 0xf0000000) >> 28) + ((data[offset+2] & 0xff) <<4) ); 
    samples[7].push_back(((data[offset+2] & 0xfff00)>>8)); 
    samples[7].push_back(((data[offset+2] & 0xfff00000) >> 20 )); 

  }
  
  // save channel 0
  for(int j = 0; j < samples[0].size(); j++){
    V1730Channel0->SetBinContent(j+1,samples[0][j]);
  }

  //Analysis parameters (eventually they have to come from the ODB).
  //Shaping parameters:
  int m = 10;   //Top width
  int M = 937; //Decay time
  int k = 20;   //Rise time
  int N = samples[0].size(); //Length of recorded trace.
  //CFD parameters:
  double att_factor = 0.3;
  int delay = 25;
  //Edge detection parameters:
  int gate = 2*k+m;        //Analysis gate (= dead time).
  double threshold = 1000; //Triggering threshold.
  bool negative = false;   //Polarity (false <-> positive polarity).

  //Shape the signal
  TrapezoidShaper shaper(M,k,m);
  double *shaped = shaper.Shape(samples[0].data(),N);  
  for(int j = 0; j < N; j++){
    V1730Shaped0->SetBinContent(j+1,shaped[j]);
  }

  //Identify edges in the shaped signal.
  LeadingEdgeDiscriminator discriminator(threshold,negative);
  discriminator.SetDeadTime(gate);
  int hits = discriminator.Analyse(shaped,N);
  std::vector<double> & triggers =  discriminator.GetTriggers();
  
  //We loop over the hits and fill the spectra.
  for(int j=0; j<hits; j++){
    int start_bin = std::floor(triggers.at(j));
    double *ptr = shaped + start_bin;
    //Extract pulse heights.
    AmplitudeAnalyser e_analyser(negative);
    double E = e_analyser.Analyse(ptr,gate);
    V1730Energy0->Fill(E);
    //Extract pulse timings.
    ConstantFractionAnalyser t_analyser(att_factor,delay,negative);
    double t = t_analyser.Analyse(ptr,gate);
    V1730Time0->Fill((start_bin + t)*16.0);
  }

}
//---------------------------------------------------------------------------------
void HandleBOR_V1740(int run, int time)
{
  
  gOutputFile->cd();

  // Need to figure out how to automatically set number of bins

  int nbins = 630;

  V1730Channel0 = new TH1D("V1740Channel0","V1740Channel0",nbins,0,((float)nbins)*16.0);
  V1730Channel0->SetXTitle("Time (ns)");

  
  V1730Shaped0 =  new TH1D("V1740Shaped0","V1740Shaped0",nbins,0,((float)nbins)*16.0);
  V1730Shaped0->SetXTitle("Time (ns)");
  V1730Energy0 = new TH1D("V1730Energy0","Energy spectrum",4096,-0.5,4095.5);
  V1730Energy0->SetXTitle("Pulse height (raw adc value)");
  V1730Time0 = new TH1D("V1730Time0","Time spectrum",nbins,0,((float)nbins)*16.0);
  V1730Time0->SetXTitle("Time (ns)");

  printf("V1740 BOR\n");
	  
}
//---------------------------------------------------------------------------------
void HandleEOR_V1740(int run, int time)
{
	printf(" in V1740 EOR\n");
}
