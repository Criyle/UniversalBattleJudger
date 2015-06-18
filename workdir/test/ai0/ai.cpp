#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
	int a, *b;
	scanf("%d", &a);
	//	for(int i = 1; i <= 100000000; i++);//putchar('1');
	printf("%d\n", a * 10);
	fflush(stdout);
	return 0;
}
