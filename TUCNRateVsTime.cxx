#include "TUCNRateVsTime.h"

#include "TDirectory.h"

const int Nchannels = 6;
 #include <sys/time.h>

int timescale[6] = {300,600,1200,2400,4800,9600};

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
    
    if(fIsLi6) sprintf(title,"Li-6 UCN Rate vs Time (%i seconds)",timescale[i]);
    else sprintf(title,"He3 UCN Rate vs Time (%i seconds)",timescale[i]);	
    
    TH1D *tmp = new TH1D(name,title,timescale[i]+100,-timescale[i],100);
    if(fIsOffline) tmp->SetXTitle("Time since last event (sec)");
    else tmp->SetXTitle("Time since now (sec)");
    tmp->SetYTitle("UCN Rate");
    // tmp->SetOptStat(0);;
    push_back(tmp);

    std::vector<std::pair<int, double> > tmp2;
    fRateVsTime.push_back(tmp2);
   }

}

int lastTimestamp = 0;
void TUCNRateVsTime::UpdateHistograms(TUCNHitCollection & hits){

  struct timeval start,stop;
  gettimeofday(&start,NULL);
  
  // Update the histogram every event... but only if the event timestamp is different from last event
  int timestamp = hits.eventTime;
  
  if(lastTimestamp != timestamp){

    //    std::cout << "Timestamp " << timestamp << std::endl;
    for(int j = 0; j < Nchannels; j++){
      for(int i = 0; i < GetHistogram(j)->GetXaxis()->GetNbins(); i++){
        GetHistogram(j)->SetBinContent(i+1,0);
      }
      
      for(int i = 0; i < fRateVsTime[j].size(); i++){
        int timediff = fRateVsTime[j][i].first - timestamp;
        GetHistogram(j)->SetBinContent(timediff+timescale[j],fRateVsTime[j][i].second );
      }
      
      // See if we have a new timestamp to deal with...
      //      std::cout << " " << 
      if(fRateVsTime[j].size() == 0 || timestamp >= (fRateVsTime[j].back().first + (2^j))){
        
        // Add new entry at back
        fRateVsTime[j].push_back( std::pair<int,double>(timestamp,0));
        
        // maybe start erasing first entry...
        if(fRateVsTime[j].size() > timescale[0]){
          //          std::cout << "erasing!! " << fRateVsTime[j].begin()->first << " " << fRateVsTime[j].size() << " " << timescale[0] << std::endl;
          fRateVsTime[j].erase(fRateVsTime[j].begin());
        }
      }
    }
  }
  lastTimestamp = timestamp;

  for(int j = 0; j < 6; j++){
    int lastTime = 99999999;
    for(int i = fRateVsTime[j].size()-1; i >= 0; i--){        
      if( i > fRateVsTime[j].size()-10)
        std::cout << i << " " << j << " Time " << fRateVsTime[j][i].first << std::endl;
    }

  }

      
  bool havehit = false;
  for(unsigned int i = 0; i < hits.size(); i++){ // loop over measurements
	
    int chan = hits[i].channel;
    uint32_t adc = hits[i].chargeLong;
    
    havehit = true;
    
  }

  if(havehit)
    std::cout << "Have hit!" << timestamp << std::endl;
  
  if(havehit){    
    // Find the right entry and update...
    for(int j = 0; j < 6; j++){
      int lastTime = 99999999;
      for(int i = fRateVsTime[j].size()-1; i >= 0; i--){      
        if(timestamp >= fRateVsTime[j][i].first && timestamp < lastTime ){
          fRateVsTime[j][i].second = fRateVsTime[j][i].second + 1.0;
          std::cout << "Adding hit " << i << " " << j << " " << fRateVsTime[j][i].second << " " << fRateVsTime[j][i].first << " "
                    << timestamp << " " << lastTime << std::endl;
          
          break;
        }
        lastTime = fRateVsTime[j][i].first;
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
