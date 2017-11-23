// Analyzer

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

    SetOutputFilename("ucn_analyzer_output");
    DisableAutoMainWindow();
    UseBatchMode();
    SetOnlineName("jsroot_server_current");
    anaManager = 0;
    anaViewer = 0;
  };

  virtual ~Analyzer() {};

  void Initialize(){

#ifdef HAVE_THTTP_SERVER
    std::cout << "Using THttpServer in read/write mode" << std::endl;
    SetTHttpServerReadWrite();
#endif

    anaManager = new TAnaManager(IsOffline());
    anaViewer  = new TUCNAnaViewer3();
    
  }

  void InitManager(){
    
    if(anaManager)
      delete anaManager;
    anaManager = new TAnaManager(IsOffline());
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

