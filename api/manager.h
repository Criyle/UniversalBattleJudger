#ifndef MANAGER_H
#define MANAGER_H

#ifndef ONLINE_JUDGE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/wait.h>
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


using namespace std;

namespace judge
{
	int _ainum, _now, _obuff_cap;
	string _result[12];
	int _score[12];
	string _pwd;
	class _cai
	{
	public:
		string path;
		int pin[2], pout[2];
		int pid;
		FILE *fin, *fout;
		void work();
		//void worker();
		//void checker();
		_cai(string path);
	};
	_cai *_ai[12];
	
	void stepend(int);
	void judgeend();
	void putresult(int, char *);
	void putresult(int, string);
	void putresult(int, const char *, ...);
	void putscore(int, int);
	void check_input();
	void check_output();
	char readchar();
	int readint();
	void linescan(const char *, ...);
	void writestr(char *);
	void writestr(string);
	void writestr(const char *, ...);
	void puterr(const char *, ...);

	_cai::_cai(string a)
	{
		path = a;
	}
	
	void _cai::work()
	{
		pipe(pin);
		pipe(pout);
		fin = fdopen(pin[1], "w");
		int flags = fcntl(pin[1], F_GETFL);
		fcntl(pin[1], F_SETFL, flags|O_NONBLOCK);
		fout = fdopen(pout[0], "r");
		flags = fcntl(pout[0], F_GETFL);
		fcntl(pout[0], F_SETFL, flags|O_NONBLOCK);
		pid = fork();
		if(pid == 0)
			{
				
				chdir((_pwd + path).c_str());
				dup2(pin[0], 0);
				dup2(pout[1], 1);
				freopen("error.out", "a+", stderr);
				execl("./ai", "./ai", (char *)NULL);
				exit(0);
				
				//	worker();
			}
	}
	
	/*
	  void _cai::worker()
	{
		nice(5);
		// open the files
		int fd;
		chdir((_pwd + path).c_str());
		dup2(pin[0], 0);
		dup2(pout[1], 1);
		freopen("error.out", "a+", stderr);

		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		//if (lang != 3) chroot(work_dir.c_str());

		//  while(setgid(1536)!=0) sleep(1);
		//	while(setuid(1536)!=0) sleep(1);
		//	while(setresuid(1536, 1536, 1536)!=0) sleep(1);
	
		struct rlimit LIM;
		LIM.rlim_max = LIM.rlim_cur = 200+1;
		setrlimit(RLIMIT_CPU, &LIM);
		alarm(0);
		alarm(10); 
		// file limit
		LIM.rlim_cur = LIM.rlim_max = 1024 * 1024 * 100;
		setrlimit(RLIMIT_FSIZE, &LIM);
		// proc limit
		
		LIM.rlim_max = LIM.rlim_cur = 1024 * 1024 << 6;
		setrlimit(RLIMIT_STACK, &LIM);
	
		// memory limit
		LIM.rlim_cur = 1024 * 1024 * 500 * 3 / 2;
		LIM.rlim_max = 1024 * 1024 * 500 * 3 / 2;
		setrlimit(RLIMIT_AS, &LIM);
	
		//run
		execl("./ai", "./ai", (char *)NULL);
		
		exit(0);
	}

	void _cai::checker()
	{
		// parent
		int tempmemory, status, sig, exitcode;
		struct user_regs_struct reg;
		struct rusage ruse;
		static int sub = 0;

		int nowPid = pid;
		//signal(SIGALRM,solve);
		ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
		wait4(pid, &status, 0, &ruse);
		alarm(0);
		alarm(10);

		int nowtime = (ruse.ru_utime.tv_sec * 1000 + ruse.ru_utime.tv_usec / 1000);
	
		//if(nowtime > 100 * round){
		//	throw time_limit_exceeded_error();
		//}
			
		exitcode = WEXITSTATUS(status);

		if (exitcode == 0x05 || exitcode == 0);
		else {
			switch (exitcode) {
			case SIGCHLD:

			case SIGKILL:

			case SIGXCPU:

			case SIGALRM:

			case SIGXFSZ:

			default:
				;
			}
		}
			
		if (WIFSIGNALED(status)) {
			sig = WTERMSIG(status);
			switch (sig) {
			case SIGCHLD:

			case SIGALRM:

			case SIGKILL:

			case SIGXCPU:

			case SIGXFSZ:

			default:
				;
			}
		}

		// check the system calls
		ptrace(PTRACE_GETREGS, pid, NULL, &reg);

		//cerr<<reg.REG_SYSCALL<<endl;


			//			if (sub == 1 && call_counter[reg.REG_SYSCALL] > 0)
			//				call_counter[reg.REG_SYSCALL]--;

		sub = 1 - sub;
	}
*/
	

	

	
	void stepend(int a)
	{

	}
		
	void judgeend()
	{
		printf("Result:\n");
		for(int i = 0; i < _ainum; i++)
			printf("AI%d:%s\n", i, _result[i].c_str());
		printf("Score:\n");
		for(int i = 0; i < _ainum; i++)
			printf("AI%d:%d\n", i, _score[i]);
		exit(0);
	}

	void putresult(int a, char *b)
	{
		_result[a] = b;
	}

	void putresult(int a, string b)
	{
		putresult(a, b.c_str());
	}
	
	void putresult(int a, const char *fmt, ...)
	{
		char buf[1024];
		va_list ap;
		va_start(ap, fmt);
		vsprintf(buf, fmt, ap);
		putresult(a, buf);
		va_end(ap);
	}
	
	void putscore(int a, int b)
	{
		_score[a] = b;
	}
	
	void init(int a)
	{
		char buf[1024];
		_ainum = a;
		getcwd(buf, 1024);
		_pwd = buf;
		_pwd += '/';
		for(int i = 0; i < _ainum; i++)
			{
				sprintf(buf, "../ai%d", i);
				_ai[i] = new _cai(buf);
				_ai[i] -> work();
				_result[i] = "Normal Operation";
				_score[i] = 1;
			}
	}

	char readchar()
	{
		int c = fgetc(_ai[_now]->fout);
		//while(c == EOF) _ai[_now]->checker(), c = fgetc(_ai[_now]->fout);
		//putchar(c);
		return c;
	}

	int readint()
	{
		int rt = 0, q = 1;
		char ch = readchar();
		while((ch < '0' || ch > '9') && ch != '-') ch = readchar();
		if(ch <= '9' && ch >= '0') q = 1;
		else if(ch == '-') q = -1, ch = readchar();
		else
			{
				putresult(_now, "invalid output");
				judgeend();
			}
		while(ch <='9' && ch >= '0')
			rt = rt * 10 + ch - '0', ch = readchar();
		return rt * q;
	}

	void linescan(const char *fmt, ...)
	{
		char buf[1024];
		int cap = 0;
		va_list ap;
		while((buf[cap] = readchar()) > 26) cap++;
		buf[cap] = '\0';
		va_start(ap, fmt);
		vsscanf(buf, fmt, ap);
		va_end(ap);
	}

	void writestr(char *a)
	{
		int l = strlen(a);
		for(int j = 0; j < l; j++)
			fputc(a[j], _ai[_now]->fin);
		fflush(_ai[_now]->fin);
	}

	void writestr(string a)
	{
		writestr(a.c_str());
	}

	void writestr(const char *fmt, ...)
	{
		char buf[1024];
		va_list ap;
		va_start(ap, fmt);
		vsprintf(buf, fmt, ap);
		writestr(buf);
		va_end(ap);
	}

	void stepbegin(int a)
	{
		_now = a;
	}

	void puterr(const char *fmt, ...)
	{
		char buf[1024];
		va_list ap;
		va_start(ap, fmt);
		vsprintf(buf, fmt, ap);
		fprintf(stderr, buf);
		va_end(ap);
	}
}

namespace graph
{



}

#else //online_judge

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <stdarg.h>
using namespace std;
namespace judge
{
	int _ainum, _now, _ibuff_cap, _ibuff_now, _obuff_cap;
	char _ibuff[10000], _obuff[10000];

	void stepend(int);
	void judgeend();
	void putresult(int, char *);
	void putresult(int, string);
	void putresult(int, const char *, ...);
	void putscore(int, int);
	void check_input();
	void check_output();
	char readchar();
	int readint();
	void linescan(const char *, ...);
	void writestr(char *);
	void writestr(string);
	void writestr(const char *, ...);
	void puterr(const char *, ...);
	
	inline void check_input()
	{
		if(_ibuff_now < _ibuff_cap) return;
		fprintf(stdout, "c");fflush(stdout);
		_ibuff_now = 0;
		int rt = 0;
		char ch = fgetc(stdin);
		while(ch >= '0' && ch <= '9')
			rt = rt * 10 + ch - '0', ch = fgetc(stdin);
		_ibuff_cap = rt;
		//for(int j = 0; j < _ibuff_cap; j++)
		//	_ibuff[j] = fgetc(stdin);
		fread(_ibuff, sizeof(char), _ibuff_cap, stdin);
	}

	inline void check_output()
	{
		if(! _obuff_cap) return;
		int l = _obuff_cap;
		fprintf(stdout, "d %d\n", l);
		//for(int j = 0; j < l; j ++)
		//	fputc(_obuff[j], stdout);
		fwrite(_obuff, sizeof(char), l, stdout);
		_obuff_cap = 0;
		fflush(stdout);
	}
	
	inline void stepend(int a)
	{
		check_output();
		fprintf(stdout, "b %d\n", a);
		fflush(stdout);
	}
		
	inline void judgeend()
	{
		fprintf(stdout, "q");
		fflush(stdout);
		exit(0);
	}

	inline void putresult(int a, char *b)
	{
		fprintf(stdout, "r %d %s\n", a, b);
		fflush(stdout);
	}

	inline void putresult(int a, string b)
	{
		putresult(a, b.c_str());
	}
	
	inline void putresult(int a, const char *fmt, ...)
	{
		char buf[1024];
		va_list ap;
		va_start(ap, fmt);
		vsprintf(buf, fmt, ap);
		putresult(a, buf);
		va_end(ap);
	}
	
	inline void putscore(int a, int b)
	{
		fprintf(stdout, "s %d %d\n", a, b);
		fflush(stdout);
	}
	
	inline void init(int a)
	{
		_ainum = a;
	}

	inline char readchar()
	{
		check_output();
		if((++ _ibuff_now) >= _ibuff_cap) check_input();
		return _ibuff[_ibuff_now];
	}

	inline int readint()
	{
		int rt = 0, q = 1;
		char ch = readchar();
		while((ch < '0' || ch > '9') && ch != '-') ch = readchar();
		if(ch <= '9' && ch >= '0') q = 1;
		else if(ch == '-') q = -1, ch = readchar();
		else
			{
				putresult(_now, "invalid output");
				judgeend();
			}
		while(ch <='9' && ch >= '0')
			rt = rt * 10 + ch - '0', ch = readchar();
		return rt * q;
	}

	inline void linescan(const char *fmt, ...)
	{
		char buf[1024];
		int cap = 0;
		va_list ap;
		while((buf[cap] = readchar()) > 26) cap++;
		buf[cap] = '\0';
		va_start(ap, fmt);
		vsscanf(buf, fmt, ap);
		va_end(ap);
	}

	inline void writestr(char *a)
	{
		int l = strlen(a);
		for(int j = 0; j < l; j++)
			_obuff[_obuff_cap ++] = a[j];
	}

	inline void writestr(string a)
	{
		writestr(a.c_str());
	}

	inline void writestr(const char *fmt, ...)
	{
		char buf[1024];
		va_list ap;
		va_start(ap, fmt);
		vsprintf(buf, fmt, ap);
		writestr(buf);
		va_end(ap);
	}

	inline void stepbegin(int a)
	{
		check_output();
		fprintf(stdout, "a %d\n", _now = a);
		fflush(stdout);
	}

	inline void puterr(const char *fmt, ...)
	{
		char buf[1024];
		va_list ap;
		va_start(ap, fmt);
		vsprintf(buf, fmt, ap);
		fprintf(stderr, buf);
		va_end(ap);
	}
}

namespace graph
{


	
}

#endif //online_judge

#endif //manager_h

