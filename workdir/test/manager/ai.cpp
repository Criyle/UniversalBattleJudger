#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../api/manager.h"

using namespace judge;
int t;

int main()
{
	init(2);
	stepbegin(0);
	writestr("%d\n", 10);
	t = readint();
	putresult(0, "%d %d", 0, t);
	putscore(0, 1);
	stepend(0);

	stepbegin(1);
	//	t = readint();
	linescan("%d", &t);
	putresult(1, "%d %d", 1, t);
	putscore(0, 1);
	stepend(1);
	puterr("OK!\n");
	
	judgeend();
}
