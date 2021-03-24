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
#include "dbmatrix.h"
#define _USE_MATH_DEFINES 
#include <math.h>
#include <stdio.h>

int main() {

	// set the seed
	srand((unsigned)time(NULL));

	// Open file to store simulation results
#pragma warning(suppress : 4996)
	FILE* res_file = fopen("sim_results.csv", "w");
	if (res_file == NULL) return -1;

	// loop variables
	unsigned i, j, t;
	unsigned progress = 0;

	fp_matrix M, wo, wf, wi, zt, zpt, z0, x0, x, r;
	fixed_point z;
	unsigned N, nsecs, learn_every;
	double p, g, alpha, dt;

	N = 300;
	p = 0.1;
	g = 1.5;	// g greater than 1 leads to chaotic networks.
	alpha = 1;
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
	amp = 6.5;
	freq = 1. / 60;

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


	// Initialize test functions
	db_matrix ft, ft2;

	dbm_init(&ft, simtime_len, 1);
	dbm_init(&ft2, simtime_len, 1);

	for (i = 0; i < simtime_len; i++) {
		ft.data[i] = ((amp / 1.0) * sin(1.0 * M_PI * freq * simtime[i]) +
			(amp / 2.0) * sin(2.0 * M_PI * freq * simtime[i]) +
			(amp / 6.0) * sin(3.0 * M_PI * freq * simtime[i]) +
			(amp / 3.0) * sin(4.0 * M_PI * freq * simtime[i])) / 1.5;
	}

	for (i = 0; i < simtime_len; i++)
		ft2.data[i] = ((amp / 1.0) * sin(1.0 * M_PI * freq * simtime[i]) +
			(amp / 2.0) * sin(2.0 * M_PI * freq * simtime[i]) +
			(amp / 6.0) * sin(3.0 * M_PI * freq * simtime[i]) +
			(amp / 3.0) * sin(4.0 * M_PI * freq * simtime[i])) / 1.5;



	/***** Learning algorithm (double)*****/

	db_matrix P, k, x_db, M_db, r_db, wf_db, wi_db, wo_db, zt_db, dw, wo_len_db;
	double z_db, rPr, e, c;
	// initialize P matrix
	dbm_init(&P, nRec2Out, nRec2Out);
	for (i = 0; i < P.rows; i++)
		for (j = 0; j < P.cols; j++)
			P.data[i * P.cols + j] = (i == j) ? 1. : 0.;
	dbm_scale(&P, (1.0 / alpha));

	// initialize k
	dbm_init(&k, nRec2Out, 1);

	dbm_init(&wo_len_db, 1, simtime_len);
	dbm_init(&dw, nRec2Out, 1);

	// transform fixed point matrixes to double
	dbm_init(&x_db, x.rows, x.cols);
	for (i = 0; i < x_db.rows * x_db.cols; i++)
		x_db.data[i] = fixed_to_float(x.data[i]);

	dbm_init(&M_db, M.rows, M.cols);
	for (i = 0; i < M_db.rows * M_db.cols; i++)
		M_db.data[i] = fixed_to_float(M.data[i]);

	dbm_init(&r_db, r.rows, r.cols);
	for (i = 0; i < r_db.rows * r_db.cols; i++)
		r_db.data[i] = fixed_to_float(r.data[i]);

	dbm_init(&wf_db, wf.rows, wf.cols);
	for (i = 0; i < wf_db.rows * wf_db.cols; i++)
		wf_db.data[i] = fixed_to_float(wf.data[i]);

	dbm_init(&wi_db, wi.rows, wi.cols);
	for (i = 0; i < wi_db.rows * wi_db.cols; i++)
		wi_db.data[i] = fixed_to_float(wi.data[i]);

	dbm_init(&wo_db, wo.rows, wo.cols);
	for (i = 0; i < wo_db.rows * wo_db.cols; i++)
		wo_db.data[i] = fixed_to_float(wo.data[i]);

	dbm_init(&zt_db, zt.rows, zt.cols);
	for (i = 0; i < zt_db.rows * zt_db.cols; i++)
		zt_db.data[i] = fixed_to_float(zt.data[i]);
	z_db = fixed_to_float(z);

	// Output file header
	fprintf(res_file, "ft, zt, wo_len, ft2, zpt\n");


	printf("Learning...\n");

	for (t = 0; t < simtime_len; t++) {
		// temporary matrix variable
		db_matrix tmp, tmp2;
		
		// % sim, so x(t) and r(t) are created.

		/*** x = (1.0-dt)*x + M*(r*dt) + wf*(z*dt) + wi*(ft(ti)*dt) ***/
		// scale x by 1-dt
		dbm_scale(&x_db, 1.0 - dt);

		// create temporary matrix to hold intermediate values, add to x
		tmp = dbm_mult(&M_db, &r_db);
		dbm_scale(&tmp, dt);
		dbm_iadd(&x_db, &tmp);

		
		dbm_destroy(&tmp);
		tmp = dbm_copy(&wf_db);
		dbm_scale(&tmp, z_db);
		dbm_scale(&tmp, dt);
		dbm_iadd(&x_db, &tmp);

		dbm_destroy(&tmp);
		tmp = dbm_copy(&wi_db);
		dbm_scale(&tmp, ft.data[t]);
		dbm_scale(&tmp, dt);
		dbm_iadd(&x_db, &tmp);

		// update r
		for (i = 0; i < r_db.cols * r_db.rows; i++)
			r_db.data[i] = tanh(x_db.data[i]);
		
		// update z
		dbm_destroy(&tmp);
		tmp = dbm_transposed(&wo_db);
		dbm_imult(&tmp, &r_db);
		//dbm_print(&tmp);
		z_db = tmp.data[0];

		if (t % learn_every == 0) {
			// update inverse correlation matrix
			dbm_destroy(&k);
			k = dbm_mult(&P, &r_db);
			//dbm_print(&P);
			dbm_destroy(&tmp);
			tmp = dbm_transposed(&r_db);	// tmp = r'
			dbm_imult(&tmp, &k); // tmp = r'k, should be 1-by-1
			rPr = tmp.data[0];
			c = 1.0 / (1.0 + rPr);

			//printf("************Pr*********\n");
			//fpm_print(&k);
			//printf("r'Pr = %f\n", fixed_to_float(rPr));
			//fpm_print(&tmp);

			dbm_destroy(&tmp);
			tmp = dbm_copy(&k); // tmp = k
			tmp2 = dbm_transposed(&k); // tmp2 = k'
			dbm_imult(&tmp, &tmp2); // tmp = k*k'
			dbm_scale(&tmp, -c); // tmp = - k*k'*c
			dbm_iadd(&P, &tmp);	// P = P - k*k'*c
			dbm_destroy(&tmp);
			dbm_destroy(&tmp2);

			// update the error for the linear readout
			e = z_db - ft.data[t];
			//printf("z = %f, ft(t) = %f, e = %f\n", fixed_to_float(z), fixed_to_float(ft.data[t]), fixed_to_float(e));


			// update the output weights
			tmp = dbm_copy(&k); // tmp = k
			dbm_scale(&tmp, c);	// tmp = k*c
			dbm_scale(&tmp, -e); // tmp = - e*k*c
			dbm_destroy(&dw);
			dw = dbm_copy(&tmp);	// dw = - e * k * c
			dbm_iadd(&wo_db, &dw);		// wo = wo + dw
			dbm_destroy(&tmp);
		}

		// store the output
		zt_db.data[t] = z_db;
		tmp = dbm_transposed(&wo_db);	// tmp = wo'
		dbm_imult(&tmp, &wo_db);	// tmp = wo'*wo
		wo_len_db.data[t] = sqrt(tmp.data[0]);
		//if (t < 10)printf("rPr = %f\n", fixed_to_float(rPr));//fpm_print(&k);
		//printf("%f, %f, %f, %f\n", ft.data[t], zt_db.data[t], e, wo_len_db.data[t]);

		
		if (t % (simtime_len / 100) == 0) {
			progress++;
			printf("\r%d %% |", progress);
			for (i = 0; i < 100; i++) printf("%c",(i < progress ? 219 : ' '));
			printf("|");
		}
	}

	// pass the final output weight vector to the network
	double wo_err = 0;
	for (i = 0; i < wo.rows * wo.cols; i++) {
		wo.data[i] = float_to_fixed(wo_db.data[i]);
		wo_err += fabs((fixed_to_float(wo.data[i]) - wo_db.data[i]));
	}

	printf("\nTotal error during conversion between training and testing wo: %f\n", wo_err);

	printf("\nTesting...\n");
	progress = 0;
	// Test the network now
	for (t = 0; t < simtime_len; t++) {
		fp_matrix tmp;

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
		fpm_scale(&tmp, float_to_fixed(ft2.data[t]));
		fpm_scale(&tmp, float_to_fixed(dt));
		fpm_iadd(&x, &tmp);
		fpm_destroy(&tmp);

		// update r
		for (i = 0; i < r.cols * r.rows; i++)
			r.data[i] = float_to_fixed(tanh(fixed_to_float(x.data[i])));

		tmp = fpm_transposed(&wo);
		fpm_imult(&tmp, &r);
		z = tmp.data[0];
		zpt.data[t] = z;

		if (t % (simtime_len / 100) == 0) {
			progress++;
			printf("\r%d %% |", progress);
			for (i = 0; i < 100; i++) printf("%c", (i < progress ? 219 : ' '));
			printf("|");
		}
	}


	// write results
	for (i = 0; i < simtime_len; i++) {
		fprintf(res_file, "%f, %f, %f, %f, %f\n", ft.data[i], zt_db.data[i], wo_len_db.data[i], ft2.data[i], fixed_to_float(zpt.data[i]));
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
	dbm_destroy(&wo_len_db);
	fpm_destroy(&zt);
	fpm_destroy(&zpt);
	fpm_destroy(&x0);
	fpm_destroy(&z0);
	fpm_destroy(&x);
	fpm_destroy(&r);
	fpm_destroy(&P);

	free(simtime);
	free(simtime2);
}