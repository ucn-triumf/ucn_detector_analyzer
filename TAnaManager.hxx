#ifndef TAnaManager_h
#define TAnaManager_h

// Use this list here to decide which type of equipment to use.

#define USE_V792
//#define USE_V1190
//#define USE_L2249
//#define USE_AGILENT
#define USE_V1720
//#define USE_V1720_CORRELATIONS
//#define USE_V1730DPP
//#define USE_V1730RAW
//#define USE_DT724

#include "TV792Histogram.h"
#include "THe3RateVsTime.h"
#include "TV1720Waveform.h"

/// This is an example of how to organize a set of different histograms
/// so that we can access the same information in a display or a batch
/// analyzer.
/// Change the set of ifdef's above to define which equipment to use.
class TAnaManager  {
public:
  TAnaManager();
  virtual ~TAnaManager(){};

  /// Processes the midas event, fills histograms, etc.
  int ProcessMidasEvent(TDataContainer& dataContainer);

	/// Methods for determining if we have a particular set of histograms.
	bool HaveV792Histograms();
	bool HaveHe3RateHistograms();
	bool HaveV1720Histograms();
	/// Methods for getting particular set of histograms.
	TV792Histograms* GetV792Histograms();
	TV1720Waveform* GetV1720Histograms();
        THe3RateVsTime* GetHe3RateHistograms();
private:

	TV792Histograms *fV792Histogram;
	TV1720Waveform *fV1720Waveform;
  THe3RateVsTime* fHe3RateVsTime;
  
};



#endif


