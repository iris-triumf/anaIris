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

		virtual void Set(std::string, Int_t, Double_t, Double_t, Int_t, Double_t, Double_t);
		virtual TH1D* SetupTH1D();
		virtual TH2D* SetupTH2D();
		virtual void Print();
		virtual void Clear();
//		ClassDef(HistPar,1)
};

#endif
// end
