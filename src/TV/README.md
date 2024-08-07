# TV1725

Notes from [plone](https://ucn.triumf.ca/vertical-ucn-source/daq/midas-daq/v1725-digitizer/?searchterm=v1725):

In 2019 we got a new V1725 16 channel 250MSPS CAEN digitizer, with online pulse finding abilities.  The digitizer is readout using fiber optic signal from daq02.

We use this digitizer to digitize both the Li-6 and He-3 signals; we also inject timing signals to indicate when the cycles start, as well as timing synchronization pulses at the start of the run.

There is no automated procedure yet for setting the V1725 baselines.

## Channel Mapping from 2019

    chan 0-8: Li-6 signals from 9 PMTs
    chan 10: "Cycle Start" signal
    chan 11: time synch at BOR (begin of run) and EOR (end or run) from PPG
    chan 13: He-3 signal from spectroscopic amplifier.

## Additional Notes

The Caen V1720 (waveform digitizer) and V785 (ADC) were used for early UCN production.

fV1725PSDQL, fV1725_PH, fV1725_QL are the ones we actually use.  These are all data from the CAEN V1725 VME module that we use for digitizating the UCN detectors

The fV1725PSDQL, fV1725_PH, fV1725_QL encapsulate the low level information about each channel of the V1725.  In particular, it includes the measurement of pulse height PH , charge long (QL) and pulse shape discrimination for each channel.

The fHe3Detector and fLi6Detector on the other hand encapsulate the higher level information about the UCN hits for each detector. For instance, the fLi6Detector has some flag (something like isUCN) which indicates whether we think this hit is from a UCN or a background; that flag is filled by making some cut on the PH and QL parameters that come from the lower level variables.