/*
 * utilityfns.C
 *
 *  Created on: 8 Jul 2014
 *      Author: vivek
 */
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <errno.h>
#include <complex.h>
#include <fftw3.h>
#include "headers/utilities.h"


void log_message(std::string message, int level){
	fprintf(stderr,message.c_str());
}


FILE* fileopen(const char* absolutename, const char* mode) {
	FILE* file = fopen(absolutename, mode);
	if (!file) {
		fprintf(stderr, "could not open '%s': %s\n", absolutename,
				strerror(errno));
		exit(1);
	}
	return file;
}
void clone(float* src, float* dest, int n){
	for(int i=0;i<n;i++) dest[i] = src[i];
}

float extract_complex_data_info( cmplx d,int type)
{
	//whattoplot = PLOT_REAL;
	float out = 0.0;
	float real = creal(d);
	float imag = cimag(d);
	switch(type)
	{
		case POWER:
							out = (float)sqrt(real*real + imag*imag);
							break;
		case REALPART:
							out = (float) real;
							break;
		case IMAGPART:
							out = (float) imag;
							break;
		case PHASE:
							out = (float) (atan2f(imag,real)*180/M_PI);
							break;
		case PHASEDEG:
							out = (float) (atan2f(imag,real)*180/M_PI);
							break;
		case PHASERAD:
							out = (float) (atan2f(imag,real));
							break;
		default:
							printf("%s\n","cannot recognize what you want to get." );
							break;
	}
	return out;
}

void extract_complex_data_info_arr(cmplx* x,float* y, int n, int type){
	for(int i=0;i<n;i++){
		y[i]=extract_complex_data_info(x[i],type);
	}
}


float get_min(float* data, int n){
	float min = 0.0;
	for(int i=0;i<n;i++) if(min > data[i] || i==0) min = data[i];
	return min;
}

float get_max(float* data, int n){
	float max = 0.0;
	for(int i=0;i<n;i++) if(max < data[i] || i==0) max = data[i];
	return max;
}

int get_max_bin(float* data, int n){
	float max = 0.0;
	int bin = 0;
	for(int i=0;i<n;i++) if(max < data[i] || i==0) { max = data[i]; bin = i; }
	return bin;
}

float calc_baseline(float* data,int n){
	float sum=0.0;
	for(int i=0;i<n;i++) sum+=data[i];
	return sum/(float)n;
}

void remove_baseline(float* data, int n) {
	float mean=calc_baseline(data,n);
	for(int i=0;i<n;i++) data[i]-=mean;
}

void remove_baseline(float*data, int n, float baseline){
	for(int i=0;i<n;i++) data[i]-=baseline;
}

float calc_rms(float* data,int n){
	float sumsq = 0.0;
	float sum =0.0;
	float mean,meansq;
	for(int i=0;i<n;i++)
	{
		sumsq+=(data[i]*data[i]);
		sum+=data[i];
	}
	mean   = sum/(double)n;
	meansq = sumsq/(double)n;
	return sqrt(meansq-mean*mean);
}

void normalise(float* data,int n)
{
	float rms = calc_rms(data,n);
	if(rms==0)return;
	for(int i=0;i<n;i++) data[i]/=rms;
}

void normalise(float* data, int n, float rms){
	for(int i=0;i<n;i++) data[i]/=rms;
}

