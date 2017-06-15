/*
 * Axisvalues.h
 *
 *  Created on: 29 May 2016
 *      Author: vkrishnan
 */

#ifndef HEADERS_AXISVALUES_H_
#define HEADERS_AXISVALUES_H_
#include<map>
class Axisvalues {
public:
	Axisvalues();
	virtual ~Axisvalues();
	static std::map<int,float*>* x_axis_map;
	static float* getxaxis(int n);
	static float* get_time(int n, float start, float TSAMP);

	static void freemap();
};

#endif /* HEADERS_AXISVALUES_H_ */
