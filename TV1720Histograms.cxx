#include "TV1720Histograms.h"

#include "TDirectory.h"



/// Reset the histograms for this canvas
TV1720QSQLHistograms::TV1720QSQLHistograms(){

  SetNumberChannelsInGroup(PSD_MAXNCHAN);
  SetGroupName("Module");
  SetChannelName("Channel");
    
  CreateHistograms();
}


void TV1720QSQLHistograms::CreateHistograms(){
  
  // check if we already have histograms.
  char tname[100];
  sprintf(tname,"V1720QSQL_%i_%i",0,0);
  
  TH2F *tmp = (TH2F*)gDirectory->Get(tname);
  if (tmp) return;

  // Otherwise make histograms
  clear();

  for(int iBoard=0; iBoard<NDPPBOARDS; iBoard++){// Loop over V1720 boards        
    for(int i = 0; i < PSD_MAXNCHAN; i++){ // loop over 8 channels

      char name[100];
      char title[100];
      sprintf(name,"V1720QSQL_%i_%i",iBoard,i);

      sprintf(title,"V1720 Gate Values for module = %i, channel=%i",iBoard,i);	

      TH2F *tmp = new TH2F(name,title,320,-20,40000,320,-20,30000);
      tmp->SetDrawOption("colz");
      tmp->SetXTitle("Q Long");
      tmp->SetYTitle("Q Short");
      push_back(tmp);
    }
  }

}


/// Update the histograms for this canvas 
void TV1720QSQLHistograms::UpdateHistogram(int board, int chan, Float_t QS, Float_t QL){

  int index = board*PSD_MAXNCHAN + chan;
  GetHistogram(index)->Fill(QL,QS);     

}



/// Take actions at begin run
void TV1720QSQLHistograms::BeginRun(int transition,int run,int time){

  CreateHistograms();
}

/// Take actions at end run  
void TV1720QSQLHistograms::EndRun(int transition,int run,int time){

}




/// Reset the histograms for this canvas
TV1720PSDQLHistograms::TV1720PSDQLHistograms(){

  SetNumberChannelsInGroup(PSD_MAXNCHAN);
  SetGroupName("Module");
  SetChannelName("Channel");
    
  CreateHistograms();
}


void TV1720PSDQLHistograms::CreateHistograms(){
  
  // check if we already have histograms.
  char tname[100];
  sprintf(tname,"V1720PSDQL_%i_%i",0,0);
  
  TH2F *tmp = (TH2F*)gDirectory->Get(tname);
  if (tmp) return;

  // Otherwise make histograms
  clear();

  for(int iBoard=0; iBoard<NDPPBOARDS; iBoard++){// Loop over V1720 boards        
    for(int i = 0; i < PSD_MAXNCHAN; i++){ // loop over 8 channels

      char name[100];
      char title[100];
      sprintf(name,"V1720PSDQL_%i_%i",iBoard,i);

      sprintf(title,"V1720 Energy Comparison for module = %i, channel=%i",iBoard,i);	

      TH2F *tmp = new TH2F(name,title,200,-20,14000,200,-1,1);
      tmp->SetDrawOption("colz");
      tmp->SetXTitle("Q Long");
      tmp->SetYTitle("PSD");
      push_back(tmp);
    }
  }

}


/// Update the histograms for this canvas 
void TV1720PSDQLHistograms::UpdateHistogram(int board, int chan, Float_t PSD, Float_t QL){

  int index = board*PSD_MAXNCHAN + chan;
  GetHistogram(index)->Fill(QL,PSD);     

}



/// Take actions at begin run
void TV1720PSDQLHistograms::BeginRun(int transition,int run,int time){

  CreateHistograms();
}

/// Take actions at end run  
void TV1720PSDQLHistograms::EndRun(int transition,int run,int time){

}
