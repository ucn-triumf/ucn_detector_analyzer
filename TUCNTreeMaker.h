#ifndef TUCNTreeMaker_h
#define TUCNTreeMaker_h

// This file contains classes for making


#include <string>
#include "TTree.h"
#include "TUCNHit.hxx"
#include "TDataContainer.hxx"

// This class will make the tree of UCN hits and the separate tree of sequence transitions
class TUCNHitsTree {
 public:
  
  TUCNHitsTree(std::string name);

  void FillHits(TUCNHitCollection& hits, int isUCN);

  
 private:

  std::string detector_name;
  
  TTree * tRunTran; // run transitions
  int     tRunNum;
  int     tTime;


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


// This class will make the tree of source epics variables.
// Currently we only save a small number of EPICS variables.  Others should
// add more...
class TUCNSourceEpicsTree {
 public:
  
  TUCNSourceEpicsTree();

  void FillTree(TDataContainer& dataContainer);

  
 private:

  TTree *tSource;

  int timestamp;
  double UCN_VAC_IGP1_RDVAC;
  double UCN_HE4_FM4_RDFLOW;
  double UCN_D2O_TS7_RDTEMP;
  double UCN_HE4_LVL1_RDLVL;

};


#endif


