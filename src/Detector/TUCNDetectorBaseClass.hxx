#ifndef TUCNDetectorBaseClass_h
#define TUCNDetectorBaseClass_h

#include <string>
#include "TDataContainer.hxx"
#include "TUCNHit.hxx"

#include "TUCNDetectorCharge.h"
#include "TUCNRateVsTime.h"
#include "TSimpleHistogramCanvas.hxx"
#include "TFancyHistogramCanvas.hxx"
#include "TUCNHitsTree.hxx"

#include "TUCNCycleParameters.hxx"


/// Generic base class that handles producing a consistent
/// set of plots and data for Li-6 and He-3 detectors
class TUCNDetectorBaseClass  {
 public:
  TUCNDetectorBaseClass(bool isOffline, bool isLi6, bool saveTree = false, bool is3HEDET1 = true);
  virtual ~TUCNDetectorBaseClass(){};


        // Get the UCN hits for this MIDAS event.
        // This method needs to be defined in each derived class.
        virtual void GetHits(TDataContainer& dataContainer) = 0;

        // We have different methods for checking for the start
        // of a sequence; either the crude method based on
        // timestamps from the VME sequencer module (with ~2second jitter)
        // or a more precise time based on hits in the particular digitizer.
        bool CheckForSequenceStartCrude(TDataContainer& dataContainer);
        virtual bool CheckForSequenceStartPrecise(TDataContainer& dataContainer){return false;};

        // Check for sequence settings
        void CheckForSequenceSettings(TDataContainer& dataContainer);

        /// Method to decide which sequence time to use.
        virtual bool UsePreciseSequenceTime(){return false;};

        // Fill plots specific to detector type.
        // Can be defined in derived class.
        virtual void FillSpecificPlots(){};

        /// Update the histograms for this canvas.
        void ProcessMidasEvent(TDataContainer& dataContainer);

        /// Take actions at begin run
        void BeginRun(int transition,int run,int time);

  /// Take actions at end run
  void EndRun(int transition,int run,int time);

  bool IsLi6(){ return fIsLi6;};
  bool IsHe3(){ return !fIsLi6;};

  bool CycleStarted() const { return fcycle_started; };

  std::string GetDetectorName(){
    if(IsLi6()) return std::string("Li6");
    if(IsHe3()) return std::string("He3");
    return std::string("notset");
  }

  // Method to force analyzer to use simpler PC timestamps
  void UsePCTime(bool usepc = true){
    fUsePCTime = usepc;
  }

        TUCNDetectorCharge* GetChargeHistograms(){return fDetectorCharge;}
        TUCNRateVsTime* GetRateVsTime(){return fRateVsTime;}
        TFancyHistogramCanvas* GetRateVsTimeCanvas(){
            TFancyHistogramCanvas *canvas = new TFancyHistogramCanvas(fRateVsTime,"Rate Vs Time");
            canvas->SetChannelName("Timescale");
            return canvas;
        }

        TH1D* GetHitsInCycle(){return fHitsInCycle;}

        TSimpleHistogramCanvas* GetHitsInCycleCanvas(){
            TSimpleHistogramCanvas *tmp = new TSimpleHistogramCanvas(fHitsInCycle,"Hits In Current Cycle");
            tmp->AddExtraHisto(fHitsInCycleIntime);
            return tmp;
        }

        TSimpleHistogramCanvas* GetHitsInCycleCumulCanvas(){
            TSimpleHistogramCanvas *tmp = new TSimpleHistogramCanvas(fHitsInCycleCumul,"Cumulative Hits In Cycle");
            tmp->AddExtraHisto(fHitsInCycleCumulIntime);
            return tmp;
        }

        TSimpleHistogramCanvas* GetHitsPerCycleCanvas(){
            TSimpleHistogramCanvas *tmp = new TSimpleHistogramCanvas(fHitsPerCycle,"Hits Per Cycle (time)");
            tmp->AddExtraGraph(fHitsPerCycleIntime);
            return tmp;
        }

  TSimpleHistogramCanvas* GetHitsPerCycleHCanvas(){
    TSimpleHistogramCanvas *tmp = new TSimpleHistogramCanvas(fHitsPerCycleH,"Hits Per Cycle","P");
    tmp->AddExtraHisto(fHitsPerCycleHIntime);
    return tmp;
  }
  //  TH1D* GetHitsPerCycle(){return fHitsPerCycle;}

  // Get Vectors of hits per cycle
  std::vector<std::pair<double, double> > GetHitsPerCycle(){return fHitsPerCycleVector;};
  std::vector<std::pair<double, double> > GetInTimeHitsPerCycle(){ return fHitsPerCycleVectorIntime;};
  std::vector<std::vector<std::pair<double, double> > > GetHitsPerCyclePerPeriod(){return fHitsPerCycleVectorPeriods;}
  std::vector<std::pair<double, double> > GetHitsPerCyclePerPeriod(int i){return fHitsPerCycleVectorPeriods[i];}
  std::vector<std::pair<double, double> > GetMonitorCountsAfterIrradiationPerCycle(){ return fMonitorCountsAfterIrradiationPerCycle; }

  TUCNCycleParameters CycleParameters;


protected:

  bool fIsLi6; // Is for Li-6 detector
  bool fIs3HEDET1; // Is this 3HEDET1?
  bool fIsOffline;

  // List of UCN hits
  TUCNHitCollection fHits;

        // List of background hits
        TUCNHitCollection fBackgroundHits;

        // List of non-UCN hits.
        TUCNHitCollection fNonHits;

        // Start time of the current sequence;
        double fCycleStartTime;
        double fValueOpenTime;
        double fLastCycleStartTime;
        double fSeqValveOpenTime;
        double fSeqValveCloseTime;
        double fSeqDelayTime; // this is the length of time that sequencer waits before opening
        double fSeqOpenInterval; // this is the time that valve is open

        bool fUsePCTime;

        // Class for making the hit trees
        TUCNHitsTree *fHitsTree;

    private:

        // UCN hit charge or pulse height spectrum histograms
        TUCNDetectorCharge *fDetectorCharge;
        TUCNRateVsTime *fRateVsTime;

  TH1D *fHitsInCycle;
  TH1D *fHitsInCycleIntime;
  TH1D *fHitsInCycleCumul;
  TH1D *fHitsInCycleCumulIntime;
  TGraph *fHitsPerCycle;
  TGraph *fHitsPerCycleIntime;
  TH1D *fHitsPerCycleH;
  TH1D *fHitsPerCycleHIntime;

    // true if last processed event started a new cycle
    bool fcycle_started;

    // UCN hits in this cycle
  int fTotalHitsCycle;
  int fTotalHitsCycleIntime;  // hits when valve open
  int fTotalHitsCyclePeriods[10];  // total hits per period.
  int fTotalMonitorCountsAfterIrradiation; // hits between end of irradiation and (end of irradiation)+(counting time)

  // vector to store the cycle hits in
  std::vector<std::pair<double, double> > fHitsPerCycleVector;
  std::vector<std::pair<double, double> > fHitsPerCycleVectorIntime;
  std::vector<std::vector<std::pair<double, double> > > fHitsPerCycleVectorPeriods;
  std::vector<std::pair<double, double> > fMonitorCountsAfterIrradiationPerCycle;
};

#endif


