#ifndef RUNNER_H
#define RUNNER_H

#include <string>
#include <stdio.h>
#include "exception.h"
#include "common.h"

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

namespace ai
{
	extern int nowPid, iocount;
	class runner
	{
	protected:
		int call_counter[512];
		string path;
		int lang;
		int run_flag;	public:
		pid_t pid;
		ai_info info;
		runner(ai_info lmt =  (ai_info){0, 60 * 1000, 0, 256 * STD_MB, 0, 2000, 100, 0});
		~runner();
		void worker();
		void checker();
		int work();
		void init_syscalls_limits();
		inline char readchar()
		{
			char c;
			nowPid = pid;
			while ((c = fgetc(fout)) == EOF) checker();
			iocount ++;
#ifdef RECORD_IO
			fprintf(logout, "%c", c);
#endif // RECORD_IO
			return c;
		}

		inline int readint()
		{
			int rt = 0, q = 0;
			char ch = readchar();
			while((ch < '0' || ch > '9') && ch != '-') ch = readchar();
			if(ch <= '9' && ch >= '0') q = 1;
			else if(ch == '-') q = -1, ch = readchar();
			else throw manager_invalid(" Not A Number");
			while(ch <= '9' && ch >= '0') rt = rt * 10 + ch - '0', ch = readchar();
			if(ch != ' ' && ch > 26) throw manager_invalid(" Not A Number");
			return rt * q;
		}

		inline void writechar(char c)
		{
			if(fputc(c, fin) == EOF) throw input_error();
#ifdef RECORD_IO
			fprintf(login, "%c", c);
#endif //RECORD_IO
		}

		inline void newround()
		{
			info.step ++;
#ifdef RECORD_IO
			fprintf(login, "\n//start round:%d\n", round);
			fprintf(logout, "\n//start round:%d\n", round);
#endif //RECORD_IO
		}

		int pin[2], pout[2];
		FILE *fin, *fout;
		FILE *login, *logout;
	};

	class aiRunner : public runner
	{
	public:
		aiRunner(string, int, ai_info lmt = (ai_info){0, 60 * 1000, 0, 256 * STD_MB, 0, 2000, 100, 0});
	};

	class managerRunner : public runner
	{
	public:
		managerRunner(string, int, ai_info lmt = (ai_info){0, 60 * 1000, 0, 256 * STD_MB, 0, 10000, 100, 1000});
	};
}
 
#endif
