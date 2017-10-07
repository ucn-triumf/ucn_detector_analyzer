//Andrew Sikora
//June 9, 2016

#include <stdio.h>
#include <algorithm>
#include <cmath>

#include "TH1D.h"
#include "TH2D.h"

#include "TFancyHistogramCanvas.hxx"
#include "TV1720Histograms.h"
#include "TV1720WaveformDisplay.h"

#include "TUCNAnaViewer3.h"

#include "TRootanaDisplay.hxx"
#include "PulseShapeStruct.h"
#include "TDataContainer.hxx"
#include "TV1720Histograms.h"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/stat.h>

//using namespace std;

int board, ch, ev, preev, eventSelect;
int firstPlot = 0;
int m=0;

char CutChoice; //< if = y then plot waves in PSDMax-PSDMin range
float PSDMax;   //< Max PSD value of cut
float PSDMin;   //< Min PSD value of cut

std::fstream eventNumber;

class UCNDisplay3: public TRootanaDisplay {

  TUCNAnaViewer3 *anaViewer;

public:

  UCNDisplay3()
  {
    SetDisplayName("UCN Display3");
  };
  
  void AddAllCanvases()
  {
    anaViewer = new TUCNAnaViewer3();
    
    TFancyHistogramCanvas* v1720_waveform = 
      new TFancyHistogramCanvas(anaViewer->fV1720WaveformDisplay, "V1720 Waveform",9,false);
    AddSingleCanvas(v1720_waveform);
    
    TFancyHistogramCanvas * v1720_QLQL = new TFancyHistogramCanvas(anaViewer->fV1720QLQL, "V1720 QLvsQL", 9, false);
    AddSingleCanvas(v1720_QLQL);

    TFancyHistogramCanvas * v1720_QSQS = new TFancyHistogramCanvas(anaViewer->fV1720QSQS, "V1720 QSvsQS", 9, false);
    AddSingleCanvas(v1720_QSQS);
    
  };
  
  ~UCNDisplay3() {};
  
  
  void UpdateHistograms(TDataContainer& dataContainer)
  {

    TMidasEvent sample = dataContainer.GetMidasEvent();
    TMidas_BANK32 * bank = NULL;  
    char * pdata = sample.GetData();
    DPP_Bank_Out_t *b; 

    /// If CutChoice hasnt been given a value then the user can choose to perfrom
    /// a PSD cut on the data bein viewed or not. If yes then user enters the max
    /// and min values of this PSD cut.
    /// A. Sikora June 2017
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

    anaViewer->ProcessMidasEvent(dataContainer, CutChoice, PSDMax, PSDMin);
  }
  
  void PlotCanvas(TDataContainer& dataContainer)
  {
    if(GetDisplayWindow()->GetCurrentTabName().compare("V1720 Waveform")==0)
      {	
	TCanvas* cl= GetDisplayWindow()->GetCanvas("V1720 Waveform");	
	cl->Clear();	
	anaViewer->fV1720WaveformDisplay->GetHistogram(1)->Draw("hist");	
	cl->Modified();	
	cl->Update();
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
  UCNDisplay3::CreateSingleton<UCNDisplay3>();
  return UCNDisplay3::Get().ExecuteLoop(argc, argv);
}




