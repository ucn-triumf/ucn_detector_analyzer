#include "TAnaManager.hxx"
#include "TV1720RawData.h"
#include "TV792NData.hxx"
#include "TStyle.h"
#include "TFitResult.h"

// Constructor
TAnaManager::TAnaManager(bool isOffline, MVOdb* odb, bool saveTree){

    fHe3CountsInSequence = new THe3CountsInSequence();
    fHe3CountsInSequence->DisableAutoUpdate();

    fHe3Detector = new THe3Detector(isOffline, true, saveTree);
    fHe3Detector2 = new THe3Detector(isOffline, false, saveTree);
    fLi6Detector = new TLi6Detector(isOffline, saveTree);

    // make trees for saving outputs
    if(saveTree){
        std::cout << "Creating EPICS trees" << std::endl;
        fLNDDetectorTree = new TLNDDetectorTree();
        fSCMTree = new TSCMTree();
        fSequencerTree = new TSequencerTree();
        fEPICSTrees = std::vector<TUCNEpicsTree*>({
            // new TUCNEpicsTree(odb,
            //                 "EPSR",         // bank name
            //                 "SourceEpics",  // tree name
            //                 "/Equipment/SourceEpics/Settings/Names"), // odb path to branch names
            new TUCNEpicsTree(odb,
                            "EPBL",
                            "BeamlineEpics",
                            "/Equipment/BeamlineEpics/Settings/Names"),
            new TUCNEpicsTree(odb,
                            "EPU2",
                            "UCN2Epics",
                            "/Equipment/UCN2Epics/Settings/Names"),
            new TUCNEpicsTree(odb,
                            "EP2T",
                            "UCN2EpicsTemperature",
                            "/Equipment/UCN2EpicsTemperature/Settings/Names"),
            new TUCNEpicsTree(odb,
                            "EP2P",
                            "UCN2EpicsPressures",
                            "/Equipment/UCN2EpicsPressures/Settings/Names"),
            new TUCNEpicsTree(odb,
                            "EP2O",
                            "UCN2EpicsOthers",
                            "/Equipment/UCN2EpicsOthers/Settings/Names"),
            new TUCNEpicsTree(odb,
                            "EPH2",
                            "UCN2EpicsPhase2B",
                            "/Equipment/UCN2EpicsPhase2B/Settings/Names"),
            new TUCNEpicsTree(odb,
                            "EP3A",
                            "UCN2EpicsPhase3",
                            "/Equipment/UCN2EpicsPhase3/Settings/Names"),
            new TUCNEpicsTree(odb,
                            "EPFA",
                            "UCN2FastEpics",
                            "/Equipment/UCN2fastEpics/Settings/Names"),
        });

    }else{
        fLNDDetectorTree = 0;
        fSCMTree = 0;
        fSequencerTree = 0;
        fEPICSTrees = std::vector<TUCNEpicsTree*>();
    }

    std::cout << "Making V1725 plots " << std::endl;

    // event histograms for runtime window
    fV1750WaveformDisplay = new TV1725WaveformDisplay();
    fV1750WaveformDisplay->DisableAutoUpdate();
    fV1725PSDQL = new TV1725PSDQL();
    fV1725PSDQL->DisableAutoUpdate();
    fV1725_PH = new TV1725_PH();
    fV1725_PH->DisableAutoUpdate();
    fV1725_QL = new TV1725_QL();
    fV1725_QL->DisableAutoUpdate();

    fV785Charge = new TV792Histograms();
    fV785Charge->DisableAutoUpdate();

    fUCNChronobox = new TUCNChronobox();

    gStyle->SetOptFit(111);
    gStyle->SetOptStat(1001111);

    // initialize graphs for displaying analysis
    for (auto itr = GRAPH_NAME_TITLE.begin(); itr != GRAPH_NAME_TITLE.end(); itr++){
        graphs[itr->first] = new TGraphErrors();
        graphs[itr->first]->SetTitle(itr->second);
    }
    std::cout << "Finished TAnaManager constructor" << std::endl;
};

void TAnaManager::BeginRun(int transition, int run, int time) {
    if(fLi6Detector) fLi6Detector->BeginRun(transition, run, time);
    if(fUCNChronobox) fUCNChronobox->BeginRun(transition, run, time);

    // reset graphs
    for (auto itr=graphs.begin(); itr != graphs.end(); itr++){
        itr->second->Set(0);
    }
}

bool insequence = 0;

// Process each midas event: get data and fill trees
int TAnaManager::ProcessMidasEvent(TDataContainer& dataContainer){

    fHe3Detector->ProcessMidasEvent(dataContainer);
    fHe3Detector2->ProcessMidasEvent(dataContainer);
    fLi6Detector->ProcessMidasEvent(dataContainer);
    fUCNChronobox->ProcessMidasEvent(dataContainer);
    fV1750WaveformDisplay->UpdateHistograms(dataContainer);
    fV1725_PH->UpdateHistograms(dataContainer);
    fV1725_QL->UpdateHistograms(dataContainer);
    fV1725PSDQL->UpdateHistograms(dataContainer);

    for (unsigned int i=0; i<fEPICSTrees.size(); i++)
        fEPICSTrees[i]->FillTree(dataContainer);
    if (fSequencerTree)         fSequencerTree->FillTree(dataContainer);
    if (fSCMTree)               fSCMTree->FillTree(dataContainer);
    if (fLNDDetectorTree)       fLNDDetectorTree->FillTree(dataContainer);

    TV792NData *data = dataContainer.GetEventData<TV792NData>("ADC0");
    if(data) {
        /// Get the Vector of ADC Measurements.
        std::vector<VADCNMeasurement> measurements = data->GetMeasurements();
        for (unsigned int i = 0; i < measurements.size(); i++) { // loop over measurements
            int channel = measurements[i].GetChannel();
            int charge = measurements[i].GetMeasurement();

            if (channel >= 0 && channel < 16)
                fV785Charge->GetHistogram(channel)->Fill(charge);
        }
    }

    // update histograms when a new cycle is started
    if ((fHe3Detector->CycleStarted() or fLi6Detector->CycleStarted()) and
        fLi6Detector->GetHitsPerCyclePerPeriod()[0].size() > 0 and
        fLi6Detector->GetHitsPerCyclePerPeriod()[0].size() == fHe3Detector->GetHitsPerCyclePerPeriod()[0].size()){

        std::vector<double> Li6Hits, He3Hits; // gather hits in each period of last cycle
        for (const auto &hits: fLi6Detector->GetHitsPerCyclePerPeriod()){
            Li6Hits.push_back(hits.back().second);
        }
        for (const auto &hits: fHe3Detector->GetHitsPerCyclePerPeriod()){
            He3Hits.push_back(hits.back().second);
        }

        // if new cycle has index 0, use timing from last cycle in supercycle, else decrement index by 1
        int cycle_index = fLi6Detector->CycleParameters.CycleIndex();
        int last_cycle_index = cycle_index > 0 ? cycle_index - 1 : fLi6Detector->CycleParameters.GetNumberCyclesInSuper() - 1;
        double storagetime = fLi6Detector->CycleParameters.GetTimeForPeriod(1, last_cycle_index);
        double countingtime = fLi6Detector->CycleParameters.GetTimeForPeriod(2, last_cycle_index);

        int N = graphs["TransmissionDuringCounting"]->GetN();

        /****** TRANSMISSION ******/
        graphs["TransmissionDuringCounting"]->Set(N + 1);
        graphs["TransmissionDuringIrradiation"]->Set(N + 1);
        graphs["Li6StorageBackground"]->Set(N + 1);

        // transmission = Li6 counts / He3 counts during counting
        double transmission = Li6Hits[1]/He3Hits[1];
        double transmissionerr = std::sqrt(1./Li6Hits[1] + 1./He3Hits[1]) * transmission;

        graphs["TransmissionDuringCounting"]->SetPoint(N, N, transmission);
        graphs["TransmissionDuringCounting"]->SetPointError(N, 0., transmissionerr);
        graphs["TransmissionDuringCounting"]->Fit("pol0", "Q");

        // transmission = Li6 counts / He3 counts during irradiation
        transmission = Li6Hits[1]/He3Hits[0];
        transmissionerr = std::sqrt(1./Li6Hits[1] + 1./He3Hits[0]) * transmission;
        graphs["TransmissionDuringIrradiation"]->SetPoint(N, N, transmission);
        graphs["TransmissionDuringIrradiation"]->SetPointError(N, 0., transmissionerr);
        graphs["TransmissionDuringIrradiation"]->Fit("pol0", "Q");

        /******** MONITOR COUNTS ********/
        graphs["He3DuringIrradiation"]->Set(N + 1);
        graphs["He3DuringIrradiation"]->SetPoint(N, N, He3Hits[0]);
        graphs["He3DuringIrradiation"]->SetPointError(N, 0., std::sqrt(He3Hits[0]));
        graphs["He3DuringIrradiation"]->Fit("pol0", "Q");
        graphs["He3DuringStorage"]->Set(N + 1);
        graphs["He3DuringStorage"]->SetPoint(N, N, He3Hits[1]);
        graphs["He3DuringStorage"]->SetPointError(N, 0., std::sqrt(He3Hits[1]));
        graphs["He3DuringStorage"]->Fit("pol0", "Q");
        graphs["He3AfterIrradiation"]->Set(N + 1);
        graphs["He3AfterIrradiation"]->SetPoint(N, N, fHe3Detector->GetMonitorCountsAfterIrradiationPerCycle().back().second);
        graphs["He3AfterIrradiation"]->SetPointError(N, 0., std::sqrt(fHe3Detector->GetMonitorCountsAfterIrradiationPerCycle().back().second));
        graphs["He3AfterIrradiation"]->Fit("pol0", "Q");

        /******** BACKGROUND DURING (PRE-)STORAGE PERIOD ********/
        // calculate background rate during storage period in a storage measurement (exclude cycles with 0s storage time)
        if (storagetime > 0) {
            graphs["Li6StorageBackground"]->SetPoint(N, N, Li6Hits[1] / storagetime);
            graphs["Li6StorageBackground"]->SetPointError(N, 0., std::sqrt(Li6Hits[1]) / storagetime);
            graphs["He3StorageBackground"]->SetPoint(N, N, He3Hits[1] / storagetime);
            graphs["He3StorageBackground"]->SetPointError(N, 0., std::sqrt(He3Hits[1]) / storagetime);
        }
        else{
            graphs["Li6StorageBackground"]->SetPoint(N, N, 0.);
            graphs["He3StorageBackground"]->SetPoint(N, N, 0.);
        }

        // get background rate averaged over all cycles
        auto Li6result = graphs["Li6StorageBackground"]->Fit("pol0", "QS");
        double Li6backgroundrate = 0., Li6backgroundrateerr = 0.;
        if (Li6result == 0){
            Li6backgroundrate = Li6result->Parameter(0);
            Li6backgroundrateerr = Li6result->ParError(0);
        }

        auto He3result = graphs["He3StorageBackground"]->Fit("pol0", "QS");
        double He3backgroundrate = 0., He3backgroundrateerr = 0.;
        if (He3result == 0){
            He3backgroundrate = He3result->Parameter(0);
            He3backgroundrateerr = He3result->ParError(0);
        }

        double Li6corrected = Li6Hits[2] - Li6backgroundrate*countingtime;
        double Li6correctederr = std::sqrt(Li6Hits[2] + std::pow(Li6backgroundrateerr*countingtime, 2));

        /******** TRANSMISSION WITH PRE-STORAGE *******/
        transmission = Li6corrected/He3Hits[1];
        transmissionerr = std::sqrt(std::pow(Li6correctederr / Li6corrected, 2) + 1. / He3Hits[1]) * transmission;
        graphs["TransmissionWithPreStorage"]->Set(N + 1);
        graphs["TransmissionWithPreStorage"]->SetPoint(N, N, transmission);
        graphs["TransmissionWithPreStorage"]->SetPointError(N, 0., transmissionerr);
        graphs["TransmissionWithPreStorage"]->Fit("pol0", "Q");

        /******** STORAGE WITH LI6 ONLY *********/
        TF1 fexpo("fexpo", "[0]*exp(-x/[1])");
        fexpo.SetParName(0, "N_{0}");
        fexpo.SetParName(1, "#tau");

        // subtract background from Li6/He3 counts, plot vs. storage time
        graphs["Li6Storage"]->Set(N + 1);
        graphs["Li6Storage"]->SetPoint(N, storagetime, Li6corrected);
        graphs["Li6Storage"]->SetPointError(N, 0., Li6correctederr);
        fexpo.SetParameters(10000, 10);
        graphs["Li6Storage"]->Fit(&fexpo, "Q");

        /******** STORAGE WITH HE3 ONLY *********/
        graphs["He3Storage"]->Set(N + 1);
        graphs["He3Storage"]->SetPoint(N, storagetime, He3Hits[2] - He3backgroundrate*countingtime);
        graphs["He3Storage"]->SetPointError(N, 0., std::sqrt(He3Hits[2] + std::pow(He3backgroundrateerr*countingtime, 2)));
        fexpo.SetParameters(1000, 10);
        graphs["He3Storage"]->Fit(&fexpo, "Q");

        /******* STORAGE WITH MONITOR ********/
        // subtract background from Li6 counts, normalize to He3 counts during irradiation, plot vs. storage time
        graphs["StorageWithMonitorDuringIrradiation"]->Set(N + 1);
        graphs["StorageWithMonitorDuringIrradiation"]->SetPoint(N, storagetime, Li6corrected/He3Hits[0]);
        graphs["StorageWithMonitorDuringIrradiation"]->SetPointError(N, 0., std::sqrt(std::pow(Li6correctederr/Li6corrected, 2) + 1./He3Hits[0])*Li6corrected/He3Hits[0]);
        fexpo.SetParameters(10, 10);
        graphs["StorageWithMonitorDuringIrradiation"]->Fit(&fexpo, "Q");

        double monitorcounts = fHe3Detector->GetMonitorCountsAfterIrradiationPerCycle().back().second;
        // subtract background from Li6 counts, normalize to He3 counts after irradiation, plot vs. storage time
        graphs["StorageWithMonitorAfterIrradiation"]->Set(N + 1);
        graphs["StorageWithMonitorAfterIrradiation"]->SetPoint(N, storagetime, Li6corrected / monitorcounts);
        graphs["StorageWithMonitorAfterIrradiation"]->SetPointError(N, 0., std::sqrt(std::pow(Li6correctederr/Li6corrected, 2) + 1./monitorcounts) * Li6corrected / monitorcounts);
        fexpo.SetParameters(10, 10);
        graphs["StorageWithMonitorAfterIrradiation"]->Fit(&fexpo, "Q");
    }
    return 1;
}

THe3CountsInSequence* TAnaManager::GetHe3CountsHistograms(){return fHe3CountsInSequence;}
