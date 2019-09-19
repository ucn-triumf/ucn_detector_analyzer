#include "TUCNChronobox.hxx"
#include "TH1D.h"
#include "TROOT.h"

TUCNRisingEdgeDiff::TUCNRisingEdgeDiff(){  CreateHistograms(); }

void TUCNRisingEdgeDiff::CreateHistograms(){
  

  // Otherwise make histograms
  clear();

  
  for(int i = 0; i < 40; i++){ // loop over channels    

    char name[100];
    char title[100];
    sprintf(name,"Rising_Edge_Diff_%i",i);
    sprintf(title,"Rising Edge Diff_%i",i);

    // Delete old histograms, if we already have them
    TH1D *old = (TH1D*)gDirectory->Get(name);
    if (old){
      delete old;
    }


    // Create new histograms
    TH1D *tmp = new TH1D(name,title,5500,-10,100);

    push_back(tmp);


  }

}

/// Fill the charge spectrum with the set of ucn hits
void  TUCNRisingEdgeDiff::UpdateHistograms(TDataContainer& dataContainer){

}


/// Take actions at begin run
void TUCNRisingEdgeDiff::BeginRun(int transition,int run,int time){

  CreateHistograms();

}

/// Take actions at end run  
void TUCNRisingEdgeDiff::EndRun(int transition,int run,int time){

}


TUCNChronobox::TUCNChronobox(){

  fNumberRollovers= 0;
  fRolloverWord = 0;
  fPPSTimestamp = -1;
  for(int i = 0; i < MaxChan; i++){
    fTimestamps[i][0] = 0;
    fTimestamps[i][1] = 0;
  }

  fLeadingDiff = new TH1D("LeadingDiff","LeadingDiff",500000,-2,2);
  fFallingDiff = new TH1D("FallingDiff","FallingDiff",500000,-2,2);
  fChronoWidth= new TH1D("ChronoWidth","ChronoDiff",500000,-5,15);

  fRisingEdgeDiff = new TUCNRisingEdgeDiff();
}

#include <sys/time.h>
#include <bitset>

int maxmax = 0;
int TUCNChronobox::ProcessMidasEvent(TDataContainer& dataContainer){
  
  TGenericData *data = dataContainer.GetEventData<TGenericData>("CBS0");
  // std::cout << "Check crhon" << std::endl;
  if(data){  
    //std::cout << "Got chronobox bank. size :" << data->GetSize()  << std::endl;
    int nwords = data->GetSize();

    struct timeval tp;
    gettimeofday(&tp,NULL);

    //std::cout << "nwords: " << nwords << std::endl;
    for(int i = 0; i < nwords; i++){
      uint32_t word = data->GetData32()[i];
      int leading = (word & 0x1);
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
	
	double tdiff = full_time - fTimestamps[ch][leading];

	if(ch == 4){

	  if(tdiff < 0){
	    std::cout << "AAA!!! " ;
	  }else{
	    std::cout << "AAA    " ;
	    
	  }
	  
	  std::cout << std::hex << " Chan " << ch << " ("<<leading<< ") timestamp=0x"<< timestamp << " bit 0x200000 = " << ((word & 0x00200000) >> 21) << std::dec 
		    << " time diff=" << full_time - fTimestamps[ch][leading] 
		    << std::endl;

	  if(!leading){
	    fLeadingDiff->Fill(tdiff);
	    fChronoWidth->Fill(full_time - fTimestamps[ch][0]);
	  }else{
	    
	    fFallingDiff->Fill(tdiff);
	    fChronoWidth->Fill(full_time - fTimestamps[ch][1]);
	  }

	  if(0)
	  std::cout << "Time differences: " << full_time - fTimestamps[ch][leading] << " "
		    << full_time - fTimestamps[0][0] << " ch=" << ch << " leading="<<leading <<std::endl;
	  if(0)	std::cout << full_time - fTimestamps[ch][leading] << " " << i << " |||" << ch << "||| " << std::dec << timestamp << " "
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
