#include <stdio.h>
#include <stdlib.h>
// This code is licensed under the New BSD license.
// See README.txt for more details.

//#include <threadweaver/Job.h>

int i = 4;
extern int j;

int main()
{
	printf("Hello World\n");
	return 0;
}

int foo()
{
	printf("Hello World %d %d %d\n", 1, 2, 3);
	return 0;
}
