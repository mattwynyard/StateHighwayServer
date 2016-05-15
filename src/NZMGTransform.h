#pragma once

#ifndef NZMGTRANSFORM_H
#define NZMGTRANSFORM_H

//NZGMTransform Header file

#include <iostream>
#include <cmath>
#include <complex>
#include <array>

/** This class transforms New Zealand Map grid coordinates to WSG84 (latitude,longitude)
 * coordinates. NZMG is intially converted to NZGD1949 and then a datum shift to
 * NZGD2000, which are compatible with WGS84. All constants and formulas were obtained
 * from the LINZ website
 * http://www.linz.govt.nz/data/geodetic-system/coordinate-conversion/projection-conversions/new-zealand-map-grid
 * Author: Matt Wynyard Jan 2016
 */

class NZMGTransform {

private:
	std::complex<double> z;
	const int N0 = 6023150; //false northing
	const int E0 = 2510000; //false easting
	const int NZMGLatOrigin = -41; //origin latitude
	const int NZMGLongOrigin = 173; //origin longitude

	const std::array<std::complex<double>, 6> c = {{std::complex<double>(1.3231270439, 0),
												   std::complex<double>(-0.577245789, -0.007809598),
												   std::complex<double>(0.508307513, -0.112208952),
												   std::complex<double>(-0.15094762, 0.18200602),
												   std::complex<double>(1.01418179, 1.64497696),
													std::complex<double>(1.9660549, 2.5127645)}};
	const std::array<std::complex<double>, 6> b = {{std::complex<double>(0.7557853228, 0.0),
													 std::complex<double>(0.249204646, 0.003371507),
													std::complex<double>(-0.001541739, 0.04105856),
													std::complex<double>(-0.10162907, 0.01727609),
													std::complex<double>(-0.26623489, -0.36249218),
													std::complex<double>(-0.6870983, -1.1651967)}};
	const std::array<double, 9> d = {{1.5627014243, 0.5185406398, -0.0333309800,
													-0.1052906000, -0.0368594000, 0.0073170000,
									                0.0122000000, 0.0039400000, -0.0013000000}};

	//datum info
	//const double NZGD1949f = 0.003367003;
	const double NZGD1949e2 = (2 * 0.003367003) - pow(0.003367003, 2);
    const double NZGD1949a = 6378388; //Semi-major axis
    
    const double NZGD2000a = 6378137;
    const double NZGD2000f = 0.003352811;
    //Double NZGD2000InverseFlattening = 298.2572221;
    const double NZGD2000e2 = (2 * 0.003352811) - pow(0.003352811, 2);
    
    //NZ1949 -> NZGD2000 differences
    const double differenceX = 54.4;
    const double differenceY = -20.1;
    const double differenceZ = 183.1;

	/** Detemines the complex number theta as a series of approximations
	 * return the complex number used for conversion
	 */
	std::complex<double> theta0();

	/** Recursively performs successive operation on theta0 to reduce to millimetre accuracy
	 * param theta0 the complex number to reduce
	 * param n the number of succesive operations
	 * return the reduce complex number used for transform
	 */
    std::complex<double> thetaSuccessive(std::complex<double> theta0, int n);

	/** Calculates the latitude and longitude given the complex number theta
	 * param theta the complex number to convert.
	 * return an array containing the latitude and longitude with uncorrected
	 * datum.
	 */
	std::array<double, 2> calcLatLong(std::complex<double> theta);

	/** Performs a datum shift from NZGD1949 to NZGD200
	* param coord latitude and longtitude NZGD1949
	* return an array containing the latitude and longtitude equivalent to WSG84
	*/
	std::array<double, 2> datumShift(	std::array<double, 2> coord);

 public:

	//Default NZMGTransform constructor
	NZMGTransform();

	//Destructor
	 ~NZMGTransform();

	/** Performs the various function calls to calcluate the conversion
	 * param array contaning NZMG easting and northing
	 * return array containing the converted latitude and longtitude
	 */
	std::array<double, 2> nzmgToNZGD1949(double *coordinate);
};


#endif
