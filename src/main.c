#include <stdio.h>
#include <fftw3.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "plplot.h"

#define N 1000

int main() {
	// file I/O variables
	FILE *stream = fopen("test_signal_100Hz_50kHzFs.csv","r");
	float all_samples[300000];
	float samples_section[250][N];
	char *curr_num;
	int i,j;
	char line[3000000];

	// plplot variables
	PLFLT x[N],y[N];
	PLFLT xmin = 0.,xmax = N,ymin = -1.,ymax = 1.;

	// fftw variables
	fftw_complex *freq_domain_array;
	double *time_domain_array;
	fftw_plan fft_plan;

	// fftw init stuff
	time_domain_array = (double*) fftw_malloc(sizeof(double) * N);
	freq_domain_array = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * ((N/2)+1));
	fft_plan = fftw_plan_dft_r2c_1d(N,time_domain_array,freq_domain_array,FFTW_ESTIMATE);
	if(0 == fft_plan)
	{
		printf("there has been a problem creating the plan :(");
		return 1;
	}

	// grab the info from the file
	fgets(line,3000000,stream);

	curr_num = strtok(line,",");
	i = 0;
	
	while(curr_num != NULL) 
	{
		//printf("%f\n",strtof(curr_num,NULL));
		all_samples[i] = strtof(curr_num,NULL);
		i++;
		curr_num = strtok(NULL,",");
	}

	for(i = 0;i < 250;i++)
	{
		for(j = 0;j < N;j++)
		{
			samples_section[i][j] = all_samples[(i+1)*j];
			//printf("%f ",samples_section[i][j]);
		}
		//printf("\n");
	}

	for(j = 0;j < N;j++)
	{
		x[j] = j;
		y[j] = (PLFLT) samples_section[1][j];
	}

	plsdev("xwin");
	plinit();

	plssub(2,1);
	plenv(xmin,xmax,ymin,ymax,0,0);
	pladv(1);
	pllab("x","y = sin(2*pi*100*x)","Simple sine plot");

	plline(N,x,y);

	for(i = 0;i<N;i++)
	{
		time_domain_array[i] = (double) y[i];
	}
	fftw_execute(fft_plan);
	for(i = 0;i<N/2;i++)
	{
		y[i] = sqrt((freq_domain_array[i][0] * freq_domain_array[i][0]) + (freq_domain_array[i][1] * freq_domain_array[i][1]));
		printf("%f\n",y[i]);
	}

	plenv(xmin,xmax/2,ymin,ymax,0,0);
	pllab("x","y = FFT(y)","FFT of previous sine wave");
	plline(xmax/2,x,y);

	fftw_destroy_plan(fft_plan);
	fftw_free(time_domain_array);fftw_free(freq_domain_array);
	plend();

	return 0;
}
