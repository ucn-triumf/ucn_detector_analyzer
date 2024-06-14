#ifndef TSequencerTree_h
#define TSequencerTree_h

#include "TTree.h"
#include "TDataContainer.hxx"
#include <iostream>

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
#endif