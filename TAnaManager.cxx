#include "TAnaManager.hxx"
#include "TV1720RawData.h"


TAnaManager::TAnaManager(bool isOffline, bool saveTree){

  fHe3CountsInSequence = new THe3CountsInSequence();
  fHe3CountsInSequence->DisableAutoUpdate();
  
  fHe3Detector = new THe3Detector(isOffline,saveTree);
  fLi6Detector = new TLi6Detector(isOffline,saveTree);

  if(saveTree){
    std::cout << "Creating EPICS tree" << std::endl;
    fSourceEpicsTree = new TUCNSourceEpicsTree();
    fBeamlineEpicsTree = new TUCNBeamlineEpicsTree();
  }else{
    fSourceEpicsTree = 0;
    fBeamlineEpicsTree = 0;
  }
  
};
      
bool insequence = 0;

int TAnaManager::ProcessMidasEvent(TDataContainer& dataContainer){
  
  //if(fV1720Waveform && 0)  fV1720Waveform->UpdateHistograms(dataContainer);
  
  fHe3Detector->ProcessMidasEvent(dataContainer);
  fLi6Detector->ProcessMidasEvent(dataContainer);
  if(fSourceEpicsTree){   
    fSourceEpicsTree->FillTree(dataContainer);
    fBeamlineEpicsTree->FillTree(dataContainer);   
  }
  return 1;
}





THe3CountsInSequence* TAnaManager::GetHe3CountsHistograms(){return fHe3CountsInSequence;}

