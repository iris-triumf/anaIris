// HandleMesytec.h

#ifndef HandleMesytec_H
#define HandleMesytec_H

#include "SetupHistos.h"

//Extern
extern int gMesytecnitems;


class det_t {
 public:
	Int_t EventNumber;
	Double_t TYdEnergy;
	Double_t TYdEnergy2;
	Double_t TYdNo;
	Double_t TYdNo2;
	Double_t TYdnadc;
	Double_t TCsI1Energy;
	Double_t TCsI2Energy;
	Double_t TCsIEnergy;
	Double_t TCsI1Energy2;
	Double_t TCsI2Energy2;
	Double_t TCsIEnergy2;
	Double_t TCsI1nadc;
	Double_t TCsI2nadc;
	Double_t TICEnergy;
	Double_t TSd2rEnergy;
	Double_t TSd2sEnergy;
	Double_t TSd1rEnergy;
	Double_t TSd1sEnergy;
	Double_t TSd2rEnergy2;
	Double_t TSd2sEnergy2;
	Double_t TSd2rEnergyCal;
	Double_t TSd1rEnergy2;
	Double_t TSd1sEnergy2;
	Double_t TSdETot;
	Double_t TSdTheta;
	Double_t TSdThetaCM;
	Double_t TSdPhi;
	Double_t TYdTheta;// Yd theta angle
	Double_t TQv; // Q-value from S3
	Double_t   QValue; //Q-value from YY1
	
	Double_t TBE; //Beam energy
	Double_t TIC00;
	Double_t TIC01;
	Double_t TIC02;
	Double_t TIC03;
	Double_t TIC04;
	Double_t TIC05;
	Double_t TIC06;
	Double_t TIC07;
	Double_t TIC08;
	Double_t TIC09;
	Double_t TIC10;
	Double_t TIC11;
	Double_t TIC12;
	Double_t TIC13;
	Double_t TIC14;
	Double_t TIC15;
	Int_t    TYdChannel;
	Int_t    TYdChannel2;
	Int_t    TCsI1Channel;
	Int_t    TCsI2Channel;
	Int_t    TCsI1Channel2;
	Int_t    TCsI2Channel2;
 	Int_t    TCsIChannel; 
	Int_t    TICChannel;
	Int_t    TSd2rChannel;
	Int_t    TSd2sChannel;
	Int_t    TSd1rChannel;
	Int_t    TSd1sChannel;
	Int_t    TSd2rChannel2;
	Int_t    TSd2sChannel2;
	Int_t    TSd1rChannel2;
	Int_t    TSd1sChannel2;
		
	Double_t TSurEnergy;
	Double_t TSurEnergy2;
	Int_t    TSurChannel;
	Int_t    TSurChannel2;
	Double_t TSusEnergy;
	Double_t TSusEnergy2;
	Int_t    TSusChannel;
	Int_t    TSusChannel2;
	Double_t TSuTheta;// Yd theta angle
	Double_t TYuEnergy;
	Double_t TYuEnergy2;
	Int_t    TYuChannel;
	Int_t    TYuChannel2;
	Double_t TYuTheta;// Yd theta angle


  void Clear();
 };

void  HandleMesytec(TMidasEvent& event, void* ptr, int wsize, int MYLABEL, det_t *pdet, hist_t *phist);
void  HandleBOR_Mesytec(int run, int time, det_t *pdet, hist_t *phist);
void  HandleEOR_Mesytec(int run, int time);

#endif
// end
