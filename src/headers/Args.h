/*
 * Args.h
 *
 *  Created on: 18 Feb 2016
 *      Author: vkrishnan
 */

#ifndef ARGS_H_
#define ARGS_H_
#include <stdlib.h>
#include <math.h>
class Args{
public:
	int bscrunch;

	int start_channel;
	int end_channel;

	int nfft;

	long start_sample;
	long num_samples;

	char* device;

	bool verbose;

	std::string file1;

	std::string file2;

	float sub_sample_resolution;

	float artificial_delay;



	Args();
	void parse(int aargc, char** argv);
};



#endif /* ARGS_H_ */
