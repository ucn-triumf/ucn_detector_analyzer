#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include "TRootanaDisplay.hxx"
#include "TH1D.h"
#include "TV792Data.hxx"
#include "TSimpleHistogramCanvas.hxx"
#include "TFancyHistogramCanvas.hxx"
#include "TInterestingEventManager.hxx"
#include "TFancyHistogramCanvas.hxx"
#include "TV1720Histograms.h"
#include "TV1720WaveformDisplay.h"
#include "TAnaManager.hxx"
#include "TUCNAnaViewer3.h"
//#include "TUCNRateViewer.h"
#include "TRootanaDisplay.hxx"
#include "PulseShapeStruct.h"
#include "TDataContainer.hxx"
#include "TV1720Histograms.h"
#include "TStyle.h"

int board, ch, ev, preev, eventSelect;
int firstPlot = 0;
int m=0;

char CutChoice; //< if = y then plot waves in PSDMax-PSDMin range
float PSDMax;   //< Max PSD value of cut
float PSDMin;   //< Min PSD value of cut

std::fstream eventNumber;


class MyTestLoop: public TRootanaDisplay { 

public:
	
  // An analysis manager.  Define and fill histograms in 
  // analysis manager.
  TAnaManager *anaManager;
  TUCNAnaViewer3 *anaViewer;
  //  TUCNRateViewer *rateViewer;
  bool fUsePCTime;

  MyTestLoop() {

    SetOutputFilename("ucn_display_output");
    SetOnlineName("UCNDisplay");
    
    DisableRootOutput(false);
    fUsePCTime = false;

  }

  bool CheckOption(std::string option){

    if(option == "--use-pc-time"){
      std::cout << "Using PC time for UCN hit times" << std::endl;
      fUsePCTime = true;
      return true;
    }
    return false;
  }
  
  void Initialize(){
    
    anaManager = new TAnaManager(IsOffline());
    anaViewer  = new TUCNAnaViewer3();
    SetOnlineUpdatingBasedSeconds();

    // Uncomment this to enable the 'interesting event' functionality.
    //iem_t::instance()->Enable();
  };

  void AddAllCanvases(){

    // Add the common plots, for He3 and Li6 detectors

    for(int i = 0; i < 3; i++){
      if(i == 1) continue;
      std::cout << "Step " << i << std::endl;
      TUCNDetectorBaseClass* detector = anaManager->GetHe3DetectorAnalyzer();
      std::string tabname = std::string("He3 Analysis");
      if(i==1){
        detector = anaManager->GetHe3Detector2Analyzer();
        tabname = std::string("He-3 DET2");
      }else if(i==2){
        detector = anaManager->GetLi6DetectorAnalyzer();
        tabname = std::string("Li6 Analysis");
      }

      // Charge Spectrum
      if(0)AddSingleCanvas(new TFancyHistogramCanvas(detector->GetChargeHistograms(),
                                                "Charge"),tabname.c_str());
      
      // UCN Rate Vs Time
      AddSingleCanvas(detector->GetRateVsTimeCanvas(),tabname.c_str());
      
      // UCN hits in cycle
      AddSingleCanvas(detector->GetHitsInCycleCanvas(),tabname.c_str());
      AddSingleCanvas(detector->GetHitsInCycleCumulCanvas(),tabname.c_str());
      
      // UCN Hits Per Cycle
      if(0)AddSingleCanvas(detector->GetHitsPerCycleCanvas(),tabname.c_str());

      // UCN Hits Per Cycle histogram
      AddSingleCanvas(detector->GetHitsPerCycleHCanvas(),tabname.c_str());
    }

    // Add detailed specific plots for V1725
    AddSingleCanvas(new TFancyHistogramCanvas(anaManager->GetV1725Waveform(),"V1725 Waveform"),"V1725 Details");
    AddSingleCanvas(new TFancyHistogramCanvas(anaManager->GetV1725PSDvsQL(),"PSD vs QLong"),"V1725 Details");
    AddSingleCanvas(new TFancyHistogramCanvas(anaManager->GetV1725_PH(),"Pulse Height"),"V1725 Details");
    AddSingleCanvas(new TFancyHistogramCanvas(anaManager->GetV1725_QL(),"Q-long"),"V1725 Details");

    // Add plots for V785 charge
    if(0)AddSingleCanvas(new TFancyHistogramCanvas(anaManager->GetV785Histo(),"V785 Charge"),"V785 Details");
 
    // Add for chronobox
    if(0)AddSingleCanvas(new TFancyHistogramCanvas(anaManager->fUCNChronobox->fRisingEdgeDiff,"Rising Edge Differences"),"Chronobox");
    if(0)AddSingleCanvas(new TFancyHistogramCanvas(anaManager->fUCNChronobox->fFallingEdgeDiff,"Falling Edge Differences"),"Chronobox");   
    /*    AddSingleCanvas(new TFancyHistogramCanvas(anaManager->fUCNChronobox->fPulseWidths,"Pulse Widths"),"Chronobox");
    AddSingleCanvas(anaManager->fUCNChronobox->GetIV3Canvas(),"Chronobox");
    TMulticanvas *canvas = new TMulticanvas("IV3 Canvas ");
    canvas->AddHistoSingle(anaManager->fUCNChronobox->fIV3DriveClosedDiff,0);
    canvas->AddHistoSingle(anaManager->fUCNChronobox->fIV3DriveOpenedDiff,1);
    canvas->AddHistoSingle(anaManager->fUCNChronobox->fIV3ClosedOpenDiff,2);
    //AddSingleCanvas(canvas,"Chronobox");*/

    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->fUCNChronobox->fIV2DriveClosedDiff, "IV2 Drive/Closed"), "Chronobox");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->fUCNChronobox->fIV2DriveOpenedDiff, "IV2 Drive/Open"), "Chronobox");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->fUCNChronobox->fIV2ClosedOpenDiff,  "IV2 Closed/Open"), "Chronobox");

    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->fUCNChronobox->fIV3DriveClosedDiff, "IV3 Drive/Closed"), "Chronobox");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->fUCNChronobox->fIV3DriveOpenedDiff, "IV3 Drive/Open"), "Chronobox");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->fUCNChronobox->fIV3ClosedOpenDiff, "IV3 Closed/Open"), "Chronobox");



    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->GetTransmissionDuringCountingGraph(), "Li6/He3 ratio during counting"), "Transmission");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->GetHe3DuringIrradiationGraph(), "He3 counts during irradiation"), "Transmission");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->GetTransmissionDuringIrradiationGraph(), "Li6 during counting/He3 during irradiation"), "Transmission");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->GetLi6StorageBackgroundGraph(), "Li6 background during pre-storage"), "Transmission");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->GetHe3DuringStorageGraph(), "He3 counts during storage"), "Transmission");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->GetTransmissionWithPreStorageGraph(), "Li6 during counting/He3 during pre-storage"), "Transmission");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->GetLi6StorageBackgroundGraph(), "Li6 background during storage"), "Storage lifetime without monitor");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->GetLi6StorageGraph(), "Li6 counts"), "Storage lifetime without monitor");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->GetHe3StorageBackgroundGraph(), "He3 background during storage"), "Storage lifetime without monitor");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->GetHe3StorageGraph(), "He3 counts"), "Storage lifetime without monitor");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->GetLi6StorageBackgroundGraph(), "Li6 background during storage"), "Storage lifetime with monitor");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->GetHe3DuringIrradiationGraph(), "He3 counts during irradiation"), "Storage lifetime with monitor");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->GetStorageWithMonitorDuringIrradiation(), "Li6 counts/He3 counts during irradiation"), "Storage lifetime with monitor");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->GetHe3AfterIrradiationGraph(), "He3 counts after irradiation"), "Storage lifetime with monitor");
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->GetStorageWithMonitorAfterIrradiation(), "Li6 counts/He3 counts after irradiation"), "Storage lifetime with monitor");
  };


  virtual ~MyTestLoop() {};

  void BeginRun(int transition,int run,int time) {
    std::cout << "User BOR method" << std::endl;
    anaManager->BeginRun(transition, run, time);
  }

  void EndRun(int transition,int run,int time) {
    std::cout << "User EOR method" << std::endl;
  }

  void ResetHistograms(){

    std::cout << "Resetting..." << std::endl;
    std::cout << "Done reset " << std::endl;
  }

  void UpdateHistograms(TDataContainer& dataContainer){

    if(fUsePCTime) anaManager->UsePCTime();
    
    anaViewer->ProcessMidasEvent(dataContainer, 'n', PSDMax, PSDMin);

    anaManager->ProcessMidasEvent(dataContainer);

    if(0)
      if(dataContainer.GetMidasData().GetSerialNumber() %1000 ==0)
	std::cout << "IV3 histogram: " << anaManager->fUCNChronobox->fIV3DriveClosedDiff->GetEntries() 
		  << " 0x" << anaManager->fUCNChronobox->fIV3DriveClosedDiff << std::endl;
  }

  void PlotCanvas(TDataContainer& dataContainer){

  }
  

  bool fileExists(const std::string& filename)
  {
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
      {
        return true;
      }
    return false;
  }
  

}; 






int main(int argc, char *argv[])
{
  MyTestLoop::CreateSingleton<MyTestLoop>();  
  return MyTestLoop::Get().ExecuteLoop(argc, argv);
}

