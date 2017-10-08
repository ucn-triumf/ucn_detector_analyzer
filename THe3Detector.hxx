#ifndef THe3Detector_h
#define THe3Detector_h

#include "TUCNDetectorBaseClass.hxx"

// A derived class from TUCNDetectorBaseClass...
// This class handles getting the hardware level 
// information and converting it into the 
// standard TUCNHit format...
class THe3Detector : public TUCNDetectorBaseClass {

public:
  THe3Detector(bool isOffline);

  void GetHits(TDataContainer& dataContainer);

};

#endif
