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
  

  // Loop over two boards
  for(int i = 0; i < NDPPBOARDS; i++){
 
    fDPP[i].ClearWaves();
    char bankname[100];
    sprintf(bankname,"W20%i",i);
    TGenericData *data = dataContainer.GetEventData<TGenericData>(bankname);
    if(!data) continue;
        
    // Decode the data
    fDPP[i].Init(((char*)data->GetData32()));

    if(fDPP[i].GetNEvents() != 1) std::cout <<"WARNING: more than one hit per W200 bank. Not expected by decoder... " << std::endl;
    
    // Fill the UCN hit structures...
    for (int ichan = 0; ichan < PSD_MAXNCHAN;ichan++) {
      for (int isubev = 0;isubev< fDPP[i].GetNWaves(ichan) ;isubev++) {
	  DPP_Bank_Out_t *out  = fDPP[i].GetPSD( isubev, ichan );
	  
	  if ( out==NULL) continue;
	  
	  TUCNHit hit = TUCNHit();
	  hit.time = (double)timestamp;
	  hit.clockTime = out->TimeTag;
	  hit.channel = out->Channel;
	  hit.chargeShort = out->ChargeShort;
	  hit.chargeLong = out->ChargeLong;
	  hit.baseline = out->Baseline;
	  
	  fHits.push_back(hit);
	  static long int chan6_time;
	  if(hit.channel == 6) chan6_time = hit.clockTime;

	  if(hit.channel != 0) std::cout << "Board " << i << " channel " << hit.channel << "has hit at time " 
					 << hit.clockTime << " diff " << hit.clockTime-chan6_time << std::endl;
      }
    }
  }

}


void TLi6Detector::FillSpecificPlots(){

  //std::cout << "
  fBaselines->UpdateHistograms(fHits);

}
