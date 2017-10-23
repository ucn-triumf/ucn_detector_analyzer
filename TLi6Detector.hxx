#ifndef TLi6Detector_h
#define TLi6Detector_h

#include "TUCNDetectorBaseClass.hxx"

#include "PulseShapeStruct.h"
#include "TMulticanvas.h"

/// Class for making histograms of V1720 baseline
class TV1720Baselines : public THistogramArrayBase {
 public:
  TV1720Baselines();
  virtual ~TV1720Baselines(){};
  
  /// We will fill this histogram with separate method
  void UpdateHistograms(TDataContainer& dataContainer){};

  /// Fill the charge spectrum with the set of ucn hits
  void UpdateHistograms(TUCNHitCollection & hits);

  /// Take actions at begin run
  void BeginRun(int transition,int run,int time);

  /// Take actions at end run  
  void EndRun(int transition,int run,int time);

 private:

  void CreateHistograms();
    
};


// A derived class from TUCNDetectorBaseClass...
// This class handles getting the hardware level 
// information and converting it into the 
// standard TUCNHit format...
class TLi6Detector : public TUCNDetectorBaseClass {

public:
  TLi6Detector(bool isOffline);

  void GetHits(TDataContainer& dataContainer);

  // Get a more precise sequence start time from v1720 bank
  virtual bool CheckForSequenceStartPrecise(TDataContainer& dataContainer);

  virtual bool UsePreciseSequenceTime(){return true;};

  
  TFancyHistogramCanvas* GetV1720BaselineCanvas(){    
    return new TFancyHistogramCanvas(fBaselines,"Baselines");
    //canvas->SetChannelName("Timescale");    
    //return canvas;
  }

  TMulticanvas* GetV1720SequenceCanvas(){    

    TMulticanvas *canvas = new TMulticanvas("Sequence Statistics");
    canvas->AddHistoSingle(fSequenceLength,0);
    canvas->AddHistoSingle(fDelayTime,1);
    canvas->AddHistoSingle(fValveOpenTime,2);    
    return canvas;

  }

  // Fill V1720 specific plots
  void FillSpecificPlots();
  
private:
  TV1720Baselines *fBaselines;

  // Decoder for the V1720 bank
  DPPBankHandler fDPP[NDPPBOARDS];

  // stuff for calculating precise time from V1720 timestamp
  long int lastClockTime;
  double initialUnixTime;
  long int initialClockTime;
  long int numberRollOvers;
  

  // histograms for keeping track of statistics for sequencing.
  TH1F *fSequenceLength;
  TH1F *fDelayTime;
  TH1F *fValveOpenTime;
  double fEndOfIrradiationTime;
  double fUCNValveOpenTime;
  double fUCNValveCloseTime;



};

#endif
