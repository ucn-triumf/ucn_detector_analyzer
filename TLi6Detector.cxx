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
    
    std::cout << hit.clockTime << " " << hit.channel << " " << hit.chargeShort <<std::endl;
    
    fHits.push_back(hit);

  }
  //double clocktime;
  //double chargeLong; // This is just charge for He-3
  //double chargeShort; // This is 0 for He-3
  //double psd; // This is 0 for He-3

  


}
