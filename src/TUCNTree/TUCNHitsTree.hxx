#ifndef TUCNHitsTree_h
#define TUCNHitsTree_h

#include <string>
#include <iostream>
#include "TTree.h"
#include "TUCNHit.hxx"
#include "TUCNCycleParameters.hxx"

// This class will make the tree of UCN hits and the separate tree of sequence transitions
class TUCNHitsTree {
    public:
        TUCNHitsTree(std::string name);
        void FillHits(TUCNHitCollection& hits, int isUCN);
        void FillTransition(double icycleStartTime, double icycleValveOpenTime, double icycleValveCloseTime,
                            double icycleDelayTime, double icycleOpenInterval, TUCNCycleParameters CycleParameters);

    private:
        std::string detector_name;
        TTree * tRunTran; // run transitions
        int     tRunNum;
        int     tTime;
        int     cycleIndex;          // cycle index
        int     superCycleIndex;          // super cycle index
        double  cycleStartTime;      // time of start of new cycle.
        double  cycleValveOpenTime;  // time that valve opened
        double  cycleValveCloseTime; // time that valve closed
        double  cycleDelayTime;      // delay between end of irradiation and opening valve
        double  cycleOpenInterval;   // length of time that valve is open
        double  cyclePeriod0EndTime; // end of period 0
        double  cyclePeriod1EndTime; // end of period 1
        double  cyclePeriod2EndTime; // end of period 2
        double  cyclePeriod3EndTime; // end of period 3
        double  cyclePeriod4EndTime; // end of period 4
        double  cyclePeriod5EndTime; // end of period 5
        double  cyclePeriod6EndTime; // end of period 6
        double  cyclePeriod7EndTime; // end of period 7
        double  cyclePeriod8EndTime; // end of period 8
        double  cyclePeriod9EndTime; // end of period 9
        int valveStatePeriod0[8]; // state of the 8 valves for period 0. array index = 0 means valve 1.
        int valveStatePeriod1[8]; // state of the 8 valves for period 1. array index = 0 means valve 1.
        int valveStatePeriod2[8]; // state of the 8 valves for period 2. array index = 0 means valve 1.
        int valveStatePeriod3[8]; // state of the 8 valves for period 3. array index = 0 means valve 1.
        int valveStatePeriod4[8]; // state of the 8 valves for period 4. array index = 0 means valve 1.
        int valveStatePeriod5[8]; // state of the 8 valves for period 5. array index = 0 means valve 1.
        int valveStatePeriod6[8]; // state of the 8 valves for period 6. array index = 0 means valve 1.
        int valveStatePeriod7[8]; // state of the 8 valves for period 7. array index = 0 means valve 1.
        int valveStatePeriod8[8]; // state of the 8 valves for period 8. array index = 0 means valve 1.
        int valveStatePeriod9[8]; // state of the 8 valves for period 9. array index = 0 means valve 1.
        bool finishedFirstSuperCycle;

        // output file
        TTree * tUCN;

        // branches

        // tUCN
        ULong_t tEntry;
        ULong_t tEvent;
        ULong64_t tTimeE;
        ULong64_t tTimeStamp;
        double tUnixTime;
        double tUnixTimePrecise;
        Short_t tChannel;
        Short_t tLength;
        //  Short_t tPulse[1000];
        Float_t tPSD;
        Short_t tChargeL;
        Short_t tChargeS;
        Short_t tBaseline;
        Short_t tIsUCN;
};
#endif