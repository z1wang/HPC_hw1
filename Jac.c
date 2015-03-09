#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "util.h"

double residue(int n, double* u){
	double res = 0;
	int s = n - 1;

	//unit is 1/h^2
	double unit = (n + 1) * (n + 1);	

	//temp is A * u - f
	double *temp = malloc(sizeof(double) * n);

	for (int i = 0; i < n; i++){
		temp[i] = 2 * u[i];
		if(i == 0){
			temp[i] = temp[i] - u[i + 1];
		}else if(i == s){
			temp[i] = temp[i] - u[i - 1];
		}else{
			temp[i] = temp[i] - u[i - 1] - u[i + 1];
		}
		temp[i] = temp[i] * unit;
		temp[i] = temp[i] - 1;
		res += temp[i] * temp[i];
	}
	free(temp);
	res = sqrt(res);
//	printf("residue is %f\n", res);
	return res;
}

double * Jacomp(int n, double * init){
	double *ret = malloc(sizeof(double) * n);

	//unit is 1/h^2
	double unit = (n + 1) * (n + 1);
	int s = n - 1;
	for (int i = 0; i < n; i++){
	//compute f_i - a * u
		if(i == 0){
			ret[i] = 1 + unit * init[i + 1];
		}else if(i == s){
			ret[i] = 1 + unit * init[i - 1];
		}else{
			ret[i] = 1 + unit * init[i - 1] + unit * init[i + 1];
		}
		ret[i] = ret[i] / 2;
		ret[i] = ret[i] / unit;
	}	
	free(init);
	return ret;
}

double * Jac(int n, int it){
	//set iterations N
	double * init = malloc(sizeof(double) * n);
	double red;

	//initial vector is set to be all 0's
	for(int i = 0; i < n; i ++){
		init[i] = 0;
	}
	double ie = residue(n, init);
	double tol = ie / 1000000;
	double * ans = Jacomp(n, init);
	it --;
//	red = residue(n, ans);
	while(it > 0){
		ans = Jacomp(n, ans);
		it --;
		red = residue(n, ans);
	}
	return ans;
}

int main(int argc, char **argv){
	if (argc != 3) {
		fprintf(stderr, "Function needs vector size as input arguments!\n");
		abort();
	}
	int n = atoi(argv[1]);
	int it = atoi(argv[2]);


	//discretization points
	double * res = Jac(n, it);
	double ie = residue(n, res);
	printf("%f\n", ie)
;/*	int i;
	for(i = 0; i < n; i++){
		printf("%f ", res[i]);
	}
	printf("\n");*/

	free(res);
	return 0;
}