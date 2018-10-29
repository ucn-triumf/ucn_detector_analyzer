// Analyzer

#include <stdio.h>
#include <iostream>
#include <time.h>

#include "TRootanaEventLoop.hxx"
#include "TAnaManager.hxx"
#include "TUCNAnaViewer3.h"
#ifdef HAVE_MIDAS
#include "midas.h"
#include "msystem.h"
#endif


HNDLE hDB;

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

#ifdef HAVE_MIDAS
  int status;
  
  
  char xhostname[HOST_NAME_LENGTH];
  char xexptname[NAME_LENGTH];
  
  /* get default from environment */
  status = cm_get_environment(xhostname, sizeof(xhostname), xexptname, sizeof(xexptname));
  assert(status == CM_SUCCESS);
  
  //int watchdog = DEFAULT_WATCHDOG_TIMEOUT;
  int watchdog = 60*1000;

  status = cm_connect_experiment1(xhostname, xexptname, "analyzer", NULL, DEFAULT_ODB_SIZE, watchdog);
  
  if (status == CM_UNDEF_EXP)
    {
      fprintf(stderr, "TMidasOnline::connect: Error: experiment \"%s\" not defined.\n", xexptname);
      return;
    }
  else if (status != CM_SUCCESS)
    {
      fprintf(stderr, "TMidasOnline::connect: Cannot connect to MIDAS, status %d.\n", status);
      return;
    }
  

  status = cm_get_experiment_database(&hDB, NULL);
  assert(status == CM_SUCCESS);
  
  cm_set_watchdog_params(true, 60*1000);
#endif

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

  double lastCycleTime[2] = {0,0};

  bool ProcessMidasEvent(TDataContainer& dataContainer){

    if(!anaManager) InitManager();




    float PSDMax, PSDMin;   
    anaViewer->ProcessMidasEvent(dataContainer, 'n', PSDMax, PSDMin);

    anaManager->ProcessMidasEvent(dataContainer);

#ifdef HAVE_MIDAS
    if(!IsOffline() && 0){
      int status;
      char date[256];
      sprintf(date,"FDSFDSFSF");
      status = db_set_value_index(hDB,0,"/Analyzer/Li6/CycleStartTimes",date,sizeof(date),0,TID_STRING,false);
      if (status != DB_SUCCESS){
	cm_msg(MERROR,"Analyzer","Couldn't write time to MIDAS");
	return false;;
      }
    }
#endif



    // Update the numbers of hits per cycle (if running online).
    if(!IsOffline()){

      // Loop over Li6 and He3
      for(int det = 0; det < 2; det++){

	// Get the list of hits per cycle.
	std::vector<std::pair<double, double> > hitsPerCycle;
	if(det == 0) hitsPerCycle =  anaManager->GetLi6DetectorAnalyzer()->GetHitsPerCycle(); 
	else hitsPerCycle         =  anaManager->GetHe3DetectorAnalyzer()->GetHitsPerCycle(); 
	
	if(!hitsPerCycle.size()) return true;
	
	if(hitsPerCycle[hitsPerCycle.size()-1].first != lastCycleTime[0]){
	  std::cout << "New cycle!!! " << hitsPerCycle[hitsPerCycle.size()-1].first 
		    << " " << hitsPerCycle[hitsPerCycle.size()-1].first - lastCycleTime[0]
		    << " " << hitsPerCycle[hitsPerCycle.size()-1].second
		    << std::endl;

	
	  int index = hitsPerCycle.size()-1;
	  for(unsigned int i = 0; i < 10; i++){

	    time_t t(hitsPerCycle[index].first);
	    struct tm *tm = localtime(&t);
	    char date[256];
	    
	    strftime(date,sizeof(date),"%Y/%m/%d %H:%M:%S",tm);
	    std::cout << i << " time " << hitsPerCycle[i].first 
		      << " " << date 
		      << " " << hitsPerCycle[index].second <<  std::endl;	
	    // Upload the new valuves 
#ifdef HAVE_MIDAS
	    int status = db_set_value_index(hDB,0,"/Analyzer/Li6/CycleStartTimes",date,sizeof(date),i,TID_STRING,false);
	    if (status != DB_SUCCESS){
	      cm_msg(MERROR,"Analyzer","Couldn't write time to MIDAS");
	      return false;;
	    }
	    double nhits = hitsPerCycle[index].second;
	    status = db_set_value_index(hDB,0,"/Analyzer/Li6/UCNHitsPerCycle",&nhits,sizeof(nhits),i,TID_DOUBLE,false);
	    if (status != DB_SUCCESS){
	      cm_msg(MERROR,"Analyzer","Couldn't write time to MIDAS");
	      return false;;
	    }
	    
	    
#endif
	    index--;
	    if(index < 0) break;
	  }
	}
	
	lastCycleTime[det] = hitsPerCycle[hitsPerCycle.size()-1].first;

      }// End loop over Li6/He3

    }

    
    return true;
  }


}; 


int main(int argc, char *argv[])
{

  Analyzer::CreateSingleton<Analyzer>();
  return Analyzer::Get().ExecuteLoop(argc, argv);

}

