#ifndef TLNDDetectorTree_h
#define TLNDDetectorTree_h

#include "TTree.h"
#include "TDataContainer.hxx"
#include "TGenericData.hxx"
#include <iostream>

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

#endif