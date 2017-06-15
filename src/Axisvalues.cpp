/*
 * Axisvalues.cpp
 *
 *  Created on: 29 May 2016
 *      Author: vkrishnan
 */

#include "headers/Axisvalues.h"
#include "headers/utilities.h"

std::map<int,float*>* Axisvalues::x_axis_map;
Axisvalues::Axisvalues() {
	// TODO Auto-generated constructor stub

}

Axisvalues::~Axisvalues() {
	// TODO Auto-generated destructor stub
}

float* Axisvalues::getxaxis(int n) {
	float* result;
	if(Axisvalues::x_axis_map==NULL) Axisvalues::x_axis_map =  new std::map<int,float*>();
	std::map<int,float*>::iterator it = Axisvalues::x_axis_map->find(n);
	if(it==Axisvalues::x_axis_map->end()){
		result = new_and_check<float>(n,"x axis");
		for(int i=0;i<n;i++)
			result[i]=i;
		Axisvalues::x_axis_map->insert(std::pair<int,float*>(n,result));
	}
	else{
		result = it->second;
	}
	return result;
}



//void Axisvalues::freemap(){
//	if(Axisvalues::x_axis_map==NULL) return;
//	std::map<int,float*>::iterator it;
//	for(it = Axisvalues::x_axis_map->begin();it!=Axisvalues::x_axis_map->end();it++){
//		delete[] it->second;
//	}
//	delete Axisvalues::x_axis_map;
//
//}
