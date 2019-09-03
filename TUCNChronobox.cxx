#include "TUCNChronobox.hxx"
#include "TH1D.h"

TUCNChronobox::TUCNChronobox(){

  fNumberRollovers= 0;
  fRolloverWord = 0;
  fPPSTimestamp = -1;
  for(int i = 0; i < MaxChan; i++){
    fTimestamps[i][0] = 0;
    fTimestamps[i][1] = 0;
  }

  fLeadingDiff = new TH1D("LeadingDiff","LeadingDiff",500000,-1,2);
  fFallingDiff = new TH1D("FallingDiff","FallingDiff",500000,-1,2);
  fChronoWidth= new TH1D("ChronoWidth","ChronoDiff",500000,-1,2);

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
	uint32_t timestamp = (word & 0xfffffe) >> 1;
	int leading = (word & 0x1);

	// Yikes, don't understand why this roll-over calculation necessary...
	int rollovers = (int)((fRolloverWord & 0xffff))/2;
	//rollovers = (int)((fRolloverWord & 0xffff))/2;

	if(!(ch >= 0 && ch < MaxChan)){
	  std::cout << "Bad channel number: " << ch << std::endl;
	  continue;
	}
	double time_precision = 0.0000002; // in seconds

	if(timestamp > maxmax) maxmax = timestamp;
	//	if(topeight == 128){std::cout << "Sub " << std::endl; rollovers -= 1;}
	double full_time = ((double)timestamp)*time_precision 
	  + ((double)0x7fffff)*time_precision*((double) rollovers);
	
	if(ch == 4){
	  if(leading){
	    fLeadingDiff->Fill(full_time - fTimestamps[ch][leading]);
	    fChronoWidth->Fill(full_time - fTimestamps[ch][0]);
	  }else{
	    fFallingDiff->Fill(full_time - fTimestamps[ch][leading]);
	    fChronoWidth->Fill(full_time - fTimestamps[ch][1]);
	  }

	  std::cout << "Time differences: " << full_time - fTimestamps[ch][leading] << " "
		    << full_time - fTimestamps[0][0] << " ch=" << ch << " leading="<<leading <<std::endl;
	  if(1)	std::cout << full_time - fTimestamps[ch][leading] << " " << i << " |||" << ch << "||| " << std::dec << timestamp << " "
			  <<" " << rollovers << " " 
			  << full_time << " " << fTimestamps[ch][leading] << " " 
			  << full_time - fTimestamps[ch][leading] << " " 
			  << maxmax << std::dec << std::endl;
	  fTimestamps[ch][leading] = full_time;
	}
      }
    }
  }

  return 1;

}
