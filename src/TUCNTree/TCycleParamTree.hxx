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
        int timestamp;

        // settings from sequence_control_multi_valve.cxx
        int NPeriods; // number of periods per cycle
        int NCycles;  // number of cycles per super-cycle

        // other settings
        static const int NValves = 8; // Number of valves
        std::vector<std::string> branch_names = {"cycle_index",
                                                 "enable",
                                                 "nperiods_in_cycle",
                                                 "ncycles_in_super",
                                                 "inf_cycles",
                                                };

        std::vector<int> values; // hold values for branches
};
#endif