// SetupHistos.h

#ifndef SetupHistos_H
#define SetupHistos_H
#include <TObject.h>
#include <TClass.h>
#include <string>
#include "HistPar.h"

class SetupHistos : public TObject {
	public:
		SetupHistos(); 
		virtual ~SetupHistos() {} //! 
		
		HistPar CsI1Energy;
		HistPar CsI2Energy;
		HistPar Sd1rEnergy;
		HistPar Sd1sEnergy;
		HistPar Sd2rEnergy;
		HistPar Sd2sEnergy;
		HistPar SurEnergy;
		HistPar SusEnergy;
		HistPar YdEnergy;
		HistPar YuEnergy;
		HistPar SSBEnergy;
		HistPar SdETot;
		HistPar SdETotMonitor;
		HistPar YdCsI1ETot;
		HistPar YdCsI2ETot;
		HistPar YdCsI1PID;
		HistPar YdCsI2PID;
		HistPar SdPID;
		HistPar YdCsI1PID_adc;
		HistPar YdCsI2PID_adc;
		HistPar YdCsI1PID_adc2;
		HistPar YdCsI2PID_adc2;
		HistPar YdRange;
		HistPar CsI1Range;
		HistPar CsI2Range;
		HistPar YdTheta;
		HistPar YdETheta;
		HistPar SdTheta;
		HistPar SdPhi;
		HistPar SdPhiTheta;
		HistPar SdETheta;
		HistPar YuTheta;
		HistPar YuETheta;
		HistPar SuTheta;
		HistPar SuETheta;
		HistPar SuPhi;
		HistPar SuPhiTheta;

		virtual void Read(char* line);
		virtual void Load(std::string filename);
		virtual void Print();
		virtual void Clear();
//		ClassDef(SetupHistos,1)
};

#endif
// end
