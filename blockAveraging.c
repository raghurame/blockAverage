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
	double average, covariance, stdev, stderr;
	double squaredAverage, averageSquare;
} BLOCKS;

typedef struct boundary
{
	double low, high;
	int count;
} BOUNDARY;

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

int countNLines2 (const char filename[], int nLines)
{
        FILE *inputFile;
        inputFile = fopen (filename, "r");

        char lineString[3000];
        nLines = 0;

        while ((fgets (lineString, 3000, inputFile) != NULL))
        {
                if (lineString[0] != '#')
                        nLines++;
        }

        fclose (inputFile);
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

double *initializeBlockValues (double *blocks, int nLines)
{
	for (int i = 0; i < nLines; ++i)
	{
		blocks[i] = -9999;
	}

	return blocks;
}

BLOCKS *computeBlockAverages (BLOCKS *blockAverages, int nLines, double *inputData)
{
	printf ("\nComputing averages 1...\n");
	int currentEntry = 0, nEntries = 0;
	int nLines_temp = 0;

	for (int i = 0; i < nLines; ++i)
	{
		if (inputData[i] > 0) {
			nLines_temp++; }
	}

	double *blocks;
	blocks = (double *) malloc (nLines_temp * sizeof (blocks));

	for (int i = 0; i < nLines_temp; ++i)
	{
		if (inputData[i] > -9999) {
			nEntries++; }
	}

	// 'i' is to iterate through various block sizes
	// 'i + 1' is the block size
	for (int i = 0; i < (int)floor (nLines_temp / 2); ++i)
	{
		printf ("computing for block %d/%d...                     \r", i, (int)floor (nLines_temp / 2));
		blocks = initializeBlockValues (blocks, nLines_temp);
		currentEntry = 0;

		// going through the blocks
		// summing the items in the blocks
		// number of blocks = floor (nLines_temp / (i + 1))
		for (int j = 0; j < floor (nLines_temp / (i + 1)); ++j)
		{
			// do averaging within the block
			// (i + 1) is the block size
			blocks[j] = 0;
			for (int k = 0; k < (i + 1); ++k)
			{
				if (inputData[currentEntry] > 0)
				{
					blocks[j] += inputData[currentEntry];
				}

				currentEntry++;
			}
		}

		// going through the blocks
		// finding local averages of these blocks
		// number of blocks = floor (nLines_temp / (i + 1))
		for (int j = 0; j < floor (nLines_temp / (i + 1)); ++j)
		{
			blocks[j] /= (i + 1);
		}

		blockAverages[i].average = 0;
		// find the global average across blocks
		for (int j = 0; j < floor (nLines_temp / (i + 1)); ++j)
		{
			if (blocks[j] == -9999)
			{
				printf("ERROR: Blocks in default value !\n");
				exit (1);
			}

			blockAverages[i].average += blocks[j];
		}

		blockAverages[i].average /= (double)(floor (nLines_temp / (i + 1)));

		// find the global stdev/covar across blocks
		for (int j = 0; j < floor (nLines_temp / (i + 1)); ++j)
		{
			blockAverages[i].covariance += ((blocks[j] - blockAverages[i].average) * (blocks[j] - blockAverages[i].average));
		}

		blockAverages[i].covariance /= (double)(floor (nLines_temp / (i + 1)) - 1);
		blockAverages[i].stdev = sqrt (blockAverages[i].covariance);
		blockAverages[i].stderr = blockAverages[i].stdev / sqrt ((double)((floor (nLines / (i + 1)))));
	}

	return blockAverages;
}

void printBlockDistribution (double *blocks, int nBlocks, int blockSize, int nBins, double *inputData, int nLines)
{
	double min, max, binWidth;
	// int nBins = 10; // default as 20.
	BOUNDARY *bin;
	bin = (BOUNDARY *) malloc (nBins * sizeof (BOUNDARY));

	char *distFileString;
	distFileString = (char *) malloc (500 * sizeof (char));
	snprintf (distFileString, 500, "blockDistributions/size_%d.dist", blockSize);

	FILE *output;
	output = fopen (distFileString, "w");

	// calculating the min and max
	for (int i = 0; i < nLines; ++i)
	{
		if (i == 0) {
			min = inputData[i];
			max = inputData[i]; }
		else {
			if (inputData[i] < min) {
				min = inputData[i]; }
			else if (inputData[i] > max) {
				max = inputData[i]; }
		}
	}

	binWidth = (max - min) / (double)nBins;

	// assigning lows and highs in boundary
	for (int i = 0; i < nBins; ++i)
	{
		if (i == 0) {
			bin[i].low = min;
			bin[i].high = bin[i].low + binWidth; }
		else {
			bin[i].low = bin[i - 1].high;
			bin[i].high = bin[i].low + binWidth; }

		bin[i].count = 0;
	}

	// calculating distribution
	for (int i = 0; i < nBlocks; ++i)
	{
		for (int j = 0; j < nBins; ++j)
		{
			if (blocks[i] < bin[j].high && blocks[i] > bin[j].low)
			{
				bin[j].count++;
			}
		}
	}

	for (int i = 0; i < nBins; ++i)
	{
		fprintf(output, "%lf %lf %d\n", bin[i].low, bin[i].high, bin[i].count);
	}

	free (distFileString);
	fclose (output);
}

BLOCKS *computeBlockAverages2 (BLOCKS *blockAverages, int nLines, double *inputData, int nBins_distribution)
{
	printf ("\nComputing averages 2...\n");
	int currentEntry = 0, nEntries = 0;

	int nLines_temp = 0;

	for (int i = 0; i < nLines; ++i)
	{
		if (inputData[i] > 0) {
			nLines_temp++; }
	}

	double *blocks;
	blocks = (double *) malloc (nLines_temp * sizeof (blocks));
	int denomVariance = 0;

	double *covariance;
	covariance = (double *) malloc (nLines_temp * sizeof (double));

	for (int i = 0; i < nLines_temp; ++i)
	{
		if (inputData[i] > -9999) {
			nEntries++; }
	}

	// 'i' is to iterate through various block sizes
	// 'i + 1' is the block size
	for (int i = 0; i < (int)floor (nLines_temp / 2); ++i)
	{
		printf ("computing for block %d/%d...                     \r", i, (int)floor (nLines_temp / 2));
		// reset the local covariance array
		for (int j = 0; j < nLines_temp; ++j)
		{
			covariance[j] = 0;
		}

		blocks = initializeBlockValues (blocks, nLines_temp);
		currentEntry = 0;

		// going through the blocks
		// summing the items in the blocks
		// number of blocks = floor (nLines_temp / (i + 1))
		for (int j = 0; j < floor (nLines_temp / (i + 1)); ++j)
		{
			// do averaging within the block
			// (i + 1) is the block size
			blocks[j] = 0;
			for (int k = 0; k < (i + 1); ++k)
			{
				if (inputData[currentEntry] > 0)
				{
					blocks[j] += inputData[currentEntry];
				}
				
				currentEntry++;
			}
		}

		// going through the blocks
		// finding local averages of these blocks
		// number of blocks = floor (nLines_temp / (i + 1))
		for (int j = 0; j < floor (nLines_temp / (i + 1)); ++j)
		{
			blocks[j] /= (i + 1);

			// testing
			// printf("%d %f\n", i + 1, blocks[j]);
			// usleep (100000);
		}

		if (nBins_distribution > 0)
		{
			printBlockDistribution (blocks, floor (nLines_temp / (i + 1)), i + 1, nBins_distribution, inputData, nLines_temp);
		}

		// find the local covariance within the blocks
		currentEntry = 0;
		for (int j = 0; j < floor (nLines_temp / (i + 1)); ++j)
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
				}
				currentEntry++;
			}

			covariance[j] /= denomVariance;
		}

		blockAverages[i].covariance = 0;
		// find the average covariance
		for (int j = 0; j < floor (nLines_temp / (i + 1)); ++j)
		{
			if (covariance[j] > 0)
			{
				blockAverages[i].covariance += covariance[j];
			}
		}

		blockAverages[i].covariance /= floor (nLines_temp / (i + 1));

		// find the global average across blocks
		blockAverages[i].average = 0;
		for (int j = 0; j < floor (nLines_temp / (i + 1)); ++j)
		{
			if (blocks[j] == -9999)
			{
				printf("ERROR: Blocks in default value !\n");
				exit (1);
			}

			blockAverages[i].average += blocks[j];
		}

		blockAverages[i].average /= (double)(floor (nLines_temp / (i + 1)));

		// find the global stdev/covar across blocks
/*		for (int j = 0; j < floor (nLines_temp / (i + 1)); ++j)
		{
			blockAverages[i].covariance += ((blocks[j] - blockAverages[i].average) * (blocks[j] - blockAverages[i].average));
		}
*/
/*		blockAverages[i].covariance /= (double)(floor (nLines_temp / (i + 1)) - 1);*/
		blockAverages[i].stdev = sqrt (blockAverages[i].covariance);
		blockAverages[i].stderr = blockAverages[i].stdev / sqrt ((double)((floor (nLines_temp / (i + 1)))));
	}

	return blockAverages;
}

double *saveInputData (double *inputData, int nLines, FILE *file_data, int requiredColumn)
{
	char lineString[2000];
	rewind (file_data);

	for (int i = 0; i < nLines; ++i)
	{
		fgets (lineString, 2000, file_data);

		if (lineString[0] != '#')
		{
			switch (requiredColumn)
			{
				case 1: sscanf (lineString, "%lf\n", &inputData[i]); break;
				case 2: sscanf (lineString, "%*f %lf\n", &inputData[i]); break;
				case 3: sscanf (lineString, "%*f %*f %lf\n", &inputData[i]); break;
				case 4: sscanf (lineString, "%*f %*f %*f %lf\n", &inputData[i]); break;
				case 5: sscanf (lineString, "%*f %*f %*f %*f %lf\n", &inputData[i]); break;
				case 6: sscanf (lineString, "%*f %*f %*f %*f %*f %lf\n", &inputData[i]); break;
				case 7: sscanf (lineString, "%*f %*f %*f %*f %*f %*f %lf\n", &inputData[i]); break;
				case 8: sscanf (lineString, "%*f %*f %*f %*f %*f %*f %*f %lf\n", &inputData[i]); break;
				case 9: sscanf (lineString, "%*f %*f %*f %*f %*f %*f %*f %*f %lf\n", &inputData[i]); break;
				case 10: sscanf (lineString, "%*f %*f %*f %*f %*f %*f %*f %*f %*f %lf\n", &inputData[i]); break;
				case 11: sscanf (lineString, "%*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %lf\n", &inputData[i]); break;
				case 12: sscanf (lineString, "%*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %lf\n", &inputData[i]); break;
				case 13: sscanf (lineString, "%*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %lf\n", &inputData[i]); break;
				case 14: sscanf (lineString, "%*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %lf\n", &inputData[i]); break;
				case 15: sscanf (lineString, "%*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %lf\n", &inputData[i]); break;
				case 16: sscanf (lineString, "%*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %lf\n", &inputData[i]); break;
				case 17: sscanf (lineString, "%*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %lf\n", &inputData[i]); break;
				case 18: sscanf (lineString, "%*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %lf\n", &inputData[i]); break;
				case 19: sscanf (lineString, "%*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %lf\n", &inputData[i]); break;
				case 20: sscanf (lineString, "%*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %*f %lf\n", &inputData[i]); break;
			}
		}
	}

	return inputData;
}

void printBlockAverageStats (FILE *file_output, BLOCKS *blockAverages, int nLines)
{
	for (int i = 0; i < floor (nLines / 2); ++i)
	{
		fprintf(file_output, "%d %lf %lf %lf %lf\n", i + 1, blockAverages[i].average, blockAverages[i].stdev, blockAverages[i].covariance*100000, blockAverages[i].stderr);
	}
}

int findOptimumBlock (BLOCKS *blockAverages, int nLines)
{
	int firstInstance = 0;
	int nInstances = (int)(ceil ((double)nLines * (-0.3)));

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
	if (argc != 5)
	{
		printf("\nINCORRECT ARGUMENTS PASSED:\n~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\nRequired arguments are,\n\n\t{~} argv[0] = program\n\t{~} argv[1] = input file containing data\n\t{~} argv[2] = required column\n\t{~} argv[3] = Number of data points to consider\n\t{~} argv[4] = Number of bins in the distribution files.\n\n");
		exit (1);
	}

	system ("rm -rf blockDistributions");

	FILE *file_data, *file_output, *file_output2;
	file_data = fopen (argv[1], "r");

	int nLines = countNLines (argv[1], nLines), requiredColumn = atoi (argv[2]), nDataPoints = atoi (argv[3]), nBins_distribution = atoi (argv[4]);

	if (nBins_distribution > 0)
	{
		system ("mkdir blockDistributions");
	}

	char *outputFileString;
	outputFileString = (char *) malloc (500 * sizeof (char));

	snprintf (outputFileString, 500, "average_%s_n%d_c%d_1.block", argv[1], nDataPoints, requiredColumn);
	file_output = fopen (outputFileString, "w");
	snprintf (outputFileString, 500, "average_%s_n%d_c%d_2.block", argv[1], nDataPoints, requiredColumn);
	file_output2 = fopen (outputFileString, "w");

	double *inputData;

	if (nDataPoints < nLines)
	{
		nLines = nDataPoints;
	}

	inputData = (double *) malloc (nLines * sizeof (double));
	inputData = saveInputData (inputData, nLines, file_data, requiredColumn);

	BLOCKS *blockAverages, *blockAverages2;
	blockAverages = (BLOCKS *) malloc (nLines * sizeof (BLOCKS));
	blockAverages2 = (BLOCKS *) malloc (nLines * sizeof (BLOCKS));

	blockAverages = initializeBlocks (blockAverages, nLines);
	blockAverages = computeBlockAverages (blockAverages, nLines, inputData);
	blockAverages2 = computeBlockAverages2 (blockAverages2, nLines, inputData, nBins_distribution);

	printBlockAverageStats (file_output, blockAverages, nLines);
	printBlockAverageStats (file_output2, blockAverages2, nLines);
	int optimumBlockSize = findOptimumBlock (blockAverages, nLines);

	printf("Optimum block size: %d\nAverage: %lf\nCovariance: %lf\nStandard deviation: %lf\nStandard error: %lf\n", optimumBlockSize, blockAverages[optimumBlockSize - 1].average, blockAverages[optimumBlockSize - 1].covariance, blockAverages[optimumBlockSize - 1].stdev, blockAverages[optimumBlockSize - 1].stderr);

	fclose (file_data);
	fclose (file_output);
	fclose (file_output2);
	return 0;
}
