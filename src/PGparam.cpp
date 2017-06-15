/*
 * PGparam.cpp
 *
 *  Created on: 3 Jul 2016
 *      Author: vkrishnan
 */

#include "headers/PGparam.h"
#include "cpgplot.h"

PGparam::PGparam() {
cpgswin=true;
pgask = true;
}



PGparam::~PGparam() {
	// TODO Auto-generated destructor stub
}


void PGparam::set_params(){

	cpgsch(pgsch);
	cpgslw(pgslw);
	cpglab(x_label.c_str(),y_label.c_str(),title.c_str());
	cpgsci(pgsci);

}

void PGparam::restore_defaults(){
		cpgsch(1);
		cpgslw(3);
		cpglab("x","y","title");
		cpgsci(3);
}

void PGparam::set_labels(std::string x, std::string y, std::string title){
	cpglab(x.c_str(),y.c_str(),title.c_str());

}
