#include "TV1725DppPsdData.hxx"

#include <iostream>

#include <iostream>
#include <iomanip>


TV1725DppPsdData::TV1725DppPsdData(int bklen, int bktype, const char* name, void *pdata):
    TGenericData(bklen, bktype, name, pdata)
{
  
  //printf("Start decoding!!! %i\n",sizeof(long int));

  // Do decoding.  Decoding is complicated by the fact that there can be 
  // multiple events in the same bank.  So need to find and save multiple
  // event headers, trailers.



  fGlobalHeader.push_back(GetData32()[0]);
  fGlobalHeader.push_back(GetData32()[1]);
  fGlobalHeader.push_back(GetData32()[2]);
  fGlobalHeader.push_back(GetData32()[3]);
  
  //  std::cout << "bklen: " << bklen << std::endl;
  // Do some sanity checking.  
  // Make sure first word has right identifier
  if( (GetData32()[0] & 0xf0000000) != 0xa0000000) 
    std::cerr << "First word has wrong identifier; first word = 0x" 
	      << std::hex << GetData32()[0] << std::dec << std::endl;
  
  int counter = 4;


  //std::cout << "TV1725 add " << std::endl;
  

  // std::cout << "Board Agg size " << (GetData32()[0] & 0xfffffff) 
  //	    << " bklen: " << bklen << std::endl;
  //  std::cout << std::hex << "0x" << GetData32()[0] << std::dec << std::endl;
  //std::cout << std::hex << " channel mask:" << GetChMask() << std::dec << std::endl;



  if(0) for (int i = 0; i < 128; i++){
    std::cout << std::hex << std::setw(8) <<  std::setfill('0') << GetData32()[i] << " ";
    if (i%8 == 7) std::cout << std::dec << std::endl;


  }
  //  std::cout << std::dec << std::endl;






  // Loop over dual channel data
  for(int ch = 0; ch < 8; ch++){
    
    if((1<<ch) & GetChMask()){
      
      
      uint32_t header0 = GetData32()[counter];
      uint32_t size = header0 & 0x3fffff;

      uint32_t ch_agg_size = size ; // calculate number of samples.
      //      std::cout << "Have data for ch group=" << ch << "  with ch_agg_size " << ch_agg_size << std::endl;
      // Let's check if the next channel aggregate is in the right place
      //for(int i = 0; i < 9; i++){
      //	std::cout << std::hex << GetData32()[counter+ch_agg_size+i]  << std::dec << std::endl;
      //}
      counter++;

      uint32_t header1 = GetData32()[counter];
      counter++;

      uint32_t n_samples_d8 = header1 & 0xffff ; // calculate number of samples.
      //      std::cout << "n_samples_d8 " << n_samples_d8 << std::endl;
      
      int total_size_ch_agg = n_samples_d8*4 + 3;
      int total_events = (ch_agg_size-2)/total_size_ch_agg;
      //std::cout << "total_size_ch_agg : " << total_size_ch_agg << std::endl;
      //  std::cout << "total number of events in channel aggregate: " << total_events << std::endl;

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
      
	if(Samples.size() != (0xffff & header1 )*8){
	  
	  std::cout << "Check2: " << Samples.size() << " " << (0xffff & header1 )*8 
		    << " whoops, mistake in decoding, sample size not as expected. counter="
		    << counter << std::endl;
	}
	
	fMeasurements.push_back(meas);
      
      }
    }
  }

  //  std::cout << "Finished looping over many! " << counter << " total measurements: " << fMeasurements.size() << std::endl;

  //std::cout << "Number of measurements: " << fMeasurements.size() << std::endl;
  if(GetChMask() == 1999999){
    std::cout << "----------------------" << std::endl;
    for(int i = 0; i < 10; i++){
      printf("%i 0x%08x\n",i,GetData32()[i]);
      //std::cout << i <<  std::hex << " 0x" << GetData32()[i] << std::dec << std::endl;
    }
  }

  // Check that the final counter seems to match the expected aggregate size.
  if(0 && (unsigned int)counter != (0xfffffff & GetData32()[0])){
    std::cout << "_________________________________________________________" << std::endl;
    std::cout << "_________________________________________________________" << std::endl;
    std::cout << "Check:  " << counter << " " << (0xfffffff & GetData32()[0]) 
	      << "Whoops, multi event readout, decoding is wrong@! " << std::endl;
    for(int i = 0; i < bklen; i++){
      std::cout << i <<  std::hex << " 0x" << GetData32()[i] << std::dec << std::endl;
    }
    std::cout << "_________________________________________________________" << std::endl;
  }
  

}



void TV1725DppPsdData::Print(){

  std::cout << "V1725Dpp decoder for bank " << GetName().c_str() << std::endl;


}
