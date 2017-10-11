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
#include "TUCNRateViewer.h"
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
  TUCNRateViewer *rateViewer;

  MyTestLoop() {

    SetOutputFilename("ucn_display_output");
    SetOnlineName("UCNDisplay");
    
    DisableRootOutput(false);

  }
  
  void Initialize(){
    
    anaManager = new TAnaManager(IsOffline());
    anaViewer  = new TUCNAnaViewer3();
    rateViewer = new TUCNRateViewer();
    // Number of events to skip before plotting one.
    //SetNumberSkipEvent(10);
    // Choose to use functionality to update after X seconds
    SetOnlineUpdatingBasedSeconds();
    // Uncomment this to enable the 'interesting event' functionality.
    //iem_t::instance()->Enable();
  };

  void AddAllCanvases(){

 
    // Add He-3 plots
    // Charge Spectrum
    AddSingleCanvas(new TFancyHistogramCanvas(anaManager->GetHe3DetectorAnalyzer()->GetChargeHistograms(),
					      "Charge"),"He3 Analysis");

    // UCN Rate Vs Time
    AddSingleCanvas(anaManager->GetHe3DetectorAnalyzer()->GetRateVsTimeCanvas(),"He3 Analysis");

    // UCN hits in cycle
    AddSingleCanvas(new TSimpleHistogramCanvas(anaManager->GetHe3DetectorAnalyzer()->GetHitsInCycle(),
					       "Hits in Cycle"),"He3 Analysis");
   // UCN Hits Per Cycle
    AddSingleCanvas(anaManager->GetHe3DetectorAnalyzer()->GetHitsPerCycleCanvas(),"He3 Analysis");


    if(anaManager->HaveV792Histograms()) 
      AddSingleCanvas(new TFancyHistogramCanvas(anaManager->GetV792Histograms(),"V792"));
    
    //if(anaManager->HaveV1720Histograms()) 
    //AddSingleCanvas(new TFancyHistogramCanvas(anaManager->GetV1720Histograms(),"V1720 Waveforms"));
    //    if(anaManager->HaveHe3RateHistograms()) 
    //AddSingleCanvas(new TFancyHistogramCanvas(anaManager->GetHe3RateHistograms(),"He3 UCN rate"));
    AddSingleCanvas(new TFancyHistogramCanvas(anaManager->GetHe3CountsHistograms(),"He3 UCN Counts"));
    AddSingleCanvas(new TFancyHistogramCanvas(anaViewer->fV1720WaveformDisplay, "V1720 Waveform",9,false));
    AddSingleCanvas(new TFancyHistogramCanvas(anaViewer->fV1720QLQL, "V1720 QLvsQL", 9, false));
    AddSingleCanvas(new TFancyHistogramCanvas(anaViewer->fV1720QSQS, "V1720 QSvsQS", 9, false));
    AddSingleCanvas("Digitizer Count Rate on all channels");
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
    if(!rateViewer->GetCurrHisto())return;
    TH1D* hC = rateViewer->GetCurrHisto()->Get();
    TH1D* hP = rateViewer->GetPrevHisto()->Get();
    if ( hC ) hC->Reset();
    if ( hP ) hP->Reset();
    std::cout << "Done reset " << std::endl;
  }

  void UpdateHistograms(TDataContainer& dataContainer){

    //TMidasEvent sample = dataContainer.GetMidasEvent();
    //TMidas_BANK32 * bank = NULL;  
    //char * pdata = sample.GetData();
    //DPP_Bank_Out_t *b; 
    //std::string banklist(sample.GetBankList());

    /// If CutChoice hasnt been given a value then the user can choose to perfrom
    /// a PSD cut on the data bein viewed or not. If yes then user enters the max
    /// and min values of this PSD cut.
    /// A. Sikora June 2017
    CutChoice = 'n';
    if (!CutChoice)
      {
	std::cout<<"Would you like to enter a PSD cut? (y = yes, n = no)"<<std::endl;
	std::cin>>CutChoice;
	if (CutChoice == 'y')
	  {
	    std::cout<<"Enter max PSD value "<<std::endl;
	    std::cin>>PSDMax;
	    std::cout<<"Enter min PSD value "<<std::endl;
	    std::cin>>PSDMin;
	  }
      }
    //std::cout<<"anaViewer2 Update Histograms"<<std::endl;

    //rateViewer->ProcessMidasEvent(dataContainer,CutChoice, PSDMax, PSDMin);

    //std::cout<<"anaViewer Update Histograms"<<std::endl;
    //if(banklist.find("W20") != std::string::npos)
    //anaViewer->ProcessMidasEvent(dataContainer, CutChoice, PSDMax, PSDMin);

    //std::cout<<"anaManager Update Histograms"<<std::endl;
    anaManager->ProcessMidasEvent(dataContainer);
  }

  void PlotCanvas(TDataContainer& dataContainer){
    
    if(GetDisplayWindow()->GetCurrentTabName().compare("Digitizer Count Rate on all channels") == 0){       
      TCanvas* c1 = GetDisplayWindow()->GetCanvas("Digitizer Count Rate on all channels");
    // try to force some root options
      gStyle->SetOptTitle(1);
      c1->UseCurrentStyle();

      
      c1->Clear();
      c1->Divide(1,2);
      c1->cd(1);
      UCNRateHistogram* hcur = rateViewer->GetCurrHisto();
      if ( hcur != NULL) {
	std::cout<<"hcur title="<<hcur->Get()->GetTitle()<<std::endl;
	hcur->Get()->Draw();

      }
      c1->cd(2);
      UCNRateHistogram* hprev = rateViewer->GetPrevHisto();
      if ( hprev != NULL) hprev->Get()->Draw();
      c1->Modified();
      c1->Update();
    }
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

