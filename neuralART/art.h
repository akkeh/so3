#ifndef __ART_H_
#define __ART_H_

#include <ctime>

#include "neuron.h"

#define VECSIZE 255

class ART {
public:
	ART(unsigned long t_M, unsigned long t_N, float t_vigilance);
	~ART();
	long eval(float** x, unsigned long id);
	unsigned long get_neuronCount();		
	unsigned long get_id_of(unsigned long i);
	
private:
	unsigned long M;
	unsigned long N;
	float vigilance;

	Neuron** field2;
	unsigned long neuronCount;
	float c;

	void add_Neuron(float** x, unsigned long id);
		
	void save_State();
		
};

#endif
