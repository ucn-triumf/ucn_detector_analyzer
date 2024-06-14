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

  
  for(int i = 0; i < TUCNChronobox::MaxChan; i++){ // loop over channels    

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

void TUCNChronobox::MakeSingleHistograms(){

  // If the histograms exist, exit... 
  TH1D *old = (TH1D*)gDirectory->Get("IV3DriveClosedDiff");
  if (old) return;

  // otherwise make the histograms
  fIV2DriveClosedDiff = new TH1D("IV2DriveClosedDiff","IV2: Closed State Time - Drive Time",4000,0,2);
  fIV2DriveClosedDiff->SetXTitle("Closed State Stop Time - Valve Drive Time (s)");

  fIV2DriveOpenedDiff = new TH1D("IV2DriveOpenDiff","IV2: Open State Time - Drive Time",4000,0,2);
  fIV2DriveOpenedDiff->SetXTitle("Open State Time - Valve Drive Time (s)");

  fIV2ClosedOpenedDiff = new TH1D("IV2ClosedOpenDiff","IV2: Open State Time - Closed State Time",4000,0,2);
  fIV2ClosedOpenedDiff->SetXTitle("Open State Stop Time - Closed Time (s)");

  fIV2NoDriveClosedDiff = new TH1D("IV2NoDriveClosedDiff","IV2: Closed State Time - No Drive Time",4000,0,2);
  fIV2NoDriveClosedDiff->SetXTitle("Closed State Stop Time - Valve No Drive Time (s)");

  fIV2NoDriveOpenedDiff = new TH1D("IV2NoDriveOpenDiff","IV2: Open State Time - No Drive Time",4000,0,2);
  fIV2NoDriveOpenedDiff->SetXTitle("Open State Time - Valve No Drive Time (s)");

  fIV2OpenedClosedDiff = new TH1D("IV2OpenClosedDiff","IV2: Closed State Time - Open State Time",4000,0,2);
  fIV2OpenedClosedDiff->SetXTitle("Closed State Stop Time - Open Time (s)");
  
  fIV3DriveClosedDiff = new TH1D("IV3DriveClosedDiff","IV3: Closed State Time - Drive Time",4000,0,2);
  fIV3DriveClosedDiff->SetXTitle("Closed State Stop Time - Valve Drive Time (s)");

  fIV3DriveOpenedDiff = new TH1D("IV3DriveOpenDiff","IV3: Open State Time - Drive Time",4000,0,2);
  fIV3DriveOpenedDiff->SetXTitle("Open State Time - Valve Drive Time (s)");

  fIV3ClosedOpenedDiff = new TH1D("IV3ClosedOpenDiff","IV3: Open State Time - Closed State Time",4000,0,2);
  fIV3ClosedOpenedDiff->SetXTitle("Open State Stop Time - Closed Time (s)");
  
  fIV3NoDriveClosedDiff = new TH1D("IV3NoDriveClosedDiff","IV3: Closed State Time - No Drive Time",4000,0,2);
  fIV3NoDriveClosedDiff->SetXTitle("Closed State Stop Time - Valve No Drive Time (s)");

  fIV3NoDriveOpenedDiff = new TH1D("IV3NoDriveOpenDiff","IV3: Open State Time - No Drive Time",4000,0,2);
  fIV3NoDriveOpenedDiff->SetXTitle("Open State Time - Valve No Drive Time (s)");

  fIV3OpenedClosedDiff = new TH1D("IV3OpenClosedDiff","IV3: Closed State Time - Open State Time",4000,0,2);
  fIV3OpenedClosedDiff->SetXTitle("Closed State Stop Time - Open Time (s)");

}

TUCNChronobox::TUCNChronobox(){

  std::cout << "Chronobox initialize" << std::endl;

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

  MakeSingleHistograms();

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

    if(0)    std::cout << "Chrono nwords: " << nwords << std::endl;
    for(int i = 0; i < nwords; i++){
      uint32_t word = data->GetData32()[i];
      if((word & 0xff000000) == 0xff000000){
	if(0)std::cout << "Chrono rollover: " << std::hex << word << std::dec << std::endl;
	fRolloverWord = word;
      }else if((word & 0xc0000000) == 0x80000000){	
	int ch = (word & 0x7f000000) >> 24;
	uint32_t timestamp = (word & 0xfffffe) >> 1;
	int falling = (word & 0x1);

	// Yikes, don't understand why this roll-over calculation necessary...
	int rollovers = (int)((fRolloverWord & 0xffff))/2;

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

	
	if(1 && ch != 16){
	  std::cout << "Chrono " << ch ;
	  if(falling) std::cout << " fall ";
	  else        std::cout << " rise ";
	  std::cout << "timestamp= " << full_time 
		    << " rollover=" << rollovers
	    //<< std::hex  << " ("<<falling<< ") timestamp=0x"<< timestamp << std::dec 
		    << " tdiff (same) =" << full_time - fTimestamps[ch][falling] 
		    << " tdiff (state)=" << full_time - fTimestamps[ch][!falling] 
		    << std::endl;
	}	

	// IV2
	if(ch == 9 && falling && tdiff > 2){ // Falling edge of IV2 close (ch 9) - IV2 drive start (ch 25)
	  fIV2DriveClosedDiff->Fill(full_time - fTimestamps[25][0]);
	}

	if(ch == 8 && !falling && tdiff > 2){ // Rising edge of IV2 open (ch 8) - IV2 drive start (ch 25)
	  fIV2DriveOpenedDiff->Fill(full_time - fTimestamps[25][0]);
	}

	if(ch == 8 && !falling && tdiff > 2){ // Rising edge of IV2 open (ch 8) -  falling edge of IV2 close (ch 9)
	  fIV2ClosedOpenedDiff->Fill(full_time - fTimestamps[9][1]);
	}

	if(ch == 9 && !falling && tdiff > 2){ // rising edge of IV2 close (ch 9) - IV2 no drive start (ch 25)
	  fIV2NoDriveClosedDiff->Fill(full_time - fTimestamps[25][1]);
	}

	if(ch == 8 && falling && tdiff > 2){ // Falling edge of IV2 open (ch 8) - IV2 no drive start (ch 25)
	  fIV2NoDriveOpenedDiff->Fill(full_time - fTimestamps[25][1]);
	}

	if(ch == 9 && !falling && tdiff > 2){ //  Rising edge of IV2 close (ch 9) - Falling edge of IV2 open (ch 8)
	  fIV2OpenedClosedDiff->Fill(full_time - fTimestamps[8][1]);	  
	}



	// IV3
	if(ch == 11 && falling && tdiff > 2){ // Falling edge of IV3 close (ch 11) - IV3 drive start (ch 26)
	  fIV3DriveClosedDiff->Fill(full_time - fTimestamps[26][0]);
	}

	if(ch == 10 && !falling && tdiff > 2){ // Rising edge of IV3 open (ch 10) - IV3 drive start (ch 26)
	  fIV3DriveOpenedDiff->Fill(full_time - fTimestamps[26][0]);
	}

	if(ch == 10 && !falling && tdiff > 2){ // Rising edge of IV3 open (ch 10) -  falling edge of IV3 close (ch 11)
	  fIV3ClosedOpenedDiff->Fill(full_time - fTimestamps[11][1]);
	}

	if(ch == 11 && !falling && tdiff > 2){ // rising edge of IV3 close (ch 11) - IV3 no drive start (ch 26)
	  fIV3NoDriveClosedDiff->Fill(full_time - fTimestamps[26][1]);
	}

	if(ch == 10 && falling && tdiff > 2){ // Falling edge of IV3 open (ch 10) - IV3 no drive start (ch 26)
	  fIV3NoDriveOpenedDiff->Fill(full_time - fTimestamps[26][1]);
	}

	if(ch == 11 && !falling && tdiff > 2){ //  Rising edge of IV3 close (ch 11) - Falling edge of IV3 open (ch 10)
	  fIV3OpenedClosedDiff->Fill(full_time - fTimestamps[10][1]);
	}



	fTimestamps[ch][falling] = full_time;

	//fIV3DriveClosedDiff
	
      }
    }
  }

  return 1;

}
