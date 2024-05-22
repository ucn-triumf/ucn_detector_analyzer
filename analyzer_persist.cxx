// Analyzer; nominally this analyzer will allow histograms that persist across runs...DW

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
    DisableAutoMainWindow();
    UseBatchMode();
    DisableRootOutput(true);
    SetOnlineName("jsroot_server_persist");
    anaManager = 0;
    anaViewer = 0;
  };

  virtual ~Analyzer() {};

  void Initialize(){

    anaManager = new TAnaManager(IsOffline(), GetODB());
    anaViewer  = new TUCNAnaViewer3();

  }

  void InitManager(){

    if(anaManager)
      delete anaManager;
    anaManager = new TAnaManager(IsOffline(), GetODB());
    if(anaViewer)
      delete anaViewer;
    anaViewer  = new TUCNAnaViewer3();


  }


  void BeginRun(int transition,int run,int time){

    //    InitManager();
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

