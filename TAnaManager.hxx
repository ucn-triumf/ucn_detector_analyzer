#ifndef TAnaManager_h
#define TAnaManager_h

// Use this list here to decide which type of equipment to use.

#define USE_V1720

#include "THe3CountsInSequence.h"
#include "TV1720Waveform.h"

#include "THe3Detector.hxx"
#include "TLi6Detector.hxx"
#include "TUCNTreeMaker.h"
#include "TV792Histogram.h"
#include "TV1725WaveformDisplay.h"
#include "TUCNChronobox.hxx"
#include "TGraphErrors.h"

/// This is the main analysis manager program for the UCN detectors.
class TAnaManager  {
public:
  TAnaManager(bool isOffline, bool saveTree = false);
  virtual ~TAnaManager(){};

  /// Processes the midas event, fills histograms, etc.
  int ProcessMidasEvent(TDataContainer& dataContainer);
  
  void BeginRun(int transition,int run,int time) {
    if(fLi6Detector) fLi6Detector->BeginRun(transition,run,time);
  }

  // Method to force analyzer to use simpler PC timestamps
  void UsePCTime(){
    if(fHe3Detector) fHe3Detector->UsePCTime();
    if(fLi6Detector) fLi6Detector->UsePCTime();    
  }
  /// Methods for determining if we have a particular set of histograms.
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
  TUCNChronobox* GetChronobox(){ return fUCNChronobox;}


  TUCNChronobox *fUCNChronobox;

  TGraphErrors* GetTransmissionDuringCountingGraph() const { return fTransmissionDuringCountingGraph; }
  TGraphErrors* GetTransmissionDuringIrradiationGraph() const { return fTransmissionDuringIrradiationGraph; }
  TGraphErrors* GetTransmissionWithPreStorageGraph() const { return fTransmissionWithPreStorage; }
  TGraphErrors* GetHe3DuringIrradiationGraph() const { return fHe3DuringIrradiationGraph; }
  TGraphErrors* GetHe3DuringStorageGraph() const { return fHe3DuringStorageGraph; }
  TGraphErrors* GetHe3AfterIrradiationGraph() const { return fHe3AfterIrradiationGraph; }
  TGraphErrors* GetLi6StorageBackgroundGraph() const { return fLi6StorageBackgroundGraph; }
  TGraphErrors* GetHe3StorageBackgroundGraph() const { return fHe3StorageBackgroundGraph; }
  TGraphErrors* GetLi6StorageGraph() const { return fLi6StorageGraph; }
  TGraphErrors* GetHe3StorageGraph() const { return fHe3StorageGraph; }
  TGraphErrors* GetStorageWithMonitorDuringIrradiation() const { return fStorageWithMonitorDuringIrradiation; }
  TGraphErrors* GetStorageWithMonitorAfterIrradiation() const { return fStorageWithMonitorAfterIrradiation; }
private:
  
  TV1720Waveform *fV1720Waveform;
  //THe3RateVsTime* fHe3RateVsTime;
  THe3CountsInSequence* fHe3CountsInSequence;
  
  THe3Detector  *fHe3Detector;
  THe3Detector  *fHe3Detector2;
  TLi6Detector  *fLi6Detector;

  TLNDDetectorTree *fLNDDetectorTree;
  TSCMTree *fSCMTree;
  TSequencerTree *fSequencerTree;
  TUCNSourceEpicsTree *fSourceEpicsTree;
  TUCNBeamlineEpicsTree *fBeamlineEpicsTree;

  // Histogram of all V785 Charges
  TV792Histograms *fV785Charge;

  TV1725WaveformDisplay *fV1750WaveformDisplay;

  TGraphErrors *fTransmissionDuringCountingGraph; // transmission with normalization during counting
  TGraphErrors *fTransmissionDuringIrradiationGraph; // transmission with normalization during irradiation
  TGraphErrors *fTransmissionWithPreStorage; // transmission with a pre-storage period
  TGraphErrors *fHe3DuringIrradiationGraph; // He3 monitor counts during irradiation period
  TGraphErrors *fHe3DuringStorageGraph; // He3 monitor counts during storage period
  TGraphErrors *fHe3AfterIrradiationGraph; // He3 monitor counts after irradiation
  TGraphErrors *fLi6StorageBackgroundGraph; // Li6 background during storage periods
  TGraphErrors *fHe3StorageBackgroundGraph; // He3 background during storage periods
  TGraphErrors *fLi6StorageGraph; // background-corrected Li6 counts vs. storage time
  TGraphErrors *fHe3StorageGraph; // background-corrected He3 counts vs. storage time
  TGraphErrors *fStorageWithMonitorDuringIrradiation; // background-corrected Li6 counts normalized to He3 counts during irradiation vs. storage time
  TGraphErrors *fStorageWithMonitorAfterIrradiation; // background-corrected Li6 counts normalized to He3 counts after irradiation vs. storage time
};



#endif


