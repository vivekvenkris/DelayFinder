//============================================================================
// Name        : DelayFinder.cpp
// Author      : Vivek Venkatraman Krishnan
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
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
#include <sstream>
#include <iostream>
#include <math.h>
#include <fftw3.h>

#include "headers/Args.h"
#include "headers/utilities.h"
#include "headers/Dadafile.h"
#include "headers/PGparam.h"
#include "headers/Plotter.h"


using namespace std;

int get_integer_delay(cmplx* lag_cmplx , int n);
void get_best_snr_and_bin(cmplx* lag_cmplx , int n , float* snr, int* bin);
void fft(cmplx* input,cmplx* output,int nptsfft,int scrunch,int direction);
void apply_delay(cmplx* in, cmplx* out,int n, float delay);

int main(int argc, char ** argv) {

	Args args;
	args.parse(argc, argv);

	Plotter* plotter = new Plotter();
	PGparam* param = new PGparam();


	param->pgask = true;
	param->pgsch = 1;
	param->pgsci = 3;
	param->pgslw = 3;

	plotter->param = param;
	plotter->device = "/xs";

	Dadafile* df1 =  new Dadafile(args.file1.c_str());
	Dadafile* df2 =  new Dadafile(args.file2.c_str());

	if ( Dadafile::check_similarity(df1,df2) == EXIT_FAILURE ) exit(EXIT_FAILURE);

	if(args.verbose) cerr << "Headers match.  starting operation. " << endl;

	string module_id_1 = df1->header_map["PFB_ID"].c_str();
	string module_id_2 = df2->header_map["PFB_ID"].c_str();
	float tsamp = ::atof(df1->header_map["TSAMP"].c_str());

	if(args.verbose) cerr << "IDs:  " << module_id_1  << " " << module_id_2 << endl;


	df1->start_sample = df2->start_sample = args.start_sample;
	df1->x_dim = df2->x_dim = args.num_samples;


	df1->y_dim = ::atoi(df1->header_map["NCHAN"].c_str()) * ::atoi(df1->header_map["NDIM"].c_str());
	df2->y_dim = ::atoi(df2->header_map["NCHAN"].c_str()) * ::atoi(df2->header_map["NDIM"].c_str());

	if(args.start_channel < 0 ) args.start_channel = 0;
	if(args.end_channel < 0   ) args.end_channel   = ::min ( ::atoi(df1->header_map["NCHAN"].c_str()) -1, ::atoi(df2->header_map["NCHAN"].c_str()) -1 );

	if(args.verbose) cerr << "Seeking to start sample: " << args.start_sample  <<  endl;

	if( df1->seek_to(args.start_sample) != EXIT_SUCCESS ) exit(EXIT_FAILURE);
	if( df2->seek_to(args.start_sample) != EXIT_SUCCESS ) exit(EXIT_FAILURE);

	if(args.verbose) cerr << "Seek success. Allocating memory."  <<  endl;

	if( df1->allocate_memory() != EXIT_SUCCESS ) exit(EXIT_FAILURE);
	if( df2->allocate_memory() != EXIT_SUCCESS ) exit(EXIT_FAILURE);

	if(args.verbose) cerr << "Memory allocated. Reading " <<  args.num_samples << " samples"  <<  endl;


	if( df1->readgulp() != EXIT_SUCCESS ) exit(EXIT_FAILURE);
	if( df2->readgulp() != EXIT_SUCCESS ) exit(EXIT_FAILURE);

	int scrunch_factor =  args.num_samples / args.nfft;

	int num_selected_channels = args.end_channel - args.start_channel;

	int total_spectrum_size = num_selected_channels * args.nfft;
	int scale = 1/args.nfft;

	if(args.verbose) cerr << "Read samples. starting FFT" <<  endl;

	cmplx* spectrum_1 =  new_and_check<cmplx>( total_spectrum_size , " Spectrum 1" );
	cmplx* spectrum_2 =  new_and_check<cmplx>( total_spectrum_size , " Spectrum 2 " );

	float freq =  ::atof(df2->header_map["FREQ"].c_str());
	float bw   =  ::atof(df2->header_map["BW"].c_str());



	float channel_bw = bw/ ::atoi(df2->header_map["NCHAN"].c_str());


	for(int chan_num = args.start_channel, chan_idx = 0; chan_num <= args.end_channel; chan_num++,chan_idx++){

		fft(&df1->gulp_complex[chan_num][0], &spectrum_1[chan_idx*args.nfft], args.nfft, scrunch_factor, FFTW_FORWARD);
		fft(&df2->gulp_complex[chan_num][0], &spectrum_2[chan_idx*args.nfft], args.nfft, scrunch_factor, FFTW_FORWARD);

//		if(args.artificial_delay >0 ) {
//		for(int i=0;i< args.nfft; i++)	spectrum_2[chan_idx*args.nfft+ i] = spectrum_2[chan_idx*args.nfft +i] * exp(args.artificial_delay * 2* M_PI * );
//		}

	}



	plotter->plot_y_vs_t_pt(spectrum_1, total_spectrum_size, PHASE, 1, "spectrum 1", 0, 1.28e-6, 17);
	plotter->plot_y_vs_t_pt(spectrum_2, total_spectrum_size, PHASE, 1, "spectrum 2", 0, 1.28e-6, 17);



	if(args.verbose) cerr << "Got stitched spectrum " <<  endl;

	cmplx* cross_power_spectrum = new_and_check<cmplx>( total_spectrum_size ," CPS ");

	for (int i = 0; i < total_spectrum_size; i++) {
		cross_power_spectrum[i] = spectrum_1[i]* conj(spectrum_2[i]) * scale;
		cerr << creal(cross_power_spectrum[i]) << " " << creal(spectrum_1[i]) << " " << creal(spectrum_2[i]) << " " << cimag(cross_power_spectrum[i]) << " " << cimag(spectrum_1[i]) << " " << cimag(spectrum_2[i]) <<  endl;
	}

	plotter->plot_y_vs_t_pt(cross_power_spectrum, total_spectrum_size, PHASE, 1, "cross spectrum", 0, 1.28e-6, 17);

	if(args.verbose) cerr << "Got cross power spectrum " <<  endl;


	cmplx* reuseable_cross_power_spectrum = new_and_check<cmplx>( total_spectrum_size ," CPS ");
	cmplx* reuseable_lag_spectrum = new_and_check<cmplx>( total_spectrum_size ," Lag spectrum ");


	fft(cross_power_spectrum, reuseable_lag_spectrum, total_spectrum_size, 1, FFTW_BACKWARD);

	int integer_delay = get_integer_delay(reuseable_lag_spectrum, total_spectrum_size);
	apply_delay(cross_power_spectrum,reuseable_cross_power_spectrum,total_spectrum_size,integer_delay);


	int num_trials = (int) 1/args.sub_sample_resolution;

	float snrs[num_trials];
	int   delay_bins[num_trials]; // should  always be -1, 0 or 1.

	for( int i=0; i< num_trials; i++){

		float fractional_delay =  integer_delay + args.sub_sample_resolution*i;
		apply_delay(cross_power_spectrum,reuseable_cross_power_spectrum,total_spectrum_size, fractional_delay);

		fft(cross_power_spectrum, reuseable_lag_spectrum, total_spectrum_size, 1, FFTW_BACKWARD);

		get_best_snr_and_bin(reuseable_lag_spectrum, total_spectrum_size, &snrs[i], &delay_bins[i]);


	}

	float best_snr = get_max( snrs, num_trials);
	int  best_trial = get_max_bin(snrs, num_trials);

	int best_delay = integer_delay + best_trial* args.sub_sample_resolution;

	cerr << "Best Delay: " << best_delay << endl;

	apply_delay(cross_power_spectrum,reuseable_cross_power_spectrum,total_spectrum_size, best_delay);
	fft(cross_power_spectrum, reuseable_lag_spectrum, total_spectrum_size, 1, FFTW_BACKWARD);


	delete[] cross_power_spectrum;
	delete[] reuseable_cross_power_spectrum;
	delete[] reuseable_lag_spectrum;
	delete[] spectrum_1;
	delete[] spectrum_2;

	delete df1;
	delete df2;
	return EXIT_SUCCESS;

}


int get_integer_delay(cmplx* lag_cmplx , int n){

	float* lag_power = new_and_check<float>(n,"lag power - integer delay");
	for( int i=0; i<n; i++)	lag_power[i] = extract_complex_data_info(lag_cmplx[i], POWER);

	remove_baseline(lag_power,n);
	normalise(lag_power,n);

	int integerdelay = get_max_bin(lag_power,n);

	delete[] lag_power;

	return integerdelay;
}

void get_best_snr_and_bin(cmplx* lag_cmplx , int n , float* snr, int* bin){

	float* lag_power = new_and_check<float>(n,"lag power - integer delay");
	for( int i=0; i<n; i++)	lag_power[i] = extract_complex_data_info(lag_cmplx[i], POWER);

	remove_baseline(lag_power,n);
	normalise(lag_power,n);

	*snr = get_max(lag_power, n);
	*bin = get_max_bin(lag_power,n);

	delete[] lag_power;
}


void fft(cmplx* input,cmplx* output,int nptsfft,int scrunch,int direction){


	cmplx* output_temp = new_and_check<cmplx>(nptsfft,"fft");

	int half_npts = (nptsfft > 1) ? (int)(nptsfft/2) : nptsfft;


	for(int scr=0; scr < scrunch; scr++){
		fftwf_plan pa = fftwf_plan_dft_1d(nptsfft, &input[scr*nptsfft], output_temp, direction, FFTW_ESTIMATE);

		fftwf_execute(pa);

		if(direction== FFTW_BACKWARD) for(int i=0; i < nptsfft; i++) (scr==0) ? output[i] = output_temp[i] : output[i]+=output_temp[i];
		else {
			for(int i=0; i < nptsfft; i++)
				(i < half_npts)
					? (scr==0)
						? output[i] =  output_temp[i + half_npts]
						: output[i] += output_temp[i + half_npts]
					: (scr==0)
						? output[i] =  output_temp[i - half_npts]
						: output[i] += output_temp[i - half_npts];
		}


	}

	delete[] output_temp;

}

void apply_delay(cmplx* in, cmplx* out,int n, float delay){

	for(int i=0;i<n;i++) out[(i+n/2)%n] =in[(i+n/2)%n]*(cos(-2*M_PI*delay*(i)/(float)n)+I*sin(-2*M_PI*delay*(i)/(float)n));

}
