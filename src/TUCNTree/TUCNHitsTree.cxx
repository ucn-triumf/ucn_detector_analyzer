#include "TUCNHitsTree.hxx"

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
