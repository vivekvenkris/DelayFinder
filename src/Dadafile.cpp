/*
 * Dadafile.cpp
 *
 *  Created on: 12 Feb 2016
 *      Author: vkrishnan
 */

#include "headers/Dadafile.h"

using namespace std;

Dadafile::	Dadafile(const char* file_name) {
	x_dim = y_dim = 0;

	file = fileopen(file_name,"r");
	if(get_header_map()==EXIT_FAILURE) {
		cerr << "Error in reading header" << endl;
		exit(EXIT_FAILURE);
	}

}

Dadafile::~Dadafile() {
	// TODO Auto-generated destructor stub
}

int Dadafile::seek_to(long samples){

	long bytes =(long) (this->y_dim * samples);
	return fseek(file,bytes,SEEK_CUR);
}




char* Dadafile::read_header()
{
	unsigned header_size = 16384;
	char* header = 0;
	do {
		rewind (file);
		header= (char *) realloc(header, header_size);
		if (fread(header, 1, header_size, file) != header_size) {
			fprintf(stderr, "dada_edit: could not read %u bytes: %s\n",header_size, strerror(errno));
			return NULL;
		}

	} while (header_size > header_size);
	return header;
}

int Dadafile::get_header_map(){
	char* header = read_header();
	if (header == NULL) {
		fprintf(stderr, "Header returned null at get headermap");
		return EXIT_FAILURE;
	}
	std::string keys[32] = { "HDR_VERSION", "HDR_SIZE", "BW", "FREQ", "NANT",
			"NCHAN", "NDIM", "NPOL", "NBIT", "TSAMP", "OSAMP_RATIO", "DSB",
			"RESOLUTION", "SOURCE", "RA", "DEC", "PROC_FILE", "MODE",
			"OBSERVER", "PID", "TOBS", "OBS_OFFSET", "TELESCOPE", "INSTRUMENT",
			"FILE_SIZE", "PFB_ID", "BYTES_PER_SECOND", "CHAN_OFFSET", "ORDER",
			"UTC_START", "FILE_NUMBER", "ANT_ID" };
	int keycount=32;
	for (int i = 0; i < keycount; i++) {
		char* key = (char*) keys[i].c_str();
		char val[1024];
		if (ascii_header_get(header, key, "%s", val) == 1) {

			//printf("%s %s\n", key, val);
			header_map.insert(std::pair<std::string, std::string>(key, val));
		} else {
			printf("%s %s\n", "invalid key:", key);
		}
	}
	free(header);
	return EXIT_SUCCESS;
}

int Dadafile::read_and_check(char* bval,long count)
{
	int b1;
	if ((b1 = fread(bval, 1,count, file))!= count) {
		fprintf(stderr,"b1 is only: %d bytes",b1);
		if(b1)
		{
			fprintf(stderr,"Warning: last part of file adding previous dada to make it equal to ngulp, b1:%d",b1);
			int numreverse = -count;
			fseek(file,numreverse,SEEK_CUR);
			if ((b1 = fread(bval, 1, count, file))!= count) {
				fprintf(stderr,"Sorry some stupid error in my code.: b1: %d",b1);
			}
			else
			{
				fprintf(stderr,"End of file reached on FILE 1");
			}
		}
		else
		{
			fprintf(stderr,"End of file reached on FILE 1");
		}
	}
	return b1;

}

int Dadafile::readgulp()
{
	char* bval1;
	bval1 =new_and_check<char>(x_dim*y_dim,"read gulp bval1");

	int b1 = read_and_check(bval1,x_dim*y_dim);
	if(b1) {
		for(int time=0;time<x_dim;time++)
		{
			for(int y=0;y<y_dim;y=y+2)
			{
				float _Complex temp = (bval1[y_dim*time+y]+0.5)+I* (bval1[y_dim*time+y+1]+0.5);
				gulp_complex[(int)(y/2)][time] = temp;
			}
		}
	}
	else {
		delete[] bval1;
		cerr << " here " << endl;
		return EXIT_FAILURE;
	}
	delete[] bval1;
	return(EXIT_SUCCESS);

}



int Dadafile::deallocate_memory()
{
	for (int y = 0; y < y_dim; y=y+2) delete[] gulp_complex[y/2];
	delete[] gulp_complex;
}
int  Dadafile::allocate_memory()
{
	try {
		gulp_complex = new float _Complex*[y_dim/2];
		for (int y = 0; y < y_dim; y=y+2) 	gulp_complex[y/2] = new float _Complex [x_dim];

		return EXIT_SUCCESS;
	}
	catch (std::bad_alloc& ba) {
		fprintf(stderr,"bad_alloc exception at allocating memory for gulp.");
		return EXIT_FAILURE;
	}
}

int Dadafile::check_similarity(Dadafile* df1, Dadafile* df2){

	if(::atoi(df2->header_map["NANT"].c_str())!=1 || ::atoi(df1->header_map["NANT"].c_str())!=1) {
		cerr << "Application can show you only single antenna DADA files." << endl;
		return EXIT_FAILURE;
	}

	if(atof(df2->header_map["TSAMP"].c_str())!=atof(df1->header_map["TSAMP"].c_str())){
		cerr << "Fatal error: DADA files hace  different sampling time " << endl;
		return EXIT_FAILURE;
	}
	int DIM2 = ::atoi(df2->header_map["NDIM"].c_str());
	int DIM1 = ::atoi(df1->header_map["NDIM"].c_str());

	if(DIM1 != 2 || DIM2 !=2 ) {
		cerr << "Fatal error: DADA files does not have the same NDIM = 2." << endl;
		return EXIT_FAILURE;
	}


	return EXIT_SUCCESS;

}








char* Dadafile::ascii_header_find (const char* header, const char* keyword)
{

	char* key = (char*)strstr (header, keyword);

	// keyword might be the very first word in header
	while (key > header)
	{
		//  fprintf (stderr, "found=%s", key);

		// if preceded by a new line, return the found key
		if ((*(key-1) == '\n') || (*(key-1) == '\\'))
			break;
		// otherwise, search again, starting one byte later
		key = strstr (key+1, keyword);
	}
	//printf("%s %s\n","key",key );
	return key;
}

int Dadafile::ascii_header_get (const char* header, const char* keyword,
		const char* format, ...)
{
	static const char* whitespace = " \t\n";

	va_list arguments;

	char* value = 0;
	int ret = 0;

	/* find the keyword */
	char* key = ascii_header_find (header, keyword);

	if (!key)
		return -1;

	/* find the value after the keyword */
	value = key + strcspn (key, whitespace);

	/* parse the value */
	va_start (arguments, format);
	ret = vsscanf (value, format, arguments);
	va_end (arguments);

	return ret;
}

