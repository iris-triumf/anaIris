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
#include "CalibMesytec.h"
#include "Globals.h"

const int Nchannels = 24;
const int binlimit = 1900;
int timeChannel=0; // corresponding time cahnnel for the highest energy adc channel
TH1D * hQValue1 = NULL; // QValue
TH1D * hQValue2 = NULL; // QValue
TH1D * hQValueU = NULL; // QValue
TH2D *hYdCsIEnergyTime = NULL;
TH2D *hS3EnergyTime = NULL;
TH2D *hYdCsI1Theta = NULL;
TH2D *hYdCsI2Theta = NULL;
TH2D *hYuEnergyTheta = NULL;
TH2D *hYdCsIThetaProt = NULL; //kinematics with proton gate
Double_t tRF = 0.;
double EBAC = 144.; //Beam energy from accelerator
Double_t a1, a2;
Double_t  b1,j,thetaD, Q,E1,E3,thetaR,ECsI,EYY1, EDL;
Double_t Q1,Q2,QU,EBeam,Eb1,Pb1,PA;
Double_t mA, ma,mb,mB;

//gates
CalibMesytec calPhys;
TCutG* transfer = NULL;
TCutG* elastic = NULL;
TCutG* upstream = NULL;
TCutG* upstream_sd = NULL;


void HandlePHYSICS(det_t * det, tdc_t *timeArray)
{
	if (det->TYdChannel>-1){
		timeChannel = det->TYdChannel+64;
	}

	if (det->TYuChannel>-1){
  		timeChannel = det->TYuChannel+64;
	}

  	hYdCsIEnergyTime->Fill(timeArray->timeRF[timeChannel],det->TYdEnergy+det->TCsIEnergy);
  	if (det->TSd1sChannel>-1){
 		timeChannel = det->TSd1sChannel+384;
	}
  	hS3EnergyTime->Fill(timeArray->timeRF[timeChannel],det->TSd1sEnergy+det->TSd2sEnergy);
 
	hYdCsI1Theta->Fill(det->TYdTheta,det->TCsI1Energy+det->TYdEnergy);
	hYdCsI2Theta->Fill(det->TYdTheta,det->TCsI2Energy+det->TYdEnergy);
	if (transfer->IsInside(det->TCsI1Energy,det->TYdEnergy*cos(det->TYdTheta*0.01745329))){
		hYdCsIThetaProt->Fill(det->TYdTheta,det->TCsI1Energy+det->TYdEnergy);
	}
	hYuEnergyTheta->Fill(det->TYuTheta,det->TYuEnergy);
  	if (elastic->IsInside(det->TCsI1Energy,det->TYdEnergy*cos(det->TYdTheta*0.01745329))){
     	ECsI= det->TCsIEnergy;
   		if( ECsI == -10000){
      		ECsI=0;}
      
   		else if (ECsI != -10000){
      		ECsI= ECsI;
   		}
   
  		thetaD = det->TYdTheta;
  		thetaR = thetaD*TMath::DegToRad();
		EYY1 = det->TYdEnergy ;
	 	EDL = calPhys.eldl1/cos(thetaR); //energy loss in dead layer
   		Eb1=  ECsI+EYY1+EDL; 
    	EBeam =EBAC;

  		mA=calPhys.mA;
  		ma=calPhys.ma;
  		mb=calPhys.mb1;
  		mB=calPhys.mB1;
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
   
  		thetaD = det->TYdTheta;
  		thetaR = thetaD*TMath::DegToRad();
     	EYY1 = det->TYdEnergy ;
	 	EDL = calPhys.eldl2/cos(thetaR); //energy loss in dead layer
   		Eb1=  ECsI+EYY1+EDL; 
    	EBeam =EBAC;

  		mA=calPhys.mA;
  		ma=calPhys.ma;
  		mb=calPhys.mb2;
  		mB=calPhys.mB2;

		PA = sqrt(EBeam*EBeam+2.*EBeam*mA);//beam momentum
		Pb1 = sqrt(Eb1*Eb1+2.*Eb1*mb);
	 	Q2 = mA+ma-mb- sqrt(mA*mA+mb*mb-ma*ma-2.*(mA+EBeam)*(mb+Eb1)+2.*PA*Pb1*cos(thetaR)+2.*(EBeam+mA+ma-Eb1-mb)*ma);  //Alisher's equation 
 
  		hQValue2->Fill(Q2,1.);
  	}
	
	if (upstream_sd->IsInside(det->TSurEnergy,det->TSuTheta)){
       	thetaD = det->TSuTheta;
  		thetaR = thetaD*TMath::DegToRad();
     	EYY1 = det->TSurEnergy ;
	 	EDL = calPhys.eldl3/cos(thetaR); //energy loss in dead layer
   		Eb1=  ECsI+EYY1+EDL; 
    	EBeam =EBAC;

  		mA=calPhys.mA;
  		ma=calPhys.ma;
  		mb=calPhys.mb2;
  		mB=calPhys.mB2;

		PA = sqrt(EBeam*EBeam+2.*EBeam*mA);//beam momentum
		Pb1 = sqrt(Eb1*Eb1+2.*Eb1*mb);
	 	QU = mA+ma-mb- sqrt(mA*mA+mb*mb-ma*ma-2.*(mA+EBeam)*(mb+Eb1)+2.*PA*Pb1*cos(thetaR)+2.*(EBeam+mA+ma-Eb1-mb)*ma);  //Alisher's equation 
 
  		hQValueU->Fill(QU,1.);
  	}
	
	if (upstream->IsInside(det->TYuEnergy,det->TYuTheta)){
       	thetaD = det->TYuTheta;
  		thetaR = thetaD*TMath::DegToRad();
     	EYY1 = det->TYuEnergy ;
	 	EDL = calPhys.eldl3/cos(thetaR); //energy loss in dead layer
   		Eb1=  ECsI+EYY1+EDL; 
    	EBeam =EBAC;

  		mA=calPhys.mA;
  		ma=calPhys.ma;
  		mb=calPhys.mb2;
  		mB=calPhys.mB2;

		PA = sqrt(EBeam*EBeam+2.*EBeam*mA);//beam momentum
		Pb1 = sqrt(Eb1*Eb1+2.*Eb1*mb);
	 	QU = mA+ma-mb- sqrt(mA*mA+mb*mb-ma*ma-2.*(mA+EBeam)*(mb+Eb1)+2.*PA*Pb1*cos(thetaR)+2.*(EBeam+mA+ma-Eb1-mb)*ma);  //Alisher's equation 
 
  		hQValueU->Fill(QU,1.);
  	}
}

//---------------------------------------------------------------------------------
void HandleBOR_PHYSICS(int run, int time)
{
	std::string CalibFile = "config_online.txt";
	calPhys.Load(CalibFile);
	calPhys.Print(1);

 	//proton gate
   	TFile *fgate1 = new TFile(calPhys.fGate1.data());
   	TFile *fgate2 = new TFile(calPhys.fGate2.data());
   	TFile *fgate3 = new TFile(calPhys.fGate3.data());
   	TFile *fgate4 = new TFile(calPhys.fGate4.data());
	if(calPhys.boolFGate1==kFALSE||fgate1->IsZombie()) transfer = new TCutG();
  	else{
  		transfer = (TCutG*)fgate1->FindObjectAny(calPhys.nGate1.data());
  		transfer->SetName(calPhys.nGate1.data()); //protons in Physics file
	}
	if(calPhys.boolFGate2==kFALSE||fgate2->IsZombie()) elastic = new TCutG();
  	else{
		elastic = (TCutG*)fgate2->FindObjectAny(calPhys.nGate2.data());
  		elastic->SetName(calPhys.nGate2.data()); //protons in Physics file
	}
	if(calPhys.boolFGate3==kFALSE||fgate3->IsZombie()) upstream = new TCutG();
  	else{
		upstream = (TCutG*)fgate3->FindObjectAny(calPhys.nGate3.data());
  		upstream->SetName(calPhys.nGate3.data()); //protons in Physics file
	}
	if(calPhys.boolFGate4==kFALSE||fgate4->IsZombie()) upstream_sd = new TCutG();
  	else{
		upstream_sd = (TCutG*)fgate4->FindObjectAny(calPhys.nGate4.data());
  		upstream_sd->SetName(calPhys.nGate4.data()); //protons in Physics file
	}

  	char label[32];

  	// Booking 
  	printf(" in Physics BOR... Trying to book\n");
  	gOutputFile->cd();

 
    if (gOutputFile) {
		
		hQValue1 = (TH1D*)gDirectory->Get("QValue1");
   		if (hQValue1 == 0) {
     		// Make a directory and cd to it.
        	TDirectory* PHYSICS_dir = gOutputFile->mkdir("PHYSICS");      
        	PHYSICS_dir->cd();

      		printf(" in Physics BOR... Booking histos\n");
 
			sprintf(label, "QValue1");
      		hQValue1 = new TH1D(label,label,2000,-20,20);
			printf("Booking TH1D %s \n", label);

			sprintf(label, "QValue2");
      		hQValue2 = new TH1D(label,label,2000,-20,20);
			printf("Booking TH1D %s \n", label);
      		
			sprintf(label, "QValueU");
			hQValueU = new TH1D(label,label,2000,-20,20);
			printf("Booking TH1D %s \n", label);

			sprintf(label, "YdCsIEnergyTime");
			hYdCsIEnergyTime = new TH2D(label,label,512,0,1024,512,0,20);
			printf("Booking TH2D %s \n", label);

			sprintf(label, "YdCsI1Theta");
 			hYdCsI1Theta = new TH2D(label,label,600, 0 ,60, 800, 0, 120);
			printf("Booking TH2D %s \n", label);
 			
			sprintf(label, "YdCsI2Theta");
			hYdCsI2Theta = new TH2D(label,label,600, 0 ,60, 800, 0, 120);
			printf("Booking TH2D %s \n", label);
 			
			sprintf(label, "YdCsIThetaProt");
			hYdCsIThetaProt = new TH2D(label,label,512, 0 ,60, 512, 0, 20);
			printf("Booking TH2D %s \n", label);

			sprintf(label, "YuEnergyTheta");
 			hYuEnergyTheta = new TH2D(label,label,512, 0 ,60, 512, 0, 20);
			printf("Booking TH2D %s \n", label);
	
			sprintf(label, "S3EnergyTime");
			hS3EnergyTime = new TH2D(label,label,512,0,1024,512,0,200);
			printf("Booking TH2D %s \n", label);
      		
			printf(" in Physics BOR... Booking histos Done ....\n");
    		}
	}
}

void HandleEOR_PHYSICS(int run, int time)
{
  	printf(" in Physics EOR\n");
}
