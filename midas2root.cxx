// UCN midas2root converter program

#include <stdio.h>
#include <iostream>
#include <time.h>

#include "TRootanaEventLoop.hxx"
#include "TAnaManager.hxx"
#include "TUCNAnaViewer3.h"

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

