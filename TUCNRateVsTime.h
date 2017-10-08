#ifndef TUCNRateVsTime_h
#define TUCNRateVsTime_h

#include <string>

#include "THistogramArrayBase.h"
#include "TUCNHit.hxx"

/// Class for making a plot of UCN rate vs time as seen by He-3 detector, readout with V785 peak-sensing ADC.
class TUCNRateVsTime : public THistogramArrayBase {
 public:
  TUCNRateVsTime(bool isOffline, bool isLi6);
  virtual ~TUCNRateVsTime(){};
  
  /// Don't use this update method
  void UpdateHistograms(TDataContainer& dataContainer){};

  void UpdateHistograms(TUCNHitCollection& hits);
                        
  /// Take actions at begin run
  void BeginRun(int transition,int run,int time);

  /// Take actions at end run  
  void EndRun(int transition,int run,int time);

private:

  bool fIsLi6;
  bool fIsOffline;
  
  void CreateHistograms();
    
  // Vector storing the rate vs time.
  std::vector< std::vector< std::pair<int,double> >  > fRateVsTime;
};

#endif


