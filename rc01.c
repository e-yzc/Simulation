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
#define _USE_MATH_DEFINES 
#include <math.h>


int main() {

	// loop variables
	unsigned i, j, k;

	fp_matrix M, wo, dw, wf, wi, ft, ft2, wo_len, zt, zpt, x0, z0, x, r, z;

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

	double amp, freq;
	amp = 1.3;
	freq = 1 / 60;

	fpm_init(&ft, simtime_len, 1);
	fpm_init(&ft2, simtime_len, 1);

	for (i = 0; i < simtime_len; i++)
		ft.data[i] = float_to_fixed( ((amp / 1.0) * sin(1.0 * M_PI * freq * simtime[i]) + 
		(amp / 2.0) * sin(2.0 * M_PI * freq * simtime[i]) + 
		(amp / 6.0) * sin(3.0 * M_PI * freq * simtime[i]) + 
		(amp / 3.0) * sin(4.0 * M_PI * freq * simtime[i])) / 1.5 );

	for (i = 0; i < simtime_len; i++)
		ft2.data[i] = float_to_fixed(((amp / 1.0) * sin(1.0 * M_PI * freq * simtime[i]) +
			(amp / 2.0) * sin(2.0 * M_PI * freq * simtime[i]) +
			(amp / 6.0) * sin(3.0 * M_PI * freq * simtime[i]) +
			(amp / 3.0) * sin(4.0 * M_PI * freq * simtime[i])) / 1.5);

	fpm_init(&wo_len, 1, simtime_len);
	fpm_init(&zt, 1, simtime_len);
	fpm_init(&zpt, 1, simtime_len);
	
	fpm_init(&x0, N, 1);
	fpm_fillrandn(&x0, 0, 1);
	fpm_scale(&x0, float_to_fixed(0.5));
	fpm_init(&z0, 1, 1);
	fpm_fillrandn(&z0, 0, 1);
	fpm_scale(&z0, float_to_fixed(0.5));

	x = fpm_copy(&x0);
	fpm_init(&r, x.rows, x.cols);
	for (i = 0; i < r.rows * r.cols; i++)
		r.data[i] = float_to_fixed(fixed_to_float(tanh(x.data[i])));
	z = fpm_copy(&z0);




	fpm_destroy(&M);
	fpm_destroy(&wo);
	fpm_destroy(&dw);
	fpm_destroy(&wf);
	fpm_destroy(&wi);
	free(simtime);
	free(simtime2);
}