#include "TUCNDetectorCharge.h"

#include "TV792NData.hxx"
#include "TDirectory.h"

const int Nchannels = 16;
 #include <sys/time.h>

/// Reset the histograms for this canvas
TUCNDetectorCharge::TUCNDetectorCharge(bool isLi6, bool is3HEDET1){  
  fIsLi6 = isLi6;
  if(fIsLi6) det_name = std::string("Li6");
  else{
    if(is3HEDET1){
      det_name = std::string("He3");
    }else{
      det_name = std::string("He3Det2");
    }
  }

  CreateHistograms();
}


void TUCNDetectorCharge::CreateHistograms(){
  

  // Otherwise make histograms
  clear();
  
  for(int i = 0; i < Nchannels; i++){ // loop over channels    

    char name[100];
    char title[100];
    sprintf(name,"%s_Charge_%i_%i",det_name.c_str(),0,i);

    // Delete old histograms, if we already have them
    TH1D *old = (TH1D*)gDirectory->Get(name);
    if (old){
      delete old;
    }


    // Create new histograms
    TH1D *tmp;
    if(fIsLi6){
      sprintf(title,"Li-6 Charge Short Histogram for channel=%i",i);
      tmp = new TH1D(name,title,6600,0,66000);
    }else{
      sprintf(title,"He-3 Pulse Height Histogram for channel=%i",i);	
      tmp = new TH1D(name,title,4200,0,4200);
    }
      
    tmp->SetYTitle("Number of entries");
    if(fIsLi6)     tmp->SetXTitle("Charge (ADC counts)");
    else     tmp->SetXTitle("Pulse Height (ADC counts)");

    push_back(tmp);
  }

}


/// Fill the charge spectrum with the set of ucn hits
void  TUCNDetectorCharge::UpdateHistograms(TUCNHitCollection & hits){

  for(unsigned int i = 0; i < hits.size(); i++){
    int channel = hits[i].channel;
    int charge = hits[i].chargeShort;
    
    if(channel >= 0 && channel < size())
      GetHistogram(channel)->Fill(charge);
  }
}


/// Take actions at begin run
void TUCNDetectorCharge::BeginRun(int transition,int run,int time){

  CreateHistograms();

}

/// Take actions at end run  
void TUCNDetectorCharge::EndRun(int transition,int run,int time){

}
