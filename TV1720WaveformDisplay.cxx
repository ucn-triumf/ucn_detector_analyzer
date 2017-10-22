#include "TV1720WaveformDisplay.h"
#include "TH1D.h"
#include "TF1.h"

#include "TCanvas.h"
#include "TTree.h"

#include "TDirectory.h"

#include "time.h"




/// Reset the histograms for this canvas
TV1720QLQL::TV1720QLQL(){

  SetNumberChannelsInGroup(PSD_MAXNCHAN);
  SetGroupName("Module");
  SetChannelName("Channel");
    
  CreateHistograms();
}


void TV1720QLQL::CreateHistograms(){
  
  // check if we already have histograms.
  char tname[100];
  sprintf(tname,"V1720QLQL_%i_%i",0,0);
  
  TH2F *tmp = (TH2F*)gDirectory->Get(tname);
  if (tmp) return;

  // Otherwise make histograms
  clear();

  for(int iBoard=0; iBoard<NDPPBOARDS; iBoard++){// Loop over V1720 boards        
    for(int i = 0; i < PSD_MAXNCHAN; i++){ // loop over 8 channels

      char name[100];
      char title[100];
      sprintf(name,"V1720QLQL_%i_%i",iBoard,i);

      sprintf(title,"V1720 Gate Values for module = %i, channel=%i",iBoard,i);	

      TH2F *tmp = new TH2F(name,title,320,-20,40000,320,-20,30000);
      tmp->SetDrawOption("colz");
      tmp->SetXTitle("Q Long Calculated");
      tmp->SetYTitle("Q Long From Board");
      push_back(tmp);
    }
  }

}


/// Update the histograms for this canvas 
void TV1720QLQL::UpdateHistogram(int board, int chan, uint16_t QLCal[], uint16_t QLBoard[], int nEvents){


  std::cout << "Filling histograms" << std::endl;
  int index = board*PSD_MAXNCHAN + chan;
  for (int i=0; i<nEvents; i++)
    {
      GetHistogram(index)->Fill(QLBoard[i],QLCal[i]);     
    }
}



/// Take actions at begin run
void TV1720QLQL::BeginRun(int transition,int run,int time){

  CreateHistograms();
}

/// Take actions at end run  
void TV1720QLQL::EndRun(int transition,int run,int time){

}


///////////////////////////////////////////////////////////
TV1720QSQS::TV1720QSQS(){

  SetNumberChannelsInGroup(PSD_MAXNCHAN);
  SetGroupName("Module");
  SetChannelName("Channel");
    
  CreateHistograms();
}


void TV1720QSQS::CreateHistograms(){
  
  // check if we already have histograms.
  char tname[100];
  sprintf(tname,"V1720QSQS_%i_%i",0,0);
  
  TH2F *tmp = (TH2F*)gDirectory->Get(tname);
  if (tmp) return;

  // Otherwise make histograms
  clear();

  for(int iBoard=0; iBoard<NDPPBOARDS; iBoard++){// Loop over V1720 boards        
    for(int i = 0; i < PSD_MAXNCHAN; i++){ // loop over 8 channels

      char name[100];
      char title[100];
      sprintf(name,"V1720QSQS_%i_%i",iBoard,i);

      sprintf(title,"V1720 Gate Values for module = %i, channel=%i",iBoard,i);	

      TH2F *tmp = new TH2F(name,title,320,-20,40000,320,-20,30000);
      tmp->SetDrawOption("colz");
      tmp->SetXTitle("Q Short Calculated");
      tmp->SetYTitle("Q Short From Board");
      push_back(tmp);
    }
  }

}


/// Update the histograms for this canvas 
void TV1720QSQS::UpdateHistogram(int board, int chan, uint16_t QSCal[], uint16_t QSBoard[], int nEvents){

  int index = board*PSD_MAXNCHAN + chan;
  for (int i=0; i<nEvents; i++)
    {
      GetHistogram(index)->Fill(QSBoard[i],QSCal[i]);     
    }
}



/// Take actions at begin run
void TV1720QSQS::BeginRun(int transition,int run,int time){

  CreateHistograms();
}

/// Take actions at end run  
void TV1720QSQS::EndRun(int transition,int run,int time){

}





TV1720WaveformDisplay::TV1720WaveformDisplay(){

  SetNumberChannelsInGroup(PSD_MAXNCHAN);
  SetGroupName("Module");
  SetChannelName("Channel");
    
  CreateHistograms();
}


void TV1720WaveformDisplay::CreateHistograms(){

  // check if we already have histogramss.
  char tname[100];
  sprintf(tname,"V1720_%i",0);

  TH1D *tmp = (TH1D*)gDirectory->Get(tname);
  if (tmp) return;

  //Otherwise make histograms
  clear();


  for(int iBoard=0; iBoard<NDPPBOARDS; iBoard++){
	for(int i = 0; i < PSD_MAXNCHAN; i++){ // loop over 8 channels		
		char name[100];
		char title[100];
		sprintf(name,"V1720_%i_%i",i,iBoard);

		sprintf(title,"V1720 Waveform for channel=%i Board=%i",i,iBoard);	
		
		TH1D *tmp = new TH1D(name, title, 1, 0., 4.);
		tmp->SetXTitle("ns");
		tmp->SetYTitle("ADC value");
		
		push_back(tmp);
	}
  }
}


/// Update the histograms for this canvas 
void TV1720WaveformDisplay::UpdateHistogram(int board, int chan, uint16_t * wf, int tLength,  char * ctag){

  int index = board*PSD_MAXNCHAN + chan;


  TH1* tmp = GetHistogram(index);
  if ( tmp->GetNbinsX() != tLength ) tmp->SetBins(tLength,0.,tLength*4.0);
  for (int b = 0; b<tLength; b++){
    GetHistogram(index)->SetBinContent(b+1,*(wf+b));
  }
  GetHistogram(index)->SetTitle(ctag);

}



/// Take actions at begin run
void TV1720WaveformDisplay::BeginRun(int transition,int run,int time){

  CreateHistograms();
}

/// Take actions at end run  
void TV1720WaveformDisplay::EndRun(int transition,int run,int time){

}
