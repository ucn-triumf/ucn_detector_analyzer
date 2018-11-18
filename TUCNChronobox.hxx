#ifndef TUCNChronobox_h
#define TUCNChronobox_h

// This file contains class for analyzing the chronobox


#include <string>
#include "TDataContainer.hxx"

class TUCNChronobox {

 public:

  const static int MaxChan = 16;
  const static int PPSChan = 3;
  
  TUCNChronobox();

  /// Processes the midas event, fills histograms, etc.
  int ProcessMidasEvent(TDataContainer& dataContainer);
  
 private:

  int fNumberRollovers;
  int fRolloverWord;
  double fTimestamps[MaxChan];
  int fPPSTimestamp;

};

#endif


