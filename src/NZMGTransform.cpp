/**NZMGTransform class implementation file
 * See NZMGTransform.h for function descriptions
 * www.linz.govt.nz contains explaination of mathemtics
 */
#include "NZMGTransform.h"

//Constructor
NZMGTransform::NZMGTransform() {}

//Destructor
NZMGTransform::~NZMGTransform() {

	//std::cout << "destroyed" << std::endl;
}

std::array<double, 2> NZMGTransform::nzmgToNZGD1949(double *coordinate) {
	double east = *(coordinate);
	double north = *(coordinate + 1);
	std::cout.precision(12);
	z.real((north - N0) / NZGD1949a);
	z.imag((east - E0) / NZGD1949a);
	std::complex<double> theta0 =  NZMGTransform::theta0();
	std::complex<double> theta = thetaSuccessive(theta0, 2);
	std::array<double, 2> coord = calcLatLong(theta);

	return datumShift(coord);
}

std::complex<double> NZMGTransform::theta0() {
	std::complex<double> theta0(0, 0);
	for (int i = 0; i < c.size(); i++) {
		theta0 += c[i] * std::pow(z, (i + 1));
	}
	return theta0;
}

std::complex<double> NZMGTransform::thetaSuccessive(std::complex<double> theta0, int n) {
	std::complex<double> numerator(0, 0);
	std::complex<double> denominator(0, 0);
	for (int i = 0; i < b.size(); i++) {
		std::complex<double> c(i,0);
		std::complex<double> d(i + 1, 0);
		numerator += c * b[i] * std::pow(theta0, i + 1);
		denominator += d * b[i] * std::pow(theta0, i);
	}
	numerator += z;
	std::complex<double> theta(0, 0);
	theta = numerator / denominator;
	if (n == 0) {
		return theta;
	} else {
		return thetaSuccessive(theta, n - 1);
	}
}

std::array<double, 2> NZMGTransform::calcLatLong(std::complex<double> theta) {
	const double deltaPsi = theta.real();
	const double deltaLamda = theta.imag();
	double deltaPhi = 0.0;
	for (int i = 0; i < d.size(); i++) {
		deltaPhi += (d[i] * std::pow(deltaPsi, i + 1));
	}
	const double lat = NZMGLatOrigin + ((std::pow(10, 5) / 3600) * deltaPhi);
	const double longitude = NZMGLongOrigin + ((180 / M_PI) * deltaLamda);
	std::array<double, 2> coord = {{lat, longitude}};
	return coord;
}

std::array<double, 2>  NZMGTransform::datumShift(	std::array<double, 2> coord) {
	const double latRadians = coord[0] * (M_PI / 180);
	const double lngRadians = coord[1] * (M_PI / 180);

	const double v = NZGD1949a / (sqrt(1 - (NZGD1949e2 * pow(sin(latRadians), 2))));
	double x_cart = v * cos(latRadians) * cos(lngRadians);
	double y_cart = v * cos(latRadians) * sin(lngRadians);
	double z_cart = (v * (1 - NZGD1949e2)) * sin(latRadians);
	x_cart += differenceX;
	y_cart += differenceY;
	z_cart += differenceZ;

	const double p = sqrt(pow(x_cart, 2) + pow(y_cart, 2));
	const double r = sqrt(pow(p, 2) + pow(z_cart, 2));
	const double mu = atan((z_cart / p) * ((1 - NZGD2000f) + ((NZGD2000e2 * NZGD2000a) / r)));
	const double num = (z_cart * (1 - NZGD2000f)) + (NZGD2000e2 * NZGD2000a * pow(sin(mu), 3));
	const double denom = (1 - NZGD2000f) * (p - (NZGD2000e2 * NZGD2000a * pow(cos(mu), 3)));

	double lat = atan(num / denom);
	double lng = atan(y_cart / x_cart);

	lat = (180 / M_PI) * lat;
	lng = 180 + (180 / M_PI) * lng;

	std::array<double, 2> a = {{lat, lng}};
	return a;
	
}
	

	

