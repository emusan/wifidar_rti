#include <stdio.h>
#include <fftw3.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "plplot.h"

#define N 1000

void retrieve_samples_csv(FILE *stream,int num_samples,float *input_samples);
void amplitude(fftw_complex *complex_array,double *amplitude_array,int length);

int main() 
{
	// file I/O variables
	FILE *stream = fopen("test_signal_100Hz_50kHzFs.csv","r");
	float input_samples[N];
	int i,j;

	// plplot variables
	PLFLT x[N],y[N];
	PLFLT **z;
	PLFLT xmin = 0.,xmax = N,ymin = -1.,ymax = 1.;

	PLINT xint = N/2;
	PLINT yint = 1;

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

	// take prepared data for fftw and take fft
	for(i = 0;i<N;i++)
	{
		time_domain_array[i] = (double) y[i];
	}
	fftw_execute(fft_plan);
	amplitude(freq_domain_array,y,N/2);

	// display fft
	/*
	plenv(xmin,xmax/2,ymin,ymax,0,0);
	pllab("x","y = FFT(y)","FFT of previous sine wave");
	plline(xmax/2,x,y);
	*/
	
	plAlloc2dGrid(&z,N/2,1);
	if(NULL == z)
	{
		printf("z not allocated properly");
		return 0;
	}
	for(i = 0;i < N/2;i++)
	{
		z[i][0] = y[i];
		//printf("%f\n",z[i][1]);
	}

	plenv(xmin,xmax/2,-1.0,+1.0,0,0);
	pladv(2);

	plimage((const PLFLT * const *)(z),xint,yint,1.0,(PLFLT) xint,-0.5,(PLFLT) .5,-1.0,550.0,1.0,(PLFLT) xint,-0.5,(PLFLT) 0.5);

	
	// clean up
	fftw_destroy_plan(fft_plan);
	fftw_free(time_domain_array);fftw_free(freq_domain_array);
	plFree2dGrid(z,xint,yint);
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

void amplitude(fftw_complex *complex_array,double *amplitude_array,int length)
{
	int i;

	for (i = 0; i<length;i++)
	{
		amplitude_array[i] = sqrt((complex_array[i][0] * complex_array[i][0]) + (complex_array[i][1] * complex_array[i][1]));
	}
}
