#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

typedef struct inputData
{
	long double i;
} INPUT_DATA;

int countNLines (FILE *file_input, int nInputs)
{
	char lineString[3000];
	nInputs = 0;

	while (fgets (lineString, 3000, file_input) != NULL)
	{
		if (lineString[0] != '#')
		{
			nInputs++;
		}
	}

	rewind (file_input);

	return nInputs;
}

INPUT_DATA *storeInputs (FILE *file_input, int nInputs, INPUT_DATA *inputs)
{
	char lineString[3000];
	int currentIndex = 0;

	while (fgets (lineString, 3000, file_input) != NULL)
	{
		if (lineString[0] != '#')
		{
			sscanf (lineString, "%*d %Lf\n", &inputs[currentIndex].i);
			currentIndex++;
		}
	}

	return inputs;
}

INPUT_DATA *resetCorrelation (INPUT_DATA *correlation, int nInputs)
{
	for (int i = 0; i < nInputs; ++i)
	{
		correlation[i].i = 0;
	}
	return correlation;
}

INPUT_DATA *computeCorrelation (INPUT_DATA *correlation, INPUT_DATA *inputs, int nInputs, float mean)
{
	int lag;
	int *nDenominator;
	nDenominator = (int *) malloc (nInputs * sizeof (int));

	for (int i = 0; i < nInputs; ++i) {
		nDenominator[i] = 0; }

	for (int i = 0; i < nInputs; ++i)
	{
		for (int j = i; j < nInputs; ++j)
		{
			if (1)
			{
				lag = j - i;
				correlation[lag].i += ((inputs[i].i - mean) * (inputs[j].i - mean));
				nDenominator[lag]++;
			}
		}
	}

	long double *normalizingFactor;
	normalizingFactor = (long double *) malloc (nInputs * sizeof (long double));

	for (int i = 0; i < nInputs; ++i)
	{
		normalizingFactor[i] = (long double)nDenominator[i];
	}

	for (int i = 0; i < nInputs; ++i) {
		correlation[i].i /= normalizingFactor[i];
		correlation[i].i *= 1000000;
	}

	return correlation;
}

float computeMean (long double mean, INPUT_DATA *inputs, int nInputs)
{
	mean = 0;

	for (int i = 0; i < nInputs; ++i)
	{
		mean += inputs[i].i;
	}

	mean /= (long double)nInputs;
	printf("Mean of the input sample: %Lf\n", mean);

	return mean;
}

int main(int argc, char const *argv[])
{
	FILE *file_input;
	file_input = fopen (argv[1], "r");

	int nInputs = countNLines (file_input, nInputs);

	INPUT_DATA *inputs, *correlation;
	inputs = (INPUT_DATA *) malloc (nInputs * sizeof (INPUT_DATA));
	correlation = (INPUT_DATA *) malloc (nInputs * sizeof (INPUT_DATA));

	inputs = storeInputs (file_input, nInputs, inputs);
	correlation = resetCorrelation (correlation, nInputs);

	long double mean;
	mean = computeMean (mean, inputs, nInputs);

	correlation = computeCorrelation (correlation, inputs, nInputs, mean);

	char *outputFilename;
	outputFilename = (char *) malloc (1000 * sizeof (char));

	snprintf (outputFilename, 1000, "%s.corr", argv[1]);
	FILE *file_output;
	file_output = fopen (outputFilename, "w");

	long double normalizingFactor;
	normalizingFactor = correlation[0].i;

	for (int i = 0; i < nInputs; ++i)
	{
		correlation[i].i /= normalizingFactor;
		fprintf(file_output, "%Lf\n", correlation[i].i);
	}

	fclose (file_input);
	fclose (file_output);
	return 0;
}