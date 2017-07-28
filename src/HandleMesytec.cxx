
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
#include <TGraph.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>
#include <TFolder.h>
#include <TFile.h>
#include <TMath.h>
#include <TCutG.h>
#include <TRandom3.h>

#include "Globals.h"
#include "HandleMesytec.h"
#include "CalibMesytec.h"
#include "geometry.h"
#include "web_server.h" // = IRIS WebServer =

int gMesytecnitems;

const int Nchannels = 16*32;
const int NICChannels=32;
const int NCsI2Group = 16;
const int NCsI1GroupRing = 16;
const int NCsIChannels=16;
const int NSd1rChannels=32;
const int NSd1sChannels=32;
const int NSd2rChannels=32;
const int NSd2sChannels=32;
const int NSurChannels=32;
const int NSusChannels=32;
const int NYdChannels=4*32;
const int NYuChannels=4*32;

const int adcBins = 4096;
const int csiBins = 8192;
const int energyBins = 4096;
const int channelBins = 4096;
const float scalingIC = 200.;
const float scalingYd = 200.;
const float scalingYu = 200.;
const float scalingCsI = 100.;
const float scalingSd1 = 100.;
const float scalingSd2 = 100.;
const float scalingSu = 100.;
const float scalingSSB = 10.;
const float energyLimitIC = float(energyBins)/scalingIC;
const float energyLimitYd = float(energyBins)/scalingYd;
const float energyLimitYu = float(energyBins)/scalingYu;
const float energyLimitCsI = float(energyBins)/scalingCsI;
const float energyLimitSd1 = float(energyBins)/scalingSd1;
const float energyLimitSd2 = float(energyBins)/scalingSd2;
const float energyLimitSu = float(energyBins)/scalingSu;
const float energyLimitSSB = float(energyBins)/scalingSSB;
TH1D * hMes_P[Nchannels] = {NULL}; // Q

char var[50];

 // = IRIS WebServer =
//#define SIZE_OF_ODB_MSC_TABLE 1024
//#define NSPECS (SIZE_OF_ODB_MSC_TABLE)
//#define NHITS 32
//#define NCHANNELS 128
// Declaration of the spectrum store
int spec_store_address[NSPECS];
int spec_store_type[NSPECS]; // 0=energy, 1=time, 2=waveform, 3=hitpattern
char spec_store_eName[NSPECS][32];
char spec_store_adcName[NSPECS][32];
char spec_store_energyName[NSPECS][32];
char spec_store_tName[NSPECS][32];
char spec_store_2dhitName[NSPECS][32]; // zero-crossing
char spec_store_pidName[NSPECS][32]; // cc_long
char spec_store_Sname[NSPECS][32]; // cc_short
int spec_store_eData[NSPECS][SPEC_LENGTH];
int spec_store_tData[NSPECS][SPEC_LENGTH];
int spec_store_energyData[NSPECS][SPEC_LENGTH];
int spec_store_adcData[NSPECS][SPEC_LENGTH];
int spec_store_2dhitData[NSPECS][NCHANNELS*SPEC_LENGTH_2D];
int spec_store_pidData[NSPECS][SPEC_LENGTH]; // cc_long
int spec_store_Sdata[NSPECS][SPEC_LENGTH]; // cc_short
int spec_store_hit_type[NHITS];
char spec_store_hitName[NHITS][32];
int spec_store_hitData[NHITS][NCHANNELS];
char spec_store_sum_name[NHITS][32];
int spec_store_sum_data[NHITS][SPEC_LENGTH];
 // =========================

CalibMesytec calMesy;
geometry geoM;

TCutG *protons = NULL;//proton cut
TCutG *elasticS3 = NULL;//elastic gate on S3
int usePeds = 0; //using pedestals instead of offsets for Silicon detectors AS
//AS Ion Chamber
TH1D * hIC[NICChannels] = {NULL}; // Q 
float IC[16], ICEnergy=0; //Dummy for IC energy
float ICGain[NICChannels]={1.};
float ICOffset[NICChannels]={0.};
float ICPed[NICChannels]={0.};
float ICnadc = 0;  

//AS CsI
TH1D * hCsI1[NCsIChannels] = {NULL}; // Q 
TH1D * hCsI1Sum = {NULL}; 
TH1D * hCsI2[NCsIChannels] = {NULL}; // Q 
TH1D * hCsI2Sum = {NULL};
float CsI[32]={0}, CsIEnergy, CsIEnergy2; //CsI energy 
float CsI1[32]={0}, CsI2[32]={0}, CsI1Energy, CsI1Energy2, CsI2Energy, CsI2Energy2; //CsI energy
int CsI1ADC[32] = {0}, CsI2ADC[32] ={0};
float CsI1nadc = 0;
float CsI2nadc = 0;
int CsIChannel;
int CsIChannel2; // channel with the greatest value
int CsI1Channel;
int CsI1Channel2; // channel with the greatest value
int CsI2Channel;
int CsI2Channel2; // channel with the greatest value
//double CsI1Gain[NCsIChannels]={1.};
float CsI1Gain[NCsI1GroupRing][NCsIChannels]={{0.}};
float CsI1Ped[NCsIChannels]={0.};
//double CsI2Gain[NCsIChannels]={1.};
float CsI2Gain[NCsI2Group][NCsIChannels]={{0.}};
float CsI2Ped[NCsIChannels]={0.};

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
float Sd1sGain[NSd1sChannels]={0.};
float Sd1sOffset[NSd1sChannels]={0.};
float Sd1sPed[NSd1sChannels]={0.};
TH2D *hSd1sSummary = {NULL}; //summary spectra

TH1D * hSur[NSurChannels] = {NULL}; //  Upstream S3 rings 
float Sur[NSurChannels];
float SurEnergy=0, SurEnergy2=0; //Dummy for Sur energy
int SurChannel, SurChannel2; // channel with the greatest value
float SurGain[NSurChannels]={1.};
float SurOffset[NSurChannels]={0.};
float SurPed[NSurChannels]={0.};

TH1D * hSus[NSusChannels] = {NULL}; // Upstream S3 sectors   
float Sus[NSusChannels];
int SusChannel, SusChannel2; // channel with the greatest value
float SusEnergy=0, SusEnergy2=0; //Dummy for Sus energy                                                                                          
float SusGain[NSusChannels]={1.};
float SusOffset[NSusChannels]={0.};
float SusPed[NSusChannels]={0.};


//AS YYd
TH1D * hYd[NYdChannels] = {NULL}; // Q
float Yd[NYdChannels] ={0}; 
float YdEnergy=0, YdEnergy2=0; //Dummy for Yd energy
int YdADC[32] = {0};
int YdChannel, YdChannel2, YdNo, YdNo2; // channel with the greatest value
int YdTChannel, YdTChannel2;
float Ydnadc = 0.;
int TYdChannel;
float YdGain[NYdChannels]={1.};
float YdOffset[NYdChannels]={0.};
float YdPed[NYdChannels]={0.};
TH2D *hYdSummary = {NULL};


//SSB
TH1D * hSSB = NULL;
float SSBEnergy = 0;
float SSBOffset=0;
float SSBGain=0;

float ScintEnergy = 0;

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

TH1D * hCsI1Hits = {NULL};  // CsI1
TH1D * hCsI2Hits = {NULL};  // CsI2

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
extern FILE* ASCIIIC;
extern FILE* ASCIICsI1;
extern FILE* ASCIICsI2;
extern FILE* ASCIICsI;
extern FILE* ASCIISd1;
extern FILE* ASCIISd2;

int ascii =0; // bool for writing ascii AS

uint32_t adcThresh = 0;
int ydnumber, ydnumber2;
//AS Total energies

TH1D *hSdETot = {NULL};
TH1D *hYdCsIETot = {NULL};
//AS DE-E spectra
TH2F *hSdPID = {NULL};
TH2F *hYdCsIPID2 = {NULL};
TH2F *hYdCsIPID1 = {NULL};
TH2F *hYdCsI1adcPID = {NULL};
TH2F *hYdCsI2adcPID = {NULL};
TH2F *hYdadcCsI1adcPID = {NULL};
TH2F *hYdadcCsI2adcPID = {NULL};
TH2F *Ydrange = {NULL};
TH2F *CsI1range = {NULL};
TH2F *CsI2range = {NULL};
TH2F *hYdCsI1Corr = {NULL};
TH2F *hYdCsI2Corr = {NULL};
//TH2F *hYdCsI2PID_forNa = {NULL};
//TH2F *hYdCsI2PID_forMg = {NULL};
//TH2F *hYdCsI1PID_uncalibrated = {NULL};
//TH2F *hYdCsI2PID_uncalibrated = {NULL};
//AS Angles
TH1D *hSdTheta = {NULL};
TH1D *hSdPhi = {NULL};
TH2D *hSdETheta = {NULL};
TH2D *hSdPhiTheta = {NULL};
TH2D *hSuETheta = {NULL};
TH2D *hSuPhiTheta = {NULL};
TH2D *hYuETheta = {NULL};
TH2D *hYdETheta = {NULL};
TH1D *hYdTheta = {NULL};
TH1D *hYuTheta = {NULL};
TH1D *hSuTheta = {NULL};
TH1D *hSuPhi = {NULL};

// float YdDistance = 0.; // distance from target in mm
// float YuDistance = 83.; // distance from target in mm
// float YdInnerRadius= 0., YdOuterRadius = 0. ; // inner and outer radii in mm
// float Sd1Distance = 0., Sd2Distance = 0.; //distance from target in mm
 float SuDistance = 200.; //distance from target in mm
// float SdInnerRadius = 0., SdOuterRadius= 0.; //AS Inner and outer radii of an S3 detector (in mm).

Double_t xShift = 0;//1.97;
Double_t yShift = 0;//1.3;

float YdTheta[128] = {0.}, SdTheta=0., theta = 0 ,phi = 0, theta1 = 0, phi1=0; //AS Dummies for theta and phi, to be used for filling histograms
float length = 0; //The length from target to each Yd strips Junki
float Sdtheta = 0 ,Sdphi = 0, Sdtheta1 = 0, Sdphi1=0; //AS Dummies for theta and phi, to be used for filling histograms

TRandom3 fRandom(0);
Double_t randm; //random number between 0 and 1 for each event


uint32_t modid, oformat, vpeak, resolution, evlength, timestamp;
uint32_t channel, overflow;
void HandleBOR_Mesytec(int run, int time, det_t *pdet);
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


 for (int j=0; j<NSurChannels; j++)
  Sur[j] = 0;

 for (int j=0; j<NSusChannels; j++)
  Sus[j] = 0;

 for (int j=0; j<NYdChannels; j++)
  Yd[j] = 0;

 for (int j=0; j<NYuChannels; j++)
  Yu[j] = 0;

     theta = 0; 
     length = 0;
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
	if (hMes_P[0] == NULL) HandleBOR_Mesytec(gRunNumber, 0, det);    
	
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
	
	 				if (debug) 
	  					printf("Header: id:%d of:%d res:%d el:%d\n" , modid, oformat, resolution, evlength);
	
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
	 
	  				if (debug1  && modid==1) printf("Evt#:%d items:%d - data[%d]: %d / 0x%x\n", event.GetSerialNumber(),Nchannels, i, data[i], data[i]); 
	
	  				if (debug1  && modid==1) printf("Data: ch:%d id:%d val:%f\n", channel, modid, (float) vpeak);
	  				// RK :  Energy Calibration 
	  				
	  				hMes_P[channel+(modid*32)]->Fill((float)vpeak , 1.);
					spec_store_adcData[channel+(modid*32)][vpeak]++;
	  				if ((modid==0) && (vpeak > adcThresh) && (vpeak<3840)){ // Why 3840? MH
	    				//printf("IC1\n");
						//AS Fill histogram
 						ICnadc = (float)vpeak; 
						IC[channel] = (float)vpeak;
						ICEnergy = ((float)vpeak-ICPed[channel])*ICGain[channel];
	    				//printf("IC2: %d %d\n", channel,vpeak);
	    				//hIC[channel]->Fill(ICEnergy, 1.); //IC
	    				//printf("IC3\n");
						spec_store_eData[15][int(ICEnergy)]++; // = IRIS WebServer =
	    				//printf("IC4\n");
						//spec_store_eData[15][int(ICEnergy*scalingIC)]++; // = IRIS WebServer =
						if (channel==19){
	          				ScintEnergy = float(vpeak);// *SSBGain + SSBOffset;
							spec_store_energyData[9][int(ScintEnergy)]++; // = IRIS WebServer =
						}
	    				//printf("IC5\n");

	        			if (channel==31){
	          				SSBEnergy = float(vpeak);// *SSBGain + SSBOffset;
							spec_store_energyData[8][int(SSBEnergy)]++; // = IRIS WebServer =
						}
	    				//printf("IC6\n");
	  				}
	  
	  				if (modid==1 && vpeak > adcThresh && vpeak<3840){
	    				if (channel<16){
	    					CsI1[channel] = (float)vpeak;
							hCsI1[channel]->Fill(CsI1[channel],1.);
							spec_store_eData[channel+32][int(CsI1[channel])]++; // = IRIS WebServer =
	    	    		}	    
		  				else if (channel>=16){
	    					CsI2[channel-16] = (float)vpeak;
							hCsI2[channel-16]->Fill(CsI2[channel-16],1.);
							spec_store_eData[channel+32][int(CsI2[channel-16])]++; // = IRIS WebServer =
	    	    		}	    
	  				}
	
	  				if (modid==2 && vpeak > adcThresh && vpeak<3840){
	 					S3Hit = 1; 	    
	 					if (!usePeds){
	    					Sd2r[channel] = Sd2rOffset[channel]+Sd2rGain[channel]*(float)vpeak;
						}
	 					else if (usePeds){
	   						Sd2r[channel] = Sd2rGain[channel]*(((float)vpeak)-Sd2rPed[channel]);
							} 
						hSd2r[channel]->Fill(Sd2r[channel],1.);
						spec_store_eData[channel+64][int(Sd2r[channel]*scalingSd2)]++; // = IRIS WebServer =
	  				}
	  
	  				if (modid==3 && vpeak > adcThresh && vpeak<3840){
	    				S3Hit = 1;
	    				if (!usePeds){	
	    					Sd2s[channel] = Sd2sOffset[channel]+Sd2sGain[channel]*(float)vpeak;
						}
	 					else if (usePeds){
	   						Sd2s[channel] = Sd2sGain[channel]*(((float)vpeak)-Sd2sPed[channel]);
							}
						hSd2s[channel]->Fill(Sd2s[channel],1.);
						spec_store_eData[channel+96][int(Sd2s[channel]*scalingSd2)]++; // = IRIS WebServer =
	   				}
	
	  				if (modid==4 && vpeak > adcThresh  && vpeak<3840){
	 					S3Hit = 1; 
						if (!usePeds){
	    					Sd1r[channel] = Sd1rOffset[channel]+Sd1rGain[channel]*(float)vpeak;
						}
						else if (usePeds){
	   						Sd1r[channel] = Sd1rGain[channel]*(((float)vpeak)-Sd1rPed[channel]);
							}
						hSd1r[channel]->Fill(Sd1r[channel],1.);
						spec_store_eData[channel+128][int(Sd1r[channel]*scalingSd1)]++; // = IRIS WebServer =
					}	
	  				if (modid==5 && vpeak > adcThresh  && vpeak<3840){
	     				S3Hit = 1;
	     				if (!usePeds){
	    					Sd1s[channel] = Sd1sOffset[channel]+Sd1sGain[channel]*(float)vpeak;
						}
						else if (usePeds){
	   						Sd1s[channel] = Sd1sGain[channel]*(((float)vpeak)-Sd1sPed[channel]);
						}
						hSd1s[channel]->Fill(Sd1s[channel],1.);
						spec_store_eData[channel+160][int(Sd1s[channel]*scalingSd1)]++; // = IRIS WebServer =
					}
	
					// Upstream S3 detector. Has to be properly implemented! 
	  				if (modid==10  && vpeak> adcThresh && vpeak<3840){
	   					if (!usePeds){
	    					Sur[channel] = SurOffset[channel]+SurGain[channel]*(float)vpeak;
						}
						else if (usePeds){
	   						Sur[channel] = SurGain[channel]*(((float)vpeak)-SurPed[channel]);
						}
						hSur[channel]->Fill(Sur[channel],1.);
						spec_store_eData[channel+320][int(Sur[channel]*scalingSu)]++; // = IRIS WebServer =
	  				}
	  				    
	  				if (modid==11  && vpeak > adcThresh && vpeak<3840){
	  				  	if (!usePeds){
	    					Sus[channel] = SusOffset[channel]+SusGain[channel]*(float)vpeak;
						}
						else if (usePeds){
	   						Sus[channel] = SusGain[channel]*(((float)vpeak)-SusPed[channel]);
						}
						hSus[channel]->Fill(Sus[channel],1.);
						spec_store_eData[channel+352][int(Sus[channel]*scalingSu)]++; // = IRIS WebServer =
	  				}
	  
	  				if (modid>5 && modid<10 && vpeak>adcThresh  && vpeak<3840){
	 					Yd[channel+(modid-6)*32]=YdOffset[channel+(modid-6)*32]+YdGain[channel+(modid-6)*32]*(float)vpeak;
						YdADC[channel+(modid-6)*32]=vpeak;
	    				if (channel<16) ydnumber = (modid-6)*2+1; //Yd number
	    				if (channel>15) ydnumber = (modid-6)*2+2;
						hYd[channel]->Fill(Yd[channel+(modid-6)*32],1.);
						spec_store_eData[channel+(modid-6)*32+192][int(Yd[channel+(modid-6)*32]*scalingYd)]++; // = IRIS WebServer =
	  				} //modid
	  
	 
	 				// Upstream YY detector. Has to be properly implemented! 
	  				if (modid>11 && modid<16 && vpeak >adcThresh  && vpeak<3840){  
	 					Yu[channel+(modid-12)*32]=YuOffset[channel+(modid-12)*32]+YuGain[channel+(modid-12)*32]*(float)vpeak;
						hYu[channel]->Fill(Yu[channel+(modid-12)*32],1.);
						spec_store_eData[channel+(modid-12)*32+384][int(Yu[channel+(modid-12)*32]*scalingYu)]++; // = IRIS WebServer =
	  				}
	
	  				//AS angles // Is this used anywhere??
	  				if (modid==2){
	  				  	// theta = TMath::RadToDeg()*(SdInnerRadius*(24.-channel-fRandom.Rndm())+SdOuterRadius*(channel-fRandom.Rndm()))/24./Sd1Distance; //AS theta angle for Sd (24 - number of rings)
	  				}
	  				
	  				if (modid==3){
	  				  	phi = ((channel+fRandom.Rndm())*180./32.); //AS phi angle for Sd (32 - number of sectors)
	  				}
	  				//  if (modid>5 && modid<10){
	  				  //theta = TMath::RadToDeg()*(YdInnerRadius*(16.-channel-0.5)+YdOuterRadius*(channel+0.5))/16./YdDistance;
	  				  //det.TYdTheta= theta;
	  				 // cout <<  " det.TYdTheta= " <<  det.TYdTheta << endl;
	  				//}
	
	  				if (modid==10){
	  				  	theta1 = TMath::RadToDeg()*(geoM.SdInnerRadius*(24.-channel-0.5)+geoM.SdOuterRadius*(channel+0.5))/24./SuDistance; //AS theta angle for Su (24 - number of rings)
	  				}
	  				
	  				if (modid==11){
	  				  	phi1 = (channel*180./32.); //AS phi angle for Su (32 - number of sectors)
	  				}
	
	    			//hits 
	    			if (modid==1 && vpeak>adcThresh&&channel<16){	 
	    				hCsI1Hits->Fill(channel, 1.); // CsI hit  
						spec_store_hitData[0][channel]++;	
					}
	    			if (modid==1 && vpeak>adcThresh&&channel>15){	 
	    				hCsI2Hits->Fill(channel-16, 1.); //CsI hits  
						spec_store_hitData[1][channel-16]++;	
					}

	    			if (modid==2 && vpeak>adcThresh){	 
	    				hSd2rHits->Fill(channel, 1.); //Sd2r hits  
						spec_store_hitData[4][channel]++;	
					}
	    			if (modid==3 && vpeak>adcThresh)	 {
	    				hSd2sHits->Fill(channel, 1.); //Sd2s hits
		   				spec_store_hitData[5][channel]++;
	  				}

 					if (modid==4 && vpeak>adcThresh)	 {
	    				hSd1rHits->Fill(channel, 1.); //Sd1r hits
		   				spec_store_hitData[2][channel]++;
					}	  
 					if (modid==5 && vpeak>adcThresh)	{	     
	    				hSd1sHits->Fill(channel, 1.); //Sd1s hits 
		   				spec_store_hitData[3][channel]++;
 					}

	  				if (modid==10 && vpeak>adcThresh){	 
	    				hSurHits->Fill(channel, 1.); //Sur hits 
		   				spec_store_hitData[15][channel]++;
	 				}
	  				if (modid==11 && vpeak>adcThresh){	 
	    				hSusHits->Fill(channel, 1.); //Sus hits
		   				spec_store_hitData[16][channel]++;
					}

					if (modid==6 && vpeak>adcThresh && channel<16){
					  	hYd1Hits->Fill(channel,1.);
						spec_store_hitData[6][channel]++;	
					}
					if (modid==6 && vpeak>adcThresh  && channel>15){
					  	hYd2Hits->Fill(channel-16,1.);
						spec_store_hitData[7][channel-16]++;	
					}
					if (modid==7 && vpeak>adcThresh && channel<16){
					  	hYd3Hits->Fill(channel,1.);
						spec_store_hitData[8][channel]++;	
					}
					if (modid==7 && vpeak>adcThresh && channel>15){
					  	hYd4Hits->Fill(channel-16,1.);
						spec_store_hitData[9][channel-16]++;	
					}
					if (modid==8 && vpeak>adcThresh && channel<16){
					  	hYd5Hits->Fill(channel,1.);
						spec_store_hitData[10][channel]++;	
					}
					if (modid==8 && vpeak>adcThresh && channel>15){
					  	hYd6Hits->Fill(channel-16,1.);
						spec_store_hitData[11][channel-16]++;	
					}
					if (modid==9 && vpeak>adcThresh && channel<16){
					  	hYd7Hits->Fill(channel,1.);
						spec_store_hitData[12][channel]++;	
					}
					if (modid==9 && vpeak>adcThresh && channel>15){
					  	hYd8Hits->Fill(channel-16,1.);
						spec_store_hitData[13][channel-16]++;	
					}
					if (modid>5 && modid<10 && vpeak>adcThresh && channel<16){
					  	hYdHits->Fill(channel,1.);
						spec_store_hitData[14][channel]++;	
					}
					if (modid>5 && modid<10 && vpeak>adcThresh && channel>15){
  						hYdHits->Fill(channel-16,1.);
						spec_store_hitData[14][channel-16]++;	
					}
					
					if (modid==12 && vpeak>adcThresh && channel<16){
					  	hYu1Hits->Fill(channel,1.);
						spec_store_hitData[17][channel]++;	
					}
					if (modid==12 && vpeak>adcThresh  && channel>15){
					  	hYu2Hits->Fill(channel-16,1.);
						spec_store_hitData[18][channel-16]++;	
					}
					if (modid==13 && vpeak>adcThresh && channel<16){
					  	hYu3Hits->Fill(channel,1.);
						spec_store_hitData[19][channel]++;	
					}
					if (modid==13 && vpeak>adcThresh && channel>15){
					  	hYu4Hits->Fill(channel-16,1.);
						spec_store_hitData[20][channel-16]++;	
					}
					if (modid==14 && vpeak>adcThresh && channel<16){
					  	hYu5Hits->Fill(channel,1.);
						spec_store_hitData[21][channel]++;	
					}
					if (modid==14 && vpeak>adcThresh && channel>15){
					  	hYu6Hits->Fill(channel-16,1.);
						spec_store_hitData[22][channel-16]++;	
					}
					if (modid==15 && vpeak>adcThresh && channel<16){
					  	hYu7Hits->Fill(channel,1.);
						spec_store_hitData[23][channel]++;	
					}
					if (modid==15 && vpeak>adcThresh && channel>15){
					  	hYu8Hits->Fill(channel-16,1.);
						spec_store_hitData[24][channel-16]++;	
					}

					if (modid>11 && modid<16 && vpeak>adcThresh && channel<16){
					  	hYuHits->Fill(channel,1.);
						spec_store_hitData[25][channel]++;	
					}
					if (modid>11 && modid<16 && vpeak>adcThresh && channel>15){
  						hYuHits->Fill(channel-16,1.);
						spec_store_hitData[25][channel-16]++;	
					}


	  				break;
			} // switch
	  	} // for loop
	} // nitems != 0

	//After looping over banks, fill the ascii files
    if (modid>5 && modid<10) {// check last bank
      	det->EventNumber = event.GetSerialNumber();
    
 		Sd2rEnergy=0; Sd2rEnergy2 =0; Sd2rChannel = -1; Sd2rChannel2 =-1;
 		for (int i =0; i< NSd2rChannels;i++){
      		if (Sd2rEnergy<Sd2r[i]){
          		Sd2rEnergy2 = Sd2rEnergy;
	  			Sd2rChannel2 = Sd2rChannel;
          		Sd2rEnergy=Sd2r[i];
          		Sd2rChannel = i;}
			else if (Sd2rEnergy2<Sd2r[i]){
		   		Sd2rEnergy2=Sd2r[i];
				Sd2rChannel2 = i;
			}
 		} //for

     	Sd2sEnergy=0; Sd2sEnergy2 =0; Sd2sChannel = -1; Sd2sChannel2 =-1;
    	for (int i =0; i< NSd2sChannels;i++){
			if (Sd2sEnergy<Sd2s[i]){
	  			Sd2sEnergy2 = Sd2sEnergy;
	  			Sd2sChannel2 = Sd2sChannel;
          		Sd2sEnergy=Sd2s[i];
          		Sd2sChannel = i;}
				else if (Sd2sEnergy2<Sd2s[i]){
					Sd2sEnergy2=Sd2s[i];
					Sd2sChannel2 = i;
				}
      	} //for

  		if (ascii) 
    		fprintf(ASCIISd2," %d  %d %d %d %d %d %d %d %d \n",event.GetSerialNumber(), Sd2rChannel+64, (int)Sd2rEnergy,  Sd2rChannel2+64, (int)Sd2rEnergy2,  Sd2sChannel+96, (int)Sd2sEnergy,  Sd2sChannel2+96, (int)Sd2sEnergy2);
  
	    spec_store_energyData[6][int(Sd2rEnergy*scalingSd2)]++; // = IRIS WebServer =
	    spec_store_energyData[7][int(Sd2sEnergy*scalingSd2)]++; // = IRIS WebServer =
  		//if (Sd2rEnergy >0)
  		// Sd2rEnergy = (Sd2rEnergy-Sd2rOffset[Sd2rChannel])*Sd2rGain[Sd2rChannel];
		//if (Sd2sEnergy >0)  
  		//Sd2sEnergy = (Sd2sEnergy-Sd2sOffset[Sd2sChannel])*Sd2sGain[Sd2sChannel];

 		Sd1rEnergy=0; Sd1rEnergy2 =0; Sd1rChannel = -1; Sd1rChannel2 =-1;
    	for (int i =0; i< NSd1rChannels;i++){
			if (Sd1rEnergy<Sd1r[i]){
          		Sd1rEnergy2 = Sd1rEnergy;
	  			Sd1rChannel2 = Sd1rChannel;
          		Sd1rEnergy=Sd1r[i];
          		Sd1rChannel = i;}

			else if (Sd1rEnergy2<Sd1r[i]){
		   		Sd1rEnergy2=Sd1r[i];
				Sd1rChannel2 = i;
			}
      	}//for
		theta = TMath::RadToDeg()*atan((geoM.SdInnerRadius*(24.-Sd1rChannel-0.5)+geoM.SdOuterRadius*(Sd1rChannel+0.5))/24./geoM.Sd1Distance);
		det->TSdTheta= theta;
		hSdTheta -> Fill(theta);
	   	hSdETheta -> Fill(theta,Sd1rEnergy);

 		Sd1sEnergy=0; Sd1sEnergy2 =0; Sd1sChannel = -1; Sd1sChannel2 =-1;
    	for (int i =0; i< NSd1sChannels;i++){
			if (Sd1sEnergy<Sd1s[i]){
          		Sd1sEnergy2 = Sd1sEnergy;
	  			Sd1sChannel2 = Sd1sChannel;
          		Sd1sEnergy=Sd1s[i];
          		Sd1sChannel = i;}
			else if (Sd1sEnergy2<Sd1s[i]){
		   		Sd1sEnergy2=Sd1s[i];
				Sd1sChannel2 = i;
			}
      	} //for

 		SurEnergy=0; SurEnergy2 =0; SurChannel = -1; SurChannel2 =-1;
    	for (int i =0; i< NSurChannels;i++){
			if (SurEnergy<Sur[i]){
          		SurEnergy2 = SurEnergy;
	  			SurChannel2 = SurChannel;
          		SurEnergy=Sur[i];
          		SurChannel = i;}

			else if (SurEnergy2<Sur[i]){
		   		SurEnergy2=Sur[i];
				SurChannel2 = i;
			}
      	}//for
		theta = TMath::RadToDeg()*atan((geoM.SdInnerRadius*(24.-SurChannel-0.5)+geoM.SdOuterRadius*(SurChannel+0.5))/24./geoM.SuDistance);
		det->TSuTheta= theta;
		hSuTheta -> Fill(theta);
	   	hSuETheta -> Fill(theta,SurEnergy);

 		SusEnergy=0; SusEnergy2 =0; SusChannel = -1; SusChannel2 =-1;
    	for (int i =0; i< NSusChannels;i++){
			if (SusEnergy<Sus[i]){
          		SusEnergy2 = SusEnergy;
	  			SusChannel2 = SusChannel;
          		SusEnergy=Sus[i];
          		SusChannel = i;}
			else if (SusEnergy2<Sus[i]){
		   		SusEnergy2=Sus[i];
				SusChannel2 = i;
			}
      	} //for

  		if (ascii) 
   			fprintf(ASCIISd1," %d  %d %d %d %d %d %d %d %d \n",event.GetSerialNumber(), Sd1rChannel+64, (int)Sd1rEnergy,  Sd1rChannel2+64, (int)Sd1rEnergy2,  Sd1sChannel+96, (int)Sd1sEnergy,  Sd1sChannel2+96, (int)Sd1sEnergy2);
 
	    spec_store_energyData[4][int(Sd1rEnergy*scalingSd1)]++; // = IRIS WebServer =
	    spec_store_energyData[5][int(Sd1sEnergy*scalingSd1)]++; // = IRIS WebServer =
			  
		det->TSd2rEnergy = Sd2rEnergy;
		det->TSd2sEnergy = Sd2sEnergy;
		det->TSd1rEnergy = Sd1rEnergy;
		det->TSd1sEnergy = Sd1sEnergy;
		det->TSurEnergy = SurEnergy;
		det->TSusEnergy = SusEnergy;
		det->TSd2rChannel = Sd2rChannel;
		det->TSd2sChannel = Sd2sChannel;
		det->TSd1rChannel = Sd1rChannel;
		det->TSd1sChannel = Sd1sChannel;
		det->TSurChannel = SurChannel;
		det->TSusChannel = SusChannel;

    	YdEnergy=0; YdChannel = -1; YdEnergy2=0; YdChannel2 =-1, YdTChannel = -1, YdTChannel2 = -1;
		ydnumber = YdChannel/16; //Yd number                                                                               
    	for (int i =0; i< NYdChannels;i++){
	  		if(Yd[i]>YdEnergy){
           		YdEnergy2= YdEnergy;
	   			YdChannel2 = YdChannel;
	   			YdNo2 = YdNo;
	   			YdTChannel2 = YdTChannel;
	   			YdEnergy=Yd[i];
	   			Ydnadc=YdADC[i];
	   			YdChannel = i;
	   			YdNo = i/16;
	   			YdTChannel = i - 128; //YdTChannel is the TDC channels which have max ADC 
	  		}
 			else if (Yd[i]>YdEnergy2){//if Yd[i] is between YdEnergy and YdEnergy2
	    		YdEnergy2=Yd[i];
	  			YdChannel2 = i;
	  			YdNo2 = i/16;
	  			YdTChannel2 = i-128;
	    	} //if
      	} //for      
    
  		if (ascii) 
    		fprintf(ASCIIYY1," %d  %d %d %d %d \n",event.GetSerialNumber(), YdChannel+192, (int)YdEnergy,  YdChannel2+192, (int)YdEnergy2);
  
	    spec_store_energyData[1][int(YdEnergy*scalingYd)]++; // = IRIS WebServer =
		det->TYdEnergy = YdEnergy;
		if(YdChannel>-1){
			det->TYdChannel = YdChannel;
		}
  		
		theta = TMath::RadToDeg()*atan((geoM.YdInnerRadius*(16.-YdChannel%16-0.5)+geoM.YdOuterRadius*(YdChannel%16+0.5))/16./geoM.YdDistance);
		det->TYdTheta= theta;
		length = geoM.YdDistance/cos(atan((geoM.YdInnerRadius*(16.-YdChannel%16-0.5)+geoM.YdOuterRadius*(YdChannel%16+0.5))/16./geoM.YdDistance));
		hYdTheta -> Fill(theta);
	   	hYdETheta -> Fill(theta,YdEnergy);
	 
		//YYu
    	YuEnergy=0; YuChannel = -1; YuEnergy2=0; YuChannel2 =-1;
    	for (int i =0; i< NYuChannels;i++){
          	if(Yu[i]>YuEnergy){
  				YuEnergy2= YuEnergy;
           		YuChannel2 = YuChannel;
          		YuEnergy=Yu[i];
          		YuChannel = i;
			}
 			else if (Yu[i]>YuEnergy2){//if Yu[i] is between YuEnergy and YuEnergy2
            	YuEnergy2=Yu[i];
          		YuChannel2 = i;
			} //if
      	} //for      

  		if (ascii)
    		fprintf(ASCIIYY1," %d  %d %d %d %d \n",event.GetSerialNumber(), YuChannel+384, (int)YuEnergy,  YuChannel2+384, (int)YuEnergy2);

		det->TYuEnergy = YuEnergy;
		det->TYuChannel = YuChannel;
		theta = TMath::RadToDeg()*atan((geoM.YdInnerRadius*(16.-YuChannel%16-0.5)+geoM.YdOuterRadius*(YuChannel%16+0.5))/16./geoM.YuDistance);
		det->TYuTheta= theta;
		hYuTheta -> Fill(theta);
	   	hYuETheta -> Fill(theta,YuEnergy);
  		
		CsIEnergy=0; CsIEnergy2 =0; CsIChannel = -100; CsIChannel2 =-100;
    	for (int i =0; i< NCsIChannels;i++) {
      		// printf("CsI ch: %d, value %f\n", i, CsI[i]);
      		if (CsIEnergy<CsI[i]){
	  			CsIEnergy2 = CsIEnergy;
	  			CsIChannel2 = CsIChannel;
          		CsIEnergy=CsI[i];
          		//CsI1nadc=CsIADC[i];
          		CsIChannel = i;
			}
      		else  if (CsIEnergy2<CsI[i]){
          		CsIEnergy2=CsI[i];
          		CsIChannel2 = i;
			}
    	} //for
  
  		//CsI
  
    	CsI1Energy=0; CsI1Energy2 =0;
		CsI1Channel = -100; CsI1Channel2 =-100;
		if(YdChannel>=0){
    		for (int i =0; i< 16;i++) {
      		// printf("CsI ch: %d, value %f\n", i, CsI[i]);
			   	CsI1ADC[i] = CsI1[i];
			   	CsI1[i] = (CsI1[i]-CsI1Ped[i])*CsI1Gain[YdChannel%16][i];
      			if (CsI1Energy<CsI1[i]){
					CsI1Energy2 = CsI1Energy;
					CsI1Channel2 = CsI1Channel;
					CsI1Energy=CsI1[i];
					CsI1nadc=CsI1ADC[i];
					CsI1Channel = i;
				}
      			else  if (CsI1Energy2<CsI1[i]){
					CsI1Energy2=CsI1[i];
					CsI1Channel2 = i;
				}
			}//for
    	} //Yd

		CsI2Energy=0; CsI2Energy2 =0; CsI2Channel = -100; CsI2Channel2 =-100;
		if(YdChannel>=0){
			for (int i =0; i< 16;i++) {
			// printf("CsI ch: %d, value %f\n", i, CsI[i]);                                                              
			   	CsI2ADC[i] = CsI2[i];
			   	CsI2[i] = (CsI2[i]-CsI2Ped[i])*CsI2Gain[YdChannel%16][i];
				if (CsI2Energy<CsI2[i]){
					CsI2Energy2 = CsI2Energy;
					CsI2Channel2 = CsI2Channel;
					CsI2Energy=CsI2[i];
					CsI2nadc=CsI2ADC[i];
					CsI2Channel = i;}
		  		else  if (CsI2Energy2<CsI2[i]){
					CsI2Energy2=CsI2[i];
					CsI2Channel2 = i;
				}
			}//for
		} // if 
	    
		spec_store_energyData[2][int(CsI1Energy*scalingCsI)]++; // = IRIS WebServer =
	    spec_store_energyData[3][int(CsI2Energy*scalingCsI)]++; // = IRIS WebServer =
    	
		if (ascii)  fprintf(ASCIICsI," %d  %d %d %d %d \n",event.GetSerialNumber(), CsIChannel+32, (int)CsIEnergy,  CsIChannel2+32, (int)CsIEnergy2);

		if(YdChannel>=0&&CsI1Channel>=0&&(CsI1Channel==2*YdNo||CsI1Channel==2*YdNo+1)) hYdCsI1adcPID->Fill(CsI1nadc,YdEnergy*cos(det->TYdTheta*0.01745329));
		if(YdChannel>=0&&CsI1Channel>=0&&(CsI1Channel==2*YdNo||CsI1Channel==2*YdNo+1)) hYdadcCsI1adcPID->Fill(CsI1nadc,Ydnadc);
		
		if(YdChannel>=0&&CsI1Channel>=0&&(CsI1Channel==2*YdNo||CsI1Channel==2*YdNo+1)) hYdCsIPID1->Fill(CsI1Energy,YdEnergy*cos(det->TYdTheta*0.01745329));
		if(YdChannel>=0&&CsI1Channel2>=0&&(CsI1Channel2==2*YdNo||CsI1Channel2==2*YdNo+1)) hYdCsIPID1->Fill(CsI1Energy2,YdEnergy*cos(det->TYdTheta*0.01745329));
		if(YdChannel2>=0&&CsI1Channel>=0&&(CsI1Channel==2*YdNo2||CsI1Channel==2*YdNo2+1)) hYdCsIPID1->Fill(CsI1Energy,YdEnergy2*cos(det->TYdTheta*0.01745329));
		if(YdChannel2>=0&&CsI1Channel2>=0&&(CsI1Channel2==2*YdNo2||CsI1Channel==2*YdNo2+1)) hYdCsIPID1->Fill(CsI1Energy2,YdEnergy2*cos(det->TYdTheta*0.01745329));

     	if(YdChannel>=0&&CsI2Channel>=0&&(CsI1Channel==2*YdNo||CsI1Channel==2*YdNo+1)) hYdCsI2adcPID->Fill(CsI2nadc,YdEnergy*cos(det->TYdTheta*0.01745329));
     	if(YdChannel>=0&&CsI2Channel>=0&&(CsI1Channel==2*YdNo||CsI1Channel==2*YdNo+1)) hYdadcCsI2adcPID->Fill(CsI2nadc,Ydnadc);
     	
		if(YdChannel>=0&&CsI2Channel>=0&&(CsI2Channel==2*YdNo||CsI2Channel==2*YdNo+1)) hYdCsIPID2->Fill(CsI2Energy,YdEnergy*cos(det->TYdTheta*0.01745329));
     	if(YdChannel>=0&&CsI2Channel2>=0&&(CsI2Channel2==2*YdNo||CsI2Channel2==2*YdNo+1)) hYdCsIPID2->Fill(CsI2Energy2,YdEnergy*cos(det->TYdTheta*0.01745329));
     	if(YdChannel2>=0&&CsI2Channel>=0&&(CsI2Channel==2*YdNo2||CsI2Channel==2*YdNo2+1)) hYdCsIPID2->Fill(CsI2Energy,YdEnergy2*cos(det->TYdTheta*0.01745329));
     	if(YdChannel2>=0&&CsI2Channel2>=0&&(CsI2Channel2==2*YdNo2||CsI2Channel2==2*YdNo2+1)) hYdCsIPID2->Fill(CsI2Energy2,YdEnergy2*cos(det->TYdTheta*0.01745329));

		if(YdChannel>=0&&YdEnergy>0.) Ydrange->Fill(Ydnadc,YdEnergy);
		if(CsI1Channel>=0&&CsI1Energy>0.) CsI1range->Fill(CsI1nadc,CsI1Energy);
		if(CsI2Channel>=0&&CsI2Energy>0.) CsI2range->Fill(CsI2nadc,CsI2Energy);

		if(YdChannel>=0&&CsI1Channel>=0) hYdCsI1Corr->Fill(YdNo,CsI1Channel);
		if(YdChannel>=0&&CsI2Channel>=0) hYdCsI2Corr->Fill(YdNo,CsI2Channel);
//		//printf("Ydnadc is %f\n",Ydnadc);
//		hYdCsI1PID_uncalibrated->Fill(CsI1Energy,Ydnadc*cos(det->TYdTheta*0.01745329));
//		hYdCsI2PID_uncalibrated->Fill(CsI2Energy,Ydnadc*cos(det->TYdTheta*0.01745329));
//		if (ICnadc>=40 && ICnadc <100){ // condition of ICgate :Jaspreet
// 			hYdCsI2PID_forNa->Fill(CsI2Energy,YdEnergy*cos(det->TYdTheta*0.01745329));
//		}
//		else if (ICnadc >=100 && ICnadc <350){
//			hYdCsI2PID_forMg->Fill(CsI2Energy,YdEnergy*cos(det->TYdTheta*0.01745329));
//		}
         	if (1){// if((CsIChannel-16)/2==ydnumber){
    		det->TCsIEnergy = CsI1Energy; //for filling the tree
    		det->TCsIChannel = CsI1Channel;
    	}
    	else {
    		det->TCsIEnergy = -100; //for filling the tree
    		det->TCsIChannel = -100;
    	}

		printf("etc:\n");     
     	if (1){// if((CsIChannel-16)/2==ydnumber){
    		det->TCsI1Energy = CsI1Energy; //for filling the tree
    		det->TCsI1Channel = CsI1Channel;
    	}

   		if (1){// if((CsIChannel-16)/2==ydnumber){
    		det->TCsI2Energy = CsI2Energy; //for filling the tree
    		det->TCsI2Channel = CsI2Channel;
    	}
    	//printf("modid: %d , CsIEnergy: %f \n",modid, CsIEnergy);

 		//AS angles

	  	if (Sd1rEnergy>0){
	    	Sdtheta = TMath::RadToDeg()*(geoM.SdInnerRadius*(24.-Sd1rChannel-0.5)+geoM.SdOuterRadius*(Sd1rChannel+0.5))/24./geoM.Sd1Distance; //AS theta angle for Sd (24 - number of rings)
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
	  
	  	if ((fabs(Sd1sEnergy-Sd1rEnergy)<2.) && (fabs(Sd2sEnergy-Sd2rEnergy)<2.)) hSdPID->Fill(Sd2rEnergy,Sd1rEnergy);
	  	if ((fabs(Sd1sEnergy-Sd1rEnergy)<2.) && (fabs(Sd2sEnergy-Sd2rEnergy)<2.)) hSdETot->Fill(Sd1rEnergy+Sd2rEnergy);

		if (elasticS3->IsInside(Sd2sEnergy,Sd1sEnergy)) hSd2rElHits->Fill(Sd2rChannel, 1.); //Sd2r hits with elastic gat

 		if (elasticS3->IsInside(Sd2sEnergy,Sd1sEnergy)) hSd2sElHits->Fill(Sd2sChannel, 1.); //Sd2r hits with elastic gate

 		if (elasticS3->IsInside(Sd2sEnergy,Sd1sEnergy)) hSd1rElHits->Fill(Sd1rChannel, 1.); //Sd2r hits with elastic gate

 		if (elasticS3->IsInside(Sd2sEnergy,Sd1sEnergy)) hSd1sElHits->Fill(Sd1sChannel, 1.); //Sd2r hits with elastic gate

      
		//hYdCsIPID2->Fill(CsI2Energy,YdEnergy*cos(det->TYdTheta*0.01745329));
 		
		if (YdEnergy!=0) hYdCsIETot->Fill(YdEnergy); // AS Note: Add cos theta after calculation of angles is added
 		
		//hYdCsIPID1->Fill(CsI1Energy,YdEnergy*cos(det->TYdTheta*0.01745329));
		//hYdCsIPID1->Fill(CsI1Energy,YdEnergy*cos(det->TYdTheta*0.01745329));
     	
		if (protons->IsInside(CsI1Energy,YdEnergy*cos(det->TYdTheta*1.74532925199432955e-02))){
       		hYdHitsProt->Fill(YdChannel,1.);
       	}
	} //last bank
}

//---------------------------------------------------------------------------------
void HandleBOR_Mesytec(int run, int time, det_t* pdet)
{
	std::string CalibFile = "config_online.txt";
	calMesy.Load(CalibFile);
	calMesy.Print(0);

 	//proton gate
   	TFile *fgate = new TFile("/home/iris/current/anaIris/online_gate_18O.root");
	if(fgate->IsZombie()) protons = new TCutG();
	//else{
  	//	protons = (TCutG*)fgate->FindObjectAny("protons1");
  	//	protons->SetName("protons"); //protons in Physics file
	//}
	else{
  		protons = (TCutG*)fgate->FindObjectAny("18O");
  		protons->SetName("18O"); //protons in Physics file
	}
  	elasticS3 = new TCutG();

	ascii = calMesy.boolASCII;
	
	char label[32],sig[32];
	
	geoM.ReadGeometry(calMesy.fileGeometry.data());
// ************************************************************************************

	//ASCII output file
    if (ascii) {
  		sprintf(label,"ASCIIYY1run%d.txt",gRunNumber);    
  		ASCIIYY1 = fopen(label,"w");
 		fprintf(ASCIIYY1,"Evt:  id1: vpeak1: id2: vpeak2 \n");
		
		sprintf(label,"ASCIICsI1run%d.txt",gRunNumber); 
 		ASCIICsI1 = fopen(label,"w");
 		fprintf(ASCIICsI1,"Evt:  id1: vpeak1: id2: vpeak2 \n");

  		sprintf(label,"ASCIICsI2run%d.txt",gRunNumber); 
 		ASCIICsI2 = fopen(label,"w");
 		fprintf(ASCIICsI2,"Evt:  id1: vpeak1: id2: vpeak2 \n");

 		sprintf(label,"ASCIIICrun%d.txt",gRunNumber); 
 		ASCIIIC = fopen(label,"w");
 		fprintf(ASCIIIC,"Evt:  id1: vpeak1: id2: vpeak2 \n");

  		sprintf(label,"ASCIISd2run%d.txt",gRunNumber); 
 		ASCIISd2 = fopen(label,"w");
 		fprintf(ASCIISd2,"Evt:  idr1: vpeakr1: idr2: vpeakr2:  ids1: vpeaks1: ids2: vpeaks2 \n");

 		sprintf(label,"ASCIISd1run%d.txt",gRunNumber);
		ASCIISd1 = fopen(label,"w");
 		fprintf(ASCIISd1,"Evt:  idr1: vpeakr1: idr2: vpeakr2:  ids1: vpeaks1: ids2: vpeaks2 \n");
	} //ascii AS
  	// return to overall file directory
  	gOutputFile->cd();

// Temporary variables for calibration 
 	Int_t Chan=-1;
	double a,b,c;
	int g; //for ringwise calibration of CsI

//************** Calibrate IC, not yet implemented! *********************************
	FILE * pFile;
	FILE * logFile;
 	char buffer[32];
	
	// logfile
   	logFile = fopen("anaIris.log","w");
	
   	pFile = fopen(calMesy.fileIC.data(), "r");

	if (pFile == NULL || calMesy.boolIC==false) {
		//perror ("No file");
		fprintf(logFile,"No calibration file for IC. Skipping IC calibration.\n");
		printf("No calibration file for IC. Skipping IC calibration.\n");
		for (int i =0;i<16;i++  ){
			ICPed[i] = 0.;
			ICGain[i] = 1.;
     	}
   	}  
 	else  {
		printf("Reading IC config file '%s'\n",calMesy.fileIC.data());
		// Skip first line
  		fscanf(pFile,"%s",buffer);
  		fscanf(pFile,"%s",buffer);
 		fscanf(pFile,"%s",buffer);

		for (int i =0;i<16;i++  ){
       		fscanf(pFile,"%d%lf%lf",&Chan,&a,&b);
			ICPed[Chan] = a;
			ICGain[Chan] = b;
			printf("ICPed %lf ICgain %lf\n",ICPed[Chan],ICGain[Chan]);
     	}
     	fclose (pFile);
		printf("\n");
 	}
//*****************************************************************

//*************** Calibrate CsI1 ******************************
 	Chan=-1;  
 		
 	pFile = fopen (calMesy.fileCsI1.data(), "r");
   	if (pFile == NULL || calMesy.boolCsI1==false) {
		// perror ("No file");
		fprintf(logFile,"No calibration file for CsI1. Skipping CsI1 calibration.\n");
		printf("No calibration file for CsI1. Skipping CsI1 calibration.\n");
   		for (int i =0; i<16; i++){
			CsI1Ped[i] = 0.;
			for (int j=0; j<NCsI1GroupRing; j++){
				CsI1Gain[j][i] = 1.;
 			}//for
		}
	}  
 	else {
		printf("Reading config file '%s'\n",calMesy.fileCsI1.data());
		// Skip first line
		fscanf(pFile,"%s",buffer);
  		fscanf(pFile,"%s",buffer);
 		fscanf(pFile,"%s",buffer);
 		fscanf(pFile,"%s",buffer);

		//for (int i=0; i<1; i++){
		for (int i =0; i<16; i++){
			for (int j=0; j<NCsI1GroupRing; j++){
				fscanf(pFile,"%d%d%lf%lf",&Chan,&g,&a,&b);
				CsI1Ped[Chan-32] = a;
				CsI1Gain[g][Chan-32] = b;
              	printf("CsI1 calibration par: adc =%d\tc=%d\tpeds=%f\tgain=%f\n",Chan,g,a,b);
 			}//for
		}
		fclose (pFile);
		printf("\n");
 	}//else
// ******************************************************************
//
// ******************** Calibrate CsI2 *****************************

	pFile = fopen (calMesy.fileCsI2.data(),"r");

	if (pFile == NULL || calMesy.boolCsI2==false) {
		fprintf(logFile,"No calibration file for CsI2. Skipping CsI2 calibration.\n");
		printf("No calibration file for CsI2. Skipping CsI2 calibration.\n");
		for (int i =0; i<16; i++){
			CsI2Ped[i] = 0.;
			for (int j=0; j<NCsI2Group; j++){
				CsI2Gain[j][i] = 1.;
 			}//for
		}
	}  

	else  {
		printf("Reading config file '%s'\n",calMesy.fileCsI2.data());
		// Skip first line
		fscanf(pFile,"%s",buffer);
		fscanf(pFile,"%s",buffer);
		fscanf(pFile,"%s",buffer);
		fscanf(pFile,"%s",buffer);
		for(int i=0; i<16; i++){
			for (int j=0; j<NCsI2Group; j++){
   				fscanf(pFile,"%d%d%lf%lf",&Chan,&g,&a,&b);
				CsI2Ped[Chan-48] = a;  
				CsI2Gain[g][Chan-48] = b;  
				//printf("CsIPed %lf CsIgain %lf\n",a,b);
          		printf("CsI2 calibration par: adc =%d\tc=%d\tpeds=%f\tgain=%f\n",Chan,g,a,b);
 			}
		}
		fclose (pFile);
		printf("\n");
	}

 	printf(" Reading CsI calibration parameters Done ....\n\n");
//************************************************************************

//**************** Calibrate Sd2 rings ****************************************
	Chan=-1;

	pFile = fopen (calMesy.fileSd2r.data(), "r");
	usePeds = 1;

   	if (pFile == NULL || calMesy.boolSd2r==false) {
		fprintf(logFile,"No calibration file for Sd2 rings. Skipping Sd2r calibration.\n");
		printf("No calibration file for Sd2 rings. Skipping Sd2r calibration.\n");
   		for (int i =0;i<24;i++  ){
			Sd2rPed[i] = 0.;
			Sd2rGain[i] = 1.;  
		}
	}  
 	else {
		printf("Reading Sd2r config file '%s'\n",calMesy.fileSd2r.data());
		// Skip first line
  		fscanf(pFile,"%s",buffer);
  		fscanf(pFile,"%s",buffer);
 		fscanf(pFile,"%s",buffer);

		for (int i =0;i<24;i++  ){
       		fscanf(pFile,"%d%lf%lf",&Chan,&a,&b);
       		if(!usePeds){
				Sd2rOffset[Chan-64] = a;
			}
       		else if (usePeds){
				Sd2rPed[Chan-64] = a;
				Sd2rGain[Chan-64] =  b;  
				printf("Sd2rPed %lf Sd2rgain %lf\n",a,b);
     		}
		}
     	fclose (pFile);
		printf("\n");
 	}
//************************************************************************

//**************** Calibrate Sd2 sectors ****************************************
	Chan=-1;
   	pFile = fopen (calMesy.fileSd2s.data(), "r");

 	if (pFile == NULL || calMesy.boolSd2s==false) {
		fprintf(logFile,"No calibration file for Sd2 sectors. Skipping Sd2s calibration.\n");
		printf("No calibration file for Sd2 sectors. Skipping Sd2s calibration.\n");
   		for (int i =0;i<32;i++  ){
			Sd2sPed[i] = 0.;
			Sd2sGain[i] = 1.;  
		}
	}  
 	else  {
		printf("Reading Sd2s config file '%s'\n",calMesy.fileSd2s.data());
		// Skip first line
	  	fscanf(pFile,"%s",buffer);
	  	fscanf(pFile,"%s",buffer);
	 	fscanf(pFile,"%s",buffer);

		for (int i =0;i<32;i++  ){
       		fscanf(pFile,"%d%lf%lf",&Chan,&a,&b);
       		if (!usePeds){
				Sd2sOffset[Chan-96] = a;
			}
       		else if (usePeds){
				Sd2sPed[Chan-96] = a;
				Sd2sGain[Chan-96] = b;   
				printf("Sd2sPed %lf Sd2sgain %lf\n",a,b);
			}
     	}
     	fclose (pFile);
		printf("\n");
 	}

//************************************************************************


//**************** Calibrate Sd1 rings ****************************************
	Chan=-1;
   	pFile = fopen(calMesy.fileSd1r.data(), "r");

	if (pFile == NULL || calMesy.boolSd1r==false) {
		fprintf(logFile,"No calibration file for Sd1 rings. Skipping Sd1r calibration.\n");
		printf("No calibration file for Sd1 rings. Skipping Sd1r calibration.\n");
   		for (int i =0;i<24;i++  ){
			Sd1rPed[i] = 0.;
			Sd1rGain[i] = 1.;  
		}
	}  
 	else  {
		printf("Reading Sd1r config file '%s'\n",calMesy.fileSd1r.data());
		// Skip first line
  		fscanf(pFile,"%s",buffer);
  		fscanf(pFile,"%s",buffer);
 		fscanf(pFile,"%s",buffer);

		for (int i =0;i<24;i++  ){
       		fscanf(pFile,"%d%lf%lf",&Chan,&a,&b);
       		if (!usePeds)
				Sd1rOffset[Chan-128] = a;
       		else if (usePeds)
				Sd1rPed[Chan-128] = a;
				Sd1rGain[Chan-128] = b;
				printf("Sd1rPed %lf Sd1rgain %lf\n",Sd1rPed[Chan-128],Sd1rGain[Chan-128]);
     	}
     	fclose (pFile);
		printf("\n");
 	}
//************************************************************************

//**************** Calibrate Sd1 sectors ****************************************
	Chan=-1;
   	pFile = fopen (calMesy.fileSd1s.data(), "r");

	if (pFile == NULL || calMesy.boolSd1s==false) {
		fprintf(logFile,"No calibration file for Sd1 sectors. Skipping Sd1s calibration.\n");
		printf("No calibration file for Sd1 sectors. Skipping Sd1s calibration.\n");
   		for (int i =0;i<24;i++  ){
			Sd1sPed[i] = 0.;
			Sd1sGain[i] = 1.;  
		}
	}  
 	else  {
		printf("Reading Sd1s config file '%s'\n",calMesy.fileSd1s.data());
  		// Skip first line
		fscanf(pFile,"%s",buffer);
  		fscanf(pFile,"%s",buffer);
 		fscanf(pFile,"%s",buffer);

		for (int i =0;i<32;i++  ){
       		fscanf(pFile,"%d%lf%lf",&Chan,&a,&b);
       		if (!usePeds){
				Sd1sOffset[Chan-160] = a;
			}
       		else if (usePeds){
				Sd1sPed[Chan-160] = a;
				Sd1sGain[Chan-160] = b; 
				printf("Sd1sPed %lf Sd1sgain %lf\n",a,b);
			}
 		}
     	fclose (pFile);
		printf("\n");
	}
//************************************************************************

//**************** Calibrate Su rings ****************************************
	Chan=-1;
   	pFile = fopen(calMesy.fileSur.data(), "r");

	if (pFile == NULL || calMesy.boolSur==false) {
		fprintf(logFile,"No calibration file for Su rings. Skipping Sur calibration.\n");
		printf("No calibration file for Su rings. Skipping Sur calibration.\n");
   		for (int i =0;i<24;i++  ){
			SurPed[i] = 0.;
			SurGain[i] = 1.;  
		}
	}  
 	else  {
		printf("Reading Sur config file '%s'\n",calMesy.fileSur.data());
		// Skip first line
  		fscanf(pFile,"%s",buffer);
  		fscanf(pFile,"%s",buffer);
 		fscanf(pFile,"%s",buffer);

		for (int i =0;i<24;i++  ){
       		fscanf(pFile,"%d%lf%lf",&Chan,&a,&b);
       		if (!usePeds)
				SurOffset[Chan-320] = a;
       		else if (usePeds)
				SurPed[Chan-320] = a;
				SurGain[Chan-320] = b;
				printf("SurPed %lf Surgain %lf\n",SurPed[Chan-320],SurGain[Chan-320]);
     	}
     	fclose (pFile);
		printf("\n");
 	}
//************************************************************************

//**************** Calibrate Su sectors ****************************************
	Chan=-1;
   	pFile = fopen (calMesy.fileSus.data(), "r");

	if (pFile == NULL || calMesy.boolSus==false) {
		fprintf(logFile,"No calibration file for Su sectors. Skipping Sus calibration.\n");
		printf("No calibration file for Su sectors. Skipping Sus calibration.\n");
   		for (int i =0;i<24;i++  ){
			SusPed[i] = 0.;
			SusGain[i] = 1.;  
		}
	}  
 	else  {
		printf("Reading Sus config file '%s'\n",calMesy.fileSus.data());
  		// Skip first line
		fscanf(pFile,"%s",buffer);
  		fscanf(pFile,"%s",buffer);
 		fscanf(pFile,"%s",buffer);

		for (int i =0;i<32;i++  ){
       		fscanf(pFile,"%d%lf%lf",&Chan,&a,&b);
       		if (!usePeds){
				SusOffset[Chan-352] = a;
			}
       		else if (usePeds){
				SusPed[Chan-352] = a;
				SusGain[Chan-352] = b; 
				printf("SusPed %lf Susgain %lf\n",a,b);
			}
 		}
     	fclose (pFile);
		printf("\n");
	}
//************************************************************************

//**************** Calibrate Yd ****************************************
	Chan=-1;
   	pFile = fopen (calMesy.fileYd.data(), "r");

	if (pFile == NULL || calMesy.boolYd==false) {
		printf("No calibration file for Yd. Skipping Yd calibration.\n");
		for (int i =0;i<NYdChannels;i++  ){
			YdOffset[i] = 0.;
			YdGain[i] = 1.;  
		}
	}
	else  {
		printf("Reading config file '%s'\n",calMesy.fileYd.data());
 		// Skip first line
  		fscanf(pFile,"%s",buffer);
  		fscanf(pFile,"%s",buffer);
  		fscanf(pFile,"%s",buffer);

  		for (int i =0;i< NYdChannels;i++  ){
    		fscanf(pFile,"%d%lf%lf",&Chan,&a,&b);
			YdGain[Chan-192] = b;
 			YdOffset[Chan-192] = -1.*a*b;
 			printf("gain %lf ped %lf\t",b,a);
			if((i+1)%4==0) printf("\n");
    	}
    	fclose (pFile);
		printf("\n");
  	}
//************************************************************************

//**************** Calibrate Yu, not yet impemented!  ****************************************
	Chan=-1;
   	pFile = fopen (calMesy.fileYu.data(), "r");

	if (pFile == NULL || calMesy.boolYu==false) {
		fprintf(logFile,"No calibration file for Yu. Skipping Yu calibration.\n");
		printf("No calibration file for Yu. Skipping Yu calibration.\n");
		for (int i =0;i<NYdChannels;i++  ){
			YuOffset[i] = 0.;
			YuGain[i] = 1.;  
		}
	}
	else  {
		printf("Reading config file '%s'\n",calMesy.fileYu.data());
 		// Skip first line
  		fscanf(pFile,"%s",buffer);
  		fscanf(pFile,"%s",buffer);
  		fscanf(pFile,"%s",buffer);

  		for (int i =0;i< NYuChannels;i++  ){
    		fscanf(pFile,"%d%lf%lf",&Chan,&a,&b);
			YuGain[Chan-384] = b;
 			YuOffset[Chan-384] = -1.*a*b;
 			printf("gain %lf ped %lf\t",b,a);
			if((i+1)%4==0) printf("\n");
    	}
    	fclose (pFile);
		printf("\n");
  	}

//************************************************************************
    if (gOutputFile) {
		hMes_P[0] = (TH1D*)gDirectory->Get("adc00");
  		if (hMes_P[0] == 0) {

      		// Make an ADC directory and cd to it.
      		TDirectory* adc_dir = gOutputFile->mkdir("adc");      
      		adc_dir->cd();

      		TDirectory* adcIC_dir = adc_dir->mkdir("adcIC");
      		TDirectory* adcCsI_dir = adc_dir->mkdir("adcCsI");
      		TDirectory* adcSd2_dir = adc_dir->mkdir("adcSd2");
      		TDirectory* adcSd1_dir = adc_dir->mkdir("adcSd1");
      		TDirectory* adcYYd_dir = adc_dir->mkdir("adcYYd");
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
				hMes_P[channel] = new TH1D(label, sig, adcBins, 0, adcBins);
				//printf("Booking TH1D %s \n", label);
      		}
      		printf(" in Mesytec BOR... Booking histos Done ....\n");
		}
			
		hIC[0] = (TH1D*)gDirectory->Get("IC00");
   		if (hIC[0] == 0) {
     		// Make an IC directory and cd to it.
        	TDirectory* IC_dir = gOutputFile->mkdir("IC");      
        	IC_dir->cd();
       
			printf(" in Mesytec BOR... Booking IC histos\n");
       		for (int channel=0;channel<NICChannels;channel++) {
  				sprintf(label, "IC%02d", channel);
  				sprintf(sig, "IC%03d", channel);
  				hIC[channel] = new TH1D(label, sig, adcBins, 0, adcBins);
  				printf("Booking TH1D %s \n", label);
       		}
       		printf(" in Mesytec BOR... Booking IC histos Done ....\n");
   		}
		
		hCsI1[0] = (TH1D*)gDirectory->Get("CsI00");
		if (hCsI1[0] == 0) {
			 // Make a CSI directory and cd to it.
			TDirectory* CsI_dir = gOutputFile->mkdir("CsI");      
			CsI_dir->cd();
			
			printf(" in Mesytec BOR... Booking CsI1 histos\n");
			for (int channel=0;channel<NCsIChannels;channel++) {
				sprintf(label, "CsI1%02d", channel);
				sprintf(sig, "CsI1%03d", channel);
				hCsI1[channel] = new TH1D(label, sig, csiBins, 0, csiBins);
				printf("Booking TH1D %s \n", label);
			}
			for (int channel=0;channel<NCsIChannels;channel++) {
				sprintf(label, "CsI2%02d", channel);
				sprintf(sig, "CsI2%03d", channel);
				hCsI2[channel] = new TH1D(label, sig, csiBins, 0, csiBins);
				printf("Booking TH1D %s \n", label);
			}
		 
			hCsI1Sum = new TH1D("CsI1Sum", "CsI1Sum", csiBins, 0, csiBins);
			printf("Booking TH1D CsI1Sum \n");
		
			hCsI2Sum = new TH1D("CsI2Sum", "CsI2Sum", csiBins, 0, csiBins);
			printf("Booking TH1D CsISum2 \n");
		   	printf(" in Mesytec BOR... Booking CsI histos Done ....\n");
   		}
	 
 		hSd1r[0] = (TH1D*)gDirectory->Get("Sd1r00");
		if (hSd1r[0] == 0) {
     		// Make an SD1R directory and cd to it.
     		TDirectory* Sd1r_dir = gOutputFile->mkdir("Sd1r");      
     		Sd1r_dir->cd();

       		printf(" in Mesytec BOR... Booking Sd1r histos\n");
       		for (int channel=0;channel<NSd1rChannels;channel++) {
  				sprintf(label, "Sd1r%02d", channel);
  				sprintf(sig, "Sd1r%03d", channel);
  				hSd1r[channel] = new TH1D(label, sig, energyBins, 0, energyLimitSd1);
  				printf("Booking TH1D %s \n", label);
       		}
 			hSd1rSummary = new TH2D("Sd1rSummary", "Sd1rSummary",32,0,32, energyBins, 0, energyLimitSd1);
			printf(" in Mesytec BOR... Booking Sd1r histos Done ....\n");
   		}

 		hSd1s[0] = (TH1D*)gDirectory->Get("Sd1s00");
   		if (hSd1s[0] == 0) {
     		// Make an SD1S directory and cd to it.
     		TDirectory* Sd1s_dir = gOutputFile->mkdir("Sd1s");      
     		Sd1s_dir->cd();

       		printf(" in Mesytec BOR... Booking Sd1s histos\n");
       		for (int channel=0;channel<NSd1sChannels;channel++) {
  				sprintf(label, "Sd1s%02d", channel);
  				sprintf(sig, "Sd1s%03d", channel);
  				hSd1s[channel] = new TH1D(label, sig, energyBins, 0, energyLimitSd1);
  				printf("Booking TH1D %s \n", label);
       		}
 			hSd1sSummary = new TH2D("Sd1sSummary", "Sd1sSummary",32,0,32, energyBins, 0, energyLimitSd1);
       		printf(" in Mesytec BOR... Booking Sd1s histos Done ....\n");
   		}	
 
		// Make an SD2R directory and cd to it.
		hSd2r[0] = (TH1D*)gDirectory->Get("Sd2r00");
		if (hSd2r[0] == 0) {
		   	TDirectory* Sd2r_dir = gOutputFile->mkdir("Sd2r");      
     		Sd2r_dir->cd();
			printf(" in Mesytec BOR... Booking Sd2r histos\n");
		   	for (int channel=0;channel<NSd2rChannels;channel++) {
				sprintf(label, "Sd2r%02d", channel);
				sprintf(sig, "Sd2r%03d", channel);
				hSd2r[channel] = new TH1D(label, sig, energyBins, 0, energyLimitSd2);
				printf("Booking TH1D %s \n", label);
			}
		   	hSd2rSummary = new TH2D("Sd2rSummary", "Sd2rSummary",32,0,32, energyBins, 0, energyLimitSd2);
		   	printf(" in Mesytec BOR... Booking Sd2r histos Done ....\n");
		}

		hSd2s[0] = (TH1D*)gDirectory->Get("Sd2s00");
		if (hSd2s[0] == 0) {
		   	TDirectory* Sd2s_dir = gOutputFile->mkdir("Sd2s");      
     		Sd2s_dir->cd();
		   	printf(" in Mesytec BOR... Booking Sd2s histos\n");
		   	for (int channel=0;channel<NSd2sChannels;channel++) {
				sprintf(label, "Sd2s%02d", channel);
				sprintf(sig, "Sd2s%03d", channel);
				hSd2s[channel] = new TH1D(label, sig, energyBins, 0, energyLimitSd2);
				printf("Booking TH1D %s \n", label);
			}
		   	hSd2sSummary = new TH2D("Sd2sSummary", "Sd2sSummary",32,0,32, energyBins, 0, energyLimitSd2);
		   	printf(" in Mesytec BOR... Booking Sd2s histos Done ....\n");
		}
 			
		hSus[0] = (TH1D*)gDirectory->Get("Sus00");
   		if (hSus[0] == 0) {
     		// Make an SUS directory and cd to it.
         	TDirectory* Sus_dir = gOutputFile->mkdir("Sus");      
         	Sus_dir->cd();

       		printf(" in Mesytec BOR... Booking Sus histos\n");
       		for (int channel=0;channel<NSusChannels;channel++) {
  				sprintf(label, "Sus%02d", channel);
  				sprintf(sig, "Sus%03d", channel);
  				hSus[channel] = new TH1D(label, sig, adcBins, 0, adcBins);
  				printf("Booking TH1D %s \n", label);
       		}
       		printf(" in Mesytec BOR... Booking Sus histos Done ....\n");
   		}
 
		hSur[0] = (TH1D*)gDirectory->Get("Sur00");
   		if (hSur[0] == 0) {
     		// Make an SUS directory and cd to it.
         	TDirectory* Sur_dir = gOutputFile->mkdir("Sur");      
         	Sur_dir->cd();

       		printf(" in Mesytec BOR... Booking Sur histos\n");
       		for (int channel=0;channel<NSurChannels;channel++) {
  				sprintf(label, "Sur%02d", channel);
  				sprintf(sig, "Sur%03d", channel);
  				hSur[channel] = new TH1D(label, sig, adcBins, 0, adcBins);
  				printf("Booking TH1D %s \n", label);
       		}
       		printf(" in Mesytec BOR... Booking Sur histos Done ....\n");
   		}
  
		hYd[0] = (TH1D*)gDirectory->Get("Yd00");
   		if (hYd[0] == 0) {
    		// Make a YD directory and cd to it.
           	TDirectory* Yd_dir = gOutputFile->mkdir("Yd");      
           	Yd_dir->cd();       

			printf(" in Mesytec BOR... Booking Yd histos\n");
       		for (int channel=0;channel<NYdChannels;channel++) {
  				sprintf(label, "Yd%02d", channel);
  				sprintf(sig, "Yd%03d", channel);
  				hYd[channel] = new TH1D(label, sig, energyBins, 0, energyLimitYd);
  				printf("Booking TH1D %s \n", label);
       		}
       		hYdSummary = new TH2D("YdSummary","YdSummary",NYdChannels, 0 ,NYdChannels, energyBins, 0, energyLimitYd);
       		printf(" in Mesytec BOR... Booking Yd histos Done ....\n");
   		}

		hYu[0] = (TH1D*)gDirectory->Get("Yu00");
   		if (hYu[0] == 0) {
    		// Make a YD directory and cd to it.
           	TDirectory* Yu_dir = gOutputFile->mkdir("Yu");      
           	Yu_dir->cd();       

			printf(" in Mesytec BOR... Booking Yu histos\n");
       		for (int channel=0;channel<NYuChannels;channel++) {
  				sprintf(label, "Yu%02d", channel);
  				sprintf(sig, "Yu%03d", channel);
  				hYu[channel] = new TH1D(label, sig, energyBins, 0, energyLimitYu);
  				printf("Booking TH1D %s \n", label);
       		}
       		hYuSummary = new TH2D("YuSummary","YuSummary",NYuChannels, 0 ,NYuChannels, energyBins, 0, energyLimitYu);
       		printf(" in Mesytec BOR... Booking Yu histos Done ....\n");
   		}
		
		// ====================== //
		// ==== Hit Patterns ==== //
		// ====================== //
		printf(" in Mesytec BOR... Booking Hits histo\n");
		gOutputFile->cd();
		sprintf(label, "SSB");
		sprintf(sig, "SSB");
		hSSB = new TH1D(label, sig, energyBins, 0, energyLimitSSB);
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
			
		sprintf(label, "CsI1Hits");
		sprintf(sig, "CsI1Hits");
		hCsI1Hits = new TH1D(label, sig, 16, 0, 16);
		printf("Booking TH1D %s \n", label);
		
		sprintf(label, "CsI2Hits");
		sprintf(sig, "CsI2Hits");
		hCsI2Hits = new TH1D(label, sig, 16, 0, 16);
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
		 
		sprintf(label, "Yu1Hits");
		sprintf(sig, "Yu1Hits");
		hYu1Hits = new TH1D(label, sig, 16, 0, 16);
		printf("Booking TH1D %s \n", label);
		
		sprintf(label, "Yu2Hits");
		sprintf(sig, "Yu2Hits");
		hYu2Hits = new TH1D(label, sig, 16, 0, 16);
		printf("Booking TH1D %s \n", label);
		
		sprintf(label, "Yu3Hits");
		sprintf(sig, "Yu3Hits");
		hYu3Hits = new TH1D(label, sig, 16, 0, 16);
		printf("Booking TH1D %s \n", label);
		
		sprintf(label, "Yu4Hits");
		sprintf(sig, "Yu4Hits");
		hYu4Hits = new TH1D(label, sig, 16, 0, 16);
		printf("Booking TH1D %s \n", label);
		
		sprintf(label, "Yu5Hits");
		sprintf(sig, "Yu5Hits");
		hYu5Hits = new TH1D(label, sig, 16, 0, 16);
		printf("Booking TH1D %s \n", label);
		
		sprintf(label, "Yu6Hits");
		sprintf(sig, "Yu6Hits");
		hYu6Hits = new TH1D(label, sig, 16, 0, 16);
		printf("Booking TH1D %s \n", label);
		
		sprintf(label, "Yu7Hits");
		sprintf(sig, "Yu7Hits");
		hYu7Hits = new TH1D(label, sig, 16, 0, 16);
		printf("Booking TH1D %s \n", label);
		  
		sprintf(label, "Yu8Hits");
		sprintf(sig, "Yu8Hits");
		hYu8Hits = new TH1D(label, sig, 16, 0, 16);
		printf("Booking TH1D %s \n", label);
		
		sprintf(label, "YuHits");
		sprintf(sig, "YuHits");
		hYuHits = new TH1D(label, sig, 16, 0, 16);
		printf("Booking TH1D %s \n", label);
		  
		sprintf(label, "YdHitsProt");
		sprintf(sig, "YdHitsProt");
		hYdHitsProt = new TH1D(label, sig, 128, 0, 128);
		printf("Booking TH1D %s \n", label);
		printf(" in Mesytec BOR... Booking  Hits histos Done ....\n");
		
		printf(" in Mesytec BOR... Booking ETot histos\n");
		
		sprintf(label, "SdETot");
		sprintf(sig, "SdEtot");
		hSdETot= new TH1D(label, sig, 2000., 0, 200.);
		printf("Booking TH1D %s \n", label);
       
 		sprintf(label, "YdCsIETot");
	 	sprintf(sig, "YdCsIEtot");
	 	hYdCsIETot = new TH1D(label, sig, adcBins, 0, 50);
	 	printf("Booking TH1D %s \n", label);

		printf(" in Mesytec BOR... Booking ETot histos Done ....\n");
   
		// PID Spectra
		//
       	printf(" in Mesytec BOR... Booking PID histos\n");
       	sprintf(label,"SdPID");
	 	hSdPID= new TH2F( "SdPID", "SDPID", 1000, 0., 100., 1000, 0., 200.);
	 	printf("Booking TH2F %s \n", label);
       
	 	hYdCsIPID2 = new TH2F("YdCsIPID2", "YdCsIPID2", 1500, 0., 75., 1000, 0., 30.);
	 	printf("Booking TH2F %s \n", label);

	 	hYdCsIPID1 = new TH2F("YdCsIPID1", "YdCsIPID1", 1500, 0., 75., 1000, 0., 30.);
        printf("Booking TH2F %s \n", label);

		hYdCsI1adcPID = new TH2F("YdCsI1PIDadc", "YdCsI1PIDadc", 1024, 0, 4096, 1000, 0., 10.);
        printf("Booking TH2F %s \n", label);
		hYdCsI2adcPID = new TH2F("YdCsI2PIDadc", "YdCsI2PIDadc", 1024, 0, 4096, 1000, 0., 10.);
        printf("Booking TH2F %s \n", label);

		hYdadcCsI1adcPID = new TH2F("YdadcCsI1PIDadc", "YdadcCsI1PIDadc", 1024, 0, 4096, 1024, 0., 4096.);
        printf("Booking TH2F %s \n", label);
		hYdadcCsI2adcPID = new TH2F("YdadcCsI2PIDadc", "YdadcCsI2PIDadc", 1024, 0, 4096, 1024, 0., 4096.);
        printf("Booking TH2F %s \n", label);

		Ydrange = new TH2F("Ydrange", "Ydrange", 1024, 0, 4096, 1500, 0., 20.);
        printf("Booking TH2F %s \n", label);
		CsI1range = new TH2F("CsI1range", "CsI1range", 1024, 0, 4096, 1500, 0., 100.);
        printf("Booking TH2F %s \n", label);
		CsI2range = new TH2F("CsI2range", "CsI2range", 1024, 0, 4096, 1500, 0., 100.);
        printf("Booking TH2F %s \n", label);
		
		hYdCsI1Corr = new TH2F("YdCsI1Corr", "YdCsI1Corr", 8, 0, 8, 16, 0., 16.);
        printf("Booking TH2F %s \n", label);
		hYdCsI2Corr = new TH2F("YdCsI2Corr", "YdCsI2Corr", 8, 0, 8, 16, 0., 16.);
        printf("Booking TH2F %s \n", label);

//		hYdCsI1PID_uncalibrated = new TH2F("YdCsI1PID_uncalibrated", "YdCsI1PID_uncalibrated", 4000, 0, 4000, 4000, 0, 4000);
//        printf("Booking TH2F %s \n", label);
//
//		hYdCsI2PID_uncalibrated = new TH2F("YdCsI2PID_uncalibrated", "YdCsI2PID_uncalibrated", 4000, 0, 4000, 4000, 0, 4000);
//        printf("Booking TH2F %s \n", label);

       	printf(" in Mesytec BOR... Booking PID histos Done ....\n");

       	//AS Angle histos

 		printf(" in Mesytec BOR... Booking angle histos\n");
     
  		sprintf(label,"YdTheta");
	 	hYdTheta= new TH1D( "YdTheta", "YdTheta", 360, 0, 90);
	 	printf("Booking TH1D %s \n", label);

		sprintf(label,"YdETheta");
 		hYdETheta = new TH2D(label, "YdETheta",90,0,90,750,0,25);
	 	printf("Booking T21D %s \n", label);

  		sprintf(label,"SdTheta");
	 	hSdTheta= new TH1D( "SdTheta", "SdTheta", 360, 0, 90);
	 	printf("Booking TH1D %s \n", label);
       
 		sprintf(label,"SdPhi");
	 	hSdPhi = new TH1D(label, "SdPhi", 360, 0, 360);
	 	printf("Booking TH1D %s \n", label);

 		sprintf(label,"SdPhiTheta");
 		hSdPhiTheta = new TH2D(label, "SdPhiTheta", 180, 0, 180,180,0,360);
	 	printf("Booking TH2D %s \n", label);

		sprintf(label,"SdETheta");
 		hSdETheta = new TH2D(label, "SdETheta",150,0,15,750,0,150);
	 	printf("Booking TH2D %s \n", label);
 
  		sprintf(label,"YuTheta");
	 	hYuTheta= new TH1D( "YuTheta", "YuTheta", 360, 90, 180);
	 	printf("Booking TH1D %s \n", label);

		sprintf(label,"YuETheta");
 		hYuETheta = new TH2D(label, "YuETheta",90,90,180,750,0,25);
	 	printf("Booking TH2D %s \n", label);

		sprintf(label,"SuTheta");
	 	hSuTheta= new TH1D( "SuTheta", "SuTheta", 360, 90, 180);
	 	printf("Booking TH1D %s \n", label);
 
		sprintf(label,"SuETheta");
 		hSuETheta = new TH2D(label, "SuETheta",90,90,180,750,0,25);
	 	printf("Booking TH2D %s \n", label);
      
 		sprintf(label,"SuPhi");
	 	hSuPhi = new TH1D(label, "SuPhi", 360, 0, 360);
	 	printf("Booking TH1D %s \n", label);

       	printf(" in Mesytec BOR... Booking angle histos Done ....\n");

  	} // if(gOutputFile)

	calMesy.Print(0);
	// = IRIS WebServer =
	// Zero the web spectra at BOR
	memset(spec_store_address,0,sizeof(spec_store_address));
	memset(spec_store_type,0,sizeof(spec_store_type));
	memset(spec_store_eName,0,sizeof(spec_store_eName));
	//memset(spec_store_tName,0,sizeof(spec_store_tName));
	memset(spec_store_energyName,0,sizeof(spec_store_sum_name));
	memset(spec_store_adcName,0,sizeof(spec_store_adcName));
	memset(spec_store_hitName,0,sizeof(spec_store_hitName));
	memset(spec_store_2dhitName,0,sizeof(spec_store_2dhitName));
	memset(spec_store_eData,0,sizeof(spec_store_eData));
	//memset(spec_store_tData,0,sizeof(spec_store_tData));
	memset(spec_store_energyData,0,sizeof(spec_store_energyData));
	memset(spec_store_adcData,0,sizeof(spec_store_adcData));
	memset(spec_store_hitData,0,sizeof(spec_store_hitData));
	memset(spec_store_2dhitData,0,sizeof(spec_store_2dhitData));

	sprintf(spec_store_energyName[0],"ICEnergy");
	sprintf(spec_store_energyName[1],"YdEnergy");
	sprintf(spec_store_energyName[2],"CsI1Energy");
	sprintf(spec_store_energyName[3],"CsI2Energy");
	sprintf(spec_store_energyName[4],"Sd1rEnergy");
	sprintf(spec_store_energyName[5],"Sd1sEnergy");
	sprintf(spec_store_energyName[6],"Sd2rEnergy");
	sprintf(spec_store_energyName[7],"Sd2sEnergy");
	sprintf(spec_store_energyName[8],"SSBEnergy");
	sprintf(spec_store_energyName[9],"ScintEnergy");
	sprintf(spec_store_energyName[10],"YuEnergy");
	sprintf(spec_store_energyName[11],"SurEnergy");
	sprintf(spec_store_energyName[12],"SusEnergy");
		
	sprintf(spec_store_hitName[0],"CsI1Hits");
	sprintf(spec_store_hitName[1],"CsI2Hits");
	sprintf(spec_store_hitName[2],"Sd1rHits");
	sprintf(spec_store_hitName[3],"Sd1sHits");
	sprintf(spec_store_hitName[4],"Sd2rHits");
	sprintf(spec_store_hitName[5],"Sd2sHits");
	sprintf(spec_store_hitName[6],"Yd1Hits");
	sprintf(spec_store_hitName[7],"Yd2Hits");
	sprintf(spec_store_hitName[8],"Yd3Hits");
	sprintf(spec_store_hitName[9],"Yd4Hits");
	sprintf(spec_store_hitName[10],"Yd5Hits");
	sprintf(spec_store_hitName[11],"Yd6Hits");
	sprintf(spec_store_hitName[12],"Yd7Hits");
	sprintf(spec_store_hitName[13],"Yd8Hits");
	sprintf(spec_store_hitName[14],"YdHits");
	sprintf(spec_store_hitName[15],"SurHits");
	sprintf(spec_store_hitName[16],"SusHits");
	sprintf(spec_store_hitName[17],"Yu1Hits");
	sprintf(spec_store_hitName[18],"Yu2Hits");
	sprintf(spec_store_hitName[19],"Yu3Hits");
	sprintf(spec_store_hitName[20],"Yu4Hits");
	sprintf(spec_store_hitName[21],"Yu5Hits");
	sprintf(spec_store_hitName[22],"Yu6Hits");
	sprintf(spec_store_hitName[23],"Yu7Hits");
	sprintf(spec_store_hitName[24],"Yu8Hits");
	sprintf(spec_store_hitName[25],"YuHits");


	sprintf(spec_store_2dhitName[0],"2D-YdHits");
	sprintf(spec_store_2dhitName[1],"2D-CsI1Hits");
	sprintf(spec_store_2dhitName[2],"2D-CsI2Hits");
	sprintf(spec_store_2dhitName[3],"2D-Sd1rHits");
	sprintf(spec_store_2dhitName[4],"2D-Sd1sHits");
	sprintf(spec_store_2dhitName[5],"2D-Sd2rHits");
	sprintf(spec_store_2dhitName[6],"2D-Sd2sHits");
	sprintf(spec_store_2dhitName[7],"2D-YuHits");
	sprintf(spec_store_2dhitName[8],"2D-SuHits");

	for (int chn=0;chn<Nchannels;chn++) {
		if (chn<32)
			sprintf(spec_store_eName[chn],"ICCh%02dEnergy",chn);
		else if(chn>=32 && chn <64) 
			sprintf(spec_store_eName[chn],"CsICh%02dEnergy",chn-32);
		else if(chn>=64 && chn <128) 
			sprintf(spec_store_eName[chn],"Sd2Ch%02dEnergy",chn-64);
		else if(chn>=128 && chn <192) 
			sprintf(spec_store_eName[chn],"Sd1Ch%02dEnergy",chn-128);
		else if(chn>=192 && chn < 320) 
			sprintf(spec_store_eName[chn],"YdCh%03dEnergy",chn-192);
		else if(chn>=320 && chn < 384) 
			sprintf(spec_store_eName[chn],"SuCh%02dEnergy",chn-320);
		else if(chn>=384 && chn < 512) 
			sprintf(spec_store_eName[chn],"YuCh%03dEnergy",chn-384);
	}

	for (int chn=0;chn<Nchannels;chn++) {
		if (chn<32)
			sprintf(spec_store_adcName[chn],"ICCh%02dADC",chn);
		else if(chn>=32 && chn <64) 
			sprintf(spec_store_adcName[chn],"CsICh%02dADC",chn-32);
		else if(chn>=64 && chn <128) 
			sprintf(spec_store_adcName[chn],"Sd2Ch%02dADC",chn-64);
		else if(chn>=128 && chn <192) 
			sprintf(spec_store_adcName[chn],"Sd1Ch%02dADC",chn-128);
		else if(chn>=192 && chn < 320) 
			sprintf(spec_store_adcName[chn],"YdCh%03dADC",chn-192);
		else if(chn>=320 && chn < 384) 
			sprintf(spec_store_adcName[chn],"SuCh%02dADC",chn-320);
		else if(chn>=384 && chn < 512) 
			sprintf(spec_store_adcName[chn],"YuCh%03dADC",chn-384);
	}

	for(int jj=0; jj<128; jj++)
	{
		//sprintf(spec_store_eName[jj],"YdCh%03dEnergy",jj);
	}

	memset(spec_store_eData,0,sizeof(spec_store_eData));
	memset(spec_store_adcData,0,sizeof(spec_store_adcData));
	memset(spec_store_hitData,0,sizeof(spec_store_hitData));
	// =========================
 
//************************************************************************
}

void HandleEOR_Mesytec(int run, int time)
{
  printf(" in Mesytec EOR\n");
}
