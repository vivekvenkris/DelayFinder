/*
 * PGparam.h
 *
 *  Created on: 3 Jul 2016
 *      Author: vkrishnan
 */

#ifndef PGPARAM_H_
#define PGPARAM_H_

#include <string>

class PGparam {
public:
	PGparam();
	virtual ~PGparam();
	void set_params();
	void restore_defaults();
	void set_labels(std::string x, std::string y, std::string title);
	int pgsch;
	int pgslw;
	int pgsci;
	int pgask;
	bool cpgswin;
	std::string title;
	std::string x_label;
	std::string y_label;
	bool bn_x; // baseline and normalsie X axis
	bool bn_y; // baseline and normalsie X axis

};


#endif /* PGPARAM_H_ */
