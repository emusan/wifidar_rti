#include <stdio.h>
#include <fftw3.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "plplot.h"

#define SAMPLE_RATE 48000
#define RAMP_FREQ 50
#define SAMPLES_PER_WAVEFORM 195
#define WAVEFORM_BUFFER_SIZE 100
#define SAMPLES_RTI (SAMPLES_PER_WAVEFORM/2 + 1)

void retrieve_samples_csv(FILE *stream,int num_samples,float *input_samples);
void amplitude(fftw_complex *complex_array,double *amplitude_array,int length);

int main() 
{
	// general use variables
	int i,j,k;
	//int i,j;

	// file I/O variables
	FILE *stream = fopen("/dev/ttyUSB0","r");
	double input_samples[SAMPLES_PER_WAVEFORM];
	//double input_samples_delay[SAMPLES_PER_WAVEFORM];

	// plplot variables
	//PLFLT time_x[SAMPLES_PER_WAVEFORM],time_y[SAMPLES_PER_WAVEFORM];
	/*PLFLT time_xmin = 0.,time_xmax = SAMPLES_PER_WAVEFORM;
	PLFLT time_ymin = -1.,time_ymax = 1.;*/
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
	/*plsdev("xwin");
	plinit();
	plenv(time_xmin,time_xmax,time_ymin,time_ymax,0,0);
	pllab("x","y = sin(2*pi*100*x)","Simple sine plot");*/
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

	//char line[20];
	//char *curr_num;
	//int temp;

	double temp_thing;
	int foundZero;
	int currChar;
	int tempChar;
	int curr_count = 0;
	//int debug_count = 0;

	while(1)
	{
		for(j = 0;j < 4;j++)
		{
			foundZero = 0;
			// take in samples
			/*for(curr_num = strtok(NULL,","),i=0; curr_num != NULL && i<SAMPLES_PER_WAVEFORM; curr_num = strtok(NULL,","),i++)
			{
				input_samples[i] = (float) strtof(curr_num,NULL);
			}*/

			//printf("%i\n",j);
			for(i = 0;i < SAMPLES_PER_WAVEFORM;i++)
			{
				if(0 == i)
				{
					while(!foundZero)
					{
						currChar = fgetc(stream);
						if((1 << 7 & currChar) != 0)
						{
							//debug_count = 0;
							foundZero = 1;
							currChar = 0x7F & currChar; // clear out the waveform_start bit
							tempChar = fgetc(stream);
							temp_thing = (double)((currChar << 6) | tempChar);
							if(curr_count == 0)
							{
								input_samples[i] = temp_thing;
							} else {
								input_samples[i] = temp_thing;// - input_samples_delay[i];
							}
							//printf("found zero!\n");
							//printf("%f ",input_samples[i]);
							
						}
					}
				} else {
					//debug_count = debug_count + 1;
					currChar = fgetc(stream);
					currChar = 0x7F & currChar;
					tempChar = fgetc(stream);
					temp_thing = (double)((currChar << 6) | tempChar);
					if(curr_count == 0)
					{
						input_samples[i] = temp_thing;
					} else {
						input_samples[i] = temp_thing;// - input_samples_delay[i];
					}
					//printf("%f ",temp_thing);
				}
				//input_samples_delay[i] = temp_thing;
			}
			printf("\n");
			//printf("%i\n",i);
			//printf("%i",debug_count);
			//printf("----");

			/*
			// display time domain
			for(i = 0;i < SAMPLES_PER_WAVEFORM;i++)
			{
				//time_x[i] = i;
				time_y[i] = (PLFLT) input_samples[i];
				//printf("%f %f\n",time_x[i],time_y[i]);
			}
			//plsstrm(0);

			//plline(SAMPLES_PER_WAVEFORM,time_x,time_y);
			//plpoin(SAMPLES_PER_WAVEFORM,time_x,time_y,1);
			*/

			// take FFT
			for(i = 0;i<SAMPLES_PER_WAVEFORM;i++)
			{
				time_domain_array[i] = (double) input_samples[i];
			}
			fftw_execute(fft_plan);
			
			amplitude(freq_domain_array,time_domain_array,SAMPLES_RTI);
			
			// display on RTI
			for (k = 0;k<WAVEFORM_BUFFER_SIZE;k++)
			{
				for(i = 0;i<SAMPLES_RTI;i++)
				{
					if(WAVEFORM_BUFFER_SIZE - 1 == k)
					{
						rti_z[i][k] = time_domain_array[i];
						//printf("%f\n",rti_z[i][k]);
					} else {
						rti_z[i][k] = rti_z[i][k+1];
						printf("%f\n",rti_z[i][k]);
					}
				}
			}
			//printf("-----");
			/*for(i = 0;i < SAMPLES_RTI;i++)
			{
				rti_z[i][j] = time_domain_array[i];
			}*/
		}

		plsstrm(1);

		plimage((const PLFLT * const *)(rti_z),rti_xint,rti_yint,1.0,(PLFLT) rti_xint,-30.,0.0,-0.0,10.0,1.0,(PLFLT) rti_xint,-30.,0.0);
		//printf("image updated");
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
		amplitude_array[i] = log10(sqrt((complex_array[i][0] * complex_array[i][0]) + (complex_array[i][1] * complex_array[i][1])));
	}
}
