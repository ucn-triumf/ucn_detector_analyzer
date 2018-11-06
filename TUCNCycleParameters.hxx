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

  TUCNCycleParameters(const TUCNCycleParameters &par){

   for(int i = 0; i < MaxPeriods; i++){
     for(int j = 0; j < MaxCycles; j++){
       DurationTimePeriod[i][j] = par.DurationTimePeriod[i][j];
     }
   }
   
  cycleIndex = par.cycleIndex;
  enable = par.enable;
  numberPeriodsInCycle = par.numberPeriodsInCycle;
  numberCyclesInSuper = par.numberCyclesInSuper;
  infiniteCycles = par.infiniteCycles;

  for(int i = 0; i < MaxPeriods; i++){
    Valve1State[i] = par.Valve1State[i];
    Valve2State[i] = par.Valve2State[i];
    Valve3State[i] = par.Valve3State[i];
    Valve4State[i] = par.Valve4State[i];
    Valve5State[i] = par.Valve5State[i];
    Valve6State[i] = par.Valve6State[i];
    Valve7State[i] = par.Valve7State[i];
    Valve8State[i] = par.Valve8State[i];
  } 
  



    
  }
 
  int SetCycleParameters(TGenericData *nseq);

  // Get the cumulative time of all the periods up to the specified one.
  // Can specify the cycle. Otherwise uses current cycle.
  double GetCumulativeTimeForPeriod(int period, int cycle = -1);

  int CycleIndex(){return cycleIndex;};
  int NumberPeriods(){return numberPeriodsInCycle;};

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
