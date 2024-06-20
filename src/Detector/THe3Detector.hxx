#ifndef THe3Detector_h
#define THe3Detector_h

#define USING_V1725_READOUT_HE3 1

#include "TUCNDetectorBaseClass.hxx"

// A derived class from TUCNDetectorBaseClass...
// This class handles getting the hardware level
// information and converting it into the
// standard TUCNHit format...
class THe3Detector : public TUCNDetectorBaseClass {

    public:
        THe3Detector(bool isOffline, bool is3HEDET1, bool saveTree = false);
        void GetHits(TDataContainer& dataContainer);

        // Get a more precise sequence start time from v1720 bank
        virtual bool CheckForSequenceStartPrecise(TDataContainer& dataContainer);
        virtual bool UsePreciseSequenceTime(){
            #ifdef USING_V1725_READOUT_HE3
                return true;
            #else
                return false;
            #endif
        };

    private:
        double initialUnixTime;
};
#endif
