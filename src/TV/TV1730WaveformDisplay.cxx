#include "TV1730WaveformDisplay.h"
#include "TH1D.h"
#include "TF1.h"

#include "TCanvas.h"
#include "TTree.h"

#include "TDirectory.h"

#include "time.h"

#include "TV1730DppPsdData.hxx"

/// TV1730PSDQL ----------------------------------------------
TV1730PSDQL::TV1730PSDQL(){
    SetNumberChannelsInGroup(V1730_MAXCHAN);
    SetGroupName("Module");
    SetChannelName("Channel");
    CreateHistograms();
}

void TV1730PSDQL::CreateHistograms(){

    // check if we already have histograms.
    char tname[100];
    sprintf(tname, "V1730QLQL_%i_%i", 0, 0);

    TH2F *tmp = (TH2F*)gDirectory->Get(tname);
    if (tmp) return;

    // Otherwise make histograms
    clear();

    for(int iBoard=0; iBoard<NDPPBOARDS; iBoard++){// Loop over V1730 boards
        for(int i = 0; i < V1730_MAXCHAN; i++){ // loop over 8 channels

        char name[100];
        char title[100];
        sprintf(name,"V1730PSD_VS_QL_%i_%i",iBoard,i);

        sprintf(title,"V1730 PSD vs Qlong module = %i, channel=%i", iBoard, i);

        TH2F *tmp = new TH2F(name, title, 400, -20, 10000, 200, -0.2, 1);
        tmp->SetDrawOption("colz");
        tmp->SetXTitle("Q Long");
        tmp->SetYTitle("Pulse Shape Discrimination (Qlong-Qshort)/Qlong");
        push_back(tmp);
        }
    }
}

void TV1730PSDQL::UpdateHistograms(TDataContainer& dataContainer){

    TV1730DppPsdData *data = dataContainer.GetEventData<TV1730DppPsdData>("W500");
    if(!data) return;

    /// Get the Vector of ADC Measurements.
    std::vector<ChannelMeasurement> measurements = data->GetMeasurements();

    for(unsigned int i = 0; i < measurements.size(); i++){
        ChannelMeasurement meas = measurements[i];
        int ch = meas.GetChannel();
        double psd = 0;
        if(meas.GetQlong() != 0)
            psd = (double)(meas.GetQlong() - meas.GetQshort())/(double)meas.GetQlong();
        GetHistogram(ch)->Fill(meas.GetQlong(), psd);
    }
}

void TV1730PSDQL::BeginRun(int transition,int run,int time){
    CreateHistograms();
}

void TV1730PSDQL::EndRun(int transition,int run,int time){}

/// TV1730WaveformDisplay -----------------------------------
TV1730WaveformDisplay::TV1730WaveformDisplay(){
    SetNumberChannelsInGroup(V1730_MAXCHAN);
    SetGroupName("Module");
    SetChannelName("Channel");
    CreateHistograms();
}

void TV1730WaveformDisplay::CreateHistograms(){

    // check if we already have histograms
    char tname[100];
    sprintf(tname,"V1730_0", 0);

    TH1D *tmp = (TH1D*)gDirectory->Get(tname);
    if (tmp) return;

    //Otherwise make histograms
    clear();

    for(int iBoard=0; iBoard<1; iBoard++){
        for(int ch = 0; ch < V1730_MAXCHAN; ch++){ // loop over 8 channels
            char name[100];
            char title[100];
            sprintf(name,"V1730_%i", ch);

            sprintf(title,"V1730 Waveform for channel=%i Board=%i", ch, iBoard);

            TH1D *tmp = new TH1D(name, title, 1, 0., 4.);
            tmp->SetXTitle("ns");
            tmp->SetYTitle("ADC value");

            push_back(tmp);
        }
    }
}

void TV1730WaveformDisplay::UpdateHistograms(TDataContainer& dataContainer){

  //  std::cout << "wavefomr check" << std::endl;
    TV1730DppPsdData *data = dataContainer.GetEventData<TV1730DppPsdData>("D730");
    if(!data) return;

    /// Get the Vector of ADC Measurements.
    std::vector<ChannelMeasurement> measurements = data->GetMeasurements();
    
    std::cout << "Data for TV1730: " << measurements.size() << std::endl;

    for(unsigned int i = 0; i < measurements.size(); i++){

        ChannelMeasurement meas = measurements[i];
        double timestamp = meas.GetExtendedTimeTag();
        timestamp *= 0.000000004;

	//	std::cout << meas.GetChannel() << std::endl;
        if(meas.GetChannel() == 11 || meas.GetChannel() == 10){
            std::cout << "V1730: " << meas.GetChannel() << " "
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
	std::cout << "Nsamples " << nsamples << std::endl;
        TH1* tmp = GetHistogram(ch);
        if ( tmp->GetNbinsX() != nsamples ) tmp->SetBins(nsamples,0.,nsamples*4.0);
        for (int b = 0; b<nsamples; b++){
            GetHistogram(ch)->SetBinContent(b+1,meas.GetSample(b));
        }
    }
};

void TV1730WaveformDisplay::BeginRun(int transition,int run,int time){
    CreateHistograms();
}

void TV1730WaveformDisplay::EndRun(int transition,int run,int time){}

/// TV1730_PH ----------------------------------------------
TV1730_PH::TV1730_PH(){
    SetNumberChannelsInGroup(V1730_MAXCHAN);
    SetGroupName("Module");
    SetChannelName("Channel");
    CreateHistograms();
}

void TV1730_PH::CreateHistograms(){

    // check if we already have histograms
    char tname[100];
    sprintf(tname,"TV1730_PH_%i",0);

    TH1D *tmp = (TH1D*)gDirectory->Get(tname);
    if (tmp) return;

    //Otherwise make histograms
    clear();

    for(int iBoard=0; iBoard<NDPPBOARDS; iBoard++){
        for(int ch = 0; ch < V1730_MAXCHAN; ch++){ // loop over 8 channels
            char name[100];
            char title[100];
            sprintf(name,"TV1730_PH_%i_%i", iBoard,ch);

            sprintf(title,"V1730 PH for channel=%i Board=%i", ch, iBoard);

            TH1D *tmp = new TH1D(name, title, 400, 0., 20000);
            tmp->SetXTitle("Pulse Height (ADC)");

            push_back(tmp);
        }
    }
}

void TV1730_PH::UpdateHistograms(TDataContainer& dataContainer){

    TV1730DppPsdData *data = dataContainer.GetEventData<TV1730DppPsdData>("W500");
    if(!data) return;

    /// Get the Vector of ADC Measurements.
    std::vector<ChannelMeasurement> measurements = data->GetMeasurements();

    for(unsigned int i = 0; i < measurements.size(); i++){

        ChannelMeasurement meas = measurements[i];

        int ch = meas.GetChannel();
        int nsamples = meas.GetNSamples();
        double min_value = 999999;

        for (int b = 0; b<nsamples; b++){
            if(meas.GetSample(b) < min_value)
                min_value = meas.GetSample(b);
        }

        double ph = 14718 - min_value;
        GetHistogram(ch)->Fill(ph);
    }
}

void TV1730_PH::BeginRun(int transition,int run,int time){
    CreateHistograms();
}

void TV1730_PH::EndRun(int transition,int run,int time){}

/// TV1730_QL ----------------------------------------------
TV1730_QL::TV1730_QL(){
    SetNumberChannelsInGroup(V1730_MAXCHAN);
    SetGroupName("Module");
    SetChannelName("Channel");
    CreateHistograms();
}

void TV1730_QL::CreateHistograms(){

    // check if we already have histograms
    char tname[100];
    sprintf(tname, "TV1730_QL_%i", 0);

    TH1D *tmp = (TH1D*)gDirectory->Get(tname);
    if (tmp) return;

    //Otherwise make histograms
    clear();

    for(int iBoard=0; iBoard<NDPPBOARDS; iBoard++){
        for(int ch = 0; ch < V1730_MAXCHAN; ch++){ // loop over 8 channels
            char name[100];
            char title[100];
            sprintf(name,"TV1730_QL_%i_%i", iBoard,ch);
            sprintf(title,"V1730 QL (for PSD>0.3) for channel=%i Board=%i", ch, iBoard);

	    TH1D *tmp ;
	    if (ch == 13){
	      tmp = new TH1D(name, title, 400, 0., 100000);
	    }else{
	      tmp = new TH1D(name, title, 400, 0., 10000);
	    }
            tmp->SetXTitle("Q-Long");

            push_back(tmp);

        }
    }
}

void TV1730_QL::UpdateHistograms(TDataContainer& dataContainer){

    TV1730DppPsdData *data = dataContainer.GetEventData<TV1730DppPsdData>("W500");
    if(!data) return;

    /// Get the Vector of ADC Measurements.
    std::vector<ChannelMeasurement> measurements = data->GetMeasurements();

    for(unsigned int i = 0; i < measurements.size(); i++){

        ChannelMeasurement meas = measurements[i];
        int ch = meas.GetChannel();
        double psd = 0;

	//        if(meas.GetQlong() != 0)
        //    psd = (double)(meas.GetQlong() - meas.GetQshort())/(double)meas.GetQlong();
        //if(psd > 0.3)
	GetHistogram(ch)->Fill(meas.GetQlong());
	//	if(ch == 13) std::cout << "QL :  " << meas.GetQlong() << std::endl;
    }
}

void TV1730_QL::BeginRun(int transition,int run,int time){
    CreateHistograms();
}

void TV1730_QL::EndRun(int transition,int run,int time){}



/// TV1730_QS ----------------------------------------------
TV1730_QS::TV1730_QS(){
    SetNumberChannelsInGroup(V1730_MAXCHAN);
    SetGroupName("Module");
    SetChannelName("Channel");
    CreateHistograms();
}

void TV1730_QS::CreateHistograms(){

    // check if we already have histograms
    char tname[100];
    sprintf(tname, "TV1730_QS_%i", 0);

    TH1D *tmp = (TH1D*)gDirectory->Get(tname);
    if (tmp) return;

    //Otherwise make histograms
    clear();

    for(int iBoard=0; iBoard<NDPPBOARDS; iBoard++){
        for(int ch = 0; ch < V1730_MAXCHAN; ch++){ // loop over 8 channels
            char name[100];
            char title[100];
            sprintf(name,"TV1730_QS_%i_%i", iBoard,ch);
            sprintf(title,"V1730 QS (for PSD>0.3) for channel=%i Board=%i", ch, iBoard);

	    TH1D *tmp ;
	    if (ch == 13){
	      tmp = new TH1D(name, title, 400, 0., 100000);
	    }else{
	      tmp = new TH1D(name, title, 400, 0., 10000);
	    }
            tmp->SetXTitle("Q-Long");

            push_back(tmp);

        }
    }
}

void TV1730_QS::UpdateHistograms(TDataContainer& dataContainer){

    TV1730DppPsdData *data = dataContainer.GetEventData<TV1730DppPsdData>("W500");
    if(!data) return;

    /// Get the Vector of ADC Measurements.
    std::vector<ChannelMeasurement> measurements = data->GetMeasurements();

    for(unsigned int i = 0; i < measurements.size(); i++){

        ChannelMeasurement meas = measurements[i];
        int ch = meas.GetChannel();
        double psd = 0;

	//        if(meas.GetQlong() != 0)
        //    psd = (double)(meas.GetQlong() - meas.GetQshort())/(double)meas.GetQlong();
        //if(psd > 0.3)
	GetHistogram(ch)->Fill(meas.GetQshort());
	//	if(ch == 13) std::cout << "QL :  " << meas.GetQlong() << std::endl;
    }
}

void TV1730_QS::BeginRun(int transition,int run,int time){
    CreateHistograms();
}

void TV1730_QS::EndRun(int transition,int run,int time){}
