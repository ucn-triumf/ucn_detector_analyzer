#include "TAnaManager.hxx"
#include "TV1720RawData.h"
#include "TV792NData.hxx"

// Constructor
TAnaManager::TAnaManager(bool isOffline, MVOdb* odb, bool saveTree){

    fHe3CountsInSequence = new THe3CountsInSequence();
    fHe3CountsInSequence->DisableAutoUpdate();

    fHe3Detector = new THe3Detector(isOffline,saveTree);
    fLi6Detector = new TLi6Detector(isOffline,saveTree);

    if(saveTree){
        std::cout << "Creating EPICS trees... ";
        fLNDDetectorTree = new TLNDDetectorTree(odb);
        fBeamlineEpicsTree = new TUCNBeamlineEpicsTree(odb);
        fSourceEpicsTree = new TUCNSourceEpicsTree(odb);

    }else{
        fSourceEpicsTree = 0;
        fBeamlineEpicsTree = 0;
        fLNDDetectorTree = 0;
    }

    std::cout << "done" << std::endl;

    //fV785Charge = new TUCNDetectorCharge(false)
    fV785Charge = new TV792Histograms();
    fV785Charge->DisableAutoUpdate();

};

bool insequence = 0;

// Process each midas event: get data and fill trees
int TAnaManager::ProcessMidasEvent(TDataContainer& dataContainer){

    //if(fV1720Waveform && 0)  fV1720Waveform->UpdateHistograms(dataContainer);

    fHe3Detector->ProcessMidasEvent(dataContainer);
    fLi6Detector->ProcessMidasEvent(dataContainer);
    if(fSourceEpicsTree){
        fLNDDetectorTree->FillTree(dataContainer);
        fSourceEpicsTree->FillTree(dataContainer);
        fBeamlineEpicsTree->FillTree(dataContainer);
    }

    TV792NData *data = dataContainer.GetEventData<TV792NData>("ADC0");
    if(!data)
        return 1;

    /// Get the Vector of ADC Measurements.
    std::vector<VADCNMeasurement> measurements = data->GetMeasurements();
    for(unsigned int i = 0; i < measurements.size(); i++){ // loop over measurements

        int channel = measurements[i].GetChannel();
        int charge = measurements[i].GetMeasurement();

        if(channel >= 0 && channel < 16)
        fV785Charge->GetHistogram(channel)->Fill(charge);
    }

    return 1;
}

THe3CountsInSequence* TAnaManager::GetHe3CountsHistograms(){return fHe3CountsInSequence;}