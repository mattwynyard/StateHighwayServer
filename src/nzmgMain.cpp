#include "NZMGTransform.h"

#include <iostream>
#include <string>
#include <vector>

int main () {

	std::string data;
	std::vector<double> vect;
	int count = 0;
	char nextChar;
	std::getline(std::cin, data);
	for (int i = 0; i < int(data.length()); i++) {
		nextChar = data.at(i);
		if (isspace(data[i]))
			count++; //count is the number of spaces
	}

	std::cout << count + 1 << std::endl;
	std::cout.precision(12);
	std::stringstream stream(data);
	double n;
    while(stream >> n){
		vect.push_back(n);
    }
	  
	for (int i = 0; i < count + 1; i+=2) {;
		double east = vect.at(i);
		double north = vect.at(i + 1);
		NZMGTransform x = NZMGTransform();
		double a[2] = {east, north};
		std::array<double, 2> b;

		b = x.nzmgToNZGD1949(a);
		if (i == count - 1) {
			std::cout << std::to_string(b[0]) + " " + std::to_string(b[1]);
		} else {
			std::cout << std::to_string(b[0]) + " " + std::to_string(b[1]) + " ";
		}

	}
	
	return 0;
}
