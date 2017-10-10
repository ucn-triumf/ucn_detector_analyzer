#include "TUCNDetectorBaseClass.hxx"


TUCNDetectorBaseClass::TUCNDetectorBaseClass(bool isOffline){
  
  fHits = TUCNHitCollection();
  fIsLi6 = false;
  fIsOffline = isOffline;
  
  fDetectorCharge = new TUCNDetectorCharge(fIsLi6);
  fDetectorCharge->DisableAutoUpdate();

  fRateVsTime = new TUCNRateVsTime(fIsLi6,isOffline);
  fRateVsTime->DisableAutoUpdate();

}


/// Update the histograms for this canvas.
void TUCNDetectorBaseClass::ProcessMidasEvent(TDataContainer& dataContainer){

  // Get the set of hits
  GetHits(dataContainer);

  // Fill the charge spectrum
  fDetectorCharge->UpdateHistograms(fHits);

  // Fill the rate vs time histograms
  fRateVsTime->UpdateHistograms(fHits);

  // Fill out a bunch of histograms for UCN hit rate with respect to the irradiation sequence.
  
  
  
}
