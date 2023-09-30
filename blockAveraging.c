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

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TRY TO FIX THESE FUNCTIONS
// CURRENTLY THEY ARE GIVING SEGMENTATION FAULT
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*float *initializeInputData (float *inputData, int nLines)
{
	for (int i = 0; i < nLines; ++i) {
		inputData[i] = -9999.0; }

	return inputData;
}

float *getInputColumn (float *inputData, int nLines)
{
	FILE *file_requiredColumn;
	file_requiredColumn = fopen ("required.column", "r");
	char lineString[2000];

	for (int i = 0; i < nLines; ++i)
	{
		fgets (lineString, 2000, file_requiredColumn);
		printf("%s\n", lineString);
		usleep (100000);
		sscanf (lineString, "%f\n", &inputData[i]);
		printf("%f\n", inputData[i]);
	}

	fclose (file_requiredColumn);
	return inputData;
}

void readInputData (FILE *file_data, int requiredColumn)
{
	rewind (file_data);
	char *lineString;
	lineString = (char *) malloc (2000 * sizeof (char));
	int entryNumber = 0, currentStringPosition = 0;
	float inputData_fileOutput;

	FILE *file_requiredColumn;
	file_requiredColumn = fopen ("required.column", "w");

	while (fgets (lineString, 2000, file_data) != NULL)
	{
		currentStringPosition = 0;

		if (lineString[0] != '#')
		{
			for (int i = 0; i < (requiredColumn - 1); ++i) {
				lineString += currentStringPosition;
				sscanf (lineString, "%*f %n", &currentStringPosition);
			}

			if (currentStringPosition < strlen (lineString))
			{
				lineString += currentStringPosition;
				fprintf(file_requiredColumn, "%s", lineString);
				printf("%d %s", entryNumber, lineString);
				entryNumber++;
			}
		}
	}

	fprintf(stdout, "Selected column saved as required.column\n");
	fflush (stdout);

	fclose (file_requiredColumn);
}*/

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BLOCKS *initializeBlocks (BLOCKS *blockAverages, int nLines)
{
	for (int i = 0; i < nLines; ++i) {
		blockAverages[i].size = i + 1; 
		blockAverages[i].average = 0;
		blockAverages[i].covariance = 0;
		blockAverages[i].stdev = 0;
		blockAverages[i].stderr = 0; }

	return blockAverages;
}

float *initializeBlockValues (float *blocks, int nLines)
{
	for (int i = 0; i < nLines; ++i)
	{
		blocks[i] = -9999;
	}

	return blocks;
}

BLOCKS *computeBlockAverages (BLOCKS *blockAverages, int nLines, float *inputData)
{
	int currentEntry = 0, nEntries = 0;
	float *blocks;
	blocks = (float *) malloc (nLines * sizeof (blocks));

	for (int i = 0; i < nLines; ++i)
	{
		if (inputData[i] > -9999) {
			nEntries++; }
	}

	// 'i' is to iterate through various block sizes
	// 'i + 1' is the block size
	for (int i = 0; i < floor (nLines / 2); ++i)
	{
		blocks = initializeBlockValues (blocks, nLines);
		currentEntry = 0;

		// going through the blocks
		// summing the items in the blocks
		// number of blocks = floor (nLines / (i + 1))
		for (int j = 0; j < floor (nLines / (i + 1)); ++j)
		{
			// do averaging within the block
			// (i + 1) is the block size
			blocks[j] = 0;
			for (int k = 0; k < (i + 1); ++k)
			{
				blocks[j] += inputData[currentEntry];
				currentEntry++;
			}
		}

		// going through the blocks
		// finding local averages of these blocks
		// number of blocks = floor (nLines / (i + 1))
		for (int j = 0; j < floor (nLines / (i + 1)); ++j)
		{
			blocks[j] /= (i + 1);
		}

		blockAverages[i].average = 0;
		// find the global average across blocks
		for (int j = 0; j < floor (nLines / (i + 1)); ++j)
		{
			if (blocks[j] == -9999)
			{
				printf("ERROR: Blocks in default value !\n");
				exit (1);
			}

			blockAverages[i].average += blocks[j];
		}

		blockAverages[i].average /= (float)(floor (nLines / (i + 1)));

		// find the global stdev/covar across blocks
		for (int j = 0; j < floor (nLines / (i + 1)); ++j)
		{
			blockAverages[i].covariance += ((blocks[j] - blockAverages[i].average) * (blocks[j] - blockAverages[i].average));
		}

		blockAverages[i].covariance /= (float)(floor (nLines / (i + 1)) - 1);
		blockAverages[i].stdev = sqrt (blockAverages[i].covariance);
		blockAverages[i].stderr = blockAverages[i].stdev / sqrt ((float)((floor (nLines / (i + 1)))));
	}

	return blockAverages;
}

BLOCKS *computeBlockAverages2 (BLOCKS *blockAverages, int nLines, float *inputData)
{
	int currentEntry = 0, nEntries = 0;
	float *blocks;
	blocks = (float *) malloc (nLines * sizeof (blocks));
	int denomVariance = 0;

	float *covariance;
	covariance = (float *) malloc (nLines * sizeof (float));

	for (int i = 0; i < nLines; ++i)
	{
		if (inputData[i] > -9999) {
			nEntries++; }
	}

	// 'i' is to iterate through various block sizes
	// 'i + 1' is the block size
	for (int i = 0; i < floor (nLines / 2); ++i)
	{
		// reset the local covariance array
		for (int j = 0; j < nLines; ++j)
		{
			covariance[j] = 0;
		}

		blocks = initializeBlockValues (blocks, nLines);
		currentEntry = 0;

		// going through the blocks
		// summing the items in the blocks
		// number of blocks = floor (nLines / (i + 1))
		for (int j = 0; j < floor (nLines / (i + 1)); ++j)
		{
			// do averaging within the block
			// (i + 1) is the block size
			blocks[j] = 0;
			for (int k = 0; k < (i + 1); ++k)
			{
				blocks[j] += inputData[currentEntry];
				currentEntry++;
			}
		}

		// going through the blocks
		// finding local averages of these blocks
		// number of blocks = floor (nLines / (i + 1))
		for (int j = 0; j < floor (nLines / (i + 1)); ++j)
		{
			blocks[j] /= (i + 1);
		}

		// find the local covariance within the blocks
		currentEntry = 0;
		for (int j = 0; j < floor (nLines / (i + 1)); ++j)
		{
			// calculate covariance within the block
			// (i + 1) is the block size
			denomVariance = 0;
			for (int k = 0; k < (i + 1); ++k)
			{
				if (inputData[currentEntry] > 0)
				{
					covariance[j] += ((inputData[currentEntry] - blocks[j]) * (inputData[currentEntry] - blocks[j]));
					denomVariance++;

/*					if (i > 100)
					{
						printf("%f - %f (%d) = %f\n", inputData[currentEntry], blocks[j], currentEntry, covariance[j]);
						usleep (100000);
					}
*/				}
				currentEntry++;
			}

			covariance[j] /= denomVariance;
		}

		// find the average covariance
		for (int j = 0; j < floor (nLines / (i + 1)); ++j)
		{
			if (covariance[j] > 0)
			{
				blockAverages[i].covariance += covariance[j];
			}
		}

		blockAverages[i].covariance /= floor (nLines / (i + 1));

		blockAverages[i].average = 0;
		// find the global average across blocks
		for (int j = 0; j < floor (nLines / (i + 1)); ++j)
		{
			if (blocks[j] == -9999)
			{
				printf("ERROR: Blocks in default value !\n");
				exit (1);
			}

			blockAverages[i].average += blocks[j];
		}

		blockAverages[i].average /= (float)(floor (nLines / (i + 1)));

		// find the global stdev/covar across blocks
/*		for (int j = 0; j < floor (nLines / (i + 1)); ++j)
		{
			blockAverages[i].covariance += ((blocks[j] - blockAverages[i].average) * (blocks[j] - blockAverages[i].average));
		}
*/
/*		blockAverages[i].covariance /= (float)(floor (nLines / (i + 1)) - 1);*/
		blockAverages[i].stdev = sqrt (blockAverages[i].covariance);
		blockAverages[i].stderr = blockAverages[i].stdev / sqrt ((float)((floor (nLines / (i + 1)))));
	}

	return blockAverages;
}

float *saveInputData (float *inputData, int nLines, FILE *file_data)
{
	char lineString[2000];
	rewind (file_data);

	for (int i = 0; i < nLines; ++i)
	{
		fgets (lineString, 2000, file_data);
		sscanf (lineString, "%*f %f\n", &inputData[i]);
	}

	return inputData;
}

void printBlockAverageStats (FILE *file_output, BLOCKS *blockAverages, int nLines)
{
	for (int i = 0; i < floor (nLines / 2); ++i)
	{
		fprintf(file_output, "%d %f %f %f %f\n", i + 1, blockAverages[i].average, blockAverages[i].stdev, blockAverages[i].covariance, blockAverages[i].stderr);
	}
}

int findOptimumBlock (BLOCKS *blockAverages, int nLines)
{
	int firstInstance = 0;
	int nInstances = (int)(ceil ((float)nLines * (-0.3)));

	for (int i = 1; i < floor (nLines / 2); ++i)
	{
		if ((blockAverages[i].covariance - blockAverages[i - 1].covariance) > 0)
		{
			if (firstInstance == 0) {
				firstInstance = i; }

			nInstances++;

			if (nInstances > 0)
			{
				return (i + 1);
			}
		}
	}
}

int main(int argc, char const *argv[])
{
	FILE *file_data, *file_output, *file_output2;
	file_data = fopen (argv[1], "r");
	file_output = fopen ("average.block", "w");
	file_output2 = fopen ("average2.block", "w");

	int nLines = countNLines (argv[1], nLines), requiredColumn = atoi (argv[2]);
	float *inputData;
	inputData = (float *) malloc (nLines * sizeof (float));
	inputData = saveInputData (inputData, nLines, file_data);

	BLOCKS *blockAverages, *blockAverages2;
	blockAverages = (BLOCKS *) malloc (nLines * sizeof (BLOCKS));
	blockAverages2 = (BLOCKS *) malloc (nLines * sizeof (BLOCKS));

	blockAverages = initializeBlocks (blockAverages, nLines);
	blockAverages = computeBlockAverages (blockAverages, nLines, inputData);
	blockAverages2 = computeBlockAverages2 (blockAverages2, nLines, inputData);

	printBlockAverageStats (file_output, blockAverages, nLines);
	printBlockAverageStats (file_output2, blockAverages2, nLines);
	int optimumBlockSize = findOptimumBlock (blockAverages, nLines);

	printf("Optimum block size: %d\nAverage: %f\nCovariance: %f\nStandard deviation: %f\nStandard error: %f\n", optimumBlockSize, blockAverages[optimumBlockSize - 1].average, blockAverages[optimumBlockSize - 1].covariance, blockAverages[optimumBlockSize - 1].stdev, blockAverages[optimumBlockSize - 1].stderr);

	fclose (file_data);
	fclose (file_output);
	fclose (file_output2);
	return 0;
}