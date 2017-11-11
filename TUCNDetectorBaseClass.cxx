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
  fTotalHitsCycleIntime = 0;
  fSeqValveOpenTime = 0.0;
  fSeqValveCloseTime = 0.0;
  fSeqDelayTime = 0.0;
  fSeqOpenInterval = 0.0;


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
    sprintf(hname,"hitsinsequence_li6_intime");
    sprintf(htitle,"In-time Hits Within Current Cycle: Li-6");
  }else{
    sprintf(hname,"hitsinsequence_he3_intime");
    sprintf(htitle,"In-time Hits Within Current Cycle: He-3");
  }
    
  fHitsInCycleIntime = new TH1D(hname,htitle,110,-10,100);
  fHitsInCycleIntime->SetYTitle("Counts");
  fHitsInCycleIntime->SetXTitle("Time since start of sequence (sec)");

  
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

  
  if(fIsLi6){
    sprintf(hname,"hitsinsequencecumul_li6_intime");
    sprintf(htitle,"In-time Cumulative Hits Within Cycles: Li-6");
  }else{
    sprintf(hname,"hitsinsequencecumul_he3_intime");
    sprintf(htitle,"In-time Cumulative Hits Within Cycles: He-3");
  }
    
  fHitsInCycleCumulIntime = new TH1D(hname,htitle,550,-10,100);
  fHitsInCycleCumulIntime->SetYTitle("Counts");
  fHitsInCycleCumulIntime->SetXTitle("Time since start of sequence (sec)");

  fHitsPerCycle = new TGraph();
  fHitsPerCycleIntime = new TGraph();
  if(fIsLi6){    
    if(isOffline){
      fHitsPerCycle->SetTitle("UCN Counts per Cycle: Li-6; Time since last event (sec) ; UCN hits in cycle");
      fHitsPerCycleIntime->SetTitle("UCN Counts per Cycle: Li-6; Time since last event (sec) ; UCN hits in cycle");
    }else{
      fHitsPerCycle->SetTitle("UCN Counts per Cycle: Li-6; Time since now (sec) ; UCN hits in cycle");
      fHitsPerCycleIntime->SetTitle("UCN Counts per Cycle: Li-6; Time since now (sec) ; UCN hits in cycle");
    }
  }else{
    if(isOffline){
      fHitsPerCycle->SetTitle("UCN Counts per Cycle: He-3; Time since last event (sec) ; UCN hits in cycle");
      fHitsPerCycleIntime->SetTitle("UCN Counts per Cycle: He-3; Time since last event (sec) ; UCN hits in cycle");
    }else{
      fHitsPerCycle->SetTitle("UCN Counts per Cycle: He-3; Time since now (sec) ; UCN hits in cycle");
      fHitsPerCycleIntime->SetTitle("UCN Counts per Cycle: He-3; Time since now (sec) ; UCN hits in cycle");
    }
  }

  
  if(fIsLi6){
    sprintf(hname,"hits_per_cycle_li6");
    sprintf(htitle,"Hits per Cycle: Li-6");
  }else{
    sprintf(hname,"hits_per_cycle_he3");
    sprintf(htitle,"Hits per Cycle: He-3");
  }
  fHitsPerCycleH = new TH1D(hname,htitle,50,-49.5,0.5);
  fHitsPerCycleH->SetYTitle("UCN hits in cycle");
  fHitsPerCycleH->SetXTitle("Cycle Number");
  fHitsPerCycleH->SetMarkerStyle(20);

  if(fIsLi6){
    sprintf(hname,"hits_per_cycle_li6_intime");
    sprintf(htitle,"In-time Hits per Cycle: Li-6");
  }else{
    sprintf(hname,"hits_per_cycle_he3_intime");
    sprintf(htitle,"In-time Hits per Cycle: He-3");
  }
  fHitsPerCycleHIntime = new TH1D(hname,htitle,50,-49.5,0.5);
  fHitsPerCycleHIntime->SetYTitle("UCN hits in cycle");
  fHitsPerCycleHIntime->SetXTitle("Cycle Number");
  fHitsPerCycleHIntime->SetMarkerStyle(20);

  
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

void TUCNDetectorBaseClass::CheckForSequenceSettings(TDataContainer& dataContainer){

  // Use the sequence bank to see when a new run starts:
  TGenericData *data = dataContainer.GetEventData<TGenericData>("SEQN");
  if(data){
    fSeqDelayTime = data->GetData32()[3]/1000.0;
    fSeqOpenInterval = data->GetData32()[4]/1000.0;
  }

}
  
bool TUCNDetectorBaseClass::CheckForSequenceStartCrude(TDataContainer& dataContainer){

  // Use the sequence bank to see when a new run starts:
  TGenericData *data = dataContainer.GetEventData<TGenericData>("SEQN");

  if(data){
    if(data->GetData32()[1] & 2){
      fLastCycleStartTime = fCycleStartTime;
      double tmp = ((double)dataContainer.GetMidasData().GetTimeStamp());
      fCycleStartTime = tmp;
      fSeqValveOpenTime = fCycleStartTime + fSeqDelayTime;
      fSeqValveCloseTime = fSeqValveOpenTime + fSeqOpenInterval;
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
  CheckForSequenceSettings(dataContainer);
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
    if(hittime >= fSeqValveOpenTime && hittime < fSeqValveCloseTime){
      fTotalHitsCycleIntime++;
      fHitsInCycleIntime->Fill(time_in_cycle);
      fHitsInCycleCumulIntime->Fill(time_in_cycle);
    }
  }
  
  // If new sequence started, update information for the cumulative sequence plot
  if(sequence_started && fLastCycleStartTime != 0.0){
    fHitsPerCycleVector.push_back(std::pair<double,double>(fLastCycleStartTime,fTotalHitsCycle));
    fHitsPerCycleVectorIntime.push_back(std::pair<double,double>(fLastCycleStartTime,fTotalHitsCycleIntime));
    fTotalHitsCycle = 0;
    fTotalHitsCycleIntime = 0;
    if(fHitsPerCycleVector.size() > 100){ // Save at most 100 cycles
      fHitsPerCycleVector.erase(fHitsPerCycleVector.begin());
      fHitsPerCycleVectorIntime.erase(fHitsPerCycleVectorIntime.begin());      
    }
    // Reset the hits in cycle histogram
    fHitsInCycle->Reset();
    fHitsInCycleIntime->Reset();
  }
  
  fHitsPerCycle->Set(0);
  fHitsPerCycleIntime->Set(0);
  double maxCycle = 0;
  for(unsigned int i = 0; i < fHitsPerCycleVector.size(); i++){
    double dtime = fHitsPerCycleVector[i].first - ((double)dataContainer.GetMidasData().GetTimeStamp());
    //      fLastCycleStartTime;
    fHitsPerCycle->SetPoint(i,dtime,fHitsPerCycleVector[i].second);
    fHitsPerCycleIntime->SetPoint(i,dtime,fHitsPerCycleVectorIntime[i].second);  
    if(fHitsPerCycleVector[i].second > maxCycle) maxCycle = fHitsPerCycleVector[i].second;
  }

  fHitsPerCycle->SetMinimum(0);
  fHitsPerCycle->SetMaximum(maxCycle*1.2);

  fHitsPerCycleH->Reset();
  fHitsPerCycleHIntime->Reset();
  for(unsigned int i = 0; i < fHitsPerCycleVector.size(); i++){
    int bin = 50-fHitsPerCycleVector.size() + i + 1;
    if(bin < 1) continue;
    fHitsPerCycleH->SetBinContent(bin,fHitsPerCycleVector[i].second+0.0001);
    fHitsPerCycleHIntime->SetBinContent(bin,fHitsPerCycleVectorIntime[i].second+0.0001);
  }



  // Fill the tree, if requested;
  // save both the 'UCN hits' and the 'background hits'
  if(fHitsTree){
    fHitsTree->FillHits(fHits,1);
    fHitsTree->FillHits(fBackgroundHits,0);     
    if(sequence_started){
      fHitsTree->FillTransition(fCycleStartTime);
    }
  }

  // Do detector specific plot filling
  FillSpecificPlots();
  
}
