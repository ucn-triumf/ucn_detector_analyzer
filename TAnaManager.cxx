#include "TAnaManager.hxx"
#include "TV1720RawData.h"


TAnaManager::TAnaManager(bool isOffline){

  fV792Histogram = 0;
#ifdef USE_V792
  fV792Histogram = new TV792Histograms();
  fV792Histogram->DisableAutoUpdate();  // disable auto-update.  Update histo in AnaManager.
#endif

  
  //fV1720Waveform = 0;
  //#ifdef USE_V1720
  
  //  fV1720Waveform = new TV1720Waveform();
  //	fV1720Waveform->DisableAutoUpdate();  // disable auto-update.  Update histo in AnaManager.
  //#endif
  
  //  fHe3RateVsTime = 0;
  // fHe3RateVsTime = new THe3RateVsTime();
  //fHe3RateVsTime->DisableAutoUpdate();
  
  fHe3CountsInSequence = new THe3CountsInSequence();
  fHe3CountsInSequence->DisableAutoUpdate();
  
  
  fHe3Detector = new THe3Detector(isOffline);


};
      
bool insequence = 0;

int TAnaManager::ProcessMidasEvent(TDataContainer& dataContainer){
  
  // Check if we are in sequence
  TGenericData *data = dataContainer.GetEventData<TGenericData>("SEQN");
  if(data){
    if(data->GetData32()[1]){
      if(insequence == 0){
	std::cout << "Started new sequence." << std::endl;
      }
      insequence = 1;
    }
    else{
      insequence = 0;
    }
  }
  //
  //	if(fV792Histogram) fV792Histogram->UpdateHistograms(dataContainer); 
	//std::cout << "Analyzer " << std::endl;
	//if(fV1720Waveform && 0)  fV1720Waveform->UpdateHistograms(dataContainer);
  //	if(fHe3RateVsTime) fHe3RateVsTime->UpdateHistograms(dataContainer);

  //	fHe3CountsInSequence->UpdateHistograms(dataContainer);
	//std::cout << "Finished " << std::endl;
  //
    	fHe3Detector->ProcessMidasEvent(dataContainer);


        return 1;
}



bool TAnaManager::HaveV792Histograms(){
	if(fV792Histogram) return true; 
	return false;
}

//bool TAnaManager::HaveV1720Histograms(){
//	if(fV1720Waveform) return true; 
//	return false;
//};

//bool TAnaManager::HaveHe3RateHistograms(){
//	if(fHe3RateVsTime) return true; 
//	return false;
//};

TV792Histograms* TAnaManager::GetV792Histograms() {return fV792Histogram;}

//TV1720Waveform* TAnaManager::GetV1720Histograms(){return fV1720Waveform;}

//THe3RateVsTime* TAnaManager::GetHe3RateHistograms(){return fHe3RateVsTime;}

THe3CountsInSequence* TAnaManager::GetHe3CountsHistograms(){return fHe3CountsInSequence;}

