#ifndef TLi6Detector_h
#define TLi6Detector_h

#include "TUCNDetectorBaseClass.hxx"

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

  TFancyHistogramCanvas* GetV1720BaselineCanvas(){    
    return new TFancyHistogramCanvas(fBaselines,"Baselines");
    //canvas->SetChannelName("Timescale");    
    //return canvas;
  }

  // Fill V1720 specific plots
  void FillSpecificPlots();
  
private:
  TV1720Baselines *fBaselines;
  
};

#endif
