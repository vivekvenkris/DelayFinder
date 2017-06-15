/*
 * utilityfns.h
 *
 *  Created on: 8 Jul 2014
 *      Author: vivek
 */

#ifndef UTILITYFNS_H_
#define UTILITYFNS_H_



#include <math.h>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <stdio.h>
#include <fftw3.h>
#include <vector>

#define POWER 0
#define REALPART  1
#define IMAGPART  2
#define PHASE 3
#define PHASEDEG 4
#define PHASERAD 5
#define PASS 0
#define FAIL -1

typedef float _Complex cmplx;
#define cmplx_i I ;


void log_message(std::string message, int level);


inline int pow2_round (int x)
{
    if (x < 0)
        return 0;
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x+1;
}

class PulseCompare
{
public:
	PulseCompare(int minwidth_):minwidth(minwidth_){};
	bool operator()(const int x, const int y) const {
		return minwidth < (y - x);
	}
private:
	int minwidth;
};

template<typename T> inline T sum_data(T* data,int  n){
	T sum = 0.0;
	for(int i=0;i<n;i++)sum+=data[i];
	return sum;
}

template<typename T> inline T* malloc_and_check(int N, std::string error_position)
{
	T* data = (T*)malloc(N*sizeof(T));
	if(data==NULL)
	{
		fprintf(stderr,"Error mallocing %d*%ld bytes at: %s ",N,sizeof(T),error_position.c_str());
	}
	return data;
}

template<typename T> inline T* new_and_check(int N, std::string error_position)
{
	try {
	T* data = new T[N];
	return data;
	}
	catch (std::bad_alloc& ba) {
			fprintf(stderr,"bad_alloc exception on allocating %d*%ld memory at this place: %s  ",N,sizeof(T),error_position.c_str());
			exit(EXIT_FAILURE);
		}
}


template<typename T> inline T* new_and_check_2D(int dim1,int dim2, std::string error_position)
{
	try {
	T** data = new T*[dim1];
	for(int x=0;x<dim1;x++)
		data[x] = new T[dim2];
	return data;
	}
	catch (std::bad_alloc& ba) {
			fprintf(stderr,"bad_alloc exception at allocating memory at this place: %s ",error_position.c_str());
			exit(EXIT_FAILURE);
		}
}

template<typename T> inline T** new_and_check_1Dof2D(int dim1,std::string errorposition)
{
	try {
	T** data = new T*[dim1];
	return data;
	}
	catch (std::bad_alloc& ba) {
			fprintf(stderr, "bad_alloc exception at allocating memory at this place: %s ",errorposition.c_str());
			exit(EXIT_FAILURE);
		}
}
template<typename K,typename V> const K& key(const std::pair<K, V>& keyValue)
{
    return keyValue.first;
}
template<typename K,typename V> const V& value(const std::pair<K, V>& keyValue)
{
    return keyValue.second;
}

template<typename K, typename V,typename C> inline int split_map_into_vectors(std::map<K,V,C>* map,std::vector<K>* keys,std::vector<V>* values)
{
std::transform(map->begin(), map->end(), keys->begin(), key<K,V>);
std::transform(map->begin(), map->end(), values->begin(), value<K,V>);
return EXIT_SUCCESS;
}
template<typename K, typename V> inline int split_map_into_vectors(std::map<K,V>* map,std::vector<K>* keys,std::vector<V>* values)
{
std::transform(map->begin(), map->end(), keys->begin(), key<K,V>);
std::transform(map->begin(), map->end(), values->begin(), value<K,V>);
return EXIT_SUCCESS;
}


template<typename K, typename V> inline void clone_and_cast(K* src, V* dest, int n){
	for(int i=0;i<n;i++) dest[i] =(V)src[i];
}


template<typename T> inline T get_scrunched_value(T* data,int bscrunch) {
	float out=0.0;
	for(int bs=0;bs<bscrunch;bs++)
		out+= data[bs];
	return out/bscrunch;
}

template<typename T> inline void scrunch(T* data,T* out, int n, int b){
	int j=0;
	for(int i=0;i<n;i+=b) {
		out[j++]= get_scrunched_value<T>(&data[i],b);
	}
}

float extract_complex_data_info( float _Complex d,int type);
void extract_complex_data_info_arr(float _Complex* x,float* y, int n, int type);
FILE* fileopen(const char* absolutename, const char* mode);
float get_max(float* data, int n);
float get_min(float* data, int n);
int get_max_bin(float* data, int n);
void remove_baseline(float* data, int n);
void normalise(float* data,int n);
void normalise(float* data, int n, float rms);
void clone(float* src, float* dest, int n);
#endif /* UTILITYFNS_H_ */
