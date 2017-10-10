#include "TUCNRateVsTime.h"

#include "TDirectory.h"

const int Nchannels = 8;
 #include <sys/time.h>

int timescale[8] = {300,600,1200,2400,4800,9600,19200,38400};

/// Reset the histograms for this canvas
TUCNRateVsTime::TUCNRateVsTime(bool isOffline, bool isLi6){  

  fIsLi6 = isLi6;
  fIsOffline = isOffline;
  
  CreateHistograms();
}


void TUCNRateVsTime::CreateHistograms(){
  
  // Otherwise make histograms
  clear();
  
  for(int i = 0; i < Nchannels; i++){ // loop over channels    

    char name[100];
    char title[100];
    if(fIsLi6) sprintf(name,"Li6_Rate_%i_%i",0,i);
    else sprintf(name,"He3_Rate_%i_%i",0,i);

    // Delete old histograms, if we already have them
    TH1D *old = (TH1D*)gDirectory->Get(name);
    if (old){
      delete old;
    }


    // Create new histograms
    
    if(fIsLi6) sprintf(title,"Li-6 UCN Rate vs Time (%i minutes)",timescale[i]/60);
    else sprintf(title,"He3 UCN Rate vs Time (%i minutes)",timescale[i]/60);	
    
    TH1D *tmp = new TH1D(name,title,timescale[0]*1.1,-timescale[i],timescale[i]*0.1);
    if(fIsOffline) tmp->SetXTitle("Time since last event (sec)");
    else tmp->SetXTitle("Time since now (sec)");
    tmp->SetYTitle("UCN Rate (cts/sec)");
    // tmp->SetOptStat(0);;
    push_back(tmp);

    fRateVsTime = std::vector<std::pair<int, double> >();
      
   }

}

int lastTimestamp = 0;
void TUCNRateVsTime::UpdateHistograms(TUCNHitCollection & hits){

  struct timeval start,stop;
  gettimeofday(&start,NULL);
  
  // Update the histogram every event... but only if the event timestamp is different from last event
  int timestamp = hits.eventTime;

  if(lastTimestamp != timestamp){

    for(int j = 0; j < Nchannels; j++){
      GetHistogram(j)->Reset();

      for(int i = 0; i < fRateVsTime.size(); i++){
        std::pair<int, double> entry = fRateVsTime[i];
        if( !entry.second) continue;
        int timediff = (entry.first - timestamp);
        for(int k = 0; k < entry.second; k++){
          GetHistogram(j)->Fill(timediff);
        }
      }
    }
  }

  // Check if the rateVsTime array needs new entry.
  if(fRateVsTime.size() == 0 || timestamp > fRateVsTime.back().first){
        
    // Add new entry at back
    fRateVsTime.push_back( std::pair<int,double>(timestamp,0));

    // maybe start erasing first entry...
    if(fRateVsTime.size() > timescale[Nchannels-1]){
      fRateVsTime.erase(fRateVsTime.begin());
    }
  }
  lastTimestamp = timestamp;


  if (hits.size()){
    std::cout << "hits " << hits.size() << std::endl;
  }
  for(unsigned int j = 0; j < hits.size(); j++){ // loop over measurements
	
    int hittime = (int)hits[j].time;
    
    // Find the right entry and update...
    for(int i = fRateVsTime.size()-1; i >= 0; i--){      
      if(hittime == fRateVsTime[i].first){
        fRateVsTime[i].second = fRateVsTime[i].second + 1.0;
        std::cout << "Found hit " << hittime << " "  << fRateVsTime[i].first
                  << " " << fRateVsTime[i].second << std::endl;
        break;
      }
    }    
  }



}



/// Take actions at begin run
void TUCNRateVsTime::BeginRun(int transition,int run,int time){

  CreateHistograms();

}

/// Take actions at end run  
void TUCNRateVsTime::EndRun(int transition,int run,int time){

}
