
#include "TUCNTreeMaker.h"
#include <iostream>
#include "TGenericData.hxx"

// Constructor
TUCNHitsTree::TUCNHitsTree(std::string name):detector_name(name){

    std::cout << "Creating trees for " << detector_name << std::endl;

    // tree for run transitions
    char tree_name[100];
    sprintf(tree_name,"RunTransitions_%s",detector_name.c_str());
    tRunTran = new TTree(tree_name, "RunTransitions");
    tRunTran->Branch("tRunNum", &tRunNum, "tRunNum/I" );
    tRunTran->Branch("tTime", &tTime, "tTime/I" );
    tRunTran->Branch("tEntry",&tEntry, "tEntry/l" );
    tRunTran->Branch("cycleIndex",&cycleIndex,"cycleIndex/I");
    tRunTran->Branch("superCycleIndex",&superCycleIndex,"superCycleIndex/I");
    tRunTran->Branch("cycleStartTime",&cycleStartTime,"cycleStartTime/D");
    tRunTran->Branch("cycleValveOpenTime",&cycleValveOpenTime,"cycleValveOpenTime/D");
    tRunTran->Branch("cycleValveCloseTime",&cycleValveCloseTime,"cycleValveCloseTime/D");
    tRunTran->Branch("cycleDelayTime",&cycleDelayTime,"cycleDelayTime/D");
    tRunTran->Branch("cycleValveOpenTime",&cycleValveOpenTime,"cycleValveOpenTime/D");
    tRunTran->Branch("cyclePeriod0EndTime",&cyclePeriod0EndTime,"cyclePeriod0EndTime/D");
    tRunTran->Branch("cyclePeriod1EndTime",&cyclePeriod1EndTime,"cyclePeriod1EndTime/D");
    tRunTran->Branch("cyclePeriod2EndTime",&cyclePeriod2EndTime,"cyclePeriod2EndTime/D");
    tRunTran->Branch("cyclePeriod3EndTime",&cyclePeriod3EndTime,"cyclePeriod3EndTime/D");
    tRunTran->Branch("cyclePeriod4EndTime",&cyclePeriod4EndTime,"cyclePeriod4EndTime/D");
    tRunTran->Branch("cyclePeriod5EndTime",&cyclePeriod5EndTime,"cyclePeriod5EndTime/D");
    tRunTran->Branch("cyclePeriod6EndTime",&cyclePeriod6EndTime,"cyclePeriod6EndTime/D");
    tRunTran->Branch("cyclePeriod7EndTime",&cyclePeriod7EndTime,"cyclePeriod7EndTime/D");
    tRunTran->Branch("cyclePeriod8EndTime",&cyclePeriod8EndTime,"cyclePeriod8EndTime/D");
    tRunTran->Branch("cyclePeriod9EndTime",&cyclePeriod9EndTime,"cyclePeriod9EndTime/D");
    tRunTran->Branch("valveStatePeriod0",valveStatePeriod0,"valveStatePeriod0[8]/I");
    tRunTran->Branch("valveStatePeriod1",valveStatePeriod1,"valveStatePeriod1[8]/I");
    tRunTran->Branch("valveStatePeriod2",valveStatePeriod2,"valveStatePeriod2[8]/I");
    tRunTran->Branch("valveStatePeriod3",valveStatePeriod3,"valveStatePeriod3[8]/I");
    tRunTran->Branch("valveStatePeriod4",valveStatePeriod4,"valveStatePeriod4[8]/I");
    tRunTran->Branch("valveStatePeriod5",valveStatePeriod5,"valveStatePeriod5[8]/I");
    tRunTran->Branch("valveStatePeriod6",valveStatePeriod6,"valveStatePeriod6[8]/I");
    tRunTran->Branch("valveStatePeriod7",valveStatePeriod7,"valveStatePeriod7[8]/I");
    tRunTran->Branch("valveStatePeriod8",valveStatePeriod8,"valveStatePeriod8[8]/I");
    tRunTran->Branch("valveStatePeriod9",valveStatePeriod9,"valveStatePeriod9[8]/I");

    //tRunNum = run;
    //tTime = time;
    tEntry=0;
    cycleIndex = -9999;
    finishedFirstSuperCycle = false;

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

        tTimeStamp = hit.clockTime;
        tUnixTime = hit.time;
        tUnixTimePrecise = hit.preciseTime;
        tChargeL = hit.chargeLong;
        tChargeS = hit.chargeShort;
        tBaseline = hit.baseline;
        if(hit.psd > -2.0)
        tPSD = hit.psd;
        else
        tPSD = -2.0;

        tChannel = hit.channel;
        tIsUCN = isUCN;
        tUCN->Fill();
        tEntry++;
    }
}

void TUCNHitsTree::FillTransition(double icycleStartTime, double icycleValveOpenTime, double icycleValveCloseTime,
                                  double icycleDelayTime, double icycleOpenInterval,
				  TUCNCycleParameters CycleParameters){

    // Look for cases where it seems we finished a super-cycle.
    if(CycleParameters.CycleIndex() <= cycleIndex){
        finishedFirstSuperCycle = true;
    }

    cycleIndex = CycleParameters.CycleIndex();
    cycleStartTime = icycleStartTime;
    cycleValveOpenTime = icycleValveOpenTime;
    cycleValveCloseTime = icycleValveCloseTime;
    cycleDelayTime = icycleDelayTime;
    cycleOpenInterval = icycleOpenInterval;

    /// Problems with how the data is stored means that the data needs to be modified.
    /// Need to hack it...
    /// This variable can't really be trusted...
    if(!finishedFirstSuperCycle){
        superCycleIndex = CycleParameters.SuperCycleIndex();
    }else{
        superCycleIndex = CycleParameters.SuperCycleIndex() + 1;
    }

    std::cout << "Setting super cycle (fixed): " << superCycleIndex << " "
                << std::endl;
    std::cout << "Transition time: " << cycleStartTime << " " << cycleDelayTime << std::endl;
    // Fill the parameters from the new NSEQ bank...

    std::cout << "periods diff: " << CycleParameters.CycleIndex() << " "
            << CycleParameters.NumberPeriods() << std::endl;

    cyclePeriod0EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(0);
    cyclePeriod1EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(1);
    cyclePeriod2EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(2);
    cyclePeriod3EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(3);
    cyclePeriod4EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(4);
    cyclePeriod5EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(5);
    cyclePeriod6EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(6);
    cyclePeriod7EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(7);
    cyclePeriod8EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(8);
    cyclePeriod9EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(9);

    if(0)std::cout << "NNN "  << CycleParameters.GetCumulativeTimeForPeriod(5) << " " << (int) cyclePeriod5EndTime << " "
                << CycleParameters.GetCumulativeTimeForPeriod(6) << " " << (int) cyclePeriod6EndTime << " "
                << CycleParameters.GetCumulativeTimeForPeriod(7) << " "<< (int) cyclePeriod7EndTime << " "
                << CycleParameters.GetCumulativeTimeForPeriod(8) << " "<< (int) cyclePeriod8EndTime << " "
                << CycleParameters.GetCumulativeTimeForPeriod(9) << " "<< (int) cyclePeriod9EndTime << std::endl;

    for(int i = 0; i < 8; i++){
        valveStatePeriod0[i] = CycleParameters.GetValveState(0,i);
        valveStatePeriod1[i] = CycleParameters.GetValveState(1,i);
        valveStatePeriod2[i] = CycleParameters.GetValveState(2,i);
        valveStatePeriod3[i] = CycleParameters.GetValveState(3,i);
        valveStatePeriod4[i] = CycleParameters.GetValveState(4,i);
        valveStatePeriod5[i] = CycleParameters.GetValveState(5,i);
        valveStatePeriod6[i] = CycleParameters.GetValveState(6,i);
        valveStatePeriod7[i] = CycleParameters.GetValveState(7,i);
        valveStatePeriod8[i] = CycleParameters.GetValveState(8,i);
        valveStatePeriod9[i] = CycleParameters.GetValveState(9,i);
    }
    tRunTran->Fill();
}

TLNDDetectorTree::TLNDDetectorTree() {
    std::cout << "Creating LND Thermal neutron detector tree "<< std::endl;
    tLND = new TTree("LNDDetectorTree", "LNDDetectorTree");
    tLND->Branch("timestamp", &timestamp, "timestamp/I" );
    tLND->Branch("LND_Reading", &LND_Reading, "LND_Reading/D" );
}

void TLNDDetectorTree::FillTree(TDataContainer& dataContainer){
    // Use the sequence bank to see when a new run starts:
    TGenericData *data = dataContainer.GetEventData<TGenericData>("PICO");
    if(!data) return;

    // Save the unix timestamp
    timestamp = dataContainer.GetMidasData().GetTimeStamp();
    LND_Reading =  data->GetDouble()[0];
    tLND->Fill();
}

TSequencerTree::TSequencerTree(){
    std::cout << "Creating Sequencer tree "<< std::endl;
    tSequencer = new TTree("SequencerTree", "SequencerTree");
    tSequencer->Branch("timestamp", &timestamp, "timestamp/I" );
    tSequencer->Branch("sequencerEnabled", &sequencerEnabled, "sequencerEnabled/I" );
    tSequencer->Branch("inCycle", &inCycle, "inCycle/I" );
    tSequencer->Branch("cycleStarted", &cycleStarted, "cycleStarted/I" );
};

void TSequencerTree::FillTree(TDataContainer& dataContainer){

  // Use the sequence bank to see when a new run starts:
  TGenericData *data = dataContainer.GetEventData<TGenericData>("USEQ");
  if(!data) return;

  timestamp = dataContainer.GetMidasData().GetTimeStamp();
  sequencerEnabled = data->GetData32()[5];
  inCycle = (data->GetData32()[4] & 1);
  cycleStarted = (data->GetData32()[4] & 2);

  tSequencer->Fill();

};

TSCMTree::TSCMTree() {
    std::cout << "Creating SCM tree "<< std::endl;
    tSCM = new TTree("SCMTree", "SCMTree");
    tSCM->Branch("timestamp", &timestamp, "timestamp/I" );
    tSCM->Branch("SCMTemps", SCMTemps, "SCMTemps[8]/F" );
    tSCM->Branch("SCMVoltages", SCMVoltages, "SCMVoltages[4]/F" );
}

void TSCMTree::FillTree(TDataContainer& dataContainer){

    // Grab the temperature data if it is available.
    TGenericData *data = dataContainer.GetEventData<TGenericData>("TEMP");
    if(data){;
        // Save the unix timestamp and SCM temperatures
        for(int i = 0; i < 8; i++)
            SCMTemps[i] =  data->GetFloat()[i];
        return;
    }

    // Save all the SCM voltage data
    TGenericData *data2 = dataContainer.GetEventData<TGenericData>("CH\0\0");
    if(!data2) return;

    // Save the unix timestamp and SCM temperatures
    timestamp = dataContainer.GetMidasData().GetTimeStamp();
    //  data2->Dump();
    for(int i = 0; i < 4; i++)
        SCMVoltages[i] =  data2->GetFloat()[i];

    tSCM->Fill();
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
