// UCN midas2root converter program

#include <stdio.h>
#include <iostream>
#include <time.h>

#include "TRootanaEventLoop.hxx"
#include "TAnaManager.hxx"
#include "TUCNAnaViewer3.h"
#include "XmlOdb.h"

class Analyzer: public TRootanaEventLoop {




public:

  // Two analysis managers.  Define and fill histograms in 
  // analysis manager.
  TAnaManager *anaManager;
  TUCNAnaViewer3 *anaViewer;

  
  Analyzer() {

    SetOutputFilename("ucn_tree_");
    DisableAutoMainWindow();
    UseBatchMode();
    SetOnlineName("midas2root");
    anaManager = 0;
    anaViewer = 0;
  };

  virtual ~Analyzer() {};

  void Initialize(){

#ifdef HAVE_THTTP_SERVER
    std::cout << "Using THttpServer in read/write mode" << std::endl;
    SetTHttpServerReadWrite();
#endif

    // Start the anaManager with flag to write trees.
    anaManager = 0; //new TAnaManager(IsOffline(),true);
    anaViewer  = 0; //new TUCNAnaViewer3();
    
  }

  void InitManager(){
    
    if(anaManager)
      delete anaManager;
    anaManager = new TAnaManager(IsOffline(),true);
    if(anaViewer)
      delete anaViewer;
    anaViewer  = new TUCNAnaViewer3();
    
    
  }
  
  
  void BeginRun(int transition,int run,int time){
    
    InitManager();
    anaManager->BeginRun(transition, run, time);

    // Get the run comment and the shifter from the ODB dump at BOR.
    XmlOdb *odb = dynamic_cast<XmlOdb*>(GetODB());
    TTree *headerTree = new TTree("headerTree", "headerTree");

    std::string shifter_string(odb->odbReadString("/Experiment/Edit on start/Shifters",0,"XXX"));
    std::string comment_string(odb->odbReadString("/Experiment/Edit on start/Comment",0,"XXX"));
    headerTree->Branch("shifter", &shifter_string);
    headerTree->Branch("comment", &comment_string);
    headerTree->Fill();
 
    std::cout << "Run shifter " << shifter_string << std::endl;
    std::cout << "Run comment " << comment_string << std::endl;
    
    
  }


  bool ProcessMidasEvent(TDataContainer& dataContainer){

    if(!anaManager) InitManager();

    float PSDMax, PSDMin;   
    anaViewer->ProcessMidasEvent(dataContainer, 'n', PSDMax, PSDMin);

    anaManager->ProcessMidasEvent(dataContainer);
    
    return true;
  }


}; 


int main(int argc, char *argv[])
{

  Analyzer::CreateSingleton<Analyzer>();
  return Analyzer::Get().ExecuteLoop(argc, argv);

}

