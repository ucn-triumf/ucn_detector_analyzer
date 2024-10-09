#include "TCycleParamTree.hxx"

TCycleParamTree::TCycleParamTree(MVOdb* odb){

    // for misc string operations
    std::ostringstream brname;

    // get scalar values
    odb->RI((dirname+"Settings/numberPeriodsInCycle").c_str(), &NPeriods);
    odb->RI((dirname+"Settings/numCyclesInSuper").c_str(), &NCycles);
    odb->RB((dirname+"Settings/enable").c_str(), &enable);
    odb->RI((dirname+"Settings/numberSuperCycles").c_str(), &nsuper);
    odb->RB((dirname+"Settings/infiniteCycles").c_str(), &infcycles);

    // cycle index
    period_idx = 0;

    // setup array values
    duration = std::vector<double>();
    valve = std::vector<int>();

    duration.resize(MaxCycles);
    valve.resize(NValves);

    // make the tree
    std::cout << "Creating Cycle Parameter tree "<< std::endl;
    tSequencer = new TTree("CycleParamTree", "CycleParamTree");

    // setup branches
    tSequencer->Branch("nPeriods", &NPeriods, "nPeriods/I");
    tSequencer->Branch("nCycles", &NCycles, "nCycles/I");
    tSequencer->Branch("enable", &enable);
    tSequencer->Branch("nSuperCyc", &nsuper, "nSuperCyc/I");
    tSequencer->Branch("infCyclesEnable", &infcycles);
    tSequencer->Branch("periodNumber", &period_idx, "periodNumber/I");

    for (int i = 0; i<MaxCycles; i++){
        brname << "periodDurationInCycle" << i;
        tSequencer->Branch(brname.str().c_str(), &duration[i], (brname.str()+"/D").c_str());
        brname.str(std::string()); // reset the string stream
    }

    for (int i = 0; i<NValves; i++){
        brname << "Valve" << i+1 << "State";
        tSequencer->Branch(brname.str().c_str(), &valve[i], (brname.str()+"/I").c_str());
        brname.str(std::string()); // reset the string stream
    }

    // Fill Tree
    for (period_idx = 0; period_idx<MaxPeriods; period_idx++){

        // get values from odb
        brname << dirname << "Settings/DurationTimePeriod" << period_idx;
        odb->RDA((brname.str()).c_str(), &duration);
        brname.str(std::string()); // reset the string stream

        std::cout << duration[0] << std::endl;

        for (valve_idx = 0; valve_idx<NValves; valve_idx++){

            brname << dirname << "Settings/Valve" << valve_idx+1 << "State";
            odb->RIAI(brname.str().c_str(), period_idx, &(valve[valve_idx]));
            brname.str(std::string()); // reset the string stream
        }
        tSequencer->Fill();
    }
};