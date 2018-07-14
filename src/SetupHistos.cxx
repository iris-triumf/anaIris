#include "SetupHistos.h"
#include <stdio.h>
#include <stdlib.h>

hist_t::hist_t(){
	for(Int_t i=0; i<512; i++){
		pADC[i].Set1D(Form("adc%02d",i),4000,0,4000);
	}
	for(Int_t i=0; i<16; i++){
		pIC[i].Set1D(Form("IC%02d",i),4000,0,4000);
		pCsI1[i].Set1D(Form("CsI1%02d",i),4000,0,4000);
		pCsI2[i].Set1D(Form("CsI2%02d",i),4000,0,4000);
	}
	pICSum.Set1D("ICSum",4000,0,4000);
	for(Int_t i=0; i<32; i++){
		pSd1r[i].Set1D(Form("Sd1r%02d",i),4000,0,4000);
		pSd1s[i].Set1D(Form("Sd1s%02d",i),4000,0,4000);
		pSd2r[i].Set1D(Form("Sd2r%02d",i),4000,0,4000);
		pSd2s[i].Set1D(Form("Sd2s%02d",i),4000,0,4000);
		pSur[i].Set1D(Form("Sur%02d",i),4000,0,4000);
		pSus[i].Set1D(Form("Sus%02d",i),4000,0,4000);
	}
	for(Int_t i=0; i<128; i++){
		pYd[i].Set1D(Form("Yd%02d",i),4000,0,4000);
		pYu[i].Set1D(Form("Yu%02d",i),4000,0,4000);
	}

	pCsI1Summary.Set2D("CsI1Summary",16,0,16,4000,0,4000);
	pCsI2Summary.Set2D("CsI2Summary",16,0,16,4000,0,4000);
	pSd1rSummary.Set2D("Sd1rSummary",24,0,24,4000,0,4000);
	pSd1sSummary.Set2D("Sd1sSummary",32,0,32,4000,0,4000);
	pSd2rSummary.Set2D("Sd2rSummary",24,0,24,4000,0,4000);
	pSd2sSummary.Set2D("Sd2sSummary",32,0,32,4000,0,4000);
	pSurSummary.Set2D("SurSummary",24,0,24,4000,0,4000);
	pSusSummary.Set2D("SusSummary",32,0,32,4000,0,4000);
	pYdSummary.Set2D("YdSummary",128,0,128,4000,0,4000);
	pYuSummary.Set2D("YuSummary",128,0,128,4000,0,4000);
	
	pCsI1Hits.Set1D("CsI1Hits",16,0,16);
	pCsI2Hits.Set1D("CsI2Hits",16,0,16);
	pSd1rHits.Set1D("Sd1rHits",24,0,24);
	pSd1sHits.Set1D("Sd1sHits",32,0,32);
	pSd1rElHits.Set1D("Sd1rElHits",24,0,24);
	pSd1sElHits.Set1D("Sd1sElHits",32,0,32);
	pSd2rHits.Set1D("Sd2rHits",24,0,24);
	pSd2sHits.Set1D("Sd2sHits",32,0,32);
	pSd2rElHits.Set1D("Sd2rElHits",24,0,24);
	pSd2sElHits.Set1D("Sd2sElHits",32,0,32);
	pSurHits.Set1D("SurHits",24,0,24);
	pSusHits.Set1D("SusHits",32,0,32);
	for(Int_t i=0; i<8; i++){
		pYdHits[i].Set1D(Form("YdHits%d",i+1),16,0,16);
	}
	pYdHitsAll.Set1D("YdHitsAll",128,0,128);
	pYdHitsProt.Set1D("YdHitsProt",128,0,128);
	for(Int_t i=0; i<8; i++){
		pYuHits[i].Set1D(Form("YuHits%d",i+1),16,0,16);
	}
	pYuHitsAll.Set1D("YuHitsAll",128,0,128);
	pYuHitsProt.Set1D("YuHitsProt",128,0,128);
	pYdCsI1Corr.Set2D("YdCsI1Corr",8,0,8,16,0,16);
	pYdCsI2Corr.Set2D("YdCsI2Corr",8,0,8,16,0,16);

	pCsI1Energy.Set1D("CsI1Energy",4000,0,4000);
	pCsI2Energy.Set1D("CsI2Energy",4000,0,4000);
	pSd1rEnergy.Set1D("Sd1rEnergy",4000,0,4000);
	pSd1sEnergy.Set1D("Sd1sEnergy",4000,0,4000);
	pSd2rEnergy.Set1D("Sd2rEnergy",4000,0,4000);
	pSd2sEnergy.Set1D("Sd2sEnergy",4000,0,4000);
	pSurEnergy.Set1D("SurEnergy",4000,0,4000);
	pSusEnergy.Set1D("SusEnergy",4000,0,4000);
	pYdEnergy.Set1D("YdEnergy",4000,0,4000);
	pYuEnergy.Set1D("YuEnergy",4000,0,4000);
	pSSBEnergy.Set1D("SSBEnergy",4000,0,4000);
	pSdETot.Set1D("SdETot",4000,0,4000);
	pSdETotMonitor.Set1D("SdETotMonitor",4000,0,4000);
	pYdCsI1ETot.Set1D("YdCsI1ETot",4000,0,4000);
	pYdCsI2ETot.Set1D("YdCsI2ETot",4000,0,4000);
	
	pYdCsI1PID.Set2D("YdCsI1PID",4000,0,4000,4000,0,4000);
	pYdCsI2PID.Set2D("YdCsI2PID",4000,0,4000,4000,0,4000);
	pSdPID.Set2D("SdPID",4000,0,4000,4000,0,4000);
	pYdCsI1PID_adc.Set2D("YdCsI1PID_adc",4000,0,4000,4000,0,4000);
	pYdCsI2PID_adc.Set2D("YdCsI2PID_adc",4000,0,4000,4000,0,4000);
	pYdCsI1PID_adc2.Set2D("YdCsI1PID_adc2",4000,0,4000,4000,0,4000);
	pYdCsI2PID_adc2.Set2D("YdCsI2PID_adc2",4000,0,4000,4000,0,4000);
	pYdRange.Set2D("YdRange",4000,0,4000,4000,0,4000);
	pCsI1Range.Set2D("CsI1Range",4000,0,4000,4000,0,4000);
	pCsI2Range.Set2D("CsI2Range",4000,0,4000,4000,0,4000);
	
	pSdTheta.Set1D("SdTheta",360,0,360);
	pSdPhi.Set1D("SdPhi",360,0,360);
	pSdETheta.Set2D("SdETheta",360,0,360,4000,0,4000);
	pSdPhiTheta.Set2D("SdPhiTheta",360,0,360,360,0,360);
	pSuTheta.Set1D("SuTheta",360,0,360);
	pSuPhi.Set1D("SuPhi",360,0,360);
	pSuETheta.Set2D("SuETheta",360,0,360,4000,0,4000);
	pSuPhiTheta.Set2D("SuPhiTheta",360,0,360,360,0,360);
	pYdTheta.Set1D("YdTheta",360,0,360);
	pYdETheta.Set2D("YdETheta",360,0,360,360,0,360);
	pYuTheta.Set1D("YuTheta",360,0,360);
	pYuETheta.Set2D("YuETheta",360,0,360,360,0,360);
	
	pTRIFIC_1.Set1D("TRIFIC_1",4000,0,4000);
	pTRIFIC_2.Set1D("TRIFIC_2",4000,0,4000);
	pTRIFIC_3.Set1D("TRIFIC_3",4000,0,4000);
	pTRIFIC_1v2.Set2D("TRIFIC_1v2",4000,0,4000,4000,0,4000);
	pTRIFIC_1v3.Set2D("TRIFIC_1v3",4000,0,4000,4000,0,4000);
	pTRIFIC_2v3.Set2D("TRIFIC_2v3",4000,0,4000,4000,0,4000);
	pTRIFIC_1v23.Set2D("TRIFIC_1v23",8000,0,8000,4000,0,4000);
	pTRIFIC_12v3.Set2D("TRIFIC_12v3",4000,0,4000,8000,0,8000);
	
	for(Int_t i=0; i<512; i++){
		pV1190_T[i].Set1D(Form("TimeRaw%02d",i),4000,0,40000);
		pTime[i].Set1D(Form("Time%02d",i),4000,-1000,1000);
		pTimeRF[i].Set1D(Form("TimeRF%02d",i),4000,-1000,1000);
	}
	pTall.Set1D("dTall",4000,-1000,1000);
	pTall2D.Set2D("dTall2D",512,0,512,4000,-1000,1000);

	pQValue1.Set1D("QValue1",800,-20,20);
	pQValue2.Set1D("QValue2",800,-20,20);
	pQValueU.Set1D("QValueU",800,-20,20);
	pYdCsIEnergyTime.Set2D("YdCsIEnergyTime",512,0,1024,400,0,20);
	pS3EnergyTime.Set2D("S3EnergyTime",512,0,1024,400,0,200);
	pYdCsI1Theta.Set2D("YdCsI1Theta",900,0,90,400,0,20);
	pYdCsI2Theta.Set2D("YdCsI2Theta",90,0,90,400,0,20);
	pYuEnergyTheta.Set2D("YuEnergyTheta",90,0,90,400,0,20);
	pYdCsIThetaProt.Set2D("YdCsIThetaProt",90,0,90,400,0,20);
}
    		

void hist_t::ReadParams(char* line)
{
	bool expect_val=true;
	char *from=line;
	char *to=line;
	while (*from) {
		if (*from>32) {*to=*from;to++;}
		from++;
	}
	*to=0;
	if (*line==0) return; // line is empty
	
	char* val=strchr(line,',');
	if (!val) printf("Missing value in input file, line: '%s'\n",line);
	*val=0;
	
	// trim param name
	char* trm=val-1;
	while (*trm<=32) *(trm--)=0;
	
	val++;
	if (*val==0 && expect_val) printf("Value missing for parameter %s",line);

	Int_t nbinx=0., nbiny=0.;
	Double_t xmin=0., xmax=0., ymin=0., ymax=0.;
	sscanf(val,"%d,%lf,%lf,%d,%lf,%lf",&nbinx,&xmin,&xmax,&nbiny,&ymin,&ymax);
	
	//	parameter of type string:
	if (strcmp(line,"IC")==0){
		for(Int_t i=0; i<16; i++){
			pIC[i].Set1D(Form("IC%02d",i),nbinx,xmin,xmax);
		}
	}
	if (strcmp(line,"CsI1")==0){
		for(Int_t i=0; i<16; i++){
			pCsI1[i].Set1D(Form("CsI1%02d",i),nbinx,xmin,xmax);
		}
	}
	if (strcmp(line,"CsI2")==0){
		for(Int_t i=0; i<16; i++){
			pCsI2[i].Set1D(Form("CsI2%02d",i),nbinx,xmin,xmax);
		}
	}
	if (strcmp(line,"ICSum")==0){
		pICSum.Set1D("ICSum",nbinx,xmin,xmax);
	}
	if (strcmp(line,"Sd1r")==0){
		for(Int_t i=0; i<32; i++){
			pSd1r[i].Set1D(Form("Sd1r%02d",i),nbinx,xmin,xmax);
		}
	}
	if (strcmp(line,"Sd1s")==0){
		for(Int_t i=0; i<32; i++){
			pSd1s[i].Set1D(Form("Sd1s%02d",i),nbinx,xmin,xmax);
		}
	}
	if (strcmp(line,"Sd2r")==0){
		for(Int_t i=0; i<32; i++){
			pSd2r[i].Set1D(Form("Sd2r%02d",i),nbinx,xmin,xmax);
		}
	}
	if (strcmp(line,"Sd2s")==0){
		for(Int_t i=0; i<32; i++){
			pSd2s[i].Set1D(Form("Sd2s%02d",i),nbinx,xmin,xmax);
		}
	}
	if (strcmp(line,"Sur")==0){
		for(Int_t i=0; i<32; i++){
			pSur[i].Set1D(Form("Sur%02d",i),nbinx,xmin,xmax);
		}
	}
	if (strcmp(line,"Sus")==0){
		for(Int_t i=0; i<32; i++){
			pSus[i].Set1D(Form("Sus%02d",i),nbinx,xmin,xmax);
		}
	}
	if (strcmp(line,"Yd")==0){
		for(Int_t i=0; i<128; i++){
			pYd[i].Set1D(Form("Yd%02d",i),nbinx,xmin,xmax);
		}
	}
	if (strcmp(line,"Yu")==0){
		for(Int_t i=0; i<128; i++){
			pYu[i].Set1D(Form("Yu%02d",i),nbinx,xmin,xmax);
		}
	}
	if (strcmp(line,"CsI1Summary")==0){
		pCsI1Summary.Set2D("CsI1Summary",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"CsI2Summary")==0){
		pCsI2Summary.Set2D("CsI2Summary",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"Sd1rSummary")==0){
		pSd1rSummary.Set2D("Sd1rSummary",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"Sd1sSummary")==0){
		pSd1sSummary.Set2D("Sd1sSummary",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"Sd2rSummary")==0){
		pSd2rSummary.Set2D("Sd2rSummary",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"Sd2sSummary")==0){
		pSd2sSummary.Set2D("Sd2sSummary",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"SurSummary")==0){
		pSurSummary.Set2D("SurSummary",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"SusSummary")==0){
		pSusSummary.Set2D("SusSummary",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YdSummary")==0){
		pYdSummary.Set2D("YdSummary",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YuSummary")==0){
		pYuSummary.Set2D("YuSummary",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"CsI1Energy")==0){
		pCsI1Energy.Set1D("CsI1Energy",nbinx,xmin,xmax);
	}
	if (strcmp(line,"CsI2Energy")==0){
		pCsI2Energy.Set1D("CsI2Energy",nbinx,xmin,xmax);
	}
	if (strcmp(line,"Sd1rEnergy")==0){
		pSd1rEnergy.Set1D("Sd1rEnergy",nbinx,xmin,xmax);
	}
	if (strcmp(line,"Sd1sEnergy")==0){
		pSd1sEnergy.Set1D("Sd1sEnergy",nbinx,xmin,xmax);
	}
	if (strcmp(line,"Sd2rEnergy")==0){
		pSd2rEnergy.Set1D("Sd2rEnergy",nbinx,xmin,xmax);
	}
	if (strcmp(line,"Sd2sEnergy")==0){
		pSd2sEnergy.Set1D("Sd2sEnergy",nbinx,xmin,xmax);
	}
	if (strcmp(line,"SurEnergy")==0){
		pSurEnergy.Set1D("SurEnergy",nbinx,xmin,xmax);
	}
	if (strcmp(line,"SusEnergy")==0){
		pSusEnergy.Set1D("SusEnergy",nbinx,xmin,xmax);
	}
	if (strcmp(line,"YdEnergy")==0){
		pYdEnergy.Set1D("YdEnergy",nbinx,xmin,xmax);
	}
	if (strcmp(line,"YuEnergy")==0){
		pYuEnergy.Set1D("YuEnergy",nbinx,xmin,xmax);
	}
	if (strcmp(line,"SSBEnergy")==0){
		pSSBEnergy.Set1D("SSBEnergy",nbinx,xmin,xmax);
	}
	if (strcmp(line,"SdETot")==0){
		pSdETot.Set1D("SdETot",nbinx,xmin,xmax);
	}
	if (strcmp(line,"SdETotMonitor")==0){
		pSdETotMonitor.Set1D("SdETotMonitor",nbinx,xmin,xmax);
	}
	if (strcmp(line,"YdCsI1ETot")==0){
		pYdCsI1ETot.Set1D("YdCsI1ETot",nbinx,xmin,xmax);
	}
	if (strcmp(line,"YdCsI2ETot")==0){
		pYdCsI2ETot.Set1D("YdCsI2ETot",nbinx,xmin,xmax);
	}
	if (strcmp(line,"YdCsI1PID")==0){
		pYdCsI1PID.Set2D("YdCsI1PID",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YdCsI2PID")==0){
		pYdCsI2PID.Set2D("YdCsI2PID",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"SdPID")==0){
		pSdPID.Set2D("SdPID",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YdCsI1PID_adc")==0){
		pYdCsI1PID_adc.Set2D("YdCsI1PID_adc",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YdCsI2PID_adc")==0){
		pYdCsI2PID_adc.Set2D("YdCsI2PID_adc",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YdCsI1PID_adc2")==0){
		pYdCsI1PID_adc2.Set2D("YdCsI1PID_adc2",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YdCsI2PID_adc2")==0){
		pYdCsI2PID_adc2.Set2D("YdCsI2PID_adc2",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YdRange")==0){
		pYdRange.Set2D("YdRange",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"CsI1Range")==0){
		pCsI1Range.Set2D("CsI1Range",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"CsI2Range")==0){
		pCsI2Range.Set2D("CsI2Range",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YdTheta")==0){
		pYdTheta.Set1D("YdTheta",nbinx,xmin,xmax);
	}
	if (strcmp(line,"YdETheta")==0){
		pYdETheta.Set2D("YdETheta",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"SdTheta")==0){
		pSdTheta.Set1D("SdTheta",nbinx,xmin,xmax);
	}
	if (strcmp(line,"SdPhi")==0){
		pSdPhi.Set1D("SdPhi",nbinx,xmin,xmax);
	}
	if (strcmp(line,"SdPhiTheta")==0){
		pSdPhiTheta.Set2D("SdPhiTheta",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"SdETheta")==0){
		pSdETheta.Set2D("SdETheta",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YuTheta")==0){
		pYuTheta.Set1D("YuTheta",nbinx,xmin,xmax);
	}
	if (strcmp(line,"YuETheta")==0){
		pYuETheta.Set2D("YuETheta",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"SuTheta")==0){
		pSuTheta.Set1D("SuTheta",nbinx,xmin,xmax);
	}
	if (strcmp(line,"SuETheta")==0){
		pSuETheta.Set2D("SuETheta",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"SuPhi")==0){
		pSuPhi.Set1D("SuPhi",nbinx,xmin,xmax);
	}
	if (strcmp(line,"SuPhiTheta")==0){
		pSuPhiTheta.Set2D("SuPhiTheta",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"Time")==0){
		for(Int_t i=0; i<512; i++){
			pTime[i].Set1D(Form("Time%02d",i),nbinx,xmin,xmax);
		}
	}
	if (strcmp(line,"TimeRF")==0){
		for(Int_t i=0; i<512; i++){
			pTimeRF[i].Set1D(Form("TimeRF%02d",i),nbinx,xmin,xmax);
		}
	}
	if (strcmp(line,"Tall")==0){
		pTall.Set1D("dTall",nbinx,xmin,xmax);
	}
	if (strcmp(line,"Tall2D")==0){
		pTall2D.Set2D("dTall2D",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"QValue1")==0){
		pQValue1.Set1D("QValue1",nbinx,xmin,xmax);
	}
	if (strcmp(line,"QValue2")==0){
		pQValue2.Set1D("QValue2",nbinx,xmin,xmax);
	}
	if (strcmp(line,"QValueU")==0){
		pQValueU.Set1D("QValueU",nbinx,xmin,xmax);
	}
	if (strcmp(line,"YdCsIEnergyTime")==0){
		pYdCsIEnergyTime.Set2D("YdCsIEnergyTime",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"S3EnergyTime")==0){
		pS3EnergyTime.Set2D("S3EnergyTime",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YdCsI1Theta")==0){
		pYdCsI1Theta.Set2D("YdCsI1Theta",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YdCsI2Theta")==0){
		pYdCsI2Theta.Set2D("YdCsI2Theta",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YuEnergyTheta")==0){
		pYuEnergyTheta.Set2D("YuEnergyTheta",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YdCsIThetaProt")==0){
		pYdCsIThetaProt.Set2D("YdCsIThetaProt",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}

}

void hist_t::LoadParams(std::string filename){	

	char line[256];
	FILE* file=fopen(filename.data(),"rb");
	if (!file)
	{
		printf("Cannot open config file '%s' for reading. Stop.\n",filename.data());
		exit(0);
	}
	
	printf("Reading config file '%s'\n",filename.data());
	
	while (!feof(file))
	{
		if (!fgets(line,256,file)) break;
		printf("%s",line);
		// skip leading white spaces
		char* ptr=line;
		while ((*ptr>0) && (*ptr<32)) ptr++;
		//printf("%s\n",ptr[0]);
		switch (ptr[0])
		{
			case 0   :
			case '#' :
			case '/' :  continue;
			default  :  ReadParams(ptr);
		}
	}
	fclose(file);
	file=NULL;
}

void hist_t::Print(){

}

void hist_t::Book()
{
	hADC[0] = (TH1F*)gDirectory->Get("adc00");
	if (hADC[0] == 0) {
  		TDirectory* adc_dir = gOutputFile->mkdir("adc");      
  		adc_dir->cd();

  		TDirectory* adcIC_dir = adc_dir->mkdir("adcIC");
  		TDirectory* adcCsI_dir = adc_dir->mkdir("adcCsI");
  		TDirectory* adcSd2_dir = adc_dir->mkdir("adcSd2");
  		TDirectory* adcSd1_dir = adc_dir->mkdir("adcSd1");
  		TDirectory* adcYYd_dir = adc_dir->mkdir("adcYYd");
  		TDirectory* adcSu_dir = adc_dir->mkdir("adcSu");
  		TDirectory* adcYYu_dir = adc_dir->mkdir("adcYYU");

  		printf("Booking ADC histograms.\n");
  		for (int i=0;i<512;i++) {
			if (i<32) adcIC_dir->cd();
			else if (i>=32 && i <64) adcCsI_dir->cd();
			else if (i>=64 && i <128) adcSd2_dir->cd();
			else if (i>=128 && i <192) adcSd1_dir->cd();
			else if(i>=192 && i < 320) adcYYd_dir->cd();
			else if(i>=320 && i < 384) adcSu_dir->cd();
    		else if(i>=384 && i < 512) adcYYu_dir->cd();

	 		hADC[i] = pADC[i].SetupTH1F();
  		}
	}
		
	hIC[0] = (TH1F*)gDirectory->Get("IC00");
	if (hIC[0] == 0) {
    	TDirectory* IC_dir = gOutputFile->mkdir("IC");// Make an IC directory and cd to it.
    	IC_dir->cd();
		printf("Booking IC histograms.\n");
   		for (int i=0;i<16;i++) {
	 		hIC[i] = pIC[i].SetupTH1F();
   		}
	 	hICSum = pICSum.SetupTH1F();
	}
	
	hCsI1[0] = (TH1F*)gDirectory->Get("CsI00");
	if (hCsI1[0] == 0) {
		TDirectory* CsI_dir = gOutputFile->mkdir("CsI");
		CsI_dir->cd();
		printf("Booking CsI1 histograms.\n");
		for (int i=0;i<16;i++) {
	 		hCsI1[i] = pCsI1[i].SetupTH1F();
		}
		for (int i=0;i<16;i++) {
	 		hCsI2[i] = pCsI2[i].SetupTH1F();
		}
	}
 
	hSd1r[0] = (TH1F*)gDirectory->Get("Sd1r00");
	if (hSd1r[0] == 0) {
 		TDirectory* Sd1r_dir = gOutputFile->mkdir("Sd1r");
 		Sd1r_dir->cd();
   		printf("Booking Sd1r histograms.\n");
   		for (int i=0;i<32;i++) {
	 		hSd1r[i] = pSd1r[i].SetupTH1F();
   		}
	}

	hSd1s[0] = (TH1F*)gDirectory->Get("Sd1s00");
	if (hSd1s[0] == 0) {
 		TDirectory* Sd1s_dir = gOutputFile->mkdir("Sd1s");
 		Sd1s_dir->cd();
   		printf("Booking Sd1s histograms.\n");
   		for (int i=0;i<32;i++) {
	 		hSd1s[i] = pSd1s[i].SetupTH1F();
   		}
	}	

	hSd2r[0] = (TH1F*)gDirectory->Get("Sd2r00");
	if (hSd2r[0] == 0) {
	   	TDirectory* Sd2r_dir = gOutputFile->mkdir("Sd2r");
 		Sd2r_dir->cd();
		printf("Booking Sd2r histograms.\n");
	   	for (int i=0;i<32;i++) {
	 		hSd2r[i] = pSd2r[i].SetupTH1F();
		}
	}

	hSd2s[0] = (TH1F*)gDirectory->Get("Sd2s00");
	if (hSd2s[0] == 0) {
	   	TDirectory* Sd2s_dir = gOutputFile->mkdir("Sd2s");
 		Sd2s_dir->cd();
	   	printf("Booking Sd2s histograms.\n");
	   	for (int i=0;i<32;i++) {
	 		hSd2s[i] = pSd2s[i].SetupTH1F();
		}
	}

	hSur[0] = (TH1F*)gDirectory->Get("Sur00");
	if (hSur[0] == 0) {
     	TDirectory* Sur_dir = gOutputFile->mkdir("Sur");
     	Sur_dir->cd();
   		printf("Booking Sur histograms.\n");
   		for (int i=0;i<32;i++) {
	 		hSur[i] = pSur[i].SetupTH1F();
   		}
	}
		
	hSus[0] = (TH1F*)gDirectory->Get("Sus00");
	if (hSus[0] == 0) {
     	TDirectory* Sus_dir = gOutputFile->mkdir("Sus");
     	Sus_dir->cd();
   		printf("Booking Sus histograms.\n");
   		for (int i=0;i<32;i++) {
	 		hSus[i] = pSus[i].SetupTH1F();
   		}
	}

	hYd[0] = (TH1F*)gDirectory->Get("Yd00");
	if (hYd[0] == 0) {
       	TDirectory* Yd_dir = gOutputFile->mkdir("Yd");      
       	Yd_dir->cd();       
		printf("Booking Yd histograms.\n");
   		for (int i=0;i<128;i++) {
	 		hYd[i] = pYd[i].SetupTH1F();
   		}
	}

	hYu[0] = (TH1F*)gDirectory->Get("Yu00");
	if (hYu[0] == 0) {
       	TDirectory* Yu_dir = gOutputFile->mkdir("Yu");      
       	Yu_dir->cd();       
		printf("Booking Yu histograms.\n");
   		for (int i=0;i<128;i++) {
	 		hYu[i] = pYu[i].SetupTH1F();
   		}
	}
	gOutputFile->cd();
		
	// == Summary spectra === //
	printf("Booking summary histograms\n");
	hCsI1Summary = (TH2F*)gDirectory->Get("CsI1Summary");
	if (hCsI1Summary == 0) {
    	TDirectory* summary_dir = gOutputFile->mkdir("Summaries");      
    	summary_dir->cd();
		 
	 	hCsI1Summary = pCsI1Summary.SetupTH2F();
	 	hCsI2Summary = pCsI2Summary.SetupTH2F();
	 	hSd1rSummary = pSd1rSummary.SetupTH2F();
	 	hSd1sSummary = pSd1sSummary.SetupTH2F();
	 	hSd2rSummary = pSd2rSummary.SetupTH2F();
	 	hSd2sSummary = pSd2sSummary.SetupTH2F();
	 	hSurSummary = pSurSummary.SetupTH2F();
	 	hSusSummary = pSusSummary.SetupTH2F();
	 	hYdSummary = pYdSummary.SetupTH2F();
	 	hYuSummary = pYuSummary.SetupTH2F();
	}

	// ==== Hit Patterns ==== //
	printf("Booking hit pattern histograms.\n");
	hSd2rHits = (TH1F*)gDirectory->Get("Sd2rHits");
	if (hSd2rHits == 0) {
    	TDirectory* hitPattern_dir = gOutputFile->mkdir("Hit Patterns");      
    	hitPattern_dir->cd();
		
	 	hCsI1Hits = pCsI1Hits.SetupTH1F();
	 	hCsI2Hits = pCsI2Hits.SetupTH1F();
	 	hSd1rHits = pSd1rHits.SetupTH1F();
	 	hSd1sHits = pSd1sHits.SetupTH1F();
	 	hSd2rHits = pSd2rHits.SetupTH1F();
	 	hSd2sHits = pSd2sHits.SetupTH1F();
	 	hSurHits = pSurHits.SetupTH1F();
	 	hSusHits = pSusHits.SetupTH1F();
		hSd1rElHits = pSd1rElHits.SetupTH1F();
	 	hSd1sElHits = pSd1sElHits.SetupTH1F();
	 	hSd2rElHits = pSd2rElHits.SetupTH1F();
	 	hSd2sElHits = pSd2sElHits.SetupTH1F();
		for(int i=0; i<8; i++){	
	 		hYdHits[i] = pYdHits[i].SetupTH1F();
		}
	 	hYdHitsAll = pYdHitsAll.SetupTH1F();
	 	hYdHitsProt = pYdHitsProt.SetupTH1F();
		for(int i=0; i<8; i++){	
	 		hYuHits[i] = pYuHits[i].SetupTH1F();
		}
	 	hYuHitsAll = pYuHitsAll.SetupTH1F();
	 	hYuHitsProt = pYuHitsProt.SetupTH1F();
	 	hYdCsI1Corr = pYdCsI1Corr.SetupTH2F();
	 	hYdCsI2Corr = pYdCsI2Corr.SetupTH2F();
	}

	// ==== Energies     ==== //
	printf("Booking Energy histograms.\n");
	hYdEnergy = (TH1F*)gDirectory->Get("YdEnergy");
	if (hYdEnergy == 0) {
    	TDirectory* Energy_dir = gOutputFile->mkdir("Energies");      
    	Energy_dir->cd();

	 	hYdEnergy = pYdEnergy.SetupTH1F();
	 	hYuEnergy = pYuEnergy.SetupTH1F();
	 	hCsI1Energy = pCsI1Energy.SetupTH1F();
	 	hCsI2Energy = pCsI2Energy.SetupTH1F();
	 	hSd1rEnergy = pSd1rEnergy.SetupTH1F();
	 	hSd1sEnergy = pSd1sEnergy.SetupTH1F();
	 	hSd2rEnergy = pSd2rEnergy.SetupTH1F();
	 	hSd2sEnergy = pSd2sEnergy.SetupTH1F();
	 	hSurEnergy = pSurEnergy.SetupTH1F();
	 	hSusEnergy = pSusEnergy.SetupTH1F();
	 	hSSBEnergy = pSSBEnergy.SetupTH1F();
	 	hSdETot = pSdETot.SetupTH1F();
	 	hSdETotMonitor = pSdETotMonitor.SetupTH1F();
	 	hYdCsI1ETot = pYdCsI1ETot.SetupTH1F();
	 	hYdCsI2ETot = pYdCsI2ETot.SetupTH1F();
	}

	// ==== PID Spectra  ==== //
   	printf("Booking PID histograms.\n");
	hYdCsI1PID = (TH2F*)gDirectory->Get("YdCsI1PID");
	if (hYdCsI1PID == 0) {
    	TDirectory* PID_dir = gOutputFile->mkdir("PID");      
    	PID_dir->cd();

	 	hYdCsI1PID = pYdCsI1PID.SetupTH2F();
	 	hYdCsI2PID = pYdCsI2PID.SetupTH2F();
	 	hSdPID = pSdPID.SetupTH2F();
	 	hYdCsI1PID_adc = pYdCsI1PID_adc.SetupTH2F();
	 	hYdCsI2PID_adc = pYdCsI2PID_adc.SetupTH2F();
	 	hYdCsI1PID_adc2 = pYdCsI1PID_adc2.SetupTH2F();
	 	hYdCsI2PID_adc2 = pYdCsI2PID_adc2.SetupTH2F();
	 	hYdRange = pYdRange.SetupTH2F();
	 	hCsI1Range = pCsI1Range.SetupTH2F();
	 	hCsI2Range = pCsI2Range.SetupTH2F();
	}

	// ====    Angles    ==== //
	printf("Booking angle histograms.\n");
	hYdTheta = (TH1F*)gDirectory->Get("YdTheta");
	if (hYdTheta == 0) {
    	TDirectory* Angle_dir = gOutputFile->mkdir("Angles");      
    	Angle_dir->cd();

	 	hYdTheta = pYdTheta.SetupTH1F();
	 	hYdETheta = pYdETheta.SetupTH2F();
	 	hSdTheta = pSdTheta.SetupTH1F();
	 	hSdPhi = pSdPhi.SetupTH1F();
	 	hSdPhiTheta = pSdPhiTheta.SetupTH2F();
	 	hSdETheta = pSdETheta.SetupTH2F();
	 	hYuTheta = pYuTheta.SetupTH1F();
	 	hYuETheta = pYuETheta.SetupTH2F();
	 	hSuTheta = pSuTheta.SetupTH1F();
	 	hSuETheta = pSuETheta.SetupTH2F();
	 	hSuPhi = pSuPhi.SetupTH1F();
	 	hSuPhiTheta = pSuPhiTheta.SetupTH2F();
 	}

	// ====    TRIFIC    ==== //
	printf("Booking TRIFIC histograms.\n");
	hTRIFIC_1 = (TH1F*)gDirectory->Get("TRIFIC_1");
	if (hTRIFIC_1 == 0) {
    	TDirectory* TRIFIC_dir = gOutputFile->mkdir("TRIFIC");      
    	TRIFIC_dir->cd();

	 	hTRIFIC_1 = pTRIFIC_1.SetupTH1F();
	 	hTRIFIC_2 = pTRIFIC_2.SetupTH1F();
	 	hTRIFIC_3 = pTRIFIC_3.SetupTH1F();
	 	hTRIFIC_1v2 = pTRIFIC_1v2.SetupTH2F();
	 	hTRIFIC_1v3 = pTRIFIC_1v3.SetupTH2F();
	 	hTRIFIC_2v3 = pTRIFIC_2v3.SetupTH2F();
	 	hTRIFIC_1v23 = pTRIFIC_1v23.SetupTH2F();
	 	hTRIFIC_12v3 = pTRIFIC_12v3.SetupTH2F();
	}
		
	// ==== TDC Spectra ==== //
	printf("Booking TDC histograms.\n");
	hV1190_T[0] = (TH1F*)gDirectory->Get("TimeRaw00");
	if (hV1190_T[0] == 0)
	{
		TDirectory* TimeRaw_dir = gOutputFile->mkdir("TimeRaw");
		TimeRaw_dir->cd();
	
  		TDirectory* TimeRawIC_dir = TimeRaw_dir->mkdir("TimeRawIC");
  		TDirectory* TimeRawCsI_dir = TimeRaw_dir->mkdir("TimeRawCsI");
  		TDirectory* TimeRawSd2_dir = TimeRaw_dir->mkdir("TimeRawSd2");
  		TDirectory* TimeRawSd1_dir = TimeRaw_dir->mkdir("TimeRawSd1");
  		TDirectory* TimeRawYYd_dir = TimeRaw_dir->mkdir("TimeRawYYd");
  		TDirectory* TimeRawSu_dir = TimeRaw_dir->mkdir("TimeRawSu");
  		TDirectory* TimeRawYYu_dir = TimeRaw_dir->mkdir("TimeRawYYU");
		for (int i=0;i<512;i++) {
			if (i<32) TimeRawIC_dir->cd();
			else if (i>=32 && i <64) TimeRawCsI_dir->cd();
			else if (i>=64 && i <128) TimeRawSd2_dir->cd();
			else if (i>=128 && i <192) TimeRawSd1_dir->cd();
			else if(i>=192 && i < 320) TimeRawYYd_dir->cd();
			else if(i>=320 && i < 384) TimeRawSu_dir->cd();
    		else if(i>=384 && i < 512) TimeRawYYu_dir->cd();

	 		hV1190_T[i] = pV1190_T[i].SetupTH1F();
  		}
  	}
	hTime[0] = (TH1F*)gDirectory->Get("Time00");
	if (hTime[0] == 0)
	{	
		TDirectory* Time_dir = gOutputFile->mkdir("Time");
		Time_dir->cd();
	
  		TDirectory* TimeIC_dir = Time_dir->mkdir("TimeIC");
  		TDirectory* TimeCsI_dir = Time_dir->mkdir("TimeCsI");
  		TDirectory* TimeSd2_dir = Time_dir->mkdir("TimeSd2");
  		TDirectory* TimeSd1_dir = Time_dir->mkdir("TimeSd1");
  		TDirectory* TimeYYd_dir = Time_dir->mkdir("TimeYYd");
  		TDirectory* TimeSu_dir = Time_dir->mkdir("TimeSu");
  		TDirectory* TimeYYu_dir = Time_dir->mkdir("TimeYYU");
	
		for (int i=0;i<512;i++) {
			if (i<32) TimeIC_dir->cd();
			else if (i>=32 && i <64) TimeCsI_dir->cd();
			else if (i>=64 && i <128) TimeSd2_dir->cd();
			else if (i>=128 && i <192) TimeSd1_dir->cd();
			else if(i>=192 && i < 320) TimeYYd_dir->cd();
			else if(i>=320 && i < 384) TimeSu_dir->cd();
    		else if(i>=384 && i < 512) TimeYYu_dir->cd();

	 		hTime[i] = pTime[i].SetupTH1F();
  		}
  			
		Time_dir->cd();
		hTall = pTall.SetupTH1F();
		hTall2D = pTall2D.SetupTH2F();

	}
	
	hTimeRF[0] = (TH1F*)gDirectory->Get("TimeRF00");
	if (hTimeRF[0] == 0)
	{
		TDirectory* TimeRF_dir = gOutputFile->mkdir("TimeRF");
		TimeRF_dir->cd();
	
  		TDirectory* TimeRFIC_dir = TimeRF_dir->mkdir("TimeRFIC");
  		TDirectory* TimeRFCsI_dir = TimeRF_dir->mkdir("TimeRFCsI");
  		TDirectory* TimeRFSd2_dir = TimeRF_dir->mkdir("TimeRFSd2");
  		TDirectory* TimeRFSd1_dir = TimeRF_dir->mkdir("TimeRFSd1");
  		TDirectory* TimeRFYYd_dir = TimeRF_dir->mkdir("TimeRFYYd");
  		TDirectory* TimeRFSu_dir = TimeRF_dir->mkdir("TimeRFSu");
  		TDirectory* TimeRFYYu_dir = TimeRF_dir->mkdir("TimeRFYYU");
	
		for (int i=0;i<512;i++) {
			if (i<32) TimeRFIC_dir->cd();
			else if (i>=32 && i <64) TimeRFCsI_dir->cd();
			else if (i>=64 && i <128) TimeRFSd2_dir->cd();
			else if (i>=128 && i <192) TimeRFSd1_dir->cd();
			else if(i>=192 && i < 320) TimeRFYYd_dir->cd();
			else if(i>=320 && i < 384) TimeRFSu_dir->cd();
    		else if(i>=384 && i < 512) TimeRFYYu_dir->cd();

	 		hTimeRF[i] = pTimeRF[i].SetupTH1F();
  		}
  	}
	
	// ==== PHYSICS ==== //
    printf("Booking PHYSICS histograms.\n");
	hQValue1 = (TH1F*)gDirectory->Get("QValue1");
   	if (hQValue1 == 0) {
    	TDirectory* PHYSICS_dir = gOutputFile->mkdir("PHYSICS");      
    	PHYSICS_dir->cd();
 
	 	hQValue1 = pQValue1.SetupTH1F();
	 	hQValue2 = pQValue2.SetupTH1F();
	 	hQValueU = pQValueU.SetupTH1F();
	 	hYdCsIEnergyTime = pYdCsIEnergyTime.SetupTH2F();
	 	hYdCsI1Theta = pYdCsI1Theta.SetupTH2F();
	 	hYdCsI2Theta = pYdCsI2Theta.SetupTH2F();
	 	hYdCsIThetaProt = pYdCsIThetaProt.SetupTH2F();
	 	hYuEnergyTheta = pYuEnergyTheta.SetupTH2F();
	 	hS3EnergyTime = pS3EnergyTime.SetupTH2F();
    }
}

void hist_t::Clear(){
}
