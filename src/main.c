#include <stdio.h>
#include <fftw3.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "plplot.h"

#define SAMPLE_RATE 50000
#define RAMP_FREQ 100
#define SAMPLES_PER_WAVEFORM (SAMPLE_RATE/RAMP_FREQ) - 1
#define WAVEFORM_BUFFER_SIZE 30
#define SAMPLES_RTI (SAMPLES_PER_WAVEFORM/2 + 1)

void retrieve_samples_csv(FILE *stream,int num_samples,float *input_samples);
void amplitude(fftw_complex *complex_array,double *amplitude_array,int length);

int main() 
{
	// general use variables
	int i,j,k;

	// file I/O variables
	FILE *stream = fopen("test_signal_100Hz_50kHzFs.csv","r");
	float input_samples[SAMPLES_PER_WAVEFORM];

	// plplot variables
	PLFLT time_x[SAMPLES_PER_WAVEFORM],time_y[SAMPLES_PER_WAVEFORM];
	PLFLT time_xmin = 0.,time_xmax = SAMPLES_PER_WAVEFORM;
	PLFLT time_ymin = -1.,time_ymax = 1.;
	PLFLT **rti_z;
	PLFLT rti_xmin = 0.,rti_xmax = SAMPLES_RTI;
	PLFLT rti_ymin = -30.,rti_ymax = 0.;
	PLINT rti_xint = SAMPLES_RTI,rti_yint = WAVEFORM_BUFFER_SIZE;

	// fftw variables
	fftw_complex *freq_domain_array;
	double *time_domain_array;
	fftw_plan fft_plan;

	// fftw init stuff
	time_domain_array = (double*) fftw_malloc(sizeof(double) * SAMPLES_PER_WAVEFORM);
	freq_domain_array = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (SAMPLES_RTI));
	fft_plan = fftw_plan_dft_r2c_1d(SAMPLES_PER_WAVEFORM,time_domain_array,freq_domain_array,FFTW_PATIENT);
	if(0 == fft_plan)
	{
		printf("there has been a problem creating the plan :(");
		return 1;
	}

	// init plplot and set up the window
	plsdev("xwin");
	plinit();
	plenv(time_xmin,time_xmax,time_ymin,time_ymax,0,0);
	pllab("x","y = sin(2*pi*100*x)","Simple sine plot");
	plsstrm(1);
	plsdev("xwin");
	plinit();
	plenv(rti_xmin,rti_xmax,rti_ymin,rti_ymax,0,0);
	pllab("Distance (samples)","time (s)","RTI");

	plAlloc2dGrid(&rti_z,rti_xint,rti_yint);
	if(NULL == rti_z)
	{
		printf("z not allocated properly");
		return 0;
	}

	char line[1000000];
	char *curr_num;
	fgets(line,1000000,stream);
	curr_num = strtok(line,",");


	for(j = 0;j < WAVEFORM_BUFFER_SIZE;j++)
	{
		// take in samples
		for(curr_num = strtok(NULL,","),i=0; curr_num != NULL && i<SAMPLES_PER_WAVEFORM; curr_num = strtok(NULL,","),i++)
		{
			input_samples[i] = (float) strtof(curr_num,NULL);
		}

		
		// display time domain
		for(i = 0;i < SAMPLES_PER_WAVEFORM;i++)
		{
			time_x[i] = i;
			time_y[i] = (PLFLT) input_samples[i];
			//time_y[i] = ((float)i)/SAMPLES_PER_WAVEFORM;
			//printf("%f %f\n",time_x[i],time_y[i]);
		}
		plsstrm(0);

		//plline(SAMPLES_PER_WAVEFORM,time_x,time_y);
		plpoin(SAMPLES_PER_WAVEFORM,time_x,time_y,1);

		// take FFT
		for(i = 0;i<SAMPLES_PER_WAVEFORM;i++)
		{
			time_domain_array[i] = (double) input_samples[i];
		}
		fftw_execute(fft_plan);
		
		amplitude(freq_domain_array,time_y,SAMPLES_RTI);
		
		// display on RTI
		for (k = 0;k<WAVEFORM_BUFFER_SIZE;k++)
		{
			for(i = 0;i<SAMPLES_PER_WAVEFORM;i++)
			{
				if(WAVEFORM_BUFFER_SIZE - 1 == k)
				{
					rti_z[i][k] = time_y[i];
				} else {
					rti_z[i][k] = rti_z[i][k+1];
				}
			}
		}
		/*for(i = 0;i < SAMPLES_RTI;i++)
		{
			rti_z[i][j] = time_y[i];
		}*/

		plsstrm(1);

		plimage((const PLFLT * const *)(rti_z),rti_xint,rti_yint,1.0,(PLFLT) rti_xint,-30.,0.0,-1.0,550.0,1.0,(PLFLT) rti_xint,-30.,0.0);
	}

	// clean up
	fftw_destroy_plan(fft_plan);
	fftw_free(time_domain_array);
	fftw_free(freq_domain_array);
	plFree2dGrid(rti_z,rti_xint,rti_yint);
	plend();

	return 0;
}

void amplitude(fftw_complex *complex_array,double *amplitude_array,int length)
{
	int i;

	for (i = 0; i<length;i++)
	{
		amplitude_array[i] = log(sqrt((complex_array[i][0] * complex_array[i][0]) + (complex_array[i][1] * complex_array[i][1])));
	}
}
