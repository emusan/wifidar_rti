#include <stdio.h>
#include <fftw3.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "plplot.h"

#define N 1000

void retrieve_samples_csv(FILE *stream,int num_samples,float *input_samples);

int main() 
{
	// file I/O variables
	FILE *stream = fopen("test_signal_100Hz_50kHzFs.csv","r");
	float input_samples[N];
	int i,j;

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
	retrieve_samples_csv(stream,N,input_samples);

	// prep for time-domain display
	for(j = 0;j < N;j++)
	{
		x[j] = j;
		y[j] = (PLFLT) input_samples[j];
	}

	// init plplot and set up the window
	plsdev("xwin");
	plinit();

	plssub(2,1);
	plenv(xmin,xmax,ymin,ymax,0,0);
	pladv(1);
	pllab("x","y = sin(2*pi*100*x)","Simple sine plot");

	plline(N,x,y);

	// take prepare data for fftw and take fft
	for(i = 0;i<N;i++)
	{
		time_domain_array[i] = (double) y[i];
	}
	fftw_execute(fft_plan);
	for(i = 0;i<N/2;i++)
	{
		y[i] = sqrt((freq_domain_array[i][0] * freq_domain_array[i][0]) + (freq_domain_array[i][1] * freq_domain_array[i][1]));
		//printf("%f\n",y[i]);
	}

	// display fft
	plenv(xmin,xmax/2,ymin,ymax,0,0);
	pllab("x","y = FFT(y)","FFT of previous sine wave");
	plline(xmax/2,x,y);

	
	// clean up
	fftw_destroy_plan(fft_plan);
	fftw_free(time_domain_array);fftw_free(freq_domain_array);
	plend();

	return 0;
}

// function retrieve_samples_csv
// pulls num_samples from input csv file stream and puts them into the array specified by input_samples
void retrieve_samples_csv(FILE *stream,int num_samples,float *input_samples)
{
	char line[num_samples*100];
	char *curr_num;
	int i;
	fgets(line,num_samples*100,stream);
	
	for(curr_num = strtok(line,","),i=0; curr_num != NULL && i<num_samples; curr_num = strtok(NULL,","),i++)
	{
		input_samples[i] = (float) strtof(curr_num,NULL);
	}
}

