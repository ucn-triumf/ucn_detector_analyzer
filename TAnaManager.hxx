#ifndef TAnaManager_h
#define TAnaManager_h

// Use this list here to decide which type of equipment to use.

#define USE_V792
#define USE_V1720

#include "TV792Histogram.h"
#include "THe3RateVsTime.h"
#include "THe3CountsInSequence.h"
#include "TV1720Waveform.h"

#include "THe3Detector.hxx"

/// This is the main analysis manager program for the UCN detectors.
class TAnaManager  {
public:
  TAnaManager();
  virtual ~TAnaManager(){};

  /// Processes the midas event, fills histograms, etc.
  int ProcessMidasEvent(TDataContainer& dataContainer);
  
  /// Methods for determining if we have a particular set of histograms.
  bool HaveV792Histograms();
  bool HaveHe3RateHistograms();
  bool HaveV1720Histograms();
  /// Methods for getting particular set of histograms.
  TV792Histograms* GetV792Histograms();
  TV1720Waveform* GetV1720Histograms();
  THe3RateVsTime* GetHe3RateHistograms();
  THe3CountsInSequence* GetHe3CountsHistograms();

  TUCNDetectorBaseClass* GetHe3DetectorAnalyzer(){return fHe3Detector;}
private:
  
  TV792Histograms *fV792Histogram;
  TV1720Waveform *fV1720Waveform;
  THe3RateVsTime* fHe3RateVsTime;
  THe3CountsInSequence* fHe3CountsInSequence;
  
  THe3Detector  *fHe3Detector;

};



#endif


