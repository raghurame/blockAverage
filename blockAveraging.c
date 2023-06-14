#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

typedef struct blocks
{
	int size;
	float average, covariance, stdev, stderr;
} BLOCKS;

int countNLines (const char filename[], int nLines)
{
	FILE *countingLines;
	char *countingLinesString;
	countingLinesString = (char *) malloc (200 * sizeof (char));
	snprintf (countingLinesString, 200, "wc -l %s", filename);
	countingLines = popen (countingLinesString, "r");
	char lineString[2000];
	fgets (lineString, 2000, countingLines);
	sscanf (lineString, "%d\n", &nLines);
	return nLines;
}

float *initializeInputData (float *inputData, int nLines)
{
	for (int i = 0; i < nLines; ++i) {
		inputData[i] = -1; }

	return inputData;
}

float *readInputData (FILE *file_data, float *inputData, int requiredColumn)
{
	rewind (file_data);
	char *lineString;
	lineString = (char *) malloc (2000 * sizeof (char));
	int entryNumber = 0, currentStringPosition = 0;

	while (fgets (lineString, 2000, file_data) != NULL)
	{
		currentStringPosition = 0;

		if (lineString[0] != '#')
		{
			for (int i = 0; i < (requiredColumn - 1); ++i) {
				lineString += currentStringPosition;
				sscanf (lineString, "%*f%n", &currentStringPosition); }

			lineString += currentStringPosition;
			sscanf (lineString, "%f\n", &inputData[entryNumber]);
			entryNumber++;
		}
	}

	return inputData;
}

int main(int argc, char const *argv[])
{
	FILE *file_data, *file_output;
	file_data = fopen (argv[1], "r");
	file_output = fopen ("average.block", "w");

	int nLines = countNLines (argv[1], nLines), requiredColumn = atoi (argv[2]);
	float *inputData;
	inputData = (float *) malloc (nLines * sizeof (float));
	inputData = initializeInputData (inputData, nLines);
	inputData = readInputData (file_data, inputData, requiredColumn);


	fclose (file_data);
	fclose (file_output);
	return 0;
}