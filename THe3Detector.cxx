#include "THe3Detector.hxx"
#include "TV792NData.hxx"


THe3Detector::THe3Detector(bool isOffline):TUCNDetectorBaseClass(isOffline){
  
  
  
}


void THe3Detector::GetHits(TDataContainer& dataContainer){

  fHits = TUCNHitCollection();
  int timestamp = dataContainer.GetMidasData().GetTimeStamp();
  fHits.eventTime = timestamp;
  
  TV792NData *data = dataContainer.GetEventData<TV792NData>("ADC0");
  if(!data) return;

  
  /// Get the Vector of ADC Measurements.
  std::vector<VADCNMeasurement> measurements = data->GetMeasurements();
  for(unsigned int i = 0; i < measurements.size(); i++){ // loop over measurements	
    TUCNHit hit = TUCNHit();// = new TUCNHit();
    hit.time = (double)timestamp;
    hit.channel = measurements[i].GetChannel();
    hit.chargeLong = measurements[i].GetMeasurement();
    fHits.push_back(hit);
  }


}
