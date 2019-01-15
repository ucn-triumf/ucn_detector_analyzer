
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
  tRunTran->Branch("cycleIndex",&cycleIndex,"cycleIndex/I");
  tRunTran->Branch("cycleStartTime",&cycleStartTime,"cycleStartTime/D");
  tRunTran->Branch("cycleValveOpenTime",&cycleValveOpenTime,"cycleValveOpenTime/D");
  tRunTran->Branch("cycleValveCloseTime",&cycleValveCloseTime,"cycleValveCloseTime/D");
  tRunTran->Branch("cycleDelayTime",&cycleDelayTime,"cycleDelayTime/D");
  tRunTran->Branch("cycleValveOpenTime",&cycleValveOpenTime,"cycleValveOpenTime/D");
  tRunTran->Branch("cyclePeriod0EndTime",&cyclePeriod0EndTime,"cyclePeriod0EndTime/D");
  tRunTran->Branch("cyclePeriod1EndTime",&cyclePeriod1EndTime,"cyclePeriod1EndTime/D");
  tRunTran->Branch("cyclePeriod2EndTime",&cyclePeriod2EndTime,"cyclePeriod2EndTime/D");
  tRunTran->Branch("cyclePeriod3EndTime",&cyclePeriod3EndTime,"cyclePeriod3EndTime/D");
  tRunTran->Branch("cyclePeriod4EndTime",&cyclePeriod4EndTime,"cyclePeriod4EndTime/D");
  tRunTran->Branch("cyclePeriod5EndTime",&cyclePeriod5EndTime,"cyclePeriod5EndTime/D");
  tRunTran->Branch("cyclePeriod6EndTime",&cyclePeriod6EndTime,"cyclePeriod6EndTime/D");
  tRunTran->Branch("cyclePeriod7EndTime",&cyclePeriod7EndTime,"cyclePeriod7EndTime/D");
  tRunTran->Branch("cyclePeriod8EndTime",&cyclePeriod8EndTime,"cyclePeriod8EndTime/D");
  tRunTran->Branch("cyclePeriod9EndTime",&cyclePeriod9EndTime,"cyclePeriod9EndTime/D");
  tRunTran->Branch("valveStatePeriod0",valveStatePeriod0,"valveStatePeriod0[8]/I");
  tRunTran->Branch("valveStatePeriod1",valveStatePeriod1,"valveStatePeriod1[8]/I");
  tRunTran->Branch("valveStatePeriod2",valveStatePeriod2,"valveStatePeriod2[8]/I");
  tRunTran->Branch("valveStatePeriod3",valveStatePeriod3,"valveStatePeriod3[8]/I");
  tRunTran->Branch("valveStatePeriod4",valveStatePeriod4,"valveStatePeriod4[8]/I");
  tRunTran->Branch("valveStatePeriod5",valveStatePeriod5,"valveStatePeriod5[8]/I");
  tRunTran->Branch("valveStatePeriod6",valveStatePeriod6,"valveStatePeriod6[8]/I");
  tRunTran->Branch("valveStatePeriod7",valveStatePeriod7,"valveStatePeriod7[8]/I");
  tRunTran->Branch("valveStatePeriod8",valveStatePeriod8,"valveStatePeriod8[8]/I");
  tRunTran->Branch("valveStatePeriod9",valveStatePeriod9,"valveStatePeriod9[8]/I");

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
    tUnixTimePrecise = hit.preciseTime;
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



void TUCNHitsTree::FillTransition(double icycleStartTime, double icycleValveOpenTime, double icycleValveCloseTime,
                                  double icycleDelayTime, double icycleOpenInterval,
				  TUCNCycleParameters CycleParameters){
 
  cycleIndex = CycleParameters.CycleIndex();
  cycleStartTime = icycleStartTime; 
  cycleValveOpenTime = icycleValveOpenTime;
  cycleValveCloseTime = icycleValveCloseTime;
  cycleDelayTime = icycleDelayTime;
  cycleOpenInterval = icycleOpenInterval;
  
  std::cout << "Transition time: " << cycleStartTime << " " << cycleDelayTime << std::endl;
  // Fill the parameters from the new NSEQ bank...

  std::cout << "periods diff: " << CycleParameters.CycleIndex() << " " 
	    << CycleParameters.NumberPeriods() << std::endl;
  
  cyclePeriod0EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(0);
  cyclePeriod1EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(1);
  cyclePeriod2EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(2);
  cyclePeriod3EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(3);
  cyclePeriod4EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(4);
  cyclePeriod5EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(5);
  cyclePeriod6EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(6);
  cyclePeriod7EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(7);
  cyclePeriod8EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(8);
  cyclePeriod9EndTime =  cycleStartTime + CycleParameters.GetCumulativeTimeForPeriod(9);

  if(0)std::cout << "NNN "  << CycleParameters.GetCumulativeTimeForPeriod(5) << " " << (int) cyclePeriod5EndTime << " " 
            << CycleParameters.GetCumulativeTimeForPeriod(6) << " " << (int) cyclePeriod6EndTime << " " 
            << CycleParameters.GetCumulativeTimeForPeriod(7) << " "<< (int) cyclePeriod7EndTime << " " 
            << CycleParameters.GetCumulativeTimeForPeriod(8) << " "<< (int) cyclePeriod8EndTime << " " 
            << CycleParameters.GetCumulativeTimeForPeriod(9) << " "<< (int) cyclePeriod9EndTime << std::endl;

  for(int i = 0; i < 8; i++){
    valveStatePeriod0[i] = CycleParameters.GetValveState(0,i);
    valveStatePeriod1[i] = CycleParameters.GetValveState(1,i);
    valveStatePeriod2[i] = CycleParameters.GetValveState(2,i);
    valveStatePeriod3[i] = CycleParameters.GetValveState(3,i);
    valveStatePeriod4[i] = CycleParameters.GetValveState(4,i);
    valveStatePeriod5[i] = CycleParameters.GetValveState(5,i);
    valveStatePeriod6[i] = CycleParameters.GetValveState(6,i);
    valveStatePeriod7[i] = CycleParameters.GetValveState(7,i);
    valveStatePeriod8[i] = CycleParameters.GetValveState(8,i);
    valveStatePeriod9[i] = CycleParameters.GetValveState(9,i);
  }
  

  tRunTran->Fill();

}



TLNDDetectorTree::TLNDDetectorTree() {
  std::cout << "Creating LND Thermal neutron detector tree "<< std::endl;
  tLND = new TTree("LNDDetectorTree", "LNDDetectorTree");
  tLND->Branch("timestamp", &timestamp, "timestamp/I" );
  tLND->Branch("LND_Reading", &LND_Reading, "LND_Reading/D" );
}

void TLNDDetectorTree::FillTree(TDataContainer& dataContainer){
  // Use the sequence bank to see when a new run starts:
  TGenericData *data = dataContainer.GetEventData<TGenericData>("PICO");
  if(!data) return;

  // Save the unix timestamp
  timestamp = dataContainer.GetMidasData().GetTimeStamp();  
  LND_Reading =  data->GetDouble()[0];
  tLND->Fill();
}


TSequencerTree::TSequencerTree(){
  std::cout << "Creating Sequencer tree "<< std::endl;
  tSequencer = new TTree("SequencerTree", "SequencerTree");
  tSequencer->Branch("timestamp", &timestamp, "timestamp/I" );
  tSequencer->Branch("sequencerEnabled", &sequencerEnabled, "sequencerEnabled/I" );
  tSequencer->Branch("inCycle", &inCycle, "inCycle/I" );
  tSequencer->Branch("cycleStarted", &cycleStarted, "cycleStarted/I" );
};


void TSequencerTree::FillTree(TDataContainer& dataContainer){

  // Use the sequence bank to see when a new run starts:
  TGenericData *data = dataContainer.GetEventData<TGenericData>("USEQ");
  if(!data) return;

  timestamp = dataContainer.GetMidasData().GetTimeStamp();
  sequencerEnabled = data->GetData32()[5];
  inCycle = (data->GetData32()[4] & 1);
  cycleStarted = (data->GetData32()[4] & 2);
  
  tSequencer->Fill();

};


TSCMTree::TSCMTree() {

  std::cout << "Creating SCM tree "<< std::endl;
  tSCM = new TTree("SCMTree", "SCMTree");
  tSCM->Branch("timestamp", &timestamp, "timestamp/I" );
  tSCM->Branch("SCMTemps", SCMTemps, "SCMTemps[8]/F" );
  tSCM->Branch("SCMVoltages", SCMVoltages, "SCMVoltages[4]/F" );

}

void TSCMTree::FillTree(TDataContainer& dataContainer){
  
  // Grab the temperature data if it is available.
  TGenericData *data = dataContainer.GetEventData<TGenericData>("TEMP");
  if(data){;
    // Save the unix timestamp and SCM temperatures
    for(int i = 0; i < 8; i++) SCMTemps[i] =  data->GetFloat()[i];
    return;
  }

  // Save all the SCM voltage data
  TGenericData *data2 = dataContainer.GetEventData<TGenericData>("CH\0\0");
  if(!data2) return;

  // Save the unix timestamp and SCM temperatures
  timestamp = dataContainer.GetMidasData().GetTimeStamp();
  //  data2->Dump();
  for(int i = 0; i < 4; i++) SCMVoltages[i] =  data2->GetFloat()[i];

  tSCM->Fill();

}
  

TUCNSourceEpicsTree::TUCNSourceEpicsTree(){

  std::cout << "Creating source EPICS tree "<< std::endl;

  tSource = new TTree("SourceEpicsTree", "SourceEpicsTree");
  //  tSource->Branch("tEntry",&tEntry, "tEntry/l" );
  tSource->Branch("timestamp", &timestamp, "timestamp/I" );


tSource->Branch("UCN_VAC_IGP1_RDVAC",&UCN_VAC_IGP1_RDVAC, "UCN_VAC_IGP1_RDVAC/D");
tSource->Branch("UCN_VAC_CG1A_RDVAC",&UCN_VAC_CG1A_RDVAC, "UCN_VAC_CG1A_RDVAC/D");
tSource->Branch("UCN_VAC_PG1_RDPRESS",&UCN_VAC_PG1_RDPRESS, "UCN_VAC_PG1_RDPRESS/D");
tSource->Branch("UCN_HE3_PG6L_RDPRESS",&UCN_HE3_PG6L_RDPRESS, "UCN_HE3_PG6L_RDPRESS/D");
tSource->Branch("UCN_HE3_PG6H_RDPRESS",&UCN_HE3_PG6H_RDPRESS, "UCN_HE3_PG6H_RDPRESS/D");
tSource->Branch("UCN_HE4_PG3L_RDPRESS",&UCN_HE4_PG3L_RDPRESS, "UCN_HE4_PG3L_RDPRESS/D");
tSource->Branch("UCN_HE4_PG3H_RDPRESS",&UCN_HE4_PG3H_RDPRESS, "UCN_HE4_PG3H_RDPRESS/D");
tSource->Branch("UCN_VAC_PV1_STATON",&UCN_VAC_PV1_STATON, "UCN_VAC_PV1_STATON/D");
tSource->Branch("UCN_VAC_GV1_STATON",&UCN_VAC_GV1_STATON, "UCN_VAC_GV1_STATON/D");
tSource->Branch("UCN_VAC_VV1_STATON",&UCN_VAC_VV1_STATON, "UCN_VAC_VV1_STATON/D");
tSource->Branch("UCN_VAC_CG1B_RDVAC",&UCN_VAC_CG1B_RDVAC, "UCN_VAC_CG1B_RDVAC/D");
tSource->Branch("UCN_ISO_CG2A_RDVAC",&UCN_ISO_CG2A_RDVAC, "UCN_ISO_CG2A_RDVAC/D");
tSource->Branch("UCN_ISO_CG2B_RDVAC",&UCN_ISO_CG2B_RDVAC, "UCN_ISO_CG2B_RDVAC/D");
tSource->Branch("UCN_UGD_IGP2_RDVAC",&UCN_UGD_IGP2_RDVAC, "UCN_UGD_IGP2_RDVAC/D");
tSource->Branch("UCN_ISO_PG9H_RDPRESS",&UCN_ISO_PG9H_RDPRESS, "UCN_ISO_PG9H_RDPRESS/D");
tSource->Branch("UCN_ISO_PG9L_RDPRESS",&UCN_ISO_PG9L_RDPRESS, "UCN_ISO_PG9L_RDPRESS/D");
tSource->Branch("UCN_D20_PG12_RDPRESS",&UCN_D20_PG12_RDPRESS, "UCN_D20_PG12_RDPRESS/D");
tSource->Branch("UCN_HE4_PG4_RDPRESS",&UCN_HE4_PG4_RDPRESS, "UCN_HE4_PG4_RDPRESS/D");
tSource->Branch("UCN_HE4_PG5_RDPRESS",&UCN_HE4_PG5_RDPRESS, "UCN_HE4_PG5_RDPRESS/D");
tSource->Branch("UCN_HE3_PG7_RDPRESS",&UCN_HE3_PG7_RDPRESS, "UCN_HE3_PG7_RDPRESS/D");
tSource->Branch("UCN_HE3_PG8_RDPRESS",&UCN_HE3_PG8_RDPRESS, "UCN_HE3_PG8_RDPRESS/D");
tSource->Branch("UCN_HE3_PG11_RDPRESS",&UCN_HE3_PG11_RDPRESS, "UCN_HE3_PG11_RDPRESS/D");
tSource->Branch("UCN_ISO_SV3_STATON",&UCN_ISO_SV3_STATON, "UCN_ISO_SV3_STATON/D");
tSource->Branch("UCN_ISO_SV4_STATON",&UCN_ISO_SV4_STATON, "UCN_ISO_SV4_STATON/D");
tSource->Branch("UCN_ISO_PV2_STATON",&UCN_ISO_PV2_STATON, "UCN_ISO_PV2_STATON/D");
tSource->Branch("UCN_ISO_SV5_STATON",&UCN_ISO_SV5_STATON, "UCN_ISO_SV5_STATON/D");
tSource->Branch("UCN_HE4_GV2_STATON",&UCN_HE4_GV2_STATON, "UCN_HE4_GV2_STATON/D");
tSource->Branch("UCN_HE3_GV3_STATON",&UCN_HE3_GV3_STATON, "UCN_HE3_GV3_STATON/D");
tSource->Branch("UCN_HE4_PG2_RDPRESS",&UCN_HE4_PG2_RDPRESS, "UCN_HE4_PG2_RDPRESS/D");
tSource->Branch("UCN_HE4_MP1_STATON",&UCN_HE4_MP1_STATON, "UCN_HE4_MP1_STATON/D");
tSource->Branch("UCN_HE4_MP2_STATON",&UCN_HE4_MP2_STATON, "UCN_HE4_MP2_STATON/D");
tSource->Branch("UCN_HE3_MP3_STATON",&UCN_HE3_MP3_STATON, "UCN_HE3_MP3_STATON/D");
tSource->Branch("UCN_HE3_MP4_STATON",&UCN_HE3_MP4_STATON, "UCN_HE3_MP4_STATON/D");
tSource->Branch("UCN_HE3_MP5_STATON",&UCN_HE3_MP5_STATON, "UCN_HE3_MP5_STATON/D");
tSource->Branch("UCN_VAC_BP1_STATON",&UCN_VAC_BP1_STATON, "UCN_VAC_BP1_STATON/D");
tSource->Branch("UCN_VAC_TP1_STATON",&UCN_VAC_TP1_STATON, "UCN_VAC_TP1_STATON/D");
tSource->Branch("UCN_ISO_BP2_STATON",&UCN_ISO_BP2_STATON, "UCN_ISO_BP2_STATON/D");
tSource->Branch("UCN_ISO_TP2_STATON",&UCN_ISO_TP2_STATON, "UCN_ISO_TP2_STATON/D");
tSource->Branch("UCN_HE3_PSW1_K2ON",&UCN_HE3_PSW1_K2ON, "UCN_HE3_PSW1_K2ON/D");
tSource->Branch("UCN_HE3_PSW2_K2ON",&UCN_HE3_PSW2_K2ON, "UCN_HE3_PSW2_K2ON/D");
tSource->Branch("UCN_UGD_IV1_STATON",&UCN_UGD_IV1_STATON, "UCN_UGD_IV1_STATON/D");
tSource->Branch("UCN_HE4_PG10_RDPRESS",&UCN_HE4_PG10_RDPRESS, "UCN_HE4_PG10_RDPRESS/D");
tSource->Branch("UCN_HE3_FM1_RDFLOW",&UCN_HE3_FM1_RDFLOW, "UCN_HE3_FM1_RDFLOW/D");
tSource->Branch("UCN_HE4_FM2_RDFLOW",&UCN_HE4_FM2_RDFLOW, "UCN_HE4_FM2_RDFLOW/D");
tSource->Branch("UCN_ISO_FM3_RDFLOW",&UCN_ISO_FM3_RDFLOW, "UCN_ISO_FM3_RDFLOW/D");
tSource->Branch("UCN_HE4_FM4_RDFLOW",&UCN_HE4_FM4_RDFLOW, "UCN_HE4_FM4_RDFLOW/D");
tSource->Branch("UCN_HE4_FM5_RDFLOW",&UCN_HE4_FM5_RDFLOW, "UCN_HE4_FM5_RDFLOW/D");
tSource->Branch("UCN_CRY_TS1_RDTEMP",&UCN_CRY_TS1_RDTEMP, "UCN_CRY_TS1_RDTEMP/D");
tSource->Branch("UCN_D2O_TS2_RDTEMP",&UCN_D2O_TS2_RDTEMP, "UCN_D2O_TS2_RDTEMP/D");
tSource->Branch("UCN_CRY_TS3_RDTEMP",&UCN_CRY_TS3_RDTEMP, "UCN_CRY_TS3_RDTEMP/D");
tSource->Branch("UCN_HE4_TS4_RDTEMP",&UCN_HE4_TS4_RDTEMP, "UCN_HE4_TS4_RDTEMP/D");
tSource->Branch("UCN_D2O_TS5_RDTEMP",&UCN_D2O_TS5_RDTEMP, "UCN_D2O_TS5_RDTEMP/D");
tSource->Branch("UCN_D2O_TS6_RDTEMP",&UCN_D2O_TS6_RDTEMP, "UCN_D2O_TS6_RDTEMP/D");
tSource->Branch("UCN_D2O_TS7_RDTEMP",&UCN_D2O_TS7_RDTEMP, "UCN_D2O_TS7_RDTEMP/D");
tSource->Branch("UCN_D2O_TS8_RDTEMP",&UCN_D2O_TS8_RDTEMP, "UCN_D2O_TS8_RDTEMP/D");
tSource->Branch("UCN_HE4_TS9_RDTEMP",&UCN_HE4_TS9_RDTEMP, "UCN_HE4_TS9_RDTEMP/D");
tSource->Branch("UCN_HE3_TS10_RDTEMP",&UCN_HE3_TS10_RDTEMP, "UCN_HE3_TS10_RDTEMP/D");
tSource->Branch("UCN_ISO_TS11_RDTEMP",&UCN_ISO_TS11_RDTEMP, "UCN_ISO_TS11_RDTEMP/D");
tSource->Branch("UCN_ISO_TS12_RDTEMP",&UCN_ISO_TS12_RDTEMP, "UCN_ISO_TS12_RDTEMP/D");
tSource->Branch("UCN_ISO_TS13_RDTEMP",&UCN_ISO_TS13_RDTEMP, "UCN_ISO_TS13_RDTEMP/D");
tSource->Branch("UCN_ISO_TS14_RDTEMP",&UCN_ISO_TS14_RDTEMP, "UCN_ISO_TS14_RDTEMP/D");
tSource->Branch("UCN_ISO_TS15_RDTEMP",&UCN_ISO_TS15_RDTEMP, "UCN_ISO_TS15_RDTEMP/D");
tSource->Branch("UCN_ISO_TS16_RDTEMP",&UCN_ISO_TS16_RDTEMP, "UCN_ISO_TS16_RDTEMP/D");
tSource->Branch("UCN_ISO_TS17_RDTEMP",&UCN_ISO_TS17_RDTEMP, "UCN_ISO_TS17_RDTEMP/D");
tSource->Branch("UCN_ISO_TS18_RDTEMP",&UCN_ISO_TS18_RDTEMP, "UCN_ISO_TS18_RDTEMP/D");
tSource->Branch("UCN_UGD_TS19_RDTEMP",&UCN_UGD_TS19_RDTEMP, "UCN_UGD_TS19_RDTEMP/D");
tSource->Branch("UCN_UGD_TS20_RDTEMP",&UCN_UGD_TS20_RDTEMP, "UCN_UGD_TS20_RDTEMP/D");
tSource->Branch("UCN_UGD_TS21_RDTEMP",&UCN_UGD_TS21_RDTEMP, "UCN_UGD_TS21_RDTEMP/D");
tSource->Branch("UCN_UGD_TS22_RDTEMP",&UCN_UGD_TS22_RDTEMP, "UCN_UGD_TS22_RDTEMP/D");
tSource->Branch("UCN_CRY_TS23_RDTEMP",&UCN_CRY_TS23_RDTEMP, "UCN_CRY_TS23_RDTEMP/D");
tSource->Branch("UCN_CRY_TS24_RDTEMP",&UCN_CRY_TS24_RDTEMP, "UCN_CRY_TS24_RDTEMP/D");
tSource->Branch("UCN_HE4_TS25_RDTEMP",&UCN_HE4_TS25_RDTEMP, "UCN_HE4_TS25_RDTEMP/D");
tSource->Branch("UCN_HE4_TS26_RDTEMP",&UCN_HE4_TS26_RDTEMP, "UCN_HE4_TS26_RDTEMP/D");
tSource->Branch("UCN_HE3_TS27_RDTEMP",&UCN_HE3_TS27_RDTEMP, "UCN_HE3_TS27_RDTEMP/D");
tSource->Branch("UCN_HE3_TS28_RDTEMP",&UCN_HE3_TS28_RDTEMP, "UCN_HE3_TS28_RDTEMP/D");
tSource->Branch("UCN_D2O_TS29_RDTEMP",&UCN_D2O_TS29_RDTEMP, "UCN_D2O_TS29_RDTEMP/D");
tSource->Branch("UCN_D2O_TS30_RDTEMP",&UCN_D2O_TS30_RDTEMP, "UCN_D2O_TS30_RDTEMP/D");
tSource->Branch("UCN_CRY_TS31_RDTEMP",&UCN_CRY_TS31_RDTEMP, "UCN_CRY_TS31_RDTEMP/D");
tSource->Branch("UCN_CRY_TS32_RDTEMP",&UCN_CRY_TS32_RDTEMP, "UCN_CRY_TS32_RDTEMP/D");
tSource->Branch("UCN_CRY_TS33_RDTEMP",&UCN_CRY_TS33_RDTEMP, "UCN_CRY_TS33_RDTEMP/D");
tSource->Branch("UCN_CRY_TS34_RDTEMP",&UCN_CRY_TS34_RDTEMP, "UCN_CRY_TS34_RDTEMP/D");
tSource->Branch("UCN_CRY_TS35_RDTEMP",&UCN_CRY_TS35_RDTEMP, "UCN_CRY_TS35_RDTEMP/D");
tSource->Branch("UCN_UGD_TS36_RDTEMP",&UCN_UGD_TS36_RDTEMP, "UCN_UGD_TS36_RDTEMP/D");
tSource->Branch("UCN_UGD_TS37_RDTEMP",&UCN_UGD_TS37_RDTEMP, "UCN_UGD_TS37_RDTEMP/D");
tSource->Branch("UCN_UGD_TS38_RDTEMP",&UCN_UGD_TS38_RDTEMP, "UCN_UGD_TS38_RDTEMP/D");
tSource->Branch("UCN_HE4_LVL1_RDLVL",&UCN_HE4_LVL1_RDLVL, "UCN_HE4_LVL1_RDLVL/D");
tSource->Branch("UCN_HE4_LVL2_RDLVL",&UCN_HE4_LVL2_RDLVL, "UCN_HE4_LVL2_RDLVL/D");
tSource->Branch("UCN_HE3_LVL3_RDLVL",&UCN_HE3_LVL3_RDLVL, "UCN_HE3_LVL3_RDLVL/D");
tSource->Branch("UCN_HE4_LVL4_RDLVL",&UCN_HE4_LVL4_RDLVL, "UCN_HE4_LVL4_RDLVL/D");
tSource->Branch("UCN_HE4_LVL5_RDLVL",&UCN_HE4_LVL5_RDLVL, "UCN_HE4_LVL5_RDLVL/D");
tSource->Branch("UCN_ISO_PG13_RDPRESS",&UCN_ISO_PG13_RDPRESS, "UCN_ISO_PG13_RDPRESS/D");
tSource->Branch("UCN_HE4_HTR8_SETTEMP",&UCN_HE4_HTR8_SETTEMP, "UCN_HE4_HTR8_SETTEMP/D");
tSource->Branch("UCN_HE4_HTR9_SETTEMP",&UCN_HE4_HTR9_SETTEMP, "UCN_HE4_HTR9_SETTEMP/D");
tSource->Branch("UCN_HE4_HTR10_SETTEMP",&UCN_HE4_HTR10_SETTEMP, "UCN_HE4_HTR10_SETTEMP/D");
tSource->Branch("UCN_HE4_HTR11_SETTEMP",&UCN_HE4_HTR11_SETTEMP, "UCN_HE4_HTR11_SETTEMP/D");
tSource->Branch("UCN_HE3_HTR12_SETTEMP",&UCN_HE3_HTR12_SETTEMP, "UCN_HE3_HTR12_SETTEMP/D");
tSource->Branch("UCN_CRY_CP1A_STATON",&UCN_CRY_CP1A_STATON, "UCN_CRY_CP1A_STATON/D");
tSource->Branch("UCN_CRY_CP1B_STATON",&UCN_CRY_CP1B_STATON, "UCN_CRY_CP1B_STATON/D");
tSource->Branch("UCN_HE3_CP2_STATON",&UCN_HE3_CP2_STATON, "UCN_HE3_CP2_STATON/D");
tSource->Branch("UCN_HE4_HTR8_STATON",&UCN_HE4_HTR8_STATON, "UCN_HE4_HTR8_STATON/D");
tSource->Branch("UCN_HE4_HTR9_STATON",&UCN_HE4_HTR9_STATON, "UCN_HE4_HTR9_STATON/D");
tSource->Branch("UCN_HE4_HTR10_STATON",&UCN_HE4_HTR10_STATON, "UCN_HE4_HTR10_STATON/D");
tSource->Branch("UCN_HE4_HTR11_STATON",&UCN_HE4_HTR11_STATON, "UCN_HE4_HTR11_STATON/D");
tSource->Branch("UCN_HE3_HTR12_STATON",&UCN_HE3_HTR12_STATON, "UCN_HE3_HTR12_STATON/D");

 tSource->Branch("UCN_UGD_TS49_RDTEMP",&UCN_UGD_TS49_RDTEMP, "UCN_UGD_TS49_RDTEMP/D");
tSource->Branch("UCN_UGD_TS50_RDTEMP",&UCN_UGD_TS50_RDTEMP, "UCN_UGD_TS50_RDTEMP/D");
tSource->Branch("UCN_UGD_TS51_RDTEMP",&UCN_UGD_TS51_RDTEMP, "UCN_UGD_TS51_RDTEMP/D");
tSource->Branch("UCN_UGD_HTR1_STATON",&UCN_UGD_HTR1_STATON, "UCN_UGD_HTR1_STATON/D");
tSource->Branch("UCN_UGD_HTR2_STATON",&UCN_UGD_HTR2_STATON, "UCN_UGD_HTR2_STATON/D");
tSource->Branch("UCN_UGD_HTR3_STATON",&UCN_UGD_HTR3_STATON, "UCN_UGD_HTR3_STATON/D");
tSource->Branch("UCN_UGD_HTR4_STATON",&UCN_UGD_HTR4_STATON, "UCN_UGD_HTR4_STATON/D");
tSource->Branch("UCN_UGD_HTR5_STATON",&UCN_UGD_HTR5_STATON, "UCN_UGD_HTR5_STATON/D");
tSource->Branch("UCN_ISO_IG4_RDVAC",&UCN_ISO_IG4_RDVAC, "UCN_ISO_IG4_RDVAC/D");
tSource->Branch("UCN_HE3_FPV2_RDDACP",&UCN_HE3_FPV2_RDDACP, "UCN_HE3_FPV2_RDDACP/D");
tSource->Branch("UCN_HE3_PG14_RDPRESS",&UCN_HE3_PG14_RDPRESS, "UCN_HE3_PG14_RDPRESS/D");
tSource->Branch("UCN_SER_HS1_STATON",&UCN_SER_HS1_STATON, "UCN_SER_HS1_STATON/D");
tSource->Branch("UCN_HE4_TS43_RDTEMP",&UCN_HE4_TS43_RDTEMP, "UCN_HE4_TS43_RDTEMP/D");
tSource->Branch("UCN_HE4_TS44_RDTEMP",&UCN_HE4_TS44_RDTEMP, "UCN_HE4_TS44_RDTEMP/D");
tSource->Branch("UCN_HE4_TS45_RDTEMP",&UCN_HE4_TS45_RDTEMP, "UCN_HE4_TS45_RDTEMP/D");
tSource->Branch("UCN_HE4_TS46_RDTEMP",&UCN_HE4_TS46_RDTEMP, "UCN_HE4_TS46_RDTEMP/D");
tSource->Branch("UCN_HE4_TS47_RDTEMP",&UCN_HE4_TS47_RDTEMP, "UCN_HE4_TS47_RDTEMP/D");
tSource->Branch("UCN_HE4_TS48_RDTEMP",&UCN_HE4_TS48_RDTEMP, "UCN_HE4_TS48_RDTEMP/D");
tSource->Branch("UCN_SER_TS52_RDTEMP",&UCN_SER_TS52_RDTEMP, "UCN_SER_TS52_RDTEMP/D");
tSource->Branch("UCN_ISO_MFC1_RDFLOW",&UCN_ISO_MFC1_RDFLOW, "UCN_ISO_MFC1_RDFLOW/D");
tSource->Branch("UCN_ISO_MFC1_FLOW",&UCN_ISO_MFC1_FLOW, "UCN_ISO_MFC1_FLOW/D");
tSource->Branch("UCN_HE3_CG3_RDVAC",&UCN_HE3_CG3_RDVAC, "UCN_HE3_CG3_RDVAC/D");
tSource->Branch("UCN_HE3_TS39_RDTEMP",&UCN_HE3_TS39_RDTEMP, "UCN_HE3_TS39_RDTEMP/D");
tSource->Branch("UCN_HE3_TS40_RDTEMP",&UCN_HE3_TS40_RDTEMP, "UCN_HE3_TS40_RDTEMP/D");
tSource->Branch("UCN_HE3_TS41_RDTEMP",&UCN_HE3_TS41_RDTEMP, "UCN_HE3_TS41_RDTEMP/D");
tSource->Branch("UCN_HE3_TS42_RDTEMP",&UCN_HE3_TS42_RDTEMP, "UCN_HE3_TS42_RDTEMP/D");
tSource->Branch("UCN_SER_HS1_RDHUM",&UCN_SER_HS1_RDHUM, "UCN_SER_HS1_RDHUM/D");
tSource->Branch("UCN_HE3_PG15_RDPRESS",&UCN_HE3_PG15_RDPRESS, "UCN_HE3_PG15_RDPRESS/D");
tSource->Branch("UCN_EXP_CG6A_RDVAC",&UCN_EXP_CG6A_RDVAC, "UCN_EXP_CG6A_RDVAC/D");
tSource->Branch("UCN_EXP_CG6B_RDVAC",&UCN_EXP_CG6B_RDVAC, "UCN_EXP_CG6B_RDVAC/D");
tSource->Branch("UCN_EXP_CG7A_RDVAC",&UCN_EXP_CG7A_RDVAC, "UCN_EXP_CG7A_RDVAC/D");
tSource->Branch("UCN_EXP_CG8A_RDVAC",&UCN_EXP_CG8A_RDVAC, "UCN_EXP_CG8A_RDVAC/D");
tSource->Branch("UCN_EXP_CG8B_RDVAC",&UCN_EXP_CG8B_RDVAC, "UCN_EXP_CG8B_RDVAC/D");
tSource->Branch("UCN_EXP_IG5_RDVAC",&UCN_EXP_IG5_RDVAC, "UCN_EXP_IG5_RDVAC/D");
tSource->Branch("UCN_EXP_IG6_RDVAC",&UCN_EXP_IG6_RDVAC, "UCN_EXP_IG6_RDVAC/D");
tSource->Branch("UCN_EXP_IG7_RDVAC",&UCN_EXP_IG7_RDVAC, "UCN_EXP_IG7_RDVAC/D");
tSource->Branch("UCN_EXP_IV2_STATON",&UCN_EXP_IV2_STATON, "UCN_EXP_IV2_STATON/D");
tSource->Branch("UCN_EXP_IV3_STATON",&UCN_EXP_IV3_STATON, "UCN_EXP_IV3_STATON/D");
tSource->Branch("UCN_EXP_CG9_RDVAC",&UCN_EXP_CG9_RDVAC, "UCN_EXP_CG9_RDVAC/D");
tSource->Branch("UCN_EXP_CG10_RDVAC",&UCN_EXP_CG10_RDVAC, "UCN_EXP_CG10_RDVAC/D");
tSource->Branch("UCN_EXP_CG11_RDVAC",&UCN_EXP_CG11_RDVAC, "UCN_EXP_CG11_RDVAC/D");

 

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

UCN_VAC_IGP1_RDVAC =  data->GetFloat()[0];
UCN_VAC_CG1A_RDVAC =  data->GetFloat()[1];
UCN_VAC_PG1_RDPRESS =  data->GetFloat()[2];
UCN_HE3_PG6L_RDPRESS =  data->GetFloat()[3];
UCN_HE3_PG6H_RDPRESS =  data->GetFloat()[4];
UCN_HE4_PG3L_RDPRESS =  data->GetFloat()[5];
UCN_HE4_PG3H_RDPRESS =  data->GetFloat()[6];
UCN_VAC_PV1_STATON =  data->GetFloat()[7];
UCN_VAC_GV1_STATON =  data->GetFloat()[8];
UCN_VAC_VV1_STATON =  data->GetFloat()[9];
UCN_VAC_CG1B_RDVAC =  data->GetFloat()[10];
UCN_ISO_CG2A_RDVAC =  data->GetFloat()[11];
UCN_ISO_CG2B_RDVAC =  data->GetFloat()[12];
UCN_UGD_IGP2_RDVAC =  data->GetFloat()[13];
UCN_ISO_PG9H_RDPRESS =  data->GetFloat()[14];
UCN_ISO_PG9L_RDPRESS =  data->GetFloat()[15];
UCN_D20_PG12_RDPRESS =  data->GetFloat()[16];
UCN_HE4_PG4_RDPRESS =  data->GetFloat()[17];
UCN_HE4_PG5_RDPRESS =  data->GetFloat()[18];
UCN_HE3_PG7_RDPRESS =  data->GetFloat()[20];
UCN_HE3_PG8_RDPRESS =  data->GetFloat()[21];
UCN_HE3_PG11_RDPRESS =  data->GetFloat()[22];
UCN_ISO_SV3_STATON =  data->GetFloat()[23];
UCN_ISO_SV4_STATON =  data->GetFloat()[24];
UCN_ISO_PV2_STATON =  data->GetFloat()[25];
UCN_ISO_SV5_STATON =  data->GetFloat()[26];
UCN_HE4_GV2_STATON =  data->GetFloat()[27];
UCN_HE3_GV3_STATON =  data->GetFloat()[28];
UCN_HE4_PG2_RDPRESS =  data->GetFloat()[29];
UCN_HE4_MP1_STATON =  data->GetFloat()[30];
UCN_HE4_MP2_STATON =  data->GetFloat()[31];
UCN_HE3_MP3_STATON =  data->GetFloat()[32];
UCN_HE3_MP4_STATON =  data->GetFloat()[33];
UCN_HE3_MP5_STATON =  data->GetFloat()[34];
UCN_VAC_BP1_STATON =  data->GetFloat()[35];
UCN_VAC_TP1_STATON =  data->GetFloat()[36];
UCN_ISO_BP2_STATON =  data->GetFloat()[37];
UCN_ISO_TP2_STATON =  data->GetFloat()[38];
UCN_HE3_PSW1_K2ON =  data->GetFloat()[40];
UCN_HE3_PSW2_K2ON =  data->GetFloat()[41];
UCN_UGD_IV1_STATON =  data->GetFloat()[42];
UCN_HE4_PG10_RDPRESS =  data->GetFloat()[44];
UCN_HE3_FM1_RDFLOW =  data->GetFloat()[45];
UCN_HE4_FM2_RDFLOW =  data->GetFloat()[46];
UCN_ISO_FM3_RDFLOW =  data->GetFloat()[47];
UCN_HE4_FM4_RDFLOW =  data->GetFloat()[48];
UCN_HE4_FM5_RDFLOW =  data->GetFloat()[49];
UCN_CRY_TS1_RDTEMP =  data->GetFloat()[50];
UCN_D2O_TS2_RDTEMP =  data->GetFloat()[51];
UCN_CRY_TS3_RDTEMP =  data->GetFloat()[52];
UCN_HE4_TS4_RDTEMP =  data->GetFloat()[53];
UCN_D2O_TS5_RDTEMP =  data->GetFloat()[54];
UCN_D2O_TS6_RDTEMP =  data->GetFloat()[55];
UCN_D2O_TS7_RDTEMP =  data->GetFloat()[56];
UCN_D2O_TS8_RDTEMP =  data->GetFloat()[57];
UCN_HE4_TS9_RDTEMP =  data->GetFloat()[58];
UCN_HE3_TS10_RDTEMP =  data->GetFloat()[59];
UCN_ISO_TS11_RDTEMP =  data->GetFloat()[60];
UCN_ISO_TS12_RDTEMP =  data->GetFloat()[61];
UCN_ISO_TS13_RDTEMP =  data->GetFloat()[62];
UCN_ISO_TS14_RDTEMP =  data->GetFloat()[63];
UCN_ISO_TS15_RDTEMP =  data->GetFloat()[64];
UCN_ISO_TS16_RDTEMP =  data->GetFloat()[65];
UCN_ISO_TS17_RDTEMP =  data->GetFloat()[66];
UCN_ISO_TS18_RDTEMP =  data->GetFloat()[67];
UCN_UGD_TS19_RDTEMP =  data->GetFloat()[68];
UCN_UGD_TS20_RDTEMP =  data->GetFloat()[69];
UCN_UGD_TS21_RDTEMP =  data->GetFloat()[70];
UCN_UGD_TS22_RDTEMP =  data->GetFloat()[71];
UCN_CRY_TS23_RDTEMP =  data->GetFloat()[72];
UCN_CRY_TS24_RDTEMP =  data->GetFloat()[73];
UCN_HE4_TS25_RDTEMP =  data->GetFloat()[74];
UCN_HE4_TS26_RDTEMP =  data->GetFloat()[75];
UCN_HE3_TS27_RDTEMP =  data->GetFloat()[76];
UCN_HE3_TS28_RDTEMP =  data->GetFloat()[77];
UCN_D2O_TS29_RDTEMP =  data->GetFloat()[78];
UCN_D2O_TS30_RDTEMP =  data->GetFloat()[79];
UCN_CRY_TS31_RDTEMP =  data->GetFloat()[80];
UCN_CRY_TS32_RDTEMP =  data->GetFloat()[81];
UCN_CRY_TS33_RDTEMP =  data->GetFloat()[82];
UCN_CRY_TS34_RDTEMP =  data->GetFloat()[83];
UCN_CRY_TS35_RDTEMP =  data->GetFloat()[84];
UCN_UGD_TS36_RDTEMP =  data->GetFloat()[85];
UCN_UGD_TS37_RDTEMP =  data->GetFloat()[86];
UCN_UGD_TS38_RDTEMP =  data->GetFloat()[87];
UCN_HE4_LVL1_RDLVL =  data->GetFloat()[88];
UCN_HE4_LVL2_RDLVL =  data->GetFloat()[89];
UCN_HE3_LVL3_RDLVL =  data->GetFloat()[90];
UCN_HE4_LVL4_RDLVL =  data->GetFloat()[91];
UCN_HE4_LVL5_RDLVL =  data->GetFloat()[92];
UCN_ISO_PG13_RDPRESS =  data->GetFloat()[93];
UCN_HE4_HTR8_SETTEMP =  data->GetFloat()[94];
UCN_HE4_HTR9_SETTEMP =  data->GetFloat()[95];
UCN_HE4_HTR10_SETTEMP =  data->GetFloat()[96];
UCN_HE4_HTR11_SETTEMP =  data->GetFloat()[97];
UCN_HE3_HTR12_SETTEMP =  data->GetFloat()[98];
UCN_CRY_CP1A_STATON =  data->GetFloat()[99];
UCN_CRY_CP1B_STATON =  data->GetFloat()[100];
UCN_HE3_CP2_STATON =  data->GetFloat()[101];
UCN_HE4_HTR8_STATON =  data->GetFloat()[102];
UCN_HE4_HTR9_STATON =  data->GetFloat()[103];
UCN_HE4_HTR10_STATON =  data->GetFloat()[104];
UCN_HE4_HTR11_STATON =  data->GetFloat()[105];
UCN_HE3_HTR12_STATON =  data->GetFloat()[106];

 if(data->GetSize() >= 108){
   
   UCN_UGD_TS49_RDTEMP = data->GetFloat()[107];
   UCN_UGD_TS50_RDTEMP = data->GetFloat()[108];
   UCN_UGD_TS51_RDTEMP = data->GetFloat()[109];
   UCN_UGD_HTR1_STATON = data->GetFloat()[110];
   UCN_UGD_HTR2_STATON = data->GetFloat()[111];
   UCN_UGD_HTR3_STATON = data->GetFloat()[112];
   UCN_UGD_HTR4_STATON = data->GetFloat()[113];
   UCN_UGD_HTR5_STATON = data->GetFloat()[114];
   UCN_ISO_IG4_RDVAC = data->GetFloat()[115];
   UCN_HE3_FPV2_RDDACP = data->GetFloat()[116];
   UCN_HE3_PG14_RDPRESS = data->GetFloat()[117];
   UCN_SER_HS1_STATON = data->GetFloat()[118];
   UCN_HE4_TS43_RDTEMP = data->GetFloat()[119];
   UCN_HE4_TS44_RDTEMP = data->GetFloat()[120];
   UCN_HE4_TS45_RDTEMP = data->GetFloat()[121];
   UCN_HE4_TS46_RDTEMP = data->GetFloat()[122];
   UCN_HE4_TS47_RDTEMP = data->GetFloat()[123];
   UCN_HE4_TS48_RDTEMP = data->GetFloat()[124];
   UCN_SER_TS52_RDTEMP = data->GetFloat()[125];
   UCN_ISO_MFC1_RDFLOW = data->GetFloat()[126];
   UCN_ISO_MFC1_FLOW = data->GetFloat()[127];
   UCN_HE3_CG3_RDVAC = data->GetFloat()[128];
   UCN_HE3_TS39_RDTEMP = data->GetFloat()[129];
   UCN_HE3_TS40_RDTEMP = data->GetFloat()[130];
   UCN_HE3_TS41_RDTEMP = data->GetFloat()[131];
   UCN_HE3_TS42_RDTEMP = data->GetFloat()[132];
   UCN_SER_HS1_RDHUM = data->GetFloat()[133];
   UCN_HE3_PG15_RDPRESS = data->GetFloat()[134];
   UCN_EXP_CG6A_RDVAC = data->GetFloat()[135];
   UCN_EXP_CG6B_RDVAC = data->GetFloat()[136];
   UCN_EXP_CG7A_RDVAC = data->GetFloat()[137];
   UCN_EXP_CG8A_RDVAC = data->GetFloat()[138];
   UCN_EXP_CG8B_RDVAC = data->GetFloat()[139];
   UCN_EXP_IG5_RDVAC = data->GetFloat()[140];
   UCN_EXP_IG6_RDVAC = data->GetFloat()[141];
   UCN_EXP_IG7_RDVAC = data->GetFloat()[142];
   UCN_EXP_IV2_STATON = data->GetFloat()[143];
   UCN_EXP_IV3_STATON = data->GetFloat()[144];
   UCN_EXP_CG9_RDVAC = data->GetFloat()[145];
   UCN_EXP_CG10_RDVAC = data->GetFloat()[146];
   UCN_EXP_CG11_RDVAC = data->GetFloat()[147];
 }

  tSource->Fill();
  
};




TUCNBeamlineEpicsTree::TUCNBeamlineEpicsTree(){

  std::cout << "Creating beamline EPICS tree "<< std::endl;

  tBeamline = new TTree("BeamlineEpicsTree", "BeamlineEpicsTree");
  //  tBeamline->Branch("tEntry",&tEntry, "tEntry/l" );
  tBeamline->Branch("timestamp", &timestamp, "timestamp/I" );
  
  tBeamline->Branch("B1U_TNIM2_RAW",&B1U_TNIM2_RAW, "B1U_TNIM2_RAW/D");
  tBeamline->Branch("B1U_SEPT_STATON",&B1U_SEPT_STATON, "B1U_SEPT_STATON/D");
  tBeamline->Branch("B1U_YCB0_STATON",&B1U_YCB0_STATON, "B1U_YCB0_STATON/D");
  tBeamline->Branch("B1U_B0_STATON",&B1U_B0_STATON, "B1U_B0_STATON/D");
  tBeamline->Branch("B1U_Q1_STATON",&B1U_Q1_STATON, "B1U_Q1_STATON/D");
  tBeamline->Branch("B1U_Q2_STATON",&B1U_Q2_STATON, "B1U_Q2_STATON/D");
  tBeamline->Branch("B1U_IV0_STATON",&B1U_IV0_STATON, "B1U_IV0_STATON/D");
  tBeamline->Branch("B1U_IV2_STATON",&B1U_IV2_STATON, "B1U_IV2_STATON/D");
  tBeamline->Branch("B1U_PNG0_RDVAC",&B1U_PNG0_RDVAC, "B1U_PNG0_RDVAC/D");
  tBeamline->Branch("B1U_PNG2_RDVAC",&B1U_PNG2_RDVAC, "B1U_PNG2_RDVAC/D");
  tBeamline->Branch("B1U_TGTTEMP1_RDTEMP",&B1U_TGTTEMP1_RDTEMP, "B1U_TGTTEMP1_RDTEMP/D");
  tBeamline->Branch("B1U_TGTTEMP2_RDTEMP",&B1U_TGTTEMP2_RDTEMP, "B1U_TGTTEMP2_RDTEMP/D");
  tBeamline->Branch("B1U_WTEMP1_RDTEMP",&B1U_WTEMP1_RDTEMP, "B1U_WTEMP1_RDTEMP/D");
  tBeamline->Branch("B1U_WTEMP2_RDTEMP",&B1U_WTEMP2_RDTEMP, "B1U_WTEMP2_RDTEMP/D");
  tBeamline->Branch("B1U_COL2RIGHT_RDTEMP",&B1U_COL2RIGHT_RDTEMP, "B1U_COL2RIGHT_RDTEMP/D");
  tBeamline->Branch("B1U_COL2DOWN_RDTEMP",&B1U_COL2DOWN_RDTEMP, "B1U_COL2DOWN_RDTEMP/D");
  tBeamline->Branch("B1U_COL2LEFT_RDTEMP",&B1U_COL2LEFT_RDTEMP, "B1U_COL2LEFT_RDTEMP/D");
  tBeamline->Branch("B1U_COL2UP_RDTEMP",&B1U_COL2UP_RDTEMP, "B1U_COL2UP_RDTEMP/D");
  tBeamline->Branch("B1U_TNIM2_1SECAVG",&B1U_TNIM2_1SECAVG, "B1U_TNIM2_1SECAVG/D");
  tBeamline->Branch("B1U_TNIM2_10SECAVG",&B1U_TNIM2_10SECAVG, "B1U_TNIM2_10SECAVG/D");
  tBeamline->Branch("B1U_TNIM2_5MINAVG",&B1U_TNIM2_5MINAVG, "B1U_TNIM2_5MINAVG/D");
  tBeamline->Branch("B1U_TNIM2_10MINAVG",&B1U_TNIM2_10MINAVG, "B1U_TNIM2_10MINAVG/D");
  tBeamline->Branch("B1U_TNIM2_1SECTRIP",&B1U_TNIM2_1SECTRIP, "B1U_TNIM2_1SECTRIP/D");
  tBeamline->Branch("B1U_TNIM2_10SECTRIP",&B1U_TNIM2_10SECTRIP, "B1U_TNIM2_10SECTRIP/D");
  tBeamline->Branch("B1U_TNIM2_10MINTRIP",&B1U_TNIM2_10MINTRIP, "B1U_TNIM2_10MINTRIP/D");
  tBeamline->Branch("B1V_KICK_RDHICUR",&B1V_KICK_RDHICUR, "B1V_KICK_RDHICUR/D");
  tBeamline->Branch("B1V_KSM_RDMODE_VAL1",&B1V_KSM_RDMODE_VAL1, "B1V_KSM_RDMODE_VAL1/D");
  tBeamline->Branch("B1V_KSM_RDFRCTN_VAL1",&B1V_KSM_RDFRCTN_VAL1, "B1V_KSM_RDFRCTN_VAL1/D");
  tBeamline->Branch("B1V_KSM_BONPRD",&B1V_KSM_BONPRD, "B1V_KSM_BONPRD/D");
  tBeamline->Branch("B1V_KSM_RDBEAMON_VAL1",&B1V_KSM_RDBEAMON_VAL1, "B1V_KSM_RDBEAMON_VAL1/D");
  tBeamline->Branch("B1V_KSM_RDBEAMOFF_VAL1",&B1V_KSM_RDBEAMOFF_VAL1, "B1V_KSM_RDBEAMOFF_VAL1/D");
  tBeamline->Branch("B1V_KSM_INSEQ",&B1V_KSM_INSEQ, "B1V_KSM_INSEQ/D");
  tBeamline->Branch("B1UT_PT01_RDPRESS",&B1UT_PT01_RDPRESS, "B1UT_PT01_RDPRESS/D");
  tBeamline->Branch("B1UT_PT02_RDPRESS",&B1UT_PT02_RDPRESS, "B1UT_PT02_RDPRESS/D");
  tBeamline->Branch("B1UT_PT50_RDPRESS",&B1UT_PT50_RDPRESS, "B1UT_PT50_RDPRESS/D");
  tBeamline->Branch("B1UT_LM50_RDLVL",&B1UT_LM50_RDLVL, "B1UT_LM50_RDLVL/D");
  tBeamline->Branch("B1V_KSM_PREDCUR",&B1V_KSM_PREDCUR, "B1V_KSM_PREDCUR/D");
  tBeamline->Branch("B1_FOIL_ADJCUR",&B1_FOIL_ADJCUR, "B1_FOIL_ADJCUR/D");


  B1V_KSM_PREDCUR = 0;
  B1_FOIL_ADJCUR = 0;

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

  B1U_TNIM2_RAW =  data->GetFloat()[0];
  B1U_SEPT_STATON =  data->GetFloat()[1];
  B1U_YCB0_STATON =  data->GetFloat()[2];
  B1U_B0_STATON =  data->GetFloat()[3];
  B1U_Q1_STATON =  data->GetFloat()[4];
  B1U_Q2_STATON =  data->GetFloat()[5];
  B1U_IV0_STATON =  data->GetFloat()[6];
  B1U_IV2_STATON =  data->GetFloat()[7];
  B1U_PNG0_RDVAC =  data->GetFloat()[8];
  B1U_PNG2_RDVAC =  data->GetFloat()[9];
  B1U_TGTTEMP1_RDTEMP =  data->GetFloat()[10];
  B1U_TGTTEMP2_RDTEMP =  data->GetFloat()[11];
  B1U_WTEMP1_RDTEMP =  data->GetFloat()[12];
  B1U_WTEMP2_RDTEMP =  data->GetFloat()[13];
  B1U_COL2RIGHT_RDTEMP =  data->GetFloat()[14];
  B1U_COL2DOWN_RDTEMP =  data->GetFloat()[15];
  B1U_COL2LEFT_RDTEMP =  data->GetFloat()[16];
  B1U_COL2UP_RDTEMP =  data->GetFloat()[17];
  B1U_TNIM2_1SECAVG =  data->GetFloat()[18];
  B1U_TNIM2_10SECAVG =  data->GetFloat()[20];
  B1U_TNIM2_5MINAVG =  data->GetFloat()[21];
  B1U_TNIM2_10MINAVG =  data->GetFloat()[22];
  B1U_TNIM2_1SECTRIP =  data->GetFloat()[23];
  B1U_TNIM2_10SECTRIP =  data->GetFloat()[24];
  B1U_TNIM2_10MINTRIP =  data->GetFloat()[25];
  B1V_KICK_RDHICUR =  data->GetFloat()[26];
  B1V_KSM_RDMODE_VAL1 =  data->GetFloat()[27];
  B1V_KSM_RDFRCTN_VAL1 =  data->GetFloat()[28];
  B1V_KSM_BONPRD =  data->GetFloat()[29];
  B1V_KSM_RDBEAMON_VAL1 =  data->GetFloat()[30];
  B1V_KSM_RDBEAMOFF_VAL1 =  data->GetFloat()[31];
  B1V_KSM_INSEQ =  data->GetFloat()[32];
  B1UT_PT01_RDPRESS =  data->GetFloat()[33];
  B1UT_PT02_RDPRESS =  data->GetFloat()[34];
  B1UT_PT50_RDPRESS =  data->GetFloat()[35];
  B1UT_LM50_RDLVL =  data->GetFloat()[36];
  if(data->GetSize() >= 39){
    B1V_KSM_PREDCUR = data->GetFloat()[37];
    B1_FOIL_ADJCUR = data->GetFloat()[38];
  }
  tBeamline->Fill();
  
};
