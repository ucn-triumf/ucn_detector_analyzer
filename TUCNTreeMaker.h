#ifndef TUCNTreeMaker_h
#define TUCNTreeMaker_h

// Use this list here to decide which type of equipment to use.
#include <string>
#include "TTree.h"
#include "TUCNHit.hxx"

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



#endif


