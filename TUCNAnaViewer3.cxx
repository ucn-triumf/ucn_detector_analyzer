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
    if(banklist.find("W20") == std::string::npos) return 1;

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
    int g=0;

    /// grab each subevent
    for (iboard = 0; iboard<NDPPBOARDS; iboard++){
        for (ichan = 0; ichan < PSD_MAXNCHAN; ichan++){
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
            float PSDCalculated[size];

            for (int y = 0; y<size; y++){
                qsCalculated[y] = 0;
                qlCalculated[y] = 0;
            }

            //added July 5, 2016 to sort the waveforms based on time tag
            for (int i =0; i<nEvents; i++){
                wf = fDPP[iboard].GetWaveform(i, ichan );
                b  = fDPP[iboard].GetPSD( i, ichan );

                // Fill the QS vs QL and PSD vs QL plots
                double fql = b->ChargeLong;
                double fqs = b->ChargeShort;
                UShort_t tChargeL  = qcali.get_q( iboard, ichan, fql, 0.0 );
                UShort_t tChargeS  = qcali.get_q( iboard, ichan, fqs, 0.0 );
                Float_t tPSD      = 0.0;

                if(tChargeL != 0){
                    tPSD = ((Float_t)(tChargeL)-(Float_t)(tChargeS))/((Float_t)(tChargeL));
                }

                // fill histograms
                fV1720QSQLHistograms->UpdateHistogram(iboard, ichan, tChargeS, tChargeL);
                fV1720PSDQLHistograms->UpdateHistogram(iboard, ichan, tPSD, (Float_t)tChargeL);
                if(iboard == 1 && 0){
                    std::cout << "Chan " << ichan << " " << tChargeL << " "
                            << tChargeS << " " << tPSD << std::endl;
                }

                uint16_t pulseValues[b->Length];
                TimeStampArray[i] = b->TimeTag;

                //sets the value of wf for each bin to an array
                for (int s=0; s < b->Length; s++){
                    pulseValues[s] = *(wf+s);
                }

                //adds the wf value of all the bins in the long gate (after gate offset)
                for (int a = 4; a < 54; a++){
                    qlCalculated[i] = qlCalculated[i] + (b->Baseline - pulseValues[a]);
                }

                for (int h = 4; h < 14; h++){
                    qsCalculated[i] = qsCalculated[i] + (b->Baseline - pulseValues[h]);
                }

                PSDCalculated[i] = ((float)qlCalculated[i] - (float)qsCalculated[i])/(float)qlCalculated[i];
                QLDifference[i] = ((float)qlCalculated[i])/((float)b->ChargeLong);
                QSDifference[i] = ((float)qsCalculated[i])/((float)b->ChargeShort);
                QLBoard[i] = b->ChargeLong;
                QSBoard[i] = b->ChargeShort;
            }

            std::sort(TimeStampArray, TimeStampArray + size);//sorts time tags from smallest to largest

            //added July 5, 2016 to sort the waveforms based on time tag
            for (int j =0; j<nEvents; j++){
                for (isubev = 0;isubev<nEvents;isubev++) {
                    b  = fDPP[iboard].GetPSD( isubev, ichan );
                    //added July 5, 2016 to sort the waveforms based on time tag
                    //checks if time tag in the sub event is the same as the time tag
                    //of the j element of the TimeStampArray (plots waves in order)
                    if (b->TimeTag == TimeStampArray[j]){

                        //changed isubev for q July 28, 2016
                        wf = fDPP[iboard].GetWaveform( isubev, ichan );

                        if (b==NULL) continue;
                        if (wf==NULL) continue;

                        if (b->Length){
                            if (b->Length !=0){
                                tChargeL  =  qcali.get_q( iboard, ichan, double(b->ChargeLong), 0.0 );
                                tChargeS  =qcali.get_q( iboard, ichan, double(b->ChargeShort), 0.0 );
                                float ql = float(tChargeL);
                                float qs = float(tChargeS);
                                tPSD = 0.0;
                                if ( ql!=0.0 ) tPSD = ( ql - qs ) / ql ;

                                // fill histograms
                                time_t tt=sample.GetTimeStamp();
                                int timet = sample.GetTimeStamp();
                                sprintf(htitle, "%s BL=% 5d QS=% 5d QL=% 5d PSD=%6.2f WF=%5d/%5d",
                                        ctime( &tt ), b->Baseline, tChargeS,
                                        tChargeL, tPSD,  /*q+1*/isubev+1, nEvents );

                                //passes wave info to UpdateHistograms if a cut is put on PSD values
                                if (CutChoice == 'y'){
                                    if(tPSD <= PSDMax && tPSD >= PSDMin){
                                        fV1720WaveformDisplay->UpdateHistogram(iboard, ichan, wf, b->Length, htitle);
                                    }
                                }
                                else{
                                    //std::cout<<"Passing to UpdateHistograms"<<std::endl;
                                    fV1720WaveformDisplay->UpdateHistogram(iboard, ichan, wf, b->Length, htitle);
                                }
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
