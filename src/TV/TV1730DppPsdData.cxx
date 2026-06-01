#include "TV1730DppPsdData.hxx"

#include <iostream>

#include <iostream>
#include <iomanip>


TV1730DppPsdData::TV1730DppPsdData(int bklen, int bktype, const char* name, void *pdata):
    TGenericData(bklen, bktype, name, pdata)
{
  // Save first board aggregate header for public accessors (GetChMask, GetEventCounter, etc.)
  fGlobalHeader.push_back(GetData32()[0]);
  fGlobalHeader.push_back(GetData32()[1]);
  fGlobalHeader.push_back(GetData32()[2]);
  fGlobalHeader.push_back(GetData32()[3]);

  if( (GetData32()[0] & 0xf0000000) != 0xa0000000)
    std::cerr << "First word has wrong identifier; first word = 0x"
	      << std::hex << GetData32()[0] << std::dec << std::endl;

  int counter = 0;

  // Loop over all board aggregates in the bank. The V1730 firmware can pack
  // multiple aggregates into one MIDAS bank when the buffer fills between readouts.
  while(counter < bklen) {

    // Stop if the next word doesn't have the board aggregate identifier.
    if( (GetData32()[counter] & 0xf0000000) != 0xa0000000) break;

    int agg_start = counter;
    uint32_t agg_size    = GetData32()[counter]     & 0xfffffff;
    uint32_t agg_ch_mask = GetData32()[counter + 1] & 0xff;
    counter += 4; // skip 4-word board aggregate header

    // Loop over dual channel data for this aggregate
    for(int ch = 0; ch < 8; ch++){

      if(!((1<<ch) & agg_ch_mask)) continue;

      uint32_t header0 = GetData32()[counter];
      uint32_t ch_agg_size = header0 & 0x3fffff;
      counter++;

      uint32_t header1 = GetData32()[counter];
      counter++;

      uint32_t n_samples_d8 = header1 & 0xffff;
      int total_size_ch_agg = n_samples_d8*4 + 3;
      int total_events = (ch_agg_size-2)/total_size_ch_agg;

      for(int evt = 0; evt < total_events; evt++){

        uint32_t header2 = GetData32()[counter];
        counter++;
        ChannelMeasurement meas = ChannelMeasurement(ch,header0,header1,header2);

        std::vector<uint32_t> Samples;
        for(int i = 0; i < n_samples_d8*4; i++){
          uint32_t sample = (GetData32()[counter] & 0x3fff);
          Samples.push_back(sample);
          sample = (GetData32()[counter] & 0x3fff0000) >> 16;
          Samples.push_back(sample);
          counter++;
        }
        meas.AddSamples(Samples);

        uint32_t extras = GetData32()[counter];
        counter++;
        meas.AddExtra(extras);
        uint32_t qs = GetData32()[counter];
        counter++;
        meas.AddQs(qs);

        if(Samples.size() != (0xffff & header1)*8){
          std::cout << "Check2: " << Samples.size() << " " << (0xffff & header1)*8
                    << " whoops, mistake in decoding, sample size not as expected. counter="
                    << counter << std::endl;
        }

        fMeasurements.push_back(meas);
      }
    }

    // Advance to the next board aggregate using the size from this aggregate's header.
    counter = agg_start + (int)agg_size;
  }
}



void TV1730DppPsdData::Print(){

  std::cout << "V1730Dpp decoder for bank " << GetName().c_str() << std::endl;


}
