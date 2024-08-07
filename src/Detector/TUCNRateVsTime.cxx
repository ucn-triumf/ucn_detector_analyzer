#include "TUCNRateVsTime.h"
#include "TDirectory.h"

const int Nchannels = 8;
#include <sys/time.h>

unsigned int timescale[8] = {300, 600, 1200, 2400, 4800, 9600, 19200, 38400};

/// Reset the histograms for this canvas
TUCNRateVsTime::TUCNRateVsTime(bool isLi6, bool isOffline, bool is3HEDET1){

    // save inputs
    fIsLi6 = isLi6;
    fIsOffline = isOffline;
    fIs3HEDET1 = is3HEDET1;

    // initialize
    CreateHistograms();
    lastTimestamp = 0;
}

void TUCNRateVsTime::CreateHistograms(){

    // Otherwise make histograms
    clear();

    // initialize
    fRateVsTime = std::vector<std::pair<int, double>>();

    // loop over channels
    for(int ch = 0; ch < Nchannels; ch++){

        // set name
        char name[100];
        if(fIsLi6)              sprintf(name, "Li6_Rate_%i_%i", 0, ch);
        else if(fIs3HEDET1)     sprintf(name, "He3_Rate_%i_%i", 0, ch);
        else                    sprintf(name, "He3_Det2_Rate_%i_%i", 0, ch);

        // Delete old histograms, if we already have them
        TH1D *old = (TH1D*)gDirectory->Get(name);
        if (old)
            delete old;

        // set title
        char title[100];
        if(fIsLi6)  sprintf(title, "Li-6 UCN Rate vs Time (%i minutes)", timescale[ch]/60);
        else        sprintf(title, "He3 UCN Rate vs Time (%i minutes)", timescale[ch]/60);

        // Create new histogram
        TH1D *tmp = new TH1D(name, title, timescale[0]*1.2, -timescale[ch]-0.5, timescale[ch]*0.2 - 0.5);

        // axis titles
        if(fIsOffline)  tmp->SetXTitle("Time since last event (sec)");
        else            tmp->SetXTitle("Time since now (sec)");
        tmp->SetYTitle("UCN Rate (cts/sec)");

        // add histogram to collection
        push_back(tmp);
    }
}

void TUCNRateVsTime::UpdateHistograms(TUCNHitCollection &hits){

    struct timeval start;
    gettimeofday(&start, NULL);

    // Update the histogram every event... but only if the event timestamp is different from last event
    int timestamp = hits.eventTime;

    // update time; offline we only update every couple seconds; online we can update every 2 seconds.
    int update_time;
    if(fIsOffline)  update_time = 10;
    else            update_time = 2;

    // do the update
    if(timestamp >= lastTimestamp+update_time){

        for(int ch = 0; ch < Nchannels; ch++){
            GetHistogram(ch)->Reset();

            int bin_index = 1;
            double bin_width = GetHistogram(ch)->GetBinWidth(bin_index);
            double low_bin = GetHistogram(ch)->GetBinLowEdge(bin_index);
            int last_found_bin = 0;
            int bin_total = 0;

            for(unsigned int i=0; i < fRateVsTime.size(); i++){
                std::pair<int, double> entry = fRateVsTime[i]; // (time, rate)
                if(!entry.second)
                    continue;
                int timediff = (entry.first - timestamp);

                // Ignore points that are off the graph
                if(timediff < low_bin) continue;

                // Increment bin_index until we find a bin that is lower
                while(timediff > GetHistogram(ch)->GetBinLowEdge(bin_index) + bin_width){
                    bin_index++;
                }

                // Check if we moved onto the next index
                if(last_found_bin != bin_index && last_found_bin != 0){
                    GetHistogram(ch)->SetBinContent(last_found_bin, bin_total);
                    bin_total = 0;
                }

                bin_total += entry.second;
                last_found_bin = bin_index;
            }

            // Set the last bin
            if(bin_total != 0){
                GetHistogram(ch)->SetBinContent(last_found_bin, bin_total);
            }
            GetHistogram(ch)->Scale(1/pow(2, ch));
        }
        lastTimestamp = timestamp;
    }

    // Check if the rateVsTime array needs new entry.
    if(fRateVsTime.size() == 0 || timestamp > fRateVsTime.back().first){

        // Add new entry at back
        fRateVsTime.push_back( std::pair<int, double>(timestamp, 0));

        // maybe start erasing first entry...
        if(fRateVsTime.size() > timescale[Nchannels-1]){
            fRateVsTime.erase(fRateVsTime.begin());
        }
    }

    // loop over measurements
    for(unsigned int j = 0; j < hits.size(); j++){
        int hittime = (int)hits[j].time;

        // Find the right entry and update...
        for(int i = fRateVsTime.size()-1; i >= 0; i--){
            if(hittime == fRateVsTime[i].first){
                fRateVsTime[i].second = fRateVsTime[i].second + 1.0;
                break;
            }
        }
    }
}

void TUCNRateVsTime::BeginRun(int transition, int run, int time){
    CreateHistograms();
}

void TUCNRateVsTime::EndRun(int transition, int run, int time){}
