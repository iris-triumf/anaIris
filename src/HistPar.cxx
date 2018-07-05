#include "HistPar.h"
#include <stdio.h>
#include <stdlib.h>

HistPar::HistPar(){
  	HistPar::Clear();
}

void HistPar::Set(std::string t, Int_t nx, Double_t x1, Double_t x2, Int_t ny, Double_t y1, Double_t y2 )
{
	title = t;
	nbinx = nx;
	xmin = x1;
	xmax = x2;
	nbiny = ny;
	ymin = y1;
	ymax = y2;
}

TH1D* HistPar::SetupTH1D()
{
	TH1D *h = new TH1D(title.data(),title.data(),nbinx,xmin,xmax);
	return h;
}

TH2D* HistPar::SetupTH2D()
{
	TH2D *h = new TH2D(title.data(),title.data(),nbinx,xmin,xmax,nbiny,ymin,ymax);
	return h;
}

void HistPar::Print(){
	
}

void HistPar::Clear(){

}
