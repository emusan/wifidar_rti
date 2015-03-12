#include <stdio.h>
#include <fftw3.h>

#define N 500
#define top_bit 0b10000000
#define new_bit 0b01000000

int main() {
	fftw_complex *freq_domain_array;
	double *time_domain_array;
	fftw_plan fft_plan;

	time_domain_array = (double*) fftw_malloc(sizeof(double) * N);
	freq_domain_array = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * ((N/2)+1));
	fft_plan = fftw_plan_dft_r2c_1d(N,time_domain_array,freq_domain_array,FFTW_MEASURE);

	fftw_destroy_plan(fft_plan);
	fftw_free(time_domain_array);fftw_free(freq_domain_array);
	return 0;
}
