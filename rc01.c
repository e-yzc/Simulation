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

	fp_matrix M;

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

	// 	M = sprandn(N, N, p) * g * scale;% sparse random connectivity
	fpm_init(&M, N, N);
	fpm_fillsprandn(&M, 0, 1, p);
	fpm_scale(&M, float_to_fixed(g * scale));

}