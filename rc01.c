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
#include "sim_params.h"
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

	fp_matrix M, wf, wi, x, r;
	db_matrix wo, zt, zpt;
	double z;
	unsigned N, nsecs, learn_every;
	double p, g, alpha, dt;

	N = SIM_N;		// reservoir size
	p = SIM_p;		// connection density
	g = SIM_g;	// g greater than 1 leads to chaotic networks.
	alpha = SIM_alpha;
	nsecs = SIM_nsecs;
	dt = SIM_dt;
	learn_every = SIM_learn_every;

	double scale;
	scale = 1.0 / sqrt(p * N);

	// initialize sparse-random (normal) reservoir connectivity matrix (fixed point)
	// 	M = sprandn(N, N, p) * g * scale; % sparse random connectivity
	fpm_init(&M, N, N);
	fpm_fillsprandn(&M, 0, 1, p);
	fpm_scale(&M, float_to_fixed(g * scale));

	// initialize output weights vector (double)
	dbm_init(&wo, N, 1);

	// initialize wf (fixed point)
	fpm_init(&wf, N, 1);
	fpm_fillrand(&wf, 0, float_to_fixed(1));
	fpm_iadd_scalar(&wf, float_to_fixed(-0.5));
	fpm_scale(&wf, float_to_fixed(2));
	
	fpm_init(&wi, N, 1);
	fpm_fillrand(&wi, 0, float_to_fixed(1));
	fpm_iadd_scalar(&wi, float_to_fixed(-0.5));
	fpm_scale(&wi, float_to_fixed(2));

	printf("\tN: %d\n", N);
	printf("\tg: %f\n", g);
	printf("\tp: %f\n", p);
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

	// initialize simulation output vectors
	dbm_init(&zt, 1, simtime_len);
	dbm_init(&zpt, 1, simtime_len);

	// initialize reservoir nodes vector
	fpm_init(&x, N, 1);
	fpm_fillrandn(&x, 0, 1);
	fpm_scale(&x, float_to_fixed(0.5));

	// initialize output value
	z = rand_db(0, 1) / 2.;

	// initialize reservoir nodes nonlinear state vector
	fpm_init(&r, x.rows, x.cols);
	for (i = 0; i < r.rows * r.cols; i++)
		r.data[i] = float_to_fixed(tanh(fixed_to_float(x.data[i])));

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



	/***** Learning algorithm *****/

	db_matrix P, k, x_db, M_db, r_db, wf_db, wi_db, dw, wo_len_db;
	double rPr, e, c;
	// initialize P matrix
	dbm_init(&P, N, N);
	for (i = 0; i < P.rows; i++)
		for (j = 0; j < P.cols; j++)
			P.data[i * P.cols + j] = (i == j) ? 1. : 0.;
	dbm_scale(&P, (1.0 / alpha));

	// initialize k
	dbm_init(&k, N, 1);

	dbm_init(&wo_len_db, 1, simtime_len);
	dbm_init(&dw, N, 1);

	// transform fixed point vectors to double
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

	// Output file header
	fprintf(res_file, "ft,zt,wo_len,ft2,zpt\n");


	printf("Learning...\n");

	for (t = 0; t < simtime_len; t++) {
		// temporary matrix variable
		db_matrix tmp, tmp2;
		fp_matrix tmp_fp, tmp2_fp;

		// % sim, so x(t) and r(t) are created.

		/*** x = (1.0-dt)*x + M*(r*dt) + wf*(z*dt) + wi*(ft(ti)*dt) ***/
		// scale x by 1-dt
		//dbm_scale(&x_db, 1.0 - dt);
		fpm_scale(&x, float_to_fixed(1. - dt));

		// create temporary matrix to hold intermediate values, add to x
		//tmp = dbm_mult(&M_db, &r_db);
		//dbm_scale(&tmp, dt);
		//dbm_iadd(&x_db, &tmp);

		tmp_fp = fpm_mult(&M, &r);
		fpm_scale(&tmp_fp, float_to_fixed(dt));
		fpm_iadd(&x, &tmp_fp);
		
		//dbm_destroy(&tmp);
		//tmp = dbm_copy(&wf_db);
		//dbm_scale(&tmp, z);
		//dbm_scale(&tmp, dt);
		//dbm_iadd(&x_db, &tmp);

		fpm_destroy(&tmp_fp);
		tmp_fp = fpm_copy(&wf);
		fpm_scale(&tmp_fp, float_to_fixed(z*dt));
		fpm_iadd(&x, &tmp_fp);


		//dbm_destroy(&tmp);
		//tmp = dbm_copy(&wi_db);
		//dbm_scale(&tmp, ft.data[t]);
		//dbm_scale(&tmp, dt);
		//dbm_iadd(&x_db, &tmp);
		fpm_destroy(&tmp_fp);
		tmp_fp = fpm_copy(&wi);
		fpm_scale(&tmp_fp, float_to_fixed(ft.data[t] * dt));
		fpm_iadd(&x, &tmp_fp);

		// update r
		//for (i = 0; i < r_db.cols * r_db.rows; i++)
		//	r_db.data[i] = tanh(fixed_to_float(float_to_fixed(x_db.data[i]))); // match non-linearity with fp case
		for (i = 0; i < r.cols * r.rows; i++)
			r.data[i] = float_to_fixed(tanh(fixed_to_float(x.data[i])));

		// update z
		//dbm_destroy(&tmp);
		//tmp = dbm_transposed(&wo);
		//dbm_imult(&tmp, &r_db);
		////dbm_print(&tmp);
		//z = tmp.data[0];

		z = 0.;
		for (i = 0; i < N; i++) {
			z += wo.data[i] * fixed_to_float(r.data[i]);
		}


		if (t % learn_every == 0) {
			// update r_db
			for (i = 0; i < r.cols * r.rows; i++)
				r_db.data[i] = fixed_to_float(r.data[i]);

			// update inverse correlation matrix
			dbm_destroy(&k);
			k = dbm_mult(&P, &r_db);
			//dbm_print(&P);
			//dbm_destroy(&tmp);
			tmp = dbm_transposed(&r_db);	// tmp = r'
			dbm_imult(&tmp, &k); // tmp = r'k, should be 1-by-1
			rPr = tmp.data[0];
			c = 1.0 / (1.0 + rPr);

			dbm_destroy(&tmp);
			tmp = dbm_copy(&k); // tmp = k
			tmp2 = dbm_transposed(&k); // tmp2 = k'
			dbm_imult(&tmp, &tmp2); // tmp = k*k'
			dbm_scale(&tmp, -c); // tmp = - k*k'*c
			dbm_iadd(&P, &tmp);	// P = P - k*k'*c
			dbm_destroy(&tmp);
			dbm_destroy(&tmp2);

			// update the error for the linear readout
			e = z - ft.data[t];

			// update the output weights
			tmp = dbm_copy(&k); // tmp = k
			dbm_scale(&tmp, c);	// tmp = k*c
			dbm_scale(&tmp, -e); // tmp = - e*k*c
			dbm_destroy(&dw);
			dw = dbm_copy(&tmp);	// dw = - e * k * c
			dbm_iadd(&wo, &dw);		// wo = wo + dw
			dbm_destroy(&tmp);
		}

		// store the output
		zt.data[t] = z;

		tmp = dbm_transposed(&wo);	// tmp = wo'
		dbm_imult(&tmp, &wo);	// tmp = wo'*wo
		wo_len_db.data[t] = sqrt(tmp.data[0]);

		
		if (t % (simtime_len / 100) == 0) {
			progress++;
			printf("\r%d %% |", progress);
			for (i = 0; i < 100; i++) printf("%c",(i < progress ? 219 : ' '));
			printf("|");
		}
	}

	printf("\nTesting...\n");
	progress = 0;
	z = 0;
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
		fpm_scale(&tmp, float_to_fixed(z * dt));
		fpm_iadd(&x, &tmp);
		fpm_destroy(&tmp);

		tmp = fpm_copy(&wi);
		fpm_scale(&tmp, float_to_fixed(ft2.data[t] * dt));
		fpm_iadd(&x, &tmp);
		fpm_destroy(&tmp);

		// update r
		for (i = 0; i < r.cols * r.rows; i++)
			r.data[i] = float_to_fixed(tanh(fixed_to_float(x.data[i])));


		// output with float weights
		z = 0.;
		for (i = 0; i < N; i++) {
			z += wo.data[i] * fixed_to_float(r.data[i]);
		}
		zpt.data[t] = z;


		if (t % (simtime_len / 100) == 0) {
			progress++;
			printf("\r%d %% |", progress);
			for (i = 0; i < 100; i++) printf("%c", (i < progress ? 219 : ' '));
			printf("|");
		}
	}

	printf("\n");

	// write results
	for (i = 0; i < simtime_len; i++) {
		fprintf(res_file, "%f,%f,%f,%f,%f\n", ft.data[i], zt.data[i], wo_len_db.data[i], ft2.data[i], zpt.data[i]);
	}

	// free resources
	fclose(res_file);

// TODO : free matrix

	free(simtime);
	free(simtime2);
}