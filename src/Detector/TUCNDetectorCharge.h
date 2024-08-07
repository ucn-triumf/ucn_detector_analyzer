#ifndef TUCNDetectorCharge_h
#define TUCNDetectorCharge_h

#include <string>
#include "THistogramArrayBase.h"
#include "TUCNHit.hxx"

/// Class for making histograms of charge from UCN detector
/// (either Li6 or He3)
class TUCNDetectorCharge : public THistogramArrayBase {
    public:
        TUCNDetectorCharge(bool isLi6, bool is3HEDET1);
        virtual ~TUCNDetectorCharge(){};

        /// We will fill this histogram with separate method
        void UpdateHistograms(TDataContainer& dataContainer){};

        /// Fill the charge spectrum with the set of ucn hits
        void UpdateHistograms(TUCNHitCollection & hits);

        /// Take actions at begin run
        void BeginRun(int transition,int run,int time);

        /// Take actions at end run
        void EndRun(int transition,int run,int time);

    private:
        bool fIsLi6;
        std::string det_name;       // detector name
        void CreateHistograms();
};
#endif


