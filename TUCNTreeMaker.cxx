
#include "TUCNTreeMaker.h"
#include <iostream>
#include "TGenericData.hxx"

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
    if(hit.psd > -2.0)
      tPSD = hit.psd;
    else
      tPSD = -2.0;
   
    tChannel = hit.channel;
    tIsUCN = isUCN;
    tUCN->Fill();
    tEntry++;
  }

}



TUCNSourceEpicsTree::TUCNSourceEpicsTree(){

  std::cout << "Creating source EPICS tree "<< std::endl;

  tSource = new TTree("SourceEpicsTree", "SourceEpicsTree");
  //  tSource->Branch("tEntry",&tEntry, "tEntry/l" );
  tSource->Branch("timestamp", &timestamp, "timestamp/I" );
  tSource->Branch("UCN_VAC_IGP1_RDVAC", &UCN_VAC_IGP1_RDVAC, "UCN_VAC_IGP1_RDVAC/D" );
  tSource->Branch("UCN_HE4_FM4_RDFLOW", &UCN_HE4_FM4_RDFLOW, "UCN_HE4_FM4_RDFLOW/D" );
  tSource->Branch("UCN_D2O_TS7_RDTEMP",&UCN_D2O_TS7_RDTEMP, "UCN_D2O_TS7_RDTEMP/D" );
  tSource->Branch("UCN_HE4_LVL1_RDLVL",&UCN_HE4_LVL1_RDLVL, "UCN_HE4_LVL1_RDLVL/D" );

};



void TUCNSourceEpicsTree::FillTree(TDataContainer& dataContainer){

  // Use the sequence bank to see when a new run starts:
  TGenericData *data = dataContainer.GetEventData<TGenericData>("EPSR");

  if(!data) return;

  // Save the unix timestamp
  timestamp = dataContainer.GetMidasData().GetTimeStamp();

  // Save EPICS variables
  // Need to know the semi-random mapping between array index
  // and EPICS variable to figure out how to fill the Tree variables.
  // You can see the mapping here:
  // https://ucndaq01.triumf.ca/Equipment/SourceEpics/Settings  
  UCN_VAC_IGP1_RDVAC = data->GetFloat()[0];
  UCN_HE4_FM4_RDFLOW = data->GetFloat()[48];
  UCN_D2O_TS7_RDTEMP = data->GetFloat()[56];
  UCN_HE4_LVL1_RDLVL = data->GetFloat()[88];
  tSource->Fill();
  
};




TUCNBeamlineEpicsTree::TUCNBeamlineEpicsTree(){

  std::cout << "Creating source EPICS tree "<< std::endl;

  tBeamline = new TTree("BeamlineEpicsTree", "BeamlineEpicsTree");
  //  tBeamline->Branch("tEntry",&tEntry, "tEntry/l" );
  tBeamline->Branch("timestamp", &timestamp, "timestamp/I" );
  tBeamline->Branch("B1U_TNIM2_RAW", &B1U_TNIM2_RAW, "B1U_TNIM2_RAW/D" );
  tBeamline->Branch("B1U_TNIM2_5MINAVG", &B1U_TNIM2_5MINAVG, "B1U_TNIM2_5MINAVG/D" );
  tBeamline->Branch("B1U_TNIM2_10MINAVG", &B1U_TNIM2_10MINAVG, "B1U_TNIM2_10MINAVG/D" );
  tBeamline->Branch("B1V_KSM_RDFRCTN_VAL1", &B1V_KSM_RDFRCTN_VAL1, "B1V_KSM_RDFRCTN_VAL1/D" );
  tBeamline->Branch("B1V_KSM_BONPRD", &B1V_KSM_BONPRD, "B1V_KSM_BONPRD/D" );
  tBeamline->Branch("B1V_KSM_INSEQ", &B1V_KSM_INSEQ, "B1V_KSM_INSEQ/D" );
  tBeamline->Branch("B1V_KSM_INSEQ", &B1V_KSM_INSEQ, "B1V_KSM_INSEQ/D" );
  tBeamline->Branch("B1V_KSM_RDBEAMON_VAL1", &B1V_KSM_RDBEAMON_VAL1, "B1V_KSM_RDBEAMON_VAL1/D" );
  tBeamline->Branch("B1V_KSM_RDBEAMOFF_VAL1", &B1V_KSM_RDBEAMOFF_VAL1, "B1V_KSM_RDBEAMOFF_VAL1/D" );
  tBeamline->Branch("B1V_KSM_PREDCUR", &B1V_KSM_PREDCUR, "B1V_KSM_PREDCUR/D" );
  tBeamline->Branch("B1V_FOIL_ADJCUR", &B1V_FOIL_ADJCUR, "B1V_FOIL_ADJCUR/D" );

  B1V_KSM_PREDCUR = 0;
  B1V_FOIL_ADJCUR = 0;

};



void TUCNBeamlineEpicsTree::FillTree(TDataContainer& dataContainer){

  // Use the sequence bank to see when a new run starts:
  TGenericData *data = dataContainer.GetEventData<TGenericData>("EPBL");

  if(!data) return;

  // Save the unix timestamp
  timestamp = dataContainer.GetMidasData().GetTimeStamp();

  // Save EPICS variables
  // Need to know the semi-random mapping between array index
  // and EPICS variable to figure out how to fill the Tree variables.
  // You can see the mapping here:
  // https://ucndaq01.triumf.ca/Equipment/BeamlineEpics/Settings
  B1U_TNIM2_RAW = data->GetFloat()[0];
  B1U_TNIM2_5MINAVG = data->GetFloat()[21];
  B1U_TNIM2_10MINAVG = data->GetFloat()[22] ;
  B1V_KSM_RDFRCTN_VAL1 = data->GetFloat()[28];
  B1V_KSM_BONPRD = data->GetFloat()[29];
  B1V_KSM_INSEQ = data->GetFloat()[32];
  B1V_KSM_RDBEAMON_VAL1 = data->GetFloat()[30];
  B1V_KSM_RDBEAMOFF_VAL1 = data->GetFloat()[31];
  if(data->GetSize() >= 39){
    B1V_KSM_PREDCUR = data->GetFloat()[37];
    B1V_FOIL_ADJCUR = data->GetFloat()[38];
  }
  tBeamline->Fill();
  
};
