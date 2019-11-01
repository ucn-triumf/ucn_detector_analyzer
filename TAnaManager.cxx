#include "TAnaManager.hxx"
#include "TV1720RawData.h"
#include "TV792NData.hxx"
#include "TStyle.h"
#include "TFitResult.h"


TAnaManager::TAnaManager(bool isOffline, bool saveTree){

  fHe3CountsInSequence = new THe3CountsInSequence();
  fHe3CountsInSequence->DisableAutoUpdate();
  
  fHe3Detector = new THe3Detector(isOffline,true,saveTree);
  fHe3Detector2 = new THe3Detector(isOffline,false,saveTree);
  fLi6Detector = new TLi6Detector(isOffline,saveTree);

  if(saveTree){
    std::cout << "Creating EPICS tree" << std::endl;
    fLNDDetectorTree = new TLNDDetectorTree();
    fSCMTree = new TSCMTree();
    fSequencerTree = new TSequencerTree();
    fSourceEpicsTree = new TUCNSourceEpicsTree();
    fBeamlineEpicsTree = new TUCNBeamlineEpicsTree();
  }else{
    fSourceEpicsTree = 0;
    fBeamlineEpicsTree = 0;
    fLNDDetectorTree = 0;
    fSCMTree = 0;
    fSequencerTree = 0;
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
  fTransmissionDuringCountingGraph = new TGraphErrors();
  fTransmissionDuringCountingGraph->SetTitle("Transmission normalized during counting;Cycle no.;Li6-He3 ratio");
  fTransmissionDuringIrradiationGraph = new TGraphErrors();
  fTransmissionDuringIrradiationGraph->SetTitle("Transmission normalized during irradiation;Cycle no.;Li6-He3 ratio");
  fTransmissionWithPreStorage = new TGraphErrors();
  fTransmissionWithPreStorage->SetTitle("Transmission normalized during prestorage;Cycle no.;Background-corrected Li6-He3 ratio");
  fHe3DuringIrradiationGraph = new TGraphErrors();
  fHe3DuringIrradiationGraph->SetTitle("He3 monitor counts during irradiation;Cycle no.;He3 counts");
  fHe3DuringStorageGraph = new TGraphErrors();
  fHe3DuringStorageGraph->SetTitle("He3 monitor counts during storage;Cycle no.;He3 counts");
  fHe3AfterIrradiationGraph = new TGraphErrors();
  fHe3AfterIrradiationGraph->SetTitle("He3 monitor counts after irradiation;Cycle no.;He3 counts");
  fLi6StorageBackgroundGraph = new TGraphErrors();
  fLi6StorageBackgroundGraph->SetTitle("Li6 background during storage period;Cycle no.;Li6 background rate (s^{-1})");
  fHe3StorageBackgroundGraph = new TGraphErrors();
  fHe3StorageBackgroundGraph->SetTitle("He3 background during storage period;Cycle no.;He3 background rate (s^{-1})");
  fLi6StorageGraph = new TGraphErrors();
  fLi6StorageGraph->SetTitle("Unnormalized storage with Li6 detector;Storage time (s);Background-corrected Li6 counts");
  fHe3StorageGraph = new TGraphErrors();
  fHe3StorageGraph->SetTitle("Unnormalized storage with He3 detector;Storage time (s);Background-corrected He3 counts");
  fStorageWithMonitorDuringIrradiation = new TGraphErrors();
  fStorageWithMonitorDuringIrradiation->SetTitle("Storage with normalization during irradiation;Storage time (s);Background-corrected Li6-He3 ratio");
  fStorageWithMonitorAfterIrradiation = new TGraphErrors();
  fStorageWithMonitorAfterIrradiation->SetTitle("Storage with normalization after irradiation;Storage time(s);Background-corrected Li6-He3 ratio");
  std::cout << "Finished TAnaManager constructor" << std::endl;
};

void TAnaManager::BeginRun(int transition,int run,int time) {
    if(fLi6Detector) fLi6Detector->BeginRun(transition,run,time);
    if(fUCNChronobox) fUCNChronobox->BeginRun(transition,run,time);

    // reset histograms
    fTransmissionDuringCountingGraph->Set(0);
    fTransmissionDuringIrradiationGraph->Set(0);
    fTransmissionWithPreStorage->Set(0);
    fHe3DuringIrradiationGraph->Set(0);
    fHe3DuringStorageGraph->Set(0);
    fHe3AfterIrradiationGraph->Set(0);
    fLi6StorageBackgroundGraph->Set(0);
    fHe3StorageBackgroundGraph->Set(0);
    fLi6StorageGraph->Set(0);
    fHe3StorageGraph->Set(0);
    fStorageWithMonitorDuringIrradiation->Set(0);
    fStorageWithMonitorAfterIrradiation->Set(0);
}

      
bool insequence = 0;

int TAnaManager::ProcessMidasEvent(TDataContainer& dataContainer){
  
  //if(fV1720Waveform && 0)  fV1720Waveform->UpdateHistograms(dataContainer);
  
  fHe3Detector->ProcessMidasEvent(dataContainer);
  fHe3Detector2->ProcessMidasEvent(dataContainer);
  fLi6Detector->ProcessMidasEvent(dataContainer);
  if(fSourceEpicsTree){
    fLNDDetectorTree->FillTree(dataContainer);
    fSCMTree->FillTree(dataContainer);
    fSequencerTree->FillTree(dataContainer);
    fSourceEpicsTree->FillTree(dataContainer);
    fBeamlineEpicsTree->FillTree(dataContainer);   
  }
  fUCNChronobox->ProcessMidasEvent(dataContainer);
  fV1750WaveformDisplay->UpdateHistograms(dataContainer);
  fV1725_PH->UpdateHistograms(dataContainer);
  fV1725_QL->UpdateHistograms(dataContainer);
  fV1725PSDQL->UpdateHistograms(dataContainer);
  
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
  if ((fHe3Detector->CycleStarted() or fLi6Detector->CycleStarted()) and fLi6Detector->GetHitsPerCyclePerPeriod()[0].size() > 0
  and fLi6Detector->GetHitsPerCyclePerPeriod()[0].size() == fHe3Detector->GetHitsPerCyclePerPeriod()[0].size()){
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
      int N = fTransmissionDuringCountingGraph->GetN();

      /****** TRANSMISSION ******/
      fTransmissionDuringCountingGraph->Set(N + 1);
      fTransmissionDuringIrradiationGraph->Set(N + 1);
      fLi6StorageBackgroundGraph->Set(N + 1);

      // transmission = Li6 counts / He3 counts during counting
      double transmission = Li6Hits[1]/He3Hits[1];
      double transmissionerr = std::sqrt(1./Li6Hits[1] + 1./He3Hits[1]) * transmission;
      fTransmissionDuringCountingGraph->SetPoint(N, N, transmission);
      fTransmissionDuringCountingGraph->SetPointError(N, 0., transmissionerr);
      fTransmissionDuringCountingGraph->Fit("pol0", "Q");

      // transmission = Li6 counts / He3 counts during irradiation
      transmission = Li6Hits[1]/He3Hits[0];
      transmissionerr = std::sqrt(1./Li6Hits[1] + 1./He3Hits[0]) * transmission;
      fTransmissionDuringIrradiationGraph->SetPoint(N, N, transmission);
      fTransmissionDuringIrradiationGraph->SetPointError(N, 0., transmissionerr);
      fTransmissionDuringIrradiationGraph->Fit("pol0", "Q");


      /******** MONITOR COUNTS ********/
      fHe3DuringIrradiationGraph->Set(N + 1);
      fHe3DuringIrradiationGraph->SetPoint(N, N, He3Hits[0]);
      fHe3DuringIrradiationGraph->SetPointError(N, 0., std::sqrt(He3Hits[0]));
      fHe3DuringIrradiationGraph->Fit("pol0","Q");
      fHe3DuringStorageGraph->Set(N + 1);
      fHe3DuringStorageGraph->SetPoint(N, N, He3Hits[1]);
      fHe3DuringStorageGraph->SetPointError(N, 0., std::sqrt(He3Hits[1]));
      fHe3DuringStorageGraph->Fit("pol0","Q");
      fHe3AfterIrradiationGraph->Set(N + 1);
      fHe3AfterIrradiationGraph->SetPoint(N, N, fHe3Detector->GetMonitorCountsAfterIrradiationPerCycle().back().second);
      fHe3AfterIrradiationGraph->SetPointError(N, 0., std::sqrt(fHe3Detector->GetMonitorCountsAfterIrradiationPerCycle().back().second));
      fHe3AfterIrradiationGraph->Fit("pol0","Q");


      /******** BACKGROUND DURING (PRE-)STORAGE PERIOD ********/
      // calculate background rate during storage period in a storage measurement (exclude cycles with 0s storage time)
      if (storagetime > 0) {
          fLi6StorageBackgroundGraph->SetPoint(N, N, Li6Hits[1] / storagetime);
          fLi6StorageBackgroundGraph->SetPointError(N, 0., std::sqrt(Li6Hits[1]) / storagetime);
          fHe3StorageBackgroundGraph->SetPoint(N, N, He3Hits[1] / storagetime);
          fHe3StorageBackgroundGraph->SetPointError(N, 0., std::sqrt(He3Hits[1]) / storagetime);
      }
      else{
          fLi6StorageBackgroundGraph->SetPoint(N,N,0.);
          fHe3StorageBackgroundGraph->SetPoint(N,N,0.);
      }

      // get background rate averaged over all cycles
      auto Li6result = fLi6StorageBackgroundGraph->Fit("pol0", "QS");
      double Li6backgroundrate = 0., Li6backgroundrateerr = 0.;
      if (Li6result == 0){
          Li6backgroundrate = Li6result->Parameter(0);
          Li6backgroundrateerr = Li6result->ParError(0);
      }
      auto He3result = fHe3StorageBackgroundGraph->Fit("pol0", "QS");
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
      fTransmissionWithPreStorage->Set(N + 1);
      fTransmissionWithPreStorage->SetPoint(N, N, transmission);
      fTransmissionWithPreStorage->SetPointError(N, 0., transmissionerr);
      fTransmissionWithPreStorage->Fit("pol0", "Q");


      /******** STORAGE WITH LI6 ONLY *********/
      TF1 fexpo("fexpo", "[0]*exp(-x/[1])");
      fexpo.SetParName(0, "N_{0}");
      fexpo.SetParName(1, "#tau");

       // subtract background from Li6/He3 counts, plot vs. storage time
      fLi6StorageGraph->Set(N + 1);
      fLi6StorageGraph->SetPoint(N, storagetime, Li6corrected);
      fLi6StorageGraph->SetPointError(N, 0., Li6correctederr);
      fexpo.SetParameters(10000, 10);
      fLi6StorageGraph->Fit(&fexpo,"Q");


      /******** STORAGE WITH HE3 ONLY *********/
      fHe3StorageGraph->Set(N + 1);
      fHe3StorageGraph->SetPoint(N, storagetime, He3Hits[2] - He3backgroundrate*countingtime);
      fHe3StorageGraph->SetPointError(N, 0., std::sqrt(He3Hits[2] + std::pow(He3backgroundrateerr*countingtime, 2)));
      fexpo.SetParameters(1000, 10);
      fHe3StorageGraph->Fit(&fexpo,"Q");


      /******* STORAGE WITH MONITOR ********/
      // subtract background from Li6 counts, normalize to He3 counts during irradiation, plot vs. storage time
      fStorageWithMonitorDuringIrradiation->Set(N + 1);
      fStorageWithMonitorDuringIrradiation->SetPoint(N, storagetime, Li6corrected/He3Hits[0]);
      fStorageWithMonitorDuringIrradiation->SetPointError(N, 0., std::sqrt(std::pow(Li6correctederr/Li6corrected, 2) + 1./He3Hits[0])*Li6corrected/He3Hits[0]);
      fexpo.SetParameters(10, 10);
      fStorageWithMonitorDuringIrradiation->Fit(&fexpo, "Q");

      double monitorcounts = fHe3Detector->GetMonitorCountsAfterIrradiationPerCycle().back().second;
      // subtract background from Li6 counts, normalize to He3 counts after irradiation, plot vs. storage time
      fStorageWithMonitorAfterIrradiation->Set(N + 1);
      fStorageWithMonitorAfterIrradiation->SetPoint(N, storagetime, Li6corrected / monitorcounts);
      fStorageWithMonitorAfterIrradiation->SetPointError(N, 0., std::sqrt(std::pow(Li6correctederr/Li6corrected, 2) + 1./monitorcounts) * Li6corrected / monitorcounts);
      fexpo.SetParameters(10, 10);
      fStorageWithMonitorAfterIrradiation->Fit(&fexpo, "Q");
  }



  return 1;
}





THe3CountsInSequence* TAnaManager::GetHe3CountsHistograms(){return fHe3CountsInSequence;}

