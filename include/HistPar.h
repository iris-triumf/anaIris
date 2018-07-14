// HistPar.h

#ifndef HistPar_H
#define HistPar_H
#include <TObject.h>
#include <TClass.h>
#include <string>
#include <TH1.h>
#include <TH2.h>

class HistPar : public TObject {
	public:
		HistPar(); 
		virtual ~HistPar() {} //! 
		
		std::string title;
		Int_t nbinx;
		Double_t xmin;
		Double_t xmax;
		Int_t nbiny;
		Double_t ymin;
		Double_t ymax;

		virtual void Set1D(std::string, Int_t, Double_t, Double_t);
		virtual void Set2D(std::string, Int_t, Double_t, Double_t, Int_t, Double_t, Double_t);
		virtual TH1F* SetupTH1F();
		virtual TH2F* SetupTH2F();
		virtual void Print();
		virtual void Clear();
//		ClassDef(HistPar,1)
};

#endif
// end
