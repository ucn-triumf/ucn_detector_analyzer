#ifndef TV1720WaveformDisplay_h
#define TV1720WaveformDisplay_h

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

class TV1720WaveformDisplay : public THistogramArrayBase//added June 10, 2016
{
 public: 
  TV1720WaveformDisplay();
  virtual ~TV1720WaveformDisplay(){};

  void UpdateHistograms(TDataContainer&){};
  void UpdateHistogram(int board, int chan, uint16_t * wf, int tLength,
		       char * ctag);

  void BeginRun(int transition, int run, int time);
  
  void EndRun(int transition, int run, int time);

  void CreateHistograms();
  };


/*class TV1720CLQEvNum : public THistogramArrayBase//added Aug 9, 2016
{
 public: 
  TV1720CLQEvNum();
  virtual ~TV1720CLQEvNum(){};

  void UpdateHistograms(TDataContainer&){};
  void UpdateHistogram(int board, int chan, float QLDifference[], int nEvents,
		       char * ctag );

  void BeginRun(int transition, int run, int time);
  
  void EndRun(int transition, int run, int time);

  void CreateHistograms();
  };*/

/*class TV1720CSQEvNum : public THistogramArrayBase//added Aug 9, 2016
{
 public: 
  TV1720CSQEvNum();
  virtual ~TV1720CSQEvNum(){};

  void UpdateHistograms(TDataContainer&){};
  void UpdateHistogram(int board, int chan, float QSDifference[], int nEvents,
		       char * ctag );

  void BeginRun(int transition, int run, int time);
  
  void EndRun(int transition, int run, int time);

  void CreateHistograms();
  };*/

class TV1720QLQL : public THistogramArrayBase//added Aug 10, 2016
{
 public: 
  TV1720QLQL();
  virtual ~TV1720QLQL(){};

  void UpdateHistograms(TDataContainer&){};
  void UpdateHistogram(int board, int chan, uint16_t QLCal[], uint16_t QLBoard[], int nEvents);

  void BeginRun(int transition, int run, int time);
  
  void EndRun(int transition, int run, int time);

  void CreateHistograms();
};

class TV1720QSQS : public THistogramArrayBase//added Aug 10, 2016
{
 public: 
  TV1720QSQS();
  virtual ~TV1720QSQS(){};

  void UpdateHistograms(TDataContainer&){};
  void UpdateHistogram(int board, int chan, uint16_t QSCal[], uint16_t QSBoard[], int nEvents);

  void BeginRun(int transition, int run, int time);
  
  void EndRun(int transition, int run, int time);

  void CreateHistograms();
  };

class TV1720_PH : public THistogramArrayBase//added Aug 10, 2016
{
 public: 
  TV1720_PH();
  virtual ~TV1720_PH(){};

  void UpdateHistograms(TDataContainer&){};
  void UpdateHistogram(int board, int chan, uint32_t pulseHeight);

  void BeginRun(int transition, int run, int time);
  
  void EndRun(int transition, int run, int time);

  void CreateHistograms();
  };
#endif
