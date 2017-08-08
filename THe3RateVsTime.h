#ifndef THe3RateVsTime_h
#define THe3RateVsTime_h

#include <string>

#include "THistogramArrayBase.h"

/// Class for making a plot of UCN rate vs time as seen by He-3 detector, readout with V785 peak-sensing ADC.
class THe3RateVsTime : public THistogramArrayBase {
 public:
  THe3RateVsTime();
  virtual ~THe3RateVsTime(){};
  
  /// Update the histograms for this canvas.
  void UpdateHistograms(TDataContainer& dataContainer);

  /// Take actions at begin run
  void BeginRun(int transition,int run,int time);

  /// Take actions at end run  
  void EndRun(int transition,int run,int time);

private:

  void CreateHistograms();
    
  // Vector storing the rate vs time.
  std::vector< std::vector< std::pair<int,double> >  > fRateVsTime;
};

#endif


