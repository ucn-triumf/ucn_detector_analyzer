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

  void UpdateHistograms(TDataContainer& dataContainer);

  void BeginRun(int transition, int run, int time);
  
  void EndRun(int transition, int run, int time);

  void CreateHistograms();
};


class TV1725_PH : public THistogramArrayBase 
{
 public: 
  TV1725_PH();
  virtual ~TV1725_PH(){};

  void UpdateHistograms(TDataContainer& dataContainer);

  void BeginRun(int transition, int run, int time);
  
  void EndRun(int transition, int run, int time);

  void CreateHistograms();
  };

class TV1725_QL : public THistogramArrayBase 
{
 public: 
  TV1725_QL();
  virtual ~TV1725_QL(){};

  void UpdateHistograms(TDataContainer& dataContainer);

  void BeginRun(int transition, int run, int time);
  
  void EndRun(int transition, int run, int time);

  void CreateHistograms();
  };
#endif
