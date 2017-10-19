#include "TLi6Detector.hxx"
#include "TV792NData.hxx"


const int Nchannels = 16;
#include <sys/time.h>

/// Reset the histograms for this canvas
TV1720Baselines::TV1720Baselines(){  
  CreateHistograms();
}


void TV1720Baselines::CreateHistograms(){
  

  // Otherwise make histograms
  clear();
  
  for(int i = 0; i < Nchannels; i++){ // loop over channels    

    char name[100];
    char title[100];
    sprintf(name,"v1720_baselines_%i",i);

    // Delete old histograms, if we already have them
    TH1D *old = (TH1D*)gDirectory->Get(name);
    if (old){
      delete old;
    }


    // Create new histograms
    
    sprintf(title,"V1720 Baseline Histogram for channel=%i",i);
    TH1D *tmp = new TH1D(name,title,4096,0,4096);
      
    tmp->SetYTitle("Number of entries");
    tmp->SetXTitle("Baseline (ADC counts)");
    
    push_back(tmp);
  }

}


/// Fill the charge spectrum with the set of ucn hits
void  TV1720Baselines::UpdateHistograms(TUCNHitCollection & hits){

  for(unsigned int i = 0; i < hits.size(); i++){
    int channel = hits[i].channel;
    int baseline = hits[i].baseline;
    
    if(channel >= 0 && channel < size())
      GetHistogram(channel)->Fill(baseline);
  }
}


/// Take actions at begin run
void TV1720Baselines::BeginRun(int transition,int run,int time){
  CreateHistograms();
}

/// Take actions at end run  
void TV1720Baselines::EndRun(int transition,int run,int time){
}



TLi6Detector::TLi6Detector(bool isOffline):TUCNDetectorBaseClass(isOffline,true){
  
  std::cout << "Li6 constructor : " << isOffline << std::endl;  

  fBaselines = new TV1720Baselines();
  fBaselines->DisableAutoUpdate();
  
}


void TLi6Detector::GetHits(TDataContainer& dataContainer){

  fHits = TUCNHitCollection();
  int timestamp = dataContainer.GetMidasData().GetTimeStamp();
  fHits.eventTime = timestamp;
  
  TGenericData *data = dataContainer.GetEventData<TGenericData>("W200");
  if(!data) return;

  int nhits = data->GetData32()[0];
  if(nhits != 1) std::cout <<"WARNING: more than one hit per W200 bank. Not expected by decoder... " << std::endl;

  int startHitPtr = 1;
  for(int i = 0; i < nhits; i++){

    
    TUCNHit hit = TUCNHit();
    
    hit.time = (double)timestamp;
    hit.clockTime = data->GetData32()[startHitPtr];
    hit.channel = data->GetData32()[startHitPtr+2] & 0xffff;
    hit.chargeShort = (data->GetData32()[startHitPtr+2] & 0xffff0000) >> 16;
    hit.chargeLong = data->GetData32()[startHitPtr+3] & 0xffff;
    hit.baseline = (data->GetData32()[startHitPtr+3] & 0xffff0000) >> 16;
    
    //std::cout << hit.clockTime << " " << hit.channel << " " << hit.chargeShort <<std::endl;
    
    fHits.push_back(hit);

  }
  //double clocktime;
  //double chargeLong; // This is just charge for He-3
  //double chargeShort; // This is 0 for He-3
  //double psd; // This is 0 for He-3

  


}


void TLi6Detector::FillSpecificPlots(){

  //std::cout << "
  fBaselines->UpdateHistograms(fHits);

}
