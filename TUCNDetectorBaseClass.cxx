#include "TUCNDetectorBaseClass.hxx"


TUCNDetectorBaseClass::TUCNDetectorBaseClass(bool isOffline, bool isLi6, bool saveTree){
  
  fHits = TUCNHitCollection();
  fNonHits = TUCNHitCollection();
  fBackgroundHits = TUCNHitCollection();
  fIsLi6 = isLi6;
  fIsOffline = isOffline;

  fUsePCTime = false;
    
  fDetectorCharge = new TUCNDetectorCharge(fIsLi6);
  fDetectorCharge->DisableAutoUpdate();

  fRateVsTime = new TUCNRateVsTime(fIsLi6,isOffline);
  fRateVsTime->DisableAutoUpdate();

  fCycleStartTime = 0.0;
  fLastCycleStartTime  = 0.0;
  fTotalHitsCycle = 0;

  char htitle[250], hname[250];
  if(fIsLi6){
    sprintf(hname,"hitsinsequence_li6");
    sprintf(htitle,"Hits Within Current Cycle: Li-6");
  }else{
    sprintf(hname,"hitsinsequence_he3");
    sprintf(htitle,"Hits Within Current Cycle: He-3");
  }
    
  fHitsInCycle = new TH1D(hname,htitle,110,-10,100);
  fHitsInCycle->SetYTitle("Counts");
  fHitsInCycle->SetXTitle("Time since start of sequence (sec)");

  if(fIsLi6){
    sprintf(hname,"hitsinsequencecumul_li6");
    sprintf(htitle,"Cumulative Hits Within Cycles: Li-6");
  }else{
    sprintf(hname,"hitsinsequencecumul_he3");
    sprintf(htitle,"Cumulative Hits Within Cycles: He-3");
  }
    
  fHitsInCycleCumul = new TH1D(hname,htitle,550,-10,100);
  fHitsInCycleCumul->SetYTitle("Counts");
  fHitsInCycleCumul->SetXTitle("Time since start of sequence (sec)");

  fHitsPerCycle = new TGraph();
  if(fIsLi6){
    
    if(isOffline)
      fHitsPerCycle->SetTitle("UCN Counts per Cycle: Li-6; Time since last event (sec) ; UCN hits in cycle");
    else
      fHitsPerCycle->SetTitle("UCN Counts per Cycle: Li-6; Time since now (sec) ; UCN hits in cycle");
  }else{
    if(isOffline)
      fHitsPerCycle->SetTitle("UCN Counts per Cycle: He-3; Time since last event (sec) ; UCN hits in cycle");
    else
      fHitsPerCycle->SetTitle("UCN Counts per Cycle: He-3; Time since now (sec) ; UCN hits in cycle");
      
  }  

  // Create trees, if requested
  fHitsTree = 0;
  if(saveTree){
    if(fIsLi6)
      fHitsTree = new TUCNHitsTree("Li-6");
    else
      fHitsTree = new TUCNHitsTree("He3");
  }

  std::cout << "Finished Detector Setup " << std::endl;

}

bool TUCNDetectorBaseClass::CheckForSequenceStartCrude(TDataContainer& dataContainer){

  // Use the sequence bank to see when a new run starts:
  TGenericData *data = dataContainer.GetEventData<TGenericData>("SEQN");

  if(data){
    if(data->GetData32()[1] & 2){
      fLastCycleStartTime = fCycleStartTime;
      double tmp = ((double)dataContainer.GetMidasData().GetTimeStamp());
      fCycleStartTime = tmp;
      return true;
    }
  }
  return false;
}


/// Update the histograms for this canvas.
void TUCNDetectorBaseClass::ProcessMidasEvent(TDataContainer& dataContainer){

  // Get the set of hits
  GetHits(dataContainer);

  // Fill the charge spectrum
  fDetectorCharge->UpdateHistograms(fHits);

  // Fill the rate vs time histograms
  fRateVsTime->UpdateHistograms(fHits);

  // Check for sequence start time
  bool sequence_started = false;
  if(UsePreciseSequenceTime()){
    sequence_started = CheckForSequenceStartPrecise(dataContainer);
  }else{
    sequence_started = CheckForSequenceStartCrude(dataContainer);
  }

  // Fill out a bunch of histograms for UCN hit rate with respect to the irradiation sequence.


  // Fill the Cumulative "Hits in Cycle" histogram
  for(unsigned int j = 0; j < fHits.size(); j++){ // loop over measurements   
    double hittime = fHits[j].time;
    double time_in_cycle = hittime - fCycleStartTime;
    fHitsInCycle->Fill(time_in_cycle);
    fHitsInCycleCumul->Fill(time_in_cycle);
    fTotalHitsCycle++;
  }
  
  // If new sequence started, update information for the cumulative sequence plot
  if(sequence_started && fLastCycleStartTime != 0.0){
    fHitsPerCycleVector.push_back(std::pair<double,double>(fLastCycleStartTime,fTotalHitsCycle));
    fTotalHitsCycle = 0;
    if(fHitsPerCycleVector.size() > 100){ // Save at most 100 cycles
      fHitsPerCycleVector.erase(fHitsPerCycleVector.begin());
    }
    // Reset the hits in cycle histogram
    fHitsInCycle->Reset();
  }
  
  fHitsPerCycle->Set(0);
  double maxCycle = 0;
  for(unsigned int i = 0; i < fHitsPerCycleVector.size(); i++){
    double dtime = fHitsPerCycleVector[i].first - ((double)dataContainer.GetMidasData().GetTimeStamp());
    //      fLastCycleStartTime;
    fHitsPerCycle->SetPoint(i,dtime,fHitsPerCycleVector[i].second);  
    if(fHitsPerCycleVector[i].second > maxCycle) maxCycle = fHitsPerCycleVector[i].second;
  }

  fHitsPerCycle->SetMinimum(0);
  fHitsPerCycle->SetMaximum(maxCycle*1.2);

  // Fill the tree, if requested;
  // save both the 'UCN hits' and the 'background hits'
  if(fHitsTree){
    fHitsTree->FillHits(fHits,1);
    fHitsTree->FillHits(fBackgroundHits,0);     
  }

  // Do detector specific plot filling
  FillSpecificPlots();
  
}
