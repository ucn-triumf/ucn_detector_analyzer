#include "TUCNChronobox.hxx"
#include "TH1D.h"
#include "TROOT.h"

TUCNChronoDiffArray::TUCNChronoDiffArray(std::string name){
  fName = name;
  CreateHistograms(); 
}

void TUCNChronoDiffArray::CreateHistograms(){
  

  // Otherwise make histograms
  clear();

  
  for(int i = 0; i < 40; i++){ // loop over channels    

    char name[100];
    char title[100];
    sprintf(name,"%s_%i",fName.c_str(),i);
    sprintf(title,"%s_%i",fName.c_str(),i);

    // Delete old histograms, if we already have them
    TH1D *old = (TH1D*)gDirectory->Get(name);
    if (old){
      delete old;
    }


    // Create new histograms
    TH1D *tmp = new TH1D(name,title,11000,-10,100);

    push_back(tmp);


  }

}

/// Fill the charge spectrum with the set of ucn hits
void  TUCNChronoDiffArray::UpdateHistograms(TDataContainer& dataContainer){

}


/// Take actions at begin run
void TUCNChronoDiffArray::BeginRun(int transition,int run,int time){

  CreateHistograms();

}

/// Take actions at end run  
void TUCNChronoDiffArray::EndRun(int transition,int run,int time){

}


TUCNChronobox::TUCNChronobox(){

  fNumberRollovers= 0;
  fRolloverWord = 0;
  fPPSTimestamp = -1;
  for(int i = 0; i < MaxChan; i++){
    fTimestamps[i][0] = -9999;
    fTimestamps[i][1] = -9999;
  }

  fRisingEdgeDiff = new TUCNChronoDiffArray("Rising Edge Diff");
  fFallingEdgeDiff = new TUCNChronoDiffArray("Falling Edge Diff");
  fPulseWidths = new TUCNChronoDiffArray("Pulse Widths");

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
      int falling = (word & 0x1);
      if((word & 0xff000000) == 0xff000000){
	fRolloverWord = word;
      }else if((word & 0xf0000000) == 0x80000000){	
	int ch = (word & 0x7f000000) >> 24;
	uint32_t timestamp = (word & 0xfffffe) >> 1;
	int falling = (word & 0x1);

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
	
	double tdiff = full_time - fTimestamps[ch][falling];

	
	if(!falling){
	  if(fTimestamps[ch][falling] != -9999)
	    fRisingEdgeDiff->GetHistogram(ch)->Fill(tdiff);
	  if(fTimestamps[ch][1] != -9999)
	    fPulseWidths->GetHistogram(ch)->Fill(full_time - fTimestamps[ch][1]);
	}else{
	  if(fTimestamps[ch][falling] != -9999)
	    fFallingEdgeDiff->GetHistogram(ch)->Fill(tdiff);;	  
	}


	if(0)
	  std::cout << std::hex << " Chan " << ch << " ("<<falling<< ") timestamp=0x"<< timestamp << " bit 0x200000 = " << ((word & 0x00200000) >> 21) << std::dec 
		    << " time diff=" << full_time - fTimestamps[ch][falling] 
		    << std::endl;
	
	fTimestamps[ch][falling] = full_time;
	
      }
    }
  }

  return 1;

}
