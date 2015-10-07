#include <iostream>

#include "neuron.h"

void Neuron::won() {
	/*
		notify neuron it has won
	*/
	update_w();
};

unsigned long Neuron::get_id() {
	return id;
};

float Neuron::eval(float** x) {
	y = 0;
    float error = 0;
    for(unsigned long m=0; m<M; ++m)
    	for(unsigned long n=0; n<N; ++n) {
            error = (x[m][n] - w[m][n]);
	    	y += error*error;
        }
    y = y / (M*N);
	last_x = x;
	return y;  // squared error
};

void Neuron::update_w() {
	float error = 0;
    for(unsigned long m=0; m<M; ++m)
    	for(unsigned long n=0; n<N; ++n) {
	    	error = last_x[m][n] - w[m][n];
		    w[m][n] = w[m][n] + (c * error);
    	}
};

Neuron::Neuron(float** x, unsigned long t_M, unsigned long t_N, float t_c, unsigned long t_id) {
	M = t_M;
    N = t_N;
	w = new float*[M];
    for(unsigned long m=0; m<M; ++m) {
        w[m] = new float[N];
    	for(unsigned long n=0; n<N; ++n)
	    	w[m][n] = x[m][n];
    };

	id = t_id;
	c = t_c;
	y = 0;
};	// Neuron::Neuron()

Neuron::~Neuron() {
    for(unsigned long m=0; m<M; ++m)
    	delete[] w[m];
    delete[] w;
};	// Neuron::~Neuron()
