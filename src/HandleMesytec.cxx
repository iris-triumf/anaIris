// ROOT analyzer
// detector handling

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
#include <TNtuple.h>
#include <TFolder.h>
#include <TMath.h>
#include <TCutG.h>
#include <TRandom3.h>

#include "Globals.h"
#include "HandleMesytec.h"
#include "CalibMesytec.h"
#include "geometry.h"
#include "web_server.h" // = IRIS WebServer =

int gMesytecnitems;
const int Nchannels=512;
const int NICChannels=32;
const int NCsI2Group=16;
const int NCsI1GroupRing=16;
const int NCsIChannels=16;
const int NSd1rChannels=32;
const int NSd1sChannels=32;
const int NSd2rChannels=32;
const int NSd2sChannels=32;
const int NSurChannels=32;
const int NSusChannels=32;
const int NYdChannels=128;
const int NYuChannels=128;
const int NZdxChannels = 16;
const int NZdyChannels = 16;
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

int usePeds = 0; //using pedestals instead of offsets for Silicon detectors AS
//AS Ion Chamber
float IC[16], ICEnergy=0; //Dummy for IC energy
float ICEnergy1=0, ICEnergy2=0, ICEnergy3=0, ICEnergy4=0; //Dummy for IC energy
float ICGain[NICChannels]={1.};
float ICOffset[NICChannels]={0.};
float ICPed[NICChannels]={0.};
float ICnadc = 0;  

//AS CsI
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
float Sd2r[NSd2rChannels];
float Sd2rEnergy=0, Sd2rEnergy2= 0; //Dummy for Sd2r energy
int Sd2rChannel,  Sd2rChannel2; // channel with the greatest value
float Sd2rGain[NSd2rChannels]={1.};
float Sd2rOffset[NSd2rChannels]={0.};
float Sd2rPed[NSd2rChannels]={0.};

Bool_t S3Hit;

float Sd2s[NSd2sChannels];
float Sd2sEnergy=0,  Sd2sEnergy2= 0; //Dummy for Sd2s energy  
int Sd2sChannel, Sd2sChannel2; // channel with the greatest value
float Sd2sGain[NSd2sChannels]={1.};
float Sd2sOffset[NSd2sChannels]={0.};
float Sd2sPed[NSd2sChannels]={0.};

float Sd1r[NSd1rChannels];
float Sd1rEnergy=0, Sd1rEnergy2= 0; //Dummy for Sd1r energy
int Sd1rChannel,  Sd1rChannel2; // channel with the greatest value
float Sd1rGain[NSd1rChannels]={1.};
float Sd1rOffset[NSd1rChannels]={0.};
float Sd1rPed[NSd1rChannels]={0.};

float Sd1s[NSd1sChannels];
float Sd1sEnergy=0,  Sd1sEnergy2=0; //Dummy for Sd1s energy      
int Sd1sChannel, Sd1sChannel2; // channel with the greatest value                                                                                    
float Sd1sGain[NSd1sChannels]={0.};
float Sd1sOffset[NSd1sChannels]={0.};
float Sd1sPed[NSd1sChannels]={0.};

float Sur[NSurChannels];
float SurEnergy=0, SurEnergy2=0; //Dummy for Sur energy
int SurChannel, SurChannel2; // channel with the greatest value
float SurGain[NSurChannels]={1.};
float SurOffset[NSurChannels]={0.};
float SurPed[NSurChannels]={0.};

float Sus[NSusChannels];
int SusChannel, SusChannel2; // channel with the greatest value
float SusEnergy=0, SusEnergy2=0; //Dummy for Sus energy                                                                                          
float SusGain[NSusChannels]={1.};
float SusOffset[NSusChannels]={0.};
float SusPed[NSusChannels]={0.};

//AS YYd
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

//SSB
float SSBEnergy = 0;
float SSBOffset=0;
float SSBGain=0;

float ScintEnergy = 0;

//YYu  
float Yu[NYuChannels] ={0};
float YuEnergy=0, YuEnergy2=0;//Dummy for Yu energy
int YuChannel, YuChannel2; // channel with the greatest value 
float YuGain[NYuChannels]={1.};
float YuOffset[NYuChannels]={0.};
float YuPed[NYuChannels]={0.};

int ZdxADC[NZdxChannels];
float Zdx[NZdxChannels];
float ZdxEnergy, ZdxEnergy2;
int ZdxChannel, ZdxChannel2;
float ZdxGain[NZdxChannels]={1.};
float ZdxOffset[NZdxChannels]={0.};
float ZdxPed[NZdxChannels]={0.};

int ZdyADC[NZdyChannels];
float Zdy[NZdyChannels];
float ZdyEnergy, ZdyEnergy2;
int ZdyChannel, ZdyChannel2;
float ZdyGain[NZdyChannels]={1.};
float ZdyOffset[NZdyChannels]={0.};
float ZdyPed[NZdyChannels]={0.};


//TRIFIC
float TRIFICEnergy1 = 0, TRIFICEnergy2 = 0, TRIFICEnergy3 = 0;
float TRIFICGain=0;
float TRIFICOffset=0;

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
	for (int j=0; j<NCsIChannels; j++) CsI[j] = 0;
	for (int j=0; j<NCsIChannels; j++) CsI1[j] = 0;
	for (int j=0; j<NCsIChannels; j++) CsI2[j] = 0;
	
	for (int j=0; j<NSd1rChannels; j++) Sd1r[j] = 0;
	for (int j=0; j<NSd1sChannels; j++) Sd1s[j] = 0;
	for (int j=0; j<NSd2rChannels; j++) Sd2r[j] = 0;
	for (int j=0; j<NSd2sChannels; j++) Sd2s[j] = 0;
	for (int j=0; j<NSurChannels; j++) Sur[j] = 0;
	for (int j=0; j<NSusChannels; j++) Sus[j] = 0;
	
	for (int j=0; j<NYdChannels; j++) Yd[j] = 0;
	for (int j=0; j<NYuChannels; j++) Yu[j] = 0;
	
	for (int j=0; j<NZdxChannels; j++) Zdx[j] = 0;
	for (int j=0; j<NZdyChannels; j++) Zdy[j] = 0;

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

void HandleMesytec(TMidasEvent& event, void* ptr, int nitems, int bank, det_t *det, hist_t *hist)
{
	uint32_t *data;
	int    i, debug = 0, debug1 = 0;
	//int eventId = event.GetEventId();
	
	// clearDetectors();
	
	data = (uint32_t *) ptr;
	if (hist->hADC[0] == NULL) HandleBOR_Mesytec(gRunNumber, 0, det, hist);    
	
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
	  				
	  				hist->hADC[channel+(modid*32)]->Fill((float)vpeak , 1.);
					spec_store_adcData[channel+(modid*32)][vpeak]++;
	  				if ((modid==0) && (vpeak > adcThresh) && (vpeak<3840)){
	    				//printf("IC1\n");
						//AS Fill histogram
 						ICnadc = (float)vpeak; 
						IC[channel] = (float)vpeak;
						ICEnergy = ((float)vpeak-ICPed[channel])*ICGain[channel];
						//printf("IC2: %d %d\n", channel,vpeak);
	    				hist->hIC[channel]->Fill(IC[channel], 1.); //IC
	    				//printf("IC3\n");
						spec_store_eData[15][int(ICEnergy)]++; // = IRIS WebServer =
	    				//printf("IC4\n");
						//spec_store_eData[15][int(ICEnergy*scalingIC)]++; // = IRIS WebServer =
						if (channel==19){
	          				ScintEnergy = float(vpeak);// *SSBGain + SSBOffset;
							spec_store_energyData[9][int(ScintEnergy)]++; // = IRIS WebServer =
						}
	    				//printf("IC5\n");

	        			if (channel==15){
	    					det->TICEnergy = IC[15];
						}
	        			if (channel==31){
	          				SSBEnergy = float(vpeak);// *SSBGain + SSBOffset;
							spec_store_energyData[8][int(SSBEnergy)]++; // = IRIS WebServer =
						}
	        			
						if (channel==21){
	          				TRIFICEnergy1 = float(vpeak);
						}
	        			if (channel==22){
	          				TRIFICEnergy2 = float(vpeak);
						}
	        			if (channel==23){
	          				TRIFICEnergy3 = float(vpeak);
						}
	    				//printf("IC6\n");
	  				}
	  
	  				if (modid==1 && vpeak > adcThresh && vpeak<3840){
	    				if (channel<16){
	    					CsI1[channel] = (float)vpeak;
							hist->hCsI1[channel]->Fill(CsI1[channel],1.);
							spec_store_eData[channel+32][int(CsI1[channel])]++; // = IRIS WebServer =
	    	    		}	    
		  				else if (channel>=16){
	    					CsI2[channel-16] = (float)vpeak;
							hist->hCsI2[channel-16]->Fill(CsI2[channel-16],1.);
							spec_store_eData[channel+32][int(CsI2[channel-16])]++; // = IRIS WebServer =
	    	    		}	    
	  				}
	
	  				if (modid==2 && vpeak > adcThresh && vpeak<3840){
	 					S3Hit = 1; 	    
	 					if (!usePeds){
	    					Sd2r[channel] = Sd2rOffset[channel]+Sd2rGain[channel]*(float)vpeak;
							if(channel<16) Zdy[channel] = ZdyOffset[channel]+ZdyGain[channel]*(float)vpeak;
						}
	 					else if (usePeds){
	   						Sd2r[channel] = Sd2rGain[channel]*(((float)vpeak)-Sd2rPed[channel]);
							if(channel<16) Zdy[channel] = ZdyGain[channel]*(((float)vpeak)-ZdyPed[channel]);
							} 
						hist->hSd2r[channel]->Fill(Sd2r[channel],1.);
						if(channel<16) hist->hZdy[channel]->Fill(Zdy[channel],1.);
						spec_store_eData[channel+64][int(Sd2r[channel]*scalingSd2)]++; // = IRIS WebServer =
					}
	  
	  				if (modid==3 && vpeak > adcThresh && vpeak<3840){
	    				S3Hit = 1;
	    				if (!usePeds){	
	    					Sd2s[channel] = Sd2sOffset[channel]+Sd2sGain[channel]*(float)vpeak;
							if(channel<8) Zdx[7-channel] = ZdxOffset[7-channel]+ZdxGain[7-channel]*(float)vpeak;
							if(channel>7 && channel<16) Zdx[channel] = ZdxOffset[channel]+ZdxGain[channel]*(float)vpeak;
						}
	 					else if (usePeds){
	   						Sd2s[channel] = Sd2sGain[channel]*(((float)vpeak)-Sd2sPed[channel]);
							if(channel<8) Zdx[7-channel] = ZdxGain[7-channel]*(((float)vpeak)-ZdxPed[7-channel]);
							if(channel>7 && channel<16) Zdx[channel] = ZdxGain[channel]*(((float)vpeak)-ZdxPed[channel]);
							}
						hist->hSd2s[channel]->Fill(Sd2s[channel],1.);
						if(channel<16) hist->hZdx[channel]->Fill(Zdx[channel],1.);
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
						hist->hSd1r[channel]->Fill(Sd1r[channel],1.);
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
						hist->hSd1s[channel]->Fill(Sd1s[channel],1.);
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
						hist->hSur[channel]->Fill(Sur[channel],1.);
						spec_store_eData[channel+320][int(Sur[channel]*scalingSu)]++; // = IRIS WebServer =
	  				}
	  				    
	  				if (modid==11  && vpeak > adcThresh && vpeak<3840){
	  				  	if (!usePeds){
	    					Sus[channel] = SusOffset[channel]+SusGain[channel]*(float)vpeak;
						}
						else if (usePeds){
	   						Sus[channel] = SusGain[channel]*(((float)vpeak)-SusPed[channel]);
						}
						hist->hSus[channel]->Fill(Sus[channel],1.);
						spec_store_eData[channel+352][int(Sus[channel]*scalingSu)]++; // = IRIS WebServer =
	  				}
	  
	  				if (modid>5 && modid<10 && vpeak>adcThresh  && vpeak<3840){
	 					Yd[channel+(modid-6)*32]=YdOffset[channel+(modid-6)*32]+YdGain[channel+(modid-6)*32]*(float)vpeak;
						YdADC[channel+(modid-6)*32]=vpeak;
	    				if (channel<16) ydnumber = (modid-6)*2+1; //Yd number
	    				if (channel>15) ydnumber = (modid-6)*2+2;
						hist->hYd[channel]->Fill(Yd[channel+(modid-6)*32],1.);
						spec_store_eData[channel+(modid-6)*32+192][int(Yd[channel+(modid-6)*32]*scalingYd)]++; // = IRIS WebServer =
	  				} //modid
	  
	 
	 				// Upstream YY detector. Has to be properly implemented! 
	  				if (modid>11 && modid<16 && vpeak >adcThresh  && vpeak<3840){  
	 					Yu[channel+(modid-12)*32]=YuOffset[channel+(modid-12)*32]+YuGain[channel+(modid-12)*32]*(float)vpeak;
						hist->hYu[channel]->Fill(Yu[channel+(modid-12)*32],1.);
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
	    				hist->hCsI1Hits->Fill(channel, 1.); // CsI hit  
						spec_store_hitData[0][channel]++;	
					}
	    			if (modid==1 && vpeak>adcThresh&&channel>15){	 
	    				hist->hCsI2Hits->Fill(channel-16, 1.); //CsI hits  
						spec_store_hitData[1][channel-16]++;	
					}

	    			if (modid==2 && vpeak>adcThresh){	 
	    				hist->hSd2rHits->Fill(channel, 1.); //Sd2r hits  
						if(channel<16) hist->hZdyHits->Fill(channel,1.);
						spec_store_hitData[4][channel]++;	
					}
	    			if (modid==3 && vpeak>adcThresh)	 {
	    				hist->hSd2sHits->Fill(channel, 1.); //Sd2s hits
						if(channel<8) hist->hZdxHits->Fill(7-channel,1.);
						if(channel>7&&channel<16) hist->hZdxHits->Fill(channel,1.);
		   				spec_store_hitData[5][channel]++;
	  				}

 					if (modid==4 && vpeak>adcThresh)	 {
	    				hist->hSd1rHits->Fill(channel, 1.); //Sd1r hits
		   				spec_store_hitData[2][channel]++;
					}	  
 					if (modid==5 && vpeak>adcThresh)	{	     
	    				hist->hSd1sHits->Fill(channel, 1.); //Sd1s hits 
		   				spec_store_hitData[3][channel]++;
 					}

	  				if (modid==10 && vpeak>adcThresh){	 
	    				hist->hSurHits->Fill(channel, 1.); //Sur hits 
		   				spec_store_hitData[15][channel]++;
	 				}
	  				if (modid==11 && vpeak>adcThresh){	 
	    				hist->hSusHits->Fill(channel, 1.); //Sus hits
		   				spec_store_hitData[16][channel]++;
					}

					if (modid==6 && vpeak>adcThresh && channel<16){
					  	hist->hYdHits[0]->Fill(channel,1.);
						spec_store_hitData[6][channel]++;	
					}
					if (modid==6 && vpeak>adcThresh  && channel>15){
					  	hist->hYdHits[1]->Fill(channel-16,1.);
						spec_store_hitData[7][channel-16]++;	
					}
					if (modid==7 && vpeak>adcThresh && channel<16){
					  	hist->hYdHits[2]->Fill(channel,1.);
						spec_store_hitData[8][channel]++;	
					}
					if (modid==7 && vpeak>adcThresh && channel>15){
					  	hist->hYdHits[3]->Fill(channel-16,1.);
						spec_store_hitData[9][channel-16]++;	
					}
					if (modid==8 && vpeak>adcThresh && channel<16){
					  	hist->hYdHits[4]->Fill(channel,1.);
						spec_store_hitData[10][channel]++;	
					}
					if (modid==8 && vpeak>adcThresh && channel>15){
					  	hist->hYdHits[5]->Fill(channel-16,1.);
						spec_store_hitData[11][channel-16]++;	
					}
					if (modid==9 && vpeak>adcThresh && channel<16){
					  	hist->hYdHits[6]->Fill(channel,1.);
						spec_store_hitData[12][channel]++;	
					}
					if (modid==9 && vpeak>adcThresh && channel>15){
					  	hist->hYdHits[7]->Fill(channel-16,1.);
						spec_store_hitData[13][channel-16]++;	
					}
					if (modid>5 && modid<10 && vpeak>adcThresh && channel<16){
					  	hist->hYdHitsAll->Fill(channel,1.);
						spec_store_hitData[14][channel]++;	
					}
					if (modid>5 && modid<10 && vpeak>adcThresh && channel>15){
  						hist->hYdHitsAll->Fill(channel-16,1.);
						spec_store_hitData[14][channel-16]++;	
					}
					
					if (modid==12 && vpeak>adcThresh && channel<16){
					  	hist->hYuHits[0]->Fill(channel,1.);
						spec_store_hitData[17][channel]++;	
					}
					if (modid==12 && vpeak>adcThresh  && channel>15){
					  	hist->hYuHits[1]->Fill(channel-16,1.);
						spec_store_hitData[18][channel-16]++;	
					}
					if (modid==13 && vpeak>adcThresh && channel<16){
					  	hist->hYuHits[2]->Fill(channel,1.);
						spec_store_hitData[19][channel]++;	
					}
					if (modid==13 && vpeak>adcThresh && channel>15){
					  	hist->hYuHits[3]->Fill(channel-16,1.);
						spec_store_hitData[20][channel-16]++;	
					}
					if (modid==14 && vpeak>adcThresh && channel<16){
					  	hist->hYuHits[4]->Fill(channel,1.);
						spec_store_hitData[21][channel]++;	
					}
					if (modid==14 && vpeak>adcThresh && channel>15){
					  	hist->hYuHits[5]->Fill(channel-16,1.);
						spec_store_hitData[22][channel-16]++;	
					}
					if (modid==15 && vpeak>adcThresh && channel<16){
					  	hist->hYuHits[6]->Fill(channel,1.);
						spec_store_hitData[23][channel]++;	
					}
					if (modid==15 && vpeak>adcThresh && channel>15){
					  	hist->hYuHits[7]->Fill(channel-16,1.);
						spec_store_hitData[24][channel-16]++;	
					}

					if (modid>11 && modid<16 && vpeak>adcThresh && channel<16){
					  	hist->hYuHitsAll->Fill(channel,1.);
						spec_store_hitData[25][channel]++;	
					}
					if (modid>11 && modid<16 && vpeak>adcThresh && channel>15){
  						hist->hYuHitsAll->Fill(channel-16,1.);
						spec_store_hitData[25][channel-16]++;	
					}


	  				break;
			} // switch
	  	} // for loop
	} // nitems != 0

	//After looping over banks, fill the ascii files
    //if (modid>5 && modid<10) {// check last bank
    if (bank==5) {// check last bank
      	det->EventNumber = event.GetSerialNumber();
   
		if(IC[3]>0.&&IC[7]>0.&&IC[11]>0.&&IC[15]>0.) hist->hICSum->Fill(IC[3]+IC[7]+IC[11]+IC[15]);	
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
		if(Sd2rEnergy>0){
			hist->hSd2rEnergy -> Fill(Sd2rEnergy);
		}

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
		if(Sd2sEnergy>0){
			hist->hSd2sEnergy -> Fill(Sd2sEnergy);
		}


  		if (ascii) 
    		fprintf(ASCIISd2," %d  %d %d %d %d %d %d %d %d \n",event.GetSerialNumber(), Sd2rChannel+64, (int)Sd2rEnergy,  Sd2rChannel2+64, (int)Sd2rEnergy2,  Sd2sChannel+96, (int)Sd2sEnergy,  Sd2sChannel2+96, (int)Sd2sEnergy2);
  
	    spec_store_energyData[6][int(Sd2rEnergy*scalingSd2)]++; // = IRIS WebServer =
	    spec_store_energyData[7][int(Sd2sEnergy*scalingSd2)]++; // = IRIS WebServer =

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
		if(Sd1rEnergy>0){
			hist->hSd1rEnergy -> Fill(Sd1rEnergy);
			hist->hSdTheta -> Fill(theta);
	   		hist->hSdETheta -> Fill(theta,Sd1rEnergy);
		}

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
		if(Sd1sEnergy>0){
			hist->hSd1sEnergy -> Fill(Sd1sEnergy);
		}


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
		if(SurEnergy>0&&SurChannel>-1){
			theta = TMath::RadToDeg()*atan((geoM.SdInnerRadius*(SurChannel+0.5)+geoM.SdOuterRadius*(24.-SurChannel-0.5))/24./geoM.SuDistance)+180.;
			det->TSuTheta= theta;
			hist->hSurEnergy -> Fill(SurEnergy);
			hist->hSuTheta -> Fill(theta);
			hist->hSuETheta -> Fill(theta,SurEnergy);
		}
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
		if(SusEnergy>0){
			hist->hSusEnergy -> Fill(SusEnergy);
		}

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
		det->TYdnadc = Ydnadc;
		if(YdChannel>-1){
			det->TYdChannel = YdChannel;
			det->TYdNo = YdNo;
		}
  		if(YdChannel2>-1){
			det->TYdChannel2 = YdChannel2;
			det->TYdNo2 = YdNo2;
		}
  		
		if(YdEnergy>0&&YdChannel%16>-1){
			theta = TMath::RadToDeg()*atan((geoM.YdInnerRadius*(16.-YdChannel%16-0.5)+geoM.YdOuterRadius*(YdChannel%16+0.5))/16./geoM.YdDistance);
			det->TYdTheta= theta;
			length = geoM.YdDistance/cos(atan((geoM.YdInnerRadius*(16.-YdChannel%16-0.5)+geoM.YdOuterRadius*(YdChannel%16+0.5))/16./geoM.YdDistance));
			hist->hYdEnergy -> Fill(YdEnergy);
			hist->hYdTheta -> Fill(theta);
			hist->hYdETheta -> Fill(theta,YdEnergy);
		}
	 
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
		if(YuEnergy>0&&YuChannel%16>-1){
			theta = TMath::RadToDeg()*atan((geoM.YdInnerRadius*(16.-YuChannel%16-0.5)+geoM.YdOuterRadius*(YuChannel%16+0.5))/16./geoM.YuDistance)+180.;
			det->TYuTheta= theta;
			hist->hYuEnergy -> Fill(YuEnergy);
			hist->hYuTheta -> Fill(theta);
			hist->hYuETheta -> Fill(theta,YuEnergy);
		}
  		
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
		if(CsI1Energy>0) hist->hCsI1Energy->Fill(CsI1Energy);

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
		if(CsI2Energy>0) hist->hCsI2Energy->Fill(CsI2Energy);
	 
		spec_store_energyData[2][int(CsI1Energy*scalingCsI)]++; // = IRIS WebServer =
	    spec_store_energyData[3][int(CsI2Energy*scalingCsI)]++; // = IRIS WebServer =

    		if (ascii)  fprintf(ASCIICsI," %d  %d %d %d %d \n",event.GetSerialNumber(), CsIChannel+32, (int)CsIEnergy,  CsIChannel2+32, (int)CsIEnergy2);

		//Zdx
    	ZdxEnergy=0; ZdxChannel = -1; ZdxEnergy2=0; ZdxChannel2 =-1;
    	for (int i =0; i< NZdxChannels;i++){
          	if(Zdx[i]>ZdxEnergy){
  				ZdxEnergy2= ZdxEnergy;
           		ZdxChannel2 = ZdxChannel;
          		ZdxEnergy=Zdx[i];
          		ZdxChannel = i;
			}
 			else if (Zdx[i]>ZdxEnergy2){//if Zdx[i] is between ZdxEnergy and ZdxEnergy2
            	ZdxEnergy2=Zdx[i];
          		ZdxChannel2 = i;
			} //if
      	} //for      

		det->TZdxEnergy = ZdxEnergy;
		det->TZdxChannel = ZdxChannel;
		if(ZdxEnergy>0&&ZdxChannel<16){
			hist->hZdxEnergy -> Fill(ZdxEnergy);
		}
 	
		//Zdy
    	ZdyEnergy=0; ZdyChannel = -1; ZdyEnergy2=0; ZdyChannel2 =-1;
    	for (int i =0; i< NZdyChannels;i++){
          	if(Zdy[i]>ZdyEnergy){
  				ZdyEnergy2= ZdyEnergy;
           		ZdyChannel2 = ZdyChannel;
          		ZdyEnergy=Zdy[i];
          		ZdyChannel = i;
			}
 			else if (Zdy[i]>ZdyEnergy2){//if Zdy[i] is between ZdyEnergy and ZdyEnergy2
            	ZdyEnergy2=Zdy[i];
          		ZdyChannel2 = i;
			} //if
      	} //for      

		det->TZdyEnergy = ZdyEnergy;
		det->TZdyChannel = ZdyChannel;
		if(ZdyEnergy>0&&ZdyChannel<16){
			hist->hZdyEnergy -> Fill(ZdyEnergy);
		}
    	
		if(ZdxEnergy>0&&ZdxChannel<16&&ZdyEnergy>0&&ZdyChannel<16){
			hist->hZdHits -> Fill(ZdxChannel,ZdyChannel);
			float xpos = (ZdxChannel-8)*3.+1.5;
			float ypos = (ZdyChannel-8)*3.+1.5;
			float radius = TMath::Sqrt(xpos*xpos+ypos*ypos);
			phi = TMath::ATan2(ypos,xpos)*TMath::RadToDeg();
			theta = TMath::ATan2(radius,(geoM.Sd1Distance+29.3))*TMath::RadToDeg();
			hist->hZdPhi -> Fill(phi);
			hist->hZdTheta -> Fill(theta);
			hist->hZdETheta -> Fill(theta,ZdxEnergy);
		}
		
		if(YdChannel>=0&&YdEnergy>0.) hist->hYdRange->Fill(Ydnadc,YdEnergy);
		if(CsI1Channel>=0&&CsI1Energy>0.) hist->hCsI1Range->Fill(CsI1nadc,CsI1Energy);
		if(CsI2Channel>=0&&CsI2Energy>0.) hist->hCsI2Range->Fill(CsI2nadc,CsI2Energy);

		if(YdChannel>=0&&CsI1Channel>=0) hist->hYdCsI1Corr->Fill(YdNo,CsI1Channel);
		if(YdChannel>=0&&CsI2Channel>=0) hist->hYdCsI2Corr->Fill(YdNo,CsI2Channel);
    	
		det->TCsIEnergy = CsI1Energy; //for filling the tree
    	det->TCsIChannel = CsI1Channel;

		//printf("etc:\n");     
     	if (1){// if((CsIChannel-16)/2==ydnumber){
    		det->TCsI1Energy = CsI1Energy; //for filling the tree
    		det->TCsI1Channel = CsI1Channel;
    		det->TCsI1nadc = CsI1nadc;
    	}

   		if (1){// if((CsIChannel-16)/2==ydnumber){
    		det->TCsI2Energy = CsI2Energy; //for filling the tree
    		det->TCsI2Channel = CsI2Channel;
    		det->TCsI2nadc = CsI2nadc;
    	}
    	//printf("modid: %d , CsIEnergy: %f \n",modid, CsIEnergy);

 		//AS angles

	  	if (Sd1rEnergy>0){
	    	Sdtheta = TMath::RadToDeg()*(geoM.SdInnerRadius*(24.-Sd1rChannel-0.5)+geoM.SdOuterRadius*(Sd1rChannel+0.5))/24./geoM.Sd1Distance; //AS theta angle for Sd (24 - number of rings)
	    	hist->hSdTheta->Fill(Sdtheta);
	  	}
	   	if (Sd1rEnergy>0){
	    	Sdphi = (Sd1sChannel*180./32.); //AS phi angle for Sd (32 - number of sectors)
	    	hist->hSdPhi->Fill(Sdphi);
	   	}

  		//Fill phitheta and E-theta histogram	  
	  	if (Sdtheta>0){
	    	hist->hSdPhiTheta->Fill(theta,phi);
	    	hist->hSdETheta->Fill(Sdtheta,Sd2sEnergy+Sd1sEnergy);
	  	}
	 		
		// TRIFIC histos
		if(TRIFICEnergy1>0) hist->hTRIFIC_1->Fill(TRIFICEnergy1); 
		if(TRIFICEnergy2>0) hist->hTRIFIC_2->Fill(TRIFICEnergy2); 
		if(TRIFICEnergy3>0) hist->hTRIFIC_3->Fill(TRIFICEnergy3); 
		if(TRIFICEnergy1>0&&TRIFICEnergy2>0) hist->hTRIFIC_1v2->Fill(TRIFICEnergy2,TRIFICEnergy1); 
		if(TRIFICEnergy1>0&&TRIFICEnergy3>0) hist->hTRIFIC_1v3->Fill(TRIFICEnergy3,TRIFICEnergy1); 
		if(TRIFICEnergy2>0&&TRIFICEnergy3>0) hist->hTRIFIC_2v3->Fill(TRIFICEnergy3,TRIFICEnergy2); 
		if(TRIFICEnergy1>0&&TRIFICEnergy2>0&&TRIFICEnergy3>0) hist->hTRIFIC_12v3->Fill(TRIFICEnergy3,TRIFICEnergy1+TRIFICEnergy2);
		if(TRIFICEnergy1>0&&TRIFICEnergy2>0) hist->hTRIFIC_1v23->Fill(TRIFICEnergy2+TRIFICEnergy3,TRIFICEnergy1);

	} //last bank
}

//---------------------------------------------------------------------------------
void HandleBOR_Mesytec(int run, int time, det_t* pdet, hist_t* hist)
{
	std::string CalibFile = "config_online.txt";
	calMesy.Load(CalibFile);
	calMesy.Print(0);

	ascii = calMesy.boolASCII;
	
	char label[32];
	
	geoM.ReadGeometry(calMesy.fileGeometry.data());
	
	// Load binning parameters for histograms, then create histograms
	hist->LoadParams(calMesy.fileHistos.data());
	hist->Book();
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

//**************** Calibrate Yu  ****************************************
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
 
//**************** Calibrate Zd horizontal strips ****************************************
	Chan=-1;

	pFile = fopen (calMesy.fileZdy.data(), "r");

   	if (pFile == NULL || calMesy.boolZdy==false) {
		fprintf(logFile,"No calibration file for Zd horizontal strips. Skipping Zdy calibration.\n");
		printf("No calibration file for Zd horizontal strips. Skipping Zdy calibration.\n");
   		for (int i =0;i<16;i++  ){
			ZdyPed[i] = 0.;
			ZdyOffset[i] = 0.;
			ZdyGain[i] = 1.;  
		}
	}  
 	else {
		printf("Reading Zdy config file '%s'\n",calMesy.fileZdy.data());
		// Skip first line
  		fscanf(pFile,"%s",buffer);
  		fscanf(pFile,"%s",buffer);
 		fscanf(pFile,"%s",buffer);

		for (int i =0;i<16;i++  ){
       		fscanf(pFile,"%d%lf%lf",&Chan,&a,&b);
       		if(!usePeds){
				ZdyOffset[Chan-64] = a;
				ZdyGain[Chan-64] =  b;  
				printf("ZdyOffset %lf Zdygain %lf\n",ZdyOffset[Chan-64],ZdyGain[Chan-64]);
			}
       		else if (usePeds){
				ZdyPed[Chan-64] = a;
				ZdyGain[Chan-64] =  b;  
				printf("ZdyPed %lf Zdygain %lf\n",ZdyPed[Chan-64],ZdyGain[Chan-64]);
     		}
		}
     	fclose (pFile);
		printf("\n");
 	}
//************************************************************************

//**************** Calibrate Zd vertical strips ****************************************
	Chan=-1;
   	pFile = fopen (calMesy.fileZdx.data(), "r");

 	if (pFile == NULL || calMesy.boolZdx==false) {
		fprintf(logFile,"No calibration file for Zd vertical strips. Skipping Zdx calibration.\n");
		printf("No calibration file for Zd vertical strips. Skipping Zdx calibration.\n");
   		for (int i =0;i<16;i++  ){
			ZdxPed[i] = 0.;
			ZdxOffset[i] = 0.;
			ZdxGain[i] = 1.;  
		}
	}  
 	else  {
		printf("Reading Zdx config file '%s'\n",calMesy.fileZdx.data());
		// Skip first line
	  	fscanf(pFile,"%s",buffer);
	  	fscanf(pFile,"%s",buffer);
	 	fscanf(pFile,"%s",buffer);

		for (int i =0;i<16;i++  ){
       		fscanf(pFile,"%d%lf%lf",&Chan,&a,&b);
       		if (!usePeds){
				ZdxOffset[Chan-96] = a;
				ZdxGain[Chan-96] = b;   
				printf("ZdxOffset %lf Zdxgain %lf\n",ZdxOffset[Chan-96],ZdxGain[Chan-96]);
			}
       		else if (usePeds){
				ZdxPed[Chan-96] = a;
				ZdxGain[Chan-96] = b;   
				printf("ZdxPed %lf Zdxgain %lf\n",ZdxPed[Chan-96],ZdxGain[Chan-96]);
			}
     	}
     	fclose (pFile);
		printf("\n");
 	}

//************************************************************************

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
