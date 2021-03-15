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
#include <stdio.h>

int main() {

	// set the seed
	srand((unsigned)time(NULL));

	// Open file to store simulation results
#pragma warning(suppress : 4996).
	FILE* res_file = fopen("sim_results.csv", "w");
	if (res_file == NULL) return -1;

	// loop variables
	unsigned i, j, t;

	fp_matrix M, wo, dw, wf, wi, ft, ft2, wo_len, zt, zpt, z0, x0, x, r, P, k;
	fixed_point z, rPr, c, e;
	unsigned N, nsecs, learn_every;
	double p, g, alpha, dt;

	N = 100;
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
	freq = 1. / 60;

	fpm_init(&ft, simtime_len, 1);
	fpm_init(&ft2, simtime_len, 1);

	for (i = 0; i < simtime_len; i++) {
		ft.data[i] = float_to_fixed(((amp / 1.0) * sin(1.0 * M_PI * freq * simtime[i]) +
			(amp / 2.0) * sin(2.0 * M_PI * freq * simtime[i]) +
			(amp / 6.0) * sin(3.0 * M_PI * freq * simtime[i]) +
			(amp / 3.0) * sin(4.0 * M_PI * freq * simtime[i])) / 1.5);
	}

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
		r.data[i] = float_to_fixed(tanh(fixed_to_float(x.data[i])));
	z = z0.data[0];



	// initialize P matrix
	fpm_init(&P, nRec2Out, nRec2Out);
	for (i = 0; i < P.rows; i++)
		for (j = 0; j < P.cols; j++)
			P.data[i * P.cols + j] = float_to_fixed((i == j) ? 1. : 0.);
	fpm_scale(&P, float_to_fixed((1.0 / alpha)));
	
	// initialize k
	fpm_init(&k, nRec2Out, 1);

	// Output file header
	fprintf(res_file, "ft, zt, wo_len\n");


	for (t = 0; t < simtime_len; t++) {
		// temporary matrix variable
		fp_matrix tmp, tmp2;
		
		// % sim, so x(t) and r(t) are created.

		/*** x = (1.0-dt)*x + M*(r*dt) + wf*(z*dt) + wi*(ft(ti)*dt) ***/
		// scale x by 1-dt
		fpm_scale(&x, float_to_fixed(1.0 - dt));
		
		// create temporary matrix to hold intermediate values, add to x
		tmp = fpm_mult(&M, &r);
		fpm_scale(&tmp, float_to_fixed(dt));
		fpm_iadd(&x, &tmp);

		
		fpm_destroy(&tmp);
		tmp = fpm_copy(&wf);
		fpm_scale(&tmp, z);
		fpm_scale(&tmp, float_to_fixed(dt));
		fpm_iadd(&x, &tmp);

		fpm_destroy(&tmp);
		tmp = fpm_copy(&wi);
		fpm_scale(&tmp, ft.data[t]);
		fpm_scale(&tmp, float_to_fixed(dt));
		fpm_iadd(&x, &tmp);


		// update r
		for (i = 0; i < r.cols * r.rows; i++)
			r.data[i] = float_to_fixed(tanh(fixed_to_float(x.data[i])));

		// update z
		fpm_destroy(&tmp);
		tmp = fpm_transposed(&wo);
		fpm_imult(&tmp, &r);
		z = tmp.data[0];

		if (t % learn_every == 0) {
			// update inverse correlation matrix
			fpm_destroy(&k);
			k = fpm_mult(&P, &r);

			fpm_destroy(&tmp);
			tmp = fpm_transposed(&r);	// tmp = r'
			fpm_imult(&tmp, &k); // tmp = r'k, should be 1-by-1

			rPr = tmp.data[0];
			c = float_to_fixed(1.0 / (1.0 + fixed_to_float(rPr)));

			fpm_destroy(&tmp);
			tmp = fpm_copy(&k); // tmp = k
			tmp2 = fpm_transposed(&tmp); // tmp2 = k'
			fpm_imult(&tmp, &tmp2); // tmp = k*k'
			fpm_scale(&tmp, float_to_fixed(-1.)); // tmp = - k*k'
			fpm_scale(&tmp, c); // tmp = - k*k' * c
			fpm_iadd(&P, &tmp);	// P = P - k*k'*c

			fpm_destroy(&tmp2);
			// update the error for the linear readout
			e = fp_add(z, fp_mult(ft.data[t], float_to_fixed(-1.)));

			// update the output weights
			fpm_destroy(&tmp);
			tmp = fpm_copy(&k); // tmp = k
			fpm_scale(&tmp, c+1);	// tmp = k*c
			fpm_scale(&tmp, e); // tmp = e*k*c
			fpm_scale(&tmp, float_to_fixed(-1.)); // tmp = - e * k * c
			fpm_destroy(&dw);
			dw = fpm_copy(&tmp);	// dw = - e * k * c
			fpm_iadd(&wo, &dw);		// wo = wo + dw
		}

		// store the output
		zt.data[t] = z;
		tmp = fpm_transposed(&wo);	// tmp = wo'
		fpm_imult(&tmp, &wo);	// tmp = wo'*wo
		wo_len.data[t] = float_to_fixed(sqrt(fixed_to_float(tmp.data[0])));

		//printf("%f, %f, %f, %f\n", fixed_to_float(ft.data[t]), fixed_to_float(zt.data[t]), fixed_to_float(e),fixed_to_float(wo_len.data[t]));


		if (t % (simtime_len / 100) == 0)
			printf(".");
	}

	//fpm_print(&wo);

	// write results
	for (i = 0; i < simtime_len; i++) {
		fprintf(res_file, "%f, %f, %f\n", fixed_to_float(ft.data[i]), fixed_to_float(zt.data[i]), fixed_to_float(wo_len.data[i]));
	}

	// free resources
	fclose(res_file);

	fpm_destroy(&M);
	fpm_destroy(&wo);
	fpm_destroy(&dw);
	fpm_destroy(&wf);
	fpm_destroy(&wi);
	fpm_destroy(&ft);
	fpm_destroy(&ft2);
	fpm_destroy(&wo_len);
	fpm_destroy(&zt);
	fpm_destroy(&zpt);
	fpm_destroy(&x0);
	fpm_destroy(&z0);
	fpm_destroy(&x);
	fpm_destroy(&r);
	fpm_destroy(&z);
	fpm_destroy(&P);

	free(simtime);
	free(simtime2);
}