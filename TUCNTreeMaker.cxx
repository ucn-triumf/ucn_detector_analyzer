
#include "TUCNTreeMaker.h"
#include <iostream>
#include "TGenericData.hxx"

// Constructor
TUCNHitsTree::TUCNHitsTree(std::string name):detector_name(name){

    std::cout << "Creating trees for " << detector_name << std::endl;

    // tree for run transitions
    char tree_name[100];
    sprintf(tree_name, "RunTransitions_%s", detector_name.c_str());
    tRunTran = new TTree(tree_name, "RunTransitions");
    tRunTran->Branch("tRunNum", &tRunNum, "tRunNum/I" );
    tRunTran->Branch("tTime", &tTime, "tTime/I" );
    tRunTran->Branch("tEntry",&tEntry, "tEntry/l" );
    tRunTran->Branch("cycleStartTime",&cycleStartTime,"cycleStartTime/D");
    tRunTran->Branch("cycleValveOpenTime",&cycleValveOpenTime,"cycleValveOpenTime/D");
    tRunTran->Branch("cycleValveCloseTime",&cycleValveCloseTime,"cycleValveCloseTime/D");
    tRunTran->Branch("cycleDelayTime",&cycleDelayTime,"cycleDelayTime/D");
    tRunTran->Branch("cycleOpenInterval",&cycleOpenInterval,"cycleOpenInterval/D");

    //tRunNum = run;
    //tTime = time;
    tEntry=0;

    // tree for runtime event data
    sprintf(tree_name,"UCNHits_%s",detector_name.c_str());
    tUCN=new TTree(tree_name,"Event");
    tUCN->Branch("tEntry",    &tEntry,    "tEntry/l");
    tUCN->Branch("tTimeE",    &tTimeE,    "tTimeE/l");
    tUCN->Branch("tTimeStamp",    &tTimeStamp,    "tTimeStamp/l");
    tUCN->Branch("tUnixTime",    &tUnixTime,    "tUnixTime/D");
    tUCN->Branch("tUnixTimePrecise",    &tUnixTimePrecise,    "tUnixTimePrecise/D");
    tUCN->Branch("tChannel",  &tChannel,  "tChannel/s");
    tUCN->Branch("tPSD",      &tPSD,      "tPSD/f");
    tUCN->Branch("tChargeL",  &tChargeL,  "tChargeL/s");
    tUCN->Branch("tChargeS",  &tChargeS,  "tChargeS/s");
    tUCN->Branch("tBaseline", &tBaseline, "tBaseline/s");
    tUCN->Branch("tLength",   &tLength,   "tLength/s");
    tUCN->Branch("tIsUCN",   &tIsUCN,   "tIsUCN/s");
    //tUCN->Branch("tPulse",     tPulse,    "tPulse[tLength]/s");
}

// Fill the TUCNHitsTree
void TUCNHitsTree::FillHits(TUCNHitCollection& hits, int isUCN){

    for(unsigned int j = 0; j < hits.size(); j++){ // loop over measurements

        // get measurement
        TUCNHit hit = hits[j];

        // get data from measurement
        tTimeStamp = hit.clockTime;
        tUnixTime = hit.time;
        tUnixTimePrecise = hit.preciseTime;
        tChargeL = hit.chargeLong;
        tChargeS = hit.chargeShort;
        tBaseline = hit.baseline;

        if(hit.psd > -2.0)  tPSD = hit.psd;
        else                tPSD = -2.0;

        tChannel = hit.channel;
        tIsUCN = isUCN;
        tUCN->Fill();
        tEntry++;
    }
}

void TUCNHitsTree::FillTransition(double icycleStartTime, double icycleValveOpenTime,
    double icycleValveCloseTime, double icycleDelayTime, double icycleOpenInterval){

    cycleStartTime = icycleStartTime;
    cycleValveOpenTime = icycleValveOpenTime;
    cycleValveCloseTime = icycleValveCloseTime;
    cycleDelayTime = icycleDelayTime;
    cycleOpenInterval = icycleOpenInterval;

    std::cout << "Transition time: " << cycleStartTime << " " << cycleDelayTime << std::endl;
    tRunTran->Fill();
}

TUCNBaseTree::TUCNBaseTree(MVOdb* odb, char const* bankname, char const* treename,
                            char const* odbpath){
    /**
     * @brief Base constructor for objects which create trees from MIDAS banks.
     *
     * @param odb Pointer to ODB object to read list of parameter names
     * @param bankname  Name of the MIDAS bank which holds the data, get the
     *                  value from the frontend code
     * @param treename  Name of the TTree to write out to file
     * @param odbpath   Path to the list of parameter names.
     *                  Typically /Equipment/something/Settings/Names
     */

    // save input
    bank = bankname;

    // create tree and timestamp branch
    datatree = new TTree(treename, (std::string(treename)+" TTree").c_str());
    datatree->Branch("timestamp", &timestamp, "timestamp/I" );

    // read names
    std::vector<std::string> names = std::vector<std::string>();
    odb->RSA(odbpath, &names);

    values.resize(names.size());    // prevents segfault on tree fill

    // setup branches (don't use default branches)
    for (long unsigned int i=0; i<names.size(); i++){
        if (names[i].find("Default") == std::string::npos){

            // can't have colons in the name or leaf list is expanded
            std::replace(names[i].begin(), names[i].end(), ':', '_');

            // make branch
            datatree->Branch(names[i].c_str(), &values[i], (names[i]+"/D").c_str());
        }
    }
};

void TUCNBaseTree::FillTree(TDataContainer& dataContainer){
    /**
     * @brief Fill the trees with data from MIDAS events.
     */

    // Use the sequence bank to see when a new run starts:
    TGenericData *data = dataContainer.GetEventData<TGenericData>(bank);

    if(!data) return;

    // Save the unix timestamp
    timestamp = dataContainer.GetMidasData().GetTimeStamp();

    // Save variables
    for (long unsigned int i=0; i<values.size(); i++){
        values[i] = data->GetDouble()[i];
    }
    datatree->Fill();
};

