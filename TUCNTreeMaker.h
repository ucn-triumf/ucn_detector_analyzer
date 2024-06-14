#ifndef TUCNTreeMaker_h
#define TUCNTreeMaker_h

#include <string>
#include "TTree.h"
#include "TUCNHit.hxx"
#include "TDataContainer.hxx"
#include "TUCNCycleParameters.hxx"
#include "mvodb.h"

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

// This class will make the tree of LND thermal variables.
class TLNDDetectorTree {
    public:
        TLNDDetectorTree();
        void FillTree(TDataContainer& dataContainer);

    private:
        TTree *tLND;
        int timestamp;
        double LND_Reading;
};

// This class will make the tree of SCM variables.
class TSCMTree {
    public:
        TSCMTree();
        void FillTree(TDataContainer& dataContainer);

    private:
        TTree *tSCM;
        int timestamp;
        float SCMTemps[8];
        float SCMVoltages[4];
};

// This class will make the tree of Sequencer variables.
class TSequencerTree {
    public:
        TSequencerTree();
        void FillTree(TDataContainer& dataContainer);

    private:
        TTree *tSequencer;
        int timestamp;
        int sequencerEnabled; // Is sequencer enabled
        int inCycle; // Are we in a cycle
        int cycleStarted; // Did cycleJust start?
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
