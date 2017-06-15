/*
 * args.cpp
 *
 *  Created on: 18 Feb 2016
 *      Author: vkrishnan
 */
#include <stdio.h>
#include <stdarg.h>
#include <getopt.h>
#include <ctype.h>
#include<iostream>

#include "headers/Args.h"


using namespace std;

Args::Args(){

	bscrunch = 1024;

	start_channel= -1;
	end_channel= -1;

	device = (char*)"/xs";

	verbose= false;

	start_sample= 0;
	num_samples=1024L;

	nfft = -1;

	sub_sample_resolution = 0.01;

	artificial_delay = 0;




}

void usage() {
	fprintf(stderr, "usage: DelayFinder <dada file>...\n"
			"-l   <long> length (in samples) to take after start_sample. \n"
			" -n  <long> start sample "
			"-b   <int> bscrunch \n"
			"-s  <int> start channel \n"
			"-e  <int> end channel \n"
			"-f  <int> number of points for FFT \n"
			"-g  device     pgplot device name \n"
			"-r  <float> sub sample resolution \n"
			"-d  <float> artificial delay"
	);
}


void Args::parse(int argc, char** argv){
	int c;
	while ((c = getopt(argc, argv, "b:d:s:e:n:f:g:vr:l:")) != -1) {
		printf("%c %s\n", c, optarg);
		switch (c) {

		case 'b':
			bscrunch = ::atoi(optarg);
			break;

		case 's':
			start_channel = ::atoi(optarg);
			break;

		case 'e':
			end_channel =  ::atoi(optarg);
			break;

		case 'l':
			num_samples = ::atol(optarg);
			break;
		case 'n':
			start_sample = ::atol(optarg);
			break;

		case 'f':
			nfft = ::atoi(optarg);
			break;

		case 'g':
			device = optarg;
			break;

		case 'v':
			verbose = true;
			break;

		case 'r':
			sub_sample_resolution = ::atof(optarg);
			break;
		case 'd':
			artificial_delay =  ::atof(optarg);
			break;

		default:
			abort();
		}
	}

	if( argc - optind != 2 ){

		cerr << "2 input files needed." << endl;
		usage();
		exit(EXIT_FAILURE);

	}

	file1 = string(argv[optind]);
	file2 = string(argv[optind+1]);

	nfft = (nfft  < 0 ) ? num_samples : nfft;


}



