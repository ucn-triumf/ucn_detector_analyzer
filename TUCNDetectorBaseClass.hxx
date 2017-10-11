#ifndef TUCNDetectorBaseClass_h
#define TUCNDetectorBaseClass_h

#include <string>
#include "TDataContainer.hxx"
#include "TUCNHit.hxx"

#include "TUCNDetectorCharge.h"
#include "TUCNRateVsTime.h"


/// Generic base class that handles producing a consistent 
/// set of plots and data for Li-6 and He-3 detectors
class TUCNDetectorBaseClass  {
 public:
  TUCNDetectorBaseClass(bool isOffline);
  virtual ~TUCNDetectorBaseClass(){};  


  // Get the UCN hits for this MIDAS event.
  // This method needs to be defined in each derived class.
  virtual void GetHits(TDataContainer& dataContainer) = 0;

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
  TH1D* GetHitsInCycle(){return fHitsInCycle;}
  TH1D* GetHitsPerCycle(){return fHitsPerCycle;}
  
protected:

  bool fIsLi6; // Is for Li-6 detector
  bool fIsOffline;
  
  // List of hits
  TUCNHitCollection fHits;

  // Start time of the current sequence;
  double fCycleStartTime;

private:

  // UCN hit charge or pulse height spectrum histograms
  TUCNDetectorCharge *fDetectorCharge;
  TUCNRateVsTime *fRateVsTime;

  TH1D *fHitsInCycle;
  TH1D *fHitsPerCycle;
  
  
};

#endif


