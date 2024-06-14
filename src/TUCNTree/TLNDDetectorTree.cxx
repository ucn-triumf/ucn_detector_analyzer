#include "TLNDDetectorTree.hxx"

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