#include "SetupHistos.h"
#include <stdio.h>
#include <stdlib.h>

SetupHistos::SetupHistos(){
	CsI1Energy.Set("CsI1Energy",4000,0,4000,0,0,0);
	CsI2Energy.Set("CsI2Energy",4000,0,4000,0,0,0);
	Sd1rEnergy.Set("Sd1rEnergy",4000,0,4000,0,0,0);
	Sd1sEnergy.Set("Sd1sEnergy",4000,0,4000,0,0,0);
	Sd2rEnergy.Set("Sd2rEnergy",4000,0,4000,0,0,0);
	Sd2sEnergy.Set("Sd2sEnergy",4000,0,4000,0,0,0);
	SurEnergy.Set("SurEnergy",4000,0,4000,0,0,0);
	SusEnergy.Set("SusEnergy",4000,0,4000,0,0,0);
	YdEnergy.Set("YdEnergy",4000,0,4000,0,0,0);
	YuEnergy.Set("YuEnergy",4000,0,4000,0,0,0);
}

void SetupHistos::Read(char* line)
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

	Int_t nbinx, nbiny;
	Double_t xmin, xmax, ymin, ymax;
	sscanf(val,"%d,%lf,%lf,%d,%lf,%lf",&nbinx,&xmin,&xmax,&nbiny,&ymin,&ymax);
	
	//	parameter of type string:
	if (strcmp(line,"CsI1Energy")==0){
		CsI1Energy.Set("CsI1Energy",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"CsI2Energy")==0){
		CsI2Energy.Set("CsI2Energy",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"Sd1rEnergy")==0){
		Sd1rEnergy.Set("Sd1rEnergy",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"Sd1sEnergy")==0){
		Sd1sEnergy.Set("Sd1sEnergy",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"Sd2rEnergy")==0){
		Sd2rEnergy.Set("Sd2rEnergy",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"Sd2sEnergy")==0){
		Sd2sEnergy.Set("Sd2sEnergy",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"SurEnergy")==0){
		SurEnergy.Set("SurEnergy",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"SusEnergy")==0){
		SusEnergy.Set("SusEnergy",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YdEnergy")==0){
		YdEnergy.Set("YdEnergy",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YuEnergy")==0){
		YuEnergy.Set("YuEnergy",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"SSBEnergy")==0){
		SSBEnergy.Set("SSBEnergy",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"SdETot")==0){
		SdETot.Set("SdETot",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"SdETotMonitor")==0){
		SdETotMonitor.Set("SdETotMonitor",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YdCsI1ETot")==0){
		YdCsI1ETot.Set("YdCsI1ETot",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	if (strcmp(line,"YdCsI2ETot")==0){
		YdCsI2ETot.Set("YdCsI2ETot",nbinx,xmin,xmax,nbiny,ymin,ymax);
	}
	
}

void SetupHistos::Load(std::string filename){	

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
			default  :  Read(ptr);
		}
	}
	fclose(file);
	file=NULL;
}

void SetupHistos::Print(){

}

void SetupHistos::Clear(){
}
