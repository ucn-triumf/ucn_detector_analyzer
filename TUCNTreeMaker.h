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

// Epics variables
class SourceEpics: public TUCNBaseTree {
    public:
        SourceEpics(MVOdb* odb):
            TUCNBaseTree(odb,
                        "EPSR",
                        "SourceEpics",
                        "/Equipment/SourceEpics/Settings/Names") {}
};

class BeamlineEpics: public TUCNBaseTree {
    public:
        BeamlineEpics(MVOdb* odb):
            TUCNBaseTree(odb,
                        "EPBL",
                        "BeamlineEpics",
                        "/Equipment/BeamlineEpics/Settings/Names") {}
};

class UCN2Epics: public TUCNBaseTree {
    public:
        UCN2Epics(MVOdb* odb):
            TUCNBaseTree(odb,
                        "EPU2",
                        "UCN2Epics",
                        "/Equipment/UCN2Epics/Settings/Names") {}
};

class UCN2EpicsTemperature: public TUCNBaseTree {
    public:
        UCN2EpicsTemperature(MVOdb* odb):
            TUCNBaseTree(odb,
                        "EP2T",
                        "UCN2EpicsTemperature",
                        "/Equipment/UCN2EpicsTemperature/Settings/Names") {}
};

class UCN2EpicsPressures: public TUCNBaseTree {
    public:
        UCN2EpicsPressures(MVOdb* odb):
            TUCNBaseTree(odb,
                        "EP2T", // same bank name as temperature?
                        "UCN2EpicsPressures",
                        "/Equipment/UCN2EpicsPressures/Settings/Names") {}
};

class UCN2EpicsOthers: public TUCNBaseTree {
    public:
        UCN2EpicsOthers(MVOdb* odb):
            TUCNBaseTree(odb,
                        "EP2T", // same bank name as temperature?
                        "UCN2EpicsOthers",
                        "/Equipment/UCN2EpicsOthers/Settings/Names") {}
};

class UCN2EpicsPhase2B: public TUCNBaseTree {
    public:
        UCN2EpicsPhase2B(MVOdb* odb):
            TUCNBaseTree(odb,
                        "EPH2",
                        "UCN2EpicsPhase2B",
                        "/Equipment/UCN2EpicsPhase2B/Settings/Names") {}
};

// LND thermal variables
class scPico: public TUCNBaseTree {
    public:
        scPico(MVOdb* odb):
            TUCNBaseTree(odb,
                        "PICO",
                        "scPico",
                        "/Equipment/scPico/Settings/Names") {}
};

#endif // TUCNTreeMaker_h