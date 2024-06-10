#include "PulseShapeStruct.h"

void DPP_Bank_Out_Print(DPP_Bank_Out_t *b){
    std::cout<<"DPP_Bank_Out_Print(): "<<std::endl;
    std::cout<< "  TimeTag    ="<< b->TimeTag
             << " Channel    =" << b->Channel
             << " ChargeShort=" << b->ChargeShort
             << " ChargeLong =" << b->ChargeLong
             << " Baseline   =" << b->Baseline
             << " Pur	  ="    << b->Pur
             << " Length     =" << b->Length<<std::endl;
    return;
}

// initialize structures
void DPPBankHandler::Init( char* pdata){
    fData = pdata;
    // if at the beginning of the data, initialize memory
    if ( fFirst == true ){
        // set pointer to first event
        uint32_t *value32;
        value32   = (uint32_t*)pdata;
        int NumEvents = *value32;
        pdata += sizeof(uint32_t);
        DPP_Bank_Out_t * b = (DPP_Bank_Out_t*)pdata;
        int maxsub[PSD_MAXNCHAN];
        bzero( (void*)maxsub, PSD_MAXNCHAN*sizeof(int) );

        // initialize structures
        // need to look at whole event to find maximum subevents per channel
        for (int iev = 0; iev<NumEvents; iev++){
            if ( b->Channel >= 0 && b->Channel < PSD_MAXNCHAN)
                maxsub[ b->Channel ] ++;
            pdata += sizeof( DPP_Bank_Out_t );     // increment pointer for size of dpp structure
            pdata += b->Length*sizeof( uint16_t ); // increment pointer for size of waveform
            b = (DPP_Bank_Out_t*)pdata; //seg fault seems to happen after this line but before 3....
        }

        fMaxSubEv=0; // maximum number of waveforms in each channel
        for (int ich=0; ich<PSD_MAXNCHAN; ich++){
            if (maxsub[ich] > fMaxSubEv) fMaxSubEv = maxsub[ich];
        }

        // now  init memory
        if (fMaxSubEv<50000) fMaxSubEv=50000;/////changed from 50000 A.Sikora, Aug 2 2017
        for (int ich=0; ich<PSD_MAXNCHAN; ich++){
            fDPPInfo[ich] = new DPP_Bank_Out_t*[ fMaxSubEv ];
            fWaveforms[ich] = new uint16_t*[ fMaxSubEv ];
        }

        pdata = fData;
        if(verbose)
            std::cout<<"<PulseShapeStruct::Init> FirstEvent, MaxSubEv="<<fMaxSubEv<<std::endl;
        fFirst = false;
    }

    // clear nwaves:
    bzero((void*)fNWaves, PSD_MAXNCHAN*sizeof(int) );
    if ( fMaxSubEv <= 0 ) return;

    uint32_t *value32;
    value32   = (uint32_t*)pdata;
    fNTotal = (long) *value32;
    pdata += sizeof(uint32_t);
    DPP_Bank_Out_t * b;

    // organize data into bank structure for readout
    if (verbose)  std::cout<<"fNTotal here ="<<fNTotal<<std::endl;
    for (int iev = 0; iev<fNTotal; iev++){
        b = (DPP_Bank_Out_t*)pdata;
        pdata+= sizeof(DPP_Bank_Out_t);
        uint16_t * wf = (uint16_t*)pdata;
        pdata+=sizeof(uint16_t)*b->Length;
        if(verbose) {
            std::cout<<"<PulseShapeStruct::Init>"<<std::endl;
            std::cout<<" ev="<<iev
                    <<" tt="<<b->TimeTag
                    <<" ch="<<b->Channel
                    <<" qs="<<b->ChargeShort
                    <<" ql="<<b->ChargeLong
                    <<" bl="<<b->Baseline
                    <<" wavecount="<<fNWaves[b->Channel]<<" / "<<fMaxSubEv
                    <<std::endl;
        }

        int ch = b->Channel;
        if (ch >= 0 && ch < PSD_MAXNCHAN) {
            if ( fNWaves[ch] < fMaxSubEv ){
                fDPPInfo[ch][ fNWaves[ch] ] = b;
                fWaveforms[ch][ fNWaves[ch] ] = wf;
                fNWaves[ch]++;
            } else {
                std::cout<<"<PulseShapeStruct::Init> WARNING ch="<<ch
                        <<" waveformcount="<<fNWaves[ch]
                        <<" only space for "<<fMaxSubEv<<" per channel"<<std::endl;
            }
        }
    } // end organizing for loop

    if(verbose){
        for (int ich=0; ich<PSD_MAXNCHAN; ich++){
            std::cout<<" ch["<<ich<<"] has "<<fNWaves[ich]<<" waveforms"<<std::endl;
        }
    }

  return;
}
