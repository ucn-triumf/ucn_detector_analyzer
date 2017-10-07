#ifndef TUCNHit_h
#define TUCNHit_h


// A generic class for storing UCN hits from 
// either detector... hit fields mean different 
// things for different detectors.
class TUCNHit{
public:
  TUCNHit();
    

  int module;
  int channel;
  bool isLi6;
  double hitTime; // Unix time (with sub-second precision)
  double chargeLong; // This is just charge for He-3
  double chargeShort; // This is 0 for He-3
  double psd; // This is 0 for He-3

};

#endif
