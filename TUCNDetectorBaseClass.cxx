#include "TUCNDetectorBaseClass.hxx"


TUCNDetectorBaseClass::TUCNDetectorBaseClass(bool isOffline){
  
  fHits = TUCNHitCollection();
  fIsLi6 = false;
  fIsOffline = isOffline;
  
  fDetectorCharge = new TUCNDetectorCharge(fIsLi6);
  fDetectorCharge->DisableAutoUpdate();

  fRateVsTime = new TUCNRateVsTime(fIsLi6,isOffline);
  fRateVsTime->DisableAutoUpdate();

  fCycleStartTime = 0.0;

  char htitle[250], hname[250];
  if(fIsLi6){
    sprintf(hname,"hitsinsequence_li6");
    sprintf(htitle,"Cumulative Hits Within Cycle: Li-6");
  }else{
    sprintf(hname,"hitsinsequence_he3");
    sprintf(htitle,"Cumulative Hits Within Cycle: He-3");
  }
    
  fHitsInCycle = new TH1D(hname,htitle,500,0,100);
  fHitsInCycle->SetYTitle("Counts");
  fHitsInCycle->SetXTitle("Time since start of sequence (sec)");
 //  TH1D* GetHitsPerCycle(){return fHitsPerCycle;}
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
  // Use the sequence bank to see when a new run starts:
  bool sequence_started = false;
  TGenericData *data = dataContainer.GetEventData<TGenericData>("SEQN");
  if(data){
    if(data->GetData32()[1] & 2){
      sequence_started = true;
      //std::cout << "New sequence started.\n";
      double tmp = ((double)dataContainer.GetMidasData().GetTimeStamp())
		+ ((double)data->GetData32()[0])/1000.0;
      //if(fCycleStartTime != 0.0)
      //std::cout << "Seconds since last sequence started: " << tmp - fCycleStartTime << std::endl;
      fCycleStartTime = tmp;
    }
  }


  // Fill the Cumulative "Hits in " histogram
  for(unsigned int j = 0; j < fHits.size(); j++){ // loop over measurements
    
    double hittime = (int)fHits[j].time;
    double time_in_cycle = hittime - fCycleStartTime;
    fHitsInCycle->Fill(time_in_cycle);
  }
  
  
  
  
}
