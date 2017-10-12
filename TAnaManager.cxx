#include "TAnaManager.hxx"
#include "TV1720RawData.h"


TAnaManager::TAnaManager(bool isOffline){


  
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
  
	//if(fV1720Waveform && 0)  fV1720Waveform->UpdateHistograms(dataContainer);

    	fHe3Detector->ProcessMidasEvent(dataContainer);


        return 1;
}





THe3CountsInSequence* TAnaManager::GetHe3CountsHistograms(){return fHe3CountsInSequence;}

