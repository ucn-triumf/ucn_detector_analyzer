#ifndef TV1725WaveformDisplay_h
#define TV1725WaveformDisplay_h

#include <string>
#include "THistogramArrayBase.h"
#include "TH2.h"
#include "TH1D.h"
#include "TF1.h"
#include <vector>

#include "TCanvas.h"
#include "TTree.h"

#include "TVConstants.h"

#include "time.h"

#define V1725_MAXCHAN 16

class TV1725WaveformDisplay : public THistogramArrayBase
{
 public: 
  TV1725WaveformDisplay();
  virtual ~TV1725WaveformDisplay(){};

  void UpdateHistograms(TDataContainer&);

  void BeginRun(int transition, int run, int time);
  
  void EndRun(int transition, int run, int time);

  void CreateHistograms();
  };



class TV1725PSDQL : public THistogramArrayBase
{
 public: 
  TV1725PSDQL();
  virtual ~TV1725PSDQL(){};

  void UpdateHistograms(TDataContainer&);

  void BeginRun(int transition, int run, int time);
  
  void EndRun(int transition, int run, int time);

  void CreateHistograms();
};

class TV1725QSQS : public THistogramArrayBase//added Aug 10, 2016
{
 public: 
  TV1725QSQS();
  virtual ~TV1725QSQS(){};

  void UpdateHistograms(TDataContainer&){};
  void UpdateHistogram(int board, int chan, uint16_t QSCal[], uint16_t QSBoard[], int nEvents);

  void BeginRun(int transition, int run, int time);
  
  void EndRun(int transition, int run, int time);

  void CreateHistograms();
  };

class TV1725_PH : public THistogramArrayBase//added Aug 10, 2016
{
 public: 
  TV1725_PH();
  virtual ~TV1725_PH(){};

  void UpdateHistograms(TDataContainer&){};
  void UpdateHistogram(int board, int chan, uint32_t pulseHeight);

  void BeginRun(int transition, int run, int time);
  
  void EndRun(int transition, int run, int time);

  void CreateHistograms();
  };
#endif
