#include "TLi6Detector.hxx"
#include "TV792NData.hxx"


const int Nchannels = 16;
#include <sys/time.h>

// Keep track of which V1720 have UCN hits and which have monitoring hits
const bool ucn_channels[16] = { true,  true,  true,  true,
                            true,  true,  true, false,
                            true, false, false, false,
                            false, false, false, false};
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



TLi6Detector::TLi6Detector(bool isOffline, bool saveTree):TUCNDetectorBaseClass(isOffline,true,saveTree){
  
  std::cout << "Li6 constructor : " << isOffline << std::endl;  

  fBaselines = new TV1720Baselines();
  fBaselines->DisableAutoUpdate();

  lastClockTime[0] = 0; lastClockTime[1] = 0;
  initialUnixTime = -1;
  numberRollOvers[0] = 0; numberRollOvers[1] = 0;
  initialClockTime[0] = 0; initialClockTime[1] = 0;

  fSequenceLength = new TH1F("sequencelength_v1720","Sequence Length (by V1720)",1200,0,300);
  fSequenceLength->SetXTitle("Sequence Length (sec)");
  fDelayTime = new TH1F("delaytime_v1720","Delay Time (by V1720)",1200,0,300);
  fDelayTime->SetXTitle("Delay Time (sec)");
  fValveOpenTime = new TH1F("valveopentime_v1720","Valve Open Time (by V1720)",1200,0,300);
  fValveOpenTime->SetXTitle("Valve Open Time (sec)");
 
  fEndOfIrradiationTime = 0;
  fUCNValveOpenTime = 0;
  fUCNValveCloseTime = 0;
 
  // Preliminary threshold for Qlong and PSD
  fPSDThreshold = 0.2;
  fQLongThreshold = 3000.0;

}

void TLi6Detector::BeginRun(int transition,int run,int time){
  lastClockTime[0] = 0; lastClockTime[1] = 0;
  initialUnixTime = -1;
  numberRollOvers[0] = 0; numberRollOvers[1] = 0;
  initialClockTime[0] = 0; initialClockTime[1] = 0;
  initClockSet[0] = false; initClockSet[1] = false; 
}

/// Check for clock roll-overs.  Check separately for both V1720s.
/// First hits for other channels will be nonsense, until first PPS event seen.
void TLi6Detector::CheckClockRollover(int board, TUCNHit hit, int timestamp){

  // Check for roll-over
  if( lastClockTime[board] > hit.clockTime && lastClockTime[board] > 0xd0000000 && hit.clockTime < 0x20000000){
    if(hit.clockTime % 100 == 0)
      std::cout << "Found roll-over: " << std::hex << lastClockTime[board] << " "<< hit.clockTime << std::endl;
    numberRollOvers[board]++;
  }              
  lastClockTime[board] = hit.clockTime;  

  // We check the initial clock time only with the PPS signals.
  // These are channel 0-7 and 1-4...
  if(!((board == 0 && hit.channel == 7) || (board == 1 && hit.channel == 12))) return;


  // Figure out precise unixTime from clock
  
  // Save the unix time for the first event we found
  if(initialUnixTime < 0){
    initialUnixTime = (double) timestamp;
    std::cout << "Set initial time: " << initialUnixTime << " " << hit.channel << " " << board << std::endl;
  }

  if(!initClockSet[board]){
    initialClockTime[board] = hit.clockTime;
    initClockSet[board] = true;
    std::cout << "Set initial clock set: " << initialClockTime[board] << " " << hit.channel << " " << board << std::endl;
  }
  
}



void TLi6Detector::GetHits(TDataContainer& dataContainer){

  fHits = TUCNHitCollection();
  fNonHits = TUCNHitCollection();
  fBackgroundHits = TUCNHitCollection();
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

    // Fill the UCN hit structures...
    for (int ichan = 0; ichan < PSD_MAXNCHAN;ichan++) {
      for (int isubev = 0;isubev< fDPP[i].GetNWaves(ichan) ;isubev++) {
	  DPP_Bank_Out_t *out  = fDPP[i].GetPSD( isubev, ichan );
	  
	  if ( out==NULL) continue;
	  
	  TUCNHit hit = TUCNHit();
	  hit.time = (double)timestamp;
	  hit.clockTime = (out->TimeTag & 0xffffffff);
	  hit.channel = out->Channel + i*8;
	  hit.chargeShort = out->ChargeShort;
	  hit.chargeLong = out->ChargeLong;
	  hit.baseline = out->Baseline;


          // Check for roll overs of the V1720 clock
          CheckClockRollover(i,hit,timestamp);
          
          // Get the current precise time
          hit.preciseTime = initialUnixTime + ((double)numberRollOvers[i])*17.17986918 + ((double)(hit.clockTime - initialClockTime[i]))/((double)0xffffffff) * 17.17986918;

          // If requested, we will use this precise time for all the hit timing plots.
          if(UsePreciseSequenceTime()) hit.time = hit.preciseTime;
          
          if(hit.clockTime % 20000 == 0)
            std::cout << hit.channel << " "
                      << hit.preciseTime << " " <<  initialUnixTime << " " << hit.preciseTime -  initialUnixTime
                      << " " << timestamp - initialUnixTime
                      << " " << numberRollOvers[i] << " " << hit.clockTime << " "
                      << initialClockTime[i] << " " << ((double)(hit.clockTime - initialClockTime[i])) << std::endl;
          
	  static long int chan6_time;
	  if(hit.channel == 6) chan6_time = hit.clockTime;

          // Is this a real UCN hit or a monitoring hit?
          if(ucn_channels[hit.channel]){

	    hit.psd = 0;
	    if(hit.chargeLong != 0)
	      hit.psd = ((Float_t)(hit.chargeLong)-(Float_t)(hit.chargeShort))/((Float_t)(hit.chargeLong));
	    if(hit.psd > fPSDThreshold && hit.chargeLong > fQLongThreshold ){
	      fHits.push_back(hit);		
	    }else{
	      fBackgroundHits.push_back(hit);
	    }
          }else{
            fNonHits.push_back(hit);
          }


      }
    }
  }

}

// Get a more precise sequence start time from v1720 bank
// Sequence hits in V1720 are as follows:
// channel 15 (1-7) end of irradiation
// channel 14 (1-6) UCN valve open
// channel 13 (1-5) UCN valve close (currently doesn't work)
bool TLi6Detector::CheckForSequenceStartPrecise(TDataContainer& dataContainer){

  // Check if we had a hit on channel1-7 (15) indicating the start of a new sequence
  for(unsigned int j = 0; j < fNonHits.size(); j++){ // loop over measurements   

    if(fNonHits[j].channel == 15){ // end of irradiation
      fLastCycleStartTime = fCycleStartTime;
      fCycleStartTime = fNonHits[j].preciseTime;
      fSeqValveOpenTime = fCycleStartTime + fSeqDelayTime;
      fSeqValveCloseTime = fSeqValveOpenTime + fSeqOpenInterval;
      fEndOfIrradiationTime = fCycleStartTime;
      fSequenceLength->Fill(fCycleStartTime-fLastCycleStartTime);
      if(fNonHits[j].clockTime %100 == 0)
	std::cout << "Li-6 Sequence start: "  << fCycleStartTime <<  " " << fCycleStartTime-fLastCycleStartTime << " " << fEndOfIrradiationTime
		  << std::endl;
      return true;
    } 
    
    if(fNonHits[j].channel == 14){ // UCN valve open
      fUCNValveOpenTime = fNonHits[j].preciseTime;
      fDelayTime->Fill(fUCNValveOpenTime - fEndOfIrradiationTime);
      if(fNonHits[j].clockTime %100 == 0)
	std::cout << " Valve opens " << fUCNValveOpenTime - fEndOfIrradiationTime << " " << fEndOfIrradiationTime << std::endl;
    }
    
    if(fNonHits[j].channel == 13){ // UCN valve closed
      fUCNValveCloseTime = fNonHits[j].preciseTime;
      fValveOpenTime->Fill(fUCNValveCloseTime - fUCNValveOpenTime);
      //std::cout << " SDFDF " << fUCNValveCloseTime - fUCNValveOpenTime << std::endl;
    }

  }

  return false;
  
}



void TLi6Detector::FillSpecificPlots(){

  //std::cout << "
  fBaselines->UpdateHistograms(fHits);

}
