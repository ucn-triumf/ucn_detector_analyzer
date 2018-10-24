
#ifndef _TLi6GainCalib_h_
#define _TLi6GainCalib_h_

#include <string>
#include <limits>
#include <iostream>
#include <map>
#include <vector>

/* TLI6GainCalib
 * Simple class to hold Li-6 detector charge calibration constants.
 *
 * Initialized by giving it text file holding gain per channel
 * text file has three colums: digitizer channel average-charge-long.
 *
 * Can also provide a time range over which constants are valid.
 *
 * Blair Jamieson (Mar 2018)
 */

const double Li6_Nominal_Gain = 8000.0;

struct TLi6GainCalib {
  TLi6GainCalib( const std::string& fname="calib_li6_gains.txt", 
		 double tmin=-std::numeric_limits<double>::max(),
		 double tmax=std::numeric_limits<double>::max() ){
    add_calib( fname, tmin, tmax );
  }

  // method to add another calib file and validity range
  void add_calib( const std::string& fname="calib_li6_gains.txt", 
		 double tmin=-std::numeric_limits<double>::max(),
		 double tmax=std::numeric_limits<double>::max() );

  // get calib constant
  double get_cali( int idigi, int ich, double t) const;
  double get_cali( int igch, double t) const;

  // calculate calibrated charge
  double get_q( int idigi, int ich, double qin, double t ) const;
  double get_q( int igch, double qin, double t ) const;

  friend std::ostream& operator<<( std::ostream& os, const TLi6GainCalib& gc );
protected:

  std::vector<double> vtmin; // minimum time
  std::vector<double> vtmax; // maximum time
  std::vector< std::map< int, double > > calimaps; // vector of calib constants, stored in maps of global channel, calib const
};
  
#endif
