#include "THe3CountsInSequence.h"

#include "TV792Data.hxx"
#include "TGenericData.hxx"
#include "TDirectory.h"

const int Nchannels = 1;
 #include <sys/time.h>


/// Reset the histograms for this canvas
THe3CountsInSequence::THe3CountsInSequence(){  
  
  CreateHistograms();
}


void THe3CountsInSequence::CreateHistograms(){
  

  // Otherwise make histograms
  clear();
  
  for(int i = 0; i < Nchannels; i++){ // loop over channels    

    char name[100];
    char title[100];
    sprintf(name,"He3_Counts_%i_%i",0,i);

    // Delete old histograms, if we already have them
    TH1D *old = (TH1D*)gDirectory->Get(name);
    if (old){
      delete old;
    }


    // Create new histograms
    
    sprintf(title,"He3 UCN Rate In Sequence");	
    
    TH1D *tmp = new TH1D(name,title,50,0,50);
    tmp->SetXTitle("Time after Sequence Start (s)");
    tmp->SetYTitle("UCN Counts");
    // tmp->SetOptStat(0);;
    push_back(tmp);

   }

  insequence = -1; // start with non-value...
  startSequence = 0;
}


  
/// Update the histograms for this canvas.
void THe3CountsInSequence::UpdateHistograms(TDataContainer& dataContainer){

  // Update the histogram every event... 
  int timestamp = dataContainer.GetMidasData().GetTimeStamp();

  // Check if we are in sequence
  TGenericData *sdata = dataContainer.GetEventData<TGenericData>("SEQN");
  if(sdata){
    if(sdata->GetData32()[1]){
      if(insequence == 0){
	std::cout << "Started new sequence." << std::endl;
	startSequence = timestamp;
      }
      insequence = 1;
    }
    else{
      insequence = 0;
    }
  }


  TV792Data *data = dataContainer.GetEventData<TV792Data>("ADC0");
  if(!data) return;


  /// Get the Vector of ADC Measurements.
  std::vector<VADCMeasurement> measurements = data->GetMeasurements();

  bool havehit = false;
  for(unsigned int i = 0; i < measurements.size(); i++){ // loop over measurements
	
    int chan = measurements[i].GetChannel();
    uint32_t adc = measurements[i].GetMeasurement();

    if(chan == 0){
      havehit = true;
      GetHistogram(0)->Fill(timestamp-startSequence);
    }
  }


}



/// Take actions at begin run
void THe3CountsInSequence::BeginRun(int transition,int run,int time){

  CreateHistograms();

}

/// Take actions at end run  
void THe3CountsInSequence::EndRun(int transition,int run,int time){

}
