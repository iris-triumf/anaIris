//
// ROOT analyzer
//
// PHYSICS detector handling
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
#include <TCutG.h>

#include "HandleMesytec.h"
#include "HandleV1190.h"
#include "HandlePHYSICS.h"
#include "Globals.h"

const int Nchannels = 24;
const int binlimit = 1900;
int timeChannel=0; // corresponding time cahnnel for the highest energy adc channel
TH1D * hQValue1 = NULL; // QValue
TH1D * hQValue2 = NULL; // QValue
TH1D * hYdEnergy = NULL; //YdEnergy
TH1D * hYuEnergy = NULL; //YuEnergy
TH2D *hYdCsIEnergyTime = NULL;
TH2D *hS3EnergyTime = NULL;
TH2D *hYdCsI1Theta = NULL;
TH2D *hYdCsI2Theta = NULL;
TH2D *hYuEnergyTheta = NULL;
TH2D *hYdCsIThetaProt = NULL; //kinematics with proton gate
Double_t tRF = 0.;
double EBAC = 144.; //Beam energy from accelerator
Double_t m1=11.;
Double_t m3=1.;
  Double_t m4=11.;
Double_t a1, a2;
 Double_t  b1,j,thetaD, Q,E1,E3,thetaR,ECsI,EYY1, EDL;
 Double_t Q1,Q2 ,EBeam,Eb1,Pb1,PA;
  Double_t mA, ma,mb,mB;

//gates
TCutG* transfer = NULL;
TCutG* elastic = NULL;


void HandlePHYSICS(det_t * det, tdc_t *timeArray)
{
  if (det->TYdChannel>-1)
  timeChannel = det->TYdChannel+64;
  // printf("timeChannel = %d time %lf YdEnergy %lf\n",timeChannel,timeArray->timeRF[timeChannel],det->TYdEnergy);
  hYdEnergy->Fill(det->TYdEnergy,1.);

if (det->TYuChannel>-1)
  //timeChannel = det->TYuChannel+64;
  // printf("timeChannel = %d time %lf YuEnergy %lf\n",timeChannel,timeArray->timeRF[timeChannel],det->TYuEnergy);
   hYuEnergy->Fill(det->TYuEnergy,1.);

  hYdCsIEnergyTime->Fill(timeArray->timeRF[timeChannel],det->TYdEnergy+det->TCsIEnergy);
  if (det->TSd1sChannel>-1)
 timeChannel = det->TSd1sChannel+384;
  hS3EnergyTime->Fill(timeArray->timeRF[timeChannel],det->TSd1sEnergy+det->TSd2sEnergy);
 
  hYdCsI1Theta->Fill(det->TYdTheta,det->TCsI1Energy+det->TYdEnergy);
  hYdCsI2Theta->Fill(det->TYdTheta,det->TCsI2Energy+det->TYdEnergy);
  if (transfer->IsInside(det->TCsI1Energy,det->TYdEnergy*cos(det->TYdTheta*0.01745329))){
  hYdCsIThetaProt->Fill(det->TYdTheta,det->TCsI1Energy+det->TYdEnergy);
  }
  hYuEnergyTheta->Fill(det->TYuTheta,det->TYuEnergy);
// Calculate Q-value from YY1// Jaspreet

 // ble_t  Qvalue(Double_t Q, Double_t E1, Double_t E3, Double_t m1,Double_t m3, Double_t m4, Double_t thetaR)

  	if (elastic->IsInside(det->TCsI1Energy,det->TYdEnergy*cos(det->TYdTheta*0.01745329))){
     	ECsI= det->TCsIEnergy;
   		if( ECsI == -10000){
      		ECsI=0;}
      
   		else if (ECsI != -10000){
      		ECsI= ECsI;
   		}
   
		EYY1 = det->TYdEnergy ;
	 	EDL = 0.227; //energy loss in dead layer
   		Eb1=  ECsI+EYY1+EDL; 
    	EBeam =EBAC;
  		thetaD = det->TYdTheta;
  		thetaR = thetaD*TMath::DegToRad();

  		mA=5606.556;
  		ma=107961.79;
  		mb=107961.79;
  		mB=5606.556;
		PA = sqrt(EBeam*EBeam+2.*EBeam*mA);//beam momentum
		Pb1 = sqrt(Eb1*Eb1+2.*Eb1*mb);
	 	Q1 = mA+ma-mb- sqrt(mA*mA+mb*mb-ma*ma-2.*(mA+EBeam)*(mb+Eb1)+2.*PA*Pb1*cos(thetaR)+2.*(EBeam+mA+ma-Eb1-mb)*ma);  //Alisher's equation 

  		hQValue1->Fill(Q1,1.);
  	}
  
	if (transfer->IsInside(det->TCsI1Energy,det->TYdEnergy*cos(det->TYdTheta*0.01745329))){
     	ECsI= det->TCsIEnergy;
   		if( ECsI == -10000){
      		ECsI=0;}
      
   		else if (ECsI != -10000){
      		ECsI= ECsI;
   		}
   
     	EYY1 = det->TYdEnergy ;
	 	EDL = 0.148; //energy loss in dead layer
   		Eb1=  ECsI+EYY1+EDL; 
    	EBeam =EBAC;
  		thetaD = det->TYdTheta;
  		thetaR = thetaD*TMath::DegToRad();

  		mA=5606.556;
  		ma=107961.79;
  		mb=109824.646;
  		mB=3728.401;
		PA = sqrt(EBeam*EBeam+2.*EBeam*mA);//beam momentum
		Pb1 = sqrt(Eb1*Eb1+2.*Eb1*mb);
	 	Q2 = mA+ma-mb- sqrt(mA*mA+mb*mb-ma*ma-2.*(mA+EBeam)*(mb+Eb1)+2.*PA*Pb1*cos(thetaR)+2.*(EBeam+mA+ma-Eb1-mb)*ma);  //Alisher's equation 
 
  		hQValue2->Fill(Q2,1.);
  }
}

//---------------------------------------------------------------------------------
void HandleBOR_PHYSICS(int run, int time)
{

 //proton gate
   	TFile *fgate1 = new TFile("/home/iris/current/calib/S1642/pid_gate_4He.root");
   	TFile *fgate2 = new TFile("/home/iris/current/calib/S1642/pid_gate_6He.root");
	if(fgate1->IsZombie()) transfer = new TCutG();
  	else{
  		transfer = (TCutG*)fgate1->FindObjectAny("pid_4He");
  		transfer->SetName("pid_4He"); //protons in Physics file
	}
	if(fgate2->IsZombie()) elastic = new TCutG();
  	else{
		elastic = (TCutG*)fgate2->FindObjectAny("pid_6He");
  		elastic->SetName("pid_6He"); //protons in Physics file
	}

  char label[32], sig[32];

  // Booking 
  printf(" in Physics BOR... Trying to book\n");
  gOutputFile->cd();

 
    if (gOutputFile) {
      printf(" in Physics BOR... Booking histos\n");

      hYdEnergy = new TH1D("YdEnergy","YdEnergy",512,0,20);
      hYuEnergy = new TH1D("YuEnergy","YuEnergy",512,0,20);
       hYdCsIEnergyTime = new TH2D("YdCsIEnergyTime","YdCsIEnergyTime",512,0,1024,512,0,20);
	printf("Booking TH2D  YdCsIEnergyTime\n");

 hYdCsI1Theta = new TH2D("YdCsI1Theta","YdCsI1Theta",600, 0 ,60, 800, 0, 120);
	printf("Booking TH2D  YdCsI1Theta\n");
 hYdCsI2Theta = new TH2D("YdCsI2Theta","YdCsI2Theta",600, 0 ,60, 800, 0, 120);
	printf("Booking TH2D  YdCsI2Theta\n");
 hYdCsIThetaProt = new TH2D("YdCsIThetaProt","YdCsIThetaProt",512, 0 ,60, 512, 0, 20);
	printf("Booking TH2D  YdCsIThetaProt\n");

 hYuEnergyTheta = new TH2D("YuEnergyTheta","YuEnergyTheta",512, 0 ,60, 512, 0, 20);
	printf("Booking TH2D  YdCsITheta\n");
	
hS3EnergyTime = new TH2D("S3EnergyTime","S3EnergyTime",512,0,1024,512,0,200);
   printf("Booking TH2D  S3EnergyTime\n");
      printf(" in Physics BOR... Booking histos Done ....\n");
     
      hQValue1 = new TH1D("QValue1","QValue1",2000,-20,20);
      printf("Booking T1D  QValue1\n");

      hQValue2 = new TH1D("QValue2","QValue2",2000,-20,20);
      printf("Booking T1D  QValue2\n");
}
  
  
}

void HandleEOR_PHYSICS(int run, int time)
{
  printf(" in Physics EOR\n");
}
