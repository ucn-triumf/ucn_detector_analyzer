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

  MyTestLoop() {

    SetOutputFilename("ucn_display_output");
    SetOnlineName("UCNDisplay");
    
    DisableRootOutput(false);

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

    for(int i = 0; i < 2; i++){
      TUCNDetectorBaseClass* detector = anaManager->GetHe3DetectorAnalyzer();
      std::string tabname = std::string("He3 Analysis");
      if(i==1){
        detector = anaManager->GetLi6DetectorAnalyzer();
        tabname = std::string("Li6 Analysis");
      }

      // Charge Spectrum
      AddSingleCanvas(new TFancyHistogramCanvas(detector->GetChargeHistograms(),
                                                "Charge"),tabname.c_str());
      
      // UCN Rate Vs Time
      AddSingleCanvas(detector->GetRateVsTimeCanvas(),tabname.c_str());
      
      // UCN hits in cycle
      AddSingleCanvas(detector->GetHitsInCycleCanvas(),tabname.c_str());
      AddSingleCanvas(detector->GetHitsInCycleCumulCanvas(),tabname.c_str());
      
      // UCN Hits Per Cycle
      AddSingleCanvas(detector->GetHitsPerCycleCanvas(),tabname.c_str());

    }

    // Add detailed specific plots for V1720
    //    TLi6Detector* li6detector = dynamic_cast<TLi6Detector*>(anaManager->GetLi6DetectorAnalyzer());
    //if(li6detector) AddSingleCanvas(li6detector->GetV1720BaselineCanvas(),"V1720 Details");
    
    AddSingleCanvas(new TFancyHistogramCanvas(anaViewer->fV1720WaveformDisplay, "V1720 Waveform",9,false),"V1720 Details");
    AddSingleCanvas(new TFancyHistogramCanvas(anaViewer->fV1720QSQLHistograms, "Q Short vs Q Long",16,false),"V1720 Details");
    AddSingleCanvas(new TFancyHistogramCanvas(anaViewer->fV1720PSDQLHistograms, "PSD vs Q Long",16,false),"V1720 Details");

    // Add plots about sequencing checks
    TLi6Detector* li6detector = dynamic_cast<TLi6Detector*>(anaManager->GetLi6DetectorAnalyzer());
    if(li6detector) AddSingleCanvas(li6detector->GetV1720SequenceCanvas(),"Sequencing");
    
   
  };


  virtual ~MyTestLoop() {};

  void BeginRun(int transition,int run,int time) {
    std::cout << "User BOR method" << std::endl;
  }

  void EndRun(int transition,int run,int time) {
    std::cout << "User EOR method" << std::endl;
  }

  void ResetHistograms(){

    std::cout << "Resetting..." << std::endl;
    std::cout << "Done reset " << std::endl;
  }

  void UpdateHistograms(TDataContainer& dataContainer){

    anaViewer->ProcessMidasEvent(dataContainer, 'n', PSDMax, PSDMin);

    anaManager->ProcessMidasEvent(dataContainer);
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

