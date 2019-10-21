#include "TV1725WaveformDisplay.h"
#include "TH1D.h"
#include "TF1.h"

#include "TCanvas.h"
#include "TTree.h"

#include "TDirectory.h"

#include "time.h"

#include "TV1725DppPsdData.hxx"



/// Reset the histograms for this canvas
TV1725PSDQL::TV1725PSDQL(){

  SetNumberChannelsInGroup(V1725_MAXCHAN);
  SetGroupName("Module");
  SetChannelName("Channel");
    
  CreateHistograms();
}


void TV1725PSDQL::CreateHistograms(){
  
  // check if we already have histograms.
  char tname[100];
  sprintf(tname,"V1725QLQL_%i_%i",0,0);
  
  TH2F *tmp = (TH2F*)gDirectory->Get(tname);
  if (tmp) return;

  // Otherwise make histograms
  clear();

  for(int iBoard=0; iBoard<NDPPBOARDS; iBoard++){// Loop over V1725 boards        
    for(int i = 0; i < V1725_MAXCHAN; i++){ // loop over 8 channels

      char name[100];
      char title[100];
      sprintf(name,"V1725PSD_VS_QL_%i_%i",iBoard,i);

      sprintf(title,"V1725 PSD vs Qlong module = %i, channel=%i",iBoard,i);	

      TH2F *tmp = new TH2F(name,title,200,-20,25000,200,-0.2,1);
      tmp->SetDrawOption("colz");
      tmp->SetXTitle("Q Long");
      tmp->SetYTitle("PSD");
      push_back(tmp);
    }
  }

}

void TV1725PSDQL::UpdateHistograms(TDataContainer& dataContainer){

  TV1725DppPsdData *data = dataContainer.GetEventData<TV1725DppPsdData>("W500");
  if(!data) return;
    
  /// Get the Vector of ADC Measurements.
  std::vector<ChannelMeasurement> measurements = data->GetMeasurements();

  
  for(unsigned int i = 0; i < measurements.size(); i++){
    
    ChannelMeasurement meas = measurements[i];
    int ch = meas.GetChannel();
    double psd = 0;
    if(meas.GetQlong() != 0) psd = (double)(meas.GetQlong() - meas.GetQshort())/(double)meas.GetQlong();    
    GetHistogram(ch)->Fill(meas.GetQlong(),psd);

  }

}




/// Take actions at begin run
void TV1725PSDQL::BeginRun(int transition,int run,int time){

  CreateHistograms();
}

/// Take actions at end run  
void TV1725PSDQL::EndRun(int transition,int run,int time){

}




TV1725WaveformDisplay::TV1725WaveformDisplay(){

  SetNumberChannelsInGroup(V1725_MAXCHAN);
  SetGroupName("Module");
  SetChannelName("Channel");
    
  CreateHistograms();
}


void TV1725WaveformDisplay::CreateHistograms(){

  // check if we already have histogramss.
  char tname[100];
  sprintf(tname,"V1725_%i",0);

  TH1D *tmp = (TH1D*)gDirectory->Get(tname);
  if (tmp) return;

  //Otherwise make histograms
  clear();


  for(int iBoard=0; iBoard<NDPPBOARDS; iBoard++){
	for(int i = 0; i < V1725_MAXCHAN; i++){ // loop over 8 channels		
		char name[100];
		char title[100];
		sprintf(name,"V1725_%i_%i",i,iBoard);

		sprintf(title,"V1725 Waveform for channel=%i Board=%i",i,iBoard);	
		
		TH1D *tmp = new TH1D(name, title, 1, 0., 4.);
		tmp->SetXTitle("ns");
		tmp->SetYTitle("ADC value");
		
		push_back(tmp);
	}
  }
}


void TV1725WaveformDisplay::UpdateHistograms(TDataContainer& dataContainer){

  TV1725DppPsdData *data = dataContainer.GetEventData<TV1725DppPsdData>("W500");
  if(!data) return;
  
  
  /// Get the Vector of ADC Measurements.
  std::vector<ChannelMeasurement> measurements = data->GetMeasurements();

  
  for(unsigned int i = 0; i < measurements.size(); i++){
    
    ChannelMeasurement meas = measurements[i];
    double timestamp = meas.GetExtendedTimeTag();
    timestamp *= 0.000000004;
    if(meas.GetChannel() == 11 || meas.GetChannel() == 10){
      std::cout << "V1725: " << meas.GetChannel() << " " 
		<< meas.GetQlong() << " " 
		<< meas.GetQshort() << " " 
		<< meas.GetExtendedTimeTag() << " ";
      printf("%f ",timestamp);    
      std::cout << timestamp  << " "
		<< std::hex << meas.GetHeader0() << " "
		<< meas.GetHeader2() << " "
		<< std::dec << std::endl;
    }
    int ch = meas.GetChannel();
    int nsamples = meas.GetNSamples();
    
    TH1* tmp = GetHistogram(ch);
    if ( tmp->GetNbinsX() != nsamples ) tmp->SetBins(nsamples,0.,nsamples*4.0);
    for (int b = 0; b<nsamples; b++){
      GetHistogram(ch)->SetBinContent(b+1,meas.GetSample(b));
    }

  }

};




/// Take actions at begin run
void TV1725WaveformDisplay::BeginRun(int transition,int run,int time){

  CreateHistograms();
}

/// Take actions at end run  
void TV1725WaveformDisplay::EndRun(int transition,int run,int time){


}



TV1725_PH::TV1725_PH(){

  SetNumberChannelsInGroup(V1725_MAXCHAN);
  SetGroupName("Module");
  SetChannelName("Channel");
    
  CreateHistograms();
}


void TV1725_PH::CreateHistograms(){

  // check if we already have histogramss.
  char tname[100];
  sprintf(tname,"TV1725_PH_%i",0);

  TH1D *tmp = (TH1D*)gDirectory->Get(tname);
  if (tmp) return;

  //Otherwise make histograms
  clear();


  for(int iBoard=0; iBoard<NDPPBOARDS; iBoard++){
    for(int i = 0; i < V1725_MAXCHAN; i++){ // loop over 8 channels		
      char name[100];
      char title[100];
      sprintf(name,"TV1725_PH_%i_%i",i,iBoard);
      
      sprintf(title,"V1725 PH for channel=%i Board=%i",i,iBoard);	
      
      TH1D *tmp = new TH1D(name, title, 400, 0., 16000);
      tmp->SetXTitle("Pulse Height (ADC)");
      
      push_back(tmp);
    }
  }
}


void TV1725_PH::UpdateHistograms(TDataContainer& dataContainer){

  TV1725DppPsdData *data = dataContainer.GetEventData<TV1725DppPsdData>("W500");
  if(!data) return;

  /// Get the Vector of ADC Measurements.
  std::vector<ChannelMeasurement> measurements = data->GetMeasurements();
  
  for(unsigned int i = 0; i < measurements.size(); i++){
    
    ChannelMeasurement meas = measurements[i];
    
    int ch = meas.GetChannel();
    int nsamples = meas.GetNSamples();
    
    double min_value = 999999;
    for (int b = 0; b<nsamples; b++){
      if(meas.GetSample(b) < min_value) min_value = meas.GetSample(b);
    }
    
    double ph = 14718 - min_value;
    GetHistogram(ch)->Fill(ph);

  }

}



/// Take actions at begin run
void TV1725_PH::BeginRun(int transition,int run,int time){

  CreateHistograms();
}

/// Take actions at end run  
void TV1725_PH::EndRun(int transition,int run,int time){

}

