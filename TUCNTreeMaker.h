#ifndef TUCNTreeMaker_h
#define TUCNTreeMaker_h

#include <string>
#include "TTree.h"
#include "TUCNHit.hxx"
#include "TDataContainer.hxx"
#include "mvodb.h"

// This class will make the tree of UCN hits and the separate tree of sequence transitions
class TUCNHitsTree {
    public:
        TUCNHitsTree(std::string name);
        void FillHits(TUCNHitCollection& hits, int isUCN);
        void FillTransition(double icycleStartTime, double icycleValveOpenTime,
                            double icycleValveCloseTime, double icycleDelayTime,
                            double icycleOpenInterval);

    private:
        std::string detector_name;
        TTree * tRunTran; // run transitions
        int     tRunNum;
        int     tTime;
        double  cycleStartTime;      // time of start of new cycle.
        double  cycleValveOpenTime;  // time that valve opened
        double  cycleValveCloseTime; // time that valve closed
        double  cycleDelayTime;      // delay between end of irradiation and opening valve
        double  cycleOpenInterval;   // length of time that valve is open

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

// Base class for making trees with saved data such as EPICS variables
class TUCNBaseTree {
    public:
        TUCNBaseTree(MVOdb* odb,
                     char const* bankname, // midas bank from which to fetch data
                     char const* treename, // name of tree to save in file
                     char const* odbpath); // path to list of names in odb
        void FillTree(TDataContainer& dataContainer);

    protected:
        TTree *datatree; // tree with final data
        int timestamp;
        std::vector<double> values; // holds data for placement in tree
        const char* bank; // midas bank from which to fetch data
};

/// Derived classes from TUCNBaseTree

// This class will make the tree of source epics variables.
class TUCNSourceEpicsTree: public TUCNBaseTree {
    public:
        TUCNSourceEpicsTree(MVOdb* odb):
            TUCNBaseTree(odb,
                        "EPSR",
                        "SourceEpicsTree",
                        "/Equipment/SourceEpics/Settings/Names") {}
};

// Make tree for beamline epics variables
class TUCNBeamlineEpicsTree: public TUCNBaseTree {
    public:
        TUCNBeamlineEpicsTree(MVOdb* odb):
            TUCNBaseTree(odb,
                        "EPBL",
                        "BeamlineEpicsTree",
                        "/Equipment/BeamlineEpics/Settings/Names") {}
};

// This class will make the tree of LND thermal variables.
class TLNDDetectorTree: public TUCNBaseTree {
    public:
        TLNDDetectorTree(MVOdb* odb):
            TUCNBaseTree(odb,
                        "PICO",
                        "LNDDetectorTree",
                        "/Equipment/scPico/Settings/Names") {}
};

#endif // TUCNTreeMaker_h