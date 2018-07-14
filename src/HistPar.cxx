#include "HistPar.h"
#include <stdio.h>
#include <stdlib.h>

HistPar::HistPar(){
  	HistPar::Clear();
}

void HistPar::Set1D(std::string t, Int_t nx, Double_t x1, Double_t x2)
{
	title = t;
	nbinx = nx;
	if(nbinx<=1){
		nbinx=1000;
		printf("%s: Number of bins for x-axis undefined.\n",title.data());
	}
	xmin = x1;
	xmax = x2;
	if(xmin>=xmax){
		xmin=0;
		xmax=4000;
		printf("%s: Limits for x-axis undefined.\n",title.data());
	}
	nbiny = 0;
	ymin = 0;
	ymax = 0;
}

void HistPar::Set2D(std::string t, Int_t nx, Double_t x1, Double_t x2, Int_t ny, Double_t y1, Double_t y2 )
{
	title = t;
	nbinx = nx;
	if(nbinx<=1){
		nbinx=1000;
		printf("%s: Number of bins for x-axis undefined.\n",title.data());
	}
	xmin = x1;
	xmax = x2;
	if(xmin>=xmax){
		xmin=0;
		xmax=4000;
		printf("%s: Limits for x-axis undefined.\n",title.data());
	}
	nbiny = ny;
	if(nbiny<=1){
		nbiny=1000;
		printf("%s: Number of bins for y-axis undefined.\n",title.data());
	}
	ymin = y1;
	ymax = y2;
	if(ymin>=ymax){
		ymin=0;
		ymax=4000;
		printf("%s: Limits for y-axis undefined.\n",title.data());
	}
}

TH1F* HistPar::SetupTH1F()
{
	TH1F *h = new TH1F(title.data(),title.data(),nbinx,xmin,xmax);
	return h;
}

TH2F* HistPar::SetupTH2F()
{
	TH2F *h = new TH2F(title.data(),title.data(),nbinx,xmin,xmax,nbiny,ymin,ymax);
	return h;
}

void HistPar::Print(){
	
}

void HistPar::Clear(){

}
