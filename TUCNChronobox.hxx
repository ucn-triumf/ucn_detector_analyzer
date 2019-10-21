#ifndef TUCNChronobox_h
#define TUCNChronobox_h

// This file contains class for analyzing the chronobox

#include "THistogramArrayBase.h"
#include "TUCNHit.hxx"
#include <string>
#include "TDataContainer.hxx"
#include "TH1D.h"
#include "TMulticanvas.h"

class TUCNChronoDiffArray : public THistogramArrayBase {
 public:
  TUCNChronoDiffArray(std::string name);
  virtual ~TUCNChronoDiffArray(){};
  
  void UpdateHistograms(TDataContainer& data);

  /// Take actions at begin run
  void BeginRun(int transition,int run,int time);

  /// Take actions at end run  
  void EndRun(int transition,int run,int time);

 private:

  std::string fName;

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

  TUCNChronoDiffArray *fRisingEdgeDiff;
  TUCNChronoDiffArray *fFallingEdgeDiff;
  TUCNChronoDiffArray *fPulseWidths;

  void MakeSingleHistograms();

  /// Take actions at begin run
  void BeginRun(int transition,int run,int time){
    MakeSingleHistograms();
  }

  TMulticanvas* GetIV3Canvas(){
    TMulticanvas *canvas = new TMulticanvas("IV3 Canvas");
    canvas->AddHistoSingle(fIV3DriveClosedDiff,0);
    canvas->AddHistoSingle(fIV3DriveOpenedDiff,1);
    canvas->AddHistoSingle(fIV3ClosedOpenDiff,2);
    return canvas;
  }

  TH1D *fIV3DriveClosedDiff;
  TH1D *fIV3DriveOpenedDiff;
  TH1D *fIV3ClosedOpenDiff;

 private:

  int fNumberRollovers;
  int fRolloverWord;
  double fTimestamps[MaxChan][2];  // Last timestamp for each channel and leading/falling: 
  int fPPSTimestamp;

};

#endif


