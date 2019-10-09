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
#include <vector>


HNDLE hDB;

class Analyzer: public TRootanaEventLoop {


public:

  // Two analysis managers.  Define and fill histograms in 
  // analysis manager.
  TAnaManager *anaManager;
  TUCNAnaViewer3 *anaViewer;

  // Set of times/UCN counts per cycle for two detectors.
  std::vector<std::vector<std::pair<double, double> > > fHitsPerCycle;
  // Set of times/UCN counts per cycle per period for two detectors.   
  std::vector<std::vector<std::vector<std::pair<double, double> > > > fHitsPerCyclePerPeriod;

  const int MaxPeriods = 10;
  
  Analyzer() {

    SetOutputFilename("ucn_analyzer_output");
    DisableAutoMainWindow();
    UseBatchMode();
    SetOnlineName("online UCN counting");
    anaManager = 0;
    anaViewer = 0;

    fHitsPerCycle.push_back(std::vector<std::pair<double, double> >());
    fHitsPerCycle.push_back(std::vector<std::pair<double, double> >());
    fHitsPerCycle.push_back(std::vector<std::pair<double, double> >());
    for(int det = 0; det < 3; det++){
      
      fHitsPerCyclePerPeriod.push_back(std::vector<std::vector<std::pair<double, double> > >());
      for(int i = 0; i < MaxPeriods; i++){
	fHitsPerCyclePerPeriod[det].push_back(std::vector<std::pair<double, double> >());
      }
    }
  };

  virtual ~Analyzer() {};

  void Initialize(){

#ifdef HAVE_THTTP_SERVER
    std::cout << "Using THttpServer in read/write mode" << std::endl;
    SetTHttpServerReadWrite();
#endif

    anaManager = new TAnaManager(IsOffline());
    std::cout << "Finish manager " << std::endl;
    anaViewer  = new TUCNAnaViewer3();


#ifdef HAVE_MIDAS
  int status;
  

  std::cout << "Connecting to ODB... " << std::endl;
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


  // Reset the numbers for the ODB analysis...
  if(!IsOffline()){

      // Loop over Li6 and He3
      for(int det = 0; det < 3; det++){
	char detector[10];
	if(det == 0){
	  sprintf(detector,"Li6");
	}else if(det == 1){
	  sprintf(detector,"He3");
	}else{
	  sprintf(detector,"He3Det2");
	}
	
	for(unsigned int i = 0; i < 10; i++){
	  char date[256];	    
	  sprintf(date,"N/A ");
	  char varname[100];
	  sprintf(varname,"/Analyzer/%s/CycleStartTimes",detector);
	  int status = db_set_value_index(hDB,0,varname,date,sizeof(date),i,TID_STRING,false);
	  if (status != DB_SUCCESS){
	    cm_msg(MERROR,"Analyzer","Couldn't write time to MIDAS");
	    return ;
	  }
	  double nhits = 0;
	  sprintf(varname,"/Analyzer/%s/UCNHitsPerCycle",detector);
	  status = db_set_value_index(hDB,0,varname,&nhits,sizeof(nhits),i,TID_DOUBLE,false);
	  if (status != DB_SUCCESS){
	    cm_msg(MERROR,"Analyzer","Couldn't write time to MIDAS");
	    return ;
	  }
	  for(int per = 0; per < MaxPeriods; per++){
	    sprintf(varname,"/Analyzer/%s/UCNHitsPerCyclePeriod%i",detector,per);
	    status = db_set_value_index(hDB,0,varname,&nhits,sizeof(nhits),i,TID_DOUBLE,false);
	    if (status != DB_SUCCESS){
	      cm_msg(MERROR,"Analyzer","Couldn't write time to MIDAS");
	      return ;
	    }
	  }
	   	    
#endif
	  
	}
      }// End loop over Li6/He3
    }
  }

  void InitManager(){
    
    std::cout << "Deleting and creating anaManager " << std::endl;
    if(anaManager)
      delete anaManager;
    anaManager = new TAnaManager(IsOffline());
    std::cout << "Deleting and creating anaViewer " << std::endl;
    if(anaViewer)
      delete anaViewer;
    anaViewer  = new TUCNAnaViewer3();
    std::cout << "Finished Deleting and creating " << std::endl;
    
  }
  
  
  void BeginRun(int transition,int run,int time){
    
    InitManager();
    std::cout << "BeginRun Ana" << std::endl;
    anaManager->BeginRun(transition, run, time);
    std::cout << "Finished BeginRun Ana" << std::endl;
    
  }

  double lastCycleTime[2] = {0,0};

  bool ProcessMidasEvent(TDataContainer& dataContainer){

    if(!anaManager) InitManager();
    


    float PSDMax, PSDMin;   
    anaViewer->ProcessMidasEvent(dataContainer, 'n', PSDMax, PSDMin);

    anaManager->ProcessMidasEvent(dataContainer);
    // Update the numbers of hits per cycle (if running online).
    if(!IsOffline() || 1){

      // Loop over Li6 and He3
      for(int det = 0; det < 3; det++){

	// Get the list of hits per cycle.
	std::vector<std::pair<double, double> > hitsPerCycle;
	//std::vector<std::vector<std::pair<double, double> > > hitsPerCyclePerPeriod; 
	TUCNDetectorBaseClass *idetector;
	char detector[10];
	if(det == 0){
	  sprintf(detector,"Li6");
	  hitsPerCycle =  anaManager->GetLi6DetectorAnalyzer()->GetHitsPerCycle(); 
	  idetector = anaManager->GetLi6DetectorAnalyzer();
	}else if(det == 1){
	  sprintf(detector,"He3");
	  hitsPerCycle =  anaManager->GetHe3DetectorAnalyzer()->GetHitsPerCycle(); 
	  idetector = anaManager->GetHe3DetectorAnalyzer();
	}else{
	  sprintf(detector,"He3Det2");
	  hitsPerCycle =  anaManager->GetHe3Detector2Analyzer()->GetHitsPerCycle(); 
	  idetector = anaManager->GetHe3Detector2Analyzer();
	}
	
	if(!hitsPerCycle.size()) return true;

	if(fHitsPerCycle[det].size() == 0 
	   || hitsPerCycle[hitsPerCycle.size()-1].first != fHitsPerCycle[det][fHitsPerCycle[det].size()-1].first){
	  //if(hitsPerCycle[hitsPerCycle.size()-1].first != lastCycleTime[det]){
	  
	  std::cout.precision(17);
	  if(fHitsPerCycle[det].size())

	    std::cout << "|||New cycle!!! " << det << " " << hitsPerCycle[hitsPerCycle.size()-1].first 
		      << " " << fHitsPerCycle[det][fHitsPerCycle[det].size()-1].first 
		      << " " << hitsPerCycle[hitsPerCycle.size()-1].first - 
	      fHitsPerCycle[det][fHitsPerCycle[det].size()-1].first
		      << " " << hitsPerCycle[hitsPerCycle.size()-1].second
		      << std::endl;
	  else
	    std::cout << "|||First new cycle!!! " << det << " " << hitsPerCycle[hitsPerCycle.size()-1].first 
		      << " " << hitsPerCycle[hitsPerCycle.size()-1].first
		      << " " << hitsPerCycle[hitsPerCycle.size()-1].second
		      << std::endl;
	    
	  // Add the last entry to our local list
	  fHitsPerCycle[det].push_back(hitsPerCycle[hitsPerCycle.size()-1]);
	  
	  for(int i = 0; i < MaxPeriods; i++){	    
	    std::vector<std::pair<double, double> > tmp = idetector->GetHitsPerCyclePerPeriod(i);
	    fHitsPerCyclePerPeriod[det][i].push_back(tmp[tmp.size()-1]);
	  }
	  
	  // Erase entries if too many (only 10 slots to display)...
	  if(fHitsPerCycle[det].size() > 10){
	    std::cout << "|||| eraseing  !!" << std::endl;
	    fHitsPerCycle[det].erase(fHitsPerCycle[det].begin());
	    for(int i = 0; i < MaxPeriods; i++){
	      fHitsPerCyclePerPeriod[det][i].erase(fHitsPerCyclePerPeriod[det][i].begin());
	    }
	  }

	  
	  //	  int index = hitsPerCycle.size()-1;
	  // Loop over cycles
	  for(unsigned int i = 0; i < fHitsPerCycle[det].size(); i++){
	    
	    time_t t(fHitsPerCycle[det][i].first);
	    struct tm *tm = localtime(&t);
	    char date[256];
	    
	    strftime(date,sizeof(date),"%Y/%m/%d %H:%M:%S",tm);
	    std::cout << det << " |||| " << i << " time " << fHitsPerCycle[det][i].first 
		      << " " << date 
		      << " " << fHitsPerCycle[det][i].second <<  std::endl;	
	    std::cout << "PPP ||| " ;
	    // Loop over periods...
	    for(int j = 0; j < MaxPeriods; j++){
	      std::cout << fHitsPerCyclePerPeriod[det][j][i].second << ", " ;
	    }
	    std::cout << std::endl;
	    // Upload the new valuves 
#ifdef HAVE_MIDAS
	    if(!IsOffline()){
	      char varname[100];
	      sprintf(varname,"/Analyzer/%s/CycleStartTimes",detector);
	      int status = db_set_value_index(hDB,0,varname,date,sizeof(date),i,TID_STRING,false);
	      if (status != DB_SUCCESS){
		cm_msg(MERROR,"Analyzer","Couldn't write time to MIDAS");
		return false;;
	      }
	      double nhits = fHitsPerCycle[det][i].second;
	      sprintf(varname,"/Analyzer/%s/UCNHitsPerCycle",detector);
	      status = db_set_value_index(hDB,0,varname,&nhits,sizeof(nhits),i,TID_DOUBLE,false);
	      if (status != DB_SUCCESS){
		cm_msg(MERROR,"Analyzer","Couldn't write time to MIDAS");
		return false;;
	      }
	      for(int per = 0; per < MaxPeriods; per++){
		nhits = fHitsPerCyclePerPeriod[det][per][i].second;
		sprintf(varname,"/Analyzer/%s/UCNHitsPerCyclePeriod%i",detector,per);
		status = db_set_value_index(hDB,0,varname,&nhits,sizeof(nhits),i,TID_DOUBLE,false);
		if (status != DB_SUCCESS){
		  cm_msg(MERROR,"Analyzer","Couldn't write time to MIDAS");
		  return false;;
		}
	      }
	      
	    }
#endif
	    //	    index--;
	    //if(index < 0) break;
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

