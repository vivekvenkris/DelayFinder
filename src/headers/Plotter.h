/*
 * Plotter.h
 *
 *  Created on: 17 Jun 2016
 *      Author: vkrishnan
 */

#ifndef PLOTTER_H_
#define PLOTTER_H_
#include <map>
#include <string>
#include  <fftw3.h>
#include <complex.h>
#include "cpgplot.h"
#include "utilities.h"
#include "PGparam.h"
#include "Axisvalues.h"
#include<unistd.h>
#include <sstream>

class Plotter {
public:
	std::map<std::string,int>* pgdev_map;
	PGparam* param;
	const char* device;
	Plotter();
	virtual ~Plotter();
	void plot_y_vs_t(cmplx* inp, int n, int wtp,int bscrunch, std::string desc, float start, float TSAMP);
	void plot_float_array_vs_t(float* inp, int n,int bscrunch, std::string desc, float start, float TSAMP);
	void plot_y_vs_t_pt(cmplx* inp, int n, int wtp,int bscrunch, std::string desc, float start, float TSAMP, int symbol);
	void plot_UC(cmplx* inp, int n, std::string desc, int symbol);
	void plot_histogram(cmplx* inp, int n, std::string desc, int symbol);
	void check_and_open_plot(std::string desc, bool clear);

};

#endif /* PLOTTER_H_ */


void draw_scatter(PGparam *param, float* x,float* y,int N, int bscrunch, int symbol, bool margin);
void draw_line(PGparam *param, float* x,float* y,int N, int bscrunch, bool margin);
void draw_histogram(PGparam *param, float* x,float* y,int N, bool margin);
