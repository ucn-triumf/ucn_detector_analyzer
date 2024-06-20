#ifndef THe3CountsInSequence_h
#define THe3CountsInSequence_h

#include <string>
#include "THistogramArrayBase.h"
#include "TV792NData.hxx"
#include "TGenericData.hxx"
#include "TDirectory.h"
#include <sys/time.h>


/// Class for making a plot of UCN rate vs time as seen by He-3 detector, readout with V785 peak-sensing ADC.
class THe3CountsInSequence : public THistogramArrayBase {
    public:
        THe3CountsInSequence();
        virtual ~THe3CountsInSequence(){};

        /// Update the histograms for this canvas.
        void UpdateHistograms(TDataContainer& dataContainer);

        /// Take actions at begin run
        void BeginRun(int transition,int run,int time);

        /// Take actions at end run
        void EndRun(int transition,int run,int time);

    private:

        void CreateHistograms();

        // variable to keep track of when we are in a sequence
        // so we can tell when the sequence starts.
        int insequence;

        // unix timestamp of the last time a sequence started.
        int startSequence;
};
#endif


