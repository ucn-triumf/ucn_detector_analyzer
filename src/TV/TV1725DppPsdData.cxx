#include "TV1725DppPsdData.hxx"

#include <iostream>

#include <iostream>
#include <iomanip>


// A W500 bank holds CAEN V1725 DPP-PSD data with a nested, self-describing layout:
//
//   bank
//    └── board aggregate(s)        : 4-word header (size + channel mask) + channel data
//         └── channel aggregate(s) : 2-word header (size + record format) + records
//              └── record(s)       : one hit: time tag, optional samples/extras/charge
//
// All three levels carry their own size field, so we navigate by those sizes rather
// than assuming there is exactly one of each. This matters because the firmware packs
// multiple board aggregates into one bank when its buffer fills between readouts, and
// because different channels (e.g. detector vs. timing-marker channels) enable
// different per-record fields and therefore have different record sizes.
TV1725DppPsdData::TV1725DppPsdData(int bklen, int bktype, const char* name, void *pdata):
    TGenericData(bklen, bktype, name, pdata)
{
  // Keep the first board aggregate header for the public accessors
  // (GetChMask, GetEventCounter, ...). Note this only describes the first aggregate.
  fGlobalHeader.push_back(GetData32()[0]);
  fGlobalHeader.push_back(GetData32()[1]);
  fGlobalHeader.push_back(GetData32()[2]);
  fGlobalHeader.push_back(GetData32()[3]);

  // Every board aggregate header begins with the 0xA nibble; flag if the first is wrong.
  if( (GetData32()[0] & 0xf0000000) != 0xa0000000)
    std::cerr << "First word has wrong identifier; first word = 0x"
	      << std::hex << GetData32()[0] << std::dec << std::endl;

  // 'counter' is our word offset into the bank; it walks forward through every level.
  int counter = 0;

  // ---- Level 1: board aggregates -------------------------------------------------
  // Loop over all board aggregates in the bank, not just the first. Stop once we run
  // off the end of the bank or hit a word that isn't a valid aggregate header.
  while(counter < bklen) {

    // The board aggregate header word 0 must carry the 0xA identifier nibble.
    if( (GetData32()[counter] & 0xf0000000) != 0xa0000000) break;

    int agg_start = counter;
    uint32_t agg_size    = GetData32()[counter]     & 0xfffffff; // total words in this aggregate (incl. 4-word header)
    uint32_t agg_ch_mask = GetData32()[counter + 1] & 0xff;      // which of the 8 dual-channel groups are present

    // ---- Diagnostic: detect aggregates dropped at the board (e.g. buffer overflow) ----
    // Header word 2 holds the 23-bit board aggregate counter, which increments once per
    // aggregate the board emits. A forward jump of more than 1 means the board produced
    // aggregates that never reached us: events were lost in the digitizer/DAQ, not in
    // this decoder. (A backward jump is a run reset or 23-bit wrap, which we just resync
    // to.) Word 3 is the aggregate time tag, useful for correlating with a missing marker.
    static long lastAggCounter = -1;
    uint32_t aggCounter = GetData32()[agg_start + 2] & 0x7fffff;
    if(lastAggCounter >= 0 && (long)aggCounter > lastAggCounter + 1){
      std::cerr << "TV1725: board aggregate counter gap -- "
                << (aggCounter - lastAggCounter - 1) << " aggregate(s) dropped at the board; "
                << "counter " << lastAggCounter << " -> " << aggCounter
                << ", time tag " << GetData32()[agg_start + 3]
                << ", ch mask 0x" << std::hex << agg_ch_mask << std::dec << std::endl;
    }
    lastAggCounter = aggCounter;

    counter += 4; // skip the 4-word board aggregate header

    // ---- Level 2: channel aggregates (one per enabled dual-channel group) --------
    for(int ch = 0; ch < 8; ch++){

      // Skip groups that did not trigger; only flagged groups have data in this aggregate.
      if(!((1<<ch) & agg_ch_mask)) continue;

      int ch_agg_start = counter;
      uint32_t header0 = GetData32()[counter];
      uint32_t ch_agg_size = header0 & 0x3fffff; // total words in this channel aggregate (incl. 2-word header)
      counter++;

      uint32_t header1 = GetData32()[counter]; // describes the per-record format for this group
      counter++;

      // A record's size depends on which optional fields the channel records. These
      // flags live in header1 and can differ from group to group, so a timing-marker
      // channel (e.g. the cycle-start input) may use a smaller record than a detector
      // channel. We must read the flags instead of assuming a fixed record size, or
      // the offset drifts and later channel groups are misread.
      uint32_t n_samples_d8 = header1 & 0xffff;       // waveform length / 8 (in samples)
      bool samples_enabled = (header1 & 0x08000000);  // bit 27: waveform samples present
      bool extras_enabled  = (header1 & 0x10000000);  // bit 28: extras (extended time / baseline) word present
      bool charge_enabled  = (header1 & 0x40000000);  // bit 30: charge (Qlong/Qshort) word present
      int ch_agg_end = ch_agg_start + (int)ch_agg_size; // first word past this channel aggregate

      // ---- Level 3: records ------------------------------------------------------
      // Walk records until we reach this channel aggregate's declared end. Bounding
      // the loop by ch_agg_end (rather than a computed event count) means that even
      // if a single record is decoded wrong, we still resume cleanly at the next
      // channel group instead of losing the rest of the aggregate.
      while(counter < ch_agg_end){

        // header2 is always present: it holds the trigger time tag and the
        // odd/even-channel bit that the ChannelMeasurement uses to resolve the channel.
        uint32_t header2 = GetData32()[counter];
        counter++;
        ChannelMeasurement meas = ChannelMeasurement(ch,header0,header1,header2);

        // Optional waveform: n_samples_d8*8 samples, packed two-per-word, so
        // n_samples_d8*4 words. The counter<ch_agg_end guard is defensive against a
        // bad length so we never read past the aggregate boundary.
        if(samples_enabled){
          std::vector<uint32_t> Samples;
          for(unsigned int i = 0; i < n_samples_d8*4 && counter < ch_agg_end; i++){
            uint32_t sample = (GetData32()[counter] & 0x3fff);
            Samples.push_back(sample);
            sample = (GetData32()[counter] & 0x3fff0000) >> 16;
            Samples.push_back(sample);
            counter++;
          }
          meas.AddSamples(Samples);
        }

        // Optional extras word (extended time tag / baseline), present only if enabled.
        if(extras_enabled){
          meas.AddExtra(GetData32()[counter]);
          counter++;
        }
        // Optional charge word (Qlong/Qshort), present only if enabled.
        if(charge_enabled){
          meas.AddQs(GetData32()[counter]);
          counter++;
        }

        fMeasurements.push_back(meas);
      }

      // Snap to the next channel aggregate using its declared size, absorbing any
      // residual drift from the record loop above.
      counter = ch_agg_end;
    }

    // Snap to the next board aggregate using its declared size, for the same reason.
    counter = agg_start + (int)agg_size;
  }

  // ---- Diagnostic: detect decoder desync within a bank --------------------------------
  // A correctly-parsed bank is fully consumed: the final offset should equal bklen.
  // If it stops short, a board aggregate header failed the 0xA identifier check and we
  // bailed early -- meaning the bank was malformed or misparsed and any data in the tail
  // (possibly a cycle-start marker) was dropped by *this decoder* rather than the board.
  if(counter != bklen){
    std::cerr << "TV1725: bank not fully decoded -- stopped at word " << counter
              << " of " << bklen << " (" << (bklen - counter)
              << " word(s) left undecoded); possible malformed/misparsed bank." << std::endl;
  }
}



void TV1725DppPsdData::Print(){

  std::cout << "V1725Dpp decoder for bank " << GetName().c_str() << std::endl;


}
