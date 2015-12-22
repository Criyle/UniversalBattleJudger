#include "manager.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include "exception.h"
#include "compiler.h"
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

namespace ai{
	void manager::work()
	{
		char buff[10000];
		int ok = 1;
		compiler *cp;
		for(int i = 0; i < ai_num; i++)
			{
				sprintf(buff, "ai%d", i);
				cp = new compiler(work_dir + buff, 1);
				if(cp->work())
					{
						ok = 0;
						result[i] = "Compile Error";
						score[i] = 0;
					}
				else
					{
						result[i] = "Compile OK";
						score[i] = 1;
					}
				delete cp;
			}
		sprintf(buff, "manager");
		cp = new compiler(work_dir + buff, 1);
		if(cp->work())
			{
				ok = 0;
				result[ai_num] = "Compile Error";
			}
		else
			{
				result[ai_num] = "Compile OK";
				score[ai_num] = 1;
			}	
		delete cp;
		if(!ok)return;

		for(int i = 0; i < ai_num; i++)
			{
				sprintf(buff, "ai%d", i);
				ai[i] = new aiRunner(work_dir + buff, 1, ai_lmt);
				if(ai[i]->work() == -1) 
					{
						fprintf(stderr, "runner error\n");
						return;
					}
				result[i] = "Normal Operation";
			}
		sprintf(buff, "manager");
		man = new managerRunner(work_dir + buff, 1, man_lmt);
		if(man->work() == -1)
			{
				fprintf(stderr, "runner error\n");
				return;
			}
		run_flag = 1;

		int ttt = ::time(NULL);
		try
			{
				int cout = 1;
				int count = 0;
				iocount = 0;
				while(cout)
					{
						char ch = man->readchar();
						int k, i;
						count ++;
						switch(ch)
							{
							case 'a':
								now = man->readint();
								if(now < 0 || now >= ai_num)
									throw manager_invalid();
								ai[now]->newround();
								man->newround();
								break;
							case 'b':
								now = man->readint();
								if(now < 0 || now >= ai_num)
									throw manager_invalid();
								fflush(ai[now]->fin);
								break;
							case 'c':
								i = 0;
								buff[i] = ai[now]->readchar();
								while(buff[i] != EOF)
									buff[++ i] = fgetc(ai[now]->fout);
								if(fprintf(man->fin, "%d\n", i) == EOF)
									throw input_error();
								if(fwrite(buff, sizeof(char), i, man->fin) == EOF)
									throw input_error();
								fflush(man->fin); iocount += i;
#ifdef RECORD_IO
								for(int j = 0; j < i; j++)
									man->writechar(buff[j]);
								fprintf(man->login, "%d\n", i);
								for(int j = 1; j < i; j++)
									fprintf(ai[now]->logout, "%c", buff[j]);
#endif //RECORD_IO
								break;
							case 'd':
								i = man->readint();
								if(i < 0) throw manager_invalid();
								for(int j = 0; j < i; j++)
									buff[j] = man->readchar();
								if(fwrite(buff, sizeof(char), i, ai[now]->fin) == EOF)
									throw input_error();
								fflush(ai[now]->fin); iocount += i;
#ifdef RECORD_IO
								fprintf(man->logout, "\nfin%d\n", i);
#endif //RECORD_IO
								break;
							case 'r':
								k = man->readint();
								if(k < 0 || k >= ai_num) throw manager_invalid();
								i = 0;
								memset(buff, 0, sizeof(buff));
								while(ch > 26) buff[i ++] = ch = man->readchar();
								buff[i - 1] = 0;
								result[k] = buff; iocount += i;
								break;
							case 's':
								k = man->readint();
								if(k < 0 || k >= ai_num) throw manager_invalid();
								score[k] = man->readint();
								break;
							case 'q':
								cout = 0;
								break;
							default:
								break;
							}
					}
				printf("QueryCount:%d\nIOCount:%d\n", count, iocount);
			}
		catch(const myException &e)
			{
				int i = 0;
				while(i < ai_num && ai[i]->pid != nowPid) i++;
				result[i] = e.what();
			}
		for(int i = 0; i < ai_num; i++) info[i] = ai[i]->info;
		info[ai_num] = man->info; 
		printf("RealTime:%ds\n", ::time(NULL) - ttt);
	}

	manager::~manager()
	{
		if(run_flag)
			{
				delete man;
				for(int i = 0; i < ai_num; i++) delete ai[i];
				int pid, stat;
				while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
					printf("Child Terminated pid=%d\n", pid);
			}
	}
}
