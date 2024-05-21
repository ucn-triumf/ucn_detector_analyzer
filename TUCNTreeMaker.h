#ifndef TUCNTreeMaker_h
#define TUCNTreeMaker_h

// This file contains classes for making


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

// Base class for making trees with saved data
class TUCNBaseTree {
    public:
        TUCNBaseTree(MVOdb* odb);
        void FillTree(TDataContainer& dataContainer);

    protected:
        TTree *datatree;
        int timestamp;
        std::vector<double> values;

        const char* BANKNAME;
        const char* TREENAME;
        const char* ODBPATH_KEY;
        const char* ODBPATH_NAMES;
};

// This class will make the tree of source epics variables.
class TUCNSourceEpicsTree: public TUCNBaseTree {
    public:
        using TUCNBaseTree::TUCNBaseTree;

    protected:
        static constexpr const char* BANKNAME = "EPSR";
        static constexpr const char* TREENAME = "SourceEpicsTree";
        static constexpr const char* ODBPATH_NAMES = "/Equipment/SourceEpics/Settings/Names";
};

// Make tree for beamline epics variables
class TUCNBeamlineEpicsTree: public TUCNBaseTree {
    public:
        using TUCNBaseTree::TUCNBaseTree;

    protected:
        static constexpr const char* BANKNAME = "EPBL";
        static constexpr const char* TREENAME = "BeamlineEpicsTree";
        static constexpr const char* ODBPATH_NAMES = "/Equipment/BeamlineEpics/Settings/Names";
};


#endif // TUCNTreeMaker_h