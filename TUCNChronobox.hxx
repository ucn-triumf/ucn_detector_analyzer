#ifndef TUCNChronobox_h
#define TUCNChronobox_h

// This file contains class for analyzing the chronobox

#include "THistogramArrayBase.h"
#include "TUCNHit.hxx"
#include <string>
#include "TDataContainer.hxx"
#include "TH1D.h"

class TUCNRisingEdgeDiff : public THistogramArrayBase {
 public:
  TUCNRisingEdgeDiff();
  virtual ~TUCNRisingEdgeDiff(){};
  
  void UpdateHistograms(TDataContainer& data);

  /// Take actions at begin run
  void BeginRun(int transition,int run,int time);

  /// Take actions at end run  
  void EndRun(int transition,int run,int time);

 private:

  void CreateHistograms();
    
};


class TUCNChronobox {

 public:

  const static int MaxChan = 64;
  const static int PPSChan = 3;
  
  TUCNChronobox();

  /// Processes the midas event, fills histograms, etc.
  int ProcessMidasEvent(TDataContainer& dataContainer);
  
  /// Get last timestamp for channel, leading/falling... leadingfalling == 1 means leading
  double GetTimestamp(int ch, int leading){
    if(ch <0 || ch >= MaxChan || leading < 0 || leading > 1) return 0;    
    return fTimestamps[ch][leading];
  };


  TH1D *fLeadingDiff;
  TH1D *fFallingDiff;
  TH1D *fChronoWidth;

  TUCNRisingEdgeDiff *fRisingEdgeDiff;

 private:

  int fNumberRollovers;
  int fRolloverWord;
  double fTimestamps[MaxChan][2];  // Last timestamp for each channel and leading/falling: 
  int fPPSTimestamp;

};

#endif


