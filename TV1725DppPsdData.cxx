#include "TV1725DppPsdData.hxx"

#include <iostream>



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
  
  // Loop over dual channel data
  for(int ch = 0; ch < 8; ch++){
    
    if((1<<ch) & GetChMask()){
      
      
      uint32_t header0 = GetData32()[counter];
      counter++;
      uint32_t header1 = GetData32()[counter];
      counter++;
      uint32_t header2 = GetData32()[counter];
      counter++;
      uint32_t size = header0 & 0x7fff;

      ChannelMeasurement meas = ChannelMeasurement(ch,header0,header1,header2);
      
      int nsamples = size - 5; // calculate number of samples.
      
      std::vector<uint32_t> Samples;
      for(int i = 0; i < nsamples; i++){
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
      
      if(Samples.size() != (0xffff & header1 )*8)
	std::cout << "Check2: " << Samples.size() << " " << (0xffff & header1 )*8 
		  << " whoops, mistake in decoding, sample size not as expected. " << std::endl;

      fMeasurements.push_back(meas);
      
    }
  }
  //std::cout << "Number of measurements: " << fMeasurements.size() << std::endl;

  // Check that the final counter seems to match the expected aggregate size.
  if((unsigned int)counter != (0xfffffff & GetData32()[0])){
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
