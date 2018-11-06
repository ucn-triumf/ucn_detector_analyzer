#ifndef TUCNCycleParameters_h
#define TUCNCycleParameters_h

#include <string>
#include "TDataContainer.hxx"

// Maximum number of periods per cycle
static const int MaxPeriods = 10;
// Maximum number of cycles per super-cycle
static const int MaxCycles = 20;


/// Class for storing the parameters for the cycle
/// Save information from 2018 sequencer
class  TUCNCycleParameters {
 public:
  TUCNCycleParameters();
  virtual ~TUCNCycleParameters(){};  

  int SetCycleParameters(TGenericData *nseq);
  
private:
  
  int cycleIndex;
  bool enable;  // enable the sequencing
  int numberPeriodsInCycle; // number of periods in cycle.
  int numberCyclesInSuper; // number of cycles in super-cycle
  int numberSuperCycles; // number of super-cycles to do
  bool infiniteCycles; // Should we just continue super-cycles infinitely?
  //bool Valve1State[MaxPeriods]; // valve 1 state for each period in a cycle.
  double DurationTimePeriod[MaxPeriods][MaxCycles]; // Time in seconds for periods for each cycle.
  int Valve1State[MaxPeriods]; // valve 1 state for each period in a cycle.
  int Valve2State[MaxPeriods];
  int Valve3State[MaxPeriods];
  int Valve4State[MaxPeriods];
  int Valve5State[MaxPeriods];
  int Valve6State[MaxPeriods];
  int Valve7State[MaxPeriods];
  int Valve8State[MaxPeriods];

  

};

#endif
