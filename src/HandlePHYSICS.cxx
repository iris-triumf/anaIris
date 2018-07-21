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
#include <TH1F.h>
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
#include "eloss.h"
#include "nucleus.h"
#include "runDepPar.h"
#include "Graphsdedx.h"
#include "geometry.h"
#include "Globals.h"
#include "SetupHistos.h"


int timeChannel=0; // corresponding time cahnnel for the highest energy adc channel
Double_t tRF = 0.;
double EBAC = 0.; //Beam energy from accelerator
Double_t a1, a2;
Double_t  b1,j,thetaD, Q,E1,E3,thetaR,ECsI,EYY1, EDL;
Double_t Q1,Q2,QU,EBeam,Eb1,Pb1,PA;
Double_t mA, ma,mb,mB;
Double_t betaCM, gammaCM; //CM velocity
Double_t kBF = 0.; //Ratio of Beam particle mass and 109-Ag foil nucleus mass
const Double_t MFoil = 931.494013*108.; //Ag foil mass AS
		
const Double_t ICLength=22.9*0.062; //cm*mg/cm^3 at 19.5 Torr
const Double_t ICWindow1=0.03*3.44*0.1; //mu*g/cm^3*0.1
const Double_t ICWindow2=0.05*3.44*0.1; //mu*g/cm^3*0.1


Double_t eATgt[100], eAIso[100], eAWndw[100], eAAg[100];	
Double_t dedxATgt[100], dedxAIso[100], dedxAWndw[100], dedxAAg[100];	
Double_t eBSi[100], eBTgt[100], eBSiO2[100], eBB[100], eBP[100], eBAl[100], eBMy[100], eBCsI[100];	
Double_t dedxBSi[100], dedxBTgt[100], dedxBSiO2[100], dedxBB[100], dedxBP[100], dedxBAl[100], dedxBMy[100], dedxBCsI[100];	
Double_t ebTgt[100], ebSi[100], ebAl[100], ebB[100], ebMy[100], ebP[100], ebCsI[100], ebSiO2[100];	
Double_t dedxbTgt[100], dedxbSi[100], dedxbAl[100], dedxbB[100], dedxbMy[100], dedxbP[100], dedxbCsI[100], dedxbSiO2[100];	


//gates
CalibMesytec calPhys;
geometry geoP;
Graphsdedx dedx_i, dedx_l, dedx_h;
runDep runDepPar; // run dependant parameters
nucleus beam; // beam particle properties
nucleus target; // target properties
nucleus lej; // light ejetcile properties
nucleus hej; // heavy ejectile properties  

TCutG* transfer = NULL;
TCutG* elastic = NULL;
TCutG* upstream = NULL;
TCutG* upstream_sd = NULL;

void HandlePHYSICS(det_t * det, tdc_t *timeArray, hist_t *hist)
{
	if (det->TYdChannel>-1){
		timeChannel = det->TYdChannel+64;
	}

	if (det->TYuChannel>-1){
  		timeChannel = det->TYuChannel+64;
	}

	if(det->TYdChannel>=0&&det->TCsI1Channel>=0&&(det->TCsI1Channel==2*det->TYdNo||det->TCsI1Channel==2*det->TYdNo+1)) hist->hYdCsI1PID_adc->Fill(det->TCsI1nadc,det->TYdEnergy*cos(det->TYdTheta*TMath::DegToRad()));
	if(det->TYdChannel>=0&&det->TCsI1Channel>=0&&(det->TCsI1Channel==2*det->TYdNo||det->TCsI1Channel==2*det->TYdNo+1)) hist->hYdCsI1PID_adc2->Fill(det->TCsI1nadc,det->TYdnadc);
	
	if(det->TYdChannel>=0&&det->TCsI1Channel>=0&&(det->TCsI1Channel==2*det->TYdNo||det->TCsI1Channel==2*det->TYdNo+1)) hist->hYdCsI1PID->Fill(det->TCsI1Energy,det->TYdEnergy*cos(det->TYdTheta*TMath::DegToRad()));
	if(det->TYdChannel>=0&&det->TCsI1Channel2>=0&&(det->TCsI1Channel2==2*det->TYdNo||det->TCsI1Channel2==2*det->TYdNo+1)) hist->hYdCsI1PID->Fill(det->TCsI1Energy2,det->TYdEnergy*cos(det->TYdTheta*TMath::DegToRad()));
	if(det->TYdChannel2>=0&&det->TCsI1Channel>=0&&(det->TCsI1Channel==2*det->TYdNo2||det->TCsI1Channel==2*det->TYdNo2+1)) hist->hYdCsI1PID->Fill(det->TCsI1Energy,det->TYdEnergy2*cos(det->TYdTheta*TMath::DegToRad()));
	if(det->TYdChannel2>=0&&det->TCsI1Channel2>=0&&(det->TCsI1Channel2==2*det->TYdNo2||det->TCsI1Channel==2*det->TYdNo2+1)) hist->hYdCsI1PID->Fill(det->TCsI1Energy2,det->TYdEnergy2*cos(det->TYdTheta*TMath::DegToRad()));

    if(det->TYdChannel>=0&&det->TCsI2Channel>=0&&(det->TCsI1Channel==2*det->TYdNo||det->TCsI1Channel==2*det->TYdNo+1)) hist->hYdCsI2PID_adc->Fill(det->TCsI2nadc,det->TYdEnergy*cos(det->TYdTheta*TMath::DegToRad()));
    if(det->TYdChannel>=0&&det->TCsI2Channel>=0&&(det->TCsI1Channel==2*det->TYdNo||det->TCsI1Channel==2*det->TYdNo+1)) hist->hYdCsI2PID_adc2->Fill(det->TCsI2nadc,det->TYdnadc);
    
	if(det->TYdChannel>=0&&det->TCsI2Channel>=0&&(det->TCsI2Channel==2*det->TYdNo||det->TCsI2Channel==2*det->TYdNo+1)) hist->hYdCsI2PID->Fill(det->TCsI2Energy,det->TYdEnergy*cos(det->TYdTheta*TMath::DegToRad()));
    if(det->TYdChannel>=0&&det->TCsI2Channel2>=0&&(det->TCsI2Channel2==2*det->TYdNo||det->TCsI2Channel2==2*det->TYdNo+1)) hist->hYdCsI2PID->Fill(det->TCsI2Energy2,det->TYdEnergy*cos(det->TYdTheta*TMath::DegToRad()));
    if(det->TYdChannel2>=0&&det->TCsI2Channel>=0&&(det->TCsI2Channel==2*det->TYdNo2||det->TCsI2Channel==2*det->TYdNo2+1)) hist->hYdCsI2PID->Fill(det->TCsI2Energy,det->TYdEnergy2*cos(det->TYdTheta*TMath::DegToRad()));
    if(det->TYdChannel2>=0&&det->TCsI2Channel2>=0&&(det->TCsI2Channel2==2*det->TYdNo2||det->TCsI2Channel2==2*det->TYdNo2+1)) hist->hYdCsI2PID->Fill(det->TCsI2Energy2,det->TYdEnergy2*cos(det->TYdTheta*TMath::DegToRad()));

 
	if ((fabs(det->TSd1sEnergy-det->TSd1rEnergy)<2.) && (fabs(det->TSd2sEnergy-det->TSd2rEnergy)<2.)) hist->hSdPID->Fill(det->TSd2rEnergy,det->TSd1rEnergy);
	if ((fabs(det->TSd1sEnergy-det->TSd1rEnergy)<2.) && (fabs(det->TSd2sEnergy-det->TSd2rEnergy)<2.)) hist->hSdETot->Fill(det->TSd1rEnergy+det->TSd2rEnergy);
	if (det->TICEnergy>runDepPar.ICmin&&det->TICEnergy<runDepPar.ICmax  && det->TSd1rChannel==2 && det->TSd2rChannel==3) hist->hSdETotMonitor->Fill(det->TSd1rEnergy+det->TSd2rEnergy);
	
	if (det->TYdEnergy>0. && det->TCsI1Energy>0.) hist->hYdCsI1ETot->Fill(det->TYdEnergy+det->TCsI1Energy);
	if (det->TYdEnergy>0. && det->TCsI2Energy>0.) hist->hYdCsI2ETot->Fill(det->TYdEnergy+det->TCsI2Energy);
 		

  	hist->hYdCsIEnergyTime->Fill(timeArray->timeRF[timeChannel],det->TYdEnergy+det->TCsIEnergy);
  	if (det->TSd1sChannel>-1){
 		timeChannel = det->TSd1sChannel+384;
	}
  	hist->hS3EnergyTime->Fill(timeArray->timeRF[timeChannel],det->TSd1sEnergy+det->TSd2sEnergy);
 
	hist->hYdCsI1Theta->Fill(det->TYdTheta,det->TCsI1Energy+det->TYdEnergy);
	hist->hYdCsI2Theta->Fill(det->TYdTheta,det->TCsI2Energy+det->TYdEnergy);
	if (transfer->IsInside(det->TCsI1Energy,det->TYdEnergy*cos(det->TYdTheta*TMath::DegToRad()))){
		hist->hYdCsIThetaProt->Fill(det->TYdTheta,det->TCsI1Energy+det->TYdEnergy);
	}
	hist->hYuEnergyTheta->Fill(det->TYuTheta,det->TYuEnergy);
  	if (elastic->IsInside(det->TCsI1Energy,det->TYdEnergy*cos(det->TYdTheta*TMath::DegToRad()))){
     	ECsI= det->TCsIEnergy;
   		if( ECsI == -10000){
      		ECsI=0;
		}
   		else if (ECsI != -10000){
      		ECsI= ECsI;
			ECsI= ECsI+elossFi(ECsI,0.1*1.4*6./cos(thetaR),ebMy,dedxbMy); //Mylar                                                                                  
			ECsI= ECsI+elossFi(ECsI,0.1*2.702*0.3/cos(thetaR),ebAl,dedxbAl); //0.3 u Al                                                                            
			ECsI= ECsI+elossFi(ECsI,0.1*1.822*0.1/cos(thetaR),ebP,dedxbP); // 0.1Phosphorus                                                                      
		}
   
  		thetaD = det->TYdTheta;
  		thetaR = thetaD*TMath::DegToRad();
		EYY1 = det->TYdEnergy ;
   		Eb1=  ECsI+EYY1; 
		Eb1= Eb1+elossFi(Eb1,0.1*2.35*0.05/cos(thetaR),ebB,dedxbB); //0.05 u B 
		Eb1= Eb1+elossFi(Eb1,0.1*2.702*0.1/cos(thetaR),ebAl,dedxbAl); //0.1 u Al
		Eb1= Eb1+elossFi(Eb1,geoP.TargetThickness/2./cos(thetaR),ebTgt,dedxbTgt); //deuteron energy midtarget
			
		EBeam =EBAC;

  		//mA=calPhys.mA;
  		//ma=calPhys.ma;
  		//mb=calPhys.mb1;
  		//mB=calPhys.mB1;
		PA = sqrt(EBeam*EBeam+2.*EBeam*mA);//beam momentum
		Pb1 = sqrt(Eb1*Eb1+2.*Eb1*mb);
	 	Q1 = mA+ma-mb- sqrt(mA*mA+mb*mb-ma*ma-2.*(mA+EBeam)*(mb+Eb1)+2.*PA*Pb1*cos(thetaR)+2.*(EBeam+mA+ma-Eb1-mb)*ma);  //Alisher's equation 

  		hist->hQValue1->Fill(Q1,1.);
  	}
   
	if (transfer->IsInside(det->TCsI1Energy,det->TYdEnergy*cos(det->TYdTheta*TMath::DegToRad()))){
   		ECsI= det->TCsIEnergy;
   		if( ECsI == -10000){
      		ECsI=0;
		}
   		else if (ECsI != -10000){
      		ECsI= ECsI;
			ECsI= ECsI+elossFi(ECsI,0.1*1.4*6./cos(thetaR),ebMy,dedxbMy); //Mylar                                                                                  
			ECsI= ECsI+elossFi(ECsI,0.1*2.702*0.3/cos(thetaR),ebAl,dedxbAl); //0.3 u Al                                                                            
			ECsI= ECsI+elossFi(ECsI,0.1*1.822*0.1/cos(thetaR),ebP,dedxbP); // 0.1Phosphorus                                                                      
		}

  		thetaD = det->TYdTheta;
  		thetaR = thetaD*TMath::DegToRad();
     	EYY1 = det->TYdEnergy ;
    	Eb1=  ECsI+EYY1; 
		Eb1= Eb1+elossFi(Eb1,0.1*2.35*0.05/cos(thetaR),ebB,dedxbB); //0.05 u B 
		Eb1= Eb1+elossFi(Eb1,0.1*2.702*0.1/cos(thetaR),ebAl,dedxbAl); //0.1 u Al
		Eb1= Eb1+elossFi(Eb1,geoP.TargetThickness/2./cos(thetaR),ebTgt,dedxbTgt); //deuteron energy midtarget
		
		EBeam =EBAC;
		
		PA = sqrt(EBeam*EBeam+2.*EBeam*mA);//beam momentum
		Pb1 = sqrt(Eb1*Eb1+2.*Eb1*mb);
	 	Q2 = mA+ma-mb- sqrt(mA*mA+mb*mb-ma*ma-2.*(mA+EBeam)*(mb+Eb1)+2.*PA*Pb1*cos(thetaR)+2.*(EBeam+mA+ma-Eb1-mb)*ma);  //Alisher's equation 
 
  		hist->hQValue2->Fill(Q2,1.);
  	}
	
	if (upstream_sd->IsInside(det->TSurEnergy,det->TSuTheta)){
       	thetaD = det->TSuTheta;
  		thetaR = thetaD*TMath::DegToRad();
     	EYY1 = det->TSurEnergy ;
    	EBeam =EBAC;
		
		PA = sqrt(EBeam*EBeam+2.*EBeam*mA);//beam momentum
		Pb1 = sqrt(Eb1*Eb1+2.*Eb1*mb);
	 	QU = mA+ma-mb- sqrt(mA*mA+mb*mb-ma*ma-2.*(mA+EBeam)*(mb+Eb1)+2.*PA*Pb1*cos(thetaR)+2.*(EBeam+mA+ma-Eb1-mb)*ma);  //Alisher's equation 
 
  		hist->hQValueU->Fill(QU,1.);
  	}
	
	if (upstream->IsInside(det->TYuEnergy,det->TYuTheta)){
       	thetaD = det->TYuTheta;
  		thetaR = thetaD*TMath::DegToRad();
     	EYY1 = det->TYuEnergy ;
    	Eb1= EYY1; 
		Eb1= Eb1+elossFi(Eb1,0.1*2.35*0.05/cos(thetaR),ebB,dedxbB); //0.05 u B 
		Eb1= Eb1+elossFi(Eb1,0.1*2.702*0.1/cos(thetaR),ebAl,dedxbAl); //0.1 u Al
		Eb1= Eb1+elossFi(Eb1,geoP.TargetThickness/2./cos(thetaR),ebTgt,dedxbTgt); //deuteron energy midtarget
		
		EBeam =EBAC;

  		//mA=calPhys.mA;
  		//ma=calPhys.ma;
  		//mb=calPhys.mb2;
  		//mB=calPhys.mB2;

		PA = sqrt(EBeam*EBeam+2.*EBeam*mA);//beam momentum
		Pb1 = sqrt(Eb1*Eb1+2.*Eb1*mb);
	 	QU = mA+ma-mb- sqrt(mA*mA+mb*mb-ma*ma-2.*(mA+EBeam)*(mb+Eb1)+2.*PA*Pb1*cos(thetaR)+2.*(EBeam+mA+ma-Eb1-mb)*ma);  //Alisher's equation 
 
  		hist->hQValueU->Fill(QU,1.);
  	}
}

void calculateBeamEnergy(Double_t E)
{
	runDepPar.EBAC = E;
	printf("New Beam Energy: %f\n" ,E);
	Double_t temp_E = E;
	if(calPhys.boolIC==kTRUE){
		E = E-eloss(E,ICWindow1,eAWndw,dedxAWndw);  
		E = E-eloss(E,ICLength,eAIso,dedxAIso);  
		E = E-eloss(E,ICWindow2,eAWndw,dedxAWndw);  
		printf("Energy loss in IC (including windows): %.3f MeV\n" ,temp_E-E);

		temp_E = E;
	}
	//if(geoP.FoilThickness>0.) E = runDepPar.energy-eloss(E,geoP.FoilThickness,eAAg,dedxAAg);  
	if(geoP.FoilThickness>0.) E = E-eloss(E,geoP.FoilThickness,eAAg,dedxAAg);  
	else E = temp_E;
	printf("Energy loss in silver foil: %.3f MeV\n" ,temp_E-E);
	printf("Energy after silver foil: %.3f MeV\n",E);

	temp_E = E;
	E = E-eloss(E,geoP.TargetThickness/2.,eATgt,dedxATgt);  
	printf("Energy loss in half target: %.3f MeV\n" ,temp_E-E);
	printf("Beam energy in center of target: %.3f MeV\n" ,E);

	runDepPar.energy = E;
	runDepPar.momentum = sqrt(runDepPar.energy*runDepPar.energy+2.*runDepPar.energy*beam.mass);//beam momentum
	runDepPar.beta = runDepPar.momentum/(runDepPar.energy + beam.mass + target.mass);
	runDepPar.gamma = 1./sqrt(1.-runDepPar.beta*runDepPar.beta);

	EBAC = runDepPar.EBAC;
	EBeam= runDepPar.energy;
	PA = runDepPar.momentum;//beam momentum
	betaCM = runDepPar.beta;
	gammaCM = runDepPar.gamma;
	printf("Beam momentum in center of target: %.3f MeV\n",PA);
	printf("Beta: %f\tGamma: %f\n",betaCM,gammaCM);
}

//---------------------------------------------------------------------------------
void HandleBOR_PHYSICS(std::string BinPath, int run, int time)
{
	std::string CalibFile = "config_online.txt";
	calPhys.Load(CalibFile);
	calPhys.Print(1);
	geoP.ReadGeometry(calPhys.fileGeometry.data());

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

	if(calPhys.boolRunDepPar){
		runDepPar.setRunDepPar(calPhys.fileRunDepPar);// setting run dependent parameters.
		runDepPar.Print();

		beam.getInfo(BinPath,runDepPar.nA);
		target.getInfo(BinPath,runDepPar.na);
		hej.getInfo(BinPath,runDepPar.nB);
		lej.getInfo(BinPath,runDepPar.nb);

		mA = beam.mass; //Beam mass //Reassigned in HandleBOR
		ma = target.mass;
		mb = lej.mass; //Light ejectile mass
		mB = hej.mass;
    	printf("mA=%f, ma=%f, mb=%f, mB=%f\n",mA,ma,mb,mB); 
		kBF = MFoil/mA;
	
		printf("Beam energy: %f\n", runDepPar.energy);
		// printf("Target thickness: %f\n",geoP.TargetThickness);
	
		if(calPhys.boolIdedx==kTRUE){
			printf("\n\nLoading dedx Graphs for incoming %s ...\n",runDepPar.nA.data());
			dedx_i.Load(calPhys.fileIdedx);
			dedx_i.Print();
			if(dedx_i.boolAg==kTRUE) loadELoss(dedx_i.Ag,eAAg,dedxAAg,mA);	
			if(dedx_i.boolTgt==kTRUE) loadELoss(dedx_i.Tgt,eATgt,dedxATgt,mA);	
		//	if(dedx_i.boolSi==kTRUE) loadELoss(dedx_i.Si,eBSi,dedxBSi,mB);	
		//	if(dedx_i.boolAl==kTRUE) loadELoss(dedx_i.Al,eBAl,dedxBAl,mB);	
		//	if(dedx_i.boolB==kTRUE) loadELoss(dedx_i.B, eBB,dedxBB,mB);	
		//	if(dedx_i.boolP==kTRUE) loadELoss(dedx_i.P, eBP,dedxBP,mB);	
		//	if(dedx_i.boolSiO2==kTRUE) loadELoss(dedx_i.SiO2,eBSiO2,dedxBSiO2,mB);	
			if(dedx_i.boolIso==kTRUE) loadELoss(dedx_i.Iso,eAIso,dedxAIso,mA);	
			if(dedx_i.boolWndw==kTRUE) loadELoss(dedx_i.Wndw,eAWndw,dedxAWndw,mA);	
		}

		if(calPhys.boolLdedx==kTRUE){
			printf("\n\nLoading dedx Graphs for target-like %s...\n",runDepPar.nb.data());
			dedx_l.Load(calPhys.fileLdedx);
			dedx_l.Print();
			if(dedx_l.boolSi==kTRUE) loadELoss(dedx_l.Si,ebSi,dedxbSi,mb);	
			if(dedx_l.boolAl==kTRUE) loadELoss(dedx_l.Al,ebAl,dedxbAl,mb);	
			if(dedx_l.boolB==kTRUE) loadELoss(dedx_l.B,ebB,dedxbB,mb);	
			if(dedx_l.boolP==kTRUE) loadELoss(dedx_l.P,ebP,dedxbP,mb);	
			if(dedx_l.boolMy==kTRUE) loadELoss(dedx_l.My,ebMy,dedxbMy,mb);	
			if(dedx_l.boolTgt==kTRUE) loadELoss(dedx_l.Tgt,ebTgt,dedxbTgt,mb);	
		}

		if(calPhys.boolHdedx==kTRUE){
			printf("\n\nLoading dedx Graphs for beam-like %s...\n",runDepPar.nB.data());
			dedx_h.Load(calPhys.fileHdedx);
			dedx_h.Print();
		//	if(dedx_h.boolAg==kTRUE) loadELoss(dedx_h.Ag,eAAg,dedxAAg,mA);	
		//	if(dedx_h.boolTgt==kTRUE) loadELoss(dedx_h.Tgt,eBTgt,dedxBTgt,mB);	
			if(dedx_h.boolSi==kTRUE) loadELoss(dedx_h.Si,eBSi,dedxBSi,mB);	
			if(dedx_h.boolAl==kTRUE) loadELoss(dedx_h.Al,eBAl,dedxBAl,mB);	
			if(dedx_h.boolB==kTRUE) loadELoss(dedx_h.B, eBB,dedxBB,mB);	
			if(dedx_h.boolP==kTRUE) loadELoss(dedx_h.P, eBP,dedxBP,mB);	
			if(dedx_h.boolSiO2==kTRUE) loadELoss(dedx_h.SiO2,eBSiO2,dedxBSiO2,mB);	
		//	if(dedx_h.boolIso==kTRUE) loadELoss(dedx_h.Iso,eAIso,dedxAIso,mB);	
		//	if(dedx_h.boolWndw==kTRUE) loadELoss(dedx_h.Wndw,eAWndw,dedxAWndw,mB);	
		}

		// Initialize runPar with values from first run in chain	
		calculateBeamEnergy(runDepPar.energy);
		
		printf("MBeam: %f\t MFoil: %f\t kBF: %f\n",beam.mass,MFoil,kBF);
		printf("beam mass: %f MeV (%f)\ttarget mass: %f MeV (%f)\n",mA,mA/931.494061,ma,ma/931.494061);
		printf("heavy ejectile mass: %f MeV (%f)\tlight ejectile mass: %f MeV (%f)\n",mB,mB/931.494061,mb,mb/931.494061);
	}

}

void HandleEOR_PHYSICS(int run, int time)
{
  	printf(" in Physics EOR\n");
}
