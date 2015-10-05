#ifndef __NEURON_H_
#define __NEURON_H_

class Neuron;

class Neuron {
public:
	float eval(float** x);
	void won();
	unsigned long get_id();
	Neuron(float** x, unsigned long M, unsigned long N, float t_c, unsigned long t_id);
	~Neuron();
private:
	float** w;	// weighting
	unsigned long N;
	unsigned long M;
	unsigned long id;


	float c;	// weigthing change factor	
	float y;
	
	
	float** last_x;
	void update_w();
};

#endif
