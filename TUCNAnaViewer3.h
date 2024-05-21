#ifndef _TUCNAnaViewer3_h_
#define _TUCNAnaViewer3_h_

#include "TH1D.h"
#include "TV1720Histograms.h"
#include "TV1720WaveformDisplay.h"
#include "TDataContainer.hxx"
#include "PulseShapeStruct.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TLi6GainCalib.h"

/// This is a class for managing the analysis of UCN data.
/// It creates and fills a TTree of DPP values and a variable
/// length array of the ADC values for individual pulses.
class TUCNAnaViewer3 {

    public:

        TUCNAnaViewer3();
        ~TUCNAnaViewer3();

        /// Processes the midas event, fills histograms, etc.
        int ProcessMidasEvent(TDataContainer& dataContainer,char CutChoice, float PSDMax, float PSDMin);
        int FindAndFitPulses(TDataContainer& dataContainer, char CutChoice, float PSDMax, float PSDMin);

        /// Blah, make histograms public;
        TV1720WaveformDisplay* fV1720WaveformDisplay;
        TV1720QLQL * fV1720QLQL;
        TV1720QSQS * fV1720QSQS;

        TV1720QSQLHistograms*  fV1720QSQLHistograms;
        TV1720PSDQLHistograms* fV1720PSDQLHistograms;

    private:
        TLi6GainCalib qcali;

        double fFirstPulseFittedTime;

        // branches
        Float_t tPSD;
        Short_t tChargeL;
        Short_t tChargeS;
        int16_t tLength;

        // runtime variables
        int index;
        DPP_Bank_Out_t *b;
        uint16_t *wf;
        int nEvents;
        int iboard, ichan, isubev;

        uint32_t numEvents,numSamples;
        int      verbose;
        DPPBankHandler fDPP[NDPPBOARDS];

        char htitle[200];
        int board, ch, ev, preev;
};

#endif
