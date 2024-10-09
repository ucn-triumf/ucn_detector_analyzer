#include "TCycleParamTree.hxx"

TCycleParamTree::TCycleParamTree(MVOdb* odb){

    // for misc string operations
    std::ostringstream brname;

    // get scalar values
    odb->RI((dirname+"Settings/numberPeriodsInCycle").c_str(), &NPeriods);
    odb->RI((dirname+"Settings/numCyclesInSuper").c_str(), &NCycles);
    odb->RI((dirname+"Settings/enable").c_str(), &enable);
    odb->RI((dirname+"Settings/numberSuperCycles").c_str(), &nsuper);
    odb->RI((dirname+"Settings/infiniteCycles").c_str(), &infcycles);

    // cycle index
    period_idx = 0;

    // setup array values
    duration.resize(MaxCycles);
    valve.resize(NValves);

    // make the tree
    std::cout << "Creating Cycle Parameter tree "<< std::endl;
    tSequencer = new TTree("CycleParamTree", "CycleParamTree");

    // setup branches
    tSequencer->Branch("nPeriods", &NPeriods, "nPeriods/I");
    tSequencer->Branch("nCycles", &NCycles, "nCycles/I");
    tSequencer->Branch("enable", &enable, "enable/I");
    tSequencer->Branch("nSuperCyc", &nsuper, "nSuperCyc/I");
    tSequencer->Branch("infCyclesEnable", &infcycles, "infCyclesEnable/I");
    tSequencer->Branch("periodNumber", &period_idx, "periodNumber/I");

    for (int i = 0; i<NCycles; i++){
        brname << "periodDurationInCycle" << i;
        tSequencer->Branch(brname.str().c_str(), &duration[i], (brname.str()+"/I").c_str());
        brname.str(std::string());
    }

    for (int i = 0; i<NValves; i++){
        brname << "Valve" << i+1 << "State";
        tSequencer->Branch(brname.str().c_str(), &valve[i], (brname.str()+"/I").c_str());
        brname.str(std::string());
    }

    // Fill Tree
    std::vector<int> container;
    container.resize(MaxPeriods);

    for (period_idx = 0; period_idx<MaxPeriods; period_idx++){

        // get values from odb
        brname << dirname << "Settings/DurationTimePeriod" << period_idx;
        odb->RIA((brname.str()).c_str(), &duration);
        brname.str(std::string());

        for (valve_idx = 0; valve_idx<NValves; valve_idx++){

            brname << dirname << "Settings/Valve" << valve_idx << "State";
            odb->RIA(brname.str().c_str(), &container);
            valve[valve_idx] = container[period_idx];
        }

        tSequencer->Fill();
    }
};

// void TCycleParamTree::FillTree(TDataContainer& dataContainer){

//     // Use the sequence bank to see when a new run starts:
//     TGenericData *data = dataContainer.GetEventData<TGenericData>("NSEQ");
//     if(!data) return;

//     // Save variables
//     timestamp = dataContainer.GetMidasData().GetTimeStamp();
//     for (long unsigned int i=0; i<values.size(); i++){
//         values[i] = data->GetDouble()[i];
//     }
//     tSequencer->Fill();
// };