
#include "TLi6GainCalib.h"
#include <fstream>
#include <exception>

// method to add another calib file and validity range
void TLi6GainCalib::add_calib( const std::string& fname, double tmin,double tmax ){
  vtmin.push_back( tmin );
  vtmax.push_back( tmax );
  std::map< int, double > calimap;

  std::ifstream in;
  in.open( fname );
  if ( !in.is_open() ){
    // failed to open!
    // try the default location:
    in.open("/home/ucn/online/ucn_detector_analyzer/calib_li6_gains.txt");
    if ( !in.is_open() ){
      std::cerr<<"TLi6GainCalib failed to open "<<fname<<std::endl;
      throw std::runtime_error("TLi6GainCalib::add_calib");
    }
  }

  std::cout<<"TLi6GainCalib::add_calib: tmin="<<tmin<<" tmax="<<tmax<<std::endl;

  int idigi, ich, igch;
  double qcali;
  while ( in >> idigi >> ich >> qcali ){
    igch = 8*idigi + ich;
    calimap[ igch ] = qcali;
    std::cout<<"  idigi="<<idigi<<" ich="<<ich<<" igch="<<igch<<" qcali="<<calimap[igch]<<std::endl;
  }

  calimaps.push_back( calimap );
}


// get calib constant
double TLi6GainCalib::get_cali( int idigi, int ich, double t ) const{
  double igch = 8*idigi+ich;
  //std::cout<<"get_cali idigi="<<idigi<<" ich="<<ich<<" igch="<<igch<<std::endl;
  return get_cali( igch, t );
}
double TLi6GainCalib::get_cali( int igch, double t ) const{
  static int failcount=0;
  for ( unsigned i=0; i!=vtmin.size(); ++i ){
    if ( t > vtmin[i] && t < vtmax[i] ){
      //std::cout<<"mapsize="<<calimaps[i].size()<<" igch="<<igch<<std::endl;
      if ( calimaps[i].find( igch ) == calimaps[i].end() ) break;
      const double qc = calimaps[i].at(igch);
      //std::cout<<"TLi6GainCalib::get_cali ="<<Li6_Nominal_Gain/qc<<std::endl;
      return Li6_Nominal_Gain / qc;
    }
  }
  ++failcount;
  if ( failcount < 10 ){
    std::cerr<<"TLi6GainCalib::get_cali igch="<<igch<<" t="<<t<<" Failed to get calib"
	     <<" returning 1.0"<<std::endl;
  }
  if (failcount==10) {
    std::cerr<<"Supressing warning: TLi6GainCalib::get_cali igch="<<igch<<" t="<<t<<" Failed to get calib"
	     <<" returning 1.0"<<std::endl;
  }
  return 1.0;
}


// calculate calibrated charge
double TLi6GainCalib::get_q( int idigi, int ich, double qin, double t ) const{
  double igch = 8*idigi+ich;
  //std::cout<<"get_q idigi="<<idigi<<" ich="<<ich<<" igch="<<igch<<std::endl;
  return get_q( igch, qin, t );
}
double TLi6GainCalib::get_q( int igch, double qin, double t ) const{
  //std::cout<<"get_q igch="<<igch<<std::endl;
  double fcali = get_cali( igch, t );
  //std::cout<<"fcali="<<fcali<<std::endl;
  return qin*fcali;
}


std::ostream& operator<<( std::ostream& os, const TLi6GainCalib& gc ){
  for ( unsigned i=0; i!=gc.vtmin.size(); ++i ){
    os << "TLi6GainCalib tmin="<<gc.vtmin[i]<<" to tmax="<<gc.vtmax[i] << std::endl;
    for ( unsigned ich=0; ich<gc.calimaps[i].size(); ++ich ){
      for ( const std::pair< int, double >& p : gc.calimaps[ich] ){
	os<<"  igch "<< p.first<<" "<<p.second<<" "<<Li6_Nominal_Gain/p.second<<std::endl;
      }
    }
  }
  return os;
}
