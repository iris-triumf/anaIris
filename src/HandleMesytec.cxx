
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
#include <TRandom3.h>

#include "Globals.h"
#include "HandleMesytec.h"
#include "CalibMesytec.h"
#include "geometry.h"
#include "HandleMesytec.h"
#include "web_server.h" // = IRIS WebServer for IC =

int gMesytecnitems;

const int Nchannels = 16*32;
const int NICChannels=32;
const int NCsI2Group = 16;
const int NCsI1GroupRing = 4;
const int NCsIChannels=16;
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

 // = IRIS WebServer for IC =
#define SIZE_OF_ODB_MSC_TABLE 1024
#define NSPECS (SIZE_OF_ODB_MSC_TABLE)
// Declaration of the spectrum store
int spec_store_address[NSPECS];
int spec_store_type[NSPECS]; // 0=energy, 1=time, 2=waveform, 3=hitpattern
char spec_store_Ename[NSPECS][32];
char spec_store_Pname[NSPECS][32];
char spec_store_Wname[NSPECS][32];
char spec_store_Tname[NSPECS][32];
char spec_store_Zname[NSPECS][32]; // zero-crossing
char spec_store_Lname[NSPECS][32]; // cc_long
char spec_store_Sname[NSPECS][32]; // cc_short
int spec_store_Edata[NSPECS][SPEC_LENGTH];
int spec_store_Tdata[NSPECS][SPEC_LENGTH];
int spec_store_Wdata[NSPECS][SPEC_LENGTH];
int spec_store_Pdata[NSPECS][SPEC_LENGTH];
int spec_store_Zdata[NSPECS][SPEC_LENGTH]; // zero-crossing
int spec_store_Ldata[NSPECS][SPEC_LENGTH]; // cc_long
int spec_store_Sdata[NSPECS][SPEC_LENGTH]; // cc_short
int spec_store_hit_type[NHITS];
char spec_store_hit_name[NHITS][32];
int spec_store_hit_data[NHITS][SIZE_OF_ODB_MSC_TABLE];
char spec_store_sum_name[NHITS][32];
int spec_store_sum_data[NHITS][SPEC_LENGTH];
 // =========================

CalibMesytec calMesy;
geometry geoM;



TCutG *protons = NULL;//proton cut
TCutG *elasticS3 = NULL;//elastic gate on S3
int usePeds = 1; //using pedestals instead of offsets for Silicon detectors AS
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
int CsIChannel;
int CsIChannel2; // channel with the greatest value
int CsI1Channel;
int CsI1Channel2; // channel with the greatest value
int CsI2Channel;
int CsI2Channel2; // channel with the greatest value
//double CsI1Gain[NCsIChannels]={1.};
double CsI1Gain[NCsI1GroupRing][NCsIChannels]={{0.}};
double CsI1Ped[NCsIChannels]={0.};
//double CsI2Gain[NCsIChannels]={1.};
double CsI2Gain[NCsI2Group][NCsIChannels]={{0.}};
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
float Sd1sGain[NSd1sChannels]={0.};
float Sd1sOffset[NSd1sChannels]={0.};
float Sd1sPed[NSd1sChannels]={0.};
TH2D *hSd1sSummary = {NULL}; //summary spectra

TH1D * hSur[NSurChannels] = {NULL}; //  Upstream S3 rings 
float Sur[NSurChannels];
float SurEnergy=0; //Dummy for Sur energy
int SurChannel; // channel with the greatest value
float SurGain[NSurChannels]={1.};
float SurOffset[NSurChannels]={0.};
float SurPed[NSurChannels]={0.};

TH1D * hSus[NSusChannels] = {NULL}; // Upstream S3 sectors   
float Sus[NSusChannels];
int SusChannel; // channel with the greatest value
float SusEnergy=0; //Dummy for Sus energy                                                                                          
float SusGain[NSusChannels]={1.};
float SusOffset[NSusChannels]={0.};
float SusPed[NSusChannels]={0.};


//AS YYd
TH1D * hYd[NYdChannels] = {NULL}; // Q
float Yd[NYdChannels] ={0}; 
float YdEnergy=0, YdEnergy2=0; //Dummy for Yd energy
int YdADC = 0;
int YdChannel, YdChannel2; // channel with the greatest value
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
extern FILE* ASCIIIC;
extern FILE* ASCIICsI1;
extern FILE* ASCIICsI2;
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
TH2F *YdCsI1adcPID = {NULL};
TH2F *YdCsI2adcPID = {NULL};
TH2F *hYdCsI2PID_forNa = {NULL};
TH2F *hYdCsI2PID_forMg = {NULL};
TH2F *hYdCsI2PID_uncalibrated = {NULL};
//AS Angles
TH1D *hSdTheta = {NULL};
TH1D *hSdPhi = {NULL};
TH2D *hSdETheta = {NULL};
TH2D *hSdPhiTheta = {NULL};
TH1D *hYdTheta = {NULL};
TH1D *hYuTheta = {NULL};

float YdDistance = 0.; // distance from target in mm
float YuDistance = 83.; // distance from target in mm
float YdInnerRadius= 0., YdOuterRadius = 0. ; // inner and outer radii in mm
float Sd1Distance = 0., Sd2Distance = 0.; //distance from target in mm
float SuDistance = 200.; //distance from target in mm
float SdInnerRadius = 0., SdOuterRadius= 0.; //AS Inner and outer radii of an S3 detector (in mm).

Double_t xShift = 0;//1.97;
Double_t yShift = 0;//1.3;

float YdTheta[128] = {0.}, SdTheta=0., theta = 0 ,phi = 0, theta1 = 0, phi1=0; //AS Dummies for theta and phi, to be used for filling histograms
float length = 0; //The length from target to each Yd strips Junki
float Sdtheta = 0 ,Sdphi = 0, Sdtheta1 = 0, Sdphi1=0; //AS Dummies for theta and phi, to be used for filling histograms

TRandom3 fRandom(0);
Double_t randm; //random number between 0 and 1 for each event

TH1D *hSuTheta = {NULL};
TH1D *hSuPhi = {NULL};

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

 for (int j=0; j<NYdChannels; j++)
  Yd[j] = 0;

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
					spec_store_Pdata[channel+(modid*32)][vpeak]++;
	  				if ((modid==0) && (vpeak > adcThresh) && (vpeak<3840)){ // Why 3840? MH
	    				//AS Fill histogram
 						ICnadc = (float)vpeak; 
						IC[channel] = (float)vpeak;
						ICEnergy = ((float)vpeak-ICPed[channel])*ICGain[channel];
	    				hIC[channel]->Fill(ICEnergy, 1.); //IC
	    				
						spec_store_Edata[0][int(ICnadc)]++; // = IRIS WebServer for IC =
						
						// if (channel==16) // change MA july3
	        			if (channel==31){
	          				SSBEnergy = float(vpeak);// *SSBGain + SSBOffset;
							spec_store_Edata[8][int(SSBEnergy)]++; // = IRIS WebServer for IC =
							// printf("vpeak%f",float(vpeak));
							// printf("ssb E is %d", SSBEnergy);
						}
	  				}
	  
	  				if (modid==1 && vpeak > adcThresh && vpeak<3840){
	    				if (channel<16){
	    					CsI1[channel] = (float)vpeak;
	    	    		}	    
		  				else if (channel>=16){
	    					CsI2[channel-16] = (float)vpeak;
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
	  				}
	  
	  				if (modid==3 && vpeak > adcThresh && vpeak<3840){
	    				S3Hit = 1;
	    				if (!usePeds){	
	    					Sd2s[channel] = Sd2sOffset[channel]+Sd2sGain[channel]*(float)vpeak;
						}
	 					else if (usePeds){
	   						Sd2s[channel] = Sd2sGain[channel]*(((float)vpeak)-Sd2sPed[channel]);
							}
	   					}
	
	  				if (modid==4 && vpeak > adcThresh  && vpeak<3840){
	 					S3Hit = 1; 
						if (!usePeds){
	    					Sd1r[channel] = Sd1rOffset[channel]+Sd1rGain[channel]*(float)vpeak;
						}
						else if (usePeds){
	   						Sd1r[channel] = Sd1rGain[channel]*(((float)vpeak)-Sd1rPed[channel]);
							// Sd1r[channel] = Sd1rOffset2[channel]+Sd1rGain2[channel]*Sd1r[channel]; //recalibration using data for gain matching the channels
							}
						}	
	  				if (modid==5 && vpeak > adcThresh  && vpeak<3840){
	     				S3Hit = 1;
	     				if (!usePeds){
	    					Sd1s[channel] = Sd1sOffset[channel]+Sd1sGain[channel]*(float)vpeak;
						}
						else if (usePeds){
	   						Sd1s[channel] = Sd1sGain[channel]*(((float)vpeak)-Sd1sPed[channel]);
						}
					}
	
					// Upstream S3 detector. Has to be properly implemented! 
	  				if (modid==11  && vpeak> adcThresh && vpeak<3840){
	  				  	// if (channel==0) pdet->SSB =(float)vpeak;
	  				  	//SurEnergy = ((float)vpeak-SurOffset[channel+(modid-2)*32])*SurGain[channel+(modid-2)*32];
	   					Sur[channel] = Sd1rGain[channel]*(((float)vpeak)-Sd1rPed[channel]);
	  				}
	  				    
	  				if (modid==11  && vpeak > adcThresh && vpeak<3840){
	  				  	//SusEnergy = ((float)vpeak-SusOffset[channel+(modid-2)*32])*SusGain[channel+(modid-2)*32];
	   					Sus[channel] = Sd1rGain[channel]*(((float)vpeak)-Sd1rPed[channel]);
	  				}
	  
	  				if (modid>5 && modid<10 && vpeak>adcThresh  && vpeak<3840){
						YdADC=vpeak;
	    				//YdEnergy = YdOffset[channel+(modid-6)*32]+YdGain[channel+(modid-6)*32]*(float)vpeak;
	 					//Yd[channel+(modid-6)*32]=YdEnergy;
	 					Yd[channel+(modid-6)*32]=YdOffset[channel+(modid-6)*32]+YdGain[channel+(modid-6)*32]*(float)vpeak;
	    				//printf("YdEnergy: %f, vpeak: %d, gain: %f\n",YdEnergy, vpeak, YdGain[channel+(modid-6)*32]);
	    				if (channel<16) ydNo = (modid-6)*2+1; //Yd number
	    				if (channel>15) ydNo = (modid-6)*2+2;
	  				} //modid
	  
	 
	 				// Upstream YY detector. Has to be properly implemented! 
	  				if (modid>11 && modid<16 && vpeak >adcThresh  && vpeak<3840){  
	  				  	YuEnergy = ((float)vpeak-YuOffset[channel+(modid-6)*32])*YuGain[channel+(modid-6)*32];
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
	  				  	theta1 = TMath::RadToDeg()*(SdInnerRadius*(24.-channel-0.5)+SdOuterRadius*(channel+0.5))/24./SuDistance; //AS theta angle for Su (24 - number of rings)
	  				}
	  				
	  				if (modid==11){
	  				  	phi1 = (channel*180./32.); //AS phi angle for Su (32 - number of sectors)
	  				}
	
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
			else if (Sd2rEnergy2<Sd2r[i]){
		   		Sd2rEnergy2=Sd2r[i];
				Sd2rChannel2 = i;
			}
 		} //for

     	Sd2sEnergy=0; Sd2sEnergy2 =0; Sd2sChannel = -10000; Sd2sChannel2 =-10000;
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
  
  		//if (Sd2rEnergy >0)
  		// Sd2rEnergy = (Sd2rEnergy-Sd2rOffset[Sd2rChannel])*Sd2rGain[Sd2rChannel];
		//if (Sd2sEnergy >0)  
  		//Sd2sEnergy = (Sd2sEnergy-Sd2sOffset[Sd2sChannel])*Sd2sGain[Sd2sChannel];
  

 		Sd1rEnergy=0; Sd1rEnergy2 =0; Sd1rChannel = -10000; Sd1rChannel2 =-10000;
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

 		Sd1sEnergy=0; Sd1sEnergy2 =0; Sd1sChannel = -10000; Sd1sChannel2 =-10000;
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

    	YdEnergy=0; YdChannel = -10000; YdEnergy2=0; YdChannel2 =-10000, YdTChannel = -10000, YdTChannel2 = -10000;
    	for (int i =0; i< NYdChannels;i++){
	  		if(Yd[i]>YdEnergy){
           		YdEnergy2= YdEnergy;
	   			YdChannel2 = YdChannel;
	   			YdTChannel2 = YdTChannel;
	   			YdEnergy=Yd[i];
	   			YdChannel = i;
	   			YdTChannel = i - 128; //YdTChannel is the TDC channels which have max ADC 
	  		}
 			else if (Yd[i]>YdEnergy2){//if Yd[i] is between YdEnergy and YdEnergy2
	    		YdEnergy2=Yd[i];
	  			YdChannel2 = i;
	  			YdTChannel2 = i-128;
	    	} //if
      	} //for      
    
  		if (ascii) 
    		fprintf(ASCIIYY1," %d  %d %d %d %d \n",event.GetSerialNumber(), YdChannel+192, (int)YdEnergy,  YdChannel2+192, (int)YdEnergy2);
  
  		//if (YdEnergy>0)
    	// YdEnergy = YdEnergy*YdGain[YdChannel]+YdOffset[YdChannel];
 
	    spec_store_Edata[1][int(YdEnergy)]++; // = IRIS WebServer for IC =
		det->TYdEnergy = YdEnergy;
		if(YdChannel>-1) spec_store_hit_data[0][YdChannel]++;	
		det->TYdChannel = YdChannel;
		//here
		theta = TMath::RadToDeg()*atan((YdInnerRadius*(16.-YdChannel%16-0.5)+YdOuterRadius*(YdChannel%16+0.5))/16./YdDistance);
		det->TYdTheta= theta;
		length = YdDistance/cos(atan((YdInnerRadius*(16.-YdChannel%16-0.5)+YdOuterRadius*(YdChannel%16+0.5))/16./YdDistance));
		//	 det->TYdLength= length;
	   //hYdTheta -> Fill(theta);
	 
		//YYu
    	YuEnergy=0; YuChannel = -10000; YuEnergy2=0; YuChannel2 =-10000;
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
    		fprintf(ASCIIYY1," %d  %d %d %d %d \n",event.GetSerialNumber(), YuChannel+192, (int)YuEnergy,  YuChannel2+192, (int)YuEnergy2);

  		//if (YuEnergy>0)
  		//    // YuEnergy = YuEnergy*YuGain[YuChannel]+YuOffset[YuChannel];
  
		det->TYuEnergy = YuEnergy;
		det->TYuChannel = YuChannel;
		//here
		theta = TMath::RadToDeg()*atan((YdInnerRadius*(16.-YuChannel%16-0.5)+YdOuterRadius*(YuChannel%16+0.5))/16./YuDistance);
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
          		CsIChannel = i;
			}
      		else  if (CsIEnergy2<CsI[i]){
          		CsIEnergy2=CsI[i];
          		CsIChannel2 = i;
			}
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
				CsI2Channel2 = i;
			}
		} //for                                                                                                        

    	if (ascii)  fprintf(ASCIICsI," %d  %d %d %d %d \n",event.GetSerialNumber(), CsIChannel+32, (int)CsIEnergy,  CsIChannel2+32, (int)CsIEnergy2);

		if(CsI1Channel>=0) YdCsI1adcPID->Fill(CsI1Energy,YdEnergy*cos(det->TYdTheta*0.01745329));

       	//CsI1Energy = (CsI1Energy-CsI1Ped[CsI1Channel])*CsI1Gain[CsI1Channel];
     	if(CsI2Channel>=0) YdCsI2adcPID->Fill(CsI2Energy,YdEnergy*cos(det->TYdTheta*0.01745329));

		//printf("Ydnadc is %f\n",Ydnadc);
		hYdCsI2PID_uncalibrated->Fill(CsI2Energy,Ydnadc*cos(det->TYdTheta*0.01745329));
		if (ICnadc>=40 && ICnadc <100){ // condition of ICgate :Jaspreet
 			hYdCsI2PID_forNa->Fill(CsI2Energy,YdEnergy*cos(det->TYdTheta*0.01745329));
		}
		else if (ICnadc >=100 && ICnadc <350){
			hYdCsI2PID_forMg->Fill(CsI2Energy,YdEnergy*cos(det->TYdTheta*0.01745329));
		}
     
	    int gCsI1 = (YdChannel%16)/(16/NCsI1GroupRing);
		if(CsI1Channel>=0) CsI1Energy = (CsI1Energy-CsI1Ped[CsI1Channel])*CsI1Gain[gCsI1][CsI1Channel];
		int gCsI2 = (YdChannel%16)/(16/NCsI2Group);
     	if(CsI2Channel>=0) CsI2Energy = (CsI2Energy-CsI2Ped[CsI2Channel])*CsI2Gain[gCsI2][CsI2Channel];
     	if (1){// if((CsIChannel-16)/2==ydNo){
    		det->TCsIEnergy = CsI1Energy; //for filling the tree
    		det->TCsIChannel = CsI1Channel;
    	}
    	else {
    		det->TCsIEnergy = -100; //for filling the tree
    		det->TCsIChannel = -100;
    	}

		printf("etc:\n");     
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
	    	Sdtheta = TMath::RadToDeg()*(SdInnerRadius*(24.-Sd1rChannel-0.5)+SdOuterRadius*(Sd1rChannel+0.5))/24./Sd1Distance; //AS theta angle for Sd (24 - number of rings)
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
	  
	  	if ((fabs(Sd1sEnergy-Sd1rEnergy)<1.5) && (fabs(Sd2sEnergy-Sd2rEnergy)<1.5)) hSdPID->Fill(Sd2sEnergy,Sd1sEnergy);

		// if (elasticS3->IsInside(Sd2sEnergy,Sd1sEnergy)) hSd2rElHits->Fill(Sd2rChannel, 1.); //Sd2r hits with elastic gat

 		// if (elasticS3->IsInside(Sd2sEnergy,Sd1sEnergy)) hSd2sElHits->Fill(Sd2sChannel, 1.); //Sd2r hits with elastic gate

 		// if (elasticS3->IsInside(Sd2sEnergy,Sd1sEnergy)) hSd1rElHits->Fill(Sd1rChannel, 1.); //Sd2r hits with elastic gate

 		// if (elasticS3->IsInside(Sd2sEnergy,Sd1sEnergy)) hSd1sElHits->Fill(Sd1sChannel, 1.); //Sd2r hits with elastic gate

  		ydNo = YdChannel/16; //Yd number                                                                               
      
		hYdCsIPID2->Fill(CsI2Energy,YdEnergy*cos(det->TYdTheta*0.01745329));
 		
		if (YdEnergy!=0) hYdCsIETot->Fill(YdEnergy); // AS Note: Add cos theta after calculation of angles is added
 		
		hYdCsIPID1->Fill(CsI1Energy,YdEnergy*cos(det->TYdTheta*0.01745329));
     	
		//if (protons->IsInside(CsI1Energy,YdEnergy*cos(det->TYdTheta*1.74532925199432955e-02))){
       	//	hYdHitsProt->Fill(YdChannel,1.);
       	//}
	} //last bank
}

//---------------------------------------------------------------------------------
void HandleBOR_Mesytec(int run, int time, det_t* pdet)
{
	std::string CalibFile = "config_online.txt";
	if(CalibFile=="0") printf("No calibration file specified!\n\n");
	calMesy.Load(CalibFile);
	calMesy.Print();

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
 	Int_t Chan=-10000;
	double a,b,c;
	int g; //for ringwise calibration of CsI

//************** Calibrate IC, not yet implemented! *********************************
	FILE * pFile;
	FILE * pwFile;
 	char buffer[32];

	if(calMesy.fileIC==calMesy.installPath){
		printf("No calibration file for IC specified. Skipping IC calibration.\n\n");
	}
	else{
   		pFile = fopen(calMesy.fileIC.data(), "r");

		if (pFile == NULL) {
			perror ("Error opening file");
			fprintf(pwFile,"Error opening file");
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
	}
//*****************************************************************

//*************** Calibrate CsI1 ******************************
 	Chan=-10000;  
 	
 	pFile = fopen (calMesy.fileCsI1.data(), "r");
   	if (pFile == NULL) {
		perror ("Error opening file");
		fprintf(pwFile,"Error opening file");
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

	if (pFile == NULL) {
		perror ("Error opening file");
		fprintf(pwFile,"Error opening file");
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
	Chan=-10000;

	pFile = fopen (calMesy.fileSd2r.data(), "r");
	usePeds = 1;

   	if (pFile == NULL) {
		perror ("Error opening file");
		fprintf(pwFile,"Error opening file");
   	}  
 	else {
		printf("Reading Sd2r config file '%s'\n",calMesy.fileSd2r.data());
		// Skip first line
  		fscanf(pFile,"%s",buffer);
  		fscanf(pFile,"%s",buffer);
 		fscanf(pFile,"%s",buffer);

		for (int i =0;i<32;i++  ){
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
	Chan=-10000;
   	pFile = fopen (calMesy.fileSd2s.data(), "r");

 	if (pFile == NULL) {
		perror ("Error opening file");
		fprintf(pwFile,"Error opening file");
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
	Chan=-10000;
   	pFile = fopen(calMesy.fileSd1r.data(), "r");

	if (pFile == NULL) {
		perror ("Error opening file");
		fprintf(pwFile,"Error opening file");
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
	Chan=-10000;
   	pFile = fopen (calMesy.fileSd1s.data(), "r");

	if (pFile == NULL) {
		perror ("Error opening file");
		fprintf(pwFile,"Error opening file");
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
	Chan=-10000;
	if(calMesy.fileSur==calMesy.installPath){
		printf("No calibration file for Sur specified. Skipping Sur calibration.\n");
	}
	else{
   		pFile = fopen(calMesy.fileSur.data(), "r");

		if (pFile == NULL) {
			perror ("Error opening file");
			fprintf(pwFile,"Error opening file");
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
	}
//************************************************************************

//**************** Calibrate Su sectors ****************************************
	Chan=-10000;
	if(calMesy.fileSus==calMesy.installPath){
		printf("No calibration file for Sus specified. Skipping Sus calibration.\n");
	}
	else{
   		pFile = fopen (calMesy.fileSus.data(), "r");

		if (pFile == NULL) {
			perror ("Error opening file");
			fprintf(pwFile,"Error opening file");
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
	}
//************************************************************************

//**************** Calibrate Yd ****************************************
	Chan=-10000;
   	pFile = fopen (calMesy.fileYd.data(), "r");

	if (pFile == NULL) {
  		perror ("Error opening file fro Yd");
  		fprintf(pwFile,"Error opening file for Yd");
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
	Chan=-10000;
	if(calMesy.fileYu==calMesy.installPath){
		printf("No calibration file for Yu specified. Skipping Yu calibration.\n");
	}
	else{
   		pFile = fopen (calMesy.fileYu.data(), "r");

		if (pFile == NULL) {
  			perror ("Error opening file fro Yu");
  			fprintf(pwFile,"Error opening file for Yu");
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
				hMes_P[channel] = new TH1D(label, sig, binlimit1, 0, binlimit1);
				printf("Booking TH1D %s \n", label);
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
  				hIC[channel] = new TH1D(label, sig, binlimit1, 0, binlimit1);
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
	 
 		hSd1r[0] = (TH1D*)gDirectory->Get("Sd1r00");
		if (hSd1r[0] == 0) {
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

 		hSd1s[0] = (TH1D*)gDirectory->Get("Sd1s00");
   		if (hSd1s[0] == 0) {
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
 
		// Make an SD2R directory and cd to it.
		TDirectory* Sd2r_dir = gOutputFile->mkdir("Sd2r");      
		Sd2r_dir->cd();
		
		hSd2r[0] = (TH1D*)gDirectory->Get("Sd2r00");
		if (hSd2r[0] == 0) {
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

 		hSus[0] = (TH1D*)gDirectory->Get("Sus00");
   		if (hSus[0] == 0) {
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
 
		hYd[0] = (TH1D*)gDirectory->Get("Yd00");
   		if (hYd[0] == 0) {
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
   
       	printf(" in Mesytec BOR... Booking PID histos\n");
       	sprintf(label,"SdPID");
	 	hSdPID= new TH2F( "SdPID", "SDPID", 512, 0, 100, 512, 0, 40);
	 	printf("Booking TH2F %s \n", label);
       
	 	hYdCsIPID2 = new TH2F("YdCsIPID2", "YdCsIPID2", 512, 0, 40, 512, 0, binlimitYd);
	 	printf("Booking TH2F %s \n", label);

	 	hYdCsIPID1 = new TH2F("YdCsIPID1", "YdCsIPID1", 500, 0, 40, 512, 0, 16.);
        printf("Booking TH2F %s \n", label);

		YdCsI1adcPID = new TH2F("YdCsI1PIDadc", "YdCsI1PIDadc", 500, 0, 4000, 512, 0, 16.);
        printf("Booking TH2F %s \n", label);
		YdCsI2adcPID = new TH2F("YdCsI2PIDadc", "YdCsI2PIDadc", 500, 0, 4000, 512, 0, 16.);
        printf("Booking TH2F %s \n", label);

		hYdCsI2PID_forNa = new TH2F("YdCsI2PID_forNa", "YdCsI2PID_forNa", 500, 0, 4000, 512, 0, 16.);
        printf("Booking TH2F %s \n", label);

		hYdCsI2PID_forMg = new TH2F("YdCsI2PID_forMg", "YdCsI2PID_forMg", 500, 0, 4000, 512, 0, 16.);
        printf("Booking TH2F %s \n", label);

		hYdCsI2PID_uncalibrated = new TH2F("YdCsI2PID_uncalibrated", "YdCsI2PID_uncalibrated", 500, 0, 4000, 500, 0, 4000.);
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

  	} // if(gOutputFile)

	// = IRIS WebServer for IC =
	// Zero the web spectra at BOR
	memset(spec_store_address,0,sizeof(spec_store_address));
	memset(spec_store_type,0,sizeof(spec_store_type));
	memset(spec_store_Ename,0,sizeof(spec_store_Ename));
	//memset(spec_store_Tname,0,sizeof(spec_store_Tname));
	memset(spec_store_Pname,0,sizeof(spec_store_Pname));
	memset(spec_store_hit_name,0,sizeof(spec_store_hit_name));
	memset(spec_store_Edata,0,sizeof(spec_store_Edata));
	//memset(spec_store_Tdata,0,sizeof(spec_store_Tdata));
	memset(spec_store_Pdata,0,sizeof(spec_store_Pdata));
	memset(spec_store_hit_data,0,sizeof(spec_store_hit_data));

	sprintf(spec_store_Ename[0],"ICEnergy");
	sprintf(spec_store_Ename[1],"YdEnergy");
	sprintf(spec_store_Ename[2],"CsI1Energy");
	sprintf(spec_store_Ename[3],"CsI2Energy");
	sprintf(spec_store_Ename[4],"Sd1rEnergy");
	sprintf(spec_store_Ename[5],"Sd1sEnergy");
	sprintf(spec_store_Ename[6],"Sd2rEnergy");
	sprintf(spec_store_Ename[7],"Sd2sEnergy");
	sprintf(spec_store_Ename[8],"SSBEnergy");
	
	sprintf(spec_store_hit_name[0],"YdHits");

	for (int chn=0;chn<Nchannels;chn++) {
		if (chn<32)
			sprintf(spec_store_Pname[chn],"ICCh%02dADC",chn);
		else if(chn>=32 && chn <64) 
			sprintf(spec_store_Pname[chn],"CsICh%02dADC",chn-32);
		else if(chn>=64 && chn <128) 
			sprintf(spec_store_Pname[chn],"Sd2Ch%02dADC",chn-64);
		else if(chn>=128 && chn <192) 
			sprintf(spec_store_Pname[chn],"Sd1Ch%02dADC",chn-128);
		else if(chn>=192 && chn < 320) 
			sprintf(spec_store_Pname[chn],"YdCh%03dADC",chn-192);
		else if(chn>=320 && chn < 384) 
			sprintf(spec_store_Pname[chn],"SuCh%02dADC",chn-320);
		else if(chn>=384 && chn < 512) 
			sprintf(spec_store_Pname[chn],"YuCh%03dADC",chn-384);
	}

	for(int jj=0; jj<128; jj++)
	{
		//sprintf(spec_store_Ename[jj],"YdCh%03dEnergy",jj);
	}

	memset(spec_store_Edata,0,sizeof(spec_store_Edata));
	memset(spec_store_Pdata,0,sizeof(spec_store_Pdata));
	memset(spec_store_hit_data,0,sizeof(spec_store_hit_data));
	// =========================
 
//************************************************************************
}

void HandleEOR_Mesytec(int run, int time)
{
  printf(" in Mesytec EOR\n");
}
