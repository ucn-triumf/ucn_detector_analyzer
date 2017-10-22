#ifndef PULSESHAPESTRUCT_H
#define PULSESHAPESTRUCT_H  

/// Author:  Blair Jamieson (Nov. 2013)
/// Class to parse a v1720 midas bank with PSD info and a waveform

#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <string>
#include "TVConstants.h"

// bank structure of individual events from MIDAS
typedef struct {
  //uint32_t TimeTag; // Update to 64 bit timestamp -- June 2017
  uint64_t TimeTag;
  int16_t  Channel;
  int16_t ChargeShort;
  int16_t ChargeLong;
  int16_t Baseline;
  int16_t Pur;
  int16_t Length;
} DPP_Bank_Out_t;

void DPP_Bank_Out_Print(DPP_Bank_Out_t *b);

typedef struct{
  float    fTemp1;
  uint16_t fPres;
  float    fTemp2;
  uint32_t fTime;
} slowcntl_Bank_Out_t;

/// DPPBankHanlder takes a bank of data from
/// one v1720 Module and parses it.
class DPPBankHandler{
 public: 
  
  DPPBankHandler(){ 
    fFirst=true; 
    verbose=0;
  };
  ~DPPBankHandler(){
    if ( fMaxSubEv > 0 ){
      for ( int ich=0; ich<PSD_MAXNCHAN; ich++){
	if (fDPPInfo[ich]!=NULL) delete [] fDPPInfo[ich];
	if (fWaveforms[ich]!=NULL) delete [] fWaveforms[ich];
      }
    }
  };

  /// Init takes bank pointer and parses data
  /// into a useful format
  void Init(char * pdata);

  /// Get the maximum number of waveforms per channel
  long GetNEvents(){ return fNTotal; };

  /// Make a method to clear the number of waveforms
  /// for resetting on start of new event
  void ClearWaves(){ 
    bzero((void*)fNWaves, PSD_MAXNCHAN*sizeof(int) );
  }

  /// Get the number of waveforms on a given channel
  int GetNWaves(int ichan){ return fNWaves[ichan]; };
  
  /// Get PSD Information for a given
  /// sub-event, and channel
  DPP_Bank_Out_t * GetPSD(int isubev, int ichan){
    if ( fMaxSubEv <= 0 ) return NULL;
    if ( ichan <0 || ichan>= PSD_MAXNCHAN ){
      if(verbose)
	std::cout<<"<PulseShapeStruct::GetPSD> Warning ichan="
		 <<ichan<<std::endl;
      return NULL;
    }
    if ( isubev <0 || isubev > fNWaves[ichan] ) {
      if(verbose)
	std::cout<<"<PulseShapeStruct::GetPSD> Warning isubev="
		 << isubev << " only "<<fNWaves[ichan]<<std::endl;
      return NULL;
    }
    if (verbose){
      std::cout<<"<PulseShapeStruct::GetPSD(isubev="<<isubev<<", ich="<<ichan<<") of "
	       <<fNWaves[ichan]<<" t="<<fDPPInfo[ichan][isubev]->TimeTag <<std::endl;
    }
    return fDPPInfo[ichan][isubev];
  };
  
  /// Get Waveform Information for a given
  /// sub-event, and channel
  uint16_t * GetWaveform(int isubev, int ichan){ 
    if ( fMaxSubEv <= 0 ) return NULL;
    if ( ichan <0 || ichan>= PSD_MAXNCHAN ) {
      if(verbose)
	std::cout<<"<PulseShapeStruct::GetWaveform> Warning ichan="
		 << ichan << std::endl;
      return NULL;
    }
    if ( isubev <0 || isubev > fNWaves[ichan] ) {
      if(verbose)
	std::cout<<"<PulseShapeStruct::GetWaveform> Warning ichan="
		 << isubev << std::endl;
      return NULL;
    }
    return fWaveforms[ichan][isubev];
  };
  

 private:
  bool             fFirst;                   /// check if first event loaded
  char *           fData;                    /// pointer to memory of start of bank
  int              fMaxSubEv;                /// maximum number of sub events per channel
  long             fNTotal;                  /// total number of waveforms
  int              fNWaves[PSD_MAXNCHAN];    /// number of waveforms by channel
  DPP_Bank_Out_t ** fDPPInfo[PSD_MAXNCHAN];  /// [NumChan][NumSubEv]
  uint16_t **      fWaveforms[PSD_MAXNCHAN]; /// [NumChan][NumSubEv]
  int              verbose;
  
};



#endif
