#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"plplot.h"
#include"plConfig.h"

// TODO: take fft, make RTI, ???, profit!

int main()
{
	FILE *stream = fopen("test_signal_100Hz_50kHzFs.csv","r");
	float all_samples[300000];
	float samples_section[250][1000];
	char *curr_num;
	int i,j;
	
	PLFLT x[1000],y[1000];
	PLFLT xmin = 0.,xmax = 1000.,ymin = -1.,ymax = 1.;

	char line[3000000];
	fgets(line,3000000,stream);
	//printf(line);

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
		for(j = 0;j < 1000;j++)
		{
			samples_section[i][j] = all_samples[(i+1)*j];
			//printf("%f ",samples_section[i][j]);
		}
		//printf("\n");
	}

	for(j = 0;j < 1000;j++)
	{
		x[j] = j;
		y[j] = (PLFLT) samples_section[1][j];
	}

	plsdev("xwin");
	plinit();

	plenv(xmin,xmax,ymin,ymax,0,0);
	pllab("x","y = sin(2*pi*100*x)","Simple sine plot");

	plline(1000,x,y);

	plend();

	return 0;
}
