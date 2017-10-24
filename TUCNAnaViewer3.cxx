#include "TROOT.h"
#include "TStyle.h"
#include "TUCNAnaViewer3.h"
#include "TF1.h"
#include <cmath>
#include "time.h"
#include <algorithm>

#include "TCanvas.h"
#include "TTree.h"

#include "TV1720WaveformDisplay.h"

TUCNAnaViewer3::TUCNAnaViewer3(){
  //fUCNEvent = NULL;
  verbose = 0;

  // event histograms for runtime window
  fV1720WaveformDisplay = new TV1720WaveformDisplay();
  fV1720QLQL = new TV1720QLQL();
  fV1720QSQS = new TV1720QSQS();
 
  // QS vs QL histograms
  fV1720QSQLHistograms = new TV1720QSQLHistograms();
  fV1720QSQLHistograms->DisableAutoUpdate();

  // PSD vs QL histograms
  fV1720PSDQLHistograms = new TV1720PSDQLHistograms();
  fV1720PSDQLHistograms->DisableAutoUpdate();

}

TUCNAnaViewer3::~TUCNAnaViewer3(){
}


// Analyze Data and Save to TTree
int TUCNAnaViewer3::FindAndFitPulses(TDataContainer& dataContainer, char CutChoice, float PSDMax, float PSDMin){

  // Loop over all the data, fill the histograms and and find histograms.
  TMidasEvent sample = dataContainer.GetMidasEvent();


  std::string banklist(sample.GetBankList());
  //std::cout << banklist << std::endl;
  if(banklist.find("W20") == std::string::npos) return 1;

  //  std::cout<<"==================================================================================="<<std::endl;
  //std::cout<<"=== Find and FitPulses ============================================================"<<std::endl;




  // output bank block information
  if(verbose) {
    printf("event id: %d\n",sample.GetEventId());
    printf("trigger mask: %d\n",sample.GetTriggerMask());
    printf("serial number: %d\n",sample.GetSerialNumber());
    printf("time: %d\n",sample.GetTimeStamp());
    printf("data size: %d\n",sample.GetDataSize());
    
    // got event, now to iterate through the banks
    printf("  bank list:\n");
    printf(sample.GetBankList());
    printf("\n");
  } else {
    //printf("  bank list:\n");
    //printf(sample.GetBankList());
    //printf("\n");
  }


  TMidas_BANK32 * bank = NULL;
  char * pdata = sample.GetData();

  /// We have some new waveforms, make sure to clear
  /// the previous event info so it doesnt get
  /// used again in case this board is not in this event
  /// ClearWaves Added Aug.15,2015 ABJ
  for (iboard = 0; iboard<NDPPBOARDS; iboard++) {
    fDPP[iboard].ClearWaves();
  }

  // loop over the all data for one event and organize said data
  for(sample.IterateBank32(&bank,&pdata);bank!=NULL&&pdata!=NULL;
      sample.IterateBank32(&bank,&pdata)) {

    //if(bank->fName != "W201" || bank->fName != "W200") continue;
    
    sscanf(&bank->fName[3],"%1d", &iboard);
    if ( iboard<0 || iboard >= NDPPBOARDS){
      if(verbose)
	std::cout<<"<TUCNAnaViewer3> bank="<<iboard<<" but MAXBOARDS= "<<NDPPBOARDS<<std::endl;
      continue;
    } 
    if(verbose)
      std::cout<<"<TUCNAnaViewer3> board="<<iboard<<std::endl;

    fDPP[iboard].Init( pdata );

  } // end looping through banks
  //  std::cout<<"looped through banks"<<std::endl;
  int g=0;
  /// grab each subevent
  //std::cout<<"==================================================================================="<<std::endl;

  for (iboard = 0; iboard<NDPPBOARDS; iboard++) {
    //if (fDPP[iboard].GetNEvents()!=0)
      //  std::cout<<" ================ Board "<<iboard<<" Total Waveforms = "<<fDPP[iboard].GetNEvents()
    //	       <<" ================"<<std::endl;
    for (ichan = 0; ichan < PSD_MAXNCHAN;ichan++) {  
      g++;
      nEvents = fDPP[iboard].GetNWaves(ichan);
      uint32_t size =(uint32_t)nEvents;
      uint32_t TimeStampArray[size];//added July 5, 2016 to sort the waveforms based on time tag
      uint16_t qsCalculated[size];
      uint16_t qlCalculated[size];
      uint16_t QLBoard[size];
      uint16_t QSBoard[size];
      float QLDifference[size];
      float QSDifference[size];
      for (int y = 0; y<size; y++)
	{
	  qsCalculated[y] = 0;
	  qlCalculated[y] = 0;
	}
      float PSDCalculated[size];

      for (int i =0; i<nEvents; i++)//added July 5, 2016 to sort the waveforms based on time tag
	{
	  wf = fDPP[iboard].GetWaveform(i, ichan );
	  b  = fDPP[iboard].GetPSD( i, ichan );

	  // Fill the QS vs QL and PSD vs QL plots
	  Short_t tChargeL  = b->ChargeLong;
	  Short_t tChargeS  = b->ChargeShort;
	  Float_t tPSD      = 0.0;
	  if(tChargeL != 0)
	    tPSD = ((Float_t)(tChargeL)-(Float_t)(tChargeS))/((Float_t)(tChargeL));
	  
	  // fill histograms
	  fV1720QSQLHistograms->UpdateHistogram(iboard, ichan, tChargeS, tChargeL);
	  fV1720PSDQLHistograms->UpdateHistogram(iboard, ichan, tPSD, (Float_t)tChargeL);
	  if(ichan != 0)
	    std::cout << "Chan " << ichan << " " << tChargeL << " " 
		      << tChargeS << " " << tPSD << std::endl;



	  uint16_t pulseValues[b->Length];
	  TimeStampArray[i] = b->TimeTag;

	  
	  for (int s=0; s < b->Length; s++)//sets the value of wf for each bin to an array
	    {
	      pulseValues[s] = *(wf+s);
	      
	    }
	  for (int a = 4; a < 54; a++)//adds the wf value of all the bins in the long gate (after gate offset)
	    {
	      qlCalculated[i] = qlCalculated[i] + (b->Baseline - pulseValues[a]);	       
	    }
	  for (int h = 4; h < 14; h++)
	    {
	      qsCalculated[i] = qsCalculated[i] + (b->Baseline - pulseValues[h]);
	    } 
	  PSDCalculated[i] = ((float)qlCalculated[i] - (float)qsCalculated[i])/(float)qlCalculated[i];
	  QLDifference[i] = ((float)qlCalculated[i])/((float)b->ChargeLong);
	  QSDifference[i] = ((float)qsCalculated[i])/((float)b->ChargeShort);
	  QLBoard[i] = b->ChargeLong;
	  QSBoard[i] = b->ChargeShort;

	  
	}
      
 
      std::sort(TimeStampArray, TimeStampArray + size);//sorts time tags from smallest to largest
      
      //      if (nEvents!=0 )
      //std::cout<<"*** ch="<<ichan<<" waveform "<<g<<" of nEvents= "<<nEvents<<" *** "<<std::endl;
      for (int j =0; j<nEvents; j++)//added July 5, 2016 to sort the waveforms based on time tag
      	{
          //std::cout<<"====================================================J= "<<j<<"============================"<<std::endl;
	for (isubev = 0;isubev<nEvents;isubev++) {	;
	  b  = fDPP[iboard].GetPSD( isubev, ichan );
	     if (b->TimeTag == TimeStampArray[j])//added July 5, 2016 to sort the waveforms based on time tag
	                                        //checks if time tag in the sub event is the same as the time tag
	                                        //of the j element of the TimeStampArray (plots waves in order)
		 {
		
		wf = fDPP[iboard].GetWaveform( isubev, ichan );//changed isubev for q July 28, 2016
							     
		if ( b==NULL){
                  //		  std::cout<<"b==NULL"<<std::endl;
		  continue;
		}		
		if ( wf==NULL) 
		  {
                    //		    std::cout<<"wf == NULL"<<std::endl;
		    continue;
		  }
                
		//DPP_Bank_Out_Print( b );
		
		
		if (b->Length)
		  {
		    if (b->Length !=0)
		      {
                        //			std::cout<<"tChargeL"<<std::endl;
			tChargeL  = b->ChargeLong;
			//std::cout<<"tChargeS"<<std::endl;
			tChargeS  = b->ChargeShort;
			//std::cout<<"PSD"<<std::endl;
			float ql = float(tChargeL);
			float qs = float(tChargeS);
			
			tPSD = 0.0;
			if ( ql!=0.0 ) tPSD = ( ql - qs ) / ql ;
			
			// fill histograms
			time_t tt=sample.GetTimeStamp();
			int timet = sample.GetTimeStamp();
                        //			std::cout<<"timet "<<timet<<std::endl;
			//std::cout<<"BL="<<b->Baseline<<" QL="<<tChargeL<<" PSD="<<tPSD<<" Evno="<<sample.GetEventId()
			//	 <<" WF="<<isubev<<" / "<<nEvents<<std::endl;
			sprintf( htitle, "%s BL=% 5d QS=% 5d QL=% 5d PSD=%6.2f WF=%5d/%5d",
				 ctime( &tt ),
				 b->Baseline, tChargeS, tChargeL, tPSD,  /*q+1*/isubev+1, nEvents );  
			//std::cout<<" htitle ="<<htitle<<std::endl;

			if (CutChoice == 'y')//passes wave info to UpdateHistograms if a cut is put on PSD values
			  {
			    if(tPSD <= PSDMax && tPSD >= PSDMin)
			      {
				fV1720WaveformDisplay->UpdateHistogram(iboard, ichan, wf, b->Length, htitle);
			      }			      
			  }
			else 
			  {
                            //  			std::cout<<"Passing to UpdateHistograms"<<std::endl;
						fV1720WaveformDisplay->UpdateHistogram(iboard, ichan, wf, b->Length,htitle);
			  }
		        
			//fV1720QLQL->UpdateHistogram(iboard, ichan, qlCalculated, QLBoard, nEvents);
			//fV1720QSQS->UpdateHistogram(iboard, ichan, qsCalculated, QSBoard, nEvents);
			//std::cout<<"After Update Histograms"<<std::endl;
		        

		      }//if (b->length != 0)
		  }//if (b->length)
		 }//if (b->TimeTag == TimeStampArray[j])
	}//isubev loop
	}//j loop
    }//ichan loop
  }//iboard loop  // end filling tree
  
  return 1;  

} // end FindAndFitPulses


int TUCNAnaViewer3::ProcessMidasEvent(TDataContainer& dataContainer, char CutChoice, float PSDMax, float PSDMin){
  
  // Find and fit V1720 pulses; this also fills the V1720 waveforms.
  TMidasEvent sample = dataContainer.GetMidasEvent();
  std::string banklist(sample.GetBankList());
  //std::cout << banklist << std::endl;
  int counter = 1;
  if(banklist.find("W20") != std::string::npos){
    counter = FindAndFitPulses(dataContainer, CutChoice, PSDMax, PSDMin);
  }
  
  return counter;
  
}
