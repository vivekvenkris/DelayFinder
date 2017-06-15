/*
 * Dadafile.h
 *
 *  Created on: 12 Feb 2016
 *      Author: vkrishnan
 */

#ifndef DADAFILE_H_
#define DADAFILE_H_
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <string>
#include <map>
#include <complex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <ctype.h>
#include <iostream>
#include "Dadafile.h"
#include "utilities.h"

class Dadafile{

public:
	std::map<std::string, std::string> header_map;
	float _Complex** gulp_complex;
	long x_dim;
	int y_dim;
	long start_sample;
	std::string name;
	FILE* file;
	Dadafile(const char* file);
	virtual ~Dadafile();
	int get_header_map();
	char* read_header();
	int readgulp();
	int ascii_header_get (const char* header, const char* keyword, const char* format, ...);
	char* ascii_header_find (const char* header, const char* keyword);
	int read_and_check(char* bval,long count);
	int seek_to(long start_sample);
	int deallocate_memory();
	int allocate_memory();
	int initialize();
	static int check_similarity(Dadafile* df1, Dadafile* df2);



};

#endif /* DADAFILE_H_ */
