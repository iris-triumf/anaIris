// hist_t.h

#ifndef hist_t_H
#define hist_t_H
#include <TObject.h>
#include <TClass.h>
#include <TH1.h>
#include <TH2.h>
#include <string>
#include "HistPar.h"
#include "Globals.h"

class hist_t : public TObject {
	public:
		hist_t(); 
		virtual ~hist_t() {} //! 

		// ADC spectra
		TH1F *hADC[512]; 
		HistPar pADC[512]; 
		
		// Detector spectra
		TH1F *hIC[16];  
		TH1F *hICSum;  
		TH1F *hCsI1[16];  
		TH1F *hCsI2[16];  
		TH1F *hSd2r[32];  
		TH1F *hSd2s[32];      
		TH1F *hSd1r[32];  
		TH1F *hSd1s[32];      
		TH1F *hSur[32]; //  Upstream S3 rings 
		TH1F *hSus[32]; // Upstream S3 sectors   
		TH1F *hYd[128]; 
		TH1F *hYu[128]; 
		HistPar pIC[16];  
		HistPar pICSum;  
		HistPar pCsI1[16];  
		HistPar pCsI2[16];  
		HistPar pSd2r[32];  
		HistPar pSd2s[32];      
		HistPar pSd1r[32];  
		HistPar pSd1s[32];      
		HistPar pSur[32]; //  Upstream S3 rings 
		HistPar pSus[32]; // Upstream S3 sectors   
		HistPar pYd[128]; 
		HistPar pYu[128]; 
		
		// Summary spectra
		TH2F *hCsI1Summary; 
		TH2F *hCsI2Summary; 
		TH2F *hSd1rSummary;
		TH2F *hSd1sSummary;
		TH2F *hSd2rSummary;
		TH2F *hSd2sSummary;
		TH2F *hSurSummary;
		TH2F *hSusSummary;
		TH2F *hYdSummary;
		TH2F *hYuSummary;   
		HistPar pCsI1Summary; 
		HistPar pCsI2Summary;
		HistPar pSd1rSummary;
		HistPar pSd1sSummary;
		HistPar pSd2rSummary;
		HistPar pSd2sSummary;
		HistPar pSurSummary;
		HistPar pSusSummary;
		HistPar pYdSummary;
		HistPar pYuSummary;                                           
		
		// Hit pattern histos
		TH1F *hSd2rHits; // Downstream S3 2 rings   
		TH1F *hSd2sHits; // Downstream S3 2 sectors 
		TH1F *hSd1rHits; // Downstream S3 1 rings   
		TH1F *hSd1sHits; // Downstream S3 1 sectors 
		TH1F *hSurHits; // Upstream S3 rings   
		TH1F *hSusHits; // Upstream S3 sectors   
		TH1F *hSd2rElHits; // Downstream S3 2 rings with elastic gate
		TH1F *hSd2sElHits; // Downstream S3 2 sectors with elastic gate
		TH1F *hSd1rElHits; // Downstream S3 1 rings with elastic gate
		TH1F *hSd1sElHits; // Downstream S3 1 sectors with elastic gate
		TH1F *hCsI1Hits;  // CsI1
		TH1F *hCsI2Hits;  // CsI2
		TH1F *hYdHits[8];  // Downstream Y11 1
		TH1F *hYdHitsAll;  // Downstream Y11 rings
		TH1F *hYdHitsProt;  // Downstream Y11 hits with proton gate
		TH1F *hYuHits[8];  // Upstream Y11 1
		TH1F *hYuHitsAll;  //  Upstream Y11 rings
		TH1F *hYuHitsProt;  // Upstream Y11 hits with proton gate
		TH2F *hYdCsI1Corr;
		TH2F *hYdCsI2Corr;
		HistPar pSd2rHits;
		HistPar pSd2sHits;
		HistPar pSd1rHits;
		HistPar pSd1sHits;
		HistPar pSurHits; 
		HistPar pSusHits; 
		HistPar pSd2rElHits;
		HistPar pSd2sElHits;
		HistPar pSd1rElHits;
		HistPar pSd1sElHits;
		HistPar pCsI1Hits;
		HistPar pCsI2Hits;
		HistPar pYdHits[8];
		HistPar pYdHitsAll;
		HistPar pYdHitsProt;
		HistPar pYuHits[8];
		HistPar pYuHitsAll;
		HistPar pYuHitsProt;
		HistPar pYdCsI1Corr;
		HistPar pYdCsI2Corr;
	
		// Energies
		TH1F *hCsI1Energy;
		TH1F *hCsI2Energy;
		TH1F *hSd1rEnergy;
		TH1F *hSd1sEnergy;
		TH1F *hSd2rEnergy;
		TH1F *hSd2sEnergy;
		TH1F *hSdETot;
		TH1F *hSdETotMonitor;
		TH1F *hSSBEnergy;
		TH1F *hSurEnergy;
		TH1F *hSusEnergy;
		TH1F *hYdEnergy;
		TH1F *hYdCsI1ETot;
		TH1F *hYdCsI2ETot;
		TH1F *hYuEnergy;
		HistPar pCsI1Energy;
		HistPar pCsI2Energy;
		HistPar pSd1rEnergy;
		HistPar pSd1sEnergy;
		HistPar pSd2rEnergy;
		HistPar pSd2sEnergy;
		HistPar pSdETot;
		HistPar pSdETotMonitor;
		HistPar pSSBEnergy;
		HistPar pSurEnergy;
		HistPar pSusEnergy;
		HistPar pYdEnergy;
		HistPar pYdCsI1ETot;
		HistPar pYdCsI2ETot;
		HistPar pYuEnergy;
			
		// PID
		TH2F *hCsI1Range;
		TH2F *hCsI2Range;
		TH2F *hSdPID;
		TH2F *hYdCsI2PID;
		TH2F *hYdCsI1PID;
		TH2F *hYdCsI1PID_adc;
		TH2F *hYdCsI2PID_adc;
		TH2F *hYdCsI1PID_adc2;
		TH2F *hYdCsI2PID_adc2;
		TH2F *hYdRange;
		HistPar pCsI1Range;
		HistPar pCsI2Range;
		HistPar pSdPID;
		HistPar pYdCsI1PID;
		HistPar pYdCsI2PID;
		HistPar pYdCsI1PID_adc;
		HistPar pYdCsI2PID_adc;
		HistPar pYdCsI1PID_adc2;
		HistPar pYdCsI2PID_adc2;
		HistPar pYdRange;
		
		// Angles
		TH1F *hSdTheta;
		TH1F *hSdPhi;
		TH2F *hSdETheta;
		TH2F *hSdPhiTheta;
		TH1F *hSuTheta;
		TH1F *hSuPhi;
		TH2F *hSuETheta;
		TH2F *hSuPhiTheta;
		TH1F *hYdTheta;
		TH2F *hYdETheta;
		TH1F *hYuTheta;
		TH2F *hYuETheta;
		HistPar pSdTheta;
		HistPar pSdPhi;
		HistPar pSdETheta;
		HistPar pSdPhiTheta;
		HistPar pSuTheta;
		HistPar pSuPhi;
		HistPar pSuETheta;
		HistPar pSuPhiTheta;
		HistPar pYdTheta;
		HistPar pYdETheta;
		HistPar pYuTheta;
		HistPar pYuETheta;
		
		// TRIFIC
		TH1F *hTRIFIC_1;
		TH1F *hTRIFIC_2;
		TH1F *hTRIFIC_3;
		TH2F *hTRIFIC_1v2;
		TH2F *hTRIFIC_1v3;
		TH2F *hTRIFIC_2v3;
		TH2F *hTRIFIC_1v23;
		TH2F *hTRIFIC_12v3;
		HistPar pTRIFIC_1;
		HistPar pTRIFIC_2;
		HistPar pTRIFIC_3;
		HistPar pTRIFIC_1v2;
		HistPar pTRIFIC_1v3;
		HistPar pTRIFIC_2v3;
		HistPar pTRIFIC_1v23;
		HistPar pTRIFIC_12v3;
		
		// TDC histograms
		TH1F *hV1190_T[512]; // Raw T
		TH1F *hTime[512]; // Time  
		TH1F *hTimeRF[512]; //  Time wrt RF
		TH1F *hTall; // T
		TH2F *hTall2D; // T
		HistPar pV1190_T[512];
		HistPar pTime[512];
		HistPar pTimeRF[512];
		HistPar pTall;
		HistPar pTall2D;

		// PHYSICS histograms
		TH1F *hQValue1; // QValue using CsI1
		TH1F *hQValue2; // QValue using CsI2
		TH1F *hQValueU; // QValue using upstream
		TH2F *hYdCsIEnergyTime;
		TH2F *hS3EnergyTime;
		TH2F *hYdCsI1Theta;
		TH2F *hYdCsI2Theta;
		TH2F *hYuEnergyTheta;
		TH2F *hYdCsIThetaProt; //kinematics with proton gate
		HistPar pQValue1;
		HistPar pQValue2;
		HistPar pQValueU;
		HistPar pYdCsIEnergyTime;
		HistPar pS3EnergyTime;
		HistPar pYdCsI1Theta;
		HistPar pYdCsI2Theta;
		HistPar pYuEnergyTheta;
		HistPar pYdCsIThetaProt;

		virtual void ReadParams(char* line);
		virtual void LoadParams(std::string filename);
		virtual void Book();
		virtual void Print();
		virtual void Clear();
//		ClassDef(hist_t,1)
};

#endif
// end
