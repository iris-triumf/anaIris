#include "CalibMesytec.h"
#include <stdio.h>
#include <stdlib.h>

CalibMesytec::CalibMesytec(){
  	CalibMesytec::Clear();
}

void CalibMesytec::ReadFilenames(char* line)
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
	
	char* val=strchr(line,'=');
	if (!val){ 
		val=strchr(line, '!');
		expect_val=false;
	}
	if (!val) printf("Missing = or ! in input file, line: '%s'\n",line);
	*val=0;
	
	// trim param name
	char* trm=val-1;
	while (*trm<=32) *(trm--)=0;
	
	val++;
	if (*val==0 && expect_val) printf("Value missing for parameter %s",line);

	Double_t v;
	char cval[256];	
	std::string strval;
	sscanf(val,"%s",cval);
	sscanf(val,"%lf",&v);
	strval=cval;
	
	//	parameter of type string:
	if (strcmp(line,"PATH")==0){ 
		installPath = strval;
		fileGeometry = strval;
		fileHistos = strval;
		fileELoss = strval;
		fileIC = strval;
		fileCsI1 = strval;
		fileCsI2 = strval;
		fileSd1r = strval;
		fileSd1s = strval;
		fileSd2r = strval;
		fileSd2s = strval;
		fileSur = strval;
		fileSus = strval;
		fileYd = strval;
		fileYu = strval;
	}
	if (strcmp(line,"GEOMETRY")==0){
		fileGeometry += strval;
		boolGeometry =kTRUE;
	}
	if (strcmp(line,"HISTOS")==0){
		fileHistos += strval;
		boolHistos =kTRUE;
	}
	if (strcmp(line,"ELOSS")==0){
		fileELoss += strval;
		boolELoss =kTRUE;
	}
	if (strcmp(line,"IC")==0){
		fileIC += strval;
		boolIC =kTRUE;
	}
	if (strcmp(line,"CSI1")==0){
		fileCsI1 += strval;
		boolCsI1 =kTRUE;
	}
	if (strcmp(line,"CSI2")==0){
		fileCsI2 += strval;
		boolCsI2 =kTRUE;
	}
	if (strcmp(line,"SD1R")==0){
		fileSd1r += strval;
		boolSd1r =kTRUE;
	}
	if (strcmp(line,"SD1S")==0){
		fileSd1s += strval;
		boolSd1s =kTRUE;
	}
	if (strcmp(line,"SD2R")==0){
		fileSd2r += strval;
		boolSd2r =kTRUE;
	}
	if (strcmp(line,"SD2S")==0){
		fileSd2s += strval;
		boolSd2s =kTRUE;
	}
	if (strcmp(line,"SUR")==0){
		fileSur += strval;
		boolSur =kTRUE;
	}
	if (strcmp(line,"SUS")==0){
		fileSus += strval;
		boolSus =kTRUE;
	}
	if (strcmp(line,"YD")==0){
		fileYd += strval;
		boolYd =kTRUE;
	}
	if (strcmp(line,"YU")==0){
		fileYu += strval;
		boolYu =kTRUE;
	}
	if (strcmp(line,"ZDX")==0){
		fileZdx += strval;
		boolZdx =kTRUE;
	}
	if (strcmp(line,"ZDY")==0){
		fileZdy += strval;
		boolZdy =kTRUE;
	}
	if (strcmp(line,"ASCII")==0){
		boolASCII =kTRUE;
	}
	if (strcmp(line,"GATEFILE1")==0){
		fGate1 = strval;
		boolFGate1 =kTRUE;
	}
	if (strcmp(line,"GATEFILE2")==0){
		fGate2 = strval;
		boolFGate2 =kTRUE;
	}
	if (strcmp(line,"GATEFILE3")==0){
		fGate3 = strval;
		boolFGate3 =kTRUE;
	}
	if (strcmp(line,"GATEFILE4")==0){
		fGate4 = strval;
		boolFGate4 =kTRUE;
	}
	if (strcmp(line,"GATENAME1")==0){
		nGate1 = strval;
	}
	if (strcmp(line,"GATENAME2")==0){
		nGate2 = strval;
	}
	if (strcmp(line,"GATENAME3")==0){
		nGate3 = strval;
	}
	if (strcmp(line,"GATENAME4")==0){
		nGate4 = strval;
	}
	if (strcmp(line,"RUNDEPPAR")==0){
		fileRunDepPar = strval;
		boolRunDepPar = kTRUE;
	}
	if (strcmp(line,"DEDX_I")==0){
		fileIdedx = strval;
		boolIdedx = kTRUE;
	}
	if (strcmp(line,"DEDX_L")==0){
		fileLdedx = strval;
		boolLdedx = kTRUE;
	}
	if (strcmp(line,"DEDX_H")==0){
		fileHdedx = strval;
		boolHdedx = kTRUE;
	}

}

void CalibMesytec::Load(std::string filename){	

	boolGeometry=kFALSE;
	boolHistos=kFALSE;
	boolELoss=kFALSE;
	boolIC=kTRUE;
	boolCsI1=kFALSE;
	boolCsI2=kFALSE;
	boolSd1r=kFALSE;
	boolSd1s=kFALSE;
	boolSd2r=kFALSE;
	boolSd2s=kFALSE;
	boolSur=kFALSE;
	boolSus=kFALSE;
	boolYd=kFALSE;
	boolYu=kFALSE;
	boolASCII=kFALSE;
	boolRunDepPar=kFALSE;
	boolFGate1=kFALSE;
	boolFGate2=kFALSE;
	boolFGate3=kFALSE;
	boolFGate4=kFALSE;
	boolIdedx=kFALSE;
	boolLdedx=kFALSE;
	boolHdedx=kFALSE;
	
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
			default  :  ReadFilenames(ptr);
		}
	}
	fclose(file);
	file=NULL;
}

void CalibMesytec::Print(Int_t calledfrom){
	if(calledfrom==0){
		printf("\n********************************\n");
		printf("** Mesytec calibration files: **\n");
		if(boolGeometry)	printf("Experiment geometry: %s\n",fileGeometry.data());
		else	printf("No experiment geometry specified.\n");
		if(boolELoss)	printf("Energy loss root file: %s\n",fileELoss.data());
		else	printf("No energy loss root file specified.\n");
		if(boolIC)	printf("Ionization chamber calibration: %s\n",fileIC.data());
		else	printf("No ionization chamber calibration specified.\n");
		if(boolCsI1)	printf("CsI1 calibration: %s\n",fileCsI1.data());
		else	printf("No CsI1 calibration specified.\n");
		if(boolCsI2)	printf("CsI2 calibration: %s\n",fileCsI2.data());
		else	printf("No CsI2 calibration specified.\n");
		if(boolSd1r)	printf("Sd1r calibration: %s\n",fileSd1r.data());
		else	printf("No Sd1r calibration specified.\n");
		if(boolSd1s)	printf("Sd1s calibration: %s\n",fileSd1s.data());
		else	printf("No Sd1s calibration specified.\n");
		if(boolSd2r)	printf("Sd2r calibration: %s\n",fileSd2r.data());
		else	printf("No Sd2r calibration specified.\n");
		if(boolSd2s)	printf("Sd2s calibration: %s\n",fileSd2s.data());
		else	printf("No Sd2s calibration specified.\n");
		if(boolSur)	printf("Sur calibration: %s\n",fileSur.data());
		else	printf("No Sur calibration specified.\n");
		if(boolSus)	printf("Sus calibration: %s\n",fileSus.data());
		else	printf("No Sus calibration specified.\n");
		if(boolYd)	printf("Yd calibration: %s\n",fileYd.data());
		else	printf("No Yd calibration specified.\n");
		if(boolYu)	printf("Yu calibration: %s\n",fileYu.data());
		else	printf("No Yu calibration specified.\n");
		if(boolZdx)	printf("Zdx calibration: %s\n",fileZdx.data());
		else	printf("No Zdx calibration specified.\n");
		if(boolZdy)	printf("Zdy calibration: %s\n",fileZdy.data());
		else	printf("No Zdy calibration specified.\n");
		printf("********************************\n\n");
	}
	else{	
		if(boolFGate1){	
			printf("Gate 1:\n");
			printf("File: %s\n",fGate1.data());
			printf("Name: %s\n",nGate1.data());
		}
		else	printf("No Gate1 specified.\n");
		if(boolFGate2){	
			printf("Gate 2:\n");
			printf("File: %s\n",fGate2.data());
			printf("Name: %s\n",nGate2.data());
		}
		else	printf("No Gate2 specified.\n");
		if(boolFGate3){	
			printf("Gate 3:\n");
			printf("File: %s\n",fGate3.data());
			printf("Name: %s\n",nGate3.data());
		}
		else	printf("No Gate3 specified.\n");
		if(boolFGate4){	
			printf("Gate 4:\n");
			printf("File: %s\n",fGate4.data());
			printf("Name: %s\n",nGate4.data());
		}
		else	printf("No Gate4 specified.\n");
	}
}

void CalibMesytec::Clear(){
}
