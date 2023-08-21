#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

typedef struct inputData
{
	float i;
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
			sscanf (lineString, "%*d %f\n", &inputs[currentIndex].i);
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

INPUT_DATA *computeCorrelation (INPUT_DATA *correlation, INPUT_DATA *inputs, int nInputs)
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
			if (i != j)
			{
				lag = j - i;
				correlation[lag].i += inputs[i].i * inputs[j].i;
				// printf("%d %d (%f x %f) = %f [lag: %d]\n", i, j, inputs[i].i, inputs[j].i, inputs[i].i * inputs[j].i, j - i);
				// usleep (100000);
				nDenominator[lag]++;
			}
		}
	}

	for (int i = 0; i < nInputs; ++i) {
		correlation[lag].i /= nDenominator[i]; }

	return correlation;
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
	correlation = computeCorrelation (correlation, inputs, nInputs);

	for (int i = 0; i < nInputs; ++i)
	{
		printf("%f\n", inputs[i].i / inputs[0].i);
		usleep (100000);
	}

	fclose (file_input);
	return 0;
}