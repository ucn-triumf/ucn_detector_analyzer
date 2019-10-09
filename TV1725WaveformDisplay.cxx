#include "TV1725WaveformDisplay.h"
#include "TH1D.h"
#include "TF1.h"

#include "TCanvas.h"
#include "TTree.h"

#include "TDirectory.h"

#include "time.h"

#include "TV1725DppPsdData.hxx"



/// Reset the histograms for this canvas
TV1725QLQL::TV1725QLQL(){

  SetNumberChannelsInGroup(V1725_MAXCHAN);
  SetGroupName("Module");
  SetChannelName("Channel");
    
  CreateHistograms();
}


void TV1725QLQL::CreateHistograms(){
  
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
      sprintf(name,"V1725QLQL_%i_%i",iBoard,i);

      sprintf(title,"V1725 Gate Values for module = %i, channel=%i",iBoard,i);	

      TH2F *tmp = new TH2F(name,title,320,-20,40000,320,-20,30000);
      tmp->SetDrawOption("colz");
      tmp->SetXTitle("Q Long Calculated");
      tmp->SetYTitle("Q Long From Board");
      push_back(tmp);
    }
  }

}


/// Update the histograms for this canvas 
void TV1725QLQL::UpdateHistogram(int board, int chan, uint16_t QLCal[], uint16_t QLBoard[], int nEvents){


  std::cout << "Filling histograms" << std::endl;
  int index = board*V1725_MAXCHAN + chan;
  for (int i=0; i<nEvents; i++)
    {
      GetHistogram(index)->Fill(QLBoard[i],QLCal[i]);     
    }
}



/// Take actions at begin run
void TV1725QLQL::BeginRun(int transition,int run,int time){

  CreateHistograms();
}

/// Take actions at end run  
void TV1725QLQL::EndRun(int transition,int run,int time){

}


///////////////////////////////////////////////////////////
TV1725QSQS::TV1725QSQS(){

  SetNumberChannelsInGroup(V1725_MAXCHAN);
  SetGroupName("Module");
  SetChannelName("Channel");
    
  CreateHistograms();
}


void TV1725QSQS::CreateHistograms(){
  
  // check if we already have histograms.
  char tname[100];
  sprintf(tname,"V1725QSQS_%i_%i",0,0);
  
  TH2F *tmp = (TH2F*)gDirectory->Get(tname);
  if (tmp) return;

  // Otherwise make histograms
  clear();

  for(int iBoard=0; iBoard<NDPPBOARDS; iBoard++){// Loop over V1725 boards        
    for(int i = 0; i < V1725_MAXCHAN; i++){ // loop over 8 channels

      char name[100];
      char title[100];
      sprintf(name,"V1725QSQS_%i_%i",iBoard,i);

      sprintf(title,"V1725 Gate Values for module = %i, channel=%i",iBoard,i);	

      TH2F *tmp = new TH2F(name,title,320,-20,40000,320,-20,30000);
      tmp->SetDrawOption("colz");
      tmp->SetXTitle("Q Short Calculated");
      tmp->SetYTitle("Q Short From Board");
      push_back(tmp);
    }
  }

}


/// Update the histograms for this canvas 
void TV1725QSQS::UpdateHistogram(int board, int chan, uint16_t QSCal[], uint16_t QSBoard[], int nEvents){

  int index = board*V1725_MAXCHAN + chan;
  for (int i=0; i<nEvents; i++)
    {
      GetHistogram(index)->Fill(QSBoard[i],QSCal[i]);     
    }
}



/// Take actions at begin run
void TV1725QSQS::BeginRun(int transition,int run,int time){

  CreateHistograms();
}

/// Take actions at end run  
void TV1725QSQS::EndRun(int transition,int run,int time){

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


/// Update the histograms for this canvas 
void TV1725WaveformDisplay::UpdateHistogram(int board, int chan, uint16_t * wf, int tLength,  char * ctag){

  int index = board*V1725_MAXCHAN + chan;


  TH1* tmp = GetHistogram(index);
  if ( tmp->GetNbinsX() != tLength ) tmp->SetBins(tLength,0.,tLength*4.0);
  for (int b = 0; b<tLength; b++){
    GetHistogram(index)->SetBinContent(b+1,*(wf+b));
  }
  GetHistogram(index)->SetTitle(ctag);

}



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
		
		TH1D *tmp = new TH1D(name, title, 400, 0., 4000);
		tmp->SetXTitle("Pulse Height (ADC)");
		
		push_back(tmp);
	}
  }
}



/// Update the histograms for this canvas 
void TV1725_PH::UpdateHistogram(int board, int chan, uint32_t pulseHeight){

  int index = board*V1725_MAXCHAN + chan;
  GetHistogram(index)->Fill(pulseHeight);     
}




/// Take actions at begin run
void TV1725_PH::BeginRun(int transition,int run,int time){

  CreateHistograms();
}

/// Take actions at end run  
void TV1725_PH::EndRun(int transition,int run,int time){

}

