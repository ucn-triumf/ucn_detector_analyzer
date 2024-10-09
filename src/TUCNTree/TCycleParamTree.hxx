#ifndef TCycleParamTree_h
#define TCycleParamTree_h

#include "TTree.h"
#include "TDataContainer.hxx"
#include <iostream>
#include <string>
#include <sstream>
#include "mvodb.h"


// This class will make the tree of Sequencer variables.
class TCycleParamTree {
    public:
        TCycleParamTree(MVOdb* odb = 0);
        void FillTree(TDataContainer& dataContainer);

    private:
        TTree *tSequencer;

        std::string dirname = "/Equipment/UCNSequencer2018/";

        // settings from sequence_control_multi_valve.cxx
        static const int MaxPeriods = 10; // number of periods per cycle
        static const int MaxCycles = 20;  // number of cycles per super-cycle
        static const int NValves = 8; // Number of valves
        static const int NNotes = 5; // Number of notes

        // other variables - read out from odb
        int NPeriods; // number of periods per cycle
        int NCycles;  // number of cycles per super-cycle
        int enable;
        int nsuper;
        int infcycles;
        int period_idx;
        int valve_idx;
        std::vector<int> duration;
        std::vector<int> valve;
};
#endif