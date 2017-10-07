#ifndef TUCNDetectorBaseClass_h
#define TUCNDetectorBaseClass_h

#include <string>
#include "TDataContainer.hxx"
#include "TUCNHit.hxx"

#include "TUCNDetectorCharge.h"


/// Generic base class that handles producing a consistent 
/// set of plots and data for Li-6 and He-3 detectors
class TUCNDetectorBaseClass  {
 public:
  TUCNDetectorBaseClass();
  virtual ~TUCNDetectorBaseClass(){};  


  // Get the UCN hits for this MIDAS event.
  // This method needs to be defined in each derived class.
  virtual void GetHits(TDataContainer& dataContainer);

  /// Update the histograms for this canvas.
  void ProcessMidasEvent(TDataContainer& dataContainer);

  /// Take actions at begin run
  void BeginRun(int transition,int run,int time);

  /// Take actions at end run  
  void EndRun(int transition,int run,int time);
  
  bool IsLi6(){ return isLi6;};
  bool IsHe3(){ return !isLi6;};
  
  std::string GetDetectorName(){
    if(IsLi6()) return std::string("Li6");
    if(IsHe3()) return std::string("He3");
    return std::string("notset");
  }

  TUCNDetectorCharge* GetChargeHistograms(){return fDetectorCharge;}

private:
  bool isLi6; // Is for Li-6 detector

  // List of hits
  std::vector<TUCNHit> fHits;

  // UCN hit charge or pulse height spectrum histograms
  TUCNDetectorCharge *fDetectorCharge;

};

#endif


