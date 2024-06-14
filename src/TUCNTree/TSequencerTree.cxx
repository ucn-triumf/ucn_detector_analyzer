#include "TSequencerTree.hxx"

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