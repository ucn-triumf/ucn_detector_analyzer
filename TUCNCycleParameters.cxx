#include "TUCNCycleParameters.hxx"


TUCNCycleParameters::TUCNCycleParameters(){

  cycleIndex = 0;
  enable = 0;
  numberPeriodsInCycle = 0;
  numberCyclesInSuper = 0;
  numberSuperCycles = 0;
  infiniteCycles = 0;

  for(int i = 0; i < MaxPeriods; i++){
    for(int j = 0; j < MaxCycles; j++){
      DurationTimePeriod[i][j] = 0; // Time in seconds for periods for each cycle.
    }
    Valve1State[i] = 0; // valve 1 state for each period in a cycle.
    Valve2State[i] = 0;
    Valve3State[i] = 0;
    Valve4State[i] = 0;
    Valve5State[i] = 0;
    Valve6State[i] = 0;
    Valve7State[i] = 0;
    Valve8State[i] = 0;
  }
}


int TUCNCycleParameters::SetCycleParameters(TGenericData *nseq){

  
  //  std::cout << "Bank Size " << nseq->GetSize() << std::endl;  
  if(nseq->GetSize() != 285){
    std::cout << "Bank NSEQ has wrong size: " << nseq->GetSize() <<std::endl;
    return -1;
  }

  int index = 0;
  cycleIndex = (int)nseq->GetDouble()[index++];
  enable = (bool)nseq->GetDouble()[index++];
  numberPeriodsInCycle = (int)nseq->GetDouble()[index++];
  numberCyclesInSuper = (int)nseq->GetDouble()[index++];
  infiniteCycles = (int)nseq->GetDouble()[index++];

  for(int i = 0; i < MaxPeriods; i++){
    for(int j = 0; j < MaxCycles; j++){
      DurationTimePeriod[i][j] = nseq->GetDouble()[index++]; // Time in seconds for periods for each cycle.
    }
  }
  
  for(int i = 0; i < MaxPeriods; i++){
    Valve1State[i] = (int)nseq->GetDouble()[index++]; // valve 1 state for each period in a cycle.
    Valve2State[i] = (int)nseq->GetDouble()[index++];
    Valve3State[i] = (int)nseq->GetDouble()[index++];
    Valve4State[i] = (int)nseq->GetDouble()[index++];
    Valve5State[i] = (int)nseq->GetDouble()[index++];
    Valve6State[i] = (int)nseq->GetDouble()[index++];
    Valve7State[i] = (int)nseq->GetDouble()[index++];
    Valve8State[i] = (int)nseq->GetDouble()[index++];
  } 
  
  
  std::cout << "Cycle index : " << cycleIndex << std::endl;
  std::cout << "Number periods: " <<   numberPeriodsInCycle << std::endl;
  std::cout << "Number cycles: " << numberCyclesInSuper << std::endl;
  std::cout << "This cycle summary: Duration (Valve1,Valve2,etc) " << std::endl;
  for(int k = 0; k < numberPeriodsInCycle; k++){
    std::cout << DurationTimePeriod[k][cycleIndex] << "s ("<< Valve1State[k]
              << ","<< Valve2State[k]
              << ","<< Valve3State[k]
              << ","<< Valve4State[k] << ")" << std::endl;
  }

  return 1;


}
