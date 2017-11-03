
#include "TUCNTreeMaker.h"
#include <iostream>

TUCNHitsTree::TUCNHitsTree(std::string name):detector_name(name){

  std::cout << "Creating trees for " << detector_name << std::endl;

 // tree for run transitions
  char tree_name[100];
  sprintf(tree_name,"RunTransitions_%s",detector_name.c_str());
  tRunTran = new TTree(tree_name, "RunTransitions");
  tRunTran->Branch("tRunNum", &tRunNum, "tRunNum/I" );
  tRunTran->Branch("tTime", &tTime, "tTime/I" );
  tRunTran->Branch("tEntry",&tEntry, "tEntry/l" );

  //tRunNum = run;
  //tTime = time;
  tEntry=0;
  
  // tree for runtime event data
  sprintf(tree_name,"UCNHits_%s",detector_name.c_str());
  tUCN=new TTree(tree_name,"Event");
  tUCN->Branch("tEntry",    &tEntry,    "tEntry/l");
  tUCN->Branch("tTimeE",    &tTimeE,    "tTimeE/l");
  tUCN->Branch("tTimeStamp",    &tTimeStamp,    "tTimeStamp/l");
  tUCN->Branch("tUnixTime",    &tUnixTime,    "tUnixTime/D");
  tUCN->Branch("tUnixTimePrecise",    &tUnixTimePrecise,    "tUnixTimePrecise/D");
  tUCN->Branch("tChannel",  &tChannel,  "tChannel/s");
  tUCN->Branch("tPSD",      &tPSD,      "tPSD/f");
  tUCN->Branch("tChargeL",  &tChargeL,  "tChargeL/s");
  tUCN->Branch("tChargeS",  &tChargeS,  "tChargeS/s");
  tUCN->Branch("tBaseline", &tBaseline, "tBaseline/s");
  tUCN->Branch("tLength",   &tLength,   "tLength/s");
  tUCN->Branch("tIsUCN",   &tIsUCN,   "tIsUCN/s");
  //tUCN->Branch("tPulse",     tPulse,    "tPulse[tLength]/s");



}


void TUCNHitsTree::FillHits(TUCNHitCollection& hits, int isUCN){

  for(unsigned int j = 0; j < hits.size(); j++){ // loop over measurements
    TUCNHit hit = hits[j];

    tTimeStamp = hit.clockTime;    
    tUnixTime = hit.time;
    tUnixTime = hit.preciseTime;
    tChargeL = hit.chargeLong;
    tChargeS = hit.chargeShort;
    tBaseline = hit.baseline;
    tPSD = hit.psd;
    std::cout << "PSD : " << tPSD 
	      << " " << hit.chargeLong << " " << hit.chargeShort << std::endl;
    tChannel = hit.channel;
    tIsUCN = isUCN;
    tUCN->Fill();
    tEntry++;
  }

}
