#include "compiler.h"
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace ai{
	long get_file_size(const char * filename) {
		struct stat f_stat;
		if (stat(filename, &f_stat) == -1) {
			return 0;
		}
		return (long) f_stat.st_size;
	}

	compiler::compiler(string a, int l)
	{
		lang = l;
		path = a;
	}

	int compiler::work()
	{
		int pid;
	
		pid = fork();
		if (pid == 0) {
			chdir(path.c_str());
			struct rlimit LIM;
			LIM.rlim_max = 60;
			LIM.rlim_cur = 60;
			setrlimit(RLIMIT_CPU, &LIM);
			alarm(60);
		
			LIM.rlim_max = 900 << 20;
			LIM.rlim_cur = 900 << 20;
			setrlimit(RLIMIT_FSIZE, &LIM);

			LIM.rlim_max =  (1 << 20) << 11;
			LIM.rlim_cur =  (1 << 20) << 11;
			setrlimit(RLIMIT_AS, &LIM);
		
			if (lang != 2&& lang != 11) {
				freopen("ce.txt", "w", stderr);
			} else {
				freopen("ce.txt", "w", stdout);
			}
			switch (lang) {
			case 0:
				{
					const char *CP_C[] = { "gcc", "ai.c", "-o", "ai","-fno-asm","-Wall", "-lm", "--static", "-std=c99", "-DONLINE_JUDGE", NULL};
					execvp(CP_C[0], (char * const *) CP_C);
				}
				break;
			case 1:
				{
					const char *CP_X[] = { "g++", "ai.cpp", "-o", "ai", "-fno-asm", "-Wall", "-lm", "--static","-std=c++0x", "-DONLINE_JUDGE", NULL };
					execvp(CP_X[0], (char * const *) CP_X);
				}
				break;
			case 2:
				{
					const char *CP_P[] = { "fpc", "ai.pas", "-O2","-Co", "-Ct","-Ci", NULL };
					execvp(CP_P[0], (char * const *) CP_P);
				}
				break;
			case 3:
				{
					char javac_buf[4][16];
					char *CP_J[5];
					for(int i = 0; i < 4; i ++) CP_J[i] = javac_buf[i];
					sprintf(CP_J[0], "javac");
					sprintf(CP_J[1], "-J-Xms32m");
					sprintf(CP_J[2], "-J-Xmx256m");
					sprintf(CP_J[3], "ai.java");
					CP_J[4] = (char*)NULL;	
					execvp(CP_J[0], (char * const *) CP_J);
				}
				break;
			case 4:
				{
					const char *CP_R[] = { "ruby", "-c", "ai.rb", NULL };
					execvp(CP_R[0], (char * const *) CP_R);
				}
				break;
			case 5:
				{
					const char *CP_B[] = { "chmod", "+rx", "ai.sh", NULL };
					execvp(CP_B[0], (char * const *) CP_B);
				}
				break;
			case 6:
				{
					const char *CP_Y[] = {"python", "-c", "import py_compile; py_compile.compile(r'ai.py')", NULL};
					execvp(CP_Y[0], (char * const *) CP_Y);
				}
				break;
			case 7:
				{
					const char * CP_PH[] = { "php", "-l", "ai.php", NULL };
					execvp(CP_PH[0], (char * const *) CP_PH);
				}
				break;
			case 8:
				{
					const char * CP_PL[] = { "perl", "-c", "ai.pl", NULL };
					execvp(CP_PL[0], (char * const *) CP_PL);
				}
				break;
			case 9:
				{
					const char * CP_CS[] = { "gmcs", "-warn:0", "ai.cs", NULL };
					execvp(CP_CS[0], (char * const *) CP_CS);
				}
				break;           
			case 10:
				{
					const char * CP_OC[]={"gcc", "-o" , "ai", "ai.m", "-fconstant-string-class=NSConstantString", "-I","/usr/include/GNUstep/", "-L","/usr/lib/GNUstep/Libraries/", "-lobjc", "-lgnustep-base", NULL};
					execvp(CP_OC[0], (char * const *) CP_OC);
				}
				break;
			case 11:
				{
					const char * CP_BS[]={"fbc", "ai.bas", NULL};		
					execvp(CP_BS[0], (char * const *) CP_BS);
				}
				break;
			default:
				printf("nothing to do!\n");
			}
			exit(0);
		} else {
			int status = 0;
			waitpid(pid, &status, 0);
			if(lang > 3 && lang < 7) status = get_file_size("ce.txt");
			return status;
		}
	}
}
