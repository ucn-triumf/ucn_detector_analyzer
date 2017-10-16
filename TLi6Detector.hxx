#ifndef TLi6Detector_h
#define TLi6Detector_h

#include "TUCNDetectorBaseClass.hxx"

// A derived class from TUCNDetectorBaseClass...
// This class handles getting the hardware level 
// information and converting it into the 
// standard TUCNHit format...
class TLi6Detector : public TUCNDetectorBaseClass {

public:
  TLi6Detector(bool isOffline);

  void GetHits(TDataContainer& dataContainer);

};

#endif
