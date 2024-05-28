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

        fSourceEpics = new SourceEpics(odb);
        fBeamlineEpics = new BeamlineEpics(odb);
        fUCN2Epics = new UCN2Epics(odb);
        fUCN2EpicsTemperature = new UCN2EpicsTemperature(odb);
        fUCN2EpicsPressures = new UCN2EpicsPressures(odb);
        fUCN2EpicsOthers = new UCN2EpicsOthers(odb);
        fUCN2EpicsPhase2B = new UCN2EpicsPhase2B(odb);
        fscPico = new scPico(odb);

    }else{
        fSourceEpics = 0;
        fBeamlineEpics = 0;
        fUCN2Epics = 0;
        fUCN2EpicsTemperature = 0;
        fUCN2EpicsPressures = 0;
        fUCN2EpicsOthers = 0;
        fUCN2EpicsPhase2B = 0;
        fscPico = 0;
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

    if (fSourceEpics)           fSourceEpics->FillTree(dataContainer);
    if (fBeamlineEpics)         fBeamlineEpics->FillTree(dataContainer);
    if (fUCN2Epics)             fUCN2Epics->FillTree(dataContainer);
    if (fUCN2EpicsTemperature)  fUCN2EpicsTemperature->FillTree(dataContainer);
    if (fUCN2EpicsPressures)    fUCN2EpicsPressures->FillTree(dataContainer);
    if (fUCN2EpicsOthers)       fUCN2EpicsOthers->FillTree(dataContainer);
    if (fUCN2EpicsPhase2B)      fUCN2EpicsPhase2B->FillTree(dataContainer);
    if (fscPico)                fscPico->FillTree(dataContainer);

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