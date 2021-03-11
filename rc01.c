/*******************************************
 * File : rc01.c
 * Author : F. Emre Yazici
 * Creation Date : 09.03.2021
 * 
 * this file implements Fig. 1.a 
 * z feedback into the reservoir state x aroung line 101 in the mod03
 * this file is based on rc01.m by Alexandre Schmid, which is based on force.m; 
 * 
 * Original file header: 
		 FORCE.m

		 This function generates the sum of 4 sine waves in figure 2D using the architecture of figure 1A with the RLS
		 learning rule.

		 written by David Sussillo
 ********************************************/

#include "fpmatrix.h"
#include <math.h>


int main() {

	// loop variables
	unsigned i, j, k;

	fp_matrix M, wo, dw, wf, wi;

	unsigned N, nsecs, learn_every;
	double p, g, alpha, dt;

	N = 1000;
	p = 0.1;
	g = 1.5;	// g greater than 1 leads to chaotic networks.
	alpha = 1.0;
	nsecs = 1440;
	dt = 0.1;
	learn_every = 2;

	double scale;
	scale = 1.0 / sqrt(p * N);

	// 	M = sprandn(N, N, p) * g * scale; % sparse random connectivity
	fpm_init(&M, N, N);
	fpm_fillsprandn(&M, 0, 1, p);
	fpm_scale(&M, float_to_fixed(g * scale));

	unsigned nRec2Out;
	nRec2Out = N;
	fpm_init(&wo, nRec2Out, 1);
	fpm_init(&dw, nRec2Out, 1);

	fpm_init(&wf, nRec2Out, 1);
	fpm_fillrand(&wf, 0, float_to_fixed(1));
	fpm_iadd_scalar(&wf, float_to_fixed(-0.5));
	fpm_scale(&wf, float_to_fixed(2));

	unsigned nC2In;
	nC2In = N; // fully connected input
	fpm_init(&wi, nC2In, 1);
	fpm_fillrand(&wi, 0, float_to_fixed(1));
	fpm_iadd_scalar(&wi, float_to_fixed(-0.5));
	fpm_scale(&wi, float_to_fixed(2));

	printf("\tN: %d\n", N);
	printf("\tg: %f\n", g);
	printf("\tp: %f\n", p);
	printf("\tnRec2Out: %d\n", nRec2Out);
	printf("\talpha: %f\n", alpha);
	printf("\tnsecs: %d\n", nsecs);
	printf("\tlearn_every: %d\n", learn_every);

	double* simtime;
	size_t simtime_len;
	simtime_len = (unsigned) (nsecs *  (1/dt));
	simtime = malloc(simtime_len * sizeof(double));
	printf("%d\n", simtime_len);
	for (i = 0; i < simtime_len; i++) {
		simtime[i] = i * dt;
	}
	
	double* simtime2;
	simtime2 = malloc(simtime_len * sizeof(double));
	for (i = 0; i < simtime_len; i++) {
		simtime2[i] = nsecs + i * dt;
	}



	free(simtime);
	free(simtime2);
}