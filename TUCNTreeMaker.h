#ifndef TUCNTreeMaker_h
#define TUCNTreeMaker_h

// This file contains classes for making


#include <string>
#include "TTree.h"
#include "TUCNHit.hxx"
#include "TDataContainer.hxx"
#include "TUCNCycleParameters.hxx"


// This class will make the tree of UCN hits and the separate tree of sequence transitions
class TUCNHitsTree {
 public:
  
  TUCNHitsTree(std::string name);

  void FillHits(TUCNHitCollection& hits, int isUCN);
  void FillTransition(double icycleStartTime, double icycleValveOpenTime, double icycleValveCloseTime,
                      double icycleDelayTime, double icycleOpenInterval, TUCNCycleParameters CycleParameters);

  
 private:

  std::string detector_name;
  
  
  TTree * tRunTran; // run transitions
  int     tRunNum;
  int     tTime;
  int     cycleIndex;          // cycle index
  double  cycleStartTime;      // time of start of new cycle.
  double  cycleValveOpenTime;  // time that valve opened
  double  cycleValveCloseTime; // time that valve closed
  double  cycleDelayTime;      // delay between end of irradiation and opening valve
  double  cycleOpenInterval;   // length of time that valve is open
  double  cyclePeriod0EndTime; // end of period 0
  double  cyclePeriod1EndTime; // end of period 1
  double  cyclePeriod2EndTime; // end of period 2
  double  cyclePeriod3EndTime; // end of period 3
  double  cyclePeriod4EndTime; // end of period 4
  double  cyclePeriod5EndTime; // end of period 5
  double  cyclePeriod6EndTime; // end of period 6
  double  cyclePeriod7EndTime; // end of period 7
  double  cyclePeriod8EndTime; // end of period 8
  double  cyclePeriod9EndTime; // end of period 9
  int valveStatePeriod0[8]; // state of the 8 valves for period 0. array index = 0 means valve 1.
  int valveStatePeriod1[8]; // state of the 8 valves for period 1. array index = 0 means valve 1.
  int valveStatePeriod2[8]; // state of the 8 valves for period 2. array index = 0 means valve 1.
  int valveStatePeriod3[8]; // state of the 8 valves for period 3. array index = 0 means valve 1.
  int valveStatePeriod4[8]; // state of the 8 valves for period 4. array index = 0 means valve 1.
  int valveStatePeriod5[8]; // state of the 8 valves for period 5. array index = 0 means valve 1.
  int valveStatePeriod6[8]; // state of the 8 valves for period 6. array index = 0 means valve 1.
  int valveStatePeriod7[8]; // state of the 8 valves for period 7. array index = 0 means valve 1.
  int valveStatePeriod8[8]; // state of the 8 valves for period 8. array index = 0 means valve 1.
  int valveStatePeriod9[8]; // state of the 8 valves for period 9. array index = 0 means valve 1.

  

  // output file
  TTree * tUCN;

  // branches

  // tUCN
  ULong_t tEntry;
  ULong_t tEvent;
  ULong64_t tTimeE;
  ULong64_t tTimeStamp;
  double tUnixTime;
  double tUnixTimePrecise;
  Short_t tChannel;
  Short_t tLength;
  //  Short_t tPulse[1000];
  Float_t tPSD;
  Short_t tChargeL;
  Short_t tChargeS;
  Short_t tBaseline;
  Short_t tIsUCN;


};


// This class will make the tree of LND thermal variables.
class TLNDDetectorTree {
 public:
  
  TLNDDetectorTree();

  void FillTree(TDataContainer& dataContainer);

  
 private:

  TTree *tLND;

  int timestamp;
  double LND_Reading;
};



// This class will make the tree of SCM variables.
class TSCMTree {
 public:
  
  TSCMTree();

  void FillTree(TDataContainer& dataContainer);

  
 private:

  TTree *tSCM;

  int timestamp;
  float SCMTemps[8];
  float SCMVoltages[4];
};



// This class will make the tree of Sequencer variables.
class TSequencerTree {
 public:
  TSequencerTree();

  void FillTree(TDataContainer& dataContainer);
  
 private:

  TTree *tSequencer;

  int timestamp;
  int sequencerEnabled; // Is sequencer enabled
  int inCycle; // Are we in a cycle
  int cycleStarted; // Did cycleJust start?
};


// This class will make the tree of source epics variables.
class TUCNSourceEpicsTree {
 public:
  
  TUCNSourceEpicsTree();

  void FillTree(TDataContainer& dataContainer);

  
 private:

  TTree *tSource;

  int timestamp;
// midas2root code fragments generated by epics2mdias/generate_epics_code_fragment.py
double UCN_VAC_IGP1_RDVAC;
double UCN_VAC_CG1A_RDVAC;
double UCN_VAC_PG1_RDPRESS;
double UCN_HE3_PG6L_RDPRESS;
double UCN_HE3_PG6H_RDPRESS;
double UCN_HE4_PG3L_RDPRESS;
double UCN_HE4_PG3H_RDPRESS;
double UCN_VAC_PV1_STATON;
double UCN_VAC_GV1_STATON;
double UCN_VAC_VV1_STATON;
double UCN_VAC_CG1B_RDVAC;
double UCN_ISO_CG2A_RDVAC;
double UCN_ISO_CG2B_RDVAC;
double UCN_UGD_IGP2_RDVAC;
double UCN_ISO_PG9H_RDPRESS;
double UCN_ISO_PG9L_RDPRESS;
double UCN_D20_PG12_RDPRESS;
double UCN_HE4_PG4_RDPRESS;
double UCN_HE4_PG5_RDPRESS;
double UCN_HE3_PG7_RDPRESS;
double UCN_HE3_PG8_RDPRESS;
double UCN_HE3_PG11_RDPRESS;
double UCN_ISO_SV3_STATON;
double UCN_ISO_SV4_STATON;
double UCN_ISO_PV2_STATON;
double UCN_ISO_SV5_STATON;
double UCN_HE4_GV2_STATON;
double UCN_HE3_GV3_STATON;
double UCN_HE4_PG2_RDPRESS;
double UCN_HE4_MP1_STATON;
double UCN_HE4_MP2_STATON;
double UCN_HE3_MP3_STATON;
double UCN_HE3_MP4_STATON;
double UCN_HE3_MP5_STATON;
double UCN_VAC_BP1_STATON;
double UCN_VAC_TP1_STATON;
double UCN_ISO_BP2_STATON;
double UCN_ISO_TP2_STATON;
double UCN_HE3_PSW1_K2ON;
double UCN_HE3_PSW2_K2ON;
double UCN_UGD_IV1_STATON;
double UCN_HE4_PG10_RDPRESS;
double UCN_HE3_FM1_RDFLOW;
double UCN_HE4_FM2_RDFLOW;
double UCN_ISO_FM3_RDFLOW;
double UCN_HE4_FM4_RDFLOW;
double UCN_HE4_FM5_RDFLOW;
double UCN_CRY_TS1_RDTEMP;
double UCN_D2O_TS2_RDTEMP;
double UCN_CRY_TS3_RDTEMP;
double UCN_HE4_TS4_RDTEMP;
double UCN_D2O_TS5_RDTEMP;
double UCN_D2O_TS6_RDTEMP;
double UCN_D2O_TS7_RDTEMP;
double UCN_D2O_TS8_RDTEMP;
double UCN_HE4_TS9_RDTEMP;
double UCN_HE3_TS10_RDTEMP;
double UCN_ISO_TS11_RDTEMP;
double UCN_ISO_TS12_RDTEMP;
double UCN_ISO_TS13_RDTEMP;
double UCN_ISO_TS14_RDTEMP;
double UCN_ISO_TS15_RDTEMP;
double UCN_ISO_TS16_RDTEMP;
double UCN_ISO_TS17_RDTEMP;
double UCN_ISO_TS18_RDTEMP;
double UCN_UGD_TS19_RDTEMP;
double UCN_UGD_TS20_RDTEMP;
double UCN_UGD_TS21_RDTEMP;
double UCN_UGD_TS22_RDTEMP;
double UCN_CRY_TS23_RDTEMP;
double UCN_CRY_TS24_RDTEMP;
double UCN_HE4_TS25_RDTEMP;
double UCN_HE4_TS26_RDTEMP;
double UCN_HE3_TS27_RDTEMP;
double UCN_HE3_TS28_RDTEMP;
double UCN_D2O_TS29_RDTEMP;
double UCN_D2O_TS30_RDTEMP;
double UCN_CRY_TS31_RDTEMP;
double UCN_CRY_TS32_RDTEMP;
double UCN_CRY_TS33_RDTEMP;
double UCN_CRY_TS34_RDTEMP;
double UCN_CRY_TS35_RDTEMP;
double UCN_UGD_TS36_RDTEMP;
double UCN_UGD_TS37_RDTEMP;
double UCN_UGD_TS38_RDTEMP;
double UCN_HE4_LVL1_RDLVL;
double UCN_HE4_LVL2_RDLVL;
double UCN_HE3_LVL3_RDLVL;
double UCN_HE4_LVL4_RDLVL;
double UCN_HE4_LVL5_RDLVL;
double UCN_ISO_PG13_RDPRESS;
double UCN_HE4_HTR8_SETTEMP;
double UCN_HE4_HTR9_SETTEMP;
double UCN_HE4_HTR10_SETTEMP;
double UCN_HE4_HTR11_SETTEMP;
double UCN_HE3_HTR12_SETTEMP;
double UCN_CRY_CP1A_STATON;
double UCN_CRY_CP1B_STATON;
double UCN_HE3_CP2_STATON;
double UCN_HE4_HTR8_STATON;
double UCN_HE4_HTR9_STATON;
double UCN_HE4_HTR10_STATON;
double UCN_HE4_HTR11_STATON;
double UCN_HE3_HTR12_STATON;
double UCN_UGD_TS49_RDTEMP;
double UCN_UGD_TS50_RDTEMP;
double UCN_UGD_TS51_RDTEMP;
double UCN_UGD_HTR1_STATON;
double UCN_UGD_HTR2_STATON;
double UCN_UGD_HTR3_STATON;
double UCN_UGD_HTR4_STATON;
double UCN_UGD_HTR5_STATON;
double UCN_ISO_IG4_RDVAC;
double UCN_HE3_FPV2_RDDACP;
double UCN_HE3_PG14_RDPRESS;
double UCN_SER_HS1_STATON;
double UCN_HE4_TS43_RDTEMP;
double UCN_HE4_TS44_RDTEMP;
double UCN_HE4_TS45_RDTEMP;
double UCN_HE4_TS46_RDTEMP;
double UCN_HE4_TS47_RDTEMP;
double UCN_HE4_TS48_RDTEMP;
double UCN_SER_TS52_RDTEMP;
double UCN_ISO_MFC1_RDFLOW;
double UCN_ISO_MFC1_FLOW;
double UCN_HE3_CG3_RDVAC;
double UCN_HE3_TS39_RDTEMP;
double UCN_HE3_TS40_RDTEMP;
double UCN_HE3_TS41_RDTEMP;
double UCN_HE3_TS42_RDTEMP;
double UCN_SER_HS1_RDHUM;
double UCN_HE3_PG15_RDPRESS;
double UCN_EXP_CG6A_RDVAC;
double UCN_EXP_CG6B_RDVAC;
double UCN_EXP_CG7A_RDVAC;
double UCN_EXP_CG8A_RDVAC;
double UCN_EXP_CG8B_RDVAC;
double UCN_EXP_IG5_RDVAC;
double UCN_EXP_IG6_RDVAC;
double UCN_EXP_IG7_RDVAC;
double UCN_EXP_IV2_STATON;
double UCN_EXP_IV3_STATON;
double UCN_EXP_CG9_RDVAC;
double UCN_EXP_CG10_RDVAC;
double UCN_EXP_CG11_RDVAC;

 

};




// This class will make the tree of beamline epics variables.
// Currently we only save a fraction of EPICS variables.  Others should
// add more...
class TUCNBeamlineEpicsTree {
 public:
  
  TUCNBeamlineEpicsTree();

  void FillTree(TDataContainer& dataContainer);

  
 private:

  TTree *tBeamline;

  int timestamp;

// midas2root code fragments generated by epics2mdias/generate_epics_code_fragment.py
  double B1U_TNIM2_RAW;
  double B1U_SEPT_STATON;
  double B1U_YCB0_STATON;
  double B1U_B0_STATON;
  double B1U_Q1_STATON;
  double B1U_Q2_STATON;
  double B1U_IV0_STATON;
  double B1U_IV2_STATON;
  double B1U_PNG0_RDVAC;
  double B1U_PNG2_RDVAC;
  double B1U_TGTTEMP1_RDTEMP;
  double B1U_TGTTEMP2_RDTEMP;
  double B1U_WTEMP1_RDTEMP;
  double B1U_WTEMP2_RDTEMP;
  double B1U_COL2RIGHT_RDTEMP;
  double B1U_COL2DOWN_RDTEMP;
  double B1U_COL2LEFT_RDTEMP;
  double B1U_COL2UP_RDTEMP;
  double B1U_TNIM2_1SECAVG;
  double B1U_TNIM2_10SECAVG;
  double B1U_TNIM2_5MINAVG;
  double B1U_TNIM2_10MINAVG;
  double B1U_TNIM2_1SECTRIP;
  double B1U_TNIM2_10SECTRIP;
  double B1U_TNIM2_10MINTRIP;
  double B1V_KICK_RDHICUR;
  double B1V_KSM_RDMODE_VAL1;
  double B1V_KSM_RDFRCTN_VAL1;
  double B1V_KSM_BONPRD;
  double B1V_KSM_RDBEAMON_VAL1;
  double B1V_KSM_RDBEAMOFF_VAL1;
  double B1V_KSM_INSEQ;
  double B1UT_PT01_RDPRESS;
  double B1UT_PT02_RDPRESS;
  double B1UT_PT50_RDPRESS;
  double B1UT_LM50_RDLVL;
  double B1V_KSM_PREDCUR;
  double B1_FOIL_ADJCUR;



};


#endif


