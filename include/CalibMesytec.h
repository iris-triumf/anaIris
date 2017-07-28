// CalibMesytec.h

#ifndef CalibMesytec_H
#define CalibMesytec_H
#include <TObject.h>
#include <TClass.h>
#include <string>

//Extern
//extern int gMesytecnitems;
class CalibMesytec : public TObject {
	public:
		CalibMesytec(); 
		virtual ~CalibMesytec() {} //! 

		std::string installPath;
		std::string fileGeometry;
		std::string fileELoss;
		std::string fileIC;
		std::string fileCsI1;
		std::string fileCsI2;
		std::string fileSd1r;
		std::string fileSd1s;
		std::string fileSd2r;
		std::string fileSd2s;
		std::string fileSur;
		std::string fileSus;
		std::string fileYd;
		std::string fileYu;

		std::string fGate1;
		std::string fGate2;
		std::string fGate3;
		std::string fGate4;
		std::string nGate1;
		std::string nGate2;
		std::string nGate3;
		std::string nGate4;

		Double_t EBAC;
		Double_t mA;
		Double_t ma;
		Double_t mB1;
		Double_t mB2;
		Double_t mB3;
		Double_t mb1;
		Double_t mb2;
		Double_t mb3;
		Double_t eldl1;
		Double_t eldl2;
		Double_t eldl3;

		Bool_t boolGeometry;
		Bool_t boolELoss;
		Bool_t boolIC;
		Bool_t boolCsI1;
		Bool_t boolCsI2;
		Bool_t boolSd1r;
		Bool_t boolSd1s;
		Bool_t boolSd2r;
		Bool_t boolSd2s;
		Bool_t boolSur;
		Bool_t boolSus;
		Bool_t boolYd;
		Bool_t boolYu;
		Bool_t boolASCII;
		Bool_t boolFGate1;
		Bool_t boolFGate2;
		Bool_t boolFGate3;
		Bool_t boolFGate4;

		virtual void ReadFilenames(char* line);
		virtual void Load(std::string filename);
		virtual void Print(Int_t);
		virtual void Clear();
//		ClassDef(CalibMesytec,1)
};

#endif
// end
