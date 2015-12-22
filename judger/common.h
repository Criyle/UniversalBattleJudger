#ifndef COMMON_H
#define COMMON_H
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
#include <sys/user.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#include <string>
using namespace std;

#define STD_MB 1048576
#define STD_T_LIM 2
#define STD_F_LIM (STD_MB<<6)
#define STD_M_LIM (STD_MB<<7)
#define BUFFER_SIZE 512
#define AI_MAX 12

namespace ai
{
	//extern int nowPid, iocount;
	//static char lang_ext[13][8] = {"c", "cc", "pas", "java", "rb", "sh", "py", "php","pl", "cs","m","bas","scm"};
	extern int record_syscall;
	struct ai_info
	{
		int time;
		int time_lmt;
		int memory;
		int memory_lmt;
		int step;
		int step_lmt;
		int step_time_lmt;
		int extra_time;
	};
}
#endif

