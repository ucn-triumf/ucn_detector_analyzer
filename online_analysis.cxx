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
HNDLE hDB;
#endif
#include <vector>
#include <fstream>


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

  int fRun;
  
  Analyzer() {

    SetOutputFilename("ucn_analyzer_output");
    DisableAutoMainWindow();
    UseBatchMode();
    SetOnlineName("online UCN counting");
    anaManager = 0;
    anaViewer = 0;
    fRun = 0;

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
    std::cout << "BeginRun Ana " << run << std::endl;
    anaManager->BeginRun(transition, run, time);
    std::cout << "Finished BeginRun Ana" << std::endl;
    fRun = run;    
  }

  std::string MakeDir(int run){
    char dirname[256];
    sprintf(dirname,"/home/ucn/online/online_analysis/run%i",run);
    printf("Dir name: %s\n",dirname);
    struct stat info;
    if( stat( dirname, &info ) != 0 ){
      int status = mkdir(dirname, 0777);
      if(status != 0){	
	cm_msg(MERROR,"Analyzer","Couldn't make directory %s.",dirname);
	return std::string("/fail");
      }
    }
    return std::string(dirname);
  }

  void EndRun(int transition,int run,int time){
    
    printf("Starting EOR\n");

    // Save some plots in directory.
    if(!IsOffline()){
    }else{
      printf("offline, nothing to do.\n");
    }

  }


  double lastCycleTime[2] = {0,0};

  bool ProcessMidasEvent(TDataContainer& dataContainer){

    if(!anaManager) InitManager();
    


    float PSDMax, PSDMin;   
    anaViewer->ProcessMidasEvent(dataContainer, 'n', PSDMax, PSDMin);

    anaManager->ProcessMidasEvent(dataContainer);

    // Loop over Li6 and He3
    bool needsUpdate = false;
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
      
      //std::cout << "Hit per " << hitsPerCycle.size() << std::endl;
      if(!hitsPerCycle.size()) return true;
      
      
      if(fHitsPerCycle[det].size() == 0 
	 || hitsPerCycle[hitsPerCycle.size()-1].first != fHitsPerCycle[det][fHitsPerCycle[det].size()-1].first){
	//if(hitsPerCycle[hitsPerCycle.size()-1].first != lastCycleTime[det]){
	needsUpdate = true;
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
#ifdef HAVE_MIDAS
	if(!IsOffline()){
	  
	  // Write results to file.
	  // Make the directory
	  char filename[256];
	  INT run;
	  int size = sizeof(run);
	  db_get_value(hDB, 0, "/Runinfo/Run Number", &run, &size, TID_INT, 0);
	  std::string dirname = MakeDir(run);      
	  sprintf(filename,"%s/%s_counts.json",dirname.c_str(),detector);        
	  std::fstream outfile;	    
	  outfile.open (filename, std::fstream::in | std::fstream::out | std::fstream::trunc);
	  if(!outfile.is_open()){
	    std::cout << "Failed to open outfile: " << filename << std::endl;
	    exit(0);
	  }else{
	    std::cout << "Successfully opened file " << filename << std::endl;
	  }
	  outfile << "{ \n " << std::endl;
	  
	  
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

	  	    
	    outfile << "  \"Cycle" << i << "\" : {" << std::endl;
	    outfile << "    \"Start Time\" : \"" << date << "\", " << std::endl;
	    outfile << "    \"Total Hits\" : " << fHitsPerCycle[det][i].second << ", " << std::endl;
	    outfile << "    \"Hits per period\" : [";
	    for(int per = 0; per < MaxPeriods; per++){
	      outfile << fHitsPerCyclePerPeriod[det][per][i].second;
	      if(per != (MaxPeriods-1)) outfile << ",";
	    }
	    outfile << "]" << std::endl;
	    
	    if(fHitsPerCycle[det].size()-1 == i){
	      outfile << "  }" << std::endl;
	    }else{
	      outfile << "  }," << std::endl;
	    }
	  }
	
	  // Finish the file
	  outfile << "} \n " << std::endl; 
	  outfile.close();
	}
#endif
      }
      

      
      lastCycleTime[det] = hitsPerCycle[hitsPerCycle.size()-1].first;
      
    }// End loop over Li6/He3
    
    
    if(needsUpdate){
#ifdef HAVE_MIDAS
      if(!IsOffline()){
	
	// Dump plots to file...
	
	// Make the directory
	char filename[256];
	INT run;
	int size = sizeof(run);
	db_get_value(hDB, 0, "/Runinfo/Run Number", &run, &size, TID_INT, 0);
	std::string dirname = MakeDir(run);      
	
	// Make a canvas to use.
	TCanvas *c1 = new TCanvas("c1","test",600,700);
	c1->cd();
	
	// Save interesting plots
	
	TGraphErrors* tmp = anaManager->GetTransmissionDuringCountingGraph();
	tmp->Draw("AP*"); tmp->SetMarkerStyle(20);
	sprintf(filename,"%s/TransmissionDuringCounting.png",dirname.c_str());
	c1->SaveAs(filename);
	
	tmp = anaManager->GetTransmissionWithPreStorageGraph();
	tmp->Draw("AP*"); tmp->SetMarkerStyle(20);
	sprintf(filename,"%s/TransmissionWithPreStorage.png",dirname.c_str());
	c1->SaveAs(filename);
	
	tmp = anaManager->GetTransmissionDuringIrradiationGraph();
	tmp->Draw("AP*"); tmp->SetMarkerStyle(20);
	sprintf(filename,"%s/TransmissionDuringIrradiation.png",dirname.c_str());
	c1->SaveAs(filename);
	
	tmp = anaManager->GetHe3DuringIrradiationGraph();
	tmp->Draw("AP*"); tmp->SetMarkerStyle(20);
	sprintf(filename,"%s/He3DuringIrradiation.png",dirname.c_str());
	c1->SaveAs(filename);
	
	tmp = anaManager->GetHe3DuringStorageGraph();
	tmp->Draw("AP*"); tmp->SetMarkerStyle(20);
	sprintf(filename,"%s/He3DuringStorage.png",dirname.c_str());
	c1->SaveAs(filename);
	
	tmp = anaManager->GetHe3AfterIrradiationGraph();
	tmp->Draw("AP*"); tmp->SetMarkerStyle(20);
	sprintf(filename,"%s/He3AfterIrradiation.png",dirname.c_str());
	c1->SaveAs(filename);
	
	tmp = anaManager->GetLi6StorageBackgroundGraph();
	tmp->Draw("AP*"); tmp->SetMarkerStyle(20);
	sprintf(filename,"%s/Li6StorageBackground.png",dirname.c_str());
	c1->SaveAs(filename);
	
	tmp = anaManager->GetHe3StorageBackgroundGraph();
	tmp->Draw("AP*"); tmp->SetMarkerStyle(20);
	sprintf(filename,"%s/He3StorageBackground.png",dirname.c_str());
	c1->SaveAs(filename);
	
	tmp = anaManager->GetLi6StorageGraph();
	tmp->Draw("AP*"); tmp->SetMarkerStyle(20);
	sprintf(filename,"%s/Li6Storage.png",dirname.c_str());
	c1->SaveAs(filename);
	
	tmp = anaManager->GetHe3StorageGraph();
	tmp->Draw("AP*"); tmp->SetMarkerStyle(20);
	sprintf(filename,"%s/He3Storage.png",dirname.c_str());
	c1->SaveAs(filename);
	
	tmp = anaManager->GetStorageWithMonitorDuringIrradiation();
	tmp->Draw("AP*"); tmp->SetMarkerStyle(20);
	sprintf(filename,"%s/StorageWithMonitorDuringIrradiation.png",dirname.c_str());
	c1->SaveAs(filename);
	
	tmp = anaManager->GetStorageWithMonitorAfterIrradiation();
	tmp->Draw("AP*"); tmp->SetMarkerStyle(20);
	sprintf(filename,"%s/StorageWithMonitorAfterIrradiation.png",dirname.c_str());
	c1->SaveAs(filename);
	
	TV1725PSDQL *psd_vs_ql = anaManager->GetV1725PSDvsQL();
	TH2D *tmp3 = new TH2D(*(TH2D*)psd_vs_ql->GetHistogram(0));
	for(int i = 1; i < 9; i++){
	  tmp3->Add(psd_vs_ql->GetHistogram(i));
	}
	tmp3->Draw("COLZ");
	sprintf(filename,"%s/Li6_PSD_vs_QL.png",dirname.c_str());
	c1->SaveAs(filename);
		
	TH1D *tmp2 =  anaManager->fUCNChronobox->fIV3DriveClosedDiff;
	tmp2->Draw();
	sprintf(filename,"%s/IV3DriveClosedDiff.png",dirname.c_str());
	c1->SaveAs(filename);	  
	
	
	
	// Write results to file.
	// Make the directory
	sprintf(filename,"%s/UCN_counts.json",dirname.c_str());        
	std::fstream outfile;	    
	outfile.open (filename, std::fstream::in | std::fstream::out | std::fstream::trunc);
	if(!outfile.is_open()){
	  std::cout << "Failed to open outfile: " << filename << std::endl;
	  exit(0);
	}else{
	  std::cout << "Successfully opened file " << filename << std::endl;
	}
	outfile << "{ \n " << std::endl;
	
	
	for(int det = 0; det < 3; det++){
	  
	  char detector[10];
	  if(det == 0){
	    sprintf(detector,"Li6");
	  }else if(det == 1){
	    sprintf(detector,"He3");
	  }else{
	    sprintf(detector,"He3Det2");

	  }	
	  outfile << "  \""<<detector<< "\" : { " << std::endl;;
	  for(unsigned int i = 0; i < fHitsPerCycle[det].size(); i++){

	    time_t t(fHitsPerCycle[det][i].first);
	    struct tm *tm = localtime(&t);
	    char date[256];
	    
	    strftime(date,sizeof(date),"%Y/%m/%d %H:%M:%S",tm);

	    outfile << "    \"Cycle" << i << "\" : {" << std::endl;
	    outfile << "      \"Start Time\" : \"" << date << "\", " << std::endl;
	    outfile << "      \"Total Hits\" : " << fHitsPerCycle[det][i].second << ", " << std::endl;
	    outfile << "      \"Hits per period\" : [";
	    for(int per = 0; per < MaxPeriods; per++){
	      outfile << fHitsPerCyclePerPeriod[det][per][i].second;
	      if(per != (MaxPeriods-1)) outfile << ",";
	    }
	    outfile << "]" << std::endl;
	    
	    if(fHitsPerCycle[det].size()-1 == i){
	      outfile << "    }" << std::endl;
	    }else{
	      outfile << "    }," << std::endl;
	    }
	  
	
	  }

	  outfile << "  }";
	  if(det != 2)outfile << ", ";
	  outfile << std::endl;

	}
	 
	outfile << "} \n " << std::endl; 
	outfile.close();



      }
    }  
#endif
	

    
    return true;
  }


}; 


int main(int argc, char *argv[])
{

  Analyzer::CreateSingleton<Analyzer>();
  return Analyzer::Get().ExecuteLoop(argc, argv);

}

