#ifndef TSCMTree_h
#define TSCMTree_h

#include "TTree.h"
#include "TDataContainer.hxx"
#include "TGenericData.hxx"
#include <iostream>

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
#endif