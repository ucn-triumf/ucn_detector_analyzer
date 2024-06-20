#ifndef TUCNRateVsTime_h
#define TUCNRateVsTime_h

#include <string>

#include "THistogramArrayBase.h"
#include "TUCNHit.hxx"

/// Class for making a histogram of UCN rate vs time as seen by He-3 detector,
/// readout with V785 peak-sensing ADC.
class TUCNRateVsTime : public THistogramArrayBase {
    public:
        TUCNRateVsTime(bool isLi6, bool isOffline, bool is3HEDET1);
        virtual ~TUCNRateVsTime(){};

        void UpdateHistograms(TDataContainer& dataContainer){};/// Don't use this update method
        void UpdateHistograms(TUCNHitCollection& hits);
        void BeginRun(int transition,int run,int time);
        void EndRun(int transition,int run,int time);

    private:
        bool fIsLi6;
        bool fIsOffline;
        bool fIs3HEDET1;

        void CreateHistograms();
        int lastTimestamp;

        // Vector storing the rate vs time.
        // first = time, second = rate
        std::vector< std::pair<int,double>>  fRateVsTime;
};

#endif


