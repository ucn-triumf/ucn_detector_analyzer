// Analyzer; nominally this analyzer will allow histograms that persist across runs...DW

#include <stdio.h>
#include <iostream>
#include <time.h>
#ifdef HAVE_MIDAS
#include "midas.h"
#include "msystem.h"
HNDLE hDB;
#endif

#include "TRootanaEventLoop.hxx"
#include "TV1725DppPsdData.hxx"

class Analyzer: public TRootanaEventLoop {




public:

  int bor_time;
  bool bor_check_done;
  double start_time;
  std::vector<double> time_synch_pulses;

  Analyzer() {
    DisableAutoMainWindow();
    UseBatchMode();
    DisableRootOutput(true);
    SetOnlineName("data_quality_checker");
    bor_check_done = true;


  };

  virtual ~Analyzer() {};

  void Initialize(){

#ifdef HAVE_MIDAS
    std::cout << "Connecting to ODB... " << std::endl;
    char xhostname[HOST_NAME_LENGTH];
    char xexptname[NAME_LENGTH];
    int status;
    /* get default from environment */
    status = cm_get_environment(xhostname, sizeof(xhostname), xexptname, sizeof(xexptname));
    assert(status == CM_SUCCESS);
    
    //int watchdog = DEFAULT_WATCHDOG_TIMEOUT;
    int watchdog = 60*1000;
    
    status = cm_connect_experiment1(xhostname, xexptname, "data_quality_checker", NULL, DEFAULT_ODB_SIZE, watchdog);
    
    status = cm_get_experiment_database(&hDB, NULL);
    assert(status == CM_SUCCESS);
    cm_set_watchdog_params(true, 60*1000);
    printf("Connected to ODB\n");
#endif
    
    bor_time = -1;
    bor_check_done = true;
  }

  
  
  void BeginRun(int transition,int run,int time){
    
    bor_time = -1;
    time_synch_pulses.clear();
    start_time = 0;
#ifdef HAVE_MIDAS
    // Check if the run is stopped; if the run is ongoing, the checks don't make so much sense.
    INT state;
    int size = sizeof(state);
    db_get_value(hDB, 0, "/Runinfo/State", &state, &size, TID_INT, 0);
    if(state == 1){
      printf("Run stopped, BOR check meaningful\n");
      bor_check_done = false;
    }else{
      printf("Run not stopped, BOR check not meaningful\n");
      bor_check_done = true;
    }          
#else
    bor_check_done = false;
#endif
    
  }

  void EndRun(int transition,int run,int time){
    

#ifdef HAVE_MIDAS

    std::string message;
    bool good_number = false;
    if(time_synch_pulses.size() == 10){
      good_number = true;
      message.append("#Time synch pulses = 10;");
    }else{
      cm_msg(MERROR,"Data Quality","EOR check failed; there were %i time synch pulses (should be 10).\n",
	     time_synch_pulses.size());
    }
    
    if(good_number){
      cm_msg(MINFO,"Data Quality","EOR check successful; %s",message.c_str());
    }
    if(time_synch_pulses.size() && start_time != 0){
      double tdiff = time_synch_pulses[0] - start_time;
      cm_msg(MINFO,"Data Quality","EOR; total run length was %f seconds.",tdiff);
    }
    time_synch_pulses.clear();
    
    
#else
#endif
    time_synch_pulses.clear();
  
  }


  bool ProcessMidasEvent(TDataContainer& dataContainer){

    if(bor_time < 0) bor_time = dataContainer.GetMidasData().GetTimeStamp();

    // Analyze V1725 data
    TV1725DppPsdData *data = dataContainer.GetEventData<TV1725DppPsdData>("W500");
    if(data){      
      /// Get the Vector of ADC Measurements.
      std::vector<ChannelMeasurement> measurements = data->GetMeasurements();      
      for(unsigned int i = 0; i < measurements.size(); i++){    
	ChannelMeasurement meas = measurements[i];
	double timestamp = meas.GetExtendedTimeTag();
	timestamp *= 0.000000004;
	if(meas.GetChannel() == 11){
	  time_synch_pulses.push_back(timestamp);
	}
      }
    }

    // BOR check done 15s into start of the run.
    if(!bor_check_done){
#ifdef HAVE_MIDAS
      int time = dataContainer.GetMidasData().GetTimeStamp();
      int dtime = time - bor_time;
      if(dtime > 16){
	std::cout << "BOR check!" << std::endl;

	if(start_time == 0  && time_synch_pulses.size()) start_time = time_synch_pulses[0];

	std::string message;
	bool good_number = false;
	if(time_synch_pulses.size() == 10){
          good_number = true;
	  message.append("#Time synch pulses = 10;");
	}else{
          cm_msg(MERROR,"Data Quality","BOR check failed; there were %i time synch pulses (should be 10).\n",
		 time_synch_pulses.size());
	}
	
	if(good_number){
	  cm_msg(MINFO,"Data Quality","BOR check successful; %s",message.c_str());
	}
	time_synch_pulses.clear();
	bor_check_done = true;
      }
#endif
    }

    return true;
  }


}; 


int main(int argc, char *argv[])
{

  Analyzer::CreateSingleton<Analyzer>();
  return Analyzer::Get().ExecuteLoop(argc, argv);

}

