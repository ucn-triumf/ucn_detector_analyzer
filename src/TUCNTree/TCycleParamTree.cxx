#include "TCycleParamTree.hxx"

TCycleParamTree::TCycleParamTree(MVOdb* odb){

    // get NCycles and NPeriods
    odb->RI("/Equipment/UCNSequencer2018/Settings/numberPeriodsInCycle", &NPeriods);
    odb->RI("/Equipment/UCNSequencer2018/Settings/numCyclesInSuper", &NCycles);

    // update branch names according to logic in sequence_control_multi_valve.cxx
    std::ostringstream brname;
    for(int i = 0; i < NPeriods; i++){
        for(int j = 0; j < NCycles; j++){
            brname << "duration_c" << j << "_p" << i;
            branch_names.push_back(brname.str());
            brname.str(std::string());
        }
    }
    for(int i = 0; i < NValves; i++){
        brname << "valve" << i+1 << "state";
        branch_names.push_back(brname.str());
        brname.str(std::string());
    }

    // setup values for holding data
    values.resize(branch_names.size());

    // make the tree
    std::cout << "Creating Cycle Parameter tree "<< std::endl;
    tSequencer = new TTree("CycleParamTree", "CycleParamTree");

    // setup branches
    tSequencer->Branch("timestamp", &timestamp, "timestamp/I" );
    for (long unsigned int i=0; i<branch_names.size(); i++){
        tSequencer->Branch(branch_names[i].c_str(), &values[i], (branch_names[i]+"/I").c_str());
    }
};

void TCycleParamTree::FillTree(TDataContainer& dataContainer){

    // Use the sequence bank to see when a new run starts:
    TGenericData *data = dataContainer.GetEventData<TGenericData>("NSEQ");
    if(!data) return;

    // Save variables
    timestamp = dataContainer.GetMidasData().GetTimeStamp();
    for (long unsigned int i=0; i<values.size(); i++){
        values[i] = data->GetDouble()[i];
    }
    tSequencer->Fill();
};