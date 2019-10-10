// Analyzer; nominally this analyzer will allow histograms that persist across runs...DW

#include <stdio.h>
#include <iostream>
#include <time.h>

#include "TRootanaEventLoop.hxx"
#include "TV1725DppPsdData.hxx"

class Analyzer: public TRootanaEventLoop {




public:

  int bor_time;
  bool bor_check_done;
  
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
    
    bor_time = -1;
    bor_check_done = true;
  }

  
  
  void BeginRun(int transition,int run,int time){
    
    bor_check_done = false;
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
	if(meas.GetChannel() == 11) time_synch_pulses.push_back(timestamp);
      }
    }



    // BOR check done 15s into start of the run.
    if(!bor_check_done){
      int time = dataContainer.GetMidasData().GetTimeStamp();
      int dtime = time - bor_time;
      if(dtime > 15){
	std::cout << "BOR check!" << std::endl;
	std::cout << "Number of V1725 time synchs: "
		  << time_synch_pulses.size() << std::endl;
	bor_check_done = true;
      }
    }

    return true;
  }


}; 


int main(int argc, char *argv[])
{

  Analyzer::CreateSingleton<Analyzer>();
  return Analyzer::Get().ExecuteLoop(argc, argv);

}

