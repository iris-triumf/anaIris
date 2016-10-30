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
TH1D * hQValue = NULL; // QValue
TH1D * hYdEnergy = NULL; //YdEnergy
TH1D * hYuEnergy = NULL; //YuEnergy
TH2D *hYdCsIEnergyTime = NULL;
TH2D *hS3EnergyTime = NULL;
TH2D *hYdCsITheta = NULL;
TH2D *hYuEnergyTheta = NULL;
TH2D *hYdCsIThetaProt = NULL; //kinematics with proton gate
Double_t tRF = 0.;
double EBAC = 66.; //Beam energy from accelerator
Double_t m1=11.;
Double_t m3=1.;
  Double_t m4=11.;
Double_t a1, a2;
 Double_t  b1,j,thetaD, Q,E1,E3,thetaR,ECsI,EYY1;

//gates
TCutG* protonsP = NULL;
TCutG* elasticPS3 = NULL;


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
 
  hYdCsITheta->Fill(det->TYdTheta,det->TCsI1Energy+det->TYdEnergy);
  if (protonsP->IsInside(det->TCsI1Energy,det->TYdEnergy*cos(det->TYdTheta*0.01745329))){
  hYdCsIThetaProt->Fill(det->TYdTheta,det->TCsI1Energy+det->TYdEnergy);
  }
  hYuEnergyTheta->Fill(det->TYuTheta,det->TYuEnergy);
// Calculate Q-value from YY1// Jaspreet

 // ble_t  Qvalue(Double_t Q, Double_t E1, Double_t E3, Double_t m1,Double_t m3, Double_t m4, Double_t thetaR)


 
     ECsI= det->TCsIEnergy;
   if( ECsI == -10000){
      ECsI=0;}
      
   else if (ECsI != -10000){
      ECsI= ECsI;
   }
   
   //cout<<"csi value taken is:"<<ECsI<<endl;
     EYY1 = det->TYdEnergy ;
     // cout<<"EYY ENERGY IS"<<EYY1<<endl;
   E3=  ECsI+EYY1;
   // cout<<"etot"<<E3<<endl;
    E1 =EBAC;
    //cout<<"beam energy"<<E1<<endl;
    //cout<<"Energy of projectile and ejectile(lighter particle):";
    // cin>>E1>>E3;
    // thetaD = det->TYdTheta;
  
    //thetaR = thetaD*(pi/180);
//  cout<<"thetaR value is :"<< thetaR<<endl;
    // cin.ignore();

    // a1 = 1.+(m3/m4);
     // cout<<"a1 value is:"<<a1<<endl;
 
    // a2= (1.-(m1/m4));
     // cout<<"a2 value is:"<<a2<<endl;
     b1 =(sqrt(m1*E1*m3*E3));
     //cout<<"b1 value is:"<<b1<<endl;
  j= ((2.*b1)/m4);
  // cout<<"j value is:"<<j<<endl;
  thetaD = det->TYdTheta;
  //cout<<"theta value taken"<<thetaD<<endl;
  // cin>>thetaD;
  thetaR = thetaD*TMath::DegToRad();
  // cout<<"theta value is :"<<thetaR<<endl;
  Q = ((E3*a1)-(E1*a2)-(j*(cos(thetaR))));
  // hQvalue -> Fill(Q); TEMP.
  // cout<<"Qvalue is :"<<Q<<endl;
  det->QValue = Q;
 
  hQValue->Fill(Q,1.);
}

//---------------------------------------------------------------------------------
void HandleBOR_PHYSICS(int run, int time)
{

 //proton gate
   TFile *fgate = new TFile("/home/iris/current/anaIris/cuts_online.root");
	if(fgate->IsZombie()) protonsP = new TCutG();
	else{
  		protonsP = (TCutG*)fgate->FindObjectAny("protons1");
  		protonsP->SetName("protonsP"); //protons in Physics file
	}
  	elasticPS3 = new TCutG();

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

 hYdCsITheta = new TH2D("YdCsITheta","YdCsITheta",512, 0 ,60, 512, 0, 20);
	printf("Booking TH2D  YdCsITheta\n");
 hYdCsIThetaProt = new TH2D("YdCsIThetaProt","YdCsIThetaProt",512, 0 ,60, 512, 0, 20);
	printf("Booking TH2D  YdCsIThetaProt\n");

 hYuEnergyTheta = new TH2D("YuEnergyTheta","YuEnergyTheta",512, 0 ,60, 512, 0, 20);
	printf("Booking TH2D  YdCsITheta\n");
	
hS3EnergyTime = new TH2D("S3EnergyTime","S3EnergyTime",512,0,1024,512,0,200);
   printf("Booking TH2D  S3EnergyTime\n");
      printf(" in Physics BOR... Booking histos Done ....\n");
    
      hQValue = new TH1D("QValue","QValue",1024,-10,10);
      printf("Booking T1D  QValue\n");
}
  
  
}

void HandleEOR_PHYSICS(int run, int time)
{
  printf(" in Physics EOR\n");
}
