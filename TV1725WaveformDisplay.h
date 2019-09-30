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

class TV1725WaveformDisplay : public THistogramArrayBase//added June 10, 2016
{
 public: 
  TV1725WaveformDisplay();
  virtual ~TV1725WaveformDisplay(){};

  void UpdateHistograms(TDataContainer&);
  void UpdateHistogram(int board, int chan, uint16_t * wf, int tLength,
		       char * ctag);

  void BeginRun(int transition, int run, int time);
  
  void EndRun(int transition, int run, int time);

  void CreateHistograms();
  };


/*class TV1725CLQEvNum : public THistogramArrayBase//added Aug 9, 2016
{
 public: 
  TV1725CLQEvNum();
  virtual ~TV1725CLQEvNum(){};

  void UpdateHistograms(TDataContainer&){};
  void UpdateHistogram(int board, int chan, float QLDifference[], int nEvents,
		       char * ctag );

  void BeginRun(int transition, int run, int time);
  
  void EndRun(int transition, int run, int time);

  void CreateHistograms();
  };*/

/*class TV1725CSQEvNum : public THistogramArrayBase//added Aug 9, 2016
{
 public: 
  TV1725CSQEvNum();
  virtual ~TV1725CSQEvNum(){};

  void UpdateHistograms(TDataContainer&){};
  void UpdateHistogram(int board, int chan, float QSDifference[], int nEvents,
		       char * ctag );

  void BeginRun(int transition, int run, int time);
  
  void EndRun(int transition, int run, int time);

  void CreateHistograms();
  };*/

class TV1725QLQL : public THistogramArrayBase//added Aug 10, 2016
{
 public: 
  TV1725QLQL();
  virtual ~TV1725QLQL(){};

  void UpdateHistograms(TDataContainer&){};
  void UpdateHistogram(int board, int chan, uint16_t QLCal[], uint16_t QLBoard[], int nEvents);

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
