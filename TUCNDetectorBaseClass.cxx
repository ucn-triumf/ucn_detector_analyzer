#include "TUCNDetectorBaseClass.hxx"
#include "TV792NData.hxx"


TUCNDetectorBaseClass::TUCNDetectorBaseClass(){
  
  fHits = std::vector<TUCNHit>();
  isLi6 = false;
  fDetectorCharge = new TUCNDetectorCharge(isLi6);

}


void TUCNDetectorBaseClass::GetHits(TDataContainer& dataContainer){

  fHits.clear();

  TV792NData *data = dataContainer.GetEventData<TV792NData>("ADC0");
  if(!data) return;

  /// Get the Vector of ADC Measurements.
  std::vector<VADCNMeasurement> measurements = data->GetMeasurements();
  for(unsigned int i = 0; i < measurements.size(); i++){ // loop over measurements	
    TUCNHit hit = TUCNHit();// = new TUCNHit();
    hit.channel = measurements[i].GetChannel();
    hit.chargeLong = measurements[i].GetMeasurement();
    fHits.push_back(hit);
  }


}

/// Update the histograms for this canvas.
void TUCNDetectorBaseClass::ProcessMidasEvent(TDataContainer& dataContainer){

  // Get the set of hits
  GetHits(dataContainer);

  // Fill the charge spectrum
  fDetectorCharge->UpdateHistograms(fHits);
  
}
