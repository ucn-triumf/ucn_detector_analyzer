#include "TLi6Detector.hxx"
#include "TV792NData.hxx"


TLi6Detector::TLi6Detector(bool isOffline):TUCNDetectorBaseClass(isOffline,true){
  
  std::cout << "Li6 constructor : " << isOffline << std::endl;  
  
}


void TLi6Detector::GetHits(TDataContainer& dataContainer){

  fHits = TUCNHitCollection();
  int timestamp = dataContainer.GetMidasData().GetTimeStamp();
  fHits.eventTime = timestamp;
  
  TGenericData *data = dataContainer.GetEventData<TGenericData>("W200");
  if(!data) return;

  TUCNHit hit = TUCNHit();
  hit.time = (double)timestamp;
  hit.channel = 0;
  fHits.push_back(hit);
  


}
