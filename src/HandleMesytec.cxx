
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

#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <signal.h>
#include <stdint.h>

//#include "midasServer.h"
#include "TMidasEvent.h"
#include <TApplication.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TH2F.h>
#include <TNtuple.h>
#include <TFolder.h>
#include <TFile.h>
#include <TMath.h>
#include <TCutG.h>
#include "HandleMesytec.h"
#include "Globals.h"

int gMesytecnitems;

const int Nchannels = 16*32;
const int NICChannels=32;
const int NCsIChannels=32;
const int NSd1rChannels=32;
const int NSd1sChannels=32;
const int NSd2rChannels=32;
const int NSd2sChannels=32;
const int NSurChannels=32;
const int NSusChannels=32;
const int NYdChannels=4*32;
const int NYuChannels=4*32;
const int binlimit1 = 4200;
const int binlimitYd = 16.;
const int binlimit2 = 8400;
const float binlimitSd1 = 42.000;
const float binlimitSd2 = 200.00;
TH1D * hMes_P[Nchannels] = {NULL}; // Q

char var[50];

TCutG *protons = NULL;//proton cut
TCutG *elasticS3 = NULL;//elastic gate on S3
int usePeds = 1; //using pedestals instead of offsets for Silicon detectors AS
//AS Ion Chamber
TH1D * hIC[NICChannels] = {NULL}; // Q 
float ICEnergy=0; //Dummy for IC energy
float ICGain[NICChannels]={1.};
float ICOffset[NICChannels]={0.};


//AS CsI
TH1D * hCsI1[NCsIChannels] = {NULL}; // Q 
TH1D * hCsI1Sum = {NULL}; 
TH1D * hCsI2[NCsIChannels] = {NULL}; // Q 
TH1D * hCsI2Sum = {NULL};
float CsI[32]={0}, CsIEnergy, CsIEnergy2; //CsI energy 
float CsI1[32]={0}, CsI2[32]={0}, CsI1Energy, CsI1Energy2, CsI2Energy, CsI2Energy2; //CsI energy
int CsIChannel;
int CsIChannel2; // channel with the greatest value
int CsI1Channel;
int CsI1Channel2; // channel with the greatest value
int CsI2Channel;
int CsI2Channel2; // channel with the greatest value
double CsI1Gain[NCsIChannels]={1.};
double CsI1Ped[NCsIChannels]={0.};
double CsI2Gain[NCsIChannels]={1.};
double CsI2Ped[NCsIChannels]={0.};

//AS S3
TH1D * hSd2r[NSd2rChannels] = {NULL}; // Q 
float Sd2r[NSd2rChannels];
float Sd2rEnergy=0, Sd2rEnergy2= 0; //Dummy for Sd2r energy
int Sd2rChannel,  Sd2rChannel2; // channel with the greatest value
float Sd2rGain[NSd2rChannels]={1.};
float Sd2rOffset[NSd2rChannels]={0.};
float Sd2rPed[NSd2rChannels]={0.};

TH2D *hSd2rSummary = {NULL}; //summary spectra
Bool_t S3Hit;

TH1D * hSd2s[NSd2sChannels] = {NULL}; // Q     
float Sd2s[NSd2sChannels];
float Sd2sEnergy=0,  Sd2sEnergy2= 0; //Dummy for Sd2s energy  
int Sd2sChannel, Sd2sChannel2; // channel with the greatest value
float Sd2sGain[NSd2sChannels]={1.};
float Sd2sOffset[NSd2sChannels]={0.};
float Sd2sPed[NSd2sChannels]={0.};
TH2D *hSd2sSummary = {NULL}; //summary spectra

TH1D * hSd1r[NSd1rChannels] = {NULL}; // Q 
float Sd1r[NSd1rChannels];
float Sd1rEnergy=0, Sd1rEnergy2= 0; //Dummy for Sd1r energy
int Sd1rChannel,  Sd1rChannel2; // channel with the greatest value
float Sd1rGain[NSd1rChannels]={1.};
float Sd1rOffset[NSd1rChannels]={0.};
float Sd1rPed[NSd1rChannels]={0.};
TH2D *hSd1rSummary = {NULL}; //summary spectra

TH1D * hSd1s[NSd1sChannels] = {NULL}; // Q     
float Sd1s[NSd1sChannels];
float Sd1sEnergy=0,  Sd1sEnergy2=0; //Dummy for Sd1s energy      
int Sd1sChannel, Sd1sChannel2; // channel with the greatest value                                                                                    
float Sd1sGain[NSd1sChannels]={1.};
float Sd1sOffset[NSd1sChannels]={0.};
float Sd1sPed[NSd1sChannels]={0.};
TH2D *hSd1sSummary = {NULL}; //summary spectra

TH1D * hSur[NSurChannels] = {NULL}; //  Upstream S3 rings 
float Sur[NSurChannels];
float SurEnergy=0; //Dummy for Sur energy
int SurChannel; // channel with the greatest value
float SurGain[NSurChannels]={1.};
float SurOffset[NSurChannels]={0.};

TH1D * hSus[NSusChannels] = {NULL}; // Upstream S3 sectors   
float Sus[NSusChannels];
int SusChannel; // channel with the greatest value
float SusEnergy=0; //Dummy for Sus energy                                                                                          
float SusGain[NSusChannels]={1.};
float SusOffset[NSusChannels]={0.};


//AS YYd
TH1D * hYd[NYdChannels] = {NULL}; // Q
float Yd[NYdChannels] ={0}; 
float YdEnergy=0, YdEnergy2=0; //Dummy for Yd energy
int YdChannel, YdChannel2; // channel with the greatest value
float YdGain[NYdChannels]={1.};
float YdOffset[NYdChannels]={0.};
float YdPed[NYdChannels]={0.};
TH2D *hYdSummary = {NULL};


//SSB
TH1D * hSSB = NULL;
float SSBEnergy = 0;
float SSBOffset=0;
float SSBGain=0;


//YYu  
TH1D * hYu[NYuChannels] = {NULL}; // Q
float Yu[NYuChannels] ={0};
float YuEnergy=0, YuEnergy2=0;//Dummy for Yu energy
TH2D *hYuSummary = {NULL};                                           
int YuChannel, YuChannel2; // channel with the greatest value 
float YuGain[NYuChannels]={1.};
float YuOffset[NYuChannels]={0.};
float YuPed[NYuChannels]={0.};

//AS  Hit count histos
TH1D * hSd2rHits = {NULL}; // Downstream S3 2 rings   
TH1D * hSd2sHits = {NULL}; // Downstream S3 2 sectors 
TH1D * hSd1rHits = {NULL}; // Downstream S3 1 rings   
TH1D * hSd1sHits = {NULL}; // Downstream S3 1 sectors 
TH1D * hSurHits = {NULL}; // Upstream S3 rings   
TH1D * hSusHits = {NULL}; // Upstream S3 sectors   
TH1D * hSd2rElHits = {NULL}; // Downstream S3 2 rings with elastic gate                                                                        
TH1D * hSd2sElHits = {NULL}; // Downstream S3 2 sectors with elastic gate                                                                             
TH1D * hSd1rElHits = {NULL}; // Downstream S3 1 rings with elastic gate                                                                                 
TH1D * hSd1sElHits = {NULL}; // Downstream S3 1 sectors with elastic gate

TH1D * hYd1Hits = {NULL};  // Downstream Y11 1
TH1D * hYd2Hits = {NULL};  // Downstream Y11 2
TH1D * hYd3Hits = {NULL};  // Downstream Y11 3
TH1D * hYd4Hits = {NULL};  // Downstream Y11 4
TH1D * hYd5Hits = {NULL};  // Downstream Y11 5
TH1D * hYd6Hits = {NULL};  // Downstream Y11 6
TH1D * hYd7Hits = {NULL};  // Downstream Y11 7
TH1D * hYd8Hits = {NULL};  // Downstream Y11 8
TH1D * hYdHits = {NULL};  // Downstream Y11 rings
TH1D * hYdHitsProt = {NULL};  // Downstream Y11 hits with proton gate

TH1D * hYu1Hits = {NULL};  // Upstream Y11 1
TH1D * hYu2Hits = {NULL};  // Upstream Y11 2
TH1D * hYu3Hits = {NULL};  // Upstream Y11 3
TH1D * hYu4Hits = {NULL};  // Upstream Y11 4
TH1D * hYu5Hits = {NULL};  // Upstream Y11 5
TH1D * hYu6Hits = {NULL};  // Upstream Y11 6
TH1D * hYu7Hits = {NULL};  // Upstream Y11 7
TH1D * hYu8Hits = {NULL};  // Upstream Y11 8
TH1D * hYuHits = {NULL};  //  Upstream Y11 rings
TH1D * hYuHitsProt = {NULL};  // Upstream Y11 hits with proton gate

extern FILE* ASCIIYY1;
extern FILE* ASCIICsI;
extern FILE* ASCIISd1;
extern FILE* ASCIISd2;

int ascii =0; // bool for writing ascii AS

uint32_t adcThresh = 0;
int ydNo;
//AS Total energies

TH1D *hSdETot = {NULL};
TH1D *hYdCsIETot = {NULL};
//AS DE-E spectra
TH2F *hSdPID = {NULL};
TH2F *hYdCsIPID2 = {NULL};
TH2F *hYdCsIPID1 = {NULL};

//AS Angles
TH1D *hSdTheta = {NULL};
TH1D *hSdPhi = {NULL};
TH2D *hSdETheta = {NULL};
TH2D *hSdPhiTheta = {NULL};
TH1D *hYdTheta = {NULL};
TH1D *hYuTheta = {NULL};


float YdDistance = 120; // distance from target in mm
float Yd1r=50, Yd2r = 130 ; // inner and outer radii in mm
float YuDistance = 83; // distance from target in mm
float Yu1r=50, Yu2r = 130 ; // inner and outer radii in mm
float Sd1Distance =650, sSd2Distance= 660; //distance from target in mm
float Sdr1=11,Sdr2=35; //AS Inner and outer radii of an S3 detector (in mm).
TH1D *hSuTheta = {NULL};
TH1D *hSuPhi = {NULL};
float SuDistance = 200; //AS distance from target in mm
float theta = 0 ,phi = 0, theta1 = 0, phi1=0; //AS Dummies for theta and phi, to be used for filling histograms
float Sdtheta = 0 ,Sdphi = 0, Sdtheta1 = 0, Sdphi1=0; //AS Dummies for theta and phi, to be used for filling histograms

uint32_t modid, oformat, vpeak, resolution, evlength, timestamp;
uint32_t channel, overflow;
void HandleBOR_Mesytec(int run, int time);
void HandleEOR_Mesytec(int run, int time);

int clearDetectors()
{
  for (int j=0; j<NCsIChannels; j++)
  CsI[j] = 0;

 for (int j=0; j<NSd2rChannels; j++)
  Sd2r[j] = 0;

 for (int j=0; j<NSd2sChannels; j++)
  Sd2s[j] = 0;


 for (int j=0; j<NSd1rChannels; j++)
  Sd1r[j] = 0;

 for (int j=0; j<NSd1sChannels; j++)
  Sd1s[j] = 0;

 for (int j=0; j<NYdChannels; j++)
  Yd[j] = 0;

     theta = 0; 
     phi = 0;
     theta1=0; 
     phi1=0;
     Sdtheta = 0; 
     Sdphi = 0;
     Sdtheta1=0; 
     Sdphi1=0;

  return 0;
}

void HandleMesytec(TMidasEvent& event, void* ptr, int nitems, int MYLABEL, det_t *det)
{
  uint32_t *data;
  int    i, debug = 0, debug1 = 0;
  int eventId = event.GetEventId();

  // clearDetectors();

  data = (uint32_t *) ptr;
  if ( 1 || MYLABEL == 0 ) {
    if (hMes_P[0] == NULL)
      HandleBOR_Mesytec(gRunNumber, 0);    
    // Reset Waveform histos before scanning ALL banks
    //    hMes_P[channel]->Reset();

    // published var for HandlePHYSICS
    gMesytecnitems = nitems;
 
   
    // Loop over all the banks
    if (nitems != 0) {
       if (debug) {
printf("Mesytec_ Evt#:%d nitems:%d\n", event.GetSerialNumber(), nitems);     
} 
     
      for (i=0 ; i<nitems ; i++) {

	switch (data[i] & 0xFF000000) {
	case 0x40000000: // Header
	  modid = ((data[i] & 0xFF0000) >> 16);
	  oformat = (data[i] & 0x8000) ? 1 : 0;
	  resolution = ((data[i] & 0x7000) >> 12);
	  evlength = (data[i] & 0xFFF);

	 if (debug) {
	  printf("Header: id:%d of:%d res:%d el:%d\n"
		   , modid, oformat, resolution, evlength);

	   
	  }
	  break;
	case 0xC0000000: // Trailer Event
	case 0xD0000000: // Trailer Event
	case 0xE0000000: // Trailer Event
	case 0xF0000000: // Trailer Event
	  timestamp = data[i] & 0x3FFFFFFF;
	  // if (debug) {
	  //   printf("Trailer: id:%d of:%d res:%d el:%d ts:%d\n"
	  // 	   , modid, oformat, resolution, evlength, timestamp);
	  // }
	 
	  break;
	case 0x04000000: // Data[I] Event
	  channel  = ((data[i] & 0x001F0000) >> 16);

	  if ((channel >= 0) && (channel<16))
	    channel = 15 - channel; //AS Flipping channels 1-16 in shapers (due to preamp box channel flipping issue) 0->15, 1->14, ..., 15->0
	  else if ((channel>15) && (channel <32))
	    channel = 47 - channel; //AS Flipping channels 17-32 in shapers (due to preamp box channel flipping issue) 16->31, 17->30, ...,31->16
	  overflow = (data[i] & 0x4000) ? 1 : 0;
	  vpeak    = (data[i] & 0x1FFF);
	  //Set a software threshold 

	  //if (overflow)
	  //vpeak = 0xFFFF;
	 
	 
	  if (debug1  && modid==15) printf("Evt#:%d items:%d - data[%d]: %d / 0x%x\n", event.GetSerialNumber(),Nchannels, i, data[i], data[i]); 

	  if (debug1  && modid==15) {
	    printf("Data: ch:%d id:%d val:%f\n", channel, modid, (float) vpeak);
	  }
	  hMes_P[channel+(modid*32)]->Fill((float)vpeak , 1.);
	  // RK :  Energy Calibration 
	  
	  if ((modid==0) && (vpeak > adcThresh) && (vpeak<3840)){

	    //AS Fill histogram
	    ICEnergy = ((float)vpeak-ICOffset[channel])*ICGain[channel];
	    hIC[channel]->Fill(ICEnergy, 1.); //IC
	    
	    //SSB
	    if (channel==16)
	      SSBEnergy = float(vpeak) *SSBGain + SSBOffset; 
	    hSSB->Fill(SSBEnergy,1.);
	  }
	  
	  if (modid==1 && vpeak > adcThresh && vpeak<3840){
	    if (channel<16){
	    CsI1[channel] = (float)vpeak;
	    hCsI1[channel]->Fill(CsI1[channel], 1.); //CsI		    
hCsI1Sum->Fill(CsI1[channel], 1.); //CsI
	  } 
	    else if (channel>15){
	    CsI2[channel-16] = (float)vpeak;
	    hCsI2[channel-16]->Fill(CsI2[channel-16], 1.); //CsI		    
hCsI2Sum->Fill(CsI2[channel-16], 1.); //CsI
	    } 
	  } //if modid
	  else if (vpeak==3840) {
  if (channel<16)
CsI1[channel] = -100.;
  else if (channel>15)
CsI2[channel] = -100.;
}

	  
	  if (modid==2 && vpeak > adcThresh && vpeak<3840){
	 S3Hit = 1; 
	 //Sd2rEnergy = Sd2rOffset[channel]+Sd2rGain[channel]*(float)vpeak;
	 Sd2rEnergy = Sd2rGain[channel]*((float)vpeak-Sd2rPed[channel]);
	 Sd2r[channel]=Sd2rEnergy;
	    hSd2r[channel]->Fill(Sd2rEnergy, 1.); //Sd2r 
	    hSd2rSummary->Fill(channel,Sd2rEnergy); //Sd2r 
	  }
	  
	  if (modid==3 && vpeak > adcThresh && vpeak<3840){
	    S3Hit = 1; 
	 //	    Sd2sEnergy = Sd2sOffset[channel]+Sd2sGain[channel]*(float)vpeak;
	 Sd2sEnergy = Sd2sGain[channel]*((float)vpeak - Sd2sPed[channel]);	 
	    hSd2s[channel]->Fill(Sd2sEnergy, 1.); //Sd2s 
	 Sd2s[channel]=Sd2sEnergy;
	    hSd2sSummary->Fill(channel,Sd2sEnergy); //Sd2r 	  
}

	  if (modid==4 && vpeak > adcThresh  && vpeak<3840){
 S3Hit = 1; 
	 // Sd1rEnergy = Sd1rOffset[channel]+Sd1rGain[channel]*(float)vpeak;
	 Sd1rEnergy = Sd1rGain[channel]*((float)vpeak-Sd1rPed[channel]);
	 Sd1r[channel]=Sd1rEnergy;
	    hSd1r[channel]->Fill(Sd1rEnergy, 1.); //Sd1r 
	  	    hSd1rSummary->Fill(channel,Sd1rEnergy); //Sd1r 
		    
}
	  if (modid==5 && vpeak > adcThresh  && vpeak<3840){

	     S3Hit = 1; 
	 //Sd1sEnergy = Sd1sOffset[channel]+Sd1sGain[channel]*(float)vpeak;
	    Sd1sEnergy = Sd1sGain[channel]*((float)vpeak-Sd1sPed[channel]);
	 Sd1s[channel]=Sd1sEnergy;
	    hSd1s[channel]->Fill(Sd1sEnergy, 1.); //Sd1s 
	    hSd1sSummary->Fill(channel,Sd1sEnergy); //Sd1s

 	 }
	
	  //if (S3Hit) printf("S3Hit: %d, i: %d\n",S3Hit,i);
	  //AS Total energy for downstream S3s.
	  if (Sd1rEnergy!=0) hSdETot->Fill(Sd1rEnergy+Sd2rEnergy);
	  
	  //AS DE-E for downstream S3s.
	  //	  if (Sd1rEnergy!=0 && Sd2rEnergy!=0) hSdPID->Fill(Sd2rEnergy,Sd1rEnergy);
	  
	  if (modid==10  && vpeak> adcThresh && vpeak<3840){

	    
	    SurEnergy = ((float)vpeak-SurOffset[channel+(modid-2)*32])*SurGain[channel+(modid-2)*32];
	    hSur[channel]->Fill(SurEnergy, 1.); //Sur 
	  }
	  
	  if (modid==11  && vpeak > adcThresh && vpeak<3840){
	    
	    SusEnergy = ((float)vpeak-SusOffset[channel+(modid-2)*32])*SusGain[channel+(modid-2)*32];
	    hSus[channel]->Fill(SusEnergy, 1.); //Sus 
	  }
	  
	  if (modid>5 && modid<10 && vpeak>adcThresh  && vpeak<3840){
	  
	    YdEnergy = (float)vpeak;
            
            YdEnergy=(YdEnergy-YdPed[channel+(modid-6)*32])*YdGain[channel+(modid-6)*32];
	    Yd[channel+(modid-6)*32] = YdEnergy;
	    hYd[channel+(modid-6)*32]->Fill(YdEnergy, 1.); //Yd                                                    
	    hYdSummary->Fill(channel+(modid-6)*32,YdEnergy); //Yd
	 
	    //printf("YdEnergy: %f, vpeak: %d, gain: %f\n",YdEnergy, vpeak, YdGain[channel+(modid-6)*32]);
	    if (channel<16) ydNo = (modid-6)*2+1; //Yd number
	    if (channel>15) ydNo = (modid-6)*2+2;

	    // if (CsI2[ydNo*2-1]>CsI2[ydNo*2]) CsI2Energy = CsI2[ydNo*2-1]; ///Check if a CsI is behind a YY1
	    // else if  (CsI2[ydNo*2-1]<CsI2[ydNo*2]) CsI2Energy = CsI2[ydNo*2];
 //AS Total energy for downstream YY1s and CsIs.
	 
	  
	  //AS DE-E for downstream YY1s and CsIs.

	  //if (CsIEnergy>adcThresh && YdEnergy!=0 ) hYdCsIPID->Fill(CsIEnergy,YdEnergy);//moved to out of the nitem for loop  AS
	
	  } //modid
	  
	 
	  
	  if (modid>11 && modid<16 && vpeak > adcThresh  && vpeak<3840){  
	    
	    YuEnergy = ((float)vpeak-YuOffset[channel+(modid-12)*32])*YuGain[channel+(modid-12)*32];
	   Yu[channel+(modid-12)*32] = YuEnergy; 
	   hYu[channel]->Fill(YuEnergy, 1.); //Yu
	   hYuSummary->Fill(channel+(modid-12)*32,YuEnergy); //Yu                                                             
	  }
	  

	    //hits 
	  {
	    if (modid==2 && vpeak>adcThresh){	 
	    hSd2rHits->Fill(channel, 1.); //Sd2r hits  
}
	    if (modid==3 && vpeak>500)	 {
	    hSd2sHits->Fill(channel, 1.); //Sd2s hits
	  }

 if (modid==4 && vpeak>adcThresh && vpeak<3500)	 {
	    hSd1rHits->Fill(channel, 1.); //Sd1r hits
	  }
	  
 if (modid==5 && vpeak>500)	{	     
	    hSd1sHits->Fill(channel, 1.); //Sd1s hits 
 }

	  if (modid==10 && vpeak>adcThresh)	 
	    hSurHits->Fill(channel, 1.); //Sur hits 
	  
	  if (modid==11 && vpeak>adcThresh)	 
	    hSusHits->Fill(channel, 1.); //Sus hits

if (modid==6 && vpeak>adcThresh && channel<16)
  hYd1Hits->Fill(channel,1.);

if (modid==6 && vpeak>adcThresh  && channel>15)
  hYd2Hits->Fill(channel-16,1.);

if (modid==7 && vpeak>adcThresh && channel<16)
  hYd3Hits->Fill(channel,1.);

if (modid==7 && vpeak>adcThresh && channel>15)
  hYd4Hits->Fill(channel-16,1.);

if (modid==8 && vpeak>adcThresh && channel<16)
  hYd5Hits->Fill(channel,1.);

if (modid==8 && vpeak>adcThresh && channel>15)
  hYd6Hits->Fill(channel-16,1.);

if (modid==9 && vpeak>adcThresh && channel<16)
  hYd7Hits->Fill(channel,1.);

if (modid==9 && vpeak>adcThresh && channel>15)
  hYd8Hits->Fill(channel-16,1.);

if (modid>5 && modid<10 && vpeak>adcThresh && channel<16)
  hYdHits->Fill(channel,1.);

if (modid>5 && modid<10 && vpeak>adcThresh && channel>15)
  hYdHits->Fill(channel-16,1.);
/************temp commented out AS
if (modid==12 && vpeak>adcThresh && channel<16) //Yu Hits
  hYu1Hits->Fill(channel,1.);

if (modid==12 && vpeak>adcThresh  && channel>15)
  hYu2Hits->Fill(channel-16,1.);

if (modid==13 && vpeak>adcThresh && channel<16)
  hYu3Hits->Fill(channel,1.);

if (modid==13 && vpeak>adcThresh && channel>15)
  hYu4Hits->Fill(channel-16,1.);

if (modid==14 && vpeak>adcThresh && channel<16)
  hYu5Hits->Fill(channel,1.);

if (modid==14 && vpeak>adcThresh && channel>15)
  hYu6Hits->Fill(channel-16,1.);

if (modid==15 && vpeak>adcThresh && channel<16)
  hYu7Hits->Fill(channel,1.);

if (modid==15 && vpeak>adcThresh && channel>15)
  hYu8Hits->Fill(channel-16,1.);

if (modid>11 && modid<16 && vpeak>adcThresh && channel<16)
  hYuHits->Fill(channel,1.);

if (modid>11 && modid<16 && vpeak>adcThresh && channel>15)
  hYuHits->Fill(channel-16,1.);
*************/
	  } //hits
	  
	 
	  //  if (debug1  && event.GetSerialNumber()<21 &&  event.GetSerialNumber()>13 && modid==1) printf("ch[%d modid:%d]:%d/ov:%d\n", channel, modid, vpeak, overflow);
	  break;
	} // switch
      } // for loop
 
    } // nitems != 0
    

//After looping over banks, fill the ascii files
    if (modid>5 && modid<10) {// check last bank

      det->EventNumber = event.GetSerialNumber();
    
 Sd2rEnergy=0; Sd2rEnergy2 =0; Sd2rChannel = -10000; Sd2rChannel2 =-10000;
 for (int i =0; i< NSd2rChannels;i++){
      if (Sd2rEnergy<Sd2r[i]){
          Sd2rEnergy2 = Sd2rEnergy;
	  Sd2rChannel2 = Sd2rChannel;
          Sd2rEnergy=Sd2r[i];
          Sd2rChannel = i;}
	else if (Sd2rEnergy2<Sd2r[i])
	  {     Sd2rEnergy2=Sd2r[i];
	Sd2rChannel2 = i;}
 } //for

     Sd2sEnergy=0; Sd2sEnergy2 =0; Sd2sChannel = -10000; Sd2sChannel2 =-10000;
    for (int i =0; i< NSd2sChannels;i++)
      {if (Sd2sEnergy<Sd2s[i]){
	  Sd2sEnergy2 = Sd2sEnergy;
	  Sd2sChannel2 = Sd2sChannel;
          Sd2sEnergy=Sd2s[i];
          Sd2sChannel = i;}
	else if (Sd2sEnergy2<Sd2s[i])
	  {     Sd2sEnergy2=Sd2s[i];
	Sd2sChannel2 = i;}
      } //for

  if (ascii) 
    fprintf(ASCIISd2," %d  %d %d %d %d %d %d %d %d \n",event.GetSerialNumber(), Sd2rChannel+64, (int)Sd2rEnergy,  Sd2rChannel2+64, (int)Sd2rEnergy2,  Sd2sChannel+96, (int)Sd2sEnergy,  Sd2sChannel2+96, (int)Sd2sEnergy2);
  
  //if (Sd2rEnergy >0)
  // Sd2rEnergy = (Sd2rEnergy-Sd2rOffset[Sd2rChannel])*Sd2rGain[Sd2rChannel];
//if (Sd2sEnergy >0)  
  //Sd2sEnergy = (Sd2sEnergy-Sd2sOffset[Sd2sChannel])*Sd2sGain[Sd2sChannel];
  

 Sd1rEnergy=0; Sd1rEnergy2 =0; Sd1rChannel = -10000; Sd1rChannel2 =-10000;
    for (int i =0; i< NSd1rChannels;i++)
      {if (Sd1rEnergy<Sd1r[i]){
          Sd1rEnergy2 = Sd1rEnergy;
	  Sd1rChannel2 = Sd1rChannel;
          Sd1rEnergy=Sd1r[i];
          Sd1rChannel = i;}

	else if (Sd1rEnergy2<Sd1r[i])
	  {     Sd1rEnergy2=Sd1r[i];
	Sd1rChannel2 = i;}
      }//for


 Sd1sEnergy=0; Sd1sEnergy2 =0; Sd1sChannel = -10000; Sd1sChannel2 =-10000;
    for (int i =0; i< NSd1sChannels;i++)
      {if (Sd1sEnergy<Sd1s[i]){
          Sd1sEnergy2 = Sd1sEnergy;
	  Sd1sChannel2 = Sd1sChannel;
          Sd1sEnergy=Sd1s[i];
          Sd1sChannel = i;}
	else if (Sd1sEnergy2<Sd1s[i])
	  {     Sd1sEnergy2=Sd1s[i];
	Sd1sChannel2 = i;}
      } //for

  if (ascii) 
   fprintf(ASCIISd1," %d  %d %d %d %d %d %d %d %d \n",event.GetSerialNumber(), Sd1rChannel+64, (int)Sd1rEnergy,  Sd1rChannel2+64, (int)Sd1rEnergy2,  Sd1sChannel+96, (int)Sd1sEnergy,  Sd1sChannel2+96, (int)Sd1sEnergy2);
 
  //  if (Sd1rEnergy >0)
  // Sd1rEnergy = (Sd1rEnergy-Sd1rOffset[Sd1rChannel])*Sd1rGain[Sd1rChannel];
  //if (Sd1sEnergy >0)
  //Sd1sEnergy = (Sd1sEnergy-Sd1sOffset[Sd1sChannel])*Sd1sGain[Sd1sChannel];
    
  det->TSd2rEnergy = Sd2rEnergy;
  det->TSd2sEnergy = Sd2sEnergy;
  det->TSd1rEnergy = Sd1rEnergy;
  det->TSd1sEnergy = Sd1sEnergy;
  det->TSd2rChannel = Sd2rChannel;
  det->TSd2sChannel = Sd2sChannel;
  det->TSd1rChannel = Sd1rChannel;
  det->TSd1sChannel = Sd1sChannel;

    YdEnergy=0; YdChannel = -10000; YdEnergy2=0; YdChannel2 =-10000;
    for (int i =0; i< NYdChannels;i++)
      {
	  if(Yd[i]>YdEnergy){
  YdEnergy2= YdEnergy;
	   YdChannel2 = YdChannel;
	  YdEnergy=Yd[i];
	  YdChannel = i;
	  }
 else if (Yd[i]>YdEnergy2)//if Yd[i] is between YdEnergy and YdEnergy2
	    {YdEnergy2=Yd[i];
	  YdChannel2 = i;
	    } //if

      } //for      
    
  if (ascii) 
    fprintf(ASCIIYY1," %d  %d %d %d %d \n",event.GetSerialNumber(), YdChannel+192, (int)YdEnergy,  YdChannel2+192, (int)YdEnergy2);
  
  //if (YdEnergy>0)
    // YdEnergy = YdEnergy*YdGain[YdChannel]+YdOffset[YdChannel];
   
 
  det->TYdEnergy = YdEnergy;
  det->TYdChannel = YdChannel;
  //here
  theta = TMath::RadToDeg()*atan((Yd1r*(16.-YdChannel%16-0.5)+Yd2r*(YdChannel%16+0.5))/16./YdDistance);
	   det->TYdTheta= theta;
	   //hYdTheta -> Fill(theta);
	 

//YYu
    YuEnergy=0; YuChannel = -10000; YuEnergy2=0; YuChannel2 =-10000;
    for (int i =0; i< NYuChannels;i++)
      {
          if(Yu[i]>YuEnergy){
  YuEnergy2= YuEnergy;
           YuChannel2 = YuChannel;
          YuEnergy=Yu[i];
          YuChannel = i;
          }
 else if (Yu[i]>YuEnergy2)//if Yu[i] is between YuEnergy and YuEnergy2
            {YuEnergy2=Yu[i];
          YuChannel2 = i;
            } //if

      } //for      

  if (ascii)
    fprintf(ASCIIYY1," %d  %d %d %d %d \n",event.GetSerialNumber(), YuChannel+192, (int)YuEnergy,  YuChannel2+192, (int)YuEnergy2);

  //if (YuEnergy>0)
  //    // YuEnergy = YuEnergy*YuGain[YuChannel]+YuOffset[YuChannel];
  
      det->TYuEnergy = YuEnergy;
      det->TYuChannel = YuChannel;
            //here
      theta = TMath::RadToDeg()*atan((Yu1r*(16.-YuChannel%16-0.5)+Yu2r*(YuChannel%16+0.5))/16./YuDistance);
      det->TYuTheta= theta;
      // hYuTheta -> Fill(theta);
  




  //CsI
  CsIEnergy=0; CsIEnergy2 =0; CsIChannel = -100; CsIChannel2 =-100;
    for (int i =0; i< NCsIChannels;i++) {
      // printf("CsI ch: %d, value %f\n", i, CsI[i]);
      if (CsIEnergy<CsI[i]){
	  CsIEnergy2 = CsIEnergy;
	  CsIChannel2 = CsIChannel;
          CsIEnergy=CsI[i];
          CsIChannel = i;}
      else  if (CsIEnergy2<CsI[i]){
          CsIEnergy2=CsI[i];
          CsIChannel2 = i;}
    } //for
   
    CsI1Energy=0; CsI1Energy2 =0; CsI1Channel = -100; CsI1Channel2 =-100;
    for (int i =0; i< 16;i++) {
      // printf("CsI ch: %d, value %f\n", i, CsI[i]);                                                              
      if (CsI1Energy<CsI1[i]){
	CsI1Energy2 = CsI1Energy;
	CsI1Channel2 = CsI1Channel;
	CsI1Energy=CsI1[i];
	CsI1Channel = i;}
      else  if (CsI1Energy2<CsI1[i]){
	CsI1Energy2=CsI1[i];
	CsI1Channel2 = i;}
    } //for                                                                                                        



    CsI2Energy=0; CsI2Energy2 =0; CsI2Channel = -100; CsI2Channel2 =-100;
    for (int i =0; i< 16;i++) {
      // printf("CsI ch: %d, value %f\n", i, CsI[i]);                                                              
      if (CsI2Energy<CsI2[i]){
	CsI2Energy2 = CsI2Energy;
	CsI2Channel2 = CsI2Channel;
	CsI2Energy=CsI2[i];
	CsI2Channel = i;}
      else  if (CsI2Energy2<CsI2[i]){
	CsI2Energy2=CsI2[i];
	CsI2Channel2 = i;}
    } //for                                                                                                        


    if (ascii)  fprintf(ASCIICsI," %d  %d %d %d %d \n",event.GetSerialNumber(), CsIChannel+32, (int)CsIEnergy,  CsIChannel2+32, (int)CsIEnergy2);

     if(CsI1Channel>=0)
       CsI1Energy = (CsI1Energy-CsI1Ped[CsI1Channel])*CsI1Gain[CsI1Channel];
     if(CsI2Channel>=0)
     CsI2Energy = (CsI2Energy-CsI2Ped[CsI2Channel])*CsI2Gain[CsI2Channel];
     CsI2Energy = 0.2*CsI2Energy-0.13;  //tk this is temporary until the calibratio is fixed for S1338
     // CsIEnergy = CsIEnergy*0.01;//multiplied by a rough gain of 10/ now by 0.01(to get in MeV).
     if (1){// if((CsIChannel-16)/2==ydNo){
    det->TCsIEnergy = CsI1Energy; //for filling the tree
    det->TCsIChannel = CsI1Channel;
    }

    else {
    det->TCsIEnergy = -100; //for filling the tree
    det->TCsIChannel = -100;
    }

     if (1){// if((CsIChannel-16)/2==ydNo){
    det->TCsI1Energy = CsI1Energy; //for filling the tree
    det->TCsI1Channel = CsI1Channel;
    }

   if (1){// if((CsIChannel-16)/2==ydNo){
    det->TCsI2Energy = CsI2Energy; //for filling the tree
    det->TCsI2Channel = CsI2Channel;
    }
    //printf("modid: %d , CsIEnergy: %f \n",modid, CsIEnergy);

 //AS angles

	  if (Sd1rEnergy>0){
	    Sdtheta = TMath::RadToDeg()*(Sdr1*(24.-Sd1rChannel-0.5)+Sdr2*(Sd1rChannel+0.5))/24./Sd1Distance; //AS theta angle for Sd (24 - number of rings)
	    hSdTheta->Fill(Sdtheta);
	  }
	   if (Sd1rEnergy>0){
	    Sdphi = (Sd1sChannel*180./32.); //AS phi angle for Sd (32 - number of sectors)
	    hSdPhi->Fill(Sdphi);
	 
	   }
	


  //Fill phitheta and E-theta histogram	  
	  if (Sdtheta>0){
	    hSdPhiTheta->Fill(theta,phi);
	    hSdETheta->Fill(Sdtheta,Sd2sEnergy+Sd1sEnergy);
	  }
	  
	  if ((fabs(Sd1sEnergy-Sd1rEnergy)<1.5) && (fabs(Sd2sEnergy-Sd2rEnergy)<1.5)) 
hSdPID->Fill(Sd2sEnergy,Sd1sEnergy);

	        if (elasticS3->IsInside(Sd2sEnergy,Sd1sEnergy))
	    hSd2rElHits->Fill(Sd2rChannel, 1.); //Sd2r hits with elastic gat

 if (elasticS3->IsInside(Sd2sEnergy,Sd1sEnergy))
	    hSd2sElHits->Fill(Sd2sChannel, 1.); //Sd2r hits with elastic gate

 if (elasticS3->IsInside(Sd2sEnergy,Sd1sEnergy))
	    hSd1rElHits->Fill(Sd1rChannel, 1.); //Sd2r hits with elastic gate

 if (elasticS3->IsInside(Sd2sEnergy,Sd1sEnergy))
	    hSd1sElHits->Fill(Sd1sChannel, 1.); //Sd2r hits with elastic gate

  ydNo = YdChannel/16; //Yd number                                                                               
  //{    
      hYdCsIPID2->Fill(CsI2Energy,YdEnergy*cos(det->TYdTheta*0.01745329));
       //    }
 if (YdEnergy!=0) hYdCsIETot->Fill(YdEnergy); // AS Note: Add cos theta after calculation of angles is added
       // if((CsI2Channel-16)/2==ydNo && YdChannel%16 ==1)  {    
 hYdCsIPID1->Fill(CsI1Energy,YdEnergy*cos(det->TYdTheta*0.01745329));
     // }
     if (protons->IsInside(CsI1Energy,YdEnergy*cos(det->TYdTheta*1.74532925199432955e-02))){
       hYdHitsProt->Fill(YdChannel,1.);
       }

    } //last bank
  } // label
}

//---------------------------------------------------------------------------------
void HandleBOR_Mesytec(int run, int time)
{

  //proton gate
  TFile *fgate = new TFile("/home/iris/anaIris/gates/cuts.root");
  protons = (TCutG*)fgate->FindObjectAny("protons1");
  protons->SetName("protons");
  elasticS3 =  (TCutG*)fgate->FindObjectAny("elasS312C");
  char label[32], sig[32];

  // Booking 
  printf(" in Mes BOR... Trying to book\n");
  gOutputFile->cd();

 //ASCII output file
    if (ascii) {
  sprintf(label,"ASCIIYY1run%d.txt",gRunNumber);    
  ASCIIYY1 = fopen(label,"w");
 fprintf(ASCIIYY1,"Evt:  id1: vpeak1: id2: vpeak2 \n");

  sprintf(label,"ASCIICsIrun%d.txt",gRunNumber); 
 ASCIICsI = fopen(label,"w");
 fprintf(ASCIICsI,"Evt:  id1: vpeak1: id2: vpeak2 \n");

  sprintf(label,"ASCIISd2run%d.txt",gRunNumber); 
 ASCIISd2 = fopen(label,"w");
 fprintf(ASCIISd2,"Evt:  idr1: vpeakr1: idr2: vpeakr2:  ids1: vpeaks1: ids2: vpeaks2 \n");

 sprintf(label,"ASCIISd1run%d.txt",gRunNumber);
ASCIISd1 = fopen(label,"w");
 fprintf(ASCIISd1,"Evt:  idr1: vpeakr1: idr2: vpeakr2:  ids1: vpeaks1: ids2: vpeaks2 \n");
    } //ascii AS

  hMes_P[0] = (TH1D*)gDirectory->Get("adc00");
  if (hMes_P[0] == 0) {
    if (gOutputFile) {

      // Make an ADC directory and cd to it.
      TDirectory* adc_dir = gOutputFile->mkdir("adc");      
      adc_dir->cd();

      TDirectory* adcIC_dir = adc_dir->mkdir("adcIC");
      TDirectory* adcCsI_dir = adc_dir->mkdir("adcCsI");
      TDirectory* adcSd2_dir = adc_dir->mkdir("adcSd2");
      TDirectory* adcSd1_dir = adc_dir->mkdir("adcSd1");
      TDirectory* adcYYd_dir = adc_dir->mkdir("adcYYd");
      //TDirectory* adcUPSTREAM_dir = adc_dir->mkdir("adc_UPSTREAM");
      TDirectory* adcSu_dir = adc_dir->mkdir("adcSu");
      TDirectory* adcYYu_dir = adc_dir->mkdir("adcYYU");
     

      printf(" in Mesytec BOR... Booking histos\n");
      for (int channel=0;channel<Nchannels;channel++) {
	if (channel<32) adcIC_dir->cd();
	else if (channel>=32 && channel <64) adcCsI_dir->cd();
	else if (channel>=64 && channel <128) adcSd2_dir->cd();
	else if (channel>=128 && channel <192) adcSd1_dir->cd();
	else if(channel>=192 && channel < 320) adcYYd_dir->cd();
	else if(channel>=320 && channel < 384) adcSu_dir->cd();
        else if(channel>=384 && channel < 512) adcYYu_dir->cd();



sprintf(label, "adc%02d", channel);
	sprintf(sig, "adc%03d", channel);
	hMes_P[channel] = new TH1D(label, sig, binlimit1, 0, binlimit1);
	printf("Booking TH1D %s \n", label);
      }
      printf(" in Mesytec BOR... Booking histos Done ....\n");
    }
  }
  
  // return to overall file directory
  gOutputFile->cd();

//AS Read calibration parameters from the Odb.
for(int channel = 0; channel < NICChannels; channel++){
  sprintf(var,"/Calibration/IC/Offset");	
   ICOffset[channel] = (float)gOdb->odbReadFloat(var,channel);	
 ICGain[channel] = (float)gOdb->odbReadFloat(var,channel);
//printf("Gain = %f\n",ICGain[channel]);
 }
 printf(" Reading IC calibration parameters Done ....\n");

 hIC[0] = (TH1D*)gDirectory->Get("IC00");
   if (hIC[0] == 0) {
     if (gOutputFile) {

     // Make an IC directory and cd to it.
        TDirectory* IC_dir = gOutputFile->mkdir("IC");      
        IC_dir->cd();
       
printf(" in Mesytec BOR... Booking IC histos\n");
       for (int channel=0;channel<NICChannels;channel++) {
  	sprintf(label, "IC%02d", channel);
  	sprintf(sig, "IC%03d", channel);
  	hIC[channel] = new TH1D(label, sig, binlimit1, 0, binlimit1);
  	printf("Booking TH1D %s \n", label);
       }
       printf(" in Mesytec BOR... Booking IC histos Done ....\n");
     }
   }

//AS Read calibration parameters from ./calib-files/CsICalib.txt
FILE * pFile;
FILE * pwFile;
 char buffer[30];
 Int_t CsIChan=-10000;  
 double a,b;



   pFile = fopen ("/home/iris/anaIris/calib-files/CsI1PedGainS1147_11Li.txt" , "r");
   if (pFile == NULL) {
perror ("Error opening file");
fprintf(pwFile,"Error opening file");
   }  
 else  {

  fscanf(pFile,"%s",buffer);

  fscanf(pFile,"%s",buffer);

 fscanf(pFile,"%s",buffer);


for (int i =0;i<NCsIChannels;i++  ){
       fscanf(pFile,"%d%lf%lf",&CsIChan,&a,&b);
CsI1Ped[CsIChan] = a;
CsI1Gain[CsIChan] = b;
     }
     fclose (pFile);
   }

   pFile = fopen ("/home/iris/anaIris/calib-files/CsI2PedGainS1147_11Li.txt" , "r");
   if (pFile == NULL) {
perror ("Error opening file");
fprintf(pwFile,"Error opening file");
   }  
 else  {

  fscanf(pFile,"%s",buffer);

  fscanf(pFile,"%s",buffer);

 fscanf(pFile,"%s",buffer);


for (int i =0;i<NCsIChannels;i++  ){
       fscanf(pFile,"%d%lf%lf",&CsIChan,&a,&b);
       CsI2Ped[CsIChan] = a; //a/0.177;
       CsI2Gain[CsIChan] = b; //b*0.177-23.3;
     }
     fclose (pFile);
   }


 printf(" Reading CsI calibration parameters Done ....\n");

  hCsI1[0] = (TH1D*)gDirectory->Get("CsI00");
   if (hCsI1[0] == 0) {
     if (gOutputFile) {
       // gOutputFile->cd();

     // Make a CSI directory and cd to it.
       TDirectory* CsI_dir = gOutputFile->mkdir("CsI");      
       CsI_dir->cd();

       printf(" in Mesytec BOR... Booking CsI1 histos\n");
       for (int channel=0;channel<NCsIChannels;channel++) {
  	sprintf(label, "CsI1%02d", channel);
  	sprintf(sig, "CsI1%03d", channel);
  	hCsI1[channel] = new TH1D(label, sig, binlimit2, 0, binlimit2);
  	printf("Booking TH1D %s \n", label);
       }
         for (int channel=0;channel<NCsIChannels;channel++) {
  	sprintf(label, "CsI2%02d", channel);
  	sprintf(sig, "CsI2%03d", channel);
  	hCsI2[channel] = new TH1D(label, sig, binlimit2, 0, binlimit2);
  	printf("Booking TH1D %s \n", label);
       }
       
     
  	hCsI1Sum = new TH1D("CsI1Sum", "CsI1Sum", binlimit2, 0, binlimit2);
  	printf("Booking TH1D CsI1Sum \n");

 
  	hCsI2Sum = new TH1D("CsI2Sum", "CsI2Sum", binlimit2, 0, binlimit2);
  	printf("Booking TH1D CsISum2 \n");
       printf(" in Mesytec BOR... Booking CsI histos Done ....\n");
     }
   }

   

//AS Read calibration parameters from  a calibration file.
 Int_t Sd2Chan=-10000;

 if (run<=1100)
   pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib970Sd2r.txt" , "r");
   if (run > 1100 && run <1530)
     pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib1480Sd2r.txt" , "r");
   if ( (run >=1530) && (run<2627))
     pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib1532Sd2r.txt" , "r");

 if (run>=2627)
 {
  pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib2629Sd2r.txt" , "r");
  usePeds =1;
}

   if (pFile == NULL) {
perror ("Error opening file");
fprintf(pwFile,"Error opening file");
   }  
 else  {

  fscanf(pFile,"%s",buffer);

  fscanf(pFile,"%s",buffer);

 fscanf(pFile,"%s",buffer);


for (int i =0;i<24;i++  ){
       fscanf(pFile,"%d%lf%lf",&Sd2Chan,&a,&b);
Sd2rPed[Sd2Chan-64] = a;
Sd2rGain[Sd2Chan-64] = b;
     }
     fclose (pFile);
 }

  // Make an SD2R directory and cd to it.
     TDirectory* Sd2r_dir = gOutputFile->mkdir("Sd2r");      
     Sd2r_dir->cd();

hSd2r[0] = (TH1D*)gDirectory->Get("Sd2r00");
   if (hSd2r[0] == 0) {
     if (gOutputFile) {
       printf(" in Mesytec BOR... Booking Sd2r histos\n");
       for (int channel=0;channel<NSd2rChannels;channel++) {
  	sprintf(label, "Sd2r%02d", channel);
  	sprintf(sig, "Sd2r%03d", channel);
  	hSd2r[channel] = new TH1D(label, sig, 4000, 0, binlimitSd2);
  	printf("Booking TH1D %s \n", label);
       }
       hSd2rSummary = new TH2D("Sd2rSummary", "Sd2rSummary",32,0,32, 4000, 0, binlimitSd2);
       printf(" in Mesytec BOR... Booking Sd2r histos Done ....\n");
     }
   }

//AS Read calibration parameters from  a calibration file for Sd2s.
 Sd2Chan=-10000;

 Sd2Chan=-10000;
 if(run <= 1100)
   pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib970Sd2s.txt" , "r");
   if (run > 1100 && run <1532)
     pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib1480Sd2s.txt" , "r");
  if ( (run >= 1530) && (run<2627)){
     pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib1532Sd2s.txt" , "r");  
  }
   if (run >=2627)
     {
  pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib2629Sd2s.txt" , "r");
  usePeds =1;     
}
   if (pFile == NULL) {
perror ("Error opening file");
fprintf(pwFile,"Error opening file");
   }  
 else  {

  fscanf(pFile,"%s",buffer);

  fscanf(pFile,"%s",buffer);

 fscanf(pFile,"%s",buffer);


for (int i =0;i<32;i++  ){
       fscanf(pFile,"%d%lf%lf",&Sd2Chan,&a,&b);
Sd2sPed[Sd2Chan-96] = a;
Sd2sGain[Sd2Chan-96] = b;
     }
     fclose (pFile);
 }


 hSd2s[0] = (TH1D*)gDirectory->Get("Sd2s00");
   if (hSd2s[0] == 0) {
     if (gOutputFile) {

     // Make an SD2S directory and cd to it.
       TDirectory* Sd2s_dir = gOutputFile->mkdir("Sd2s");      
       Sd2s_dir->cd();
       printf(" in Mesytec BOR... Booking Sd2s histos\n");
       for (int channel=0;channel<NSd2sChannels;channel++) {
  	sprintf(label, "Sd2s%02d", channel);
  	sprintf(sig, "Sd2s%03d", channel);
  	hSd2s[channel] = new TH1D(label, sig, 4000, 0, binlimitSd2);
  	printf("Booking TH1D %s \n", label);
       }
  hSd2sSummary = new TH2D("Sd2sSummary", "Sd2sSummary",32,0,32, 4000, 0, binlimitSd2);
       printf(" in Mesytec BOR... Booking Sd2s histos Done ....\n");
     }
   }

//AS Read calibration parameters from  a calibration file for Sd1r.
 Sd2Chan=-10000;

if (run>=872  && run!=969 && run <1000)  
 pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib970Sd1r.txt" , "r");
 if ((run < 872) || (run == 969))
  pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib969Sd1r.txt" , "r");
 if (run > 1301 && run!=1480 && run <1530)
  pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib1479Sd1r.txt" , "r");
 if ((run<=1301 && run > 1000) || run ==1480)
  pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib1480Sd1r.txt" , "r");

 if ((run >= 1530) && (run < 2667))
  pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib1530Sd1r.txt" , "r");

   if (run >=2627){
  pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib2629Sd1r.txt" , "r");
  usePeds = 1;
}

 if (pFile == NULL) {
perror ("Error opening file");
fprintf(pwFile,"Error opening file");
   }  
 else  {

  fscanf(pFile,"%s",buffer);

  fscanf(pFile,"%s",buffer);

 fscanf(pFile,"%s",buffer);


for (int i =0;i<32;i++  ){
       fscanf(pFile,"%d%lf%lf",&Sd2Chan,&a,&b);
Sd1rPed[Sd2Chan-128] = a;
Sd1rGain[Sd2Chan-128] = b;
if (run>1655 && run < 1770)
   Sd1rGain[Sd2Chan-128] = Sd1rGain[Sd2Chan-128]*13.3;//The gain was changed from #15 to #2 on the shaper 
    }
     fclose (pFile);
 }

 hSd1r[0] = (TH1D*)gDirectory->Get("Sd1r00");
   if (hSd1r[0] == 0) {
     if (gOutputFile) {
     // Make an SD1R directory and cd to it.
     TDirectory* Sd1r_dir = gOutputFile->mkdir("Sd1r");      
     Sd1r_dir->cd();

       printf(" in Mesytec BOR... Booking Sd1r histos\n");
       for (int channel=0;channel<NSd1rChannels;channel++) {
  	sprintf(label, "Sd1r%02d", channel);
  	sprintf(sig, "Sd1r%03d", channel);
  	hSd1r[channel] = new TH1D(label, sig, 4000, 0, binlimitSd1);
  	printf("Booking TH1D %s \n", label);
       }
 hSd1rSummary = new TH2D("Sd1rSummary", "Sd1rSummary",32,0,32, 4000, 0, binlimitSd1);
       printf(" in Mesytec BOR... Booking Sd1r histos Done ....\n");
     }
   }

//AS Read calibration parameters from  a calibration file for Sd1s.
 Sd2Chan=-10000;
 if (run>=872  && run!=969 && run <1000)  
 pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib970Sd1s.txt" , "r");
 if ((run < 872) || (run == 969))
  pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib969Sd1s.txt" , "r");
 if (run > 1301 && run!=1480 && run <1530)
  pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib1479Sd1s.txt" , "r");
 if ((run<=1301 && run > 1000) || run ==1480)
  pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib1480Sd1s.txt" , "r");
 if ((run >= 1530) && (run <2627))
  pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib1530Sd1s.txt" , "r");
 if (run >=2627){
  pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib2629Sd1s.txt" , "r");
}
if (pFile == NULL) {
perror ("Error opening file");
fprintf(pwFile,"Error opening file");
   }  
 else  {

  fscanf(pFile,"%s",buffer);

  fscanf(pFile,"%s",buffer);

 fscanf(pFile,"%s",buffer);


for (int i =0;i<32;i++  ){
       fscanf(pFile,"%d%lf%lf",&Sd2Chan,&a,&b);
Sd1sPed[Sd2Chan-160] = a;
Sd1sGain[Sd2Chan-160] = b;
if (run>1655 && run < 1770){
   Sd1sGain[Sd2Chan-128] = Sd1sGain[Sd2Chan-128]*13.3;//The gain was changed from #15 to #2 on the shaper
 }
      }
     fclose (pFile);
 }

 hSd1s[0] = (TH1D*)gDirectory->Get("Sd1s00");
   if (hSd1s[0] == 0) {
     if (gOutputFile) {
     // Make an SD1S directory and cd to it.
     TDirectory* Sd1s_dir = gOutputFile->mkdir("Sd1s");      
     Sd1s_dir->cd();

       printf(" in Mesytec BOR... Booking Sd1s histos\n");
       for (int channel=0;channel<NSd1sChannels;channel++) {
  	sprintf(label, "Sd1s%02d", channel);
  	sprintf(sig, "Sd1s%03d", channel);
  	hSd1s[channel] = new TH1D(label, sig, 4000, 0, binlimitSd1);
  	printf("Booking TH1D %s \n", label);
       }
 hSd1sSummary = new TH2D("Sd1sSummary", "Sd1sSummary",32,0,32, 4000, 0, binlimitSd1);
       printf(" in Mesytec BOR... Booking Sd1s histos Done ....\n");
     }
   }

//AS Read calibration parameters from the Odb.
for (int channel = 0; channel< NSurChannels; channel++){
sprintf(var,"/Calibration/Sur/Offset");	
SurOffset[channel] = (float)gOdb->odbReadFloat(var,channel);
sprintf(var,"/Calibration/Sur/Gain");	
SurGain[channel] = (float)gOdb->odbReadFloat(var,channel);
 }
 hSur[0] = (TH1D*)gDirectory->Get("Sur00");
   if (hSur[0] == 0) {
     if (gOutputFile) {
     // Make an Sur directory and cd to it.
          TDirectory* Sur_dir = gOutputFile->mkdir("Sur");      
          Sur_dir->cd();

       printf(" in Mesytec BOR... Booking Sur histos\n");
       for (int channel=0;channel<NSurChannels;channel++) {
  	sprintf(label, "Sur%02d", channel);
  	sprintf(sig, "Sur%03d", channel);
  	hSur[channel] = new TH1D(label, sig, binlimit2, 0, binlimit2);
  	printf("Booking TH1D %s \n", label);
       }
       printf(" in Mesytec BOR... Booking Sur histos Done ....\n");
     }
   }

//AS Read calibration parameters from the Odb.
for (int channel = 0; channel< NSusChannels; channel++){
sprintf(var,"/Calibration/Sus/Offset");	
SusOffset[channel] = (float)gOdb->odbReadFloat(var,channel);
sprintf(var,"/Calibration/Sus/Gain");	
SusGain[channel] = (float)gOdb->odbReadFloat(var,channel);
 }
 hSus[0] = (TH1D*)gDirectory->Get("Sus00");
   if (hSus[0] == 0) {
     if (gOutputFile) {
     // Make an SUS directory and cd to it.
         TDirectory* Sus_dir = gOutputFile->mkdir("Sus");      
         Sus_dir->cd();

       printf(" in Mesytec BOR... Booking Sus histos\n");
       for (int channel=0;channel<NSusChannels;channel++) {
  	sprintf(label, "Sus%02d", channel);
  	sprintf(sig, "Sus%03d", channel);
  	hSus[channel] = new TH1D(label, sig, binlimit2, 0, binlimit2);
  	printf("Booking TH1D %s \n", label);
       }
       printf(" in Mesytec BOR... Booking Sus histos Done ....\n");
     }
   }

// //AS Read calibration parameters from the Odb.
// for (int channel = 0; channel< NYdChannels; channel++){
// sprintf(var,"/Calibration/Yd/Offset");	
//  YdOffset[channel] = (float)gOdb->odbReadFloat(var,channel);//25.0
// sprintf(var,"/Calibration/Yd/Gain");	
//  YdGain[channel] = (float)gOdb->odbReadFloat(var,channel); // 2.0;
//  }




//AS Read calibration parameters from  a calibration file for Yd.                                                            
 Sd2Chan=-10000;
 if (run<1530)
   pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib970YY1.txt" , "r");
  if ((run>=1530) && (run <2627))
 pFile = fopen ("/home/iris/Alisher/treeIris/calib-files/alphaCalibYY11530.txt" , "r");
  // pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib1530YY1.txt" , "r");

   if ( (run<2685) && (run >=2878) ){

  pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalibn2633.txt" , "r");
}

 if (run >= 2902  && run <3009){
	pFile = fopen ("/home/iris/anaIris/calib-files/alphaCalib2927YYd.txt","r");
}
if (run >= 3025 ){
	pFile = fopen ("/home/iris/Jaspreet/Calibration/alphaCalib3027.txt","r");
}
	
 if (pFile == NULL) {
   perror ("Error opening file");
   fprintf(pwFile,"Error opening file");
 }
 else  {

   fscanf(pFile,"%s",buffer);

   fscanf(pFile,"%s",buffer);

   fscanf(pFile,"%s",buffer);
   fscanf(pFile,"%s",buffer);

   fscanf(pFile,"%s",buffer);

   for (int i =0;i< NYdChannels;i++  ){
     fscanf(pFile,"%d%lf%lf",&Sd2Chan,&a,&b);
     YdPed[Sd2Chan-192] = a; //change temporary for 86Kr test
     YdGain[Sd2Chan-192] = b;
     YuPed[Sd2Chan-384] = a;
     YuGain[Sd2Chan-384] = b;
   }
   fclose (pFile);
 }



 hYd[0] = (TH1D*)gDirectory->Get("Yd00");
   if (hYd[0] == 0) {
     if (gOutputFile) {
       gOutputFile->cd();

    // Make a YD directory and cd to it.
           TDirectory* Yd_dir = gOutputFile->mkdir("Yd");      
           Yd_dir->cd();       

printf(" in Mesytec BOR... Booking Yd histos\n");
       for (int channel=0;channel<NYdChannels;channel++) {
  	sprintf(label, "Yd%02d", channel);
  	sprintf(sig, "Yd%03d", channel);
  	hYd[channel] = new TH1D(label, sig, 4096, 0, binlimitYd);
  	printf("Booking TH1D %s \n", label);
       }
       hYdSummary = new TH2D("YdSummary","YdSummary",NYdChannels, 0 ,NYdChannels, 2048, 0, binlimitYd);
      

       printf(" in Mesytec BOR... Booking Yd histos Done ....\n");
     }

   }

//AS Read calibration parameters from the Odb.
for (int channel = 0; channel< NYuChannels; channel++){
sprintf(var,"/Calibration/Yu/Offset");	
YuOffset[channel] = (float)gOdb->odbReadFloat(var,channel);
sprintf(var,"/Calibration/Yu/Gain");	
YuGain[channel] = (float)gOdb->odbReadFloat(var,channel);
 }
 hYu[0] = (TH1D*)gDirectory->Get("Yu00");
   if (hYu[0] == 0) {
     if (gOutputFile) {
    // Make a Yu directory and cd to it.
            TDirectory* Yu_dir = gOutputFile->mkdir("Yu");      
            Yu_dir->cd();       

printf(" in Mesytec BOR... Booking Yu histos\n");
       for (int channel=0;channel<NYuChannels;channel++) {
  	sprintf(label, "Yu%02d", channel);
  	sprintf(sig, "Yu%03d", channel);
  	hYu[channel] = new TH1D(label, sig, binlimit1, 0, binlimit1);
  	printf("Booking TH1D %s \n", label);
       }
      hYuSummary = new TH2D("YuSummary", "YuSummary",128,0,128, 4000, 0, binlimitSd2);
       printf(" in Mesytec BOR... Booking Yu histos Done ....\n");
     }

   } 
 


   pFile = fopen ("/home/iris/anaIris/calib-files/SSBCalib.dat" , "r");
   if (pFile == NULL) {
perror ("Error opening file");
fprintf(pwFile,"Error opening file");
   }  
 else  {

  fscanf(pFile,"%s",buffer);

  fscanf(pFile,"%s",buffer);

 fscanf(pFile,"%s",buffer);



       fscanf(pFile,"%d%lf%lf",&CsIChan,&a,&b);
       SSBOffset = a;
       SSBGain = b;
    
     fclose (pFile);
   }


     if (gOutputFile) {
     printf(" in Mesytec BOR... Booking Hits histo\n");
     gOutputFile->cd();
	 sprintf(label, "SSB");
	 sprintf(sig, "SSB");
	 hSSB = new TH1D(label, sig, 1024, 0, 32);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "Sd2rHits");
	 sprintf(sig, "Sd2rHits");
	 hSd2rHits = new TH1D(label, sig, 32, 0, 32);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "Sd2sHits");
	 sprintf(sig, "Sd2sHits");
	 hSd2sHits = new TH1D(label, sig, 32, 0, 32);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "Sd1rHits");
	 sprintf(sig, "Sd1rHits");
	 hSd1rHits = new TH1D(label, sig, 32, 0, 32);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "Sd1sHits");
	 sprintf(sig, "Sd1sHits");
	 hSd1sHits = new TH1D(label, sig, 32, 0, 32);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "SurHits");
	 sprintf(sig, "SurHits");
	 hSurHits = new TH1D(label, sig, 32, 0, 32);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "SusHits");
	 sprintf(sig, "SusHits");
	 hSusHits = new TH1D(label, sig, 32, 0, 32);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "Sd2rElHits");
	 sprintf(sig, "Sd2rElHits");
	 hSd2rElHits = new TH1D(label, sig, 32, 0, 32);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "Sd2sElHits");
	 sprintf(sig, "Sd2sElHits");
	 hSd2sElHits = new TH1D(label, sig, 32, 0, 32);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "Sd1rElHits");
	 sprintf(sig, "Sd1rElHits");
	 hSd1rElHits = new TH1D(label, sig, 32, 0, 32);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "Sd1sElHits");
	 sprintf(sig, "Sd1sElHits");
	 hSd1sElHits = new TH1D(label, sig, 32, 0, 32);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "Yd1Hits");
	 sprintf(sig, "Yd1Hits");
	 hYd1Hits = new TH1D(label, sig, 16, 0, 16);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "Yd2Hits");
	 sprintf(sig, "Yd2Hits");
	 hYd2Hits = new TH1D(label, sig, 16, 0, 16);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "Yd3Hits");
	 sprintf(sig, "Yd3Hits");
	 hYd3Hits = new TH1D(label, sig, 16, 0, 16);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "Yd4Hits");
	 sprintf(sig, "Yd4Hits");
	 hYd4Hits = new TH1D(label, sig, 16, 0, 16);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "Yd5Hits");
	 sprintf(sig, "Yd5Hits");
	 hYd5Hits = new TH1D(label, sig, 16, 0, 16);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "Yd6Hits");
	 sprintf(sig, "Yd6Hits");
	 hYd6Hits = new TH1D(label, sig, 16, 0, 16);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "Yd7Hits");
	 sprintf(sig, "Yd7Hits");
	 hYd7Hits = new TH1D(label, sig, 16, 0, 16);
	 printf("Booking TH1D %s \n", label);
       
	 sprintf(label, "Yd8Hits");
	 sprintf(sig, "Yd8Hits");
	 hYd8Hits = new TH1D(label, sig, 16, 0, 16);
	 printf("Booking TH1D %s \n", label);

	 sprintf(label, "YdHits");
	 sprintf(sig, "YdHits");
	 hYdHits = new TH1D(label, sig, 16, 0, 16);
	 printf("Booking TH1D %s \n", label);
       
	 sprintf(label, "YdHitsProt");
	 sprintf(sig, "YdHitsProt");
	 hYdHitsProt = new TH1D(label, sig, 128, 0, 128);
	 printf("Booking TH1D %s \n", label);
       printf(" in Mesytec BOR... Booking  Hits histos Done ....\n");
     }
   
 
     if (gOutputFile) {
       printf(" in Mesytec BOR... Booking ETot histos\n");
     
	 sprintf(label, "SdETot");
	 sprintf(sig, "SdEtot");
	 hSdETot= new TH1D(label, sig, binlimit1, 0, binlimit1);
	 printf("Booking TH1D %s \n", label);
       
 sprintf(label, "YdCsIETot");
	 sprintf(sig, "YdCsIEtot");
	 hYdCsIETot = new TH1D(label, sig, binlimit1, 0, 50);
	 printf("Booking TH1D %s \n", label);


       printf(" in Mesytec BOR... Booking ETot histos Done ....\n");
     }
   
 if (gOutputFile) {
       printf(" in Mesytec BOR... Booking PID histos\n");
       sprintf(label,"SdPID");
	 hSdPID= new TH2F( "SdPID", "SDPID", 512, 0, 100, 512, 0, 40);
	 printf("Booking TH2F %s \n", label);
       

	 hYdCsIPID2 = new TH2F("YdCsIPID2", "YdCsIPID2", 512, 0, 40, 512, 0, binlimitYd);
	 printf("Booking TH2F %s \n", label);

	 hYdCsIPID1 = new TH2F("YdCsIPID1", "YdCsIPID1", 500, 0, 40, 512, 0, 16.);
         printf("Booking TH2F %s \n", label);


       printf(" in Mesytec BOR... Booking PID histos Done ....\n");

       //AS Angle histos

 printf(" in Mesytec BOR... Booking angle histos\n");
    
  sprintf(label,"SdTheta");
	 hSdTheta= new TH1D( "SdTheta", "SdTheta", 512, 0, 90);
	 printf("Booking TH1D %s \n", label);
       
 sprintf(label,"SdPhi");
	 hSdPhi = new TH1D(label, "SdPhi", 512, 0, 360);
	 printf("Booking TH1D %s \n", label);

 sprintf(label,"SdPhiTheta");
 hSdPhiTheta = new TH2D(label, "SdPhiTheta", 512, 0, 180,512,0,360);
	 printf("Booking TH1D %s \n", label);

sprintf(label,"SdETheta");
 hSdETheta = new TH2D(label, "SdETheta", 500, 0, 10,512,0,120);
	 printf("Booking TH1D %s \n", label);

sprintf(label,"SuTheta");
	 hSuTheta= new TH1D( "SuTheta", "SuTheta", 512, 0, 90);
	 printf("Booking TH1D %s \n", label);
       
 sprintf(label,"SuPhi");
	 hSuPhi = new TH1D(label, "SuPhi", 512, 0, 360);
	 printf("Booking TH1D %s \n", label);

       printf(" in Mesytec BOR... Booking angle histos Done ....\n");

     }

  
}

void HandleEOR_Mesytec(int run, int time)
{
  printf(" in Mesytec EOR\n");
}
