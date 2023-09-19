#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	FILE *inputViscosity;
	inputViscosity = fopen (argv[1], "r");

	

	fclose (inputViscosity);
	return 0;
}