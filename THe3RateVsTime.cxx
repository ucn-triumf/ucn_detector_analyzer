#include "THe3RateVsTime.h"

#include "TV792Data.hxx"
#include "TDirectory.h"

const int Nchannels = 6;
 #include <sys/time.h>

int timescale[6] = {300,600,1200,2400,4800,9600};

/// Reset the histograms for this canvas
THe3RateVsTime::THe3RateVsTime(){  
  
  CreateHistograms();
}


void THe3RateVsTime::CreateHistograms(){
  

  // Otherwise make histograms
  clear();
  
  for(int i = 0; i < Nchannels; i++){ // loop over channels    

    char name[100];
    char title[100];
    sprintf(name,"He3_Rate_%i_%i",0,i);

    // Delete old histograms, if we already have them
    TH1D *old = (TH1D*)gDirectory->Get(name);
    if (old){
      delete old;
    }


    // Create new histograms
    
    sprintf(title,"He3 UCN Rate vs Time (%i seconds)",timescale[i]);	
    
    TH1D *tmp = new TH1D(name,title,timescale[i]+100,-timescale[i],100);
    tmp->SetXTitle("Time since now (sec)");
    tmp->SetYTitle("UCN Rate");
    // tmp->SetOptStat(0);;
    push_back(tmp);

std::vector<std::pair<int, double> > tmp2;
fRateVsTime.push_back(tmp2);
   }

}


  
/// Update the histograms for this canvas.
void THe3RateVsTime::UpdateHistograms(TDataContainer& dataContainer){

  struct timeval start,stop;
  gettimeofday(&start,NULL);

  // Update the histogram every event... even if not a V785 event...
  int timestamp = dataContainer.GetMidasData().GetTimeStamp();

  for(int j = 0; j < Nchannels; j++){
    for(int i = 0; i < GetHistogram(j)->GetXaxis()->GetNbins(); i++){
      GetHistogram(j)->SetBinContent(i+1,0);
    }

    for(int i = 0; i < fRateVsTime[j].size(); i++){
      int timediff = fRateVsTime[j][i].first - timestamp;
      GetHistogram(j)->SetBinContent(timediff+timescale[j],fRateVsTime[j][i].second );
    }

    // See if we have a new timestamp to deal with...
    if(fRateVsTime[j].size() == 0 || timestamp > fRateVsTime[j].back().first){

      // Add new entry at back
      fRateVsTime[j].push_back( std::pair<int,double>(timestamp,0));
    // maybe start erasing first entry...
      if(fRateVsTime[j].size() > timescale[j]){
//std::cout << "erasing!! " << std::endl;
        fRateVsTime[j].erase(fRateVsTime[j].begin());
}
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
    }



  }

  if(havehit){    
    // Find the right entry and update...
for(int j = 0; j < 6; j++){
    for(int i = fRateVsTime[j].size()-1; i >= 0; i--){      
      if(fRateVsTime[j][i].first == timestamp){
	fRateVsTime[j][i].second = fRateVsTime[j][i].second + 1.0;
	break;
      }
    }
}}

}



/// Take actions at begin run
void THe3RateVsTime::BeginRun(int transition,int run,int time){

  CreateHistograms();

}

/// Take actions at end run  
void THe3RateVsTime::EndRun(int transition,int run,int time){

}
