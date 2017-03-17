#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include "exception.h"
#include "common.h"

#define ZOJ_COM
#include "manager.h"
//#include "connect.h"
#include "compiler.h"

string oj_home = "/home/criyle/UniversalBattleJudger/workdir/liquidwar/";

char buff[1000];

int main(int argc, char** argv) {
	int ai_num = 2;
	chdir(oj_home.c_str());
	
	ai::manager *ju = new ai::manager(oj_home, ai_num);
	ju->work();

	for(int i = 0; i < ai_num + 1; i++)
		{
			printf("AI%d:\n Result:%s\n Score:%d\n Time:%d\n Memory:%d\n", i, ju->result[i].c_str(), ju->score[i], ju->info[i].time, ju->info[i].memory);
			putchar('\n');
		}
	delete ju;
}
 
