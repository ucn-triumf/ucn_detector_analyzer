#include "THe3Detector.hxx"
#include "TV792NData.hxx"
#include "TV1725DppPsdData.hxx"


THe3Detector::THe3Detector(bool isOffline, bool is3HEDET1, bool saveTree):TUCNDetectorBaseClass(isOffline,false,saveTree,is3HEDET1){
  
  std::cout << "He3 constructor : " << isOffline << std::endl;  
  initialUnixTime = -1;

}


void THe3Detector::GetHits(TDataContainer& dataContainer){

  fHits = TUCNHitCollection();
  fNonHits = TUCNHitCollection();
  fBackgroundHits = TUCNHitCollection();
  int timestamp = dataContainer.GetMidasData().GetTimeStamp();
  fHits.eventTime = timestamp;
  
#ifdef USING_V1725_READOUT_HE3

  TV1725DppPsdData *data = dataContainer.GetEventData<TV1725DppPsdData>("W500");
  if(!data) return;
    
  /// Get the Vector of ADC Measurements.
  std::vector<ChannelMeasurement> measurements = data->GetMeasurements();
  for(unsigned int i = 0; i < measurements.size(); i++){
    
    ChannelMeasurement meas = measurements[i];
    int ch = meas.GetChannel();

    // Use the first time synchronization pulse to set the initial unix time
    if(initialUnixTime < 0){
      if(ch == 11){	
	initialUnixTime = (double) timestamp;
	std::cout << "Set initial time: " << initialUnixTime << std::endl;
      }
    }

    double hittime = meas.GetExtendedTimeTag() * 0.000000004; // in seconds
    TUCNHit hit = TUCNHit();
    hit.time = initialUnixTime + hittime;
    hit.preciseTime = initialUnixTime + hittime;    
    hit.clockTime = meas.GetTimeTag();
    hit.channel = meas.GetChannel();
          
    // calculate pulse height and store in value...
    uint32_t min_value = 99999;
    int nsamples = meas.GetNSamples();
    
    for (int b = 0; b<nsamples; b++){
      if(meas.GetSample(b) < min_value) min_value = meas.GetSample(b);
    }
    int pulse_height = 14718 - (int)min_value;
    hit.chargeShort = pulse_height;
    hit.chargeLong  = pulse_height;


    if((fIs3HEDET1 && (ch == 13))||
       (!fIs3HEDET1 && (ch == 12))){

      if(hit.chargeShort > 1500){
	fHits.push_back(hit);
      }else
	fBackgroundHits.push_back(hit);
    }else if (ch == 10 || ch == 11){
      std::cout << "He-3 PH: " << hit.chargeShort << " " << ch << std::endl;
      
      fNonHits.push_back(hit);

    }

  }
  

#else 


  
  TV792NData *data = dataContainer.GetEventData<TV792NData>("ADC0");
  if(!data) return;
  
  /// Get the Vector of ADC Measurements.
  std::vector<VADCNMeasurement> measurements = data->GetMeasurements();
  for(unsigned int i = 0; i < measurements.size(); i++){ // loop over measurements	

    // Only channel 0 has UCN hits...
    if(fIs3HEDET1){
      if(measurements[i].GetChannel() != 0) continue;
    }else{
      if(measurements[i].GetChannel() != 1) continue;
    }
    
    TUCNHit hit = TUCNHit();// = new TUCNHit();
    hit.time = (double)timestamp;
    hit.channel = measurements[i].GetChannel();
    hit.chargeLong = measurements[i].GetMeasurement();
    hit.chargeShort = measurements[i].GetMeasurement();
    
    double threshold = 0;
    if(fIs3HEDET1){
      threshold = 300;
    }else{
      threshold = 400;
    }

    if(hit.chargeShort > threshold)
      fHits.push_back(hit);
    else
      fBackgroundHits.push_back(hit);
  }

#endif

}

// Get a more precise sequence start time from v1725 bank
bool THe3Detector::CheckForSequenceStartPrecise(TDataContainer& dataContainer){

  // Channel 10 for cycle start signal.
  for(unsigned int j = 0; j < fNonHits.size(); j++){ // loop over measurements   
    if(fNonHits[j].channel == 10){ // start of cycle
      fLastCycleStartTime = fCycleStartTime;
      fCycleStartTime = fNonHits[j].preciseTime;
      std::cout << "He-3 Cycle start: "  << fCycleStartTime <<  " " << fCycleStartTime-fLastCycleStartTime  << std::endl;
      return true;
    } 
  }

  return false;
}

