#ifndef TAnaManager_h
#define TAnaManager_h

#include "mvodb.h"

// Use this list here to decide which type of equipment to use.

#define USE_V1720

#include "THe3CountsInSequence.h"
#include "TV1720Waveform.h"

#include "THe3Detector.hxx"
#include "TLi6Detector.hxx"
#include "TUCNTreeMaker.h"
#include "TV792Histogram.h"

/// This is the main analysis manager program for the UCN detectors.
class TAnaManager  {
    public:
        TAnaManager(bool isOffline, MVOdb* odb = 0, bool saveTree = false);
        virtual ~TAnaManager(){};

        /// Processes the midas event, fills histograms, etc.
        int ProcessMidasEvent(TDataContainer& dataContainer);

        void BeginRun(int transition, int run, int time) {
            if(fLi6Detector) fLi6Detector->BeginRun(transition,run,time);
        }

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
        TUCNDetectorBaseClass* GetLi6DetectorAnalyzer(){return fLi6Detector;}

        TV792Histograms* GetV785Histo(){return fV785Charge;}

    private:

        TV1720Waveform *fV1720Waveform;
        //THe3RateVsTime* fHe3RateVsTime;
        THe3CountsInSequence* fHe3CountsInSequence;

        THe3Detector  *fHe3Detector;
        TLi6Detector  *fLi6Detector;

        // equipment list
        SourceEpics *fSourceEpics; // vertical source
        BeamlineEpics *fBeamlineEpics;
        UCN2Epics *fUCN2Epics;
        UCN2EpicsTemperature *fUCN2EpicsTemperature;
        UCN2EpicsPressures *fUCN2EpicsPressures;
        UCN2EpicsOthers *fUCN2EpicsOthers;
        UCN2EpicsPhase2B *fUCN2EpicsPhase2B;
        scPico *fscPico;

        // Histogram of all V785 Charges
        TV792Histograms *fV785Charge;

};



#endif


