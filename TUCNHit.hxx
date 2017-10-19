#ifndef TUCNHit_h
#define TUCNHit_h

#include<vector>

// A generic class for storing UCN hits from 
// either detector... hit fields mean different 
// things for different detectors.
class TUCNHit{
public:
  TUCNHit();
    

  int module;
  int channel;
  bool isLi6;
  double time; // Unix time (with sub-second precision)
  uint64_t clockTime;
  double chargeLong; // This is just change for He-3
  double chargeShort; // This is just charge for He-3
  double psd; // This is 0 for He-3
  double baseline; // This is 0 for He-3

};



// a set of hits, with the time of current event.
class TUCNHitCollection : public std::vector<TUCNHit>{
public:

  TUCNHitCollection(){eventTime = 999999;}
  TUCNHitCollection(int thisEventTime){eventTime = thisEventTime;}

  int eventTime;

};


#endif
