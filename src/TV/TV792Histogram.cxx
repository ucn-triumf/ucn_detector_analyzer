#include "TV792Histogram.h"

#include "TV792NData.hxx"
#include "TDirectory.h"

const int Nchannels = 16;
 #include <sys/time.h>

/// Reset the histograms for this canvas
TV792Histograms::TV792Histograms(){  
  
  CreateHistograms();
}


void TV792Histograms::CreateHistograms(){
  

  // Otherwise make histograms
  clear();
  
  for(int i = 0; i < Nchannels; i++){ // loop over channels    

    char name[100];
    char title[100];
    sprintf(name,"V792_%i_%i",0,i);

    // Delete old histograms, if we already have them
    TH1D *old = (TH1D*)gDirectory->Get(name);
    if (old){
      delete old;
    }


    // Create new histograms
    
    sprintf(title,"V785 Pulse Height Histogram for channel=%i",i);	
    
    TH1D *tmp = new TH1D(name,title,4200,0,4200);
    tmp->SetYTitle("Number of entries");
    tmp->SetXTitle("Pulse Height (ADC counts)");
    push_back(tmp);
  }

}



  
/// Update the histograms for this canvas.
void TV792Histograms::UpdateHistograms(TDataContainer& dataContainer){

  struct timeval start,stop;
 gettimeofday(&start,NULL);
 //printf ("About to sert rtrequest: %f\n",start.tv_sec
 //    + 0.000001*start.tv_usec); 

  TV792NData *data = dataContainer.GetEventData<TV792NData>("ADC0");
  if(!data) return;

  /// Get the Vector of ADC Measurements.
  std::vector<VADCNMeasurement> measurements = data->GetMeasurements();
  for(unsigned int i = 0; i < measurements.size(); i++){ // loop over measurements
	
    int chan = measurements[i].GetChannel();
    uint32_t adc = measurements[i].GetMeasurement();

    //    std::cout << i << " under/over: " << measurements[i].IsUnderThreshold() << " " << measurements[i].IsOverFlow() << " " << measurements[i].GetMeasurement()<< std::endl;


    if(chan >= 0 && chan < Nchannels)
      GetHistogram(chan)->Fill(adc);


  }

}



/// Take actions at begin run
void TV792Histograms::BeginRun(int transition,int run,int time){

  CreateHistograms();

}

/// Take actions at end run  
void TV792Histograms::EndRun(int transition,int run,int time){

}