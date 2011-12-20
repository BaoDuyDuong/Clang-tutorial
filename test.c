//#include <stdio.h>
// This code is licensed under the New BSD license.
// See README.txt for more details.

int i = 4;
extern int j;

int main()
{
	printf("Hello World\n");
	return 0;
}

int foo()
{
	printf("Hello World\n", 1, 2, 3);
	return 0;
}
