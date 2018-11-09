#include "THe3Detector.hxx"
#include "TV792NData.hxx"


THe3Detector::THe3Detector(bool isOffline, bool is3HEDET1, bool saveTree):TUCNDetectorBaseClass(isOffline,false,saveTree,is3HEDET1){
  
  std::cout << "He3 constructor : " << isOffline << std::endl;  
  
}


void THe3Detector::GetHits(TDataContainer& dataContainer){

  fHits = TUCNHitCollection();
  fNonHits = TUCNHitCollection();
  fBackgroundHits = TUCNHitCollection();

  int timestamp = dataContainer.GetMidasData().GetTimeStamp();
  fHits.eventTime = timestamp;
  
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


}
