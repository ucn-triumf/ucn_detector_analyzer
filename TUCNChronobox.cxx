#include "TUCNChronobox.hxx"


TUCNChronobox::TUCNChronobox(){

  fNumberRollovers= 0;
  fRolloverWord = 0;
  fPPSTimestamp = -1;
  for(int i = 0; i < MaxChan; i++) fTimestamps[i] = 0;


}

#include <sys/time.h>

int maxmax = 0;
int TUCNChronobox::ProcessMidasEvent(TDataContainer& dataContainer){
  
  TGenericData *data = dataContainer.GetEventData<TGenericData>("CBS0");
  // std::cout << "Check crhon" << std::endl;
  if(data){  
    //std::cout << "Got chronobox bank. size :" << data->GetSize()  << std::endl;
    int nwords = data->GetSize();

    struct timeval tp;
    gettimeofday(&tp,NULL);


    for(int i = 0; i < nwords; i++){
      uint32_t word = data->GetData32()[i];
      if((word & 0xff000000) == 0xff000000){
	fRolloverWord = word;
      }else if((word & 0xf0000000) == 0x80000000){	
	int ch = (word & 0x7f000000) >> 24;
	uint32_t timestamp = (word & 0xffffff);

	// Yikes, don't understand why this roll-over calculation necessary...
	int rollovers = (int)((fRolloverWord & 0xffff)-1)/2;

	if(!(ch >= 0 && ch < MaxChan)){
	  std::cout << "Bad channel number: " << ch << std::endl;
	  continue;
	}
	double time_precision = 0.00000016; // in seconds

	if(timestamp > maxmax) maxmax = timestamp;
	//	if(topeight == 128){std::cout << "Sub " << std::endl; rollovers -= 1;}
	double full_time = ((double)timestamp)*time_precision 
	  + ((double)0xffffff)*time_precision*((double) rollovers);

	std::cout << "Time differences: " << full_time - fTimestamps[ch] << " "
		  << full_time - fTimestamps[0] << " ch=" << ch << std::endl;
	if(0)	std::cout << full_time - fTimestamps[ch] << i << " |||" << ch << "||| " << std::dec << timestamp << " "
		  <<" " << rollovers << " " 
		  << full_time << " " << fTimestamps[ch] << " " 
		  << full_time - fTimestamps[ch] << " " 
		  << maxmax << std::dec << std::endl;
	fTimestamps[ch] = full_time;
      
      }
    }
  }

  return 1;

}
