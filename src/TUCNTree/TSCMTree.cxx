#include "TSCMTree.hxx"

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

    for(int i = 0; i < 4; i++)
        SCMVoltages[i] =  data2->GetFloat()[i];

    tSCM->Fill();
}