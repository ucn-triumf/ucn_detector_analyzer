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


  // event histograms for runtime window
  fV1750WaveformDisplay = new TV1725WaveformDisplay();
  fV1750WaveformDisplay->DisableAutoUpdate();

  fV785Charge = new TV792Histograms();
  fV785Charge->DisableAutoUpdate();

  fUCNChronobox = new TUCNChronobox();

  gStyle->SetOptFit(111);
  gStyle->SetOptStat(1001111);
  fTransmissionDuringCountingGraph = new TGraphErrors();
  fTransmissionDuringCountingGraph->SetTitle("Transmission normalized during counting;Cycle no.;Li6-He3 ratio");
  fTransmissionDuringIrradiationGraph = new TGraphErrors();
  fTransmissionDuringIrradiationGraph->SetTitle("Transmission normalized during irradiation;Cycle no.;Li6-He3 ratio");
  fLi6StorageBackgroundGraph = new TGraphErrors();
  fLi6StorageBackgroundGraph->SetTitle("Li6 background during storage period;Cycle no.;Li6 background rate (s^{-1})");
  fHe3StorageBackgroundGraph = new TGraphErrors();
  fHe3StorageBackgroundGraph->SetTitle("He3 background during storage period;Cycle no.;He3 background rate (s^{-1})");
  fLi6StorageGraph = new TGraphErrors();
  fLi6StorageGraph->SetTitle(";Storage time (s);Background-corrected Li6 counts");
  fHe3StorageGraph = new TGraphErrors();
  fHe3StorageGraph->SetTitle(";Storage time (s);Background-corrected He3 counts");
  fStorageWithMonitorDuringIrradiation = new TGraphErrors();
  fStorageWithMonitorDuringIrradiation->SetTitle("Storage with normalization during irradiation;Storage time (s);Background-corrected Li6-He3 ratio");
  fStorageWithMonitorAfterIrradiation = new TGraphErrors();
  fStorageWithMonitorAfterIrradiation->SetTitle("storage with normalization after irradiation;Storage time(s);Background-corrected Li6-He3 ratio");
};
      
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

  const auto &Li6Hits = fLi6Detector->GetHitsPerCyclePerPeriod();
  const auto &He3Hits = fHe3Detector->GetHitsPerCyclePerPeriod();
  size_t n_cycles = Li6Hits[0].size();
  // update histograms when a new cycle is started
  if ((fHe3Detector->CycleStarted() or fLi6Detector->CycleStarted()) and n_cycles > 0 and n_cycles == He3Hits[0].size()){
      auto &CycleParameters = fLi6Detector->CycleParameters;
      fTransmissionDuringCountingGraph->Set(n_cycles);
      fTransmissionDuringIrradiationGraph->Set(n_cycles);
      fLi6StorageBackgroundGraph->Set(n_cycles);
      for (size_t i = 0; i < n_cycles; ++i) {
          // transmission = Li6 counts / He3 counts during counting
          double transmission = Li6Hits[1][i].second / He3Hits[1][i].second;
          double transmissionerr = std::sqrt(1. / Li6Hits[1][i].second + 1. / He3Hits[1][i].second) * transmission;
          fTransmissionDuringCountingGraph->SetPoint(i, i, transmission);
          fTransmissionDuringCountingGraph->SetPointError(i, 0., transmissionerr);

          // transmission = Li6 counts / He3 counts during irradiation
          transmission = Li6Hits[1][i].second / He3Hits[0][i].second;
          transmissionerr = std::sqrt(1. / Li6Hits[1][i].second + 1. / He3Hits[0][i].second) * transmission;
          fTransmissionDuringIrradiationGraph->SetPoint(i, i, transmission);
          fTransmissionDuringIrradiationGraph->SetPointError(i, 0., transmissionerr);

          // calculate background rate during storage period in a storage measurement (exclude cycles with 0s storage time)
          if (CycleParameters.GetTimeForPeriod(1, i) > 0) {
              fLi6StorageBackgroundGraph->SetPoint(i, i, Li6Hits[1][i].second / CycleParameters.GetTimeForPeriod(1, i));
              fLi6StorageBackgroundGraph->SetPointError(i, 0., std::sqrt(Li6Hits[1][i].second) / CycleParameters.GetTimeForPeriod(1, i));
              fHe3StorageBackgroundGraph->SetPoint(i, i, He3Hits[1][i].second / CycleParameters.GetTimeForPeriod(1, i));
              fHe3StorageBackgroundGraph->SetPointError(i, 0., std::sqrt(He3Hits[1][i].second) / CycleParameters.GetTimeForPeriod(1, i));
          }

      }
      fTransmissionDuringCountingGraph->Fit("pol0", "Q");
      fTransmissionDuringIrradiationGraph->Fit("pol0", "Q");

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

      fLi6StorageGraph->Set(n_cycles);
      fHe3StorageGraph->Set(n_cycles);
      fStorageWithMonitorDuringIrradiation->Set(n_cycles);
      TF1 fexpo("fexpo", "[0]*exp(-x/[1])");
      fexpo.SetParName(0, "N_{0}");
      fexpo.SetParName(1, "#tau");

      for (size_t i = 0; i < n_cycles; ++i){
          double storagetime = CycleParameters.GetTimeForPeriod(1, i);
          double countingtime = CycleParameters.GetTimeForPeriod(2, i);

          // subtract background from Li6/He3 counts, plot vs. storage time
          double Li6corrected = Li6Hits[2][i].second - Li6backgroundrate*countingtime;
          double Li6correctederr = std::sqrt(Li6Hits[2][i].second + std::pow(Li6backgroundrateerr*countingtime, 2));
          fLi6StorageGraph->SetPoint(i, storagetime, Li6corrected);
          fLi6StorageGraph->SetPointError(i, 0., Li6correctederr);
          fHe3StorageGraph->SetPoint(i, storagetime, He3Hits[2][i].second - He3backgroundrate*countingtime);
          fHe3StorageGraph->SetPointError(i, 0., std::sqrt(He3Hits[2][i].second + std::pow(He3backgroundrateerr*countingtime, 2)));

          // subtract background from Li6 counts, normalize to He3 counts during irradiation, plot vs. storage time
          fStorageWithMonitorDuringIrradiation->SetPoint(i, storagetime, Li6corrected/He3Hits[0][i].second);
          fStorageWithMonitorDuringIrradiation->SetPointError(i, 0., std::sqrt(std::pow(Li6correctederr/Li6corrected, 2) + 1./He3Hits[0][i].second)*Li6corrected/He3Hits[0][i].second);

          double monitorcounts = fHe3Detector->GetMonitorCountsAfterIrradiationPerCycle()[i].second;
          // subtract background from Li6 counts, normalize to He3 counts after irradiation, plot vs. storage time
          fStorageWithMonitorAfterIrradiation->SetPoint(i, storagetime, Li6corrected / monitorcounts);
          fStorageWithMonitorAfterIrradiation->SetPointError(i, 0., std::sqrt(std::pow(Li6correctederr/Li6corrected, 2) + 1./monitorcounts) * Li6corrected / monitorcounts);
      }
      fexpo.SetParameters(10000, 10);
      fLi6StorageGraph->Fit(&fexpo,"Q");
      fexpo.SetParameters(1000, 10);
      fHe3StorageGraph->Fit(&fexpo,"Q");
      fexpo.SetParameters(10, 10);
      fStorageWithMonitorDuringIrradiation->Fit(&fexpo, "Q");
      fexpo.SetParameters(10, 10);
      fStorageWithMonitorAfterIrradiation->Fit(&fexpo, "Q");
  }

  return 1;
}





THe3CountsInSequence* TAnaManager::GetHe3CountsHistograms(){return fHe3CountsInSequence;}

