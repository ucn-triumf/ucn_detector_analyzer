#ifndef TV1730WaveformDisplay_h
#define TV1730WaveformDisplay_h

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

class TV1730WaveformDisplay : public THistogramArrayBase{
    public:
        TV1730WaveformDisplay();
        virtual ~TV1730WaveformDisplay(){};
        void UpdateHistograms(TDataContainer&);
        void BeginRun(int transition, int run, int time);
        void EndRun(int transition, int run, int time);
        void CreateHistograms();
};

// pulse shape discrimination charge long
class TV1730PSDQL : public THistogramArrayBase{
    public:
        TV1730PSDQL();
        virtual ~TV1730PSDQL(){};
        void UpdateHistograms(TDataContainer& dataContainer);
        void BeginRun(int transition, int run, int time);
        void EndRun(int transition, int run, int time);
        void CreateHistograms();
};

// pulse height
class TV1730_PH : public THistogramArrayBase{
    public:
        TV1730_PH();
        virtual ~TV1730_PH(){};
        void UpdateHistograms(TDataContainer& dataContainer);
        void BeginRun(int transition, int run, int time);
        void EndRun(int transition, int run, int time);
        void CreateHistograms();
};

// charge long
class TV1730_QL : public THistogramArrayBase{
    public:
        TV1730_QL();
        virtual ~TV1730_QL(){};
        void UpdateHistograms(TDataContainer& dataContainer);
        void BeginRun(int transition, int run, int time);
        void EndRun(int transition, int run, int time);
        void CreateHistograms();
};

// charge short
class TV1730_QS : public THistogramArrayBase{
    public:
        TV1730_QS();
        virtual ~TV1730_QS(){};
        void UpdateHistograms(TDataContainer& dataContainer);
        void BeginRun(int transition, int run, int time);
        void EndRun(int transition, int run, int time);
        void CreateHistograms();
};
#endif
