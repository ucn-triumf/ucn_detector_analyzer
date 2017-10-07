#include "TUCNDetectorBaseClass.hxx"


TUCNDetectorBaseClass::TUCNDetectorBaseClass(){
  
  fHits = std::vector<TUCNHit>();
  isLi6 = false;
  fDetectorCharge = new TUCNDetectorCharge(isLi6);

}


/// Update the histograms for this canvas.
void TUCNDetectorBaseClass::ProcessMidasEvent(TDataContainer& dataContainer){

  // Get the set of hits
  GetHits(dataContainer);

  // Fill the charge spectrum
  fDetectorCharge->UpdateHistograms(fHits);
  
}
