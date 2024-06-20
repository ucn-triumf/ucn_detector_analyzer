#ifndef TV1720Histograms_h
#define TV1720Histograms_h

#include <string>
#include "THistogramArrayBase.h"
#include "TH2.h"
#include <vector>

#include "TVConstants.h"

/// Class for making histograms of Q short vs Q long
class TV1720QSQLHistograms : public THistogramArrayBase {
    public:
        TV1720QSQLHistograms();
        virtual ~TV1720QSQLHistograms(){};

        /// This method works, but is generally not used; instead, TUCNAnaManager handles filling this histogram.
        void UpdateHistograms(TDataContainer&){};
        void UpdateHistogram(int board, int chan, Float_t QS, Float_t QL);

        void BeginRun(int transition,int run,int time);
        void EndRun(int transition,int run,int time);
        void CreateHistograms();
};

/// Class for making histograms of PSD vs Q long
class TV1720PSDQLHistograms : public THistogramArrayBase {
    public:
        TV1720PSDQLHistograms();
        virtual ~TV1720PSDQLHistograms(){};

        /// This method works, but is generally not used; instead, TUCNAnaManager handles filling this histogram.
        void UpdateHistograms(TDataContainer&){};
        void UpdateHistogram(int board, int chan, Float_t PSD, Float_t QL);

        void BeginRun(int transition,int run,int time);
        void EndRun(int transition,int run,int time);
        void CreateHistograms();
};
#endif


