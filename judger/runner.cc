#include "runner.h"
//#include "okcalls.h"
#include <stdio.h>
#include <fcntl.h>
#include "exception.h"
#include <stdlib.h>
#include <iostream>
#include <sys/wait.h>
#include <sys/resource.h>
#include <dirent.h>
#include <time.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <unistd.h>

#ifdef __i386
#define REG_SYSCALL orig_eax
#define REG_RET eax
#define REG_ARG0 ebx
#define REG_ARG1 ecx
#else
#define REG_SYSCALL orig_rax
#define REG_RET rax
#define REG_ARG0 rdi
#define REG_ARG1 rsi
#endif

//#define MACOS
#ifdef MACOS

#define PTRACE_KILL PT_KILL
#define PTRACE_TRACEME PT_TRACE_ME
#define PTRACE_SYSCALL PT_SYSCALL
#define PTRACE_GETREGS PT_GETREGS

#endif

using namespace std;

namespace ai
{
	const int call_array_size = 512;
	int nowPid, iocount;
	int record_syscall;
	
	int execute_cmd(const char * fmt, ...) {
		char cmd[1024];
		int ret = 0;
		va_list ap;
		va_start(ap, fmt);
		vsprintf(cmd, fmt, ap);
		ret = system(cmd);
		va_end(ap);
		return ret;
	}

	runner::runner(ai_info info)
		: info(info)
	{
	}

	void solve(int p)
	{
		kill(nowPid, SIGALRM);
	}

	int runner::work()
	{
		int flags;
		if (record_syscall == 0)
			init_syscalls_limits();
		//copy_runtime(lang, path.c_str());
		// create pipes for IO
		// in
		if(pipe(pin) == -1)
			{
				fprintf(stderr, "Pipe error 1\n");
				return -1;
			}
		if((fin = fdopen(pin[1], "w")) == NULL)	return -1;
		if((flags = fcntl(pin[1], F_GETFL)) == -1) return -1;
		if(fcntl(pin[1], F_SETFL, flags|O_NONBLOCK) == -1) return -1;
		// out
		if(pipe(pout) == -1)
			{
				fprintf(stderr, "Pipe error 2\n");
				return -1;
			}
		if((fout = fdopen(pout[0], "r")) == NULL) return -1;
		if((flags = fcntl(pout[0], F_GETFL))==-1) return -1;
		if(fcntl(pout[0], F_SETFL, flags|O_NONBLOCK) == -1) return -1;
		// rocord
#ifdef RECORD_IO
		login = fopen((path + "record.in").c_str(), "w");
		logout = fopen((path + "record.out").c_str(), "w");
#endif // RECORD_IO
		// init child process
		signal(SIGALRM,solve);
		pid = fork();
		if(pid == 0)
			{
				worker();
				exit(0);
			}
		run_flag = 1;
		fprintf(stderr, "Runner path=%s pid=%d\n", path.c_str(), pid);
		return 0;
	}

	aiRunner::aiRunner(string a, int b, ai_info lmt)
	{
		path = a;
		lang = b;
		run_flag = 0;
		info.step = 0;
		info = lmt;
	}

	managerRunner::managerRunner(string a, int b, ai_info lmt)
	{
		path = a;
		lang = b;
		run_flag = 0;
		info.step = 0;
		info = lmt;
	}

	runner::~runner()
	{
		if(run_flag)
			{
				fclose(fin);
				fclose(fout);
				ptrace(PTRACE_KILL, pid, NULL, NULL);
			}
	}


	void runner::init_syscalls_limits() {
		int i;
		memset(call_counter, 0, sizeof(call_counter));	
	}

	void runner::worker()
	{
		nice(5);
		// open the files
		chdir(path.c_str());
		dup2(pin[0], 0);
		dup2(pout[1], 1);
		freopen("error.out", "a+", stderr);

		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		if (lang != 3) chroot(path.c_str());

		//  while(setgid(1536)!=0) sleep(1);
		//	while(setuid(1536)!=0) sleep(1);
		//	while(setresuid(1536, 1536, 1536)!=0) sleep(1);
	
		struct rlimit LIM;
		LIM.rlim_max = LIM.rlim_cur = (info.time_lmt + 1000) / 1000;
		setrlimit(RLIMIT_CPU, &LIM);
		alarm(0);
		alarm(info.time_lmt * 5 / 1000); 
		// file limit
		LIM.rlim_cur = LIM.rlim_max = (1 << 20) * 200;
		setrlimit(RLIMIT_FSIZE, &LIM);
		// proc limit
		switch(lang){
		case 3:  //java
		case 12:
			LIM.rlim_cur = LIM.rlim_max = 50;
			break;
		case 5: //bash
			LIM.rlim_cur = LIM.rlim_max = 3;
			break;
		case 9: //C#
			LIM.rlim_cur = LIM.rlim_max = 30;
			break;
		default:
			LIM.rlim_cur = LIM.rlim_max = 1;
		}
		setrlimit(RLIMIT_NPROC, &LIM);
	
		LIM.rlim_max = LIM.rlim_cur = (1 << 20) << 6;
		setrlimit(RLIMIT_STACK, &LIM);
	
		// memory limit
		LIM.rlim_cur = info.memory_lmt * 3 / 2;
		LIM.rlim_max = info.memory_lmt * 2;
		setrlimit(RLIMIT_AS, &LIM);
	
		//run
		switch (lang) {
		case 0:
		case 1:
		case 2:
		case 10:
		case 11:
			execl("./ai", "./ai", (char *)NULL);
			break;
		case 3:
			execl("/usr/bin/java", "/usr/bin/java", "-Xms32m" , "-Xmx256m", "-Djava.security.manager", "-Djava.security.policy=../etc/java0.policy", "ai", (char *)NULL);
			break;
		case 4:
			execl("/ruby","/ruby", "ai.rb", (char *)NULL);
			break;
		case 5: //bash
			execl("/bin/bash","/bin/bash" ,"ai.sh",(char *)NULL);
			break;
		case 6: //Python
			execl("/python","/python","ai.py",(char *)NULL);
			break;
		case 7: //php
			execl("/php","/php","ai.php",(char *)NULL);
			break;
		case 8: //perl
			execl("/perl","/perl","ai.pl",(char *)NULL);
			break;
		case 9: //Mono C#
			execl("/mono","/mono","--debug","ai.exe",(char *)NULL);
			break;
		case 12: //guile
			execl("/guile","/guile","ai.scm",(char *)NULL);
			break;
		}
		exit(0);
	}

	int get_page_fault_mem(struct rusage & ruse, pid_t & pidApp) {
		//java use pagefault
		int m_vmpeak, m_vmdata, m_minflt;
		m_minflt = ruse.ru_minflt * getpagesize();
		return m_minflt;
	}
	/*
	int get_proc_status(int pid, const char * mark) {
		FILE * pf;
		char fn[1024], buf[1024];
		int ret = 0;
		sprintf(fn, "/proc/%d/status", pid);
		pf = fopen(fn, "r");
		int m = strlen(mark);
		while (pf && fgets(buf, 1023, pf)) {
			buf[strlen(buf) - 1] = 0;
			if (strncmp(buf, mark, m) == 0) {
				sscanf(buf + m + 1, "%d", &ret);
			}
		}
		if (pf) fclose(pf);
		return ret;
	}
	*/

	void runner::checker()
	{
		// parent
		int tempmemory, status, sig, exitcode;
		struct user_regs_struct reg;
		struct rusage ruse;
		static int sub = 0;

		nowPid = pid;
		ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
		wait4(pid, &status, 0, &ruse);
		alarm(0);
		alarm(10);

		info.time = (ruse.ru_utime.tv_sec * 1000 + ruse.ru_utime.tv_usec / 1000);
	
		if(info.time > min(info.time_lmt, info.step_time_lmt * info.step + info.extra_time)){
			throw time_limit_exceeded_error();
		}

		if (lang == 3) {
			tempmemory = get_page_fault_mem(ruse, pid);
		} else {//other use VmPeak
			tempmemory = get_page_fault_mem(ruse, pid);//get_proc_status(pid, "VmPeak:") << 10;
		}
		if (tempmemory > info.memory_lmt) {
			throw memory_limit_exceeded_error();
		}
		if (tempmemory > info.memory) info.memory = tempmemory;
		
		if (WIFEXITED(status))
			throw runtime_error_error("EXITED");
			
		exitcode = WEXITSTATUS(status);
		/*exitcode == 5 waiting for next CPU allocation
		 * ruby using system to run,exit 17 ok
		 */
		if ((lang >= 3 && exitcode == 17) || exitcode == 0x05 || exitcode == 0);
		else {
			switch (exitcode) {
			case SIGCHLD:
				throw runtime_error_error("SIGCHLD");
			case SIGKILL:
				throw runtime_error_error("SIGKILL");
			case SIGXCPU:
				throw runtime_error_error("SIGCPU");
			case SIGALRM:
				throw real_time_limit_exceeded_error();
			case SIGXFSZ:
				throw output_limit_exceeded_error();
			default:
				throw runtime_error_error("DEFAULT");
			}
		}
			
		if (WIFSIGNALED(status)) {
			/*  WIFSIGNALED: if the process is terminated by signal
			 *
			 *  psignal(int sig, char *s)，like perror(char *s)，print out s, with error msg from system of sig  
			 * sig = 5 means Trace/breakpoint trap
			 * sig = 11 means Segmentation fault
			 * sig = 25 means File size limit exceeded
			 */
			sig = WTERMSIG(status);
			switch (sig) {
			case SIGCHLD:
				throw runtime_error_error("SIGCHLD-SIGNAL");
			case SIGALRM:
				throw runtime_error_error("SIGALRM-SIGNAL");
			case SIGKILL:
				throw runtime_error_error("SIGKILL-SIGNAL");
			case SIGXCPU:
				throw real_time_limit_exceeded_error("SIGXCPU-SIGNAL");
			case SIGXFSZ:
				throw output_limit_exceeded_error();
			default:
				throw runtime_error_error("DEFAULT-SIGNAL");
			}
		}

		// check the system calls
		ptrace(PTRACE_GETREGS, pid, NULL, &reg);
		if (record_syscall == 1)
			{
				if(sub == 1) call_counter[reg.REG_SYSCALL] ++;
			}
		else
			{
				if (call_counter[reg.REG_SYSCALL] == 0) { //do not limit JVM syscall for using different JVM
					cerr<<reg.REG_SYSCALL<<endl;
					throw runtime_error_error("INVALID SYSCALL");
				}else{
					if (sub == 1 && call_counter[reg.REG_SYSCALL] > 0)
						call_counter[reg.REG_SYSCALL]--;
				}
			}
		sub = 1 - sub;
	}
}
