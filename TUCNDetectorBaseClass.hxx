#ifndef TUCNDetectorBaseClass_h
#define TUCNDetectorBaseClass_h

#include <string>
#include "TDataContainer.hxx"
#include "TUCNHit.hxx"

#include "TUCNDetectorCharge.h"
#include "TUCNRateVsTime.h"
#include "TSimpleHistogramCanvas.hxx"
#include "TFancyHistogramCanvas.hxx"

/// Generic base class that handles producing a consistent 
/// set of plots and data for Li-6 and He-3 detectors
class TUCNDetectorBaseClass  {
 public:
  TUCNDetectorBaseClass(bool isOffline, bool isLi6);
  virtual ~TUCNDetectorBaseClass(){};  


  // Get the UCN hits for this MIDAS event.
  // This method needs to be defined in each derived class.
  virtual void GetHits(TDataContainer& dataContainer) = 0;

  // We have different methods for checking for the start
  // of a sequence; either the crude method based on
  // timestamps from the VME sequencer module (with ~2second jitter)
  // or a more precise time based on hits in the particular digitizer.
  bool CheckForSequenceStartCrude(TDataContainer& dataContainer);
  virtual bool CheckForSequenceStartPrecise(TDataContainer& dataContainer){return false;};

  /// Method to decide which sequence time to use.
  virtual bool UsePreciseSequenceTime(){return false;};
  
  // Fill plots specific to detector type.
  // Can be defined in derived class.
  virtual void FillSpecificPlots(){};
  
  /// Update the histograms for this canvas.
  void ProcessMidasEvent(TDataContainer& dataContainer);

  /// Take actions at begin run
  void BeginRun(int transition,int run,int time);

  /// Take actions at end run  
  void EndRun(int transition,int run,int time);
  
  bool IsLi6(){ return fIsLi6;};
  bool IsHe3(){ return !fIsLi6;};
  
  std::string GetDetectorName(){
    if(IsLi6()) return std::string("Li6");
    if(IsHe3()) return std::string("He3");
    return std::string("notset");
  }

  TUCNDetectorCharge* GetChargeHistograms(){return fDetectorCharge;}
  TUCNRateVsTime* GetRateVsTime(){return fRateVsTime;}
  TFancyHistogramCanvas* GetRateVsTimeCanvas(){    
    TFancyHistogramCanvas *canvas = new TFancyHistogramCanvas(fRateVsTime,"Rate Vs Time");
    canvas->SetChannelName("Timescale");    
    return canvas;
  }

  TH1D* GetHitsInCycle(){return fHitsInCycle;}

  TSimpleHistogramCanvas* GetHitsInCycleCanvas(){    
    return  new TSimpleHistogramCanvas(fHitsInCycle,"Hits In Current Cycle");
  }

  TSimpleHistogramCanvas* GetHitsInCycleCumulCanvas(){    
    return  new TSimpleHistogramCanvas(fHitsInCycleCumul,"Cumulative Hits In Cycle");
  }

  TSimpleHistogramCanvas* GetHitsPerCycleCanvas(){    
    return  new TSimpleHistogramCanvas(fHitsPerCycle,"Hits Per Cycle");
  }
  //  TH1D* GetHitsPerCycle(){return fHitsPerCycle;}


  
protected:

  bool fIsLi6; // Is for Li-6 detector
  bool fIsOffline;
  
  // List of UCN hits
  TUCNHitCollection fHits;

  // List of non-UCN hits.
  TUCNHitCollection fNonHits;

  // Start time of the current sequence;
  double fCycleStartTime;
  double fValueOpenTime;
  double fLastCycleStartTime;

  
  
private:

  // UCN hit charge or pulse height spectrum histograms
  TUCNDetectorCharge *fDetectorCharge;
  TUCNRateVsTime *fRateVsTime;

  TH1D *fHitsInCycle;
  TH1D *fHitsInCycleCumul;
  TGraph *fHitsPerCycle;

  // UCN hits in this cycle
  int fTotalHitsCycle;

  // vector to store the cycle hits in
  std::vector<std::pair<double, double> > fHitsPerCycleVector;
};

#endif


