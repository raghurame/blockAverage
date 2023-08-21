#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char const *argv[])
{
	FILE *file_input;
	file_input = fopen (argv[1], "r");

	fclose (file_input);
	return 0;
}