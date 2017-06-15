/*
 * Plotter.cpp
 *
 *  Created on: 17 Jun 2016
 *      Author: vkrishnan
 */

#include "headers/Plotter.h"

Plotter::Plotter() {
	pgdev_map = new std::map<std::string,int>();
	device = "/xs";
}

Plotter::~Plotter() {
	// TODO Auto-generated destructor stub
}

void Plotter::check_and_open_plot(std::string desc,bool clear){
	int plot;

	if(pgdev_map->find(desc)==pgdev_map->end()){
		plot = cpgopen(device);
		cpgask(param->pgask);
		pgdev_map->insert(std::pair<std::string,int>(desc,plot));
	}
	else {
		plot = pgdev_map->find(desc)->second;
	}
	cpgslct(plot);
	if(clear)cpgeras();

}

void Plotter::plot_y_vs_t(cmplx* inp, int n, int wtp,int bscrunch, std::string desc, float start, float TSAMP){

	check_and_open_plot(desc,true);

	float* data = new_and_check<float>(n,"plot Y vs T");
	extract_complex_data_info_arr(inp,data,n,wtp);

	float* x= Axisvalues::getxaxis(n);
	float* t = new_and_check<float>(n, "plot Y vs T");
	clone(x, t, n);
	if(TSAMP!=1) for(int i=0;i<n;i++) t[i] = t[i]*(TSAMP*bscrunch) + start;

	draw_line(param,x,data,n,bscrunch,true);

	delete[] data;
	delete[] t;

}

void Plotter::plot_y_vs_t_pt(cmplx* inp, int n, int wtp,int bscrunch, std::string desc, float start, float TSAMP, int symbol){

	check_and_open_plot(desc,true);

	float* data = new_and_check<float>(n,"plot Y vs T");
	extract_complex_data_info_arr(inp,data,n,wtp);

	float* x= Axisvalues::getxaxis(n);
	float* t = new_and_check<float>(n, "plot Y vs T");
	clone(x, t, n);
	if(TSAMP!=1) for(int i=0;i<n;i++) t[i] = t[i]*(TSAMP*bscrunch) + start;

	draw_scatter(param,x,data,n,bscrunch,symbol,true);

	delete[] data;
	delete[] t;

}

void Plotter::plot_UC(cmplx* inp, int n, std::string desc, int symbol){
	check_and_open_plot(desc,false);

	float* real = new_and_check<float>(n,"Plot UC");
	float* imag = new_and_check<float>(n,"Plot UC");

	extract_complex_data_info_arr(inp,real,n,REALPART);
	extract_complex_data_info_arr(inp,imag,n,IMAGPART);

	remove_baseline(real,n);
	normalise(real,n);
	remove_baseline(imag,n);
	normalise(imag,n);

	fprintf(stderr,"\n \n  ********** extremes %f %f %f %f ******* \n \n ", get_min(real,n),get_max(imag,n),get_min(imag,n),get_max(real,n));


	draw_scatter(param,real,imag,n,1,symbol,false);

	delete[] real;
	delete[] imag;
}

void Plotter::plot_histogram(cmplx* inp, int n, std::string desc, int symbol){
	int nbin=32;
	check_and_open_plot(desc,true);
	float* num = new_and_check<float>(n,"plot_histogram");
	float* phase= new_and_check<float>(n,"plot_histogram");
	extract_complex_data_info_arr(inp,phase,n,PHASE);
	int min= -180;
	int max = 180;
	int step = (max-min)/nbin;
	for(int i=0;i<nbin;i++){
		num[i] = 0;
		int below = min + step*(i-1);
		int above = min + step*i;
		for(int j=0;j<n;j++){
			if(phase[j] > below && phase[j]<=above) num[i]++;
		}
	}
	//draw_histogram(param, x,y, n,false);
	float* x = Axisvalues::getxaxis(nbin);
	draw_line(param, x, num, nbin, 1, false);

}


void Plotter::plot_float_array_vs_t(float* data, int n,int bscrunch, std::string desc, float start, float TSAMP){

	check_and_open_plot(desc,true);

	float* x= Axisvalues::getxaxis(n);
	float* t = new_and_check<float>(n, "plot Y vs T");
	clone(x, t, n);
	if(start!=0 && TSAMP!=1) for(int i=0;i<n;i++) t[i] = t[i]*TSAMP + start;

	draw_line(param,x,data,n,bscrunch,true);

	delete[] t;
}



void clearplots(){
	std::ostringstream oss;
	std::string device = "/xs";
	for(int i=8;i>=1;i--){
		oss<<i<<device;
		int temp = cpgopen(oss.str().c_str());
		cpgslct(temp);
		cpgeras();
		printf("%s cleared \n",oss.str().c_str());
		oss.clear();
		oss.str("");
		cpgclos();
	}
}

void draw_histogram(PGparam *param, float* x,float* y,int N, bool margin){


	float min_x, max_x, min_y, max_y;

	/* get boundaries */
	min_x = get_min(x,N);
	min_y = get_min(y,N);
	max_x = get_max(x,N);
	max_y = get_max(y,N);

	if(margin){
		min_x = min_x - (max_x - min_x) / N;
		min_y = min_y - (max_y - min_y) / N;
		max_x = max_x + (max_x - min_x) / N;
		max_y = max_y + (max_y - min_y) / N;
	}
	fprintf(stderr," values: %f %f %f %f",min_x,max_x,min_y,max_y);
	param->set_params();
	cpgswin(min_y,max_y,min_x, max_x);
	cpgbox("BCNST",0.0,0,"BCNST",0.0,0);
	cpgbin(N, y, x, true);

}


void draw_line(PGparam *param, float* x,float* y,int N, int bscrunch, bool margin){

	int size = N/bscrunch;
	float *x_temp, *y_temp;
	bool alloced = false;

	/* scrunch as needed*/
	if(bscrunch > 1) {
		x_temp = new_and_check<float>(N/bscrunch,"draw_scatter");
		y_temp = new_and_check<float>(N/bscrunch,"draw_scatter");
		alloced= true;
		scrunch<float>(x,x_temp,N,bscrunch);
		scrunch<float>(y,y_temp,N,bscrunch);

	}
	else {
		x_temp = x;
		y_temp = y;
	}

		remove_baseline(x_temp,size);
		normalise(x_temp,size);

	remove_baseline(y_temp,size);
	normalise(y_temp,size);

	//for(int i=0;i<size;i++) fprintf(stderr,"%f \n",x_temp[i]);

	float min_x, max_x, min_y, max_y;

	/* get boundaries */
	min_x = get_min(x_temp,size);
	min_y = get_min(y_temp,size);
	max_x = get_max(x_temp,size);
	max_y = get_max(y_temp,size);



	/* some margin*/
	if(margin){
		min_x = min_x - (max_x - min_x) / size;
		min_y = min_y - (max_y - min_y) / size;
		max_x = max_x + (max_x - min_x) / size;
		max_y = max_y + (max_y - min_y) / size;
	}

	fprintf(stderr," values: %f %f %f %f",min_x,max_x,min_y,max_y);
	param->set_params();
	cpgswin(min_x, max_x,min_y,max_y);
	cpgbox("BCNST",0.0,0,"BCNST",0.0,0);
	cpgline(size,x_temp,y_temp);

	if(alloced){
		delete[] x_temp;
		delete[] y_temp;
	}


}


void draw_scatter(PGparam *param, float* x,float* y,int N, int bscrunch, int symbol, bool margin){

	int size = N/bscrunch;
	float *x_temp, *y_temp;
	bool alloced = false;

	/* scrunch as needed*/
	if(bscrunch > 1) {
		x_temp = new_and_check<float>(N/bscrunch,"draw_scatter");
		y_temp = new_and_check<float>(N/bscrunch,"draw_scatter");
		alloced= true;
		scrunch<float>(x,x_temp,N,bscrunch);
		scrunch<float>(y,y_temp,N,bscrunch);

	}
	else {
		x_temp = x;
		y_temp = y;
	}

		//remove_baseline(x_temp,size);
		//normalise(x_temp,size);

	//remove_baseline(y_temp,size);
	//normalise(y_temp,size);

	//for(int i=0;i<size;i++) fprintf(stderr,"%f \n",x_temp[i]);

	float min_x, max_x, min_y, max_y;

	/* get boundaries */
	min_x = get_min(x_temp,size);
	min_y = get_min(y_temp,size);
	max_x = get_max(x_temp,size);
	max_y = get_max(y_temp,size);



	/* some margin*/
	if(margin){
		min_x = min_x - (max_x - min_x) / size;
		min_y = min_y - (max_y - min_y) / size;
		max_x = max_x + (max_x - min_x) / size;
		max_y = max_y + (max_y - min_y) / size;
	}

	fprintf(stderr," values: %f %f %f %f",min_x,max_x,min_y,max_y);
	param->set_params();
	if(param->cpgswin) {
	cpgswin(min_x, max_x,min_y,max_y);
	cpgbox("BCNST",0.0,0,"BCNST",0.0,0);
	}
	cpgpt(size,x_temp,y_temp, symbol);

	if(alloced){
		delete[] x_temp;
		delete[] y_temp;
	}


}


