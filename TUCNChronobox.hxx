#ifndef TUCNChronobox_h
#define TUCNChronobox_h

// This file contains class for analyzing the chronobox


#include <string>
#include "TDataContainer.hxx"

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


 private:

  int fNumberRollovers;
  int fRolloverWord;
  double fTimestamps[MaxChan][2];  // Last timestamp for each channel and leading/falling: 
  int fPPSTimestamp;

};

#endif


