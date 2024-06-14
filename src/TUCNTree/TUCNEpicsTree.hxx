#ifndef TUCNEpicsTree_h
#define TUCNEpicsTree_h

#include "mvodb.h"
#include "TTree.h"
#include "TGenericData.hxx"
#include "TDataContainer.hxx"
#include <string>
#include <iostream>

// Base class for making trees with saved data such as EPICS variables
class TUCNEpicsTree {
    public:
        TUCNEpicsTree(MVOdb* odb,
                     char const* bankname, // midas bank from which to fetch data
                     char const* treename, // name of tree to save in file
                     char const* odbpath); // path to list of names in odb for branches
        void FillTree(TDataContainer& dataContainer);

    protected:
        TTree *datatree; // tree with final data
        int timestamp;
        std::vector<double> values; // holds data for placement in tree
        const char* bank; // midas bank from which to fetch data
};

#endif