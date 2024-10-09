#ifndef TAnaManager_h
#define TAnaManager_h

#include "mvodb.h"

// Use this list here to decide which type of equipment to use.

#define USE_V1720

#include "THe3CountsInSequence.h"
#include "TV1720Waveform.h"

#include "THe3Detector.hxx"
#include "TLi6Detector.hxx"
#include "TUCNEpicsTree.hxx"
#include "TLNDDetectorTree.hxx"
#include "TSequencerTree.hxx"
#include "TCycleParamTree.hxx"
#include "TSCMTree.hxx"
#include "TV792Histogram.h"
#include "TV1725WaveformDisplay.h"
#include "TUCNChronobox.hxx"
#include "TGraphErrors.h"

/// This is the main analysis manager program for the UCN detectors.
class TAnaManager  {
    public:
        TAnaManager(bool isOffline, MVOdb* odb = 0, bool saveTree = false);
        virtual ~TAnaManager(){};

        /// Processes the midas event, fills histograms, etc.
        int ProcessMidasEvent(TDataContainer& dataContainer);
        void BeginRun(int transition,int run,int time);

        // Method to force analyzer to use simpler PC timestamps
        void UsePCTime(){
            if(fHe3Detector) fHe3Detector->UsePCTime();
            if(fLi6Detector) fLi6Detector->UsePCTime();
        }

        // Methods for determining if we have a particular set of histograms.
        //bool HaveHe3RateHistograms();
        bool HaveV1720Histograms();

        /// Methods for getting particular set of histograms.
        TV1720Waveform* GetV1720Histograms();
        //THe3RateVsTime* GetHe3RateHistograms();
        THe3CountsInSequence* GetHe3CountsHistograms();

        TUCNDetectorBaseClass* GetHe3DetectorAnalyzer(){return fHe3Detector;}
        TUCNDetectorBaseClass* GetHe3Detector2Analyzer(){return fHe3Detector2;}
        TUCNDetectorBaseClass* GetLi6DetectorAnalyzer(){return fLi6Detector;}

        TV792Histograms* GetV785Histo(){return fV785Charge;}
        TV1725WaveformDisplay* GetV1725Waveform(){return fV1750WaveformDisplay;}
        TV1725PSDQL* GetV1725PSDvsQL(){return fV1725PSDQL;}
        TV1725_PH* GetV1725_PH(){ return fV1725_PH;}
        TV1725_QL* GetV1725_QL(){ return fV1725_QL;}
        TUCNChronobox* GetChronobox(){ return fUCNChronobox;}
        TUCNChronobox *fUCNChronobox;

        std::map<const char*, TGraphErrors*> graphs;

    private:

        TV1720Waveform *fV1720Waveform;
        //THe3RateVsTime* fHe3RateVsTime;
        THe3CountsInSequence* fHe3CountsInSequence;

        THe3Detector  *fHe3Detector;
        THe3Detector  *fHe3Detector2;
        TLi6Detector  *fLi6Detector;

        // EPICS variables
        std::vector<TUCNEpicsTree*> fEPICSTrees;
        TLNDDetectorTree *fLNDDetectorTree;
        TSCMTree *fSCMTree;
        TSequencerTree *fSequencerTree;
        TCycleParamTree *fCycleParamTree;

        // Histogram of all V785 Charges
        TV792Histograms *fV785Charge;

        TV1725WaveformDisplay *fV1750WaveformDisplay;
        TV1725PSDQL* fV1725PSDQL;   // pulse shape discrimination charge long
        TV1725_PH* fV1725_PH;       // pulse height
        TV1725_QL* fV1725_QL;       // charge long

        // define names and titles of TGraphErrors to draw in anaDisplay
        std::map<const char*, const char*> GRAPH_NAME_TITLE = {
            {"TransmissionDuringCounting",          "Transmission normalized during counting;Cycle no.;Li6-He3 ratio"},
            {"TransmissionDuringIrradiation",       "Transmission normalized during irradiation;Cycle no.;Li6-He3 ratio"},
            {"TransmissionWithPreStorage",          "Transmission normalized during prestorage;Cycle no.;Background-corrected Li6-He3 ratio"},
            {"He3DuringIrradiation",                "He3 monitor counts during irradiation;Cycle no.;He3 counts"},
            {"He3DuringStorage",                    "He3 monitor counts during storage;Cycle no.;He3 counts"},
            {"He3AfterIrradiation",                 "He3 monitor counts after irradiation;Cycle no.;He3 counts"},
            {"Li6StorageBackground",                "Li6 background during storage period;Cycle no.;Li6 background rate (s^{-1})"},
            {"He3StorageBackground",                "He3 background during storage period;Cycle no.;He3 background rate (s^{-1})"},
            {"Li6Storage",                          "Unnormalized storage with Li6 detector;Storage time (s);Background-corrected Li6 counts"},
            {"He3Storage",                          "Unnormalized storage with He3 detector;Storage time (s);Background-corrected He3 counts"},
            {"StorageWithMonitorDuringIrradiation", "Storage with normalization during irradiation;Storage time (s);Background-corrected Li6-He3 ratio"},
            {"StorageWithMonitorAfterIrradiation",  "Storage with normalization after irradiation;Storage time(s);Background-corrected Li6-He3 ratio"},
        };
};

#endif


